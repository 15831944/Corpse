#pragma once

#include "libMain.h"

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

	int ReadCard(LPTSTR lpsz);
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