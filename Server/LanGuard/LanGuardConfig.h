#pragma once

typedef enum tagDEBUG_TYPE
{
	DEBUG_TYPE_NONE		= 0,	//	�޵�����Ϣ���
	DEBUG_TYPE_OUTPUT	=	1,	// ������Ϣ�����DbView
	DEBUG_TYPE_FILE			=	2,	//	������Ϣ������ļ�
}DEBUG_TYPE;

class CLanGuardConfig
{
public:
	CLanGuardConfig();
	~CLanGuardConfig();

public:
	BOOL Load(LPCTSTR lpszConfigPath);
	WORD GetPort() const;
	DEBUG_TYPE GetDebugType() const;

protected:
	WORD				m_wPort;
	DEBUG_TYPE	m_DbType;
};