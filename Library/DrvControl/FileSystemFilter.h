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
	virtual BOOL DeviceIoControl(LPVOID lpInBuffer, DWORD nInBufferSize, LPVOID lpOutBuffer = NULL, DWORD nOutBufferSize = 0, LPDWORD lpBytesReturned = NULL);
public:

	// ��ȡ�Ƿ���������USB�洢�豸�ϵĳ��򼰽ű�
	BOOL GetEnableUsbDiskExecute();
	// �����Ƿ���������USB�洢�豸�ϵĳ��򼰽ű�
	BOOL SetEnableUsbDiskExecute(BOOL bEnable);
	// ��ѯ���������Ƿ����ֹͣ
	BOOL GetCanStopFsDrv();
	// ���ù��������Ƿ����ֹͣ
	BOOL SetFsFltFilterCanStop(BOOL bCanStop /*= TRUE*/);
	// ��ȡ���������汾
	DWORD GetFsFltVersion();
	// �жϹ��������Ƿ��������汾
	BOOL FsFltVersionCorrectly();
public:	
	BOOL InstallMiniFilter();
	BOOL CreateFsFilterSvc();
	//��ʵ������Ϣ����·�����������ͣ���������
	BOOL VerifyFsFilterSvcInfo();
};