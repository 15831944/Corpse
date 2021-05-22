#pragma once

#ifdef LS_STATIC_LIB_CALL
#define DLL_API 
#elif defined  DLL_EXPORTS
#define DLL_API __declspec(dllexport)
#else
#define DLL_API __declspec(dllimport)
#endif

typedef struct tagTRAYDATAFIREWALL
{
	HWND		hMainWnd;
	UINT		uID;
	UINT		uCallbackMessage;	
	DWORD	Reserved[2];		
	HICON		hIcon;								//����ͼ��ľ��	
	DWORD	Reserved2[3];
	TCHAR		szExePath[MAX_PATH];
	TCHAR		szTip[128];
}TRAYDATAFIREWALL,*PTRAYDATAFIREWALL;

typedef struct tagTRAYDATAFIREWALL64
{
	INT64		hMainWnd;
	UINT		uID;
	UINT		uCallbackMessage;	
	DWORD	Reserved[2];
	HICON		hIcon;								//����ͼ��ľ��	
	DWORD	Reserved2[5];
	TCHAR		szExePath[MAX_PATH];
	TCHAR		szTip[128];
}TRAYDATAFIREWALL64,*PTRAYDATAFIREWALL64;

typedef enum tagTRAY_AREA
{
	Tray = 0,
	Overflow,
}TRAY_AREA;

typedef struct tagTRAYDATAINFO
{
	CString		strTitle;						//������ʾ����
	CString		strFilePath;					//	�ļ�·��
	HICON		hTrayIcon;					//����ͼ��
	HWND		hWnd;							//���ھ��
	UINT		uID;								//ͼ��ID
	UINT		uBtnID;
	UINT		uCallbackMessage;		//�ص�������Ϣ��
	BOOL		bShowTray;					//�Ƿ���ʾͼ��
	UINT		bTrayIcon;					//����ͼ��
	TRAY_AREA TrayArea;
}TRAYDATAINFO,*PTRAYDATAINFO;

typedef CDuiList<TRAYDATAINFO,const TRAYDATAINFO&> CTrayDataInfo;

typedef struct tagTBBUTTON64 
{
	int iBitmap;
	int idCommand;
	BYTE fsState;
	BYTE fsStyle;
	BYTE bReserved[6];          // padding for alignment
	DWORD_PTR dwData;
	INT_PTR iString;
} TBBUTTON64, NEAR* PTBBUTTON64;

/************************************************************************/
/* ��  ��  ����GetTrayWndHwnd                                                                */
/* �������ܣ���ȡ����ͼ�괰�ھ��															   */
/* �����������																						   */
/* ��  �� ֵ����ȡ�ɹ����ش��ھ�������򷵻�NULL								   */
/************************************************************************/
DLL_API HWND GetTrayWndHwnd();

/************************************************************************/
/* ��  ��  ����GetOverflowHwnd                                                               */
/* �������ܣ���ȡ����ͼ��������򴰿ھ��											   */
/* �����������																						   */
/* ��  �� ֵ����ȡ�ɹ����ش��ھ�������򷵻�NULL								   */
/************************************************************************/
DLL_API HWND GetOverflowHwnd();

/************************************************************************/
/* ��  ��  ����GetTrayIcon			                                                               */
/* �������ܣ���ȡ32λ�汾��Ӧ��ϵͳͼ��												   */
/* ���������hWndTray	���̴��ھ��													   */
/*					 TrayDataInfoList ���ڱ�������ͼ����Ϣ����   					   */
/* ��  �� ֵ���ɹ�����TRUE�����򷵻�FALSE											   */
/************************************************************************/
DLL_API BOOL GetTrayIcon(HWND hWndTray, CTrayDataInfo& TrayDataInfoList);

/************************************************************************/
/* ��  ��  ����GetTrayIcon			                                                               */
/* �������ܣ���ȡ64λ�汾��Ӧ��ϵͳͼ��												   */
/* ���������hWndTray	���̴��ھ��													   */
/*					 TrayDataInfoList ���ڱ�������ͼ����Ϣ����   					   */
/* ��  �� ֵ���ɹ�����TRUE�����򷵻�FALSE											   */
/************************************************************************/
DLL_API BOOL GetTrayIcon64(HWND hWndTray, CTrayDataInfo& TrayDataInfoList);