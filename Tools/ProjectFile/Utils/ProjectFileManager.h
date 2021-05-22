#pragma once

typedef enum tagPROJECT_FILE_PATH_TYPE
{
	PROJECT_FILE_TYPE_RELATIVE	=	1,	// ʹ�������ļ����·��
	PROJECT_FILE_TYPE_TOOLS		=	2,	// ʹ�ó��򹤾����·��
	PROJECT_FILE_TYPE_ABSOLUTE	=	3,	// ʹ�������ļ������õľ���·��
	PROJECT_FILE_TYPE_LAST			= 4,
}PROJECT_FILE_PATH_TYPE;

class CProjectFileManager
{
public:
	CProjectFileManager();
	~CProjectFileManager();
public:
	CString GetProjectName() const;
	void SetProjectName(LPCTSTR lpszProjectName);

	CString GetProjectFilePath() const;
	void SetProjectFilePath(LPCTSTR lpszProjectFilePath);

	DWORD GetNoSuchFileColor() const;
	void SetNoSuchFileColor(DWORD dwNoSuchFileColor);

	DWORD GetErrorAttributeColor() const;
	void SetErrorAttributeColor(DWORD dwErrorAttributeColor);

	PROJECT_FILE_PATH_TYPE GetProjectFileType() const;
	void SetProjectFileType(PROJECT_FILE_PATH_TYPE ProjectFileType);
protected:
	DWORD	m_dwNoSuchFileColor;
	DWORD	m_dwErrorAttributeColor;
	CString		m_strProjectName;
	CString		m_strProjectFilePath;
	PROJECT_FILE_PATH_TYPE m_ProjectFileType;
};