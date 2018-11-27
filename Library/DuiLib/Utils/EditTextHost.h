#pragma once

namespace DuiLib
{
	typedef enum tagCALC_CARET_TYPE
	{
		CALC_CARET_TYPE_NONE				= 0,
		CALC_CARET_TYPE_POS					= 1,		// ��������������λ��
		CALC_CARET_TYPE_SELECT_MODE = 2,		// ����ѡ�����ֵķ�Χ
		CALC_CARET_TYPE_SELECT_ALL		 = 4,		// ȫѡģʽ
	}CALC_CARET_TYPE;

	class UILIB_API CEditTextHost
	{
	public:
		CEditTextHost();
		~CEditTextHost();

	public:
		void SetOwner(CControlUI* pOwner);
		BOOL DoEvent(TEventUI& event);
		void PaintCaret(HDC hDC);
		void PaintText(HDC hDC);
		void PaintStatusImage(HDC hDC);

		void CalcStartSelRect(HDC hDC,const CDuiString& sText,int nPos,RECT& rcRange);
		void CalcTextRect(HDC hDC,const CDuiString& strText,RECT& rcSelection,int nStart,int nLen);
	public:
		void SetText(LPCTSTR pstrText);
		CDuiString GetText() const;
		CDuiString GetEditText() const;
		CDuiString GetSelectionText() const;

		BOOL OnKeyEvent(TEventUI& event);
		void RestartCaretBlinkTimer();
		BOOL AdjustImeWindowPos();
	public:
		void SetTextColor(DWORD dwTextColor);
		DWORD GetTextColor() const;
		void SetDisabledTextColor(DWORD dwTextColor);
		DWORD GetDisabledTextColor() const;
		RECT GetTextPadding() const;
		void SetTextPadding(RECT rc);
		UINT GetTextState() const;

		BOOL IsReadOnly() const;
		void SetReadOnly(BOOL bReadOnly = TRUE);

		BOOL IsPasswordMode() const;
		void SetPasswordMode(BOOL bPassword = TRUE);
		void SetPasswordChar(LPCTSTR lpszPasswordChar);

		BOOL IsDigitalMode() const;
		void SetDigitalMode(BOOL bDigital = TRUE);

		BOOL IsWaterMode() const;
		void SetWaterMode(BOOL bWaterMode = TRUE);

		UINT GetLimitLength() const;
		void SetLimitLength(UINT nLimitLength);

		BOOL IsEnabled() const;
		BOOL IsFocused() const;
		CPaintManagerUI* GetManager();
		void Invalidate();

		void SetFont(int iFont);
		void SetTextStyle(int uTextStyle);

		void SetCaretColor(DWORD dwCaretColor);
		void SetCaretWidth(int nCaretWidth);

		void SetWaterText(LPCTSTR lpszWaterText);
		void SetWaterTextColor(DWORD dwWaterColor);
		void SetSelectBkColor(DWORD dwSelectBkColor);
		void SetSelectTextColor(DWORD dwSelectTextColor);
	public:
		void SelectAll();
		BOOL CanPaste();
		void OnCopy(int nPos, int nLen);
		int OnPaste(int nMin, int nMax);
		void OnCut(int nPos, int nLen);
	protected:
		void DrawText(HDC hDC,const RECT& rc,const CDuiString& sText);
		void DrawSelectionText(HDC hDC,const RECT& rc,const CDuiString& sText);

		void FixedCaretPos(int nFixed);
		int GetCaretPos(HDC hDC, const CDuiString& sText);
		void CalcCaretRect(HDC hDC,const CDuiString& sText);
	protected:
		CControlUI*		m_pOwner;									// ��Ӧ����ؼ�
		CDuiString		m_sText;										// �����������
		CDuiString		m_strWaterText;							// ˮӡ����
		CDuiString		m_strTextBak;								// ���ڱ���ԭʼ����

		CALC_CARET_TYPE CalcCaretType;						// ���������ǵķ�ʽ

		int					m_nCaretWidth;							// �����ǿ��
		DWORD			m_dwCaretColor;							// ��������ɫ
		BOOL				m_bCaretOn;								//	��ʾ������
		BOOL				m_bKeyDown;								// �˸�ť�Ѱ���
		BOOL				m_bFocused;								// ���ǻ�ȡ�˽���
		UINT				m_nTextHostState;						// ����״̬
		RECT				m_rcCaret;									//	����������λ��
		POINT				m_szCaretPt;								// ����ڱ༭����ʱ��λ��
		RECT				m_rcTextPadding;						// ������ʾλ��
		int					m_iFont;										//	������ʾ����
		int					m_uTextStyle;								//	����������ʽ
		int					m_nSelStart;								// ѡ�����ֵĿ�ʼλ��
		int					m_nCaretPos;								// �����������ֵ����
		int					m_nCaretOffset;							// ��������ڱ༭���ƫ����

		BOOL				m_bReadOnly;								//	ֻ��ģʽ
		BOOL				m_bPasswordMode;					//	����ģʽ
		BOOL				m_bDigitalMode;						//	����ģʽ
		TCHAR				m_cPasswordChar;						//	����ģʽ��ʾ���ַ�
		BOOL				m_bWaterMode;							// ˮӡģʽ
		DWORD			m_nLimitLength;							//	�����ַ�����

		DWORD			m_dwTextColor;							// �ļ�������ʾ����ɫ
		DWORD			m_dwDisabledTextColor;			// ���ֽ���ʱ����ɫ
		DWORD			m_dwWaterColor;						//	ˮӡ������ɫ
		DWORD			m_dwSelectTextColor;				//	ѡ���ı���������ɫ
		DWORD			m_dwSelectbkColor;					//	ѡ���ı������ֱ���ɫ
	};
}