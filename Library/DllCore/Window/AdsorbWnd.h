#pragma once

#ifdef LS_STATIC_LIB_CALL
#define DLL_API 
#elif defined  DLL_EXPORTS
#define DLL_API __declspec(dllexport)
#else
#define DLL_API __declspec(dllimport)
#endif

#define DOCK_DISTANCE	30	// ����ͣ������10���ز���ͣ��Ч��

typedef enum tagDOCK_TYPE
{
	DOCK_TYPE_LEFT			= 0,	// ͣ�����������
	DOCK_TYPE_TOP				=	1,	// ͣ���������ϲ�
	DOCK_TYPE_RIGHT			=	2,	// ͣ���������Ҳ�
	DOCK_TYPE_BOTTOM	=	3,	// ͣ���������²�
	DOCK_TYPE_ALL				=	4,	// �Զ�ʶ��ͣ��
}DOCK_TYPE;

class DLL_API AdsorbWnd
{
public:
	static AdsorbWnd* GetInstance();
public:
	void Initialize(HWND hWnd, UINT nDistance = DOCK_DISTANCE, DOCK_TYPE DockType = DOCK_TYPE_RIGHT);
	void SetScreenSize(LPARAM lParam);
protected:
	AdsorbWnd();
	~AdsorbWnd();
protected:
	void UpdateSubWndPos(HWND hWnd, POINT pt);
	static UINT_PTR __WndProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
	UINT_PTR AdsorbWndProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
private:
	BOOL			m_bDocked;
	POINT			m_PtLastMouse;
	INT				m_nDistance;
	DOCK_TYPE	m_DockType;
	WNDPROC  m_pWndProc;
	LONG		    m_nScreenWidth;
	LONG			m_nScreenHeight;
	static AdsorbWnd m_sAdsorbWnd;
};