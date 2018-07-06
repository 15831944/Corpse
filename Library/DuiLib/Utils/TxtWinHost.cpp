#include "StdAfx.h"
#include "TxtWinHost.h"

namespace DuiLib
{
	EXTERN_C const IID IID_ITextServices = { // 8d33f740-cf58-11ce-a89d-00aa006cadc5
		0x8d33f740,
		0xcf58,
		0x11ce,
		{0xa8, 0x9d, 0x00, 0xaa, 0x00, 0x6c, 0xad, 0xc5}
	};

	EXTERN_C const IID IID_ITextHost = { /* c5bdd8d0-d26e-11ce-a89e-00aa006cadc5 */
		0xc5bdd8d0,
		0xd26e,
		0x11ce,
		{0xa8, 0x9e, 0x00, 0xaa, 0x00, 0x6c, 0xad, 0xc5}
	};

	// Convert Pixels on the X axis to Himetric
	LONG DXtoHimetricX(LONG dx, LONG xPerInch)
	{
		return (LONG) MulDiv(dx, HIMETRIC_PER_INCH, xPerInch);
	}

	// Convert Pixels on the Y axis to Himetric
	LONG DYtoHimetricY(LONG dy, LONG yPerInch)
	{
		return (LONG) MulDiv(dy, HIMETRIC_PER_INCH, yPerInch);
	}

	HRESULT InitDefaultCharFormat(CControlUI* pControl, CHARFORMAT2W* pcf, HFONT hFont) 
	{
		CRichEditUI* pRich = (CRichEditUI*)pControl->GetInterface(DUI_CTR_RICHEDIT);
		if (pRich == NULL)
			return E_FAIL;

		memset(pcf, 0, sizeof(CHARFORMAT2W));
		LOGFONT lf;
		if (!hFont)
			hFont = pRich->GetManager()->GetFont(pRich->GetFont());
		::GetObject(hFont, sizeof(LOGFONT), &lf);

		DWORD dwColor = pRich->GetTextColor();
		if (pRich->GetManager()->IsLayered())
			CRenderEngine::CheckAlphaColor(dwColor);

		pcf->cbSize = sizeof(CHARFORMAT2W);
		pcf->crTextColor = RGB(GetBValue(dwColor), GetGValue(dwColor), GetRValue(dwColor));
		LONG yPixPerInch = GetDeviceCaps(pRich->GetManager()->GetPaintDC(), LOGPIXELSY);
		pcf->yHeight = -lf.lfHeight * LY_PER_INCH / yPixPerInch;
		pcf->yOffset = 0;
		pcf->dwEffects = 0;
		pcf->dwMask = CFM_SIZE | CFM_OFFSET | CFM_FACE | CFM_CHARSET | CFM_COLOR | CFM_BOLD | CFM_ITALIC | CFM_UNDERLINE;
		if(lf.lfWeight >= FW_BOLD)
			pcf->dwEffects |= CFE_BOLD;
		if(lf.lfItalic)
			pcf->dwEffects |= CFE_ITALIC;
		if(lf.lfUnderline)
			pcf->dwEffects |= CFE_UNDERLINE;
		pcf->bCharSet = lf.lfCharSet;
		pcf->bPitchAndFamily = lf.lfPitchAndFamily;
#ifdef _UNICODE
		_tcscpy(pcf->szFaceName, lf.lfFaceName);
#else
		//need to thunk pcf->szFaceName to a standard char string.in this case it's easy because our thunk is also our copy
		MultiByteToWideChar(CP_ACP, 0, lf.lfFaceName, LF_FACESIZE, pcf->szFaceName, LF_FACESIZE) ;
#endif

		return S_OK;
	}

	HRESULT InitDefaultParaFormat(CControlUI* pControl, PARAFORMAT2* ppf) 
	{	
		CRichEditUI* pRich = (CRichEditUI*)pControl->GetInterface(DUI_CTR_RICHEDIT);
		if (pRich == NULL)
			return E_FAIL;

		memset(ppf, 0, sizeof(PARAFORMAT2));
		ppf->cbSize = sizeof(PARAFORMAT2);
		ppf->dwMask = PFM_ALL;
		ppf->wAlignment = PFA_LEFT;
		ppf->cTabCount = 1;
		ppf->rgxTabs[0] = lDefaultTab;

		return S_OK;
	}

	HRESULT CreateHost(CControlUI* pControl, const CREATESTRUCT *pcs, CTxtWinHost **pptec)
	{
		HRESULT hRet = E_FAIL;
		//GdiSetBatchLimit(1);

		CTxtWinHost *phost = new CTxtWinHost();
		if(phost)
		{
			if (phost->Init(pControl, pcs))
			{
				*pptec = phost;
				hRet = S_OK;
			}
		}

		if (FAILED(hRet))
		{
			delete phost;
		}

		return TRUE;
	}

	CTxtWinHost::CTxtWinHost() 
		: m_pControl(NULL)
	{
		::ZeroMemory(&m_cRefs, sizeof(CTxtWinHost) - offsetof(CTxtWinHost, m_cRefs));
		cchTextMost = cInitTextMax;
		laccelpos = -1;
	}

	CTxtWinHost::~CTxtWinHost()
	{
		if (m_pTextSrv)
		{
			m_pTextSrv->OnTxInPlaceDeactivate();
			m_pTextSrv->Release();
		}
		
		if (m_pDocument)
			m_pDocument->Release();
	}

	////////////////////// Create/Init/Destruct Commands ///////////////////////

	BOOL CTxtWinHost::Init(CControlUI* pControl, const CREATESTRUCT *pcs)
	{
		BOOL bSuccess = FALSE;
		IUnknown *pUnk = NULL;
		HRESULT hRet = E_FAIL;

		do 
		{
			// ֻ��ʹ��RichEdit �� �����������ʼ��
			CRichEditUI* pRich = (CRichEditUI*)pControl->GetInterface(DUI_CTR_RICHEDIT);
			if (pRich == NULL)
				break;

			m_pControl = pControl;
			// Initialize Reference count
			m_cRefs = 1;

			// Create and cache CHARFORMAT for this control
			if (FAILED(InitDefaultCharFormat(pControl, &cf, NULL)))
				break;

			// Create and cache PARAFORMAT for this control
			if (FAILED(InitDefaultParaFormat(pControl, &pf)))
				break;

			// edit controls created without a window are multiline by default
			// so that paragraph formats can be
			dwStyle = ES_MULTILINE;
			// edit controls are rich by default
			fRich = pRich->IsRich();

			cchTextMost = pRich->GetLimitText();

			if (pcs) {
				dwStyle = pcs->style;

				if ((dwStyle & (ES_AUTOHSCROLL | WS_HSCROLL)) == FALSE)
					fWordWrap = TRUE;
			}

			if ((dwStyle & ES_LEFT) == FALSE)
			{
				if (dwStyle & ES_CENTER)
					pf.wAlignment = PFA_CENTER;
				else if (dwStyle & ES_RIGHT)
					pf.wAlignment = PFA_RIGHT;
			}

			fInplaceActive = TRUE;

			// Create Text Services component
			//if(FAILED(CreateTextServices(NULL, this, &pUnk)))
			//    goto err;

			PCreateTextServices pTextServicesProc;

			//����Win10ϵͳ��RichEdit�����ַ���ʾ���������
#ifdef _UNICODE		
			HMODULE hMod = LoadLibrary(_T("Msftedit.dll"));
#else
			HMODULE hMod = LoadLibrary(_T("Riched20.dll"));
#endif

			if (hMod == NULL)
				break;

			pTextServicesProc = (PCreateTextServices)GetProcAddress(hMod,"CreateTextServices");
			if (pTextServicesProc == NULL)
				break;

			hRet = pTextServicesProc(NULL, this, &pUnk);
			if (FAILED(hRet))
				break;
			
			hRet = pUnk->QueryInterface(IID_ITextServices, (void **)&m_pTextSrv);
			if (FAILED(hRet))
				break;

			hRet = pUnk->QueryInterface(__uuidof(ITextDocument), (void**)&m_pDocument);
			if (FAILED(hRet))
				break;
			// Whether the previous call succeeded or failed we are done
			// with the private interface.
			

			if (FAILED(hRet))
				break;

			// Set window text
			if(pcs && pcs->lpszName)
			{
#ifdef _UNICODE		
				if (FAILED(m_pTextSrv->TxSetText((TCHAR *)pcs->lpszName)))
					break;
#else
				size_t iLen = _tcslen(pcs->lpszName);
				LPWSTR lpText = new WCHAR[iLen + 1];
				::ZeroMemory(lpText, (iLen + 1) * sizeof(WCHAR));
				::MultiByteToWideChar(CP_ACP, 0, pcs->lpszName, -1, (LPWSTR)lpText, iLen) ;
				if(FAILED(pserv->TxSetText((LPWSTR)lpText))) {
					delete[] lpText;
					break;
				}
				delete[] lpText;
#endif
			}

			bSuccess = TRUE;
		} while (FALSE);		

		if (pUnk != NULL)
			pUnk->Release();

		return bSuccess;
	}

	/////////////////////////////////  IUnknown ////////////////////////////////


	HRESULT CTxtWinHost::QueryInterface(REFIID riid, void **ppvObject)
	{
		HRESULT hRet = E_NOINTERFACE;
		*ppvObject = NULL;

		if (IsEqualIID(riid, IID_IUnknown) || IsEqualIID(riid, IID_ITextHost)) 
		{
			AddRef();
			*ppvObject = (ITextHost *) this;
			hRet = S_OK;
		}

		return hRet;
	}

	ULONG CTxtWinHost::AddRef(void)
	{
		return ++m_cRefs;
	}

	ULONG CTxtWinHost::Release(void)
	{
		ULONG c_Refs = --m_cRefs;

		if (c_Refs == 0)
			delete this;

		return c_Refs;
	}

	/////////////////////////////////  Far East Support  //////////////////////////////////////

	HIMC CTxtWinHost::TxImmGetContext(void)
	{
		return NULL;
	}

	void CTxtWinHost::TxImmReleaseContext(HIMC himc)
	{
		//::ImmReleaseContext( hwnd, himc );
	}

	//////////////////////////// ITextHost Interface  ////////////////////////////

	HDC CTxtWinHost::TxGetDC()
	{
		return m_pControl->GetManager()->GetPaintDC();
	}

	int CTxtWinHost::TxReleaseDC(HDC hdc)
	{
		return 1;
	}

	BOOL CTxtWinHost::TxShowScrollBar(INT fnBar, BOOL fShow)
	{
		CRichEditUI* pRich = (CRichEditUI*)m_pControl->GetInterface(DUI_CTR_RICHEDIT);
		if (pRich == NULL)
			return FALSE;

		CScrollBarUI* pVerticalScrollBar = pRich->GetVerticalScrollBar();
		CScrollBarUI* pHorizontalScrollBar = pRich->GetHorizontalScrollBar();
		if( fnBar == SB_VERT && pVerticalScrollBar ) {
			pVerticalScrollBar->SetVisible(fShow == TRUE);
		}
		else if( fnBar == SB_HORZ && pHorizontalScrollBar ) {
			pHorizontalScrollBar->SetVisible(fShow == TRUE);
		}
		else if( fnBar == SB_BOTH ) {
			if( pVerticalScrollBar ) pVerticalScrollBar->SetVisible(fShow == TRUE);
			if( pHorizontalScrollBar ) pHorizontalScrollBar->SetVisible(fShow == TRUE);
		}
		return TRUE;
	}

	BOOL CTxtWinHost::TxEnableScrollBar (INT fuSBFlags, INT fuArrowflags)
	{
		CRichEditUI* pRich = (CRichEditUI*)m_pControl->GetInterface(DUI_CTR_RICHEDIT);
		if (pRich == NULL)
			return FALSE;

		if( fuSBFlags == SB_VERT ) {
			pRich->EnableScrollBar(true, pRich->GetHorizontalScrollBar() != NULL);
			pRich->GetVerticalScrollBar()->SetVisible(fuArrowflags != ESB_DISABLE_BOTH);
		}
		else if( fuSBFlags == SB_HORZ ) {
			pRich->EnableScrollBar(pRich->GetVerticalScrollBar() != NULL, true);
			pRich->GetHorizontalScrollBar()->SetVisible(fuArrowflags != ESB_DISABLE_BOTH);
		}
		else if( fuSBFlags == SB_BOTH ) {
			pRich->EnableScrollBar(true, true);
			pRich->GetVerticalScrollBar()->SetVisible(fuArrowflags != ESB_DISABLE_BOTH);
			pRich->GetHorizontalScrollBar()->SetVisible(fuArrowflags != ESB_DISABLE_BOTH);
		}
		return TRUE;
	}

	BOOL CTxtWinHost::TxSetScrollRange(INT fnBar, LONG nMinPos, INT nMaxPos, BOOL fRedraw)
	{
		CRichEditUI* pRich = (CRichEditUI*)m_pControl->GetInterface(DUI_CTR_RICHEDIT);
		if (pRich == NULL)
			return FALSE;

		CScrollBarUI* pVerticalScrollBar = pRich->GetVerticalScrollBar();
		CScrollBarUI* pHorizontalScrollBar = pRich->GetHorizontalScrollBar();
		if( fnBar == SB_VERT && pVerticalScrollBar ) {
			if( nMaxPos - nMinPos - rcClient.bottom + rcClient.top <= 0 ) {
				pVerticalScrollBar->SetVisible(false);
			}
			else {
				pVerticalScrollBar->SetVisible(true);
				pVerticalScrollBar->SetScrollRange(nMaxPos - nMinPos - rcClient.bottom + rcClient.top);
			}
		}
		else if( fnBar == SB_HORZ && pHorizontalScrollBar ) {
			if( nMaxPos - nMinPos - rcClient.right + rcClient.left <= 0 ) {
				pHorizontalScrollBar->SetVisible(false);
			}
			else {
				pHorizontalScrollBar->SetVisible(true);
				pHorizontalScrollBar->SetScrollRange(nMaxPos - nMinPos - rcClient.right + rcClient.left);
			}   
		}
		return TRUE;
	}

	BOOL CTxtWinHost::TxSetScrollPos (INT fnBar, INT nPos, BOOL fRedraw)
	{
		CRichEditUI* pRich = (CRichEditUI*)m_pControl->GetInterface(DUI_CTR_RICHEDIT);
		if (pRich == NULL)
			return FALSE;

		CScrollBarUI* pVerticalScrollBar = pRich->GetVerticalScrollBar();
		CScrollBarUI* pHorizontalScrollBar = pRich->GetHorizontalScrollBar();
		SIZE szPos = {0};
		if( fnBar == SB_VERT && pVerticalScrollBar ) {
			//pVerticalScrollBar->SetScrollPos(nPos);
			szPos.cy = nPos;
			pRich->SetScrollPos(szPos);
		}
		else if( fnBar == SB_HORZ && pHorizontalScrollBar ) {
			//pHorizontalScrollBar->SetScrollPos(nPos);
			szPos.cx = nPos;
			pRich->SetScrollPos(szPos);
		}
		return TRUE;
	}

	void CTxtWinHost::TxInvalidateRect(LPCRECT prc, BOOL fMode)
	{
		if( prc == NULL ) {
			m_pControl->GetManager()->Invalidate(rcClient);
			return;
		}
		RECT rc = *prc;
		m_pControl->GetManager()->Invalidate(rc);
	}

	void CTxtWinHost::TxViewChange(BOOL fUpdate) 
	{
		CRichEditUI* pRich = (CRichEditUI*)m_pControl->GetInterface(DUI_CTR_RICHEDIT);

		if(pRich != NULL && pRich->OnTxViewChanged() ) m_pControl->Invalidate();
	}

	BOOL CTxtWinHost::TxCreateCaret(HBITMAP hbmp, INT xWidth, INT yHeight)
	{
		CRichEditUI* pRich = (CRichEditUI*)m_pControl->GetInterface(DUI_CTR_RICHEDIT);

		if (pRich != NULL && pRich->GetManager()->IsLayered())
			return pRich->CreateCaret(xWidth,yHeight);
		return ::CreateCaret(m_pControl->GetManager()->GetPaintWindow(), hbmp, xWidth, yHeight);
	}

	BOOL CTxtWinHost::TxShowCaret(BOOL fShow)
	{
		CRichEditUI* pRich = (CRichEditUI*)m_pControl->GetInterface(DUI_CTR_RICHEDIT);

		if (pRich != NULL && pRich->GetManager()->IsLayered())
			return	pRich->ShowCaret(fShow);
		if(fShow)
			return ::ShowCaret(m_pControl->GetManager()->GetPaintWindow());
		else
			return ::HideCaret(m_pControl->GetManager()->GetPaintWindow());
	}

	BOOL CTxtWinHost::TxSetCaretPos(INT x, INT y)
	{
		CRichEditUI* pRich = (CRichEditUI*)m_pControl->GetInterface(DUI_CTR_RICHEDIT);
		if (pRich != NULL && pRich->GetManager()->IsLayered())
			pRich->SetCaretPos(x,y);
		return ::SetCaretPos(x, y);
	}

	BOOL CTxtWinHost::TxSetTimer(UINT idTimer, UINT uTimeout)
	{
		fTimer = TRUE;
		return m_pControl->GetManager()->SetTimer(m_pControl, idTimer, uTimeout) == TRUE;
	}

	void CTxtWinHost::TxKillTimer(UINT idTimer)
	{
		m_pControl->GetManager()->KillTimer(m_pControl, idTimer);
		fTimer = FALSE;
	}

	void CTxtWinHost::TxScrollWindowEx (INT dx, INT dy, LPCRECT lprcScroll,	LPCRECT lprcClip,	HRGN hrgnUpdate, LPRECT lprcUpdate,	UINT fuScroll)	
	{
		return;
	}

	void CTxtWinHost::TxSetCapture(BOOL fCapture)
	{
		if (fCapture) m_pControl->GetManager()->SetCapture();
		else m_pControl->GetManager()->ReleaseCapture();
		fCaptured = fCapture;
	}

	void CTxtWinHost::TxSetFocus()
	{
		m_pControl->SetFocus();
	}

	void CTxtWinHost::TxSetCursor(HCURSOR hcur,	BOOL fText)
	{
		::SetCursor(hcur);
	}

	BOOL CTxtWinHost::TxScreenToClient(LPPOINT lppt)
	{
		return ::ScreenToClient(m_pControl->GetManager()->GetPaintWindow(), lppt);	
	}

	BOOL CTxtWinHost::TxClientToScreen(LPPOINT lppt)
	{
		return ::ClientToScreen(m_pControl->GetManager()->GetPaintWindow(), lppt);
	}

	HRESULT CTxtWinHost::TxActivate(LONG *plOldState)
	{
		return S_OK;
	}

	HRESULT CTxtWinHost::TxDeactivate(LONG lNewState)
	{
		return S_OK;
	}

	HRESULT CTxtWinHost::TxGetClientRect(LPRECT prc)
	{
		*prc = rcClient;
		GetControlRect(prc);
		return NOERROR;
	}

	HRESULT CTxtWinHost::TxGetViewInset(LPRECT prc) 
	{
		prc->left = prc->right = prc->top = prc->bottom = 0;
		return NOERROR;	
	}

	HRESULT CTxtWinHost::TxGetCharFormat(const CHARFORMATW **ppCF)
	{
		*ppCF = &cf;
		return NOERROR;
	}

	HRESULT CTxtWinHost::TxGetParaFormat(const PARAFORMAT **ppPF)
	{
		*ppPF = &pf;
		return NOERROR;
	}

	COLORREF CTxtWinHost::TxGetSysColor(int nIndex) 
	{
		DWORD dwColor = ::GetSysColor(nIndex);
		CRenderEngine::CheckAlphaColor(dwColor);
		return dwColor;
	}

	HRESULT CTxtWinHost::TxGetBackStyle(TXTBACKSTYLE *pstyle)
	{
		*pstyle = !fTransparent ? TXTBACK_OPAQUE : TXTBACK_TRANSPARENT;
		return NOERROR;
	}

	HRESULT CTxtWinHost::TxGetMaxLength(DWORD *pLength)
	{
		*pLength = cchTextMost;
		return NOERROR;
	}

	HRESULT CTxtWinHost::TxGetScrollBars(DWORD *pdwScrollBar)
	{
		*pdwScrollBar =  dwStyle & (WS_VSCROLL | WS_HSCROLL | ES_AUTOVSCROLL | 
			ES_AUTOHSCROLL | ES_DISABLENOSCROLL);

		return NOERROR;
	}

	HRESULT CTxtWinHost::TxGetPasswordChar(TCHAR *pch)
	{
#ifdef _UNICODE
		*pch = chPasswordChar;
#else
		::WideCharToMultiByte(CP_ACP, 0, &chPasswordChar, 1, pch, 1, NULL, NULL) ;
#endif
		return NOERROR;
	}

	HRESULT CTxtWinHost::TxGetAcceleratorPos(LONG *pcp)
	{
		*pcp = laccelpos;
		return S_OK;
	} 										   

	HRESULT CTxtWinHost::OnTxCharFormatChange(const CHARFORMATW *pcf)
	{
		return S_OK;
	}

	HRESULT CTxtWinHost::OnTxParaFormatChange(const PARAFORMAT *ppf)
	{
		return S_OK;
	}

	HRESULT CTxtWinHost::TxGetPropertyBits(DWORD dwMask, DWORD *pdwBits) 
	{
		DWORD dwProperties = 0;

		if (fRich)
		{
			dwProperties = TXTBIT_RICHTEXT;
		}

		if (dwStyle & ES_MULTILINE)
		{
			dwProperties |= TXTBIT_MULTILINE;
		}

		if (dwStyle & ES_READONLY)
		{
			dwProperties |= TXTBIT_READONLY;
		}

		if (dwStyle & ES_PASSWORD)
		{
			dwProperties |= TXTBIT_USEPASSWORD;
		}

		if (!(dwStyle & ES_NOHIDESEL))
		{
			dwProperties |= TXTBIT_HIDESELECTION;
		}

		if (fEnableAutoWordSel)
		{
			dwProperties |= TXTBIT_AUTOWORDSEL;
		}

		if (fWordWrap)
		{
			dwProperties |= TXTBIT_WORDWRAP;
		}

		if (fAllowBeep)
		{
			dwProperties |= TXTBIT_ALLOWBEEP;
		}

		if (fSaveSelection)
		{
			dwProperties |= TXTBIT_SAVESELECTION;
		}

		*pdwBits = dwProperties & dwMask; 
		return NOERROR;
	}


	HRESULT CTxtWinHost::TxNotify(DWORD iNotify, void *pv)
	{
		// ����û�ж� m_pControl�ؼ���Ч�Խ����ж�
		CRichEditUI* pRich = (CRichEditUI*)m_pControl->GetInterface(DUI_CTR_RICHEDIT);
		
		if( iNotify == EN_REQUESTRESIZE ) {
			RECT rc;
			REQRESIZE *preqsz = (REQRESIZE *)pv;
			GetControlRect(&rc);
			rc.bottom = rc.top + preqsz->rc.bottom;
			rc.right  = rc.left + preqsz->rc.right;
			SetClientRect(&rc);
		}
		else if (iNotify == EN_LINK){
			ENLINK* l_pENLink = (ENLINK*)pv;
			if (l_pENLink->msg == WM_LBUTTONDOWN || l_pENLink->msg == WM_LBUTTONDBLCLK)
			{
				CHARRANGE crCharRange;
				pRich->GetSel(crCharRange);//�õ�ԭ��ѡ�е��ַ���
				pRich->SetSel(l_pENLink->chrg);
				CDuiString strSelText = pRich->GetSelText();
				pRich->SetSel(crCharRange);
			}
		}
		pRich->OnTxNotify(iNotify, pv);
		return S_OK;
	}

	HRESULT CTxtWinHost::TxGetExtent(LPSIZEL lpExtent)
	{
		*lpExtent = sizelExtent;
		return S_OK;
	}

	HRESULT	CTxtWinHost::TxGetSelectionBarWidth (LONG *plSelBarWidth)
	{
		*plSelBarWidth = lSelBarWidth;
		return S_OK;
	}

	void CTxtWinHost::SetWordWrap(BOOL fWordWrap)
	{
		fWordWrap = fWordWrap;
		if (m_pTextSrv)
			m_pTextSrv->OnTxPropertyBitsChange(TXTBIT_WORDWRAP, fWordWrap ? TXTBIT_WORDWRAP : 0);
	}

	BOOL CTxtWinHost::GetReadOnly()
	{
		return (dwStyle & ES_READONLY) != 0;
	}

	void CTxtWinHost::SetReadOnly(BOOL fReadOnly)
	{
		if (fReadOnly)
			dwStyle |= ES_READONLY;
		else
			dwStyle &= ~ES_READONLY;

		if (m_pTextSrv)
			m_pTextSrv->OnTxPropertyBitsChange(TXTBIT_READONLY, fReadOnly ? TXTBIT_READONLY : 0);
	}

	void CTxtWinHost::SetFont(HFONT hFont) 
	{
		if( hFont == NULL ) return;
		LOGFONT lf;
		::GetObject(hFont, sizeof(LOGFONT), &lf);
		LONG yPixPerInch = ::GetDeviceCaps(m_pControl->GetManager()->GetPaintDC(), LOGPIXELSY);
		cf.yHeight = -lf.lfHeight * LY_PER_INCH / yPixPerInch;
		if(lf.lfWeight >= FW_BOLD)
			cf.dwEffects |= CFE_BOLD;
		if(lf.lfItalic)
			cf.dwEffects |= CFE_ITALIC;
		if(lf.lfUnderline)
			cf.dwEffects |= CFE_UNDERLINE;
		cf.bCharSet = lf.lfCharSet;
		cf.bPitchAndFamily = lf.lfPitchAndFamily;
#ifdef _UNICODE
		_tcscpy(cf.szFaceName, lf.lfFaceName);
#else
		//need to thunk pcf->szFaceName to a standard char string.in this case it's easy because our thunk is also our copy
		MultiByteToWideChar(CP_ACP, 0, lf.lfFaceName, LF_FACESIZE, cf.szFaceName, LF_FACESIZE) ;
#endif

		if (m_pTextSrv)
			m_pTextSrv->OnTxPropertyBitsChange(TXTBIT_CHARFORMATCHANGE, TXTBIT_CHARFORMATCHANGE);
	}

	void CTxtWinHost::SetColor(DWORD dwColor)
	{
		CRenderEngine::CheckAlphaColor(dwColor);
		cf.crTextColor = RGB(GetBValue(dwColor), GetGValue(dwColor), GetRValue(dwColor));
		if (m_pTextSrv)
			m_pTextSrv->OnTxPropertyBitsChange(TXTBIT_CHARFORMATCHANGE, TXTBIT_CHARFORMATCHANGE);
	}

	SIZEL* CTxtWinHost::GetExtent() 
	{
		return &sizelExtent;
	}

	void CTxtWinHost::SetExtent(SIZEL *psizelExtent) 
	{ 
		sizelExtent = *psizelExtent; 
		if (m_pTextSrv)
			m_pTextSrv->OnTxPropertyBitsChange(TXTBIT_EXTENTCHANGE, TXTBIT_EXTENTCHANGE);
	}

	void CTxtWinHost::LimitText(LONG nChars)
	{
		cchTextMost = nChars;
		if( cchTextMost <= 0 ) cchTextMost = cInitTextMax;
		if (m_pTextSrv)
			m_pTextSrv->OnTxPropertyBitsChange(TXTBIT_MAXLENGTHCHANGE, TXTBIT_MAXLENGTHCHANGE);
	}

	BOOL CTxtWinHost::IsCaptured()
	{
		return fCaptured;
	}

	BOOL CTxtWinHost::GetAllowBeep()
	{
		return fAllowBeep;
	}

	void CTxtWinHost::SetAllowBeep(BOOL fAllowBeep)
	{
		fAllowBeep = fAllowBeep;

		if (m_pTextSrv)
			m_pTextSrv->OnTxPropertyBitsChange(TXTBIT_ALLOWBEEP, fAllowBeep ? TXTBIT_ALLOWBEEP : 0);
	}

	WORD CTxtWinHost::GetDefaultAlign()
	{
		return pf.wAlignment;
	}

	void CTxtWinHost::SetDefaultAlign(WORD wNewAlign)
	{
		pf.wAlignment = wNewAlign;

		// Notify control of property change
		if (m_pTextSrv)
			m_pTextSrv->OnTxPropertyBitsChange(TXTBIT_PARAFORMATCHANGE, 0);
	}

	BOOL CTxtWinHost::GetRichTextFlag()
	{
		return fRich;
	}

	void CTxtWinHost::SetRichTextFlag(BOOL fNew)
	{
		fRich = fNew;
		if (m_pTextSrv)
			m_pTextSrv->OnTxPropertyBitsChange(TXTBIT_RICHTEXT, fNew ? TXTBIT_RICHTEXT : 0);
	}

	LONG CTxtWinHost::GetDefaultLeftIndent()
	{
		return pf.dxOffset;
	}

	void CTxtWinHost::SetDefaultLeftIndent(LONG lNewIndent)
	{
		pf.dxOffset = lNewIndent;
		if (m_pTextSrv)
			m_pTextSrv->OnTxPropertyBitsChange(TXTBIT_PARAFORMATCHANGE, 0);
	}

	void CTxtWinHost::SetClientRect(RECT *prc) 
	{
		rcClient = *prc;

		LONG xPerInch = ::GetDeviceCaps(m_pControl->GetManager()->GetPaintDC(), LOGPIXELSX); 
		LONG yPerInch =	::GetDeviceCaps(m_pControl->GetManager()->GetPaintDC(), LOGPIXELSY); 
		sizelExtent.cx = DXtoHimetricX(rcClient.right - rcClient.left, xPerInch);
		sizelExtent.cy = DYtoHimetricY(rcClient.bottom - rcClient.top, yPerInch);

		if (m_pTextSrv)
			m_pTextSrv->OnTxPropertyBitsChange(TXTBIT_VIEWINSETCHANGE, TXTBIT_VIEWINSETCHANGE);
	}

	BOOL CTxtWinHost::SetSaveSelection(BOOL f_SaveSelection)
	{
		BOOL fResult = f_SaveSelection;

		fSaveSelection = f_SaveSelection;

		// notify text services of property change
		if (m_pTextSrv)
			m_pTextSrv->OnTxPropertyBitsChange(TXTBIT_SAVESELECTION, fSaveSelection ? TXTBIT_SAVESELECTION : 0);

		return fResult;		
	}

	HRESULT	CTxtWinHost::OnTxInPlaceDeactivate()
	{
		HRESULT hRet = m_pTextSrv->OnTxInPlaceDeactivate();

		if (SUCCEEDED(hRet))
			fInplaceActive = FALSE;

		return hRet;
	}

	HRESULT	CTxtWinHost::OnTxInPlaceActivate(LPCRECT prcClient)
	{
		fInplaceActive = TRUE;

		HRESULT hRet = m_pTextSrv->OnTxInPlaceActivate(prcClient);

		if (FAILED(hRet))
			fInplaceActive = FALSE;

		return hRet;
	}

	BOOL CTxtWinHost::DoSetCursor(RECT *prc, POINT *pt)
	{
		RECT rc = prc ? *prc : rcClient;

		// Is this in our rectangle?
		if (PtInRect(&rc, *pt))
		{
			RECT *prcClient = (!fInplaceActive || prc) ? &rc : NULL;
			m_pTextSrv->OnTxSetCursor(DVASPECT_CONTENT,	-1, NULL, NULL,  m_pControl->GetManager()->GetPaintDC(),
				NULL, prcClient, pt->x, pt->y);

			return TRUE;
		}

		return FALSE;
	}

	void CTxtWinHost::GetControlRect(LPRECT prc)
	{
		prc->top = rcClient.top;
		prc->bottom = rcClient.bottom;
		prc->left = rcClient.left;
		prc->right = rcClient.right;
	}

	void CTxtWinHost::SetTransparent(BOOL f_Transparent)
	{
		fTransparent = f_Transparent;

		// notify text services of property change
		if (m_pTextSrv)
			m_pTextSrv->OnTxPropertyBitsChange(TXTBIT_BACKSTYLECHANGE, 0);
	}

	LONG CTxtWinHost::SetAccelPos(LONG l_accelpos)
	{
		LONG laccelposOld = l_accelpos;

		laccelpos = l_accelpos;

		// notify text services of property change
		if (m_pTextSrv)
			m_pTextSrv->OnTxPropertyBitsChange(TXTBIT_SHOWACCELERATOR, 0);

		return laccelposOld;
	}

	WCHAR CTxtWinHost::SetPasswordChar(WCHAR ch_PasswordChar)
	{
		WCHAR chOldPasswordChar = chPasswordChar;

		chPasswordChar = ch_PasswordChar;

		// notify text services of property change
		if (m_pTextSrv)
			m_pTextSrv->OnTxPropertyBitsChange(TXTBIT_USEPASSWORD, (chPasswordChar != 0) ? TXTBIT_USEPASSWORD : 0);

		return chOldPasswordChar;
	}

	void CTxtWinHost::SetDisabled(BOOL fOn)
	{
		cf.dwMask	 |= CFM_COLOR | CFM_DISABLED;
		cf.dwEffects |= CFE_AUTOCOLOR | CFE_DISABLED;

		if( !fOn )
		{
			cf.dwEffects &= ~CFE_DISABLED;
		}

		if (m_pTextSrv)
			m_pTextSrv->OnTxPropertyBitsChange(TXTBIT_CHARFORMATCHANGE, TXTBIT_CHARFORMATCHANGE);
	}

	LONG CTxtWinHost::SetSelBarWidth(LONG l_SelBarWidth)
	{
		LONG lOldSelBarWidth = lSelBarWidth;

		lSelBarWidth = l_SelBarWidth;

		if (lSelBarWidth)
			dwStyle |= ES_SELECTIONBAR;
		else
			dwStyle &= (~ES_SELECTIONBAR);

		if (m_pTextSrv)
			m_pTextSrv->OnTxPropertyBitsChange(TXTBIT_SELBARCHANGE, TXTBIT_SELBARCHANGE);

		return lOldSelBarWidth;
	}

	BOOL CTxtWinHost::GetTimerState()
	{
		return fTimer;
	}

	void CTxtWinHost::SetCharFormat(CHARFORMAT2W &c)
	{
		cf = c;
	}

	void CTxtWinHost::SetParaFormat(PARAFORMAT2 &p)
	{
		pf = p;
	}
}