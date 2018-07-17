#pragma once

#include "libuv/uv.h"
#include <map>
#include <stdio.h>
#include <cstdlib>
#include "ITCPHandler.h"

typedef void (* FN_Connect)(size_t ClientId);
//typedef void (* ServerRecvcb)(size_t ClientId,const char* Buf, int nBufSize);

class ClientData;
typedef std::map<size_t, ClientData*> ClientConnMap;

class CTCPServer
{
public:
	CTCPServer(uv_loop_t* loop = uv_default_loop());
	virtual ~CTCPServer();

public:
	BOOL Start(LPCTSTR lpszListenIP, WORD wPort);

	BOOL StartV6(LPCTSTR lpszListenIP, WORD wPort);

	/************************************************************************/
	// �������ƣ�Stop
	// �������ܣ��˳���uv_run�����Ĳ���
	// ������������
	// ��  �� ֵ����
	/************************************************************************/
	void Stop();

	/************************************************************************/
	// �������ƣ�Close
	// �������ܣ�����ͻ��˼��ϣ��ͷ��ڴ�
	// ������������
	// ��  �� ֵ����
	/************************************************************************/
	void Close();

	/************************************************************************/
	// �������ƣ�CloseServer
	// �������ܣ������رշ������׽��֣����ٽ����µ�����
	// ������������
	// ��  �� ֵ����
	/************************************************************************/
	void CloseServer();

	BOOL IsServerClosed();

	/************************************************************************/
	// �������ƣ�AutoClose
	// �������ܣ������������ر��׽��ֺ󣬲��ٻظ��ѽ����׽��ֵĿͻ���
	// ������������
	// ��  �� ֵ����
	/************************************************************************/
	void SetAutoClose(BOOL bAutoClose);

	BOOL IsAutoClose();

	BOOL SetNoDelay(BOOL bDelay = FALSE);
	BOOL SetKeepAlive(BOOL bAlive, UINT nDelay);

	/************************************************************************/
	// �������ƣ�Send
	// �������ܣ���ͻ��˷�������
	// ����������ClientId �ͻ������ӱ�ʶ
	//					 data		��Ҫ���͵�����
	//					 dwLen		�������ݳ���
	// ��  �� ֵ�����ͳ���
	/************************************************************************/
	int Send(size_t ClientId, const char* data, size_t dwLen, BOOL bSend = TRUE);

	/************************************************************************/
	// �������ƣ�SetConnectExtra
	// �������ܣ��ͻ�������ʱ�ص�����
	// ����������cb �ص�����ָ��
	// ��  �� ֵ����
	/************************************************************************/
	void SetConnectExtra(FN_Connect cb);

	/************************************************************************/
	// �������ƣ�SetTCPHandlerCB
	// �������ܣ��ͻ��˴������
	// ����������ClientId �ͻ������ӱ�ʶ
	//					pTCPHandler ������̶���ָ��	
	// ��  �� ֵ����
	/************************************************************************/
	void SetTCPHandlerCB(size_t ClientId, ITCPHandler* pTCPHandler);

protected:
	size_t GetAvailaClientID() const;			// ��ȡ���õ�client id  
	BOOL DeleteClient(size_t ClientId);	// ɾ�������еĿͻ���  
	void CloseClient(size_t nClientId);		// ɾ�������еĿͻ��ˣ�����ͻ��˼���Ϊ�գ��رշ�����

	static void AfterClientClose(uv_handle_t* uvhClient);
	static void AfterServerClose(uv_handle_t* uvhClient);
	static void AfterSend(uv_write_t* uvWriteReq, int nStatus);
	static void AcceptConnection(uv_stream_t* uvServer, int nStatus);
	static void AfterServerRecv(uv_stream_t *uvhClient, ssize_t nRead, const uv_buf_t* buf);
	static void OnAllocBuffer(uv_handle_t *uvhClient, size_t suggested_size, uv_buf_t *ReadBuf);
private:
	BOOL Init();
	BOOL Run(int nuvRunType = UV_RUN_DEFAULT);
	BOOL Bind(LPCTSTR lpszListenIP, WORD wPort);
	BOOL BindV6(LPCTSTR lpszListenIP, WORD wPort);
	BOOL Listen(int nListen = 1024);

private:
	BOOL				m_bServerClosed;		// �����������ر�
	BOOL				m_bAutoClosed;			//	�������رպ�رտͻ��ˣ��Զ��˳�
	BOOL				m_bInit;						// �Ƿ񾭹���ʼ��
	uv_tcp_t			m_uvServer;					//	����������
	uv_loop_t*		m_uvpLoop;				//	
	uv_mutex_t		m_uvhMutex;				//	�ͻ��˼��ϻ�����
	FN_Connect		m_pfnConcb;					// �½�����
	ClientConnMap ClientObjMap;			// �ӿͻ��˼���
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