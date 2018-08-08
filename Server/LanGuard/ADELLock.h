#pragma once

#include "libMain.h"

typedef enum tagADEL_LOCK_CMD
{
	ADEL_LOCK_CMD_DEFAULT			=	101,						
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

typedef enum tagCARD_TYPE
{
	CARD_TYPE_NONE				= 0,	//δ����
	CARD_TYPE_PRESIDENT		= 1,	// �ܲ�
	CARD_TYPE_VILLA				=	2,	// ����
	CARD_TYPE_MANAGE			= 3,	// ����
	CARD_TYPE_CONTROL		= 4,	//�ܿؿ�
	CARD_TYPE_ROOM_TIME	= 5,	// ʱ�ӿ�
	CARD_TYPE_LEADER			=	6, // �ڰ࿨
	CARD_TYPE_FLOOR				=	7,	// ¥�㿨
	CARD_TYPE_CLEAR				=	8, // ��࿨
	CARD_TYPE_CUSTOM			=	9, // ���˿�
	CARD_TYPE_FINISH				=10, // ��ֹ��
	CARD_TYPE_MEETING			=11, // ���鿨
	CARD_TYPE_EMERGENCY	=12, // Ӧ����
	CARD_TYPE_CHECK_OUT	=13, // �˷���
	CARD_TYPE_RESVERED		=14, // ����
}CARD_TYPE;

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
	//OPEN_WAY OpenWay;							//	���ŷ�ʽ
	int			nBreakfast;								//	�����Ϣ
}CARD_DATA,*PCARD_DATA;

typedef struct tagBUFF_INFO
{
	CString			strCardData;		// ��������
	int				CardType;		// ������
	int				CardState;		// ��״̬
	int				lCardNo;			// �����
	CString			strRoomNum;	// ���϶�Ӧ�ķ���
	CString			strGName;		//	��������
	CString			strValidTime;	// ����Ӧ����Ч��
}BUFF_INFO, *PBUFF_INFO;

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
						LPCTSTR lpszGuestId, LPCTSTR lpszLift, BOOL bCovered, int nBreakfast, LONG* pCardNo,
						LPCTSTR lpszTrack1, LPCTSTR lpszTrack2);

	int DupKey(LPCTSTR lpszRoomNum, LPCTSTR lpszGate, LPCTSTR lpszValidTime, LPCTSTR lpszGuestName,
						LPCTSTR lpszGuestId, LPCTSTR lpszLift, BOOL bCovered, int nBreakfast, LONG* pCardNo,
						LPCTSTR lpszTrack1, LPCTSTR lpszTrack2);

	int CheckOut(LPCTSTR lpszRoom, LONG lCardno);
	int LostCard(LPCTSTR lpszRoom, LONG lCardno);
	int PopCard(void);
	int SetPort(int nSoftware, int nPort, int nEncoder, int nTMEncoder);
	int ReadCard(CARD_DATA* pCardData);
	int EraseCard(LONG lCardNo, LPCTSTR lpszTrack1, LPCTSTR lpszTrack2);
	int ReadCardId(LONG* lpCardNo);
	int CheckSC(LPCTSTR lpszSC);

	int ReadIC(UINT nStart, UINT nLen, LPCTSTR lpszData);
	int WriteIC(UINT nStart, UINT nLen, LPCTSTR lpszData);

	int WriteMagCard(LPCTSTR lpszTrack1, LPCTSTR lpszTrack2, LPCTSTR lpszTrack3);
	int ReadMagCard(LPTSTR lpszTrack1, LPTSTR lpszTrack2, LPTSTR lpszTrack3);

	int Reader_Beep(int Sound);

	int GetBuffInfo(BUFF_INFO* pBuffInfo);
	int GetMagBuffInfo(BUFF_INFO* pBuffInfo);

	int GetCardInfo(BUFF_INFO* pBuffInfo);
	//int ReadCardData(int nCardType, int nStart, int nLen, CString& strData);
	//int WriteCardData(int nCardType, int nStart, int nLen, LPCTSTR lpszData);
protected:
	~CADELLock();
private:
	HMODULE			m_hModule;
	FN_INIT				m_pfnInit;							// ��ʼ����
	FN_EndSession	m_pfnEndSession;				//	���������ڡ�
	FN_ChangeUser	m_pfnChangeUser;				//	���Ĳ���Ա��
	FN_NewKey			m_pfnNewKey;					//	�����¿��˿���
	FN_DupKey			m_pfnDupKey;						//	�����ơ����˿�
	//FN_AddKey			m_pfnAddKey;						//	�����ơ����˿�
	FN_ReadCard		m_pfnReadCard;					//	������
	FN_EraseCard		m_pfnEraseCard;					//	ע������
	FN_ReadId			m_pfnReadId;						// ��ȡ��Ƭ��ID��
	//FN_ReadCardData	m_pfnReadCardData;		// ��ȡ��Ƭ�����ݡ�
	//FN_WriteCardData	m_pfnWriteCardData;		// ��Ƭд�����ݡ�
	FN_CheckOut		m_pfnCheckOut;					//�˷������������ݿ⣬��ע����
	FN_LostCard		m_pfnLostCard;					// ��ʧ��
	FN_PopCard		m_pfnPopCard;					// ������
	FN_SetPort			m_pfnSetPort;						// ���ö˿�
	FN_CheckSC		m_pfnCheckSC;					// У�鿨����

	FN_ReadIC			m_pfnReadIC;						//	��ȡ������
	FN_WriteIC			m_pfnWriteIC;						//	д������

	FN_WriteMagCard	m_pfnWriteMagCard;		//д�ſ���
	FN_ReadMagCard	m_pfnReadMagCard;		// ���ſ�

	FN_ReaderBeep		m_pfnReaderBeep;				// ������

	FN_GetBuffInfo		m_pfnGetBuffInfo;				
	FN_GetMagBuffInfo m_pfnGetMagBuffInfo;	

	FN_GetCardInfo		m_pfnGetCardInfo;
};