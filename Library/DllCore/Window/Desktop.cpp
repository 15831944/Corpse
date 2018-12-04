#include "StdAfx.h"
#include "Desktop.h"

typedef struct tagLVITEM64A
{
	UINT mask;
	int iItem;
	int iSubItem;
	UINT state;
	UINT stateMask;
	INT64 pszText;
	int cchTextMax;
	int iImage;
	LPARAM lParam;
#if (_WIN32_IE >= 0x0300)
	int iIndent;
#endif
#if (_WIN32_WINNT >= 0x501)
	int iGroupId;
	UINT cColumns; // tile view columns
	PUINT puColumns;
#endif
} LVITEM64A, *LPLVITEM64A;

// ��ȡ����ͼ�������������ֵ
#define MAX_BUFFER_LEN		512

HWND GetDesktopHWND()
{
	// ������SysListView32�Ĵ��ھ��
	HWND hDeskWnd = NULL;
	// �ȵ�WIN7ϵͳ����
	HWND hWnd = ::FindWindow(_T("WorkerW"),NULL);
	while(hWnd)
	{
		HWND hShellView = ::FindWindowEx(hWnd, NULL, _T("SHELLDLL_DefView"), NULL);
		if(hShellView)
		{
			hDeskWnd = ::FindWindowEx(hShellView, NULL, _T("SysListView32"), NULL);
			break;
		}
		hWnd = ::GetWindow(hWnd, GW_HWNDNEXT);
	}

	// ���û�ҵ����ٰ�XP��ʽ����
	if (hDeskWnd == NULL)
	{
		hWnd = ::FindWindow(_T("Progman"), _T("Program Manager"));
		if(hWnd){
			hWnd = ::FindWindowEx(hWnd, NULL, _T("SHELLDLL_DefView"), NULL);
			hDeskWnd = ::FindWindowEx(hWnd, NULL, _T("SysListView32"), NULL);
		}
	}

	return hDeskWnd;
}

BOOL GetDestopIcon(HWND hDeskWnd, CDesktopInfoList& DesktopInfoList)
{
	HANDLE hProcess = NULL;
	do 
	{
		DWORD dwPid = 0;
		GetWindowThreadProcessId(hDeskWnd, &dwPid);

		hProcess = OpenProcess(PROCESS_ALL_ACCESS, FALSE, dwPid);
		if (hProcess == NULL)
			break;

		// ����LVITEMA�ṹ���ڴ�
		LVITEMA* pLVITEM=(LVITEMA*)VirtualAllocEx(hProcess, NULL, sizeof(LVITEM), MEM_COMMIT, PAGE_READWRITE);
		// ����ͼ�������ڴ�
		CHAR* pszText = (CHAR*)VirtualAllocEx(hProcess, NULL, MAX_BUFFER_LEN, MEM_COMMIT, PAGE_READWRITE);
		// ����ͼ��λ���ڴ�
		RECT* pItemPos = (RECT*)VirtualAllocEx(hProcess, NULL, sizeof(RECT), MEM_COMMIT,PAGE_READWRITE);

		if (pItemPos == NULL || pLVITEM == NULL)
			break;

		LVITEMA lVItem;
		lVItem.mask=LVIF_TEXT;
		lVItem.cchTextMax = MAX_BUFFER_LEN;
		lVItem.pszText=pszText;
		CHAR ItemBuf[MAX_BUFFER_LEN];
		int nCount=::SendMessage(hDeskWnd, LVM_GETITEMCOUNT, 0, 0);
		for(int iItem=0; iItem<nCount; iItem++)
		{
			lVItem.iItem = iItem;
			lVItem.iSubItem=0;
			// �����úõĽṹ����Ŀ�����
			WriteProcessMemory(hProcess, pLVITEM, &lVItem, sizeof(LVITEM), NULL);
			// ����LVM_GETITEM��Ϣ
			BOOL bRet = (BOOL)::SendMessage(hDeskWnd, LVM_GETITEMTEXTA, iItem, (LPARAM)pLVITEM);
			// ��ȡpszText
			ReadProcessMemory(hProcess, pszText, ItemBuf, _countof(ItemBuf), NULL);

			DESKTOP_INFO DesktopInfo;

			RECT rcPos;
			DesktopInfo.strIconName = ItemBuf;
			::SendMessage(hDeskWnd, LVM_GETITEMRECT, iItem, (LPARAM)pItemPos);
			ReadProcessMemory(hProcess, pItemPos, &rcPos, sizeof(RECT), NULL);
			memcpy(&DesktopInfo.rcPos, &rcPos, sizeof(RECT));
			DesktopInfoList.AddTail(DesktopInfo);
		}

		// �ͷ��ڴ�
		VirtualFreeEx(hProcess, pLVITEM, 0,MEM_RELEASE);
		VirtualFreeEx(hProcess, pszText, 0, MEM_RELEASE);
		VirtualFreeEx(hProcess, pItemPos, 0,MEM_RELEASE);

	} while (FALSE);

	if (hProcess != NULL)
		CloseHandle(hProcess);

	return DesktopInfoList.IsEmpty() == FALSE;
}

BOOL GetDestopIcon64(HWND hDeskWnd, CDesktopInfoList& DesktopInfoList)
{
	HANDLE hProcess = NULL;
	do 
	{
		DWORD dwPid = 0;
		GetWindowThreadProcessId(hDeskWnd, &dwPid);

		hProcess = OpenProcess(PROCESS_ALL_ACCESS, FALSE, dwPid);
		if (hProcess == NULL)
			break;

		// ����LVITEMA�ṹ���ڴ�
		LVITEM64A* pLVITEM=(LVITEM64A*)VirtualAllocEx(hProcess, NULL, sizeof(LVITEM64A), MEM_COMMIT, PAGE_READWRITE);
		// ����ͼ�������ڴ�
		CHAR* pszText = (CHAR*)VirtualAllocEx(hProcess, NULL, MAX_BUFFER_LEN, MEM_COMMIT, PAGE_READWRITE);
		// ����ͼ��λ���ڴ�
		RECT* pItemPos = (RECT*)VirtualAllocEx(hProcess,NULL,sizeof(RECT),MEM_COMMIT,PAGE_READWRITE);

		if (pItemPos == NULL || pLVITEM == NULL)
			break;

		LVITEM64A lVItem;
		lVItem.mask = LVIF_TEXT;
		lVItem.cchTextMax = MAX_BUFFER_LEN;
		lVItem.pszText = (INT64)pszText;
		CHAR ItemBuf[MAX_BUFFER_LEN];
		int nCount=::SendMessage(hDeskWnd, LVM_GETITEMCOUNT, 0, 0);
		for(int iItem=0; iItem<nCount; iItem++)
		{
			lVItem.iItem = iItem;
			lVItem.iSubItem=0;
			//�����úõĽṹ����Ŀ�����
			WriteProcessMemory(hProcess, pLVITEM, &lVItem, sizeof(LVITEM), NULL);
			//����LVM_GETITEM��Ϣ
			BOOL bRet = (BOOL)::SendMessage(hDeskWnd, LVM_GETITEMTEXTA, iItem, (LPARAM)pLVITEM);
			//��ȡpszText
			ReadProcessMemory(hProcess, pszText, ItemBuf, _countof(ItemBuf), NULL);

			DESKTOP_INFO DesktopInfo;

			RECT rcPos;
			DesktopInfo.strIconName = ItemBuf;
			::SendMessage(hDeskWnd, LVM_GETITEMRECT, iItem, (LPARAM)pItemPos);
			ReadProcessMemory(hProcess, pItemPos, &rcPos, sizeof(RECT), NULL);
			memcpy(&DesktopInfo.rcPos, &rcPos, sizeof(RECT));

			DesktopInfoList.AddTail(DesktopInfo);
		}

		// �ͷ��ڴ�
		VirtualFreeEx(hProcess, pLVITEM, 0,MEM_RELEASE);
		VirtualFreeEx(hProcess, pszText, 0, MEM_RELEASE);
		VirtualFreeEx(hProcess, pItemPos, 0,MEM_RELEASE);

	}while(FALSE);
	if (hProcess != NULL)
		CloseHandle(hProcess);

	return DesktopInfoList.IsEmpty() == FALSE;
}

BOOL GetDesktopIconRect(HWND hDeskWnd, LPCTSTR lpszIconName, LPRECT lpRect, BOOL bVague/* = FALSE*/)
{
	BOOL bSuccess = FALSE;
	HANDLE hProcess = NULL;
	do 
	{
		DWORD dwPid = 0;
		GetWindowThreadProcessId(hDeskWnd, &dwPid);

		hProcess = OpenProcess(PROCESS_ALL_ACCESS, FALSE, dwPid);
		if (hProcess == NULL)
			break;

		// ����LVITEMA�ṹ���ڴ�
		LVITEMA* pLVITEM=(LVITEMA*)VirtualAllocEx(hProcess, NULL, sizeof(LVITEM), MEM_COMMIT, PAGE_READWRITE);
		// ����ͼ�������ڴ�
		CHAR* pszText = (CHAR*)VirtualAllocEx(hProcess, NULL, MAX_BUFFER_LEN, MEM_COMMIT, PAGE_READWRITE);
		// ����ͼ��λ���ڴ�
		RECT* pItemPos = (RECT*)VirtualAllocEx(hProcess, NULL, sizeof(RECT), MEM_COMMIT,PAGE_READWRITE);

		if (pItemPos == NULL || pLVITEM == NULL)
			break;

		LVITEMA lVItem;
		lVItem.mask=LVIF_TEXT;
		lVItem.cchTextMax = MAX_BUFFER_LEN;
		lVItem.pszText=pszText;
		CHAR ItemBuf[MAX_BUFFER_LEN];
		int nCount=::SendMessage(hDeskWnd, LVM_GETITEMCOUNT, 0, 0);
		for(int iItem=0; iItem<nCount; iItem++)
		{
			lVItem.iItem = iItem;
			lVItem.iSubItem=0;
			// �����úõĽṹ����Ŀ�����
			WriteProcessMemory(hProcess, pLVITEM, &lVItem, sizeof(LVITEM), NULL);
			// ����LVM_GETITEM��Ϣ
			BOOL bRet = (BOOL)::SendMessage(hDeskWnd, LVM_GETITEMTEXTA, iItem, (LPARAM)pLVITEM);
			// ��ȡpszText
			ReadProcessMemory(hProcess, pszText, ItemBuf, _countof(ItemBuf), NULL);

			CString strItemName = CString(ItemBuf);
			BOOL bFind = FALSE;
			if (bVague != FALSE)
				bFind = _tcsnicmp(strItemName, lpszIconName, _tcslen(lpszIconName)) == 0;
			else 
				bFind = strItemName.CompareNoCase(lpszIconName) == 0;
			if (bFind)
			{
				RECT rcPos;
				::SendMessage(hDeskWnd, LVM_GETITEMRECT, iItem, (LPARAM)pItemPos);
				ReadProcessMemory(hProcess, pItemPos, &rcPos, sizeof(RECT), NULL);
				memcpy(lpRect, &rcPos, sizeof(RECT));
				bSuccess = TRUE;
				break;
			}
		}

		// �ͷ��ڴ�
		VirtualFreeEx(hProcess, pLVITEM, 0,MEM_RELEASE);
		VirtualFreeEx(hProcess, pszText, 0, MEM_RELEASE);
		VirtualFreeEx(hProcess, pItemPos, 0,MEM_RELEASE);
		
	} while (FALSE);
	
	if (hProcess != NULL)
		CloseHandle(hProcess);

	return bSuccess;
}

BOOL GetDesktopIconRect64(HWND hDeskWnd, LPCTSTR lpszIconName, LPRECT lpRect, BOOL bVague/* = FALSE*/)
{
	BOOL bSuccess = FALSE;
	HANDLE hProcess = NULL;
	do 
	{
		DWORD dwPid = 0;
		GetWindowThreadProcessId(hDeskWnd, &dwPid);

		hProcess = OpenProcess(PROCESS_ALL_ACCESS, FALSE, dwPid);
		if (hProcess == NULL)
			break;

		// ����LVITEMA�ṹ���ڴ�
		LVITEM64A* pLVITEM=(LVITEM64A*)VirtualAllocEx(hProcess, NULL, sizeof(LVITEM64A), MEM_COMMIT, PAGE_READWRITE);
		// ����ͼ�������ڴ�
		CHAR* pszText = (CHAR*)VirtualAllocEx(hProcess, NULL, MAX_BUFFER_LEN, MEM_COMMIT, PAGE_READWRITE);
		// ����ͼ��λ���ڴ�
		RECT* pItemPos = (RECT*)VirtualAllocEx(hProcess,NULL,sizeof(RECT),MEM_COMMIT,PAGE_READWRITE);
		
		if (pItemPos == NULL || pLVITEM == NULL)
			break;

		LVITEM64A lVItem;
		lVItem.mask = LVIF_TEXT;
		lVItem.cchTextMax = MAX_BUFFER_LEN;
		lVItem.pszText = (INT64)pszText;
		CHAR ItemBuf[MAX_BUFFER_LEN];
		int nCount=::SendMessage(hDeskWnd, LVM_GETITEMCOUNT, 0, 0);
		for(int iItem=0; iItem<nCount; iItem++)
		{
			lVItem.iItem = iItem;
			lVItem.iSubItem=0;
			//�����úõĽṹ����Ŀ�����
			WriteProcessMemory(hProcess, pLVITEM, &lVItem, sizeof(LVITEM), NULL);
			//����LVM_GETITEM��Ϣ
			BOOL bRet = (BOOL)::SendMessage(hDeskWnd, LVM_GETITEMTEXTA, iItem, (LPARAM)pLVITEM);
			//��ȡpszText
			ReadProcessMemory(hProcess, pszText, ItemBuf, _countof(ItemBuf), NULL);

			CString strItemName = CString(ItemBuf);
			BOOL bFind = FALSE;
			if (bVague != FALSE)
				bFind = _tcsnicmp(strItemName, lpszIconName, _tcslen(lpszIconName)) == 0;
			else 
				bFind = strItemName.CompareNoCase(lpszIconName) == 0;
			if (bFind)
			{
				RECT rcPos;
				::SendMessage(hDeskWnd,LVM_GETITEMRECT,iItem,(LPARAM)pItemPos);
				ReadProcessMemory(hProcess, pItemPos, &rcPos, sizeof(RECT), NULL);
				memcpy(lpRect, &rcPos, sizeof(RECT));
			
				bSuccess = TRUE;
				break;
			}
		}

		// �ͷ��ڴ�
		VirtualFreeEx(hProcess, pLVITEM, 0,MEM_RELEASE);
		VirtualFreeEx(hProcess, pszText, 0, MEM_RELEASE);
		VirtualFreeEx(hProcess, pItemPos, 0,MEM_RELEASE);
	}while(FALSE);
	if (hProcess != NULL)
		CloseHandle(hProcess);

	return bSuccess;
}