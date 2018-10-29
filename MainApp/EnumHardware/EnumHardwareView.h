
// EnumHardwareView.h : CEnumHardwareView ��Ľӿ�
//


#pragma once
#include "LsCommon/Class/Hardware.h"
typedef struct tgaDevName 
{
	CString strDevName;
	GUID DevGuid;
	UINT  DevIcon;
}DEVNAME,*PDEVNAME;

typedef CList<DEVNAME,DEVNAME&>CDevNameList;

//�豸״̬
#define DEV_DISABLE				0 //�豸���ڽ���״̬
#define DEV_ENABLE				1 //�豸��������״̬
#define DEV_PROBLEM			2 //�豸��������
#define DEV_NOT_EXIST		3 //�豸������

//�豸����
#define DEV_OP_NONE				0 //�豸�޲���
#define DEV_OP_DISABLE			1 //�����豸
#define DEV_OP_ENABLE			2 //�����豸
#define DEV_OP_RESET			3 //�����豸
#define	DEV_OP_REVERSE			4 //�豸״̬ȡ��

#define MICROSOFT_BLUETOOTH		1//΢��Ĭ������
#define THIRD_BLUETOOTH			2//����������

//���������
typedef struct _Outer_Dev_Manager
{
	CString strDevClassName;	//�����е���������
	CString strDevClassDisplayName;	//�����豸����ʾ������
	GUID	DevChangeGuid;		//ע��Ӳ���仯֪ͨ��Ϣʱ�õ�GUID
	GUID	DevClassGuid;	//ö��Ӳ��ʱö��Ӳ��ʱ�õ�GUID
	DWORD	dwSubClass;		//�����,Ŀǰ��Ҫ�������ִ�����
	DWORD	dwOperator;		//��������
}OUTER_DEV_MANAGER,*POUTER_DEV_MANAGER;
typedef CArray<OUTER_DEV_MANAGER,OUTER_DEV_MANAGER&> COuterDevManagerArray;

class CEnumHardwareView : public CTreeView
{
protected: // �������л�����
	CEnumHardwareView();
	DECLARE_DYNCREATE(CEnumHardwareView)

// ����
public:
	CEnumHardwareDoc* GetDocument() const;

// ����
public:

// ��д
public:
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);
protected:
	virtual void OnInitialUpdate(); // ������һ�ε���

// ʵ��
public:
	virtual ~CEnumHardwareView();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

protected:
	BOOL m_bShowAll;
	CDevInfoList m_DevInfoList;
	CDevNameList m_DevNameList;
	CImageList m_ImgList;
// ���ɵ���Ϣӳ�亯��
protected:
	afx_msg LRESULT OnShowHardware(WPARAM wParam,LPARAM lParam);
	afx_msg void OnRClick(NMHDR* pNMHdr,LPARAM* lResult);
	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnShowall();
	afx_msg void OnDonotShowHide();
	afx_msg void OnHardwareScanf();
	afx_msg void OnHardwareProperty();
	afx_msg void OnHardwareEnable();
	afx_msg void OnHardwareDisable();

protected:
	BOOL SetDeviceStatus(OUTER_DEV_MANAGER& DeviceRuleManage);
	DWORD GetPhysicalDiskIndex(const DEV_INFO& DeviceInfo);
	BOOL GetDevInfoFromClassGuid(IN GUID DevClassGuid, OUT CDevInfoList& DevInfoList,BOOL bAllDevice);
};

#ifndef _DEBUG  // EnumHardwareView.cpp �еĵ��԰汾
inline CEnumHardwareDoc* CEnumHardwareView::GetDocument() const
   { return reinterpret_cast<CEnumHardwareDoc*>(m_pDocument); }
#endif

