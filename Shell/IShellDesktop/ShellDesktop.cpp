#include "StdAfx.h"
#include "ShellDesktop.h"

const UINT WM_TOGGLEDESKTOP			= RegisterWindowMessage(TOGGLEDESKTOP);
const UINT WM_TOGGLEDESKTOPMSG	= RegisterWindowMessage(TOGGLEDESKTOPMSG);
const UINT WM_FreeLibrary						= RegisterWindowMessage(FREELIBRARYANDEXIT);

typedef BOOL (WINAPI* FN_ChangeWindowMessageFilter)(UINT uMsg, DWORD dwFlag);

#ifndef MSGFLT_ADD
	#define MSGFLT_ADD 1
	#define MSGFLT_REMOVE 2
#endif

CShellDesktop::CShellDesktop(IFinalCallback* pFinalCallback)
{
	m_lRef = 0;
	m_pFinalCallback = pFinalCallback;
}

CShellDesktop::~CShellDesktop()
{

}


void CShellDesktop::Init(HWND hWnd)
{ 
	WindowImplBase::Init(hWnd);

	// ChangeWindowMessageFilter ��Vistaϵͳ�Ŵ��ڣ���Ҫʹ�ö�̬����
	HMODULE hModule = LoadLibrary(_T("user32.dll"));
	if (hModule)
	{
		FN_ChangeWindowMessageFilter fnChangeWndMsgFilter = 
											(FN_ChangeWindowMessageFilter)GetProcAddress(hModule, "ChangeWindowMessageFilter");
		if (fnChangeWndMsgFilter)
		{
			fnChangeWndMsgFilter(WM_TOGGLEDESKTOPMSG, MSGFLT_ADD);
			fnChangeWndMsgFilter(WM_FreeLibrary, MSGFLT_ADD);
		}
	}
}

void CShellDesktop::OnFinalMessage(HWND hWnd)
{
	WindowImplBase::OnFinalMessage(hWnd);
	if (m_pFinalCallback)
		m_pFinalCallback->OnFinalCallback();
	delete this;
}

LRESULT CShellDesktop::HandleCustomMessage(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	// ��ǰӦ���ͷ�����������
	if (uMsg == WM_FreeLibrary) {
		LONG lRef = 0;
		if (wParam == FALSE)
			lRef = -- m_lRef;
		
		// ���wParam == TRUE,��Ҫǿ��ж�ز�����������޷�����
		if (lRef <= 0 || wParam == TRUE)
			OnFinalMessage(m_hWnd);

		bHandled = TRUE;
		return FALSE;
	} else if (uMsg == WM_TOGGLEDESKTOPMSG) {
		// ��ȡ������������״̬
		LRESULT lRet = FALSE;
		if (wParam == DESKTOP_PLUGIN_WORKING)
			lRet = TRUE;
		else if (wParam == DESKTOP_PLUGIN_COUNT)
			lRet = m_lRef;
		else if (wParam == DESKTOP_PLUGIN_ADDREF)
			lRet = m_lRef++;
		bHandled = TRUE;
		return lRet;
	}

	switch(uMsg)
	{
		case DTM_RAISE:
				PostMessage(HWND_BROADCAST, WM_TOGGLEDESKTOP, wParam, lParam);
			break;
	}

	return WindowImplBase::HandleCustomMessage(uMsg, wParam, lParam, bHandled);
}