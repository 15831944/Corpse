#pragma once

//��typeΪCP_ACPʱ��UNICODEת��ΪGBK����typeΪCP_UTF8ʱ��UNICODEת��ΪUTF8
BOOL Transform(LPCWSTR lpszwch, int& nSize, LPSTR lpszConvert, int nType = CP_ACP);

// ��typeΪCP_ACPʱ��GBKת��ΪUNICODE����typeΪCP_UTF8ʱ��UTF8ת��ΪUNICODE
BOOL Transform(LPCSTR lpszch, int& nSize, LPWSTR lpszConvert, int nType = CP_ACP);