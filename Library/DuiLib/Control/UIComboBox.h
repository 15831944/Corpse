#ifndef __UICOMBOBOX_H__
#define __UICOMBOBOX_H__

#pragma once

namespace DuiLib
{
	/// ��չ�����б��
	/// ����arrowimage����,һ��ͼƬƽ���ֳ�5��,Normal/Hot/Pushed/Focused/Disabled(������source����)
	/// <Default name="ComboBox" value="arrowimage=&quot;file='sys_combo_btn.png' source='0,0,16,16'&quot; "/>
	class UILIB_API CComboBoxUI : public CComboUI
	{
	public:
		CComboBoxUI();
		~CComboBoxUI();
	public:
		LPCTSTR GetClass() const;
		UINT GetControlFlags() const;
		LPVOID GetInterface(LPCTSTR pstrName);

		void DoInit();
		void DoEvent(TEventUI& event);
		void SetAttribute(LPCTSTR pstrName, LPCTSTR pstrValue);

		CDuiString GetText() const;
		void PaintText(HDC hDC);
		void PaintBorder(HDC hDC);
		void PaintStatusImage(HDC hDC);
	public:
		bool SelectItem(int iIndex, bool bTakeFocus = false);
		void SetCaretWidth(UINT nCaretWidth);
		void SetArrowWidth(UINT nArrowWidth);
		void SetDigitalMode(BOOL bDigital);
		void SetLimitLength(UINT nLimitLen);
	protected:
		int						m_nArrowWidth;
		CEditTextHost*	m_pTextHost;
	};
}

#endif // __UICOMBOBOX_H__
