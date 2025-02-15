#pragma once
#include "DllCore/Utils/ServiceControl.h"
#include "Utils/UIFileList.h"
typedef enum tagSVR_TYPE
{
	SERVICE_TYPE_KERNEL_DRIVER						= 0x1,		//The service is a device driver.
	SERVICE_TYPE_FILE_SYSTEM_DRIVER				= 0x2,		//The service is a file system driver.
	SERVICE_TYPE_WIN32_OWN_PROCESS			= 0x10,	//The service runs in its own process.
	SERVICE_TYPE_WIN32_SHARE_PROCESS		= 0x20,	//The service shares a process with other services.
}SVR_TYPE;

typedef enum tagSVR_STARTTYPE
{
	BOOT_START = 0,			//引导启动
	SYSTEM_START = 1,		//系统启动
	AUTO_START = 2,			//自动启动
	DEMAND_START = 3,		//手动启动
	DISABLED_START = 4,	//禁止启动
}SVR_STARTTYPE;

typedef enum tagSVR_STATE
{
	STATE_STOPPED = 1,			//The service has stopped.
	START_PENDING = 2,			//The service is starting.
	STOP_PENDING = 3,			//The service is stopping.
	RUNNING = 4,					//The service is running.
	CONTINUE_PENDING = 5,	//The service is about to continue.
	PAUSE_PENDING = 6,			//The service is pausing.
	PAUSED = 7,						//The service is paused.
}SVR_STATE;

typedef CDuiList<CDuiString,LPCTSTR> CDependList;

typedef enum tagCOPY_ITEM_INFO
{
	COPY_ITEM_INFO_SVR_NAME	= 0,
	COPY_ITEM_INFO_DISPNAME	= 1,
	COPY_ITEM_INFO_SVR_PATH		=	2,
}COPY_ITEM_INFO;

//typedef struct tagSERVICEINFO
//{
//	CDuiString strSvrName;				//服务名称
//	CDuiString strDisplayName;		//显示名称
//	CDuiString strDescription;			//服务描述
//	CDuiString strPath;						//服务路径
//	DWORD    dwPID;						//进程ID
//	SVR_STATE    dwRunStatus;		//服务状态
//	SVR_STARTTYPE    dwStartType;	//启动状态
//	SVR_TYPE    dwType;					//服务类型
//	//CStdValArray strDependencies; //依赖服务
//	//CDependList	DependList;			//依赖服务
//}SERVICEINFO;

/*typedef CDuiList<SERVICEINFO,const SERVICEINFO&> CSvrInfo;*/

class CServices : public CNotifyPump
{
public:
	CServices();
	~CServices();
public:
	void NotifyTask(PCNTCHDR pNTCHDR);
	void SetPaintManager(CPaintManagerUI* pPaintMgr);
	void OnServiceMenu(CControlUI* pControl);
protected:
	DUI_DECLARE_MESSAGE_MAP()
	void OnLoadItem(TNotifyUI& msg);
	void OnMenu(TNotifyUI& msg);
	// 主要重新加载服务项的启动状态，运行状态，进程ID等，如果需要其他信息，请使用全部刷新
	void OnKeyDown(TNotifyUI& msg);
	void OnRefresh(TNotifyUI& msg);
protected:
	//	菜单相应
	void StartSelectedSrv(CFileListItemUI* pElement);
	void StopSelectedSrv(CFileListItemUI* pItem);
	void StopProcessSrv(CFileListItemUI* pElementItem);
	void ReStartSrv(CFileListItemUI* pElement);
	void OnRefreshItem(CFileListItemUI* pServiceItem);

	void ModifyStartType(CFileListItemUI* pServiceItem, SVR_STARTTYPE StartType);
	void CopySvrInfo(CFileListItemUI* pSvrItem, COPY_ITEM_INFO CopySvrInfo);
	//	菜单相应
	BOOL GetSvrInfo(CSrvInfoList& SvrInfoList);
protected:
	static void FormatPid(DWORD Pid, CDuiString& strPid);
	static void FormatRunStatus(DWORD dwCurrentState, CDuiString& strRunState);
	static void FormatStartType(DWORD dwStartType, CDuiString& strStartType);
	static BOOL TerminateProcessByProcessId(DWORD dwProcessId);
protected:
	CDuiString				m_strInput;
	DWORD					m_dwTick;
	DWORD					m_dwSearch;
	CControlBuilder		m_RootBuilder;
	CDialogBuilder		m_DialogBuilder;
	CSrvInfoList				m_ServicesInfo;
	CPaintManagerUI*	m_pPaintManager;
};