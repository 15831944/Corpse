#pragma once

// ָ���ʼ��ʧ�ܣ�һ�������ڶ�̬�����ʧ��
#define INVALID_PTR_VALUE		((INT)(LONG)-1)
// �ͻ��˴��������а汾����ȷ
#define ERROR_VERSION				((INT)(LONG)-2)
// �ͻ��˿��������ȷ
#define ERROR_CMD						((INT)(LONG)-3)

CString GetADELErrorInfo(int nErrorCode);