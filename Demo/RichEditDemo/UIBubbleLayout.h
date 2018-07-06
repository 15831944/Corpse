#pragma once

#define DUI_CTR_BUBBLELAYOUT  _T("BubbleLayout")

class CBubbleLayoutUI : public CHorizontalLayoutUI
{
public:
	CBubbleLayoutUI();
	~CBubbleLayoutUI();

public:
	LPCTSTR GetClass() const;
	LPVOID GetInterface(LPCTSTR pstrName);
	//UINT GetControlFlags() const;

	//void DoInit();
	//bool Add(CControlUI* pControl);
	void SetPos(RECT rc);
	SIZE EstimateSize(SIZE szAvailable);
public:
	void RejustBubble();
	void SetParaSpace(DWORD dwParaSpace);
	void AddBubbleMsg(LPCTSTR lpszNickName, LPCTSTR lpszBubbleMsg);
	void SetBubbleSize(SIZE size);
	void SetBubbleImage(LPCTSTR lpszBubbleImage);

	void SetRichTextRange(DWORD dwStart, DWORD dwEnd);
	void SetRichTextRange(const CHARRANGE& CharRange);
	
	void SetNickNameLen(DWORD dwNickLen);
protected:
	void CalAsLeft();
	void CalAsRight();
	
	bool OnLabelSize(LPVOID lParam);
protected:
	//CHARRANGE	m_CharRange;		// ������ݿ�ͷ�ͽ���λ��
	int			m_dwNickNameLen;		// �ǳ����ֿ�ͷ����,�����س����г���
	DWORD			m_dwParaSpace;				// �ǳ����ֽ�������

	int					m_dwTextStart;				// ��Ϣ���ֿ�ͷ����
	int					m_dwTextEnd;					//	��Ϣ���ֽ�������
	BOOL				m_bLeftAlign;					// �Ƿ������
	DWORD			m_dwLIndent;					//	��Ϣ����������
	DWORD			m_dwRIndent;					//	��Ϣ����������
	//BOOL				m_bEstimateSized;
	SIZE					m_szAvailable;
	CDuiString		m_strBubbleImage;
};
