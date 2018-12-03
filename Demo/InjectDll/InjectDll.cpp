#include "StdAfx.h"
#include "InjectDll.h"
#include "Resource.h"
#include "Wow64Ext/Wow64InjectDll.h"
#include <CommDlg.h>
#include <tlhelp32.h>

CInjectDll::CInjectDll()
{
	m_pListView = NULL;
}

CInjectDll::~CInjectDll()
{
	if (m_pListView != NULL)
	{
		delete m_pListView;
	}
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
	else if (msg.sType == DUI_MSGTYPE_TEXTCHANGED)
		OnTextChanged(msg);
	else if (msg.sType == DUI_MSGTYPE_WINDOWINIT)
		OnWindowInit(msg);
	else if (msg.sType == DUI_MSGTYPE_RETURN)
		OnReturn(msg);
	else if (msg.sType == DUI_MSGTYPE_KILLFOCUS)
		OnKillFocus(msg);
}

void CInjectDll::InitWindow()
{
	SetIcon(IDI_MAINFRAME);

	//��32λ�����Ӧ�ó���ֻ��ע�뵽32λӦ�ó�����
	//m_RemoteInject.Attach(_T("explorer.exe"), _T("C:\\test\\ITrayClock.dll"), 2000);
	//Wow64InjectX64(6016,_T("E:\\ProjectCode\\Remote V0.0.1\\Corpse\\Bin\\x64_Release\\ITrayClock.dll"));
	CEditUI2* pProcessID = (CEditUI2*)m_PaintManager.FindControl(_T("ProcessID"));
	//pProcessID->Event += MakeDelegate(OnMouseHover);
	pProcessID->SetFocus();
}

LRESULT CInjectDll::OnSetFocus(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	/*m_pListView->ShowWindow(true, false);*/
	if (IsWindowVisible(m_hWnd) == FALSE)
		return TRUE;
	POINT pt = { GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam) };
	CControlUI* pHover = m_PaintManager.FindControl(pt);
	if( pHover == NULL ) return 0;
	if (pHover->GetName() == _T("ProcessID"))
		m_pListView->ShowWindow(true,false);

	return TRUE;
}

LRESULT CInjectDll::HandleMessage(UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	BOOL bHandle = FALSE;
	if (uMsg == WM_KEYDOWN) {
		if (m_pListView->OnKeyDown(uMsg, wParam, lParam, bHandle) == FALSE)
			return FALSE;
	} else if (uMsg == WM_WINDOWPOSCHANGING) {
		if (m_pListView == NULL)
			return TRUE;
		return m_pListView->OnPosChanged(uMsg, wParam, lParam, bHandle);
	} else if (uMsg == WM_MOUSEWHEEL) {
		return m_pListView->OnMouseWheel(uMsg, wParam, lParam, bHandle);
	} else if (uMsg == WM_NCLBUTTONDOWN || uMsg == WM_NCRBUTTONDOWN) {
		// ���Ӧ�ó��������Ӧ�ùر���ʾ��
		if (wParam == HTCAPTION)
			m_pListView->ShowWindow(false);
	}
	return WindowImplBase::HandleMessage(uMsg, wParam, lParam);
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
		//MessageBox(m_hWnd, pProcessID->GetText(), _T("��ʾ"), MB_OK|MB_ICONINFORMATION);
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

		UINT nProcessId = pProcessID->GetTag();

		if (Wow64InjectX64(nProcessId, pDllFile->GetText()) != FALSE)
			MessageBox(m_hWnd, _T("Զ�̽��̼��ض�̬��ɹ���"), _T("��ʾ"), MB_OK|MB_ICONINFORMATION);
	}
}

void CInjectDll::OnKillFocus(TNotifyUI& msg)
{
	if (msg.pSender->GetName() == _T("ProcessID"))
	{
		OutputDebugString(_T("OnKillFocus"));
		if (m_pListView != NULL)
			m_pListView->ShowWindow(false);
	}
}

void CInjectDll::OnWindowInit(TNotifyUI& msg)
{
	CEditUI2* pPID = (CEditUI2*)m_PaintManager.FindControl(_T("ProcessID"));
	m_pListView = new CListViewWnd(m_hWnd, pPID);	
}

void CInjectDll::OnTextChanged(TNotifyUI& msg)
{
	if (msg.pSender->GetName() == _T("ProcessID"))
	{
		CDuiString strProcess = msg.pSender->GetText();

		CProcessInfoList ProcessInfoList;
		EnumProcessesBySnapshot(ProcessInfoList);
		m_pListView->RemoveAll();

		POSITION pos = ProcessInfoList.GetHeadPosition();
		while(pos)
		{
			const PROCESS_INFO& ProcessInfo = ProcessInfoList.GetNext(pos);
			if (strProcess.IsEmpty() || StrStrI(ProcessInfo.strProcessName, strProcess) == NULL)
				continue;
			m_pListView->Add(ProcessInfo);
		}
		m_pListView->ResizeWnd();		
	}
}

void CInjectDll::OnReturn(TNotifyUI& msg)
{
	if (msg.pSender->GetName() == _T("ProcessID"))
	{
		OutputDebugString(_T("OnReturn"));
		m_pListView->ShowWindow(false);
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

BOOL CInjectDll::EnumProcessesBySnapshot(CProcessInfoList& ProcessInfoList)
{
	BOOL bSuccess = FALSE;
	HANDLE hSnap;
	DWORD dwWinlogonProcessId = 0;
	do
	{
		hSnap = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
		if (hSnap == INVALID_HANDLE_VALUE)
			break;

		PROCESSENTRY32 ProcessEntry;
		ProcessEntry.dwSize = sizeof(PROCESSENTRY32);
		if (Process32First(hSnap, &ProcessEntry) == FALSE)
			break;

		do
		{
			PROCESS_INFO ProcessInfo;
			ProcessInfo.nProcessId = ProcessEntry.th32ProcessID;
			ProcessInfo.strProcessName = ProcessEntry.szExeFile;
			ProcessInfoList.AddTail(ProcessInfo);
		}while(Process32Next(hSnap, &ProcessEntry) == TRUE);

		bSuccess = TRUE;
	}while(FALSE);

	if (hSnap != INVALID_HANDLE_VALUE)
		CloseHandle(hSnap);

	return bSuccess;
}

BOOL CInjectDll::InjectDll(LPCTSTR lpszFilePath, DWORD dwRemoteProcessId)
{
	return TRUE;
}