// CherryLinkCtrl.cpp : ���� �����Դϴ�.
//

#include "stdafx.h"
#include "CherryLinkCtrl.h"


// CCherryLinkCtrl

IMPLEMENT_DYNAMIC(CCherryLinkCtrl, CCherryStatic)

CCherryLinkCtrl::CCherryLinkCtrl()
{
	m_bHover = FALSE;
	m_bTracking = FALSE;
	m_bEnableHoverHandCursor = TRUE;
}

CCherryLinkCtrl::~CCherryLinkCtrl()
{
}

// CCherryLinkCtrl �޽��� ó�����Դϴ�.

BEGIN_MESSAGE_MAP(CCherryLinkCtrl, CCherryStatic)
	ON_WM_MOUSEMOVE()
	ON_WM_MOUSEHOVER()
	ON_WM_MOUSELEAVE()
	ON_WM_LBUTTONDOWN()
	ON_WM_LBUTTONUP()
	ON_WM_ENABLE()
END_MESSAGE_MAP()

CHERRY_RET CCherryLinkCtrl::Create(LPCTSTR lpszText,
							 DWORD dwCherryStyle,
							 DWORD dwStyle, 
							 const CCherryRect &cherryRect,
							 CWnd *pParentWnd, 
							 UINT nID)
{
	CHERRY_RET cherryRet = CCherryException::ERROR_CHERRY_SUCCESS;

	try
	{
		if (CCherryStatic::Create(lpszText, dwCherryStyle, dwStyle, cherryRect, pParentWnd, nID) != CCherryException::ERROR_CHERRY_SUCCESS)
			throw CCherryException::ERROR_LINKCTRL_CREATE_FAIL;

		// ��ũ ��Ʈ�� �⺻ ��Ʈ ��Ÿ�� ����
		COLORREF color = GetSysColor(COLOR_HOTLIGHT);
		BYTE r = (BYTE)GetRValue(color);
		BYTE g = (BYTE)GetGValue(color);
		BYTE b = (BYTE)GetBValue(color);

		SetNormalFontColor(Color(r, g, b));
		SetHoverFontColor(Color(r, g, b));
		SetHoverFontStyle(STYLE_UNDERLINE);
		SetDownFontColor(Color(r, g, b));
		SetDownFontStyle(STYLE_UNDERLINE);

		color = GetSysColor(COLOR_GRAYTEXT);
		r = (BYTE)GetRValue(color);
		g = (BYTE)GetGValue(color);
		b = (BYTE)GetBValue(color);
		SetDisableFontColor(Color(r, g, b));
	}
	catch (const CHERRY_RET &errorRet)
	{
		cherryRet = errorRet;
	}

	return cherryRet;
}

void CCherryLinkCtrl::SetCherryFont(LPCTSTR lpszFontName, float fFontSize, Color fontColor, DWORD dwFontStyle)
{
	// ��ũ ��Ʈ�� �⺻ ��Ʈ ��Ÿ�� ����
	SetNormalFont(lpszFontName, fFontSize, fontColor, dwFontStyle);
	SetHoverFont(lpszFontName, fFontSize, fontColor, STYLE_UNDERLINE);
	SetDownFont(lpszFontName, fFontSize, fontColor, STYLE_UNDERLINE);

	COLORREF disableColor = GetSysColor(COLOR_GRAYTEXT);
	SetDisableFont(lpszFontName, fFontSize, Color(GetRValue(disableColor), GetGValue(disableColor), GetBValue(disableColor)), dwFontStyle);

	// CherryCheckBox::STYLE_AUTORESIZE �����Ǿ� �ִٸ� ������ Resize
	if (GetCherryStyle() & STYLE_AUTORESIZE)
		ResizeWindow();	
}

BOOL CCherryLinkCtrl::SetCherryFont(CCherryFont *pCherryFont)
{
	if (NULL == pCherryFont)
		return FALSE;

	SetCherryFont(pCherryFont->GetFontName(), pCherryFont->GetFontSize(), pCherryFont->GetFontColor(), pCherryFont->GetFontStyle());

	return TRUE;
}

void CCherryLinkCtrl::SetFontByStatus(CCherryFont *pNormalFont, CCherryFont *pHoverFont, CCherryFont *pDownFont, CCherryFont *pDisableFont)
{
	SetNormalFont(pNormalFont);
	SetHoverFont(pHoverFont);
	SetDownFont(pDownFont);
	SetDisableFont(pDisableFont);
}

// �� ��Ʈ ������ ���� �������� ����� �����Ǿ� �ִ� ��� �������̵� �Ͽ� �������� �Ѵ�.
void CCherryLinkCtrl::SetNormalFont(LPCTSTR lpszFontName, float fFontSize, Color fontColor, DWORD dwFontStyle)
{
	CCherryFontByStatus::SetNormalFont(lpszFontName, fFontSize, fontColor, dwFontStyle);

	if (GetCherryStyle() & STYLE_AUTORESIZE)
		ResizeWindow();

	Invalidate();
}

BOOL CCherryLinkCtrl::SetNormalFont(CCherryFont *pNormalFont)
{
	if (!CCherryFontByStatus::SetNormalFont(pNormalFont))
		return FALSE;

	if (GetCherryStyle() & STYLE_AUTORESIZE)
		ResizeWindow();

	Invalidate();

	return TRUE;
}

void CCherryLinkCtrl::SetNormalFontName(LPCTSTR lpszFontName)
{
	CCherryFontByStatus::SetNormalFontName(lpszFontName);
	
	// STYLE_AUTORESIZE �����Ǿ� �ִٸ� ������ Resize
	if (GetCherryStyle() & STYLE_AUTORESIZE)
		ResizeWindow();	
}

void CCherryLinkCtrl::SetNormalFontSize(float fFontSize)
{
	CCherryFontByStatus::SetNormalFontSize(fFontSize);

	// STYLE_AUTORESIZE �����Ǿ� �ִٸ� ������ Resize
	if (GetCherryStyle() & STYLE_AUTORESIZE)
		ResizeWindow();	
}

void CCherryLinkCtrl::SetNormalFontColor(Color fontColor)
{
	CCherryFontByStatus::SetNormalFontColor(fontColor);
	// ������ ������ ������ �̷������ �������� �������� �� �ʿ䰡 ����.
}

void CCherryLinkCtrl::SetNormalFontStyle(DWORD dwFontStyle)
{
	CCherryFontByStatus::SetNormalFontStyle(dwFontStyle);

	// STYLE_AUTORESIZE �����Ǿ� �ִٸ� ������ Resize
	if (GetCherryStyle() & STYLE_AUTORESIZE)
		ResizeWindow();	
}

void CCherryLinkCtrl::SetHoverFont(LPCTSTR lpszFontName, float fFontSize, Color fontColor, DWORD dwFontStyle)
{
	CCherryFontByStatus::SetHoverFont(lpszFontName, fFontSize, fontColor, dwFontStyle);

	if (GetCherryStyle() & STYLE_AUTORESIZE)
		ResizeWindow();
	
	Invalidate();
}

BOOL CCherryLinkCtrl::SetHoverFont(CCherryFont *pHoverFont)
{
	if (!CCherryFontByStatus::SetHoverFont(pHoverFont))
		return FALSE;

	if (GetCherryStyle() & STYLE_AUTORESIZE)
		ResizeWindow();

	Invalidate();

	return TRUE;
}

void CCherryLinkCtrl::SetHoverFontName(LPCTSTR lpszFontName)
{
	CCherryFontByStatus::SetHoverFontName(lpszFontName);
	
	// STYLE_AUTORESIZE �����Ǿ� �ִٸ� ������ Resize
	if (GetCherryStyle() & STYLE_AUTORESIZE)
		ResizeWindow();	
}

void CCherryLinkCtrl::SetHoverFontSize(float fFontSize)
{
	CCherryFontByStatus::SetHoverFontSize(fFontSize);

	// STYLE_AUTORESIZE �����Ǿ� �ִٸ� ������ Resize
	if (GetCherryStyle() & STYLE_AUTORESIZE)
		ResizeWindow();	
}

void CCherryLinkCtrl::SetHoverFontColor(Color fontColor)
{
	CCherryFontByStatus::SetHoverFontColor(fontColor);
	// ������ ������ ������ �̷������ �������� �������� �� �ʿ䰡 ����.
}

void CCherryLinkCtrl::SetHoverFontStyle(DWORD dwFontStyle)
{
	CCherryFontByStatus::SetHoverFontStyle(dwFontStyle);

	// STYLE_AUTORESIZE �����Ǿ� �ִٸ� ������ Resize
	if (GetCherryStyle() & STYLE_AUTORESIZE)
		ResizeWindow();	
}

void CCherryLinkCtrl::SetDownFont(LPCTSTR lpszFontName, float fFontSize, Color fontColor, DWORD dwFontStyle)
{
	CCherryFontByStatus::SetDownFont(lpszFontName, fFontSize, fontColor, dwFontStyle);

	if (GetCherryStyle() & STYLE_AUTORESIZE)
		ResizeWindow();

	Invalidate();
}

BOOL CCherryLinkCtrl::SetDownFont(CCherryFont *pDownFont)
{
	if (!CCherryFontByStatus::SetDownFont(pDownFont))
		return FALSE;

	if (GetCherryStyle() & STYLE_AUTORESIZE)
		ResizeWindow();

	Invalidate();

	return TRUE;
}

void CCherryLinkCtrl::SetDownFontName(LPCTSTR lpszFontName)
{
	CCherryFontByStatus::SetDownFontName(lpszFontName);
	
	// STYLE_AUTORESIZE �����Ǿ� �ִٸ� ������ Resize
	if (GetCherryStyle() & STYLE_AUTORESIZE)
		ResizeWindow();	
}

void CCherryLinkCtrl::SetDownFontSize(float fFontSize)
{
	CCherryFontByStatus::SetDownFontSize(fFontSize);

	// STYLE_AUTORESIZE �����Ǿ� �ִٸ� ������ Resize
	if (GetCherryStyle() & STYLE_AUTORESIZE)
		ResizeWindow();	
}

void CCherryLinkCtrl::SetDownFontColor(Color fontColor)
{
	CCherryFontByStatus::SetDownFontColor(fontColor);
	// ������ ������ ������ �̷������ �������� �������� �� �ʿ䰡 ����.
}

void CCherryLinkCtrl::SetDownFontStyle(DWORD dwFontStyle)
{
	CCherryFontByStatus::SetDownFontStyle(dwFontStyle);

	// STYLE_AUTORESIZE �����Ǿ� �ִٸ� ������ Resize
	if (GetCherryStyle() & STYLE_AUTORESIZE)
		ResizeWindow();	
}

void CCherryLinkCtrl::SetDisableFont(LPCTSTR lpszFontName, float fFontSize, Color fontColor, DWORD dwFontStyle)
{
	CCherryFontByStatus::SetDisableFont(lpszFontName, fFontSize, fontColor, dwFontStyle);

	if (GetCherryStyle() & STYLE_AUTORESIZE)
		ResizeWindow();

	Invalidate();
}

BOOL CCherryLinkCtrl::SetDisableFont(CCherryFont *pDisableFont)
{
	if (!CCherryFontByStatus::SetDownFont(pDisableFont))
		return FALSE;

	if (GetCherryStyle() & STYLE_AUTORESIZE)
		ResizeWindow();

	Invalidate();

	return TRUE;
}

void CCherryLinkCtrl::SetDisableFontName(LPCTSTR lpszFontName)
{
	CCherryFontByStatus::SetDisableFontName(lpszFontName);
	
	// STYLE_AUTORESIZE �����Ǿ� �ִٸ� ������ Resize
	if (GetCherryStyle() & STYLE_AUTORESIZE)
		ResizeWindow();	
}

void CCherryLinkCtrl::SetDisableFontSize(float fFontSize)
{
	CCherryFontByStatus::SetDisableFontSize(fFontSize);

	// STYLE_AUTORESIZE �����Ǿ� �ִٸ� ������ Resize
	if (GetCherryStyle() & STYLE_AUTORESIZE)
		ResizeWindow();	
}

void CCherryLinkCtrl::SetDisableFontColor(Color fontColor)
{
	CCherryFontByStatus::SetDisableFontColor(fontColor);
	// ������ ������ ������ �̷������ �������� �������� �� �ʿ䰡 ����.
}

void CCherryLinkCtrl::SetDisableFontStyle(DWORD dwFontStyle)
{
	CCherryFontByStatus::SetDisableFontStyle(dwFontStyle);

	// STYLE_AUTORESIZE �����Ǿ� �ִٸ� ������ Resize
	if (GetCherryStyle() & STYLE_AUTORESIZE)
		ResizeWindow();	
}

void CCherryLinkCtrl::OnMouseMove(UINT nFlags, CPoint point)
{
	if (!m_bTracking)
	{
		TRACKMOUSEEVENT trackMouseEvent;
		trackMouseEvent.cbSize = sizeof(TRACKMOUSEEVENT);
		trackMouseEvent.dwFlags = TME_HOVER | TME_LEAVE;
		trackMouseEvent.hwndTrack = m_hWnd;
		trackMouseEvent.dwHoverTime = 1;
		
		m_bTracking = _TrackMouseEvent(&trackMouseEvent);
	}

	CCherryStatic::OnMouseMove(nFlags, point);
}

void CCherryLinkCtrl::OnMouseHover(UINT nFlags, CPoint point)
{
	m_bHover = TRUE;
	SetCurrentFont(GetHoverFont());

	if (m_bEnableHoverHandCursor)
		SetClassLong(GetSafeHwnd(), GCL_HCURSOR, (LONG)AfxGetApp()->LoadStandardCursor(IDC_HAND));

	Invalidate();

	CCherryStatic::OnMouseHover(nFlags, point);
}

void CCherryLinkCtrl::OnMouseLeave()
{
	// �ڵ� Ŀ���� ���� ���� ���� �ʴ� ��찡 �߻��ϱ� ���� ���� �ڵ�� �̵��� ���� ó��(�ݵ�� ȣ��Ǿ����� �Ѵ�)
	if (m_bEnableHoverHandCursor)
		SetClassLong(GetSafeHwnd(), GCL_HCURSOR, (LONG)AfxGetApp()->LoadStandardCursor(IDC_ARROW));

	// ��ũ�� Ŀ���� �÷����� ���¿��� Disable�� �Ǹ� �������� �ʽ��ϴ�.
	if (!IsWindowEnabled())
		return;

	if (m_bTracking)
	{
		TRACKMOUSEEVENT trackMouseEvent;
		trackMouseEvent.cbSize = sizeof(TRACKMOUSEEVENT);
		trackMouseEvent.dwFlags = TME_CANCEL;
		trackMouseEvent.hwndTrack = m_hWnd;
		trackMouseEvent.dwHoverTime = 1;
		_TrackMouseEvent(&trackMouseEvent);

		m_bTracking = FALSE;
	}

	m_bHover = FALSE;
	SetCurrentFont(GetNormalFont());
	Invalidate();

	CCherryStatic::OnMouseLeave();
}

void CCherryLinkCtrl::OnLButtonDown(UINT nFlags, CPoint point)
{
	SetCurrentFont(GetDownFont());
	Invalidate();

	CCherryStatic::OnLButtonDown(nFlags, point);
}

void CCherryLinkCtrl::OnLButtonUp(UINT nFlags, CPoint point)
{
	SetCurrentFont(GetHoverFont());
	Invalidate();

	CCherryStatic::OnLButtonUp(nFlags, point);
}

void CCherryLinkCtrl::DrawItem(LPDRAWITEMSTRUCT lpDrawItemStruct)
{
	CRect clientRect;
	GetClientRect(&clientRect);

	CDC *pDC = CDC::FromHandle(lpDrawItemStruct->hDC);
	CCherryMemDC memDC(pDC, clientRect);
	Graphics graphics(memDC.GetSafeHdc());

	if (m_pBackMemDC)
		m_pBackMemDC->Draw(&memDC);

	CString strText;
	GetWindowText(strText);

	if (!strText.IsEmpty())
	{
		strText.Replace(_T("&"), _T(""));
		GetCurrentFont()->DrawText(&graphics, strText, clientRect);
	}
}

BOOL CCherryLinkCtrl::PreTranslateMessage(MSG* pMsg)
{
	if (pMsg->message == WM_LBUTTONDBLCLK)
		pMsg->message = WM_LBUTTONDOWN;

	return CCherryStatic::PreTranslateMessage(pMsg);
}

void CCherryLinkCtrl::EnableHoverHandCursor(BOOL bEnable)
{
	m_bEnableHoverHandCursor = bEnable;
}

void CCherryLinkCtrl::OnEnable(BOOL bEnable)
{
	CCherryStatic::OnEnable(bEnable);

	if (!bEnable)
	{
		SetCurrentFont(GetDisableFont());
		Invalidate();
	}
}

void CCherryLinkCtrl::EnableAutoResize(BOOL bEnable)
{
	if (bEnable)
		ModifyCherryStyle(0, STYLE_AUTORESIZE);
	else
		ModifyCherryStyle(STYLE_AUTORESIZE, 0);
}

void CCherryLinkCtrl::ResizeWindow()
{
	// �� ���� �� ��Ʈ �߿� ���� ��Ʈ ũ�Ⱑ ū ���¸� �������� ������¡ �Ѵ�.
	const UINT nFontNum = 4;
	CCherryFont *pFonts[nFontNum], *pGreatestSizeFont;

	pFonts[0] = GetNormalFont();
	pFonts[1] = GetHoverFont();
	pFonts[2] = GetDownFont();
	pFonts[3] = GetDisableFont();

	pGreatestSizeFont = pFonts[0];

	for (UINT i = 0; i < nFontNum - 1; i++)
	{
		if (pGreatestSizeFont->GetFontSize() < pFonts[i + 1]->GetFontSize())
			pGreatestSizeFont = pFonts[i + 1];
	}

	CRect textRect;
	CString strText;
	
	GetWindowText(strText);

	if (!strText.IsEmpty())
	{
		CPaintDC dc(this);
		Graphics graphics(dc.GetSafeHdc());

		textRect = pGreatestSizeFont->MeasureString(&graphics, strText);
	}

	CRect windowRect;
	GetWindowRect(&windowRect);
	GetParent()->ScreenToClient(&windowRect);

	windowRect.right = windowRect.left + textRect.Width() + 3;	// ������ �����ϱ� ���� 3
	windowRect.bottom = windowRect.top + textRect.Height();

	MoveWindow(windowRect);
}