#pragma once

// ����CString ģ��δ��������
#pragma warning(disable : 4251)

class DRV_API CDrvObject
{
public:
	CDrvObject();
	~CDrvObject();
public:
	virtual BOOL StartDrvService();
	virtual BOOL StopDrvService();
	virtual BOOL DelDrvService();
	virtual LPCTSTR GetDrvFileName() = 0;
	virtual CString GetDrvFilePath();
public:
	BOOL OpenDrv();
	// ���ܳ����쳣���󣬱�����������ֹͣ��������������ô���������ﷵ��TRUE�����ǲ��ܺ���������ͨѶ
	BOOL IsOpen();
	void CloseDrv();
	BOOL DeviceIoControl(DWORD dwIoControlCode, LPVOID lpInBuffer, DWORD nInBufferSize, LPVOID lpOutBuffer = NULL , DWORD nOutBufferSize = 0 , LPDWORD lpBytesReturned = NULL);
protected:
	HANDLE		m_hDrv;
	CString			m_strLinkName;		// Ӧ�ò���ײ����������ӷ���
};