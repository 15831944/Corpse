#include "stdafx.h"
#include "CityWeatherInfo.h"

#define CITY_WEATHER_INFO_TIMED_ID		0x1001

CCityWeatherInfo::CCityWeatherInfo(HWND hParent, CCityHelper* pCityInfo)
{
	m_pCityInfo = pCityInfo;
	m_hParent = hParent;
	Create(hParent, _T("CityWeatherInfo"), UI_WNDSTYLE_DIALOG, WS_EX_TOOLWINDOW, 0, 0, 0, 0);
}

CCityWeatherInfo::~CCityWeatherInfo()
{

}

void CCityWeatherInfo::OnFinalMessage( HWND hWnd )
{
	WindowImplBase::OnFinalMessage(hWnd);
	delete this;
}

LPCTSTR CCityWeatherInfo::GetWindowClassName() const
{
	return _T("CityWeatherInfo");
}

CDuiString CCityWeatherInfo::GetSkinFile()
{
	return _T("WeatherInfo.xml");
}

CDuiString CCityWeatherInfo::GetZIPFileName() const
{
	return _T("");
}

CDuiString CCityWeatherInfo::GetSkinFolder()
{
	return _T("TTimerHelper");
}

UILIB_RESOURCETYPE CCityWeatherInfo::GetResourceType() const
{
	return UILIB_FILE;
}

void CCityWeatherInfo::Notify(TNotifyUI& msg)
{
	if (msg.sType == DUI_MSGTYPE_TIMER)
		OnTimer(msg);
}

void CCityWeatherInfo::InitWindow()
{
	CControlUI* pLayout = m_PaintManager.FindControl(_T("HLayoutTotal"));
	if (pLayout != NULL)
		m_PaintManager.SetTimer(pLayout, CITY_WEATHER_INFO_TIMED_ID, 500);

	if (m_pCityInfo != NULL)
	{
		//显示当前城市位置
		CString strCityLocation;
		m_pCityInfo->GetCityLocation(strCityLocation);
		CLabelUI* pLocation = (CLabelUI*)m_PaintManager.FindControl(_T("Location"));
		if (pLocation && strCityLocation.IsEmpty() == FALSE)
			pLocation->SetText(strCityLocation);

		//显示当前城市天气温度范围
		CString strCityTemperatureRange;
		m_pCityInfo->GetCityTemperatureRange(strCityTemperatureRange);
		CLabelUI* pRange = (CLabelUI*)m_PaintManager.FindControl(_T("Range"));
		if (pRange && strCityTemperatureRange.IsEmpty() == FALSE)
			pRange->SetText(strCityTemperatureRange);

		//显示当前城市天气文字信息
		CString strCityWeatherInfo;
		m_pCityInfo->GetCityWeatherInfo(strCityWeatherInfo);
		CLabelUI* pWeatherInfo = (CLabelUI*)m_PaintManager.FindControl(_T("WeatherInfo"));
		if (pWeatherInfo && strCityWeatherInfo.IsEmpty() == FALSE)
			pWeatherInfo->SetText(strCityWeatherInfo);

		//显示当前城市天气图标
		CLabelUI* pWeatherType = (CLabelUI*)m_PaintManager.FindControl(_T("ImgType"));
		if (pWeatherType)
		{
			TCHAR szWeatherInfoType[MAX_PATH];
			_stprintf_s(szWeatherInfoType, _countof(szWeatherInfoType), _T("Info/%s.png"), strCityWeatherInfo);
			pWeatherType->SetBkImage(szWeatherInfoType);
		}

		//显示当前城市PM2.5具体信息
		CString strCityPM25Info, strCityPM25Remark;
		m_pCityInfo->GetCityPM25Info(strCityPM25Info);
		m_pCityInfo->GetCityPM25Remark(strCityPM25Remark);
		CLabelUI* pPM25Info = (CLabelUI*)m_PaintManager.FindControl(_T("PM25Info"));
		if (pPM25Info && strCityPM25Info.IsEmpty() == FALSE)
		{
			if (strCityPM25Info.GetLength() > 6)
			{
				CLabelUI* pTitle = (CLabelUI*)m_PaintManager.FindControl(_T("PM25Title"));
				if (pTitle)
					pTitle->SetVisible(FALSE);
			}
			pPM25Info->SetText(strCityPM25Info);
			pPM25Info->SetToolTip(strCityPM25Remark);
		}
	}

	RECT rcWnd;
	GetWindowRect(m_hParent, &rcWnd);

	SetWindowPos(m_hWnd, HWND_TOPMOST, rcWnd.left, rcWnd.bottom, 0, 0, SWP_NOACTIVATE | SWP_NOSIZE | SWP_SHOWWINDOW);
}

LRESULT CCityWeatherInfo::OnKillFocus(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	bHandled = TRUE;
	Close(IDOK);

	return 0;
}

void CCityWeatherInfo::OnTimer(TNotifyUI& msg)
{
	if (msg.wParam == CITY_WEATHER_INFO_TIMED_ID)
	{
		POINT pt;
		GetCursorPos(&pt);
		//不在父窗口并且不在本窗口
		HWND hWnd = m_hWnd;
		do 
		{
			RECT rcWnd;
			GetWindowRect(hWnd, &rcWnd);
			if (::PtInRect(&rcWnd, pt))
				break;

			//如果都已经循环到父窗口了，还是没有检测到鼠标在指定区域内，则认为鼠标在指定区域外
			if (hWnd == m_hParent)
			{
				Close(IDCANCEL);
				m_PaintManager.KillTimer(msg.pSender);
				break;
			}

			hWnd = GetParent(hWnd);
		} while (hWnd != NULL);
	}
}