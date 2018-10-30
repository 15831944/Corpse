#pragma once

#pragma pack(push)
#pragma pack(8)

//�豸����
typedef enum tagDEVICE_TYPE_INFO
{
	DEVICE_TYPE_USB_STORAGE		=	0,
	DEVICE_TYPE_CD_ROM				=	1,
	DEVICE_TYPE_FLOPPY					=	2,
	DEVICE_TYPE_NETWORK			=	3,
	DEVICE_TYPE_DISK						=	4,
	DEVICE_TYPE_MAX						=	5,//���ֵ
	DEVICE_TYPE_OTHER					= 0x80,
}DEVICE_TYPE_INFO, *PDEVICE_TYPE_INFO;

// �������������ļ���ƥ�䷽ʽ
typedef enum tagMATCH_MODE
{
	MATCH_MODE_PRE			=	0,				// ��ƥ���ļ���/�ļ���ǰ׺
	MATCH_MODE_SUB		=	1,				// ��ƥ���ļ���/�ļ��������Ӵ�
	MATCH_MODE_NAME	=	2,				// ��ƥ���ļ���/�ļ���
	MATCH_MODE_ALL			=	3,				// ƥ������·��
}MATCH_MODE;

//�������ظ�Ӧ�ò�汾����Ϣ
#define FS_FLT_DRV_VERSION				1

//����������
typedef enum tagFS_FLT_COMMAND
{
	FS_FLT_GET_FLT_ENABLE = 1,									// ��ȡ�������˹����Ƿ���������ΪFS_FLT_COMMAND�����ΪBOOL
	FS_FLT_SET_FLT_ENABLE,											// �����������˹����Ƿ���������ΪSET_FLT_ENABLE_INPUT�����Ϊ��
	FS_FLT_GET_DRV_VERSION = 3,									// ��ȡ���������汾�ţ�����ΪFS_FLT_COMMAND, ���ΪDWORD
	FS_FLT_GET_CAN_STOP_DRV,									// ��ȡ�Ƿ��ܹ�ֹͣ����
	FS_FLT_SET_CAN_STOP_DRV,
	//FS_FLT_GET_DEVICE_ACCESS_RIGHTS,						// ��ȡ(USB�����̡�����)�洢�豸����Ȩ�ޣ�����ΪLS_FS_FLT_COMMAND�����ΪLS_FLT_DEVICE_ACCESS_RIGHTS
	//FS_FLT_SET_DEVICE_ACCESS_RIGHTS,						// ����(USB�����̡�����)�洢�豸����Ȩ�ޣ�����ΪLS_FLT_DEVICE_ACCESS_RIGHTS�����Ϊ��
	//FS_FLT_GET_FILE_OPERATION_LOG_COUNT,			//��ȡ�ļ�������־����������ΪLS_FS_FLT_COMMAND�����ΪULONG
	//FS_FLT_GET_FILE_OPERATION_LOG,							//��ȡ�ļ�������־������ΪLS_FS_FLT_COMMAND�����ΪLS_FILE_OPERATION_LOG_APP
	//FS_FLT_CLEAR_FILE_OPERATION_LOG,						//����ļ�������־������ΪLS_FS_FLT_COMMAND�����Ϊ��
	//FLT_SET_FILE_PATH_RULE,											//�����ļ�ָ��·�����ʹ���,��С����ΪDWORD�����Ϊ��
	//FS_FLT_SET_LANGUARD_PROCESS_ID,						//�����ն�ǰ̨����ID������ΪFSFLT_LANGUARD_PROCESS_ID , ���Ϊ��
	//FS_FLT_SET_LS_CD_BURN_PROCESS_ID,					//�������Ź��̿�¼����ID������ΪFSFLT_LANGUARD_PROCESS_ID�����Ϊ��
	//FS_FLT_SET_ENABLE_CD_BURN,									//�����Ƿ�������̿�¼������ΪSET_FUNCTION_ENABLE�����Ϊ��
	FS_FLT_GET_ENABLE_USB_DISK_EXECUTE,				//��ȡ�Ƿ�������̿�¼������ΪLS_FS_FLT_COMMAND�����ΪBOOL
	FS_FLT_SET_ENABLE_USB_DISK_EXECUTE,					//�����Ƿ�������̿�¼������ΪSET_FUNCTION_ENABLE�����Ϊ��
	FS_FLT_GET_PATH_FILE_RULE_COUNT,						// ��ȡ�������������ļ���Ϣ��������
	FS_FLT_GET_PATH_FILE_INFO,										// ��ȡ�������������ļ���Ϣ
	FS_FLT_SET_PATH_FILE_INFO,										// ���ù������������ļ���Ϣ
}FS_FLT_COMMAND;

//�����������˹����Ƿ���(�����ܿ���)
typedef struct tagSET_FLT_ENABLE_INPUT
{
	FS_FLT_COMMAND FltCommand;
	BOOL bFltEnable;
}SET_FLT_ENABLE_INPUT,*PSET_FLT_ENABLE_INPUT;

typedef struct tagFS_FLT_COMMAND_SIZE
{
	FS_FLT_COMMAND FltCommand;
	ULONG dwMinInputBufferSize;
	ULONG dwMinOutputBufferSize;
}FS_FLT_COMMAND_SIZE, *PFS_FLT_COMMAND_SIZE;

typedef struct tagFS_FLT_LANGUARD_PROCESS_ID
{
	FS_FLT_COMMAND FltCommand;
	ULONG dwLanguardProcessId;//��������
}FS_FLT_LANGUARD_PROCESS_ID, *PFS_FLT_LANGUARD_PROCESS_ID;

//�����������˹����Ƿ������U�����п�ִ�г���
typedef struct tagSET_FUNCTION_ENABLE
{
	FS_FLT_COMMAND FltCommand;
	BOOL bEnable;
}SET_FUNCTION_ENABLE, *PSET_FUNCTION_ENABLE;

//�ļ�ָ��·��
typedef struct tagUSER_FILE_PATH_RULE_ITEM
{
	//�ļ�·��,��:C:\Docume~1\MyUser\My Documents\Test.txt (�����Զ�ת��Ϊ�豸������,C:\-->\Device\HarddiskVolume1\)
	WCHAR				wszFilePath[MAX_PATH]; 
	BOOL					bEnable;
	MATCH_MODE	MatchMode;
}USER_FILE_PATH_RULE_ITEM,*PUSER_FILE_PATH_RULE_ITEM;

// �������������ļ���Ϣ
typedef struct tagUSER_FILE_PATH_RULE
{
	ULONG dwRuleItemCount;						//��������
	USER_FILE_PATH_RULE_ITEM FilePathRuleItem[1];
}USER_FILE_PATH_RULE, *PUSER_FILE_PATH_RULE;

// ���ù������������ļ���Ϣ
typedef struct tagSET_USER_FILE_PATH_RULE
{
	FS_FLT_COMMAND FltCommand;
	ULONG dwRuleItemCount;						//��������
	USER_FILE_PATH_RULE_ITEM FilePathRuleItem[1];
}SET_USER_FILE_PATH_RULE, *PSET_USER_FILE_PATH_RULE;

//ÿ�������ֶ�Ӧ�Ļ�������С
const static FS_FLT_COMMAND_SIZE sFsFltCommandSize[]=
{
	{FS_FLT_GET_FLT_ENABLE , 0 , sizeof(BOOL)} ,																			// ��ȡ���������Ƿ���
	{FS_FLT_SET_FLT_ENABLE , sizeof(SET_FLT_ENABLE_INPUT) , 0},												// ���ù��������������˹���
	{FS_FLT_GET_DRV_VERSION, 0, sizeof(DWORD)},																		// ��ȡ���������汾
	{FS_FLT_GET_CAN_STOP_DRV , 0 , sizeof(BOOL)} ,																	// ��ȡ�Ƿ����ֹͣ��������
	{FS_FLT_SET_CAN_STOP_DRV, sizeof(SET_FUNCTION_ENABLE), 0},											// ���ù��������Ƿ����ֹͣ
	/*{FS_FLT_SET_LANGUARD_PROCESS_ID , sizeof(FS_FLT_LANGUARD_PROCESS_ID)},
	{FS_FLT_SET_LS_CD_BURN_PROCESS_ID , sizeof(FS_FLT_LANGUARD_PROCESS_ID)},
	{FS_FLT_SET_ENABLE_CD_BURN , sizeof(SET_FUNCTION_ENABLE)},*/
	{FS_FLT_GET_ENABLE_USB_DISK_EXECUTE , 0 , sizeof(BOOL)},													// ��ȡ�Ƿ������U��ִ��Ӧ�ó���
	{FS_FLT_SET_ENABLE_USB_DISK_EXECUTE , sizeof(SET_FUNCTION_ENABLE) , 0},					// �����Ƿ������U��ִ��Ӧ�ó���
	{FS_FLT_GET_PATH_FILE_RULE_COUNT, 0, sizeof(ULONG)},														// ��ȡ�������������ļ���������
	{FS_FLT_GET_PATH_FILE_INFO, 0, sizeof(USER_FILE_PATH_RULE)},											// ��ȡ�������������ļ���Ϣ
	{FS_FLT_SET_PATH_FILE_INFO, offsetof(SET_USER_FILE_PATH_RULE, FilePathRuleItem), 0},	// ���ù������������ļ���Ϣ
};
#pragma pack(pop)
