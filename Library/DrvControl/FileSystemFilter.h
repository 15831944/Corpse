#pragma once

class DRV_API CFileSystemFilter : public CDrvObject
{
public:
	CFileSystemFilter();
	~CFileSystemFilter();

public:
	BOOL OpenDrv();
	BOOL StartDrvService();
	BOOL StopDrvService();
	BOOL DelDrvService();
	virtual LPCTSTR GetDrvFileName();
	virtual BOOL DeviceIoControl(LPVOID lpInBuffer , DWORD nInBufferSize , LPVOID lpOutBuffer /* = NULL  */, DWORD nOutBufferSize /* = 0  */, LPDWORD lpBytesReturned /* = NULL */);
public:	
	BOOL InstallMiniFilter();
	BOOL CreateFsFilterSvc();
	//��ʵ������Ϣ����·�����������ͣ���������
	BOOL VerifyFsFilterSvcInfo();
	HRESULT UnloadFilterNotMandatory(LPCTSTR lpszFilterName);
};