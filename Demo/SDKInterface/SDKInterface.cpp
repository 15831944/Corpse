#include "StdAfx.h"
#include "Resource.h"
#include "SDKInterface.h"
#include "ADELErrorInfo.h"

CSDKInterface::CSDKInterface()
{
	m_pADELLock = new CADELLock;
}

CSDKInterface::~CSDKInterface()
{
	m_pADELLock->Release();
}

LPCTSTR CSDKInterface::GetWindowClassName() const
{
	return _T("SDKInterface");
}

UILIB_RESOURCETYPE CSDKInterface::GetResourceType() const
{
#ifdef _DEBUG
	return UILIB_FILE;
#else
	return UILIB_ZIP;
#endif
}

CDuiString CSDKInterface::GetZIPFileName() const
{
	return _T("SDKInterface.zip");
}

CDuiString CSDKInterface::GetSkinFile()
{
	return _T("SDKInterface.xml");
}

CDuiString CSDKInterface::GetSkinFolder()
{
#ifdef _DEBUG
	return _T("SDKInterface");
#else
	return _T("");
#endif	
}

void CSDKInterface::OnFinalMessage( HWND hWnd )
{
	WindowImplBase::OnFinalMessage(hWnd);
	delete this;
}

void CSDKInterface::InitWindow()
{
	SetIcon(IDI_MAINFRAME);
}

void CSDKInterface::Notify(TNotifyUI& msg)
{
	if (msg.sType == DUI_MSGTYPE_CLICK)
		OnClick(msg);
	else if (msg.sType == DUI_MSGTYPE_SELECTCHANGED)
		OnSelectedChanged(msg);
}

void CSDKInterface::OnClick(TNotifyUI& msg)
{
	CDuiString strNotifyName = msg.pSender->GetName();
	if (strNotifyName == _T("BtnClose"))
		Close(IDOK);
	else if (strNotifyName == _T("BtnMin"))
		SendMessage(WM_SYSCOMMAND, SC_MINIMIZE, 0);
	else if (strNotifyName == _T("BtnInit"))
		OnInit(msg);
	else if (strNotifyName == _T("BtnExit"))
		Close(IDOK);
	else if (strNotifyName == _T("BtnNewKey"))
		OnNewKey(msg, 1);
	else if (strNotifyName == _T("BtnDupKey"))
		OnNewKey(msg, 2);
	else if (strNotifyName == _T("BtnAddKey"))
		OnNewKey(msg, 3);
	else if (strNotifyName == _T("BtnReadCard"))
		OnReadCard(msg);
	else if (strNotifyName == _T("BtnEraseCard"))
		OnEraseCard(msg);
	else if (strNotifyName == _T("BtnChangeUser"))
		OnChangeUser(msg);
	else if (strNotifyName == _T("BtnReadCardId"))
		OnReadCardId(msg);
	else if (strNotifyName == _T("BtnReadData"))
		OnReadData(msg);
	else if (strNotifyName == _T("BtnWriteData"))
		OnWriteData(msg);
}

void CSDKInterface::OnSelectedChanged(TNotifyUI& msg)
{
	CDuiString strNotifyName = msg.pSender->GetName();
	if (strNotifyName == _T("BtnMulti"))
	{
		COptionUI* pMulti = (COptionUI*)msg.pSender->GetInterface(DUI_CTR_OPTION);
		CEditUI2* pMultiElevator = (CEditUI2*)m_PaintManager.FindControl(_T("Elevator"));
		if (pMulti != NULL && pMultiElevator != NULL)
			pMultiElevator->SetEnabled(pMulti->IsSelected());
	}
}

void CSDKInterface::OnInit(TNotifyUI& msg)
{
	CDuiString strSQLAddr;
	int nPort = 0;
	CEditUI2* pSQLAddr = (CEditUI2*)m_PaintManager.FindControl(_T("SQLAddr"));
	if (pSQLAddr != NULL)
		strSQLAddr = pSQLAddr->GetText();
	CComboUI* pComType = (CComboUI*)m_PaintManager.FindControl(_T("ComType"));
	if (pComType != NULL)
		nPort = pComType->GetCurSel();

	int nRet = m_pADELLock->Init(strSQLAddr, _T("test"), nPort);
	if (nRet != 0)
	{
		CDuiString strTipInfo;
		strTipInfo.Format(_T("Init����ʧ�ܣ�����ֵ[%d]:%s"), nRet, GetADELErrorInfo(nRet));
		MessageBox(m_hWnd, strTipInfo, _T("��ʾ"), MB_OK|MB_ICONINFORMATION);
		return;
	}
	else
	{
		CDuiString strTipInfo;
		strTipInfo.Format(_T("Init���óɹ�������ֵ[%d]"), nRet);
		MessageBox(m_hWnd, strTipInfo, _T("��ʾ"), MB_OK|MB_ICONINFORMATION);
		return;
	}
}

void CSDKInterface::OnNewKey(TNotifyUI& msg, int nNewKey/* = 1*/)
{
	CDuiString strRoomNum, strValidTime, strGuestName, strGuestId, strElevator, strGate;
	LONG lCardNo = 0, nBreakfast = 0, nOpenWay = 0;
	BOOL bCover = FALSE;
	BOOL bGetFinger = FALSE;

	// ����
	CEditUI2* pRoomNum = (CEditUI2*)m_PaintManager.FindControl(_T("RoomNum"));
	if (pRoomNum != NULL)
		strRoomNum = pRoomNum->GetText();

	// ��Ч��
	CEditUI2* pValidTime = (CEditUI2*)m_PaintManager.FindControl(_T("ValidTime"));
	if (pValidTime != NULL)
		strValidTime = pValidTime->GetText();

	// �Ƿ񸲸�
	COptionUI* pCoverCard = (COptionUI*)m_PaintManager.FindControl(_T("BtnCoverCard"));
	if (pCoverCard != NULL)
		bCover	= pCoverCard->IsSelected();

	// ����
	CEditUI2* pCardNum = (CEditUI2*)m_PaintManager.FindControl(_T("CardNum"));
	if (pCardNum != NULL)
		lCardNo = _ttoi(pCardNum->GetText());
		
	// ����������ID
	CEditUI2* pGuestName = (CEditUI2*)m_PaintManager.FindControl(_T("UserName"));
	if (pGuestName != NULL)
		strGuestName = pGuestName->GetText();
	CEditUI2* pGuestId = (CEditUI2*)m_PaintManager.FindControl(_T("UserID"));
	if (pGuestId != NULL)
		strGuestId = pGuestId->GetText();

	// ͨ��
	CEditUI2* pGate = (CEditUI2*)m_PaintManager.FindControl(_T("Gate"));
	if (pGate != NULL)
		strGate = pGate->GetText();

	// �������
	CComboUI* pBreakfast = (CComboUI*)m_PaintManager.FindControl(_T("Breakfast"));
	if (pBreakfast != NULL)
		nBreakfast = pBreakfast->GetCurSel();

	// ���ŷ�ʽ
	CComboUI* pOpenWay = (CComboUI*)m_PaintManager.FindControl(_T("OpenWay"));
	if (pOpenWay != NULL)
		nOpenWay = pOpenWay->GetCurSel();

	// �Ƿ��Ƕ�·����
	COptionUI* pMuitl = (COptionUI*)m_PaintManager.FindControl(_T("BtnMulti"));
	if (pMuitl != NULL && pMuitl->IsSelected())
	{
		CEditUI2* pMuitlElevator = (CEditUI2*)m_PaintManager.FindControl(_T("Elevator"));
		if (pMuitlElevator != NULL)
			strElevator = pMuitlElevator->GetText();
	}

	// �Ƿ�ɼ�ָ��
	COptionUI* pGetFinger = (COptionUI*)m_PaintManager.FindControl(_T("BtnCollecte"));
	if (pGetFinger != NULL)
		bGetFinger = pGetFinger->IsSelected();

	int nRet = 0;
	if (nNewKey == 1)
		nRet = m_pADELLock->NewKey(strRoomNum, strGate, strValidTime, strGuestName, strGuestId, 
																bCover, nOpenWay, &lCardNo, nBreakfast, NULL, NULL, strElevator, bGetFinger);
	else if (nNewKey == 2)
		nRet = m_pADELLock->DupKey(strRoomNum, strGate, strValidTime, strGuestName, strGuestId, 
															bCover, nOpenWay, &lCardNo, nBreakfast, NULL, NULL, strElevator, bGetFinger);
	else
		nRet = m_pADELLock->AddKey(strRoomNum, strGate, strValidTime, strGuestName, strGuestId, 
															bCover, nOpenWay, &lCardNo, nBreakfast, NULL, NULL, strElevator, bGetFinger);
	if (nRet != 0)
	{
		CDuiString strTipInfo;
		strTipInfo.Format(_T("NewKey/DupKey����ʧ�ܣ�����ֵ[%d]:%s"), nRet, GetADELErrorInfo(nRet));
		MessageBox(m_hWnd, strTipInfo, _T("��ʾ"), MB_OK|MB_ICONINFORMATION);
		return;
	}
}

void CSDKInterface::OnReadCard(TNotifyUI& msg)
{
	CARD_DATA CardData;

	int nRet = m_pADELLock->ReadCard(&CardData);
	if (nRet != 0)
	{
		CDuiString strTipInfo;
		strTipInfo.Format(_T("ReadCard����ʧ�ܣ�����ֵ[%d]:%s"), nRet, GetADELErrorInfo(nRet));
		MessageBox(m_hWnd, strTipInfo, _T("��ʾ"), MB_OK|MB_ICONINFORMATION);
		return;
	}

	CEditUI2* pRoom = (CEditUI2*)m_PaintManager.FindControl(_T("GetRoomNum"));
	if (pRoom != NULL)
		pRoom->SetText(CardData.strRoom);

	CEditUI2* pVaildTime = (CEditUI2*)m_PaintManager.FindControl(_T("GetValidTime"));
	if (pVaildTime != NULL)
		pVaildTime->SetText(CardData.strVaildTime);

	CEditUI2* pCardNum = (CEditUI2*)m_PaintManager.FindControl(_T("GetCardNum"));
	if (pCardNum != NULL)
	{
		CDuiString strCardNo;
		strCardNo.Format(_T("%d"), CardData.lCardNo);
		pCardNum->SetText(strCardNo);
	}

	CEditUI2* pGuestName = (CEditUI2*)m_PaintManager.FindControl(_T("GetUserName"));
	if (pGuestName != NULL)
		pGuestName->SetText(CardData.strGuestName);

	CEditUI2* pGuestId = (CEditUI2*)m_PaintManager.FindControl(_T("GetUserID"));
	if (pGuestId != NULL)
		pGuestId->SetText(CardData.strGuestId);

	CEditUI2* pGate = (CEditUI2*)m_PaintManager.FindControl(_T("GetGate"));
	if (pGate != NULL)
		pGate->SetText(CardData.strGate);

	CEditUI2* pBreakfast = (CEditUI2*)m_PaintManager.FindControl(_T("GetBreakfast"));
	if (pBreakfast != NULL)
	{
		CDuiString strBreakfast;
		strBreakfast.Format(_T("%d"), CardData.nBreakfast);
		pBreakfast->SetText(strBreakfast);
	}

	CEditUI2* pOpenWay = (CEditUI2*)m_PaintManager.FindControl(_T("GetOpenWay"));
	if (pOpenWay != NULL)
	{
		CDuiString strOpenWay;
		strOpenWay.Format(_T("%d"), CardData.OpenWay);
		pOpenWay->SetText(strOpenWay);
	}

	CEditUI2* pMutilElevator = (CEditUI2*)m_PaintManager.FindControl(_T("GetMuitl"));
	if (pMutilElevator != NULL)
		pMutilElevator->SetText(CardData.strElevator);

	// ��дע���õķ��źͿ���
	CEditUI2* pSetRoomNum = (CEditUI2*)m_PaintManager.FindControl(_T("SetRoomNum"));
	if (pSetRoomNum != NULL)
		pSetRoomNum->SetText(CardData.strRoom);

	CEditUI2* pSetCardId = (CEditUI2*)m_PaintManager.FindControl(_T("SetCardNum"));
	if (pSetCardId != NULL)
	{
		CDuiString strCardId;
		strCardId.Format(_T("%d"), CardData.lCardNo);
		pSetCardId->SetText(strCardId);
	}
}

void CSDKInterface::OnEraseCard(TNotifyUI& msg)
{
	CDuiString strRoomNum;
	LONG lCardNo = 0;
	CEditUI2* pRoomNum = (CEditUI2*)m_PaintManager.FindControl(_T("SetRoomNum"));
	if (pRoomNum != NULL)
		strRoomNum = pRoomNum->GetText();

	CEditUI2* pCardNo = (CEditUI2*)m_PaintManager.FindControl(_T("SetCardNum"));
	if (pCardNo != NULL)
		lCardNo = _ttoi(pCardNo->GetText());

	if (lCardNo == 0)
	{
		MessageBox(m_hWnd, _T("�����뿨��^_^"), _T("��ʾ"), MB_OK|MB_ICONINFORMATION);
		pCardNo->SetFocus();
		return;
	}

	int nfpFinger = 0;
	int nRet = m_pADELLock->EraseCard(lCardNo, NULL, NULL, &nfpFinger);
	if (nRet != 0)
	{
		CDuiString strTipInfo;
		strTipInfo.Format(_T("EraseCard����ʧ�ܣ�����ֵ[%d]:%s"), nRet, GetADELErrorInfo(nRet));
		MessageBox(m_hWnd, strTipInfo, _T("��ʾ"), MB_OK|MB_ICONINFORMATION);
		//return;
	}

	pCardNo->SetText(_T(""));
	
	if (nfpFinger >0)
		MessageBox(m_hWnd, _T("�ֿ��˵�ָ�����˷��󣬻����Կ��š���Ҫʹ��ע����ע���÷����ָ�ơ�"), _T("��ʾ"), MB_OK|MB_ICONINFORMATION);
}

void CSDKInterface::OnChangeUser(TNotifyUI& msg)
{
	CDuiString strAdmin;
	CEditUI2* pAdmin = (CEditUI2*)m_PaintManager.FindControl(_T("SetAdmin"));
	if (pAdmin != NULL)
		strAdmin = pAdmin->GetText();

	if (strAdmin.IsEmpty())
	{
		MessageBox(m_hWnd, _T("���������Ա���ơ�"), _T("��ʾ"), MB_OK|MB_ICONINFORMATION);
		pAdmin->SetFocus();
		return;
	}

	m_pADELLock->ChangeUser(strAdmin);
}

void CSDKInterface::OnReadCardId(TNotifyUI& msg)
{
	LONG lCardId = 0;
	int nRet = m_pADELLock->ReadCardId(&lCardId);
	if (nRet != 0)
	{
		CDuiString strTipInfo;
		strTipInfo.Format(_T("ReadId����ʧ�ܣ�����ֵ[%d]:%s"), nRet, GetADELErrorInfo(nRet));
		MessageBox(m_hWnd, strTipInfo, _T("��ʾ"), MB_OK|MB_ICONINFORMATION);
		return;
	}
	CEditUI2* pCardId = (CEditUI2*)m_PaintManager.FindControl(_T("GetCardInfo"));
	if (pCardId != NULL)
	{
		CDuiString strCardId;
		strCardId.Format(_T("0x%X"), lCardId);
		pCardId->SetText(strCardId);
	}
}

void CSDKInterface::OnReadData(TNotifyUI& msg)
{
	int nLen = 0, nBlock = 0, nSection = 0;

	CComboUI* pSection = (CComboUI*)m_PaintManager.FindControl(_T("ComboSection"));
	if (pSection != NULL)
		nSection = pSection->GetCurSel();

	CComboUI* pBlock = (CComboUI*)m_PaintManager.FindControl(_T("ComboBlock"));
	if (pBlock != NULL)
		nBlock = pBlock->GetCurSel();

	CComboUI* pLen = (CComboUI*)m_PaintManager.FindControl(_T("ComboLen"));
	if (pLen != NULL)
		nLen = _ttoi(pLen->GetText());

	int nStart = nBlock | (nSection < 8);
	CString strData;
	int nRet = m_pADELLock->ReadCardData(18, nStart, nLen, strData);

	if (nRet > 0)
	{
		CDuiString strTipInfo;
		strTipInfo.Format(_T("����ReadCardDataʧ�ܣ�����ֵ[%d]:%s"), nRet, GetADELErrorInfo(nRet));
		MessageBox(m_hWnd, strTipInfo, _T("��ʾ"), MB_OK|MB_ICONINFORMATION);
		return;
	}

	CEditUI2* pReadData = (CEditUI2*)m_PaintManager.FindControl(_T("SetData"));
	if (pReadData != NULL)
		pReadData->SetText(strData);
}

void CSDKInterface::OnWriteData(TNotifyUI& msg)
{
	int nAuth = 0, nLen = 0, nBlock = 0, nSection = 0;
	CDuiString strPassword, strWriteData;

	// ��ȡ����
	CEditUI2* pCardPassword = (CEditUI2*)m_PaintManager.FindControl(_T("Password"));
	if (pCardPassword != NULL)
		strPassword = pCardPassword->GetText();

	// ��ȡ��Ȩ����
	CComboUI* pAuthType = (CComboUI*)m_PaintManager.FindControl(_T("AuthType"));
	if (pAuthType != NULL)
		nAuth = pAuthType->GetCurSel();

	// ��ȡ����
	CComboUI* pSection = (CComboUI*)m_PaintManager.FindControl(_T("ComboSection"));
	if (pSection != NULL)
		nSection = pSection->GetCurSel();

	// ��ȡ��
	CComboUI* pBlock = (CComboUI*)m_PaintManager.FindControl(_T("ComboBlock"));
	if (pBlock != NULL)
		nBlock = pBlock->GetCurSel();

	// ��ȡ����
	CComboUI* pLen = (CComboUI*)m_PaintManager.FindControl(_T("ComboLen"));
	if (pLen != NULL)
		nLen = _ttoi(pLen->GetText());

	int nStart = nBlock | (nSection < 8);
	// ��ȡд������
	CEditUI2* pWriteData = (CEditUI2*)m_PaintManager.FindControl(_T("SetData"));
	if (pWriteData != NULL)
		strWriteData = pWriteData->GetText();

	CDuiString strWriteCardData;
	strWriteCardData.Format(_T("%d%s%s"), nAuth, (LPCTSTR)strPassword.Left(7), (LPCTSTR)strWriteData);

	int nRet = m_pADELLock->WriteCardData(18, nStart, nLen, strWriteCardData);
	if (nRet != 0)
	{
		CDuiString strTipInfo;
		strTipInfo.Format(_T("����WriteCardDataʧ�ܣ�����ֵ[%d]:%s"), nRet, GetADELErrorInfo(nRet));
		MessageBox(m_hWnd, strTipInfo, _T("��ʾ"), MB_OK|MB_ICONINFORMATION);
		return;
	}
}

LRESULT CSDKInterface::OnClose(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& bHandled)
{
	PostQuitMessage(0);
	// ��Ҫ����bHandle = FALSE , ����ϵͳ���ܸ����ڷ���WM_NCDESTROY��Ϣ���Ӷ���ȫ�˳����
	bHandled = FALSE;
	return 0;
}