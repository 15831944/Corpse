#pragma once

#ifdef LS_STATIC_LIB_CALL
#define DLL_API 
#elif defined  DLL_EXPORTS
#define DLL_API __declspec(dllexport)
#else
#define DLL_API __declspec(dllimport)
#endif

class DLL_API CSignHelper
{
public:
	CSignHelper();
	~CSignHelper();

public:
	/*
	ʹ�÷�ʽ
	if (CSign::VerifySignTrust(_T("C:\\Aquarius.exe")) != ERROR_SUCCESS)
	{
		��ȡ����ţ���ʽ�������֤���
	}*/
	static LONG VerifySignTrust(LPCTSTR lpszFilePath);
};