#include "StdAfx.h"
#include "LanGuardConfig.h"
#include "DllCore/Utils/ErrorInfo.h"

#define APP_CONFIG					_T("Server")		//	�����ļ���APP����
#define KEY_PORT_CONFIG		_T("Port")			//	�����ļ��ж˿ں�
#define KEY_DEBUG_CONFIG		_T("Debug")		//	�����ļ��е��Է�ʽ


#define DEFAULT_SERVER_PORT		6611						//	������Ĭ�϶˿�

#define DEFAULT_DEBUG_TYPE		DEBUG_TYPE_NONE	//	���������Է�ʽ

CLanGuardConfig::CLanGuardConfig()
	: m_wPort(DEFAULT_SERVER_PORT)
	, m_DbType(DEFAULT_DEBUG_TYPE)
{

}

CLanGuardConfig::~CLanGuardConfig()
{

}

BOOL CLanGuardConfig::Load(LPCTSTR lpszConfigPath)
{
	BOOL bSuccess = FALSE;
	do 
	{
		// ��ȡ��ǰִ�г���·��
 		TCHAR szModuleFilePath[MAX_PATH];
 		GetModuleFileName(NULL, szModuleFilePath, _countof(szModuleFilePath));
 		PathRemoveFileSpec(szModuleFilePath);
		// ƴ�������ļ�·��
		PathAppend(szModuleFilePath, lpszConfigPath);

		m_wPort = GetPrivateProfileInt(APP_CONFIG, KEY_PORT_CONFIG, DEFAULT_SERVER_PORT, szModuleFilePath);

		m_DbType = (DEBUG_TYPE)GetPrivateProfileInt(APP_CONFIG, KEY_DEBUG_CONFIG, DEFAULT_DEBUG_TYPE, szModuleFilePath);

		bSuccess = TRUE;
	} while (FALSE);

	return bSuccess;
}

WORD CLanGuardConfig::GetPort() const
{
	return m_wPort;
}

DEBUG_TYPE CLanGuardConfig::GetDebugType() const
{
	return m_DbType;
}