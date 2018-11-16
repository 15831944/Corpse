#include "StdAfx.h"
#include "InjectDll.h"
#include "Resource.h"
#include "Wow64Ext/Wow64InjectDll.h"
#include <CommDlg.h>
CInjectDll::CInjectDll()
{

}

CInjectDll::~CInjectDll()
{

}

void CInjectDll::OnFinalMessage(HWND hWnd)
{
	WindowImplBase::OnFinalMessage(hWnd);
	delete this;
}

LPCTSTR CInjectDll::GetWindowClassName() const
{
	return _T("InjectDll");
}

UILIB_RESOURCETYPE CInjectDll::GetResourceType() const
{
	return UILIB_FILE;
}

CDuiString CInjectDll::GetZIPFileName() const
{
	return _T("InjectDll.zip");
}

CDuiString CInjectDll::GetSkinFile()
{
	return _T("InjectDll.xml");
}

CDuiString CInjectDll::GetSkinFolder()
{
	return _T("InjectDll");
}

void CInjectDll::Notify(TNotifyUI& msg)
{
	if (msg.sType == DUI_MSGTYPE_CLICK)
		OnClick(msg);
}

void CInjectDll::InitWindow()
{
	SetIcon(IDI_MAINFRAME);

	//��32λ�����Ӧ�ó���ֻ��ע�뵽32λӦ�ó�����
	//m_RemoteInject.Attach(_T("explorer.exe"), _T("C:\\test\\ITrayClock.dll"), 2000);
	//Wow64Injectx64(6016,_T("E:\\ProjectCode\\Remote V0.0.1\\Corpse\\Bin\\x64_Release\\ITrayClock.dll"));
}

void CInjectDll::OnClick(TNotifyUI& msg)
{
	if (msg.pSender == m_PaintManager.FindControl(_T("BtnClose")))
		PostQuitMessage(0);
	else if (msg.pSender == m_PaintManager.FindControl(_T("BtnMin")))
		PostMessage(WM_SYSCOMMAND, SC_MINIMIZE, NULL);
	else if (msg.pSender == m_PaintManager.FindControl(_T("BtnLookup")))
		OnFileLookup(msg);
	else if (msg.pSender == m_PaintManager.FindControl(_T("BtnInject")))
	{
		CEditUI2* pProcessID = (CEditUI2*)m_PaintManager.FindControl(_T("ProcessID"));
		CEditUI2* pDllFile = (CEditUI2*)m_PaintManager.FindControl(_T("DllFile"));
		if (pProcessID->GetText().IsEmpty())
		{
			MessageBox(m_hWnd, _T("��Ҫע�����ID����Ϊ�գ����������ID��"), _T("��ʾ"), MB_OK|MB_ICONINFORMATION);
			pProcessID->SetFocus();
			return;
		}

		if (pDllFile->GetText().IsEmpty())
		{
			MessageBox(m_hWnd, _T("��ѡ����Ҫע��Ķ�̬���ļ���"), _T("��ʾ"), MB_OK|MB_ICONINFORMATION);
			return;
		}

		if (Wow64Injectx64(_ttoi(pProcessID->GetText()), pDllFile->GetText()) != FALSE)
			MessageBox(m_hWnd, _T("Զ�̽��̼��ض�̬��ɹ���"), _T("��ʾ"), MB_OK|MB_ICONINFORMATION);
	}
}

void CInjectDll::OnFileLookup(TNotifyUI& msg)
{
	TCHAR szFilePath[MAX_PATH];
	ZeroMemory(szFilePath, sizeof(szFilePath));

	OPENFILENAME SaveFileName = {0};
	SaveFileName.lStructSize		= sizeof(SaveFileName); 
	SaveFileName.hwndOwner		= m_hWnd; 
	SaveFileName.lpstrFile			= szFilePath; 
	SaveFileName.nMaxFile			= _countof(szFilePath);
	SaveFileName.lpstrFilter			= _T("��̬���ӿ�\0*.dll\0\0"); 
	SaveFileName.nFilterIndex		= 1; 
	SaveFileName.lpstrFileTitle	= NULL; 
	SaveFileName.nMaxFileTitle	= 0; 
	SaveFileName.lpstrTitle			= _T("��ѡ����Ҫע��Ķ�̬��");
	SaveFileName.lpstrInitialDir	= NULL ; 
	SaveFileName.Flags				= OFN_PATHMUSTEXIST|OFN_FILEMUSTEXIST ; 

	if (GetOpenFileName(&SaveFileName) == FALSE)
		return ;

	CEditUI2* pDllFilePath = (CEditUI2*)m_PaintManager.FindControl(_T("DllFile"));
	if (pDllFilePath != NULL)
		pDllFilePath->SetText(szFilePath);
}

BOOL CInjectDll::InjectDll(LPCTSTR lpszFilePath, DWORD dwRemoteProcessId)
{
	return TRUE;
}