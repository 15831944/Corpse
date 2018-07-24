#pragma once

#include "libMain.h"

typedef enum tagADEL_LOCK_CMD
{
	ADEL_LOCK_CMD_DEFAULT			=	100,						
	ADEL_LOCK_CMD_INIT					=	101,
	ADEL_LOCK_CMD_END					=	102,
	ADEL_LOCK_CMD_CHANGE			=	103,
	ADEL_LOCK_CMD_NEW					=	104,
	ADEL_LOCK_CMD_DUP					=	105,
	ADEL_LOCK_CMD_READCARD		=	106,
	ADEL_LOCK_CMD_ERASE				=	107,
	ADEL_LOCK_CMD_READDATA		=	108,
	ADEL_LOCK_CMD_WRITEDATA		=	109,
	ADEL_LOCK_CMD_READID				=	110,
}ADEL_LOCK_CMD;

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
	int Init(int nSoftware, LPCTSTR lpszSQLAddr, LPCTSTR lpszAdmin, int nPort, int nEncoder = 0, int nTMEncoder = 5);
	int EndSession();
	int ChangeUser(LPCTSTR lpszAdmin);
	int NewKey(LPCTSTR lpszRoomNum, LPCTSTR lpszGate, LPCTSTR lpszValidTime, LPCTSTR lpszGuestName,
						LPCTSTR lpszGuestId, BOOL bCovered, int nOpenWay, LONG* pCardNo, int nBreakfast, 
						LPCTSTR lpszTrack1, LPCTSTR lpszTrack2, LPCTSTR lpszElevator, BOOL bFinger);

	int DupKey(LPCTSTR lpszRoomNum, LPCTSTR lpszGate, LPCTSTR lpszValidTime, LPCTSTR lpszGuestName,
						LPCTSTR lpszGuestId, BOOL bCovered, int nOpenWay, LONG* pCardNo, int nBreakfast, 
						LPCTSTR lpszTrack1, LPCTSTR lpszTrack2, LPCTSTR lpszElevator, BOOL bFinger);

	int AddKey(LPCTSTR lpszRoomNum, LPCTSTR lpszGate, LPCTSTR lpszValidTime, LPCTSTR lpszGuestName,
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
	FN_AddKey			m_pfnAddKey;						//	�����ơ����˿�
	FN_ReadCard		m_pfnReadCard;					//	������
	FN_EraseCard		m_pfnEraseCard;					//	ע������
	FN_ReadId			m_pfnReadId;						// ��ȡ��Ƭ��ID��
	FN_ReadCardData	m_pfnReadCardData;		// ��ȡ��Ƭ�����ݡ�
	FN_WriteCardData	m_pfnWriteCardData;		// ��Ƭд�����ݡ�
};