#pragma once

namespace DuiLib
{
	/* hit test codes */
	#define HEHT_LEFTMARGIN				(0x1)
	#define HEHT_ADDRESS						(0x2)
	#define HEHT_ADDRESSSPACING		(0x3)
	#define HEHT_HEXDUMP					(0x4)
	#define HEHT_HEXDUMPSPACING	(0x5)
	#define HEHT_ASCIIDUMP					(0x6)
	#define HEHT_RIGHTMARGIN			(0x7)

	typedef struct tagWINHEX_DATA
	{
		HLOCAL	hBuffer;
		DWORD	dwStyle;
		DWORD	dwMaxSize;
		UINT		uColumnsPerLine;
		UINT		uLines;
		UINT		uVisibleLinesComplete;
		UINT		uVisibleLines;							// �ɼ���
		UINT		uIndex;										// ��ǰ�ַ�����
		UINT		uLineHeight;							// �ַ�ƽ���߶�
		UINT		uCharWidth;								// �ַ�ƽ����ȣ���Ҫʹ�õȿ�����
		BOOL		bVisible;

		INT			nLeftMargin;							// ������
		INT			nAddressSpaceing;					// ��ַ������
		INT			nSplitSpacing;							// �ָ������

		BOOL		bHexField;								// �Ƿ��ڱ༭�ռ���
		INT			nCaretCol;								// �к�
		INT			nCaretLine;								//	�к�
		BOOL		bInMid;									// �Ƿ��ǵ�ǰ�ֽڵĵ�4λ����

		INT			nSelStart;									// ѡ���ı���ʼ����
		INT			nSelEnd;									// ѡ���ı���������
	} WINHEX_DATA, *PWINHEX_DATA;

	class UILIB_API CWinHexUI : public CVerticalLayoutUI
	{
	public:
		CWinHexUI();
		~CWinHexUI();

	public:
		LPCTSTR GetClass() const;
		UINT GetControlFlags() const;
		LPVOID GetInterface(LPCTSTR pstrName);

		void DoInit();

		void DoEvent(TEventUI& event);
		void PaintText(HDC hDC);
		void PaintBorder(HDC hDC);
		void PaintStatusImage(HDC hDC);
		void PaintAddress(HDC hDC, RECT& rc, INT nLine);
		void PaintHexText(HDC hDC);
		void PaintAsciiText(HDC hDC);
		void SetAttribute(LPCTSTR pstrName, LPCTSTR pstrValue);
	public:
		void SetFont(int nFont);
		void SetTextColor(DWORD dwTextColor);
		UINT GetFirstVisibleLine();
	protected:
		void Update();
		void PaintLines(HDC hDC, DWORD dwScrollPos, DWORD dwFirst, DWORD dwLast, RECT& rc);
	public:
		void Cut();
		void Copy();
		void Paste();
		void Delete();
		void SelectAll();
		
		void MoveCaret(BOOL bScroll);
		DWORD OnHitRegionTest(POINT pt);
		DWORD GetIndexFromPoint(POINT pt, DWORD Hit, POINT *EditPos, BOOL *EditField);
	public:
		void SetMaxBufferSize(DWORD dwMaxSize);
		LRESULT LoadBuffer(PVOID pBuffer, DWORD dwSize);
		LRESULT CopyBuffer(PVOID pBuffer, DWORD dwSize);
		CHARRANGE GetSelection() const;
	protected:
		void OnEventChar(TEventUI& event);
		void OnEventKeyDown(TEventUI& event);
	protected:
		DWORD				m_iFont;
		BOOL					m_bCaretOn;
		POINT					m_ptCaret;
		INT						m_nHitTest;
		//UINT					m_ClipboardFormatID;
		UINT					m_uButtonState;
		DWORD				m_dwTextColor;
		DWORD				m_dwBackColor;
		DWORD				m_dwCaretColor;
		PWINHEX_DATA m_pWinHexData;
	};
}