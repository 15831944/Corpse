#pragma once

#ifdef LS_STATIC_LIB_CALL
#define DLL_API 
#elif defined  DLL_EXPORTS
#define DLL_API __declspec(dllexport)
#else
#define DLL_API __declspec(dllimport)
#endif

typedef ULONG (WINAPI *PFNNtUnmapViewOfSection)(IN HANDLE ProcessHandle, IN PVOID BaseAddress);

/************************************************************************/
/* �������ƣ�EnablePrivilege	                                                                  */
/* �������ã���������Ȩ��				     													   */
/* ���������lpszPrivilegeName Ȩ��������   ��:SE_DEBUG_NAME		   */
/*					 bEnable �Ƿ�ʹ����Ȩ															   */
/*	�� �� ֵ���ɹ�����TRUE��ʧ�ܷ���FALSE								               */
/************************************************************************/
DLL_API BOOL EnablePrivilege(LPCTSTR lpszPrivilegeName, BOOL bEnable = TRUE);

/************************************************************************/
/* �������ƣ�InjectProcess                                                                         */
/* �������ã����ļ�����dllע��Զ�̽���													   */
/* ���������lpszFilePath �ļ�·��															   */
/*					 dwRemoteProcessId ����ID	         									   */
/*					dwWaitTime �ȴ�ʱ�䣬-1Ϊһֱ�ȴ���ֱ����Ӧ����λ����  */
/*	�� �� ֵ���ɹ�����TRUE��ʧ�ܷ���FALSE								              */
/************************************************************************/
//DLL_API BOOL InjectProcess(LPCTSTR lpszFilePath, DWORD dwRemoteProcessId, DWORD dwWaitTime);

/************************************************************************/
/* �������ƣ�UnLoadViewOfModule                                                         */
/* �������ã�ж��Զ�̽���ָ��ģ��      													   */
/* ���������dwProcessId Զ�̽���ID														   */
/*					 lpBaseAddr ģ�����ַ			         									   */
/*	�� �� ֵ���ɹ�����TRUE��ʧ�ܷ���FALSE								              */
/************************************************************************/
DLL_API BOOL UnLoadViewOfModule(DWORD dwProcessId, LPVOID lpBaseAddr);

DLL_API BOOL ModifyObjectSecurityToAccessAll(HANDLE hObject);
/************************************************************************/
/* �������ƣ�ModifyAccessToEveryOne                                                   */
/* �������ã���ע�����ķ���Ȩ���޸�Ϊ�κ���										   */
/* ���������lpszRegPath ��Ҫ�޸ĵ�ע�����·��									   */
/* ����������CLASSES_ROOT\SomePath												   */
/*					 \\ComputerName\CLASSES_ROOT\SomePath.					   */
/*	�� �� ֵ���ɹ�����TRUE��ʧ�ܷ���FALSE								               */
/************************************************************************/
DLL_API BOOL ModifyAccessToEveryOne(LPCTSTR lpszRegPath);