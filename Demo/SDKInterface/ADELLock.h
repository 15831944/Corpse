#pragma once

#include "libMain.h"

typedef enum tagCARD_STATE
{
	CARD_STATE_NORMAL		= 1,	// ����ʹ��
	CARD_STATE_LOGOUT		=	3,	// ����ע��
	CARD_STATE_LOST				=	4,	// ��ʧע��
	CARD_STATE_BROKEN		=	5,	// ���ע��
	CARD_STATE_AUTO			=	6,	//	�Զ�ע��
}CARD_STATE;

typedef enum tagOPEN_WAY
{
	OPEN_WAY_CARD							=	1,	//	��ʹ�ÿ�����
	OPEN_WAY_CARD_OR_FIGNER		=	2,	//	ʹ�ÿ�����ָ�ƿ���
	OPEN_WAY_CARD_FIGNER				=	3,	// ��ʹ��ָ�ƿ���
	OPEN_WAY_CARD_AND_FIGNER	= 4,	//	ʹ�ÿ���ָ�ƿ���
}OPEN_WAY;

typedef struct tagCARD_DATA
{
	CString		strRoom;									// ������Ϣ
	CString		strGate;									//	ͨ����Ϣ
	CString		strVaildTime;							//	��Чʱ��
	CString		strGuestName;						//	��������
	CString		strGuestId;								//	����id
	CString		strResvered1;							//	����
	CString		strResvered2;							//	����
	CString		strElevator;								//	��·����
	LONG		lCardNo;									//	��ID
	CARD_STATE CardState;							//	��״̬
	OPEN_WAY OpenWay;							//	���ŷ�ʽ
	int			nBreakfast;								//	�����Ϣ
}CARD_DATA,*PCARD_DATA;

class CADELLock
{
public:
	CADELLock();
	void Release();
public:
	int Init(LPCTSTR lpszSQLAddr, int nPort);
	int EndSession();
	int ChangeUser(LPCTSTR lpszAdmin);
	int NewKey(LPCTSTR lpszRoomNum, LPCTSTR lpszGate, LPCTSTR lpszValidTime, LPCTSTR lpszGuestName,
						LPCTSTR lpszGuestId, BOOL bCovered, int nOpenWay, LONG* pCardNo, int nBreakfast, 
						LPCTSTR lpszTrack1, LPCTSTR lpszTrack2, LPCTSTR lpszElevator, BOOL bFinger);

	int DupKey(LPCTSTR lpszRoomNum, LPCTSTR lpszGate, LPCTSTR lpszValidTime, LPCTSTR lpszGuestName,
						LPCTSTR lpszGuestId, BOOL bCovered, int nOpenWay, LONG* pCardNo, int nBreakfast, 
						LPCTSTR lpszTrack1, LPCTSTR lpszTrack2, LPCTSTR lpszElevator, BOOL bFinger);

	int ReadCard(CARD_DATA* pCardData);
	int EraseCard(LONG lCardNo, LPCTSTR lpszTrack1, LPCTSTR lpszTrack2, int* pFinger);
	int ReadCardId(LONG* lpCardNo);
	int ReadCardData(int nCardType, int nStart, int nLen, CString& strData);
	int WriteCardData(int nCardType, int nStart, int nLen, LPCTSTR lpszData);
protected:
	~CADELLock();
private:
	HMODULE			m_hModule;
	FN_INIT				m_pfnInit;							// ��ʼ����
	FN_EndSession	m_pfnEndSession;				//	���������ڡ�
	FN_ChangeUser	m_pfnChangeUser;				//	���Ĳ���Ա��
	FN_NewKey			m_pfnNewKey;					//	�����¿��˿���
	FN_DupKey			m_pfnDupKey;						//	�����ơ����˿�
	FN_ReadCard		m_pfnReadCard;					//	������
	FN_EraseCard		m_pfnEraseCard;					//	ע������
	FN_ReadId			m_pfnReadId;						// ��ȡ��Ƭ��ID��
	FN_ReadCardData	m_pfnReadCardData;		// ��ȡ��Ƭ�����ݡ�
	FN_WriteCardData	m_pfnWriteCardData;		// ��Ƭд�����ݡ�
};