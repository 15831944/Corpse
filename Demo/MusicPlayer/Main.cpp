#include "stdafx.h"
#include "resource.h"
#include "PlayerMain.h"


int APIENTRY WinMain(HINSTANCE hInstance, HINSTANCE /*hPrevInstance*/, LPSTR /*lpCmdLine*/, int nCmdShow)
{
	ULONG_PTR gdiplusToken; 
	GdiplusStartupInput gdiplusStartupInput;
	GdiplusStartup(&gdiplusToken, &gdiplusStartupInput, NULL);

	CPaintManagerUI::SetInstance(hInstance);

	HRESULT hRet = ::CoInitialize(NULL);
	if (FAILED(hRet)) 
		return 0;

	/*CLyrics* pLyric = new CLyrics(_T("C:\\���õ��� - ��ظ�ԭ.lrc"));
	DWORD dwTime = 12170;
	CString strLyric = pLyric->GetLyric(dwTime);
	int nProcess = pLyric->GetLyricProgress(dwTime);*/

	CPlayerMain* pPlayerMain = new CPlayerMain();
	if( pPlayerMain == NULL ) 
		return 0;
	
	//pPlayerMain->Create(NULL, _T("��������--������"), UI_WNDSTYLE_DIALOG, UI_WNDSTYLE_EX_DIALOG, 0, 0, 884, 652);
	pPlayerMain->Create(NULL, _T("��������--������"), UI_WNDSTYLE_DIALOG, 0, 0, 0, 884, 652);
	pPlayerMain->CenterWindow();
	pPlayerMain->ShowWindow(true);
	CPaintManagerUI::MessageLoop();

	// ж��GDI+
	GdiplusShutdown(gdiplusToken);

	::CoUninitialize();
	return 0;
}