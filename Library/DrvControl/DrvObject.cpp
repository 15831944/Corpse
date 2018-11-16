#include "StdAfx.h"
#include "DrvObject.h"
#include "DllCore/Utils/ErrorInfo.h"

// Ĭ���������λ��
#define DRV_FILE_PATH		_T("\\SystemRoot\\system32\\drivers\\")

CDrvObject::CDrvObject()
{
	m_hDrv = INVALID_HANDLE_VALUE;
}

CDrvObject::~CDrvObject()
{
	CloseDrv();
}

BOOL CDrvObject::OpenDrv()
{
	if (m_hDrv != INVALID_HANDLE_VALUE)
		return TRUE;

	m_hDrv = CreateFile(m_strLinkName, GENERIC_READ|GENERIC_WRITE, 0, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
	if (m_hDrv == INVALID_HANDLE_VALUE)
		return FALSE;

	return TRUE;
}

BOOL CDrvObject::IsOpen()
{
	return m_hDrv != INVALID_HANDLE_VALUE;
}

void CDrvObject::CloseDrv()
{
	if (m_hDrv != INVALID_HANDLE_VALUE)
	{
		CloseHandle(m_hDrv);
		m_hDrv = INVALID_HANDLE_VALUE;
	}
}

//ͨ��DeviceIoControl����
BOOL CDrvObject::DeviceIoControl(DWORD dwIoControlCode , LPVOID lpInBuffer , DWORD nInBufferSize, LPVOID lpOutBuffer/* = NULL*/, DWORD nOutBufferSize/* = 0*/ , LPDWORD lpBytesReturned/* = NULL*/)
{
	DWORD dwBytesReturned = 0;
	if (lpBytesReturned == NULL)
		lpBytesReturned = &dwBytesReturned;

	if (OpenDrv() == FALSE)
	{
		SetErrorInfo(CUSTOM_ERROR,0,_T("���ÿ����뵽����ʧ�ܣ���ȷ�������ɹ��Ѿ���"));
		return FALSE;
	}

	if (::DeviceIoControl(m_hDrv , dwIoControlCode , lpInBuffer , nInBufferSize , lpOutBuffer , nOutBufferSize , lpBytesReturned , NULL) == FALSE)
	{
		SetErrorInfo(SYSTEM_ERROR , 0 , _T("��ײ�����ͨ��ʧ��"));
		return FALSE;
	}

	return TRUE;
}

BOOL CDrvObject::StartDrvService()
{
	return FALSE;
}

BOOL CDrvObject::StopDrvService()
{
	return FALSE;
}

BOOL CDrvObject::DelDrvService()
{
	return FALSE;
}

CString CDrvObject::GetDrvFilePath()
{
	CString strDrvFilePath;
	strDrvFilePath.Format(_T("%s%s"), DRV_FILE_PATH, GetDrvFileName());
	return strDrvFilePath;
}
