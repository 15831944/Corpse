/*-----------------------------------------------------------------------------
 *
 *   Copyright (C) 2002-2015, Asiainfo Inc.  All Rights Reserved.
 *
 *   This program is an unpublished copyrighted work which is proprietary
 *   to Asiainfo, Inc. and contains confidential information that is not
 *   to be reproduced or disclosed to any other person or entity without
 *   prior written consent from Asiainfo, Inc. in each and every instance.
 *
 *   WARNING:  Unauthorized reproduction of this program as well as
 *   unauthorized preparation of derivative works based upon the
 *   program or distribution of copies by sale, rental, lease or
 *   lending are violations of federal copyright laws and state trade
 *   secret laws, punishable by civil and criminal penalties.
 *
 *-----------------------------------------------------------------------------*/
#pragma once

// Allow use of features specific to Windows 2000 or later.

#ifndef WINVER
#   define WINVER 0x0500
#endif

#ifndef _WIN32_WINNT
#   define _WIN32_WINNT 0x0500
#endif

#ifndef _WIN32_WINDOWS
#   define _WIN32_WINDOWS 0x0500
#endif
