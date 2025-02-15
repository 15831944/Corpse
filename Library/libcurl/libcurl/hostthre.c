/***************************************************************************
 *                                  _   _ ____  _
 *  Project                     ___| | | |  _ \| |
 *                             / __| | | | |_) | |
 *                            | (__| |_| |  _ <| |___
 *                             \___|\___/|_| \_\_____|
 *
 * Copyright (C) 1998 - 2010, Daniel Stenberg, <daniel@haxx.se>, et al.
 *
 * This software is licensed as described in the file COPYING, which
 * you should have received as part of this distribution. The terms
 * are also available at http://curl.haxx.se/docs/copyright.html.
 *
 * You may opt to use, copy, modify, merge, publish, distribute and/or sell
 * copies of the Software, and permit persons to whom the Software is
 * furnished to do so, under the terms of the COPYING file.
 *
 * This software is distributed on an "AS IS" basis, WITHOUT WARRANTY OF ANY
 * KIND, either express or implied.
 *
 ***************************************************************************/

#include "setup.h"

#include <string.h>
#include <errno.h>

#ifdef HAVE_SYS_SOCKET_H
#include <sys/socket.h>
#endif
#ifdef HAVE_NETINET_IN_H
#include <netinet/in.h>
#endif
#ifdef HAVE_NETDB_H
#include <netdb.h>
#endif
#ifdef HAVE_ARPA_INET_H
#include <arpa/inet.h>
#endif
#ifdef HAVE_STDLIB_H
#include <stdlib.h>     /* required for free() prototypes */
#endif
#ifdef HAVE_UNISTD_H
#include <unistd.h>     /* for the close() proto */
#endif
#ifdef __VMS
#include <in.h>
#include <inet.h>
#include <stdlib.h>
#endif

#if defined(USE_THREADS_POSIX)
#  ifdef HAVE_PTHREAD_H
#    include <pthread.h>
#  endif
#elif defined(USE_THREADS_WIN32)
#  ifdef HAVE_PROCESS_H
#    include <process.h>
#  endif
#endif

#if (defined(NETWARE) && defined(__NOVELL_LIBC__))
#undef in_addr_t
#define in_addr_t unsigned long
#endif

#include "urldata.h"
#include "sendf.h"
#include "hostip.h"
#include "hash.h"
#include "share.h"
#include "strerror.h"
#include "url.h"
#include "multiif.h"
#include "inet_pton.h"
#include "inet_ntop.h"
#include "curl_threads.h"

#define _MPRINTF_REPLACE /* use our functions only */
#include "curl/mprintf.h"

#include "curl_memory.h"
/* The last #include file should be: */
#include "memdebug.h"

/***********************************************************************
 * Only for threaded name resolves builds
 **********************************************************************/
#ifdef CURLRES_THREADED

/* This function is used to init a threaded resolve */
static bool init_resolve_thread(struct connectdata *conn,
                                const char *hostname, int port,
                                const struct addrinfo *hints);


/* Data for synchronization between resolver thread and its parent */
struct thread_sync_data {
  curl_mutex_t * mtx;
  int done;

  char * hostname;        /* hostname to resolve, Curl_async.hostname
                             duplicate */
  int port;
  int sock_error;
  Curl_addrinfo *res;
#ifdef HAVE_GETADDRINFO
  struct addrinfo hints;
#endif
};

struct thread_data {
  curl_thread_t thread_hnd;
  curl_socket_t dummy_sock;
  unsigned int poll_interval;
  int interval_end;
  struct thread_sync_data tsd;
};

static struct thread_sync_data * conn_thread_sync_data(struct connectdata *conn)
{
  return &(((struct thread_data *)conn->async.os_specific)->tsd);
}

#define CONN_THREAD_SYNC_DATA(conn) &(((conn)->async.os_specific)->tsd);

/* Destroy resolver thread synchronization data */
static
void destroy_thread_sync_data(struct thread_sync_data * tsd)
{
  if (tsd->mtx) {
    Curl_mutex_destroy(tsd->mtx);
    free(tsd->mtx);
  }

  if(tsd->hostname)
    free(tsd->hostname);

  if (tsd->res)
    Curl_freeaddrinfo(tsd->res);

  memset(tsd,0,sizeof(*tsd));
}

/* Initialize resolver thread synchronization data */
static
int init_thread_sync_data(struct thread_sync_data * tsd,
                           const char * hostname,
                           int port,
                           const struct addrinfo *hints)
{
  memset(tsd, 0, sizeof(*tsd));

  tsd->port = port;
#ifdef CURLRES_IPV6
  DEBUGASSERT(hints);
  tsd->hints = *hints;
#else
  (void) hints;
#endif

  tsd->mtx = malloc(sizeof(curl_mutex_t));
  if (tsd->mtx == NULL) goto err_exit;

  Curl_mutex_init(tsd->mtx);

  tsd->sock_error = CURL_ASYNC_SUCCESS;

  /* Copying hostname string because original can be destroyed by parent
   * thread during gethostbyname execution.
   */
  tsd->hostname = strdup(hostname);
  if (!tsd->hostname) goto err_exit;

  return 1;

 err_exit:
  /* Memory allocation failed */
  destroy_thread_sync_data(tsd);
  return 0;
}

static int getaddrinfo_complete(struct connectdata *conn)
{
  struct thread_sync_data *tsd = conn_thread_sync_data(conn);
  int rc;

  rc = Curl_addrinfo_callback(conn, tsd->sock_error, tsd->res);
  /* The tsd->res structure has been copied to async.dns and perhaps the DNS cache.
     Set our copy to NULL so destroy_thread_sync_data doesn't free it.
   */
  tsd->res = NULL;

  return rc;
}


#ifdef HAVE_GETADDRINFO

/*
 * getaddrinfo_thread() resolves a name and then exits.
 *
 * For builds without ARES, but with ENABLE_IPV6, create a resolver thread
 * and wait on it.
 */
static unsigned int CURL_STDCALL getaddrinfo_thread (void *arg)
{
  struct thread_sync_data *tsd = (struct thread_sync_data*)arg;
  char   service [NI_MAXSERV];
  int rc;

  snprintf(service, sizeof(service), "%d", tsd->port);

  rc = Curl_getaddrinfo_ex(tsd->hostname, service, &tsd->hints, &tsd->res);

  if (rc != 0) {
    tsd->sock_error = SOCKERRNO;
    if (tsd->sock_error == 0)
      tsd->sock_error = ENOMEM;
  }

  Curl_mutex_acquire(tsd->mtx);
  tsd->done = 1;
  Curl_mutex_release(tsd->mtx);

  return 0;
}

#else /* HAVE_GETADDRINFO */

/*
 * gethostbyname_thread() resolves a name and then exits.
 */
static unsigned int CURL_STDCALL gethostbyname_thread (void *arg)
{
  struct thread_sync_data *tsd = (struct thread_sync_data *)arg;

  tsd->res = Curl_ipv4_resolve_r(tsd->hostname, tsd->port);

  if (!tsd->res) {
    tsd->sock_error = SOCKERRNO;
    if (tsd->sock_error == 0)
      tsd->sock_error = ENOMEM;
  }

  Curl_mutex_acquire(tsd->mtx);
  tsd->done = 1;
  Curl_mutex_release(tsd->mtx);

  return 0;
}

#endif /* HAVE_GETADDRINFO */

/*
 * Curl_destroy_thread_data() cleans up async resolver data and thread handle.
 * Complementary of ares_destroy.
 */
void Curl_destroy_thread_data (struct Curl_async *async)
{
  if(async->hostname)
    free(async->hostname);

  if(async->os_specific) {
    struct thread_data *td = (struct thread_data*) async->os_specific;

    if (td->dummy_sock != CURL_SOCKET_BAD)
      sclose(td->dummy_sock);

    if (td->thread_hnd != curl_thread_t_null)
      Curl_thread_join(&td->thread_hnd);

    destroy_thread_sync_data(&td->tsd);

    free(async->os_specific);
  }
  async->hostname = NULL;
  async->os_specific = NULL;
}

/*
 * init_resolve_thread() starts a new thread that performs the actual
 * resolve. This function returns before the resolve is done.
 *
 * Returns FALSE in case of failure, otherwise TRUE.
 */
static bool init_resolve_thread (struct connectdata *conn,
                                 const char *hostname, int port,
                                 const struct addrinfo *hints)
{
  struct thread_data *td = calloc(1, sizeof(struct thread_data));
  int err = ENOMEM;

  conn->async.os_specific = (void*) td;
  if(!td)
    goto err_exit;

  conn->async.port = port;
  conn->async.done = FALSE;
  conn->async.status = 0;
  conn->async.dns = NULL;
  td->dummy_sock = CURL_SOCKET_BAD;
  td->thread_hnd = curl_thread_t_null;

  if (!init_thread_sync_data(&td->tsd, hostname, port, hints))
    goto err_exit;

  Curl_safefree(conn->async.hostname);
  conn->async.hostname = strdup(hostname);
  if(!conn->async.hostname)
    goto err_exit;

#ifdef WIN32
  /* This socket is only to keep Curl_resolv_fdset() and select() happy;
   * should never become signalled for read since it's unbound but
   * Windows needs at least 1 socket in select().
   */
  td->dummy_sock = socket(AF_INET, SOCK_DGRAM, 0);
  if (td->dummy_sock == CURL_SOCKET_BAD)
    goto err_exit;
#endif

#ifdef HAVE_GETADDRINFO
  td->thread_hnd = Curl_thread_create(getaddrinfo_thread, &td->tsd);
#else
  td->thread_hnd = Curl_thread_create(gethostbyname_thread, &td->tsd);
#endif

  if(!td->thread_hnd) {
#ifndef _WIN32_WCE
    err = errno;
#endif
    goto err_exit;
  }

  return TRUE;

 err_exit:
  Curl_destroy_thread_data(&conn->async);

  SET_ERRNO(err);

  return FALSE;
}


/*
 * Curl_wait_for_resolv() waits for a resolve to finish. This function should
 * be avoided since using this risk getting the multi interface to "hang".
 *
 * If 'entry' is non-NULL, make it point to the resolved dns entry
 *
 * This is the version for resolves-in-a-thread.
 */
CURLcode Curl_wait_for_resolv(struct connectdata *conn,
                              struct Curl_dns_entry **entry)
{
  struct thread_data   *td = (struct thread_data*) conn->async.os_specific;
  struct SessionHandle *data = conn->data;
  CURLcode rc = CURLE_OK;

  DEBUGASSERT(conn && td);

  /* wait for the thread to resolve the name */
  if (Curl_thread_join(&td->thread_hnd)) {
    rc = getaddrinfo_complete(conn);
  } else {
    DEBUGASSERT(0);
  }

  conn->async.done = TRUE;

  if(entry)
    *entry = conn->async.dns;

  if(!conn->async.dns) {
    /* a name was not resolved */
    if (conn->bits.httpproxy) {
      failf(data, "Could not resolve proxy: %s; %s",
            conn->async.hostname, Curl_strerror(conn, conn->async.status));
      rc = CURLE_COULDNT_RESOLVE_PROXY;
    } else {
      failf(data, "Could not resolve host: %s; %s",
            conn->async.hostname, Curl_strerror(conn, conn->async.status));
      rc = CURLE_COULDNT_RESOLVE_HOST;
    }
  }

  Curl_destroy_thread_data(&conn->async);

  if(!conn->async.dns)
    conn->bits.close = TRUE;

  return (rc);
}

/*
 * Curl_is_resolved() is called repeatedly to check if a previous name resolve
 * request has completed. It should also make sure to time-out if the
 * operation seems to take too long.
 */
CURLcode Curl_is_resolved(struct connectdata *conn,
                          struct Curl_dns_entry **entry)
{
  struct SessionHandle *data = conn->data;
  struct thread_data   *td = (struct thread_data*) conn->async.os_specific;
  int done = 0;

  *entry = NULL;

  if (!td) {
    DEBUGASSERT(td);
    return CURLE_COULDNT_RESOLVE_HOST;
  }

  Curl_mutex_acquire(td->tsd.mtx);
  done = td->tsd.done;
  Curl_mutex_release(td->tsd.mtx);

  if (done) {
    getaddrinfo_complete(conn);
    Curl_destroy_thread_data(&conn->async);

    if(!conn->async.dns) {
      failf(data, "Could not resolve host: %s; %s",
            conn->host.name, Curl_strerror(conn, conn->async.status));
      return CURLE_COULDNT_RESOLVE_HOST;
    }
    *entry = conn->async.dns;
  } else {
    /* poll for name lookup done with exponential backoff up to 250ms */
    int elapsed = Curl_tvdiff(Curl_tvnow(), data->progress.t_startsingle);
    if (elapsed < 0)
      elapsed = 0;

    if (td->poll_interval == 0)
      /* Start at 1ms poll interval */
      td->poll_interval = 1;
    else if (elapsed >= td->interval_end)
      /* Back-off exponentially if last interval expired  */
      td->poll_interval *= 2;

    if (td->poll_interval > 250)
      td->poll_interval = 250;

    td->interval_end = elapsed + td->poll_interval;
    Curl_expire(conn->data, td->poll_interval);
  }

  return CURLE_OK;
}

int Curl_resolv_getsock(struct connectdata *conn,
                        curl_socket_t *socks,
                        int numsocks)
{
  const struct thread_data *td =
    (const struct thread_data *) conn->async.os_specific;

  if(td && td->dummy_sock != CURL_SOCKET_BAD) {
    if(numsocks) {
      /* return one socket waiting for readable, even though this is just
         a dummy */
      socks[0] = td->dummy_sock;
      return GETSOCK_READSOCK(0);
    }
  }
  return 0;
}

#ifndef HAVE_GETADDRINFO
/*
 * Curl_getaddrinfo() - for platforms without getaddrinfo
 */
Curl_addrinfo *Curl_getaddrinfo(struct connectdata *conn,
                                const char *hostname,
                                int port,
                                int *waitp)
{
  struct in_addr in;

  *waitp = 0; /* default to synchronous response */

  if(Curl_inet_pton(AF_INET, hostname, &in) > 0)
    /* This is a dotted IP address 123.123.123.123-style */
    return Curl_ip2addr(AF_INET, &in, hostname, port);

  /* fire up a new resolver thread! */
  if(init_resolve_thread(conn, hostname, port, NULL)) {
    *waitp = 1; /* expect asynchronous response */
    return NULL;
  }

  /* fall-back to blocking version */
  return Curl_ipv4_resolve_r(hostname, port);
}

#else /* !HAVE_GETADDRINFO */

/*
 * Curl_getaddrinfo() - for getaddrinfo
 */
Curl_addrinfo *Curl_getaddrinfo(struct connectdata *conn,
                                const char *hostname,
                                int port,
                                int *waitp)
{
  struct addrinfo hints;
  Curl_addrinfo *res;
  int error;
  char sbuf[NI_MAXSERV];
  int pf = PF_INET;
  struct SessionHandle *data = conn->data;

  *waitp = 0; /* default to synchronous response */

#ifndef CURLRES_IPV4
  /*
   * Check if a limited name resolve has been requested.
   */
  switch(data->set.ip_version) {
  case CURL_IPRESOLVE_V4:
    pf = PF_INET;
    break;
  case CURL_IPRESOLVE_V6:
    pf = PF_INET6;
    break;
  default:
    pf = PF_UNSPEC;
    break;
  }

  if (pf != PF_INET) {
    /* see if we have an IPv6 stack */
    curl_socket_t s = socket(PF_INET6, SOCK_DGRAM, 0);
    if(s == CURL_SOCKET_BAD) {
      /* Some non-IPv6 stacks have been found to make very slow name resolves
       * when PF_UNSPEC is used, so thus we switch to a mere PF_INET lookup if
       * the stack seems to be a non-ipv6 one. */

      pf = PF_INET;
    }
    else {
      /* This seems to be an IPv6-capable stack, use PF_UNSPEC for the widest
       * possible checks. And close the socket again.
       */
      sclose(s);
    }
  }
#endif /* !CURLRES_IPV4 */

  memset(&hints, 0, sizeof(hints));
  hints.ai_family = pf;
  hints.ai_socktype = conn->socktype;

  snprintf(sbuf, sizeof(sbuf), "%d", port);

  /* fire up a new resolver thread! */
  if(init_resolve_thread(conn, hostname, port, &hints)) {
    *waitp = 1; /* expect asynchronous response */
    return NULL;
  }

  /* fall-back to blocking version */
  infof(data, "init_resolve_thread() failed for %s; %s\n",
        hostname, Curl_strerror(conn, ERRNO));

  error = Curl_getaddrinfo_ex(hostname, sbuf, &hints, &res);
  if(error) {
    infof(data, "getaddrinfo() failed for %s:%d; %s\n",
          hostname, port, Curl_strerror(conn, SOCKERRNO));
    return NULL;
  }
  return res;
}

#endif /* !HAVE_GETADDRINFO */

#endif /* CURLRES_THREADED */
