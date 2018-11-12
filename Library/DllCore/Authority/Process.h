#pragma once

#include "DllCore/DllCore.h"

#ifdef LS_STATIC_LIB_CALL
#define DLL_API 
#elif defined  DLL_EXPORTS
#define DLL_API __declspec(dllexport)
#else
#define DLL_API __declspec(dllimport)
#endif

// ģ�⵱ǰ�û�Ȩ��
#define  RunFunctionAsCurrentUser() \
	CImpersonateSessionUser ImpersionateSessionUser;	\
	ImpersionateSessionUser.Impersonate()

// ģ��System�û�Ȩ��
#define  RunFunctionAsSystemUser() \
	CImpersonateSystemUser ImpersonateSystemUser; \
	ImpersonateSystemUser.Impersonate()

// ģ��TrustedInstall�û�Ȩ��
#define RunFunctionAsTrustedInstallUser() \
	CImpersonateTrustedInstallUser ImpersonateTrustedInstallUser;	\
	ImpersonateTrustedInstallUser.Impersonate()

typedef struct  _tgaPROCESS_ITEM_INFO
{
	DWORD dwProcessId;
	CString strProcessPath;
	CString strProcessName;
	CString strUserName;
	DWORD dwMemUse;
	DWORD dwThread;
	DWORD dwHandle;
	DWORD dwConsole;
	__int64 ullProcessUseTime;		//�������õ�ϵͳʱ�䣬����CPUʹ����ʱ��
}PROCESS_ITEM_INFO,*PPROCESS_ITEM_INFO;

typedef CDuiList<PROCESS_ITEM_INFO, const PROCESS_ITEM_INFO&> CProcessItemInfoList;

/************************************************************************/
// �������ƣ�GetProcessUserName
// �������ܣ���ȡ�����û���
// ����������dwPID ����ID 
//					 strProcessName     �����û���
// ��  �� ֵ�����ִ�гɹ����򷵻�TRUE
/************************************************************************/
DLL_API BOOL GetProcessUserName(DWORD dwProcessId, CString& strProcessName);

/************************************************************************/
// �������ƣ�GetProcessCommandLine
// �������ܣ���ȡ���̵�������
// ����������dwPid ָ������
//					 strCmdLine     ָ�����̵�������
// ��  �� ֵ�����ִ�гɹ����򷵻�TRUE
/************************************************************************/
DLL_API BOOL GetProcessCommandLine(DWORD dwPid,CString& strCmdLine);

/************************************************************************/
// �������ƣ�IsRunAsAdmin
// �������ܣ��ж��Ƿ��Թ���ԱȨ������
// ������������
// ��  �� ֵ������Թ���ԱȨ������Ż�TRUE������Ż�FALSE
/************************************************************************/
DLL_API BOOL IsRunAsAdmin();

/************************************************************************/
// �������ƣ�IsRunAsSystem
// �������ܣ��ж��Ƿ��Է���Ȩ������
// ������������
// ��  �� ֵ������Է���Ȩ������Ż�TRUE������Ż�FALSE
/************************************************************************/
DLL_API BOOL IsRunAsSystem();

DLL_API BOOL ModifyObjectSecurityToAccessAll(HANDLE hObject);

//DLL_API BOOL GetProcessHandle(DWORD dwPID, HANDLE& hProcess);

DLL_API BOOL GetProcessFullPath(DWORD dwPID, CString& strFullPath);

DLL_API BOOL DosPathToNtPath(LPCTSTR lpszDosPath, CString& strNtPath);

DLL_API BOOL TerminateProcessByProcessId(DWORD dwProcessId);

DLL_API BOOL GetProcessCurrentDirctory(DWORD dwPid,CString& strProcessCurrentDirctory);

DLL_API BOOL GetParentProcessID(DWORD dwPid,DWORD& dwParentProcessID);

DLL_API BOOL GetPriorityClass(DWORD dwPid,CString& strPricrityDescribe);

/************************************************************************/
// �������ƣ�EnumProcessInfo
// �������ܣ�ö�ٲ���ϵͳ�еĽ���
// ����������ProcessItemInfoList �����б�ṹ��
// ��  �� ֵ�����ִ�гɹ����򷵻ض�Ӧ�ļ���ͼ����ImgList�е�����
/************************************************************************/
DLL_API BOOL EnumProcessInfo(CProcessItemInfoList& ProcessItemInfoList);