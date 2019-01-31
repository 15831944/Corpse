#pragma once
#include "DllCore/Thread/LsThread.h"

typedef struct tagHOST_INFO 
{
	CString strHostName;			// ����������IP��ַ
	DWORD dwCount;				// ����Ping������
	DWORD dwInterval;			// ÿ�η���ʱ����
	DWORD dwTTL;					// ���ݰ����ֵ
	DWORD dwPacketSize;		// ���ݰ���С
	DWORD dwTimeout;			// ��ʱʱ��
}HOST_INFO, *PHOST_INFO;

class CIPv4Result : public CNotifyPump, public CLsThread
{
public:
	CIPv4Result();
	~CIPv4Result();

public:
	void SetPaintMagager(CPaintManagerUI* pPaintMgr);
	void SetVirtualWnd(LPCTSTR lpszVirtualWnd);
	LPCTSTR GetVirtualWnd() const;
	void NotifyTask(PCNTCHDR pNTCHDR);
protected:
	BOOL IsIPv4String(LPCTSTR lpszIpString);
protected:
	//Ĭ���̺߳���
	virtual UINT ThreadProc();
protected:
	DUI_DECLARE_MESSAGE_MAP()
		void OnLoadItem(TNotifyUI& msg);
		void	OnClick(TNotifyUI& msg);
		void OnStop();
private:
	BOOL m_bLoad;
	HOST_INFO*		m_pHostInfo;
	CString					m_strVirtualWnd;
	CControlBuilder	m_RootBuilder;
	CDialogBuilder	m_DialogBuilder;
	CPaintManagerUI* m_pPaintManager;
};