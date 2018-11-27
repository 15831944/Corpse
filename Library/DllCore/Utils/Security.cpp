#include "StdAfx.h"
#include "Security.h"
#include "ErrorInfo.h"
#include <AclAPI.h>

BOOL EnablePrivilege(LPCTSTR lpszPrivilegeName, BOOL bEnable /*= TRUE*/)
{
	HANDLE hToken;
	BOOL bSuccess = FALSE;

	do 
	{
		//�򿪽������ƻ�
		if (OpenProcessToken(GetCurrentProcess(), TOKEN_ADJUST_PRIVILEGES|TOKEN_QUERY, &hToken) == FALSE)
		{
			SetErrorInfo(SYSTEM_ERROR, 0, _T("�򿪵�ǰ���̳��ִ���"));
			break;
		}

		//	��ý��̱���ΨһID
		LUID luid = {0};
		if (LookupPrivilegeValue(NULL, lpszPrivilegeName, &luid) == FALSE)
		{
			SetErrorInfo(SYSTEM_ERROR, 0, _T("��ȡ %s ��ʶʧ��"), lpszPrivilegeName);
			break;
		}

		TOKEN_PRIVILEGES Token;
		Token.PrivilegeCount = 1;
		Token.Privileges[0].Attributes = SE_PRIVILEGE_ENABLED;
		Token.Privileges[0].Luid = luid;

		//����Ȩ��
		if (AdjustTokenPrivileges(hToken ,0, &Token, sizeof(TOKEN_PRIVILEGES), NULL, NULL) == FALSE)
		{
			SetErrorInfo(SYSTEM_ERROR, 0, _T("��������Ȩ��ʧ��"));
			break;
		}

		bSuccess = TRUE;
	} while (FALSE);
	
	if (hToken != NULL)
	{
		CloseHandle(hToken);
		hToken = NULL;
	}

	return bSuccess;
}

//BOOL InjectProcess(LPCTSTR lpszFilePath, DWORD dwRemoteProcessId, DWORD dwWaitTime)
//{
//	HANDLE hRemoteThread = NULL;
//	HANDLE hRemoteProcess = NULL;
//	LPVOID pRemoteAddr = NULL;
//	DWORD dwSize = 0;
//
//	BOOL bSuccess = FALSE;
//
//	do 
//	{
//		 hRemoteProcess = OpenProcess(PROCESS_ALL_ACCESS, FALSE, dwRemoteProcessId );
//		if (hRemoteProcess == NULL)
//		{
//			SetErrorInfo(SYSTEM_ERROR, 0, _T("�򿪽���[%d]ʧ��"), dwRemoteProcessId);
//			break;
//		}
//		dwSize = (_tcslen(lpszFilePath)+1) * sizeof(TCHAR);
//		//ʹ��VirtualAllocEx������Զ�̽��̵��ڴ��ַ�ռ����DLL�ļ����ռ�
//		pRemoteAddr = (char *) VirtualAllocEx( hRemoteProcess, NULL, dwSize, MEM_COMMIT, PAGE_READWRITE);
//		if (pRemoteAddr == NULL)
//		{
//			SetErrorInfo(SYSTEM_ERROR, 0, _T("����Զ�̽����ڴ�ռ�ʧ��"));
//			break;
//		}
//
//		//ʹ��WriteProcessMemory������DLL��·����д�뵽Զ�̽��̵��ڴ�ռ�
//		if (WriteProcessMemory(hRemoteProcess, pRemoteAddr, (void *)lpszFilePath, dwSize, NULL) == FALSE)
//		{
//			SetErrorInfo(SYSTEM_ERROR, 0, _T("������д��Զ�̽���ʧ��"));
//			break;
//		}
//
//		//����LoadLibraryA����ڵ�ַ
//#ifdef _UNICODE
//		PTHREAD_START_ROUTINE pfnStartAddr = (PTHREAD_START_ROUTINE)GetProcAddress(GetModuleHandle(_T("Kernel32")),"LoadLibraryW");
//#else
//		PTHREAD_START_ROUTINE pfnStartAddr = (PTHREAD_START_ROUTINE)GetProcAddress(GetModuleHandle(_T("Kernel32")),"LoadLibraryA");
//#endif
//
//		//����Զ���߳�LoadLibrary��ͨ��Զ���̵߳��ô����µ��߳�
//		if( (hRemoteThread = CreateRemoteThread(hRemoteProcess, NULL, 0, (LPTHREAD_START_ROUTINE)pfnStartAddr, pRemoteAddr, 0, NULL) ) == NULL)
//		{
//			SetErrorInfo(SYSTEM_ERROR, 0, _T("����Զ���߳�ʧ��"));
//			break;
//		}
//
//		if (dwWaitTime != 0 )
//			WaitForSingleObject(hRemoteThread, dwWaitTime);
//		
//		bSuccess = TRUE;
//	} while (FALSE);
//
//	if (hRemoteThread != NULL)
//	{
//		CloseHandle(hRemoteThread);
//		hRemoteThread = NULL;
//	}
//	
//	if (hRemoteProcess != NULL)
//	{
//		VirtualFreeEx(hRemoteProcess, pRemoteAddr, dwSize, MEM_COMMIT);
//		CloseHandle(hRemoteProcess);
//		hRemoteProcess = NULL;
//	}
//
//	return bSuccess;
//}

BOOL UnLoadViewOfModule(DWORD dwProcessId, LPVOID lpBaseAddr)
{
	BOOL bSuccess = FALSE;
	HANDLE hProcess = NULL;
	do 
	{
		HMODULE hModule = GetModuleHandle(_T("ntdll.dll"));
		if (hModule == NULL)
			hModule = LoadLibrary(_T("ntdll.dll"));

		if (hModule == NULL)
		{
			SetErrorInfo(CUSTOM_ERROR, 0, _T("��ntdll���ʧ��"));
			break;
		}

		PFNNtUnmapViewOfSection pfnUnLoadModule = (PFNNtUnmapViewOfSection)GetProcAddress(hModule,"NtUnmapViewOfSection");
		if (pfnUnLoadModule == NULL)
		{
			SetErrorInfo(CUSTOM_ERROR, 0, _T("��ȡ����[NtUnmapViewOfSection]��ַʧ��"));
			break;
		}

		hProcess = OpenProcess(PROCESS_ALL_ACCESS, TRUE, dwProcessId);
		if (hProcess == NULL)
		{
			SetErrorInfo(SYSTEM_ERROR, 0, _T("�򿪽���ʧ��"));
			break;
		}

		if (pfnUnLoadModule(hProcess, lpBaseAddr) != 0)
		{
			SetErrorInfo(SYSTEM_ERROR, 0, _T("ж��Զ��ָ��ģ��ʧ��"));
			break;
		}

		bSuccess = TRUE;
	} while (FALSE);

	if (hProcess != NULL)
	{
		CloseHandle(hProcess);
		hProcess = NULL;
	}

	return bSuccess;
}

BOOL ModifyObjectSecurityToAccessAll(HANDLE hObject)
{
	BOOL bSuccess = FALSE;
	DWORD dwLen = 0;
	PSECURITY_DESCRIPTOR pSecurityInfo = NULL;
	do 
	{
		if (hObject == NULL)
			break;

		// ��ȡ��ȫ����������
		if (GetKernelObjectSecurity(hObject, DACL_SECURITY_INFORMATION, 0, 0, &dwLen) == FALSE)
			break;

		// ��ȡ��ȫ��������
		pSecurityInfo = (PSECURITY_DESCRIPTOR)new BYTE[dwLen];
		if (GetKernelObjectSecurity(hObject, DACL_SECURITY_INFORMATION, pSecurityInfo, dwLen,&dwLen) == FALSE)
			break;

		BOOL bDaclPresent,bDaclDefaulted;
		PACL pDacl = NULL;
		GetSecurityDescriptorDacl(pSecurityInfo, &bDaclPresent, &pDacl, &bDaclDefaulted);

		TCHAR szUserName[MAX_PATH];DWORD dwBufferSize=MAX_PATH;
		GetUserName(szUserName, &dwBufferSize);

		EXPLICIT_ACCESS ea;
		BuildExplicitAccessWithName(&ea,szUserName,TOKEN_ALL_ACCESS,GRANT_ACCESS,FALSE);

		PACL pNewDacl;
		SetEntriesInAcl(1,&ea,pDacl,&pNewDacl);
		LocalFree(pDacl);

		DWORD dwAbsoluteSDSize=0,dwAbsDaclSize=0,dwSaclSize=0,dwOwnerSize=0,dwPrimaryGroupSize=0;
		MakeAbsoluteSD(pSecurityInfo,0,&dwAbsoluteSDSize,0,&dwAbsDaclSize,0,&dwSaclSize,0,&dwOwnerSize,0,&dwPrimaryGroupSize);

		PSECURITY_DESCRIPTOR pAbsoluteSD=0;
		PACL pAbsDacl=0,pSacl=0;PSID pOwner=0,pPrimaryGroup=0;
		if (dwAbsoluteSDSize)
			pAbsoluteSD=LocalAlloc(LPTR,dwAbsoluteSDSize);
		if (dwAbsDaclSize)
			pAbsDacl=(PACL)LocalAlloc(LPTR,dwAbsDaclSize);
		if (dwSaclSize)
			pSacl=(PACL)LocalAlloc(LPTR,dwSaclSize);
		if (dwOwnerSize)
			pOwner=(PSID)LocalAlloc(LPTR,dwOwnerSize);
		if (dwPrimaryGroupSize)
			pPrimaryGroup=(PSID)LocalAlloc(LPTR,dwPrimaryGroupSize);
		MakeAbsoluteSD(pSecurityInfo,pAbsoluteSD,&dwAbsoluteSDSize,pAbsDacl,&dwAbsDaclSize,pSacl,&dwSaclSize,pOwner,&dwOwnerSize,pPrimaryGroup,&dwPrimaryGroupSize);
		SetSecurityDescriptorDacl(pAbsoluteSD,bDaclPresent,pNewDacl,bDaclDefaulted);
		SetKernelObjectSecurity(hObject,DACL_SECURITY_INFORMATION, pAbsoluteSD);

		if (pAbsoluteSD)
			LocalFree(pAbsoluteSD);
		if (pAbsDacl)
			LocalFree(pAbsDacl);
		if (pSacl)
			LocalFree(pSacl);
		if (pOwner)
			LocalFree(pOwner);
		if (pPrimaryGroup)
			LocalFree(pPrimaryGroup);

		bSuccess = TRUE;
	} while (FALSE);
	
	if (pSecurityInfo)
		delete[] pSecurityInfo;

	return bSuccess;
}

BOOL ModifyAccessToEveryOne(LPCTSTR lpszRegPath)
{
	BOOL bSuccess = FALSE;

	PSECURITY_DESCRIPTOR pSD = NULL;
	PACL pOldDacl = NULL;
	PACL pNewDacl = NULL;

	do
	{
		//��ȡ������DACL     
		DWORD dwRet = GetNamedSecurityInfo((LPTSTR)lpszRegPath, SE_REGISTRY_KEY, DACL_SECURITY_INFORMATION,     
																			NULL, NULL, &pOldDacl, NULL, &pSD);
		if (dwRet != ERROR_SUCCESS)
		{
			SetErrorInfo(SYSTEM_ERROR, dwRet, _T("��ȡ����DACLʧ��"));
			break;
		}     

		//����һ��ACE������Everyone��ȫ���ƶ��󣬲������Ӷ���̳д�Ȩ��
		EXPLICIT_ACCESS ea;     
		ZeroMemory(&ea, sizeof(EXPLICIT_ACCESS));     
		BuildExplicitAccessWithName(&ea, _T("Everyone"), KEY_ALL_ACCESS, SET_ACCESS, SUB_CONTAINERS_AND_OBJECTS_INHERIT);     

		//���µ�ACE����DACL     
		dwRet = SetEntriesInAcl(1, &ea, pOldDacl, &pNewDacl);     
		if (dwRet !=ERROR_SUCCESS)     
		{
			SetErrorInfo(SYSTEM_ERROR, dwRet, _T("���´�����ACE����DACLʧ��"));
			break;
		}     

		//����������DACL     
		dwRet = SetNamedSecurityInfo((LPTSTR)lpszRegPath, SE_REGISTRY_KEY, DACL_SECURITY_INFORMATION,     
			NULL, NULL, pNewDacl, NULL);     
		if (dwRet != ERROR_SUCCESS)     
		{
			SetErrorInfo(SYSTEM_ERROR, dwRet, _T("����������DACLʧ��"));
			break;
		}

		bSuccess = TRUE;
	}while(FALSE);

	if (pSD)
		LocalFree(pSD);
	if (pOldDacl)
		LocalFree(pOldDacl);
	if (pNewDacl)
		LocalFree(pNewDacl);

	return bSuccess;
}