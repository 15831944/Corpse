#pragma once

#ifdef LS_STATIC_LIB_CALL
#define NET_API 
#elif defined  NETCORE_EXPORTS
#define NET_API __declspec(dllexport)
#else
#define NET_API __declspec(dllimport)
#endif

#include <WinSock2.h>

#define DEF_PACKET_SIZE 32
#define ECHO_REQUEST		8
#define ECHO_REPLY			0

typedef struct tagIPHeader
{
	BYTE m_byVerHLen;					//4λ�汾+4λ�ײ�����
	BYTE m_byTOS;							//��������
	USHORT m_usTotalLen;				//�ܳ���
	USHORT m_usID;						//��ʶ
	USHORT m_usFlagFragOffset;	//3λ��־+13λƬƫ��
	BYTE m_byTTL;							//TTL
	BYTE m_byProtocol;					//Э��
	USHORT m_usHChecksum;			//�ײ������
	ULONG m_ulSrcIP;						//ԴIP��ַ
	ULONG m_ulDestIP;					//Ŀ��IP��ַ
}IPHeader;

typedef struct tagICMPHeader
{ 
	BYTE m_byType;							//����
	BYTE m_byCode;							//����
	USHORT m_usChecksum;			//����� 
	USHORT m_usID;						//��ʶ��
	USHORT m_usSeq;						//���
	ULONG m_ulTimeStamp;			//ʱ������Ǳ�׼ICMPͷ����
}ICMPHeader;

typedef struct tagPING_REPLY
{
	USHORT m_usSeq;
	DWORD m_dwRoundTripTime;
	DWORD m_dwBytes;
	DWORD m_dwTTL;
}PING_REPLY, *PPING_REPLY;

typedef enum tagPING_RESULT
{
	ICMP_REQUEST_SUCCESS	= 0,		// ICMP�ɹ�
	ICMP_REQUEST_FAILED		=	1,		// SOCKET��׼���󣬾��������WSAGetLastError
	ICMP_REQUEST_INITFAILD = 2,		// ICMP��ʼ��ʧ��
	ICMP_REQUEST_NOADDR	=	3,		// Ŀ�ĵ�ַΪ��
	ICMP_REQUEST_TIMEOUT	=	4,		// ICMP����ʱ
	ICMP_REQUEST_xxx			= 5,
}PING_RESULT;

class NET_API CPingHelper
{
public:
	CPingHelper();
	~CPingHelper();
public:
	void SetPacketSize(DWORD dwPacketSize);
	PING_RESULT Ping(DWORD dwDestIP, PING_REPLY *pPingReply = NULL, DWORD dwTimeout = 2000);
	PING_RESULT Ping(LPCTSTR szDestIP, PING_REPLY *pPingReply = NULL, DWORD dwTimeout = 2000);
private:
	PING_RESULT PingCore(DWORD dwDestIP, PING_REPLY *pPingReply, DWORD dwTimeout);
	USHORT CalCheckSum(USHORT *pBuffer, int nSize);
	ULONG GetTickCountCalibrate();
private:
	BOOL			m_bIsInitSucc;				// ��ʼ���Ƿ�ɹ�
	SOCKET		m_sockRaw;					// ����ԭʼ�׽���
	WSAEVENT m_hEvent;					// �¼�
	USHORT		m_usCurrentProcID;		// ����ID
	CHAR*			m_szICMPData;			// ���ݰ�����
	DWORD		m_dwPacketSize;			// Ping����С���ֽڣ�
private:
	static USHORT s_usPacketSeq;
};