#pragma once

// ������Ϣ��¼
typedef struct tagBUDDY_MSG_LOG
{
	UINT		uID;
	UINT		uUTalkNum;
	CString		strNickName;
	UINT64	nTime;
	BOOL		bSendFlag;
	CString		strContent;
}BUDDY_MSG_LOG;

// Ⱥ��Ϣ��¼
typedef struct tagGROUP_MSG_LOG
{
	UINT		nID;
	UINT		nGroupNum;
	UINT		nUTalkNum;
	CString		strNickName;
	UINT64	nTime;
	CString		strContent;
}GROUP_MSG_LOG;

class CMessageLog
{
public:
	CMessageLog();
	~CMessageLog();

public:
	// ��ȡ��Ϣ��¼�ļ�����
	CString GetMsgLogFileName();
	// ������Ϣ��¼�ļ�����
	void SetMsgLogFileName(LPCTSTR lpszFileName);

	// �ж���Ϣ��¼�ļ��Ƿ���Ч
	BOOL IsMsgLogFileValid();
	// ������Ϣ��¼�ļ�
	BOOL CreateMsgLogFile();
};