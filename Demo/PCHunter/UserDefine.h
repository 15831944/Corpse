#pragma once

typedef enum tagWIZARD_ID
{
	WIZARD_ID_TASK		=	0,
	WIZARD_ID_NET			=	1,
}WIZARD_ID;

typedef struct tagNTCHDR
{	
	WIZARD_ID	nWizardId;			// ������������
	CString			strTabFrom;			// �����Ǹ���ǩ��
	CString			strTabTo;				// �л�����ǩ�������
	CString			strData;				// ��Json���������
}NTCHDR,*PNTCHDR;

typedef const PNTCHDR PCNTCHDR;