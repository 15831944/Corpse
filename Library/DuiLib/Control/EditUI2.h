#ifndef __UIEDIT2_H__
#define __UIEDIT2_H__

#pragma once

namespace DuiLib
{
	class UILIB_API CEditUI2 : public CLabelUI
	{
	public:
		CEditUI2();
		~CEditUI2();

	public:
		LPCTSTR GetClass() const;
		LPVOID GetInterface(LPCTSTR pstrName);
		UINT GetControlFlags() const;
		void DoInit();

		void DoEvent(TEventUI& event);
		void SetAttribute(LPCTSTR pstrName, LPCTSTR pstrValue);

		void PaintStatusImage(HDC hDC);
		void PaintText(HDC hDC);
		void PaintBorder(HDC hDC);

		void SetPos(RECT rc);
	public:
		BOOL IsReadOnly() const;
		void SetReadOnly(BOOL bReadonly);
		BOOL IsPasswordMode() const;
		void SetPasswordMode(BOOL bpassword);
		void SetPasswordChar(LPCTSTR lpszPasswordChar);
		BOOL IsDigitalMode() const;
		void SetDigitalMode(BOOL bDigital);
		void SetLimitLength(UINT nLimitLength);
		UINT GetLimitLength() const;
		void SetNormalImage(LPCTSTR lpszNormalImage);
		void SetHotImage(LPCTSTR lpszHotImage);
		void SetFocusedImage(LPCTSTR lpszFocusedImage);
		void SetDisabledImage(LPCTSTR lpszDisabledImage);

		void SetCaretWidth(int nCaretWidth);
		void SetSelectTextColor(DWORD dwSelectTextColor);
		void SetSelectBkColor(DWORD dwSelectbkColor);
		void SetText(LPCTSTR pstrText);
		void SetWaterText(LPCTSTR lpszWaterText);
		void SetWaterTextColor(DWORD dwWaterColor);

		BOOL CanPaste() const;
		BOOL IsWaterMode() const;
	protected:
		void OnCut(int nPos,int nLen);
		int OnPaste(int nMin,int nMax);
		void OnCopy(int nPos,int nLen);
		void OnSelectAll();

		UINT GetTextState() const;

		void SetWaterMode(BOOL bWaterMode);
		bool GetEditText(CDuiString& strEditText);
		void FixedCaretPos(int nFixed);

		void SaveEditText(LPCTSTR lpszEditText);
		bool OnKeyEvent(TEventUI& event);
		void PaintCaret(HDC hDC);
		void CalcCaretRect(HDC hDC,const CDuiString& sText);
		void CalcStartSelRect(HDC hDC,const CDuiString& sText,int nPos,RECT& rcRange);
		int GetCaretPos(HDC hDC,const CDuiString& sText);
		int GetTextLen(HDC hDC,int nPos);
		bool AdjustImeWindowPos();
		void RestartCaretBlinkTimer();
		void DrawText(HDC hDC,const RECT& rc,const CDuiString& sText);
		void DrawSelectionText(HDC hDC,const RECT& rc,const CDuiString& sText);
		/************************************************************************/
		/* �������ƣ�CalcTextRect                                                                         */
		/* �������ܣ����ַ���strText�д�nStartλ�ÿ�ʼȡnLen���ַ���			   */
		/*                  ������ռ��λ��																	   */
		/************************************************************************/
		void CalcTextRect(HDC hDC,const CDuiString& strText,RECT& rcSelection,int nStart,int nLen);

	protected:
		CEditTextHost*	m_pTextHost;
		CDuiString m_sNormalImage;			// �༭����ͨ״̬ͼƬ
		CDuiString m_sFocusedImage;			// �༭���ȡ����״̬ͼƬ
		CDuiString m_sHotImage;					// ��������ڱ༭����
		CDuiString m_sDisabledImage;			//	�༭���ڽ���״̬
	};
}
#endif // __UIEDIT2_H__