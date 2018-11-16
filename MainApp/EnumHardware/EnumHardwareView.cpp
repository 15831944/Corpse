
// EnumHardwareView.cpp : CEnumHardwareView ���ʵ��
//

#include "stdafx.h"
#include "EnumHardware.h"
#include <InitGuid.h>
#include "EnumHardwareDoc.h"
#include "EnumHardwareView.h"
#include "LsCommon/OsInfo.h"
#include "LsCommon/ErrorInfo.h"
#include <SetupAPI.h>
#include "MainFrm.h"
#include <devguid.h>
#include <Uxtheme.h>
#pragma comment(lib,"uxtheme.lib")
#pragma comment(lib,"setupapi.lib")
#include "LsCommon/File/LogicalDiskMgr.h"
#include "LsCommon/File/DiskVolumeInfo.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// CEnumHardwareView
#define	WM_SHOW_HARDWARE		(WM_USER + 0x100)
#define INTERFACE_DETAIL_SIZE	8096

IMPLEMENT_DYNCREATE(CEnumHardwareView, CTreeView)
	
BEGIN_MESSAGE_MAP(CEnumHardwareView, CTreeView)
	ON_MESSAGE(WM_SHOW_HARDWARE,&CEnumHardwareView::OnShowHardware)
	ON_NOTIFY_REFLECT(NM_RCLICK,&CEnumHardwareView::OnRClick)
	ON_COMMAND(ID_SHOWALL, &CEnumHardwareView::OnShowall)
	ON_COMMAND(ID_DONOT_SHOW_HIDE, &CEnumHardwareView::OnDonotShowHide)
	ON_COMMAND(ID_HARDWARE_SCANF, &CEnumHardwareView::OnHardwareScanf)
	ON_COMMAND(ID_HARDWARE_PROPERTY, &CEnumHardwareView::OnHardwareProperty)
	ON_COMMAND(ID_HARDWARE_DISABLE, &CEnumHardwareView::OnHardwareDisable)
	ON_COMMAND(ID_HARDWARE_ENABLE, &CEnumHardwareView::OnHardwareEnable)
END_MESSAGE_MAP()

// CEnumHardwareView ����/����

CEnumHardwareView::CEnumHardwareView()
{
	// TODO: �ڴ˴���ӹ������
	m_bShowAll = TRUE;
	m_ImgList.DeleteImageList();
	m_ImgList.Create(16,16,ILC_COLOR32|ILC_MASK,10,1);
}

CEnumHardwareView::~CEnumHardwareView()
{
}

BOOL CEnumHardwareView::PreCreateWindow(CREATESTRUCT& cs)
{
	// TODO: �ڴ˴�ͨ���޸�
	//  CREATESTRUCT cs ���޸Ĵ��������ʽ
	cs.style |= TVS_HASBUTTONS|TVS_HASLINES|TVS_LINESATROOT;
	return CTreeView::PreCreateWindow(cs);
}

void CEnumHardwareView::OnInitialUpdate()
{
	CTreeView::OnInitialUpdate();
	SetWindowTheme(m_hWnd,L"Explorer",NULL);

	CTreeCtrl& TreeCtrl = GetTreeCtrl();
	TreeCtrl.SetImageList(&m_ImgList,TVSIL_NORMAL);
	//m_DevInfoMap._GetBaseClass//

	UINT nIDIcon[] = {IDI_ICON_COMPUTER,IDI_ICON_DVD,IDI_ICON_IDE,IDI_ICON_CPU,IDI_ICON_DISK,IDI_ICON_DISK,IDI_ICON_DISK,
		IDI_ICON_PORT,IDI_ICON_HIDE,IDI_ICON_SYSTEM,IDI_ICON_MONITOR,IDI_ICON_KEYBORD,IDI_ICON_BODY,
		IDI_ICON_SOUND,IDI_ICON_MOUSE,IDI_ICON_USB,IDI_ICON_NETWORK,IDI_ICON_SYSTEM,
		IDI_ICON_DISPLAY,IDI_ICON_SAFEREADER,IDI_ICON_SAFEREADER,IDI_ICON_HIDE};

	LPCTSTR lpszDevName[] = {_T("DVD/CD-ROM������"),_T("IDE ATA/ATAPI ������"),_T("������"),_T("��Я�豸"),_T("����������"),
								_T("���̾�"),_T("�˿�(COM��LPT)"),_T("�Ǽ��弴����������"),_T("�����"),_T("������"),_T("����"),_T("����ѧ�����豸"),_T("��������������Ϸ������"),
								_T("��������ָ���豸"),_T("ͨ�ô������߿�����"),_T("����������"),_T("ϵͳ�豸"),_T("��ʾ�豸��"),
								_T("���ܿ�������"),_T("���ܿ�ɸѡ��"),_T("δ֪�豸")};

	GUID DevGuid[] = {GUID_DEVCLASS_CDROM,GUID_DEVCLASS_HDC,GUID_DEVCLASS_PROCESSOR,GUID_DEVCLASS_WPD,GUID_DEVCLASS_DISKDRIVE,
		GUID_DEVCLASS_VOLUME,GUID_DEVCLASS_PORTS,GUID_DEVCLASS_LEGACYDRIVER,GUID_DEVCLASS_COMPUTER,GUID_DEVCLASS_MONITOR,
		GUID_DEVCLASS_KEYBOARD,GUID_DEVCLASS_HIDCLASS,GUID_DEVCLASS_MEDIA,GUID_DEVCLASS_MOUSE,GUID_DEVCLASS_USB,GUID_DEVCLASS_NET,
		GUID_DEVCLASS_SYSTEM,GUID_DEVCLASS_DISPLAY,GUID_DEVCLASS_SMARTCARDREADER,GUID_DEVCLASS_SMARTCARDFILTER,GUID_DEVCLASS_NODRIVER};


	//for (int i=0;i<_countof(nIDIcon);++i)
	m_ImgList.Add(LoadIcon(AfxGetInstanceHandle(),MAKEINTRESOURCE(nIDIcon[0])));

	m_DevNameList.RemoveAll();
	DEVNAME DevName;
	for (int i=0;i<_countof(lpszDevName);++i)
	{
		DevName.strDevName = lpszDevName[i];
		DevName.DevGuid = DevGuid[i];
		DevName.DevIcon = m_ImgList.Add(LoadIcon(AfxGetInstanceHandle(),MAKEINTRESOURCE(nIDIcon[i+1])));
		m_DevNameList.AddTail(DevName);
	}

	PostMessage(WM_SHOW_HARDWARE,0,0);
}


// CEnumHardwareView ���

#ifdef _DEBUG
void CEnumHardwareView::AssertValid() const
{
	CTreeView::AssertValid();
}

void CEnumHardwareView::Dump(CDumpContext& dc) const
{
	CTreeView::Dump(dc);
}

CEnumHardwareDoc* CEnumHardwareView::GetDocument() const // �ǵ��԰汾��������
{
	ASSERT(m_pDocument->IsKindOf(RUNTIME_CLASS(CEnumHardwareDoc)));
	return (CEnumHardwareDoc*)m_pDocument;
}
#endif //_DEBUG


// CEnumHardwareView ��Ϣ�������
LRESULT CEnumHardwareView::OnShowHardware(WPARAM wParam,LPARAM lParam)
{
	m_DevInfoList.RemoveAll();
	CTreeCtrl& TreeCtrl = GetTreeCtrl();
	TreeCtrl.DeleteAllItems();

	HTREEITEM hRoot = TreeCtrl.InsertItem(GetComputerName(),0);
	
	POSITION pos = m_DevNameList.GetHeadPosition();
	
	CDevInfoList DevInfoList;
	while(pos)
	{
		DevInfoList.RemoveAll();
		DEVNAME& DevName = m_DevNameList.GetNext(pos);
		if (GetDevInfo(DevName.DevGuid,DevInfoList,m_bShowAll) != FALSE)
		{
			if (DevInfoList.IsEmpty() != TRUE)
			{
				HTREEITEM hItem = TreeCtrl.InsertItem(TVIF_TEXT|TVIF_IMAGE|TVIF_SELECTEDIMAGE|TVIF_PARAM,
							DevName.strDevName,DevName.DevIcon,DevName.DevIcon,0,0,-1,hRoot,TVI_LAST);

				POSITION pos = DevInfoList.GetHeadPosition();
				while(pos)
				{
					DEV_INFO& DevInfo = DevInfoList.GetNext(pos);
					POSITION CurrentPos = m_DevInfoList.AddTail(DevInfo);

					CString strDevName;
					strDevName.Format(_T("%s[%d]"), DevInfo.strDevName, DevInfo.DevInst);
					if (strDevName.IsEmpty())
						strDevName = DevInfo.strDevDesc;

					TreeCtrl.InsertItem(TVIF_TEXT|TVIF_IMAGE|TVIF_SELECTEDIMAGE|TVIF_PARAM,strDevName,
													DevName.DevIcon,DevName.DevIcon,0,0,(LPARAM)CurrentPos,hItem,TVI_SORT);
				}
			}
		}
	}

	TreeCtrl.SelectItem(hRoot);
	return TreeCtrl.Expand(hRoot,TVE_EXPAND);
}

void CEnumHardwareView::OnRClick(NMHDR* pNMHdr,LPARAM* lResult)
{
	NMTREEVIEW* pTreeView = (NMTREEVIEW*)pNMHdr;
	
	CMenu Menu,*pRClickMenu;
	Menu.LoadMenu(IDR_MENU_TREEVIEW);
	pRClickMenu = Menu.GetSubMenu(0);

	CTreeCtrl& TreeCtrl = GetTreeCtrl();
	CPoint pt;
	GetCursorPos(&pt);
	TreeCtrl.ScreenToClient(&pt);
	UINT uFlag;
	HTREEITEM ht = TreeCtrl.HitTest(pt,&uFlag);

	if (ht == NULL)
		return;

	if (TreeCtrl.ItemHasChildren(ht))
	{
		pRClickMenu->DeleteMenu(ID_HARDWARE_UPDATE_DRIVERS,MF_BYCOMMAND);

		pRClickMenu->DeleteMenu(ID_HARDWARE_DISABLE,MF_BYCOMMAND);

		pRClickMenu->DeleteMenu(ID_HARDWARE_ENABLE,MF_BYCOMMAND);

		pRClickMenu->DeleteMenu(ID_HARDWARE_UNISTALL,MF_BYCOMMAND);
		pRClickMenu->DeleteMenu(ID_HARDWARE_PROPERTY,MF_BYCOMMAND);
		pRClickMenu->DeleteMenu(0,MF_BYPOSITION);
	}
	else
	{
		POSITION pos = (POSITION)TreeCtrl.GetItemData(ht);
		DEV_INFO& DevInfo = m_DevInfoList.GetAt(pos);

		if (DevInfo.dwStatus == DEV_DISABLE)
			pRClickMenu->DeleteMenu(ID_HARDWARE_DISABLE,MF_BYCOMMAND);
		else
			pRClickMenu->DeleteMenu(ID_HARDWARE_ENABLE,MF_BYCOMMAND);
	}
	SetForegroundWindow();

	GetCursorPos(&pt);
	pRClickMenu->TrackPopupMenu(TPM_LEFTALIGN,pt.x,pt.y,this);
}

void CEnumHardwareView::OnShowall()
{
	// TODO: �ڴ���������������
	m_bShowAll = TRUE;
	PostMessage(WM_SHOW_HARDWARE,0,0);
}

void CEnumHardwareView::OnDonotShowHide()
{
	// TODO: �ڴ���������������
	m_bShowAll = FALSE;
	PostMessage(WM_SHOW_HARDWARE,0,0);
}

void CEnumHardwareView::OnHardwareScanf()
{
	// TODO: �ڴ���������������
	CTreeCtrl& TreeCtrl = GetTreeCtrl();
	HTREEITEM ht = TreeCtrl.GetSelectedItem();
	if (ht == NULL)
		return;

	ScanfHardwareInfo();
}

void CEnumHardwareView::OnHardwareProperty()
{
	// TODO: �ڴ���������������
	//ShellExecute(m_hWnd,)
}

void CEnumHardwareView::OnHardwareEnable()
{
	CTreeCtrl& TreeCtrl = GetTreeCtrl();
	HTREEITEM ht = TreeCtrl.GetSelectedItem();
	if (ht == NULL)
		return;

	POSITION pos = (POSITION)TreeCtrl.GetItemData(ht);
	DEV_INFO& DevInfo = m_DevInfoList.GetAt(pos);

	OUTER_DEV_MANAGER DeviceRuleManage;
	DeviceRuleManage.DevChangeGuid = GUID_DEVINTERFACE_DISK;
	DeviceRuleManage.DevClassGuid = DevInfo.ClassGuid;
	DeviceRuleManage.dwOperator = DEV_OP_ENABLE;
	DeviceRuleManage.dwSubClass = 0;
	DeviceRuleManage.strDevClassDisplayName = DevInfo.strDisplayName;
	DeviceRuleManage.strDevClassName = DevInfo.strClassName;

	SetDeviceStatus(DeviceRuleManage);
}

void CEnumHardwareView::OnHardwareDisable()
{
	CTreeCtrl& TreeCtrl = GetTreeCtrl();
	HTREEITEM ht = TreeCtrl.GetSelectedItem();
	if (ht == NULL)
		return;

	POSITION pos = (POSITION)TreeCtrl.GetItemData(ht);
	DEV_INFO& DevInfo = m_DevInfoList.GetAt(pos);

	OUTER_DEV_MANAGER DeviceRuleManage;
	DeviceRuleManage.DevChangeGuid = GUID_DEVINTERFACE_DISK;
	DeviceRuleManage.DevClassGuid = DevInfo.ClassGuid;
	DeviceRuleManage.dwOperator = DEV_OP_DISABLE;
	DeviceRuleManage.dwSubClass = 0;
	DeviceRuleManage.strDevClassDisplayName = DevInfo.strDisplayName;
	DeviceRuleManage.strDevClassName = DevInfo.strClassName;

	SetDeviceStatus(DeviceRuleManage);
}

BOOL CEnumHardwareView::SetDeviceStatus(OUTER_DEV_MANAGER& DeviceRuleManage)
{
	BOOL bSuccess = FALSE;
	do 
	{
		CDevInfoList DevInfoList; //�豸״̬����

		//�������Ҫ���ø����µ��豸����Ҫö�������豸
		BOOL bAllDevice = DeviceRuleManage.dwOperator == DEV_OP_ENABLE;

		//��ȡָ�������µĵ��豸
		if (GetDevInfoFromClassGuid(DeviceRuleManage.DevClassGuid, DevInfoList, bAllDevice) == FALSE)
			break;

		//��¼�豸״̬
		//WriteOutDevInfo(DeviceRuleManage);

		POSITION pos = DevInfoList.GetHeadPosition();
		while(pos)
		{
			const DEV_INFO& DeviceInfo = DevInfoList.GetNext(pos);

			BOOL bChangeRet = FALSE;

			//�����Ӳ���豸
			if (DeviceInfo.ClassGuid == GUID_DEVCLASS_DISKDRIVE)
			{
				//�����豸״̬����ֵ�������ֵ����Ӧ����Ҫ������Ӧ�Ĵ������ֵ�Ķ���
				if (DeviceInfo.dwStatus == DeviceRuleManage.dwOperator - 1 ||
					DeviceInfo.dwStatus == DEV_NOT_EXIST)
					continue;

				//������豸��Ҫ���ã����жϸ��豸�Ƿ�����Ҫ���õ��豸
				if (DeviceRuleManage.dwOperator != DEV_OP_ENABLE)
				{
					//��ȡ��ǰϵͳ����Ӳ�����
					CLogicalDiskMgr LogicalDisk;
					DWORD dwIndex = LogicalDisk.GetSystemDiskIndex();
					DWORD dwPhysicalItem = GetPhysicalDiskIndex(DeviceInfo);
					if (dwIndex == dwPhysicalItem || dwPhysicalItem == -1)
						continue;

					//����IDE�����豸��STAT�����豸
					STORAGE_BUS_TYPE StorageBusType;
					BOOL bRet = CLogicalDiskMgr::GetStorageDeviceBusType(dwPhysicalItem, StorageBusType);
					if (bRet != FALSE && StorageBusType != BusTypeUnknown)
					{
						if (StorageBusType == BusTypeSata || StorageBusType == BusTypeAta)	//IDE (ATA) Bus
							continue;
					}

					//ö�ٵ�ǰ�ƶ��洢�豸�����о��豸������������Ȼ��ж���豸
					CDiskVolumeInfo DiskVolume;
					CStringArray strVolumeArray;

					LS_PHYSICAL_DISK_SIZE LsPhysicalDisk;
					if (CLogicalDiskMgr::GetPhysicalDiskSize(dwPhysicalItem, LsPhysicalDisk) == FALSE)
						continue;

					LARGE_INTEGER DiskSize;
					DiskSize.QuadPart = LsPhysicalDisk.DiskSectors.QuadPart*LsPhysicalDisk.dwBytesPerSector;

					if (DiskVolume.EnumVolumeForDisk(dwPhysicalItem, 0, DiskSize.QuadPart, strVolumeArray) != FALSE)
					{
						int nIndex = 0;
						DWORD dwVolumeCount = (DWORD)strVolumeArray.GetCount();
						CString strVolumeName;
						for(DWORD i=0;i<dwVolumeCount;i++)
						{
							CLogicalDiskMgr LogicalDisk;
							strVolumeName.Format(_T("%s"), strVolumeArray.GetAt(i));
							DefineDosDevice(DDD_REMOVE_DEFINITION, strVolumeName.TrimRight(_T("\\")),NULL);
						}
					}
				}				

				//DevStatusChange(DeviceInfo.ClassGuid, DeviceInfo.strDevInstId, DeviceRuleManage.dwOperator);
				//����ɾ�����豸�ķ������ӣ���Ȼ�����᷵��ʧ�ܣ����ǹ����Ѿ���Ч
				//SaveDeviceResult(DeviceRuleManage,DeviceInfo, TRUE);
			}
		}

		bSuccess = TRUE;
	} while (FALSE);

	return bSuccess;
}

DWORD CEnumHardwareView::GetPhysicalDiskIndex(const DEV_INFO& DeviceInfo)
{
	DWORD dwDiskIndex = -1;
	HDEVINFO hDevInfoSet = INVALID_HANDLE_VALUE;
	HANDLE hPhysicalDisk = INVALID_HANDLE_VALUE;

	do 
	{
		hDevInfoSet = SetupDiGetClassDevs(&GUID_DEVINTERFACE_DISK,	// class GUID 
			NULL,										// �޹ؼ���
			NULL,										// ��ָ�������ھ��
			DIGCF_PRESENT | DIGCF_DEVICEINTERFACE);	// Ŀǰ���ڵ��豸

		if (hDevInfoSet == INVALID_HANDLE_VALUE)
		{
			SetErrorInfo(SYSTEM_ERROR, 0, _T("��ȡϵͳ�����д���ʧ��"));
			break;
		}

		SP_DEVICE_INTERFACE_DATA DeviceInterfaceData;
		ZeroMemory(&DeviceInterfaceData, sizeof(SP_DEVICE_INTERFACE_DATA));
		DeviceInterfaceData.cbSize = sizeof(SP_DEVICE_INTERFACE_DATA);		

		for(int n=0; n<0xff ;++n)
		{
			// ö�ٷ��ϸ�GUID���豸�ӿ�
			BOOL bRet = SetupDiEnumDeviceInterfaces(hDevInfoSet,NULL,&GUID_DEVINTERFACE_DISK,n,&DeviceInterfaceData);
			if (bRet == FALSE)
				break;

			SP_DEVINFO_DATA DeviceInfoData = {sizeof(SP_DEVINFO_DATA)}; //�豸��Ϣ�ṹ��

			bRet = SetupDiEnumDeviceInfo(hDevInfoSet, n ,&DeviceInfoData);
			if (bRet == FALSE)
				break;

			//�ж��Ƿ�Ϊ��ǰ�豸
			if (DeviceInfoData.DevInst != DeviceInfo.DevInst)
				continue;

			BYTE byDetailBuffer[INTERFACE_DETAIL_SIZE];
			ZeroMemory(byDetailBuffer, INTERFACE_DETAIL_SIZE);
			PSP_DEVICE_INTERFACE_DETAIL_DATA pDeviceInterfaceDetailData = (PSP_DEVICE_INTERFACE_DETAIL_DATA)byDetailBuffer;
			pDeviceInterfaceDetailData->cbSize=sizeof(SP_DEVICE_INTERFACE_DETAIL_DATA);

			// ȡ�ø��豸�ӿڵ�ϸ��(�豸·��)
			if (SetupDiGetInterfaceDeviceDetail(hDevInfoSet,									// �豸��Ϣ�����
				&DeviceInterfaceData,					// �豸�ӿ���Ϣ
				pDeviceInterfaceDetailData,			// �豸�ӿ�ϸ��(�豸·��)
				INTERFACE_DETAIL_SIZE,				// �����������С
				NULL,												// ������������������С(ֱ�����趨ֵ)
				&DeviceInfoData) == FALSE)				// ���������豸����								
				break;

			hPhysicalDisk = CreateFile(pDeviceInterfaceDetailData->DevicePath, GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, 0 ,NULL);
			if (hPhysicalDisk == INVALID_HANDLE_VALUE)
				break;

			//��ȡ��ǰ�����豸Ӳ�����
			STORAGE_DEVICE_NUMBER StorageDeviceNumber;
			DWORD dwSize = sizeof(STORAGE_DEVICE_NUMBER);
			bRet = DeviceIoControl(hPhysicalDisk, IOCTL_STORAGE_GET_DEVICE_NUMBER, 
				NULL, NULL, &StorageDeviceNumber, dwSize, &dwSize ,NULL);

			if (bRet == FALSE)
				break;

			if (StorageDeviceNumber.DeviceNumber != -1)
			{
				dwDiskIndex = StorageDeviceNumber.DeviceNumber;
				break;
			}
		}

	} while (FALSE);

	if (hPhysicalDisk != INVALID_HANDLE_VALUE)
	{
		CloseHandle(hPhysicalDisk);
		hPhysicalDisk = INVALID_HANDLE_VALUE;
	}

	if (hDevInfoSet != INVALID_HANDLE_VALUE)
	{
		SetupDiDestroyDeviceInfoList(hDevInfoSet);
		hDevInfoSet = INVALID_HANDLE_VALUE;
	}

	return dwDiskIndex;
}

BOOL CEnumHardwareView::GetDevInfoFromClassGuid(IN GUID DevClassGuid, OUT CDevInfoList& DevInfoList,BOOL bAllDevice)
{
	BOOL bRet = TRUE;

	DWORD dwFlags = 0;
	if(bAllDevice == FALSE)
		dwFlags = DIGCF_PRESENT | DIGCF_PROFILE; //Ĭ�ϱ�־��

	BOOL bHiddenClass = IsClassHidden(DevClassGuid); //�ж��Ƿ����ص��豸��

	HDEVINFO hDev = SetupDiGetClassDevs(&DevClassGuid,NULL,NULL,dwFlags);
	if(INVALID_HANDLE_VALUE == hDev)
	{
		CString strTipInfo;
		strTipInfo.Format(_T("��ȡ�豸��Ϣʱ��ȡӲ���б���Ϣʧ��,������:%d."),GetLastError());
		OutputDebugString(strTipInfo);
		return FALSE;
	}

	DWORD dwError = 0;
	DWORD dwRequiredSize = 0;

	TCHAR pDataBuffer[10*4*MAX_PATH];
	DWORD dwSize = _countof(pDataBuffer);
	SP_DEVINFO_DATA DeviceInfoData = {sizeof(SP_DEVINFO_DATA)}; //�豸��Ϣ�ṹ��

	for(int i=0; SetupDiEnumDeviceInfo(hDev,i,&DeviceInfoData); ++i)
	{
		DEV_INFO DevInfo;

		LPBYTE pDevClassGuid = NULL;
		DevInfo.DevInst = DeviceInfoData.DevInst;
		DevInfo.ClassGuid = DeviceInfoData.ClassGuid;
		DevInfo.dwBluetoothFlag = 0;
		if(DevInfo.ClassGuid==GUID_DEVCLASS_BLUETOOTH)
			DevInfo.dwBluetoothFlag = MICROSOFT_BLUETOOTH;

		DEVINST ParentInst;
		TCHAR szParentInstId[MAX_DEVICE_ID_LEN] = {0};
		if (CR_SUCCESS != (dwError = CM_Get_Parent(&ParentInst,DevInfo.DevInst,0)))
		{
			dwError = GetLastError();
			CString strTipInfo;
			strTipInfo.Format(_T("��ȡ�豸��Ϣʱ��ȡ�豸���ڵ����,�����%d"),dwError);
			OutputDebugString(strTipInfo);
			continue;
		}

		if(CR_SUCCESS != (dwError = CM_Get_Device_ID(ParentInst,szParentInstId,MAX_DEVICE_ID_LEN,0)))
		{
			dwError = GetLastError();
			CString strTipInfo;
			strTipInfo.Format(_T("��ȡ�豸��Ϣʱ��ȡ�豸���ڵ�ʵ��ID����,�����%d"),dwError);
			OutputDebugString(strTipInfo);
			continue;
		}

		DevInfo.strParentInstId = szParentInstId;

		if (FALSE == SetupDiClassNameFromGuid(&DevInfo.ClassGuid,pDataBuffer,dwSize,&dwRequiredSize))
		{
			dwError = GetLastError();
			CString strTipInfo;
			strTipInfo.Format(_T("��ȡ�豸��Ϣʱ��ȡ�豸���ڵ����,�����%d"),dwError);
			OutputDebugString(strTipInfo);
			continue;
		}

		DevInfo.strClassName = pDataBuffer;

		if (FALSE == SetupDiGetDeviceInstanceId(hDev,&DeviceInfoData,pDataBuffer,dwSize,&dwRequiredSize))
		{
			dwError = GetLastError();
			CString strTipInfo;
			strTipInfo.Format(_T("��ȡ�豸��Ϣʱ��ȡ�豸ʵ��ID����,�����%d"),dwError);
			OutputDebugString(strTipInfo);
			continue;
		}

		DevInfo.strDevInstId = pDataBuffer;

		if (FALSE == SetupDiGetDeviceRegistryProperty(hDev,&DeviceInfoData,SPDRP_ENUMERATOR_NAME,NULL,(LPBYTE)pDataBuffer,dwSize,&dwRequiredSize))
		{
			dwError = GetLastError();
			CString strTipInfo;
			strTipInfo.Format(_T("��ȡ�豸��Ϣʱ��ȡ�豸�������ͳ���,�����%d"),dwError);
			OutputDebugString(strTipInfo);
		}
		DevInfo.strDevEnumerator = pDataBuffer;

		if (FALSE == SetupDiGetDeviceRegistryProperty(hDev,&DeviceInfoData,SPDRP_DEVICEDESC,NULL,(LPBYTE)pDataBuffer,dwSize,&dwRequiredSize)) //�豸����
		{
			dwError = GetLastError();
			CString strTipInfo;
			strTipInfo.Format(_T("��ȡ�豸��Ϣʱ��ȡ�豸��������,�����%d"),dwError);
			OutputDebugString(strTipInfo);
		}

		DWORD dwDescLen = (DWORD)_tcslen(pDataBuffer);
		if (dwDescLen)
			DevInfo.strDevDesc = pDataBuffer;

		if (FALSE == SetupDiGetDeviceRegistryProperty(hDev,&DeviceInfoData,SPDRP_FRIENDLYNAME,NULL,(LPBYTE)pDataBuffer,dwSize,&dwRequiredSize))
		{
			dwError = GetLastError();
			//OutputDebugStringWhenDebugMode("��ȡ�豸��Ϣʱ��ȡ�Ѻ����Ƴ���,�����%d",dwError);
		}

		DevInfo.strDevName = pDataBuffer;

		if (dwDescLen == 0)
			DevInfo.strDevDesc = DevInfo.strDevName;

		DevInfo.strServiceName = _T("");
		DevInfo.strDisplayName = _T("");
		DevInfo.dwFirstStartType = 0;
		DevInfo.dwBluetoothFlag = 0;
		if(DevClassGuid==GUID_DEVCLASS_BLUETOOTH)
			DevInfo.dwBluetoothFlag = MICROSOFT_BLUETOOTH;
		//��ȡӲ��״̬
		DWORD dwStatus; //�豸״̬
		DWORD dwProblem; //������
		dwError = CM_Get_DevNode_Status(&dwStatus,&dwProblem,DeviceInfoData.DevInst,0);
		if(CR_SUCCESS != dwError)
		{
			if(DevInfo.ClassGuid==GUID_DEVCLASS_DISKDRIVE)
			{	
				DevInfo.dwStatus = DEV_NOT_EXIST;
				DevInfo.bHidden = FALSE;
				DevInfoList.AddTail(DevInfo);
			}
			else
			{
				CString strTipInfo;
				strTipInfo.Format(_T("��ȡ�豸��Ϣʱ��ȡ�豸״̬����,�����%d"),dwError);
				OutputDebugString(strTipInfo);
			}
			continue;
		}

		if ((dwStatus&DN_NO_SHOW_IN_DM) || bHiddenClass) //�Ƿ������ص��豸�����豸��
			DevInfo.bHidden = TRUE;
		else
			DevInfo.bHidden = FALSE;

		if ((dwStatus&DN_STARTED) && (dwProblem==0))
		{
			DevInfo.dwStatus = DEV_ENABLE;
		}
		else if ((dwStatus&DN_HAS_PROBLEM) && (dwProblem==CM_PROB_DISABLED))
		{
			DevInfo.dwStatus = DEV_DISABLE;
		}
		else if ((dwStatus&DN_HAS_PROBLEM) && (dwProblem!=CM_PROB_DISABLED))
		{
			DevInfo.dwStatus = DEV_PROBLEM;
		}
		else
		{
			DevInfo.dwStatus = DEV_NOT_EXIST;
		}

		DevInfoList.AddTail(DevInfo);
	}

	dwError = GetLastError();

	if(hDev != INVALID_HANDLE_VALUE)
	{
		SetupDiDestroyDeviceInfoList(hDev);
		hDev = INVALID_HANDLE_VALUE;
	}

	return bRet;
}