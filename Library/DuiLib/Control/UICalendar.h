#pragma once

namespace DuiLib
{
	typedef struct tagCalendarStyle
	{
		DWORD dwWeekendColor;					//��ĩ������ɫ
		DWORD dwWeekdayColor;					//������������ɫ
		CDuiString strHotImage;							//���ڻ�ȡ���㱳��ͼƬ
		DWORD dwDayHotColor;						//���ڻ�ȡ���㱳����ɫ
		DWORD dwDayHotTextColor;				//���ڻ�ȡ����������ɫ
		CDuiString strPushedImage;					//���ڱ����µı���ͼƬ
		DWORD dwDayPushedColor;					//���ڱ����µı�����ɫ
		CDuiString strSelectedImage;					//���ڱ�ѡ�еı�����ɫ
		DWORD dwDaySelectedColor;				//���ڱ�ѡ�еı�����ɫ
		DWORD dwDaySelectedTextColor;		//���ڱ�ѡ�е�������ɫ
		DWORD dwDayDisabledColor;				//���ڱ����õı�����ɫ
		DWORD dwNotMonthDayColor;			//�Ǳ������ڱ�����ɫ
		//DWORD dwStatusBkColor;						//�ײ���Ϣ������ɫ
	}TCalendarStyle;
	
	typedef struct tag_CalendarInfo
	{
		int		nYear;
		int		nMooth;
		int		nDay;
		int		nAsMooth;									// �Ƿ��Ǳ������ڣ������п�����ʾ��һ�»���һ�µ����ڣ�
		int		nWeek;										//	�Ƿ��ǹ�����
		int		nWeekLine;									// ������Ϊ����0������
	}TCalendarInfo;

	class  CCalendarUI : public CVerticalLayoutUI
	{
	public:
		CCalendarUI(void);
		~CCalendarUI(void);

		LPCTSTR	GetClass() const;
		LPVOID	GetInterface(LPCTSTR pstrName);
		void	SetAttribute(LPCTSTR pstrName, LPCTSTR pstrValue);

		CControlUI* CreateStatusPanel();
		void SetWeekPanel(CControlUI* pControl,int nWeek);
		void SetDayPanel(COptionUI* pControl,bool bWeekEnd,TCalendarInfo* pCalendarInfo);
		void SetDayPanel(COptionUI* pControl,bool bWeekEnd,int nYear,int nMooth,int nDay,int nWeek,int nWeekLine,int nAsMooth);
		void UpdateMainTitle(int nYear,int nMooth);
	public:
		bool IsLeapYear(int nYear);
		int  DaysOfMonth(int nMooth,int nYear = -1);
		void ShowCalendar(int nYear,int nMooth);
		void SetDateTime(TCalendarInfo* pTCalendarInfo);
		void SetDateTime(int nYear,int nMooth,int nDay);
		CDuiString GetDateTime();
	public:
		void SetCalendarStyle(TCalendarStyle& CalendarStyle);

	public:
		void OnLastYear(TNotifyUI* pMsg);
		void OnMoothSelect(TNotifyUI* pMsg);
		void OnLastMonth(TNotifyUI* pMsg);
		void OnNextMonth(TNotifyUI* pMsg);
		void OnNextYear(TNotifyUI* pMsg);
		void OnSelectMooth(TNotifyUI* pMsg);
		void OnSelcetDay(TNotifyUI* pMsg);
		void OnToday(TNotifyUI* pMsg);

		bool Notify(LPVOID lParam);
		void OnClick(TNotifyUI* pMsg);
		void OnKillFocus(TNotifyUI* pMsg);
		void OnSelectChanged(TNotifyUI* pMsg);
	public:
		void SetEnabledMonthSel(bool bEnabled = true);
		bool GetEnabledMonthSel();
		void SetEnabledYearSel(bool bEnabled = true);
		bool GetEnabledYearSel();
		void SetMainTitleHeight(int nHeight);
		int GetMainTitleHeight();
		void SetSubTitleHeight(int nHeight);
		int GetSubTitleHeight();
		void SetStatusInfoHeight(int nHeight);
		int GetStatusInfoHeight();

		void ParseCalendarStyle(LPCTSTR lpszStyle,TCalendarStyle* pCalendarStyle = NULL);
	protected:
		void NormalizeTime(int& nYear,int& nMonth);
	private:
		CButtonUI*					m_pLastYear;					//ȥ��
		CButtonUI*					m_pLastMonth;				// ��һ��
		CLabelUI*					m_pMonthSelect;			//�·�ѡ��ť
		CButtonUI*					m_pNextMonth;				//��һ��
		CButtonUI*					m_pNextYear;					//����
		CHorizontalLayoutUI*	m_pMainTitleHoriz;			//�����⣬��ʾ���
		CHorizontalLayoutUI*	m_pSubTitleHoriz;			//�����⣬��ʾ����

		CVerticalLayoutUI*		m_pWeekPanelVert;		//������ʾ��
		CHorizontalLayoutUI*	m_pInfoPanelHorz;			//�ײ���Ϣ��
		CButtonUI*					m_pBtnToDay;					//

		int								m_nYear;								// �������
		int								m_nMonth;							//	�����·�
		int								m_nToday;							// ���ں���
		bool								pEnabledYearSel;
		bool								pEnabledMonthSel;

		SYSTEMTIME				m_LocalTime;
		TCalendarStyle			m_CalendarStyle;
	};
}
