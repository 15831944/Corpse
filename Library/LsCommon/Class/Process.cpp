#include "stdafx.h"
#include "Process.h"
#include "../CommonFunc.h"
#include "../ErrorInfo.h"
#include "../FileTools.h"
#include "../NtDll.h"
#include "../OsInfo.h"
#include <Aclapi.h>

typedef LONG (WINAPI *PROCNTQSIP)(HANDLE,UINT,PVOID,ULONG,PULONG);

BOOL EnumSystemProcess(CLsSystemProcessList& LsSystemProcessList)
{
	LsSystemProcessList.RemoveAll();
	BOOL bSuccess = FALSE;
	LPBYTE lpBuffData = NULL;
	do 
	{
		DWORD dwSize = 0;
		NTSTATUS Ntstaus = NtQuerySystemInformation(SystemProcessInformation,lpBuffData,0,&dwSize);
		if (Ntstaus == STATUS_INFO_LENGTH_MISMATCH )
		{
			lpBuffData = new BYTE[dwSize+1];
			if (lpBuffData == NULL)
			{
				SetErrorInfo(SYSTEM_ERROR,0,_T("分配内存失败"));
				break;
			}

			Ntstaus = NtQuerySystemInformation(SystemProcessInformation,lpBuffData,dwSize+1,&dwSize);
			if (NT_SUCCESS(Ntstaus) == FALSE)
			{
				SetErrorInfo(SYSTEM_ERROR,0,_T("获取系统进程信息失败"));
				OutputDebugString(GetThreadErrorInfoString());
				break;
			}
		}

		if (NT_SUCCESS(Ntstaus) == TRUE)
		{
			PLS_SYSTEM_PROCESSES_INFO pLsSystemProcessInfo = (PLS_SYSTEM_PROCESSES_INFO)lpBuffData;
			while(pLsSystemProcessInfo != NULL)
			{
				LS_SYSTEM_PROCESS LsSystemProcess;
				LsSystemProcess.strProcessName = CString(pLsSystemProcessInfo->ProcessName.Buffer,pLsSystemProcessInfo->ProcessName.Length);
				LsSystemProcess.dwProcessId = pLsSystemProcessInfo->ProcessId;
				GetProcessFullPath(pLsSystemProcessInfo->ProcessId,LsSystemProcess.strProcessPath);
				if (LsSystemProcess.dwProcessId == 0)
				{
					LsSystemProcess.strProcessName = _T("System Idle Process");
					LsSystemProcess.strUserName = _T("SYSTEM");
				}

				if (LsSystemProcess.dwProcessId == 4)
				{
					LsSystemProcess.strProcessPath = GetSystemDirectoryEx(_T("ntkrnlpa.exe"));
					LsSystemProcess.strUserName = _T("SYSTEM");
				}
				LsSystemProcess.dwThread = pLsSystemProcessInfo->ThreadCount;
				LsSystemProcess.dwHandle = pLsSystemProcessInfo->HandleCount;
				LsSystemProcess.dwMemUse = pLsSystemProcessInfo->MemoryCount;
				
				LsSystemProcess.ullProcessUseTime = pLsSystemProcessInfo->KernelTime + pLsSystemProcessInfo->UserTime;
				
				LsSystemProcess.dwCPU = 0;

				GetProcessUserName(pLsSystemProcessInfo->ProcessId,LsSystemProcess.strUserName);
				LsSystemProcess.dwConsole = pLsSystemProcessInfo->dwSessionId;
				GetFileVersionEx(LsSystemProcess.strProcessPath,FILE_VERSION_FileDescription,LsSystemProcess.strDescrible);
				GetFileVersionEx(LsSystemProcess.strProcessPath,FILE_VERSION_FileVersion,LsSystemProcess.strFileVersion);
				GetFileVersionEx(LsSystemProcess.strProcessPath,FILE_VERSION_CompanyName,LsSystemProcess.strCompanyName);
				LsSystemProcessList.AddTail(LsSystemProcess);

				if (pLsSystemProcessInfo->NextEntryDelta == NULL)
					pLsSystemProcessInfo = NULL;
				else
					pLsSystemProcessInfo =(PLS_SYSTEM_PROCESSES_INFO)((DWORD)pLsSystemProcessInfo + pLsSystemProcessInfo->NextEntryDelta);
			}
		}

		bSuccess = TRUE;
	} while (FALSE);	


	if (lpBuffData != NULL)
	{
		delete[] lpBuffData;
		lpBuffData = NULL;
	}

	return bSuccess;
}

UINT GetModuleFileIcon(LPCTSTR lpszProcessPath,CImageList* ImgList)
{
	if (lpszProcessPath == NULL || lpszProcessPath[0] == NULL)
		return 0;

	if (PathFileExists(lpszProcessPath) == FALSE)
		return 0;

	SHFILEINFO SHFileInfo;
	SHGetFileInfo(lpszProcessPath,0,&SHFileInfo,sizeof(SHFileInfo),SHGFI_ICON|SHGFI_SMALLICON);

	UINT nIcoIndex = ImgList->Add(SHFileInfo.hIcon);
	DestroyIcon(SHFileInfo.hIcon);

	return nIcoIndex;
}

BOOL GetProcessHandle(DWORD dwPID, HANDLE& hProcess)
{
	hProcess = OpenProcess(PROCESS_ALL_ACCESS,FALSE,dwPID);

	if (NULL == hProcess)
		hProcess = OpenProcess( PROCESS_VM_READ|PROCESS_QUERY_INFORMATION,TRUE, dwPID);

	if(NULL == hProcess)
		hProcess = OpenProcess(PROCESS_QUERY_LIMITED_INFORMATION,FALSE, dwPID);
	
	if(hProcess == NULL)
		return FALSE;

	return TRUE;
}

BOOL GetProcessFullPath(DWORD dwPID, CString& strFullPath)
{
	TCHAR		szImagePath[MAX_PATH];
	HANDLE		hProcess = NULL;

	BOOL bSuccess = FALSE;
	do 
	{
		strFullPath.Empty();
		if (GetProcessHandle(dwPID,hProcess) == FALSE)
			break;

		if(GetProcessImageFileName(hProcess, szImagePath, MAX_PATH) == FALSE)
			break;

		if(DosPathToNtPath(szImagePath, strFullPath) == FALSE)
			break;

		bSuccess = TRUE;

	} while (FALSE);

	if (hProcess != NULL)
		CloseHandle(hProcess);

	return TRUE;
}
// 
// BOOL ConvertToDriveLetter(LPCTSTR lpszDeviceName,CString& strSysbolLnkName)
// {
// 	BOOL bSuccess = FALSE;
// 	do 
// 	{
// 		CStringArray strLogicalDrive;
// 		if (GetLogicalDriveStrings(strLogicalDrive) == FALSE)
// 		{
// 			SetErrorTitle(_T("获取计算机磁盘列表失败"));
// 			break;
// 		}
// 
// 		TCHAR szTargetPath[MAX_PATH];
// 		for (int i=0;i<strLogicalDrive.GetSize();++i)
// 		{
// 			if (QueryDosDevice(strLogicalDrive,szTargetPath,_countof(szTargetPath)))
// 			{
// 				if (_tcscmp(szTargetPath,lpszDeviceName) == 0)
// 				{
// 					strSysbolLnkName = strLogicalDrive;
// 					break;
// 				}
// 			}
// 		}
// 
// 		bSuccess = TRUE;
// 	} while (FALSE);
// 
// 	return bSuccess;
// }

BOOL DosPathToNtPath(LPCTSTR lpszDosPath, CString& strNtPath)
{
	TCHAR			szDriveStr[500];
	TCHAR			szDrive[3];
	TCHAR			szDevName[100];
	INT				cchDevName;
	INT				i;

	ASSERT_STR_NOT_NULL(lpszDosPath);
	//获取本地磁盘字符串
	if(GetLogicalDriveStrings(sizeof(szDriveStr), szDriveStr))
	{
		for(i = 0; szDriveStr[i]; i += 4)
		{
			if(!lstrcmpi(&(szDriveStr[i]), _T("A:\\")) || !lstrcmpi(&(szDriveStr[i]), _T("B:\\")))
				continue;

			szDrive[0] = szDriveStr[i];
			szDrive[1] = szDriveStr[i + 1];
			szDrive[2] = _T('\0');
			if(!QueryDosDevice(szDrive, szDevName, 100))//查询 Dos 设备名
				return FALSE;

			cchDevName = lstrlen(szDevName);
			if(_tcsnicmp(lpszDosPath, szDevName, cchDevName) == 0)//命中
			{
				strNtPath.Format(_T("%s%s"),szDrive,lpszDosPath+cchDevName);
				return TRUE;
			}
		}
	}
	strNtPath = lpszDosPath;
	return FALSE;
}

BOOL GetProcessUserName(DWORD dwProcessId,CString& strProcessName)
{
	BOOL bSuccess = FALSE;
	HANDLE hProcess = NULL;
	HANDLE hToken = NULL;
	PTOKEN_USER pTokenUser=NULL; 
	do 
	{
		if (GetProcessHandle(dwProcessId,hProcess) == FALSE)
		{
			SetErrorInfo(SYSTEM_ERROR,0,_T("打开进程%d句柄失败"),dwProcessId);
			break;
		}
		
		if (OpenProcessToken(hProcess,TOKEN_QUERY,&hToken) == FALSE)
		{
			SetErrorInfo(SYSTEM_ERROR,0,_T("获取进程%dToken查询权限失败"),dwProcessId);
			break;
		}

		TCHAR szUserName[256]={0}; 
		TCHAR szDomain[256]={0}; 
		DWORD dwDomainSize=256; 
		DWORD dwNameSize=256; 
		DWORD dwSize = 0;

		SID_NAME_USE    SNU; 
		
		BOOL bRet = GetTokenInformation(hToken,TokenUser,pTokenUser,dwSize,&dwSize);
		if (bRet == FALSE)
		{
			pTokenUser = (PTOKEN_USER)new BYTE[dwSize];
			if (pTokenUser == NULL)
			{
				SetErrorInfo(SYSTEM_ERROR,0,_T("获取进程Token信息分配内存失败"));
				break;
			}

			bRet = GetTokenInformation(hToken,TokenUser,pTokenUser,dwSize,&dwSize);
		}

		if (bRet != TRUE)
			break;
		
		if (LookupAccountSid(NULL,pTokenUser->User.Sid,szUserName,&dwNameSize,szDomain,&dwDomainSize,&SNU) == FALSE)
		{
			SetErrorInfo(SYSTEM_ERROR,0,_T("获取进程%d用户名失败"),dwProcessId);
			break;
		}
		strProcessName = szUserName;

		bSuccess = TRUE;
	} while (FALSE);

	if (pTokenUser != NULL)
	{	
		delete[] pTokenUser;
		pTokenUser = NULL;
	}

	if (hToken != NULL)
		CloseHandle(hToken);

	if (hProcess != NULL)
		CloseHandle(hProcess);
	return bSuccess;
}
//
//BOOL GetCpuUsage(PLS_SYSTEM_PROCESSES_INFO pLsSystemProcess,DWORD& dwCpuUsage)
//{	
//	BOOL bSuccess = FALSE;
//
//	do 
//	{
//		__int64 last_time_ = 0;
//		__int64 last_system_time_ = 0;
//		__int64 SystemTime = 0;
//		__int64 Runtime = 0;
//
//		__int64 SystemTimeIdel;
//		__int64 RuntimeIdel;
//
//		FILETIME FileTimeNow,KernelFileTime,UserFileTime;
//		static int ProcessorCount_  = 2;
//		GetSystemTimeAsFileTime(&FileTimeNow);
//
//		HANDLE hProcess = NULL;
//		GetProcessHandle(pLsSystemProcess->ProcessId,hProcess);
//		if (!GetProcessTimes(hProcess, NULL, NULL, &KernelFileTime, &UserFileTime))
//		{
//			return -1;
//		}
//
//		SystemTime = (FileTime2I64(&KernelFileTime) + FileTime2I64(&UserFileTime)) / ProcessorCount_;  
//
//		Runtime = FileTime2I64(&FileTimeNow);
//
//		if ((last_system_time_ == 0) || (last_time_ == 0))
//		{
//			last_system_time_ = SystemTime;
//			last_time_ = Runtime;
//			return -1;
//		}
//
//		SystemTimeIdel = SystemTime - last_system_time_;
//		RuntimeIdel = Runtime - last_time_;
//
//		if (RuntimeIdel == 0)
//			return -1;
//
//		dwCpuUsage = (int)((SystemTimeIdel * 100 + RuntimeIdel / 2) / RuntimeIdel);
//		last_system_time_ = SystemTime;
//		last_time_ = Runtime;
//
//		bSuccess = TRUE;
//	} while (FALSE);
//
//	return bSuccess;
//}
//
//ULONGLONG FileTime2I64(const FILETIME* pFileTime)
//{
//	LARGE_INTEGER li;
//
//	li.LowPart = pFileTime->dwLowDateTime;
//	li.HighPart = pFileTime->dwHighDateTime;
//
//	return li.QuadPart;
//}


BOOL TerminateProcessByProcessId(DWORD dwProcessId)
{
	HANDLE hProcess = INVALID_HANDLE_VALUE;
	BOOL bSuccess = FALSE;
	do 
	{
		if (GetProcessHandle(dwProcessId,hProcess) == FALSE)
		{
			SetErrorInfo(SYSTEM_ERROR,0,_T("打开进程%d失败"),dwProcessId);
			break;
		}

		if (TerminateProcess(hProcess,0) == FALSE)
		{
			SetErrorInfo(SYSTEM_ERROR,0,_T("终止进程%d失败"),dwProcessId);
			break;
		}

		bSuccess = TRUE;
	} while (FALSE);

	if (hProcess != INVALID_HANDLE_VALUE)
		CloseHandle(hProcess);

	return bSuccess;
}

BOOL GetProcessCommandLine(DWORD dwPid,CString& strCmdLine)
{
	PROCNTQSIP NtQueryInformationProcess;
	LONG                      status;
	HANDLE                    hProcess;
	PROCESS_BASIC_INFORMATION pbi;
	PEB                       Peb;
	PROCESS_PARAMETERS        ProcParam;
	DWORD                     dwDummy;
	DWORD                     dwSize;
	LPVOID                    lpAddress;
	BOOL                      bRet = FALSE;

	GetProcessHandle(dwPid,hProcess);
	if (hProcess == NULL)
		return FALSE;

	do 
	{
		NtQueryInformationProcess = (PROCNTQSIP)GetProcAddress(
			GetModuleHandle(_T("ntdll")),
			"NtQueryInformationProcess"
			);

		status = NtQueryInformationProcess( hProcess,
			ProcessBasicInformation,(PVOID)&pbi,
			sizeof(PROCESS_BASIC_INFORMATION),
			NULL);

		if (status)
			break;

		if (ReadProcessMemory( hProcess,
			pbi.PebBaseAddress,&Peb,
			sizeof(PEB),&dwDummy) == FALSE)
			break;

		if (ReadProcessMemory( hProcess,
			Peb.ProcessParameters,&ProcParam,
			sizeof(PROCESS_PARAMETERS),&dwDummy) == FALSE)
			break;

		lpAddress = ProcParam.CommandLine.Buffer;
		dwSize = ProcParam.CommandLine.Length;

		BYTE* bCmdLine = new BYTE[dwSize];
		if (bCmdLine == NULL)
		{
			SetErrorInfo(SYSTEM_ERROR,0,_T("获取进程的CMD命令行失败"));
			break;
		}

		BOOL 	bSuccess = FALSE;
		do 
		{
			bSuccess = ReadProcessMemory( hProcess,lpAddress,bCmdLine,dwSize,&dwDummy);
			if (bSuccess == FALSE)
				break;

			CStringW strCmdLineW((LPWSTR)bCmdLine,dwSize/sizeof(TCHAR));
			strCmdLine = strCmdLineW;

		} while (FALSE);
		
		delete[] bCmdLine;

		bRet = TRUE;
	} while (FALSE);

	if (hProcess)
		CloseHandle (hProcess);
	return bRet;
}


BOOL GetProcessCurrentDirctory(DWORD dwPid,CString& strProcessCurrentDirctory)
{
	PROCNTQSIP NtQueryInformationProcess;
	LONG                      status;
	HANDLE                    hProcess;
	PROCESS_BASIC_INFORMATION pbi;
	PEB                       Peb;
	PROCESS_PARAMETERS        ProcParam;
	DWORD                     dwDummy;
	DWORD                     dwSize;
	LPVOID                    lpAddress;
	BOOL                      bRet = FALSE;

	TCHAR szCurrentDirctory[MAX_PATH] = {0};
	GetProcessHandle(dwPid,hProcess);
	if (hProcess == NULL)
		return FALSE;

	do 
	{
		NtQueryInformationProcess = (PROCNTQSIP)GetProcAddress(
			GetModuleHandle(_T("ntdll")),
			"NtQueryInformationProcess"
			);

		status = NtQueryInformationProcess( hProcess,
			ProcessBasicInformation,(PVOID)&pbi,
			sizeof(PROCESS_BASIC_INFORMATION),
			NULL);

		if (status)
			break;

		if (ReadProcessMemory( hProcess,
			pbi.PebBaseAddress,&Peb,
			sizeof(PEB),&dwDummy) == FALSE)
			break;

		if (ReadProcessMemory( hProcess,
			Peb.ProcessParameters,&ProcParam,
			sizeof(PROCESS_PARAMETERS),&dwDummy) == FALSE)
			break;

		lpAddress = ProcParam.CurrentDirectory.Buffer;
		dwSize = ProcParam.CurrentDirectory.Length;

		if (_countof(szCurrentDirctory)<dwSize)
			break;

		if (ReadProcessMemory( hProcess,lpAddress,szCurrentDirctory,dwSize,&dwDummy)== FALSE)
			break;

		strProcessCurrentDirctory = szCurrentDirctory;
		bRet = TRUE;
	} while (FALSE);

	if (hProcess)
		CloseHandle (hProcess);
	return bRet;
}

BOOL GetParentProcessID(DWORD dwPid,DWORD& dwParentProcessID)
{
	PROCNTQSIP NtQueryInformationProcess;
	LONG                      status;
	HANDLE                    hProcess;
	PROCESS_BASIC_INFORMATION pbi;
	BOOL                      bRet = FALSE;

	if (dwPid == 0)
	{
		SetErrorInfo(CUSTOM_ERROR , 0 , _T("没有找到该进程的父进程！"));
		return FALSE;
	}

	GetProcessHandle(dwPid,hProcess);
	if (hProcess == NULL)
		return FALSE;

	do 
	{
		NtQueryInformationProcess = (PROCNTQSIP)GetProcAddress(
			GetModuleHandle(_T("ntdll")),
			"NtQueryInformationProcess"
			);

		status = NtQueryInformationProcess( hProcess,
			ProcessBasicInformation,(PVOID)&pbi,
			sizeof(PROCESS_BASIC_INFORMATION),
			NULL);

		if (status)
			break;

		dwParentProcessID = pbi.InheritedFromUniqueProcessId;
		bRet = TRUE;
	} while (FALSE);

	if (hProcess)
		CloseHandle (hProcess);

	return bRet;
}

BOOL GetPriorityClass(DWORD dwPid,CString& strPricrityDescribe)
{
	strPricrityDescribe.Empty();
	HANDLE hProcess;
	GetProcessHandle(dwPid,hProcess);
	if (hProcess == NULL)
		return FALSE;

	DWORD dwPriorityClass = GetPriorityClass(hProcess);
	if (dwPriorityClass == 0)
		return FALSE;

	switch(dwPriorityClass)
	{
	case REALTIME_PRIORITY_CLASS:
		strPricrityDescribe = _T("实时");
		break;
	case HIGH_PRIORITY_CLASS:
		strPricrityDescribe = _T("高");
		break;
	case ABOVE_NORMAL_PRIORITY_CLASS:
		strPricrityDescribe = _T("高于标准");
		break;
	case NORMAL_PRIORITY_CLASS:
		strPricrityDescribe = _T("普通");
		break;
	case BELOW_NORMAL_PRIORITY_CLASS:
		strPricrityDescribe = _T("低于标准");
		break;		
	case IDLE_PRIORITY_CLASS:
		strPricrityDescribe = _T("低");
		break;
	default:
		strPricrityDescribe = _T("未知");
	}

	CloseHandle(hProcess);
	return strPricrityDescribe.IsEmpty() == FALSE;
}


BOOL TerminateProcessTreeByProcessId(DWORD dwPid)
{
	return FALSE;
}

BOOL FreeRemoteLibrary(DWORD dwProcessID,LPCTSTR lpszModulePath)
{
	HANDLE hProcess = NULL;   
	hProcess = OpenProcess(PROCESS_ALL_ACCESS,FALSE,dwProcessID);   
	if (hProcess == NULL)
	{
		SetErrorInfo(SYSTEM_ERROR,0,_T("打开进程%d失败"),dwProcessID);
		return FALSE;
	}

	BOOL bSuccess = FALSE;
	LPVOID lpVirtuallBuf = NULL;
	DWORD dwSize, dwWritten;  
	do 
	{
		dwSize = (_tcslen(lpszModulePath)+1) * sizeof(TCHAR);   
		lpVirtuallBuf = VirtualAllocEx( hProcess,NULL,dwSize,MEM_COMMIT,PAGE_READWRITE );   
		if( NULL==lpVirtuallBuf )   
		{   
			SetErrorInfo(SYSTEM_ERROR,0,_T("卸载模块时,分配内存%d失败"),dwSize);
			break;
		}  

		if(WriteProcessMemory(hProcess,lpVirtuallBuf,(LPVOID)lpszModulePath,dwSize,&dwWritten) == FALSE)  
		{
			SetErrorInfo(SYSTEM_ERROR,0,_T("将模块路径写入进程%d空间失败"),dwProcessID);
			break;
		}

		if( dwWritten !=dwSize )   
		{   
			SetErrorInfo(SYSTEM_ERROR,0,_T("写入内容时,返回写入大小%d与写入内容大小%d不一致"),dwWritten,dwSize);
			break;
		}   

		DWORD dwHandle, dwID;   
		LPVOID pFunc = GetModuleHandle;   
		HANDLE hThread;   
		//使用GetModuleHandle在远程进程中创建线程，参数是模块地址
		hThread = CreateRemoteThread( hProcess,NULL,0,(LPTHREAD_START_ROUTINE)pFunc,lpVirtuallBuf,0,&dwID );   
		if (hThread == NULL)
		{
			SetErrorInfo(SYSTEM_ERROR,0,_T("在进程%d中创建线程失败"),dwProcessID);
			break;
		}

		WaitForSingleObject( hThread,INFINITE);   
		//通过线程返回值获取模块句柄
		GetExitCodeThread(hThread,&dwHandle);   
		//关闭远程线程
		CloseHandle(hThread);   

		pFunc = FreeLibrary;   
		hThread = CreateRemoteThread(hProcess,NULL,0,(LPTHREAD_START_ROUTINE)pFunc,    
			(LPVOID)dwHandle,0,&dwID);   

		WaitForSingleObject( hThread,INFINITE );   
		CloseHandle(hThread);   

		bSuccess = TRUE;
	} while (FALSE);
	
	if (lpVirtuallBuf != NULL)
		VirtualFreeEx(hProcess,lpVirtuallBuf,dwSize,MEM_DECOMMIT ); 

	if (hProcess != NULL)
		CloseHandle(hProcess);
	
	return true;   
}

DWORD GetThreadEntryPtr(DWORD dwThreadID)
{
	HANDLE hThread = NULL;
	DWORD dwStartAddr = NULL;
	do 
	{
		OpenThread(THREAD_QUERY_INFORMATION, FALSE, dwThreadID);
		if (hThread == NULL)
		{
			SetErrorInfo(SYSTEM_ERROR,0,_T("打开线程%d失败"),dwThreadID);
			break;
		}

		DWORD dwReturnLength = 0;
		if(NtQueryInformationThread(hThread, ThreadQuerySetWin32StartAddress,
			&dwStartAddr, sizeof(dwStartAddr), &dwReturnLength))
		{
			SetErrorInfo(SYSTEM_ERROR,0,_T("查询线程%d信息失败"),dwThreadID);
			break;
		}

	} while (FALSE);

	if (hThread != NULL)
		CloseHandle(hThread);

	return dwStartAddr;
}

BOOL GetFilePathByFileHandle(DWORD dwPID,HANDLE hFile,CString& strFilePath)
{
	BOOL bSuccess = FALSE;
	HANDLE hFileMap;

	do 
	{
		DWORD dwFileSizeHigh = 0;
		DWORD dwFileSizeLow = GetFileSize(hFile, &dwFileSizeHigh); 

		if( dwFileSizeLow == 0 && dwFileSizeHigh == 0 )
		{
			SetErrorInfo(SYSTEM_ERROR,0,_T("不能为空文件创建映像文件"));
			break;
		}

		hFileMap = CreateFileMapping(hFile, NULL, PAGE_READONLY,0, MAX_PATH,NULL);
		if (hFileMap == INVALID_HANDLE_VALUE)
		{
			SetErrorInfo(SYSTEM_ERROR,0,_T("创建文件映像失败"));
			break;
		}

		void* pMem = MapViewOfFile(hFileMap, FILE_MAP_READ, 0, 0, 1);

		if (pMem != NULL) 
		{
			TCHAR pszFilename[MAX_PATH+1];
			dwPID = dwPID == 0 ? GetCurrentProcessId() : dwPID;
			HANDLE hProcess = NULL;
			if (GetProcessHandle(dwPID,hProcess) == FALSE)
			{
				SetErrorTitle(_T("获取进程%d句柄失败"),dwPID);
				break;
			}

			if (GetMappedFileName (hProcess,pMem,pszFilename,_countof(pszFilename))) 
				bSuccess = DosPathToNtPath(pszFilename,strFilePath);
		} 
		
		if (pMem != NULL)
			UnmapViewOfFile(pMem);

	} while (FALSE);
	
	if (hFileMap != NULL)
		CloseHandle(hFileMap);	

	return bSuccess;
}

CString GetModuleAddressRange(MODULEINFO& ModuleInfo)
{
	CString strModuleAddressRange;
	strModuleAddressRange.Format(_T("0x%08X-0x%08X"),(DWORD*)ModuleInfo.lpBaseOfDll,(DWORD*)((LPBYTE)ModuleInfo.lpBaseOfDll+ModuleInfo.SizeOfImage));
	return strModuleAddressRange;
}


BOOL ModifyObjectSecurityToAccessAll(HANDLE hObject)
{
	DWORD dwLengthNeeded=0;PSECURITY_DESCRIPTOR pSecurityInfo=0;
	GetKernelObjectSecurity(hObject,DACL_SECURITY_INFORMATION,0,0,&dwLengthNeeded);
	if (dwLengthNeeded)
	{
		pSecurityInfo=(PSECURITY_DESCRIPTOR)new BYTE[dwLengthNeeded];
		GetKernelObjectSecurity(hObject,DACL_SECURITY_INFORMATION,pSecurityInfo,dwLengthNeeded,&dwLengthNeeded);

		BOOL bDaclPresent,bDaclDefaulted;PACL pDacl;
		GetSecurityDescriptorDacl(pSecurityInfo,&bDaclPresent,&pDacl,&bDaclDefaulted);

		TCHAR szUserName[MAX_PATH];DWORD dwBufferSize=MAX_PATH;
		GetUserName(szUserName,&dwBufferSize);

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
	}

	if (pSecurityInfo)
		delete[] pSecurityInfo;
	return TRUE;
}


BOOL IsSystemUserProcess()
{
	static BOOL bSystemUser = -1;
	if (bSystemUser != -1)
		return bSystemUser;

	bSystemUser = FALSE;
	TCHAR szUserName[MAX_PATH];
	DWORD dwUserNameLen = _countof(szUserName);
	if (GetUserName(szUserName , &dwUserNameLen))
	{
		if (_tcsicmp(szUserName , _T("system")) == 0)
			bSystemUser = TRUE;		
	}

	return bSystemUser;
}
