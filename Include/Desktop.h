#pragma once

// ���洰���Զ�����Ϣ�����ڡ���ʾ���桱
#define DTM_RAISE                       (WM_USER + 83)
// ����ֵ�Ǹ���Ϣ�ţ���Ӧ��LPARAM����
#define DTRF_RAISE		  0			// ��ʾ����
#define DTRF_LOWER      1				// �������棬��ʾӦ��
#define DTRF_QUERY       2			// ��ѯ״̬

typedef enum tagTOGGLEDESKTOP
{
	DESKTOP_PLUGIN_WORKING	= 0,	// ������Ϣ����Ƿ�������
	DESKTOP_PLUGIN_COUNT			=	1,	// ��ȡ���������ü���
	DESKTOP_PLUGIN_ADDREF		=	2,	// �������������ü���
};

// �Զ��塰��ʾ���桱��Ϣ������ת������ʾ���桱��Ϣ
#define TOGGLEDESKTOP					_T("ToggleDesktop")
// �Զ��塰��ʾ���桱��Ϣ�����ڻ�ȡ�����ò������״̬
#define TOGGLEDESKTOPMSG			_T("GetToggleDesktopMsg")
// �Զ��塰ж�ز������Ϣ������ж��ע������еĲ��
#define FREELIBRARYANDEXIT			_T("FreeLibraryAndExitThread")