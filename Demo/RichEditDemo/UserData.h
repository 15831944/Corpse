#pragma once

// �Զ��������Ϣ
class CCustomFaceInfo
{
public:
	CCustomFaceInfo();
	~CCustomFaceInfo();
public:
protected:
	UINT		m_nFileId;
	CString		m_strKey;
	CString		m_strServer;

	// �����Զ������ʹ�ò���
	DWORD	m_dwFileSize;
	CString		m_strFileName;
	CString		m_strFilePath;
	//�����ļ�����ʱ�����ֶδ����ļ��������ļ����������ļ�
	BOOL		m_bOnline;
};
// ��Ϣ����
class CContent 
{
public:
	CContent();
	~CContent();

public:
protected:
	// ��������
	CONTENT_TYPE m_nType;
	// ������Ϣ
	CFontInfo		m_FontInfo;
	// �ı���Ϣ
	CString			m_strText;
	// ϵͳ����
	int				m_nFaceId;
	// ���ڶ�������
	int				m_nShakeTime;
	// �Զ��������Ϣ�������͵���m_nType���ļ�ʱ��m_CFaceInfo���ļ����й���Ϣ��
	CCustomFaceInfo	m_CFaceInfo;
};

typedef std::vector<CContent*> CBuddyContent;