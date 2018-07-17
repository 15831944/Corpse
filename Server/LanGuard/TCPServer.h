#pragma once

#include "libuv/uv.h"
#include <map>
#include <stdio.h>
#include <cstdlib>
#include "ITCPHandler.h"

typedef BOOL (* NewConnect)(size_t ClientId);
//typedef void (* ServerRecvcb)(size_t ClientId,const char* Buf, int nBufSize);

class ClientData;
typedef std::map<size_t, ClientData*> ClientConnMap;

class CTCPServer
{
public:
	CTCPServer(uv_loop_t* loop = uv_default_loop());
	virtual ~CTCPServer();

public:
	/************************************************************************/
	/*                                                                      */
	/************************************************************************/
	BOOL Start(LPCTSTR lpszListenIP, WORD wPort);

	/************************************************************************/
	/*                                                                      */
	/************************************************************************/
	BOOL StartV6(LPCTSTR lpszListenIP, WORD wPort);

	void Stop();
	void Close();
	void CloseServer();

	BOOL SetNoDelay(BOOL bDelay = FALSE);
	BOOL SetKeepAlive(BOOL bAlive, UINT nDelay);

	int Send(size_t ClientId, const char* data, size_t dwLen, BOOL bSend = TRUE);

	void SetConnectExtra(NewConnect cb);
	void SetTCPHandlerCB(size_t ClientId, ITCPHandler* pTCPHandler);

protected:
	size_t GetAvailaClientID() const;				// ��ȡ���õ�client id  
	BOOL DeleteClient(size_t ClientId);	// ɾ�������еĿͻ���  
	
	static void AfterClientClose(uv_handle_t* uvhClient);
	static void AfterServerClose(uv_handle_t* uvhClient);
	static void AfterSend(uv_write_t* uvWriteReq, int nStatus);
	static void AcceptConnection(uv_stream_t* uvServer, int nStatus);
	static void AfterServerRecv(uv_stream_t *uvhClient, ssize_t nRead, const uv_buf_t* buf);
	static void onAllocBuffer(uv_handle_t *uvhClient, size_t suggested_size, uv_buf_t *ReadBuf);
private:
	BOOL Init();
	BOOL Run(int nuvRunType = UV_RUN_DEFAULT);
	BOOL Bind(LPCTSTR lpszListenIP, WORD wPort);
	BOOL BindV6(LPCTSTR lpszListenIP, WORD wPort);
	BOOL Listen(int nListen = 1024);

private:
	BOOL			m_bClose;
	BOOL			m_bInit;
	uv_tcp_t		m_uvServer;		//����������
	uv_loop_t*	m_uvpLoop;	//
	uv_mutex_t  m_uvhMutex;
	NewConnect m_pNewConcb;
	ClientConnMap ClientObjMap;//�ӿͻ������� 
};

class ClientData
{
public:
	ClientData(size_t ClientId);
	virtual ~ClientData();

public:
	size_t			m_ClientId;			//�ͻ���id��Ψһ
	uv_tcp_t*		m_uvhClient;		//�ͻ��˾��
	uv_buf_t		m_uvReadBuf;		//�������ݵ�buf
	uv_buf_t		m_uvWriteBuf;		//д���ݵ�buf
	CTCPServer* m_pTcpServer;	//���������
	//ServerRecvcb m_pRecvcb;		//�������ݻص����û��ĺ���
	ITCPHandler*	m_pHandler;
};