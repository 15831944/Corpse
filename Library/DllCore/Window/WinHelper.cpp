#include "StdAfx.h"
#include "WinHelper.h"
#include <ShlDisp.h>

void ShowDesktop()
{
	IShellDispatch4* pDisp = NULL;
	CoCreateInstance(CLSID_Shell, NULL, CLSCTX_ALL, __uuidof(IShellDispatch4), (void **)&pDisp);
	if (pDisp != NULL)
	{
		// ��ʾ����
		pDisp->ToggleDesktop();
		pDisp->Release();
	}
}