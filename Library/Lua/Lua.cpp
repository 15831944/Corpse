#include "StdAfx.h"
#include "Lua.h"

#include "DllCore/Log/LogHelper.h"

CLua::CLua()
	:m_pLua(NULL)
{
	 
}

CLua::~CLua()
{
	Close();
}

void CLua::Init()
{
	if (m_pLua == NULL)
	{
		m_pLua = luaL_newstate();
		luaL_openlibs(m_pLua);
	}
}

void CLua::Close()
{
	if (m_pLua != NULL)
	{
		lua_close(m_pLua);
		m_pLua = NULL;
	}
}

BOOL CLua::LoadFile(LPCTSTR lpszFileName)
{
	BOOL bSuccess = FALSE;
	do 
	{
		if (m_pLua == NULL)
			break;

		bSuccess = luaL_dofile(m_pLua, CStringA(lpszFileName));

		lua_pcall(m_pLua,0,0,0);
	} while (FALSE);

	return bSuccess;
}

BOOL CLua::DoFn(LPCTSTR lpszFunName, const CParamGroup& ParamIn, CParamGroup& ParamOut)
{
	BOOL bSuccess = FALSE;
	do 
	{
		if (m_pLua == NULL)
			break;

		lua_getglobal(m_pLua, CStringA(lpszFunName));
		// �����������
		for (int n=0; n<ParamIn.GetCount(); ++n)
			PushLuaData(m_pLua, ParamIn.GetParam(n));

		int nRet = lua_pcall(m_pLua, ParamIn.GetCount(), ParamOut.GetCount(), 0);
		if (nRet != 0)
		{
			QLOG_INFO(_T("���ú���[%s]ʧ�� : %s !"), lpszFunName, CString(lua_tostring(m_pLua, -1)));
			break;
		}

		// ��ȡ����ֵ
		int nPos = 0;
		for (int n=ParamOut.GetCount() - 1; n>=0; --n)
		{
			--nPos;
			PopLuaData(m_pLua, ParamOut.GetParam(n), nPos);
		}

		// ��Ϊ�Ҳ�֪�����ز����м����������һ���ݷ��ز����ĸ�����������ջ����
		// ���������Է�������������ջ��������ɾ���
		int nCount = lua_gettop(m_pLua);
		lua_settop(m_pLua, -1-ParamOut.GetCount());

		bSuccess = TRUE;
	} while (FALSE);

	return bSuccess;
}

BOOL CLua::PushLuaData(lua_State* pState, CParamData* pParam)
{
	BOOL bSuccess = FALSE;
	do 
	{
		if (pParam == NULL)
			break;

		if (pParam->GetParamType() == PARAM_TYPE_STRING)
			lua_pushstring(pState, (LPCSTR)pParam->GetParam());
		else if (pParam->GetParamType() == PARAM_TYPE_INT)
			lua_pushnumber(pState, (int)pParam->GetParam());
		else
			lua_pushlightuserdata(pState, pParam->GetParam());
		bSuccess = TRUE;
	} while (FALSE);

	return bSuccess;
}

BOOL CLua::PopLuaData(lua_State* pState, CParamData* pParam, int nIndex)
{
	BOOL bSuccess = FALSE;
	do 
	{
		if (pParam == NULL)
			break;

		if (pParam->GetParamType() == PARAM_TYPE_STRING)
		{
			if (lua_isstring(pState, nIndex) == 1)
			{
				const char* pData = (const char*)lua_tostring(pState, nIndex);
				pParam->SetData((void*)pParam, (int)strlen(pData));
			}
		}
		else if (pParam->GetParamType() == PARAM_TYPE_INT)
		{
			if (lua_isnumber(pState, nIndex) == 1)
			{
				int nData = (int)lua_tonumber(pState, nIndex);
				pParam->SetData((void*)nData, sizeof(int));
			}
		}
		else
		{

		}

		bSuccess = TRUE;
	} while (FALSE);

	return bSuccess;
}

//void LuaDo()
//{
//	 ����Lua������ 
//	//luaL_openlibs(m_pLua);
//	//lua_register(m_pLua, "average2", average);
//	///BOOL bRet = luaL_dofile(m_pLua,"avg.lua");
//	if (bRet)
//	{
//		printf("Load File Error");
//	}
//
//	//lua_pcall(m_pLua,0,0,0);
//	lua_getglobal(m_pLua,"mystr");
//	//printf("%s\r\n",lua_tostring(m_pLua,-1));
//	char funName[20];
//	strcpy(funName, lua_tostring(m_pLua,-1));
//	lua_getglobal(m_pLua, funName);
//	printf("#############\r\n");
//	lua_getglobal(m_pLua,"a");
//	lua_getglobal(m_pLua,"b");
//	int iRet = lua_pcall(m_pLua,2,1,0);
//	if (iRet)
//	{
//		printf("%s\r\n",lua_tostring(m_pLua,-1));
//	}
//	if (lua_isnumber(m_pLua,-1))
//		printf("%d.\r\n",lua_tointeger(m_pLua,-1));
//	//printf("%s.\r\n",lua_tostring(m_pLua,-1));
//	//lua_register(m_pLua, lua_tostring(m_pLua,-1), average);
//
//	//luaL_dofile(m_pLua, "avg.lua");
//}