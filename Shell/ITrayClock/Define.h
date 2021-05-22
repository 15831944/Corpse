#pragma once

#define WM_TCLOCK_CALC_SIZE		(WM_USER + 0x64)		//����ʱ��ؼ����ڴ�С
//#define WM_TCLOCK_RESVERED1		(WM_USER + 0x64)		// δ֪������
#define WM_TCLOCK_LBUTTON		(WM_USER + 0x66)		// ������ʱ��������������
#define WM_TCLOCK_TOOLTIP			(WM_USER + 0x67)		// ������ʱ����ʾToolTip
#define WM_TCLOCK_HIGHLIGHT		(WM_USER + 0x68)		// ������ʱ�ӱ�������

#define TCLOCK_TIMER_ID_REFRESH	(0x1000)					// ���涨ʱˢ��

#ifndef CLS_HOT
#define CLS_HOT				2
#define CLS_PRESSED			3
#endif

typedef enum tagTCLOCK_HIGHLIGHT_WPARAM
{
	TCLOCK_NORMAL	= CLS_NORMAL,
	TCLOCK_HOT			= CLS_HOT,
	TCLOCK_PRESSED	= CLS_PRESSED,
}TCLOCK_HIGHLIGHT_WPARAM;

typedef enum tagMENU_ITEM
{
	IDM_ITEM_TEST1		= 0,
	IDM_ITEM_TEST2		= 1,
	IDM_ITEM_SEP			=	2,
	IDM_ITEM_ENABLE		= 3,
	IDM_ITEM_DISABLE	=	4,
	IDM_ITEM_EXIT			=	5,
}MENU_ITEM;