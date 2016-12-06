// CherryCheckBox.cpp : ���� �����Դϴ�.
//

#include "stdafx.h"
#include "CherryCheckBox.h"


// CCherryCheckBox

IMPLEMENT_DYNAMIC(CCherryCheckBox, CCherryButton)

CCherryCheckBox::CCherryCheckBox()
{
	m_checkStatus = STATUS_UNCHECKED;
	//m_dwCherryStyle = 0;
	//m_bEnableHoverHandCursor = FALSE;
	//m_pBackMemDC = NULL;
}

CCherryCheckBox::~CCherryCheckBox()
{
}

BEGIN_MESSAGE_MAP(CCherryCheckBox, CCherryButton)
	ON_WM_LBUTTONUP()
END_MESSAGE_MAP()

// CCherryCheckBox �޽��� ó�����Դϴ�.
CHERRY_RET CCherryCheckBox::Create(LPCTSTR lpszCaption, LPCTSTR lpszImagePath, DWORD dwCherryStyle, DWORD dwStyle, const CCherryRect &cherryRect, CWnd *pParentWnd, UINT nID)
{
	CHERRY_RET cherryRet = CCherryException::ERROR_CHERRY_SUCCESS;

	try
	{
		dwStyle |= BS_OWNERDRAW;	// �߰�

		if (!CButton::Create(lpszCaption, dwStyle, cherryRect, pParentWnd, nID))
			throw CCherryException::ERROR_CHECKBOX_CREATE_FAIL;

		// CherryCheckBox ���� ��Ÿ�� bit �÷���
		ModifyCherryStyle(0, dwCherryStyle);

		if (dwCherryStyle & STYLE_BUTTONTYPE)
		{
			GetNormalFont()->SetFontStyle(CCherryFont::STYLE_CENTER | CCherryFont::STYLE_VCENTER);
			GetHoverFont()->SetFontStyle(CCherryFont::STYLE_CENTER | CCherryFont::STYLE_VCENTER);
			GetDownFont()->SetFontStyle(CCherryFont::STYLE_CENTER | CCherryFont::STYLE_VCENTER);
			GetDisableFont()->SetFontStyle(CCherryFont::STYLE_CENTER | CCherryFont::STYLE_VCENTER);
		}
		else
		{
			// �⺻ ���� ����
			GetNormalFont()->SetFontStyle(CCherryFont::STYLE_VCENTER);
			GetHoverFont()->SetFontStyle(CCherryFont::STYLE_VCENTER);
			GetDownFont()->SetFontStyle(CCherryFont::STYLE_VCENTER);
			GetDisableFont()->SetFontStyle(CCherryFont::STYLE_VCENTER);
		}

		// �⺻ ��Ȱ��ȭ �ؽ�Ʈ �Ӽ�
		COLORREF disableColor = GetSysColor(COLOR_GRAYTEXT);
		GetDisableFont()->SetFontColor(Color(GetRValue(disableColor), GetGValue(disableColor), GetBValue(disableColor)));

		//SetWindowText(lpszCaption);
	}
	catch (const CHERRY_RET &errorRet)
	{
		cherryRet = errorRet;
	}

	return SetImage(lpszImagePath);
}

CHERRY_RET CCherryCheckBox::SetImage(LPCTSTR lpszImagePath)
{
	CHERRY_RET cherryRet = CCherryException::ERROR_CHERRY_SUCCESS;

	try
	{
		// ���� �̹��� �ε�
		CCherryImage sourceImage;
		if ((cherryRet = sourceImage.LoadImage(lpszImagePath)) != CCherryException::ERROR_CHERRY_SUCCESS)
			throw cherryRet;

		UINT nRawWidth = sourceImage.GetWidth() / STATUS_MAX_COUNT;
		UINT nRawHeight = sourceImage.GetHeight() / 2;

		// üũ���� ���� �̹��� �� ���� �̹��� �߶� �ٿ��ֱ�
		for (UINT i = STATUS_NORMAL; i < STATUS_MAX_COUNT; i++)
		{
			if ((cherryRet = m_images[i].LoadImage(sourceImage.GetBitmap()->Clone(Rect(nRawWidth * i, 0, nRawWidth, nRawHeight), PixelFormatDontCare))) != CCherryException::ERROR_CHERRY_SUCCESS)
				throw cherryRet;
		}

		// üũ�� �̹��� �� ���� �̹��� �߶� �ٿ��ֱ�
		for (UINT j = STATUS_NORMAL; j < STATUS_MAX_COUNT; j++)
		{
			if ((cherryRet = m_checkedImages[j].LoadImage(sourceImage.GetBitmap()->Clone(Rect(nRawWidth * j, nRawHeight, nRawWidth, nRawHeight), PixelFormatDontCare))) != CCherryException::ERROR_CHERRY_SUCCESS)
				throw cherryRet;
		}

		if (GetCherryStyle() & STYLE_AUTORESIZE)
		{
			ResizeWindow(nRawWidth, nRawHeight);
		}
		else
		{
			CRect windowRect;
			GetWindowRect(&windowRect);
			GetParent()->ScreenToClient(&windowRect);

			UINT nWidth, nHeight;

			if (windowRect.Width() > 0)
				nWidth = windowRect.Width();
			else
				nWidth = nRawWidth;

			if (windowRect.Height() > 0)
				nHeight = windowRect.Height();
			else
				nHeight = nRawHeight;

			if (nWidth > 0 && nHeight > 0)
			{
				windowRect.right = windowRect.left + nWidth;
				windowRect.bottom = windowRect.top + nHeight;
				MoveWindow(windowRect);
			}
			else // nWidth || nHeight ��� �ϳ��� 0�̸� Auto Resizing
			{
				ResizeWindow(nRawWidth, nRawHeight);
			}
		}
	}
	catch (const CHERRY_RET &errorRet)
	{
		cherryRet = errorRet;
	}

	return cherryRet;
}

void CCherryCheckBox::DrawItem(LPDRAWITEMSTRUCT lpDrawItemStruct)
{
	CRect clientRect;
	GetClientRect(&clientRect);
	
	CDC *pDC = CDC::FromHandle(lpDrawItemStruct->hDC);
	CCherryMemDC memDC(pDC, clientRect);
	Graphics graphics(memDC.GetSafeHdc());

	graphics.SetInterpolationMode(InterpolationModeDefault);
	graphics.SetPixelOffsetMode(PixelOffsetModeNone);

	if (m_pBackMemDC)
		m_pBackMemDC->Draw(&memDC);

	CCherryImage *pCurrentImage = NULL;

	// Down
	if (lpDrawItemStruct->itemState & ODS_SELECTED) 
		pCurrentImage = m_checkStatus == STATUS_UNCHECKED ? &m_images[STATUS_DOWN] : &m_checkedImages[STATUS_DOWN];
	// Disable
	else if (lpDrawItemStruct->itemState & ODS_DISABLED)
		pCurrentImage = m_checkStatus == STATUS_UNCHECKED ? &m_images[STATUS_DISABLE] : &m_checkedImages[STATUS_DISABLE];
	// Hover
	else if (m_bHover)
		pCurrentImage = m_checkStatus == STATUS_UNCHECKED ? &m_images[STATUS_HOVER] : &m_checkedImages[STATUS_HOVER];	
	// Normal
	else
		pCurrentImage = m_checkStatus == STATUS_UNCHECKED ? &m_images[STATUS_NORMAL] : &m_checkedImages[STATUS_NORMAL];

	if (pCurrentImage->GetBitmapLastStatus() == Ok)
	{
		if ((UINT)clientRect.Width() > pCurrentImage->GetWidth() ||
			(UINT)clientRect.Height() > pCurrentImage->GetHeight())
		{
			// ���� �̹��� ���� Client ������ ū ��� 3x3 Ȯ���Ͽ� ����Ѵ�.
			int nTop = 0;
			int nWidth = 0, nHeight = 0;

			// üũ �ڽ� ��Ÿ�Ͽ� ���� �̹��� ũ�⸦ �����Ѵ�.
			if (GetCherryStyle() & STYLE_BUTTONTYPE)
			{
				nWidth = clientRect.Width();
				nHeight = clientRect.Height();

				pCurrentImage->Draw9PatchImage(&graphics, 0, nTop, nWidth, nHeight);
			}
			else
			{
				if (clientRect.Height() > static_cast<int>(pCurrentImage->GetHeight()))
					nTop = (clientRect.Height() - pCurrentImage->GetHeight()) / 2;

				nWidth = pCurrentImage->GetWidth();
				nHeight = pCurrentImage->GetHeight();

				pCurrentImage->DrawImage(&graphics, 0, nTop, nWidth, nHeight);
			}
		}
		else
		{
			// Source �̹��� ũ�⺸�� Client�� �۰ų� ���� ���� Client ũ��� ����Ѵ�.
			pCurrentImage->DrawImage(&graphics, clientRect);
		}
	}

	CString strText;
	GetWindowText(strText);
	if (!strText.IsEmpty())
	{
		CRect textRect(clientRect);

		// üũ �ڽ� ��Ÿ�Ͽ� ���� ��Ʈ ��ġ�� �����Ѵ�.
		if (!(GetCherryStyle() & STYLE_BUTTONTYPE))
		{
			// �̹��� �����ʿ� �ؽ�Ʈ ǥ��
			int nCheckBoxWidth = m_images[STATUS_NORMAL].GetWidth();

			textRect.left += nCheckBoxWidth;
		}

		strText.Replace(_T("&"), _T(""));
		GetCurrentFont()->DrawText(&graphics, strText, textRect);
	}
}

void CCherryCheckBox::ResizeWindow(UINT nWidth, UINT nHeight)
{
	if (GetCherryStyle() & STYLE_BUTTONTYPE)
	{
		CCherryButton::ResizeWindow(nWidth, nHeight);
	}
	else
	{
		// �� ���� �� ��Ʈ �߿� ���� ��Ʈ ũ�Ⱑ ū ���¸� �������� ������¡ �Ѵ�.
		CCherryFont *pFonts[STATUS_MAX_COUNT], *pGreatestSizeFont;
		pFonts[STATUS_NORMAL] = GetNormalFont();
		pFonts[STATUS_HOVER] = GetHoverFont();
		pFonts[STATUS_DOWN] = GetDownFont();
		pFonts[STATUS_DISABLE] = GetDisableFont();

		pGreatestSizeFont = pFonts[STATUS_NORMAL];

		for (UINT i = STATUS_NORMAL; i < STATUS_MAX_COUNT - 1; i++)
		{
			if (pGreatestSizeFont->GetFontSize() < pFonts[i + 1]->GetFontSize())
				pGreatestSizeFont = pFonts[i + 1];
		}

		CRect windowRect;
		GetWindowRect(&windowRect);
		GetParent()->ScreenToClient(&windowRect);
		windowRect.right = windowRect.left + nWidth;
		windowRect.bottom = windowRect.top + nHeight;

		CRect textRect;
		CString strText;
		GetWindowText(strText);
		if (!strText.IsEmpty())
		{
			CPaintDC dc(this);
			Graphics graphics(dc.GetSafeHdc());
			textRect = pGreatestSizeFont->MeasureString(&graphics, strText);
			windowRect.right += textRect.Width() + 3;	// ������ �����ϱ� ���� 3

			// �ؽ�Ʈ ���̰� �̹��� ���̺��� �� ũ��
			if (nHeight < (UINT)textRect.Height())
				windowRect.bottom = windowRect.top + textRect.Height();
		}

		MoveWindow(windowRect);
	}
}

void CCherryCheckBox::SetWindowText(LPCTSTR lpszText)
{
	CCherryButton::SetWindowText(lpszText);

	if (GetCherryStyle() & STYLE_AUTORESIZE)
	{
		if (m_images[STATUS_NORMAL].IsLoadedImage())
			ResizeWindow(m_images[STATUS_NORMAL].GetWidth(), m_images[STATUS_NORMAL].GetHeight());
	}
}

void CCherryCheckBox::OnLButtonUp(UINT nFlags, CPoint point)
{
	if (GetCherryStyle() & STYLE_AUTOCHECKBOX)
	{
		if (GetCheck() == STATUS_UNCHECKED)
			//m_checkStatus = STATUS_CHECKED;
			SetCheck(STATUS_CHECKED);
		else
			//m_checkStatus = STATUS_UNCHECKED;
			SetCheck(STATUS_UNCHECKED);
	}

	CCherryButton::OnLButtonUp(nFlags, point);
}

CCherryCheckBox::STATUS CCherryCheckBox::GetCheck() const
{
	return m_checkStatus;
}

void CCherryCheckBox::SetCheck(STATUS checkStatus)
{
	m_checkStatus = checkStatus;

	Invalidate(FALSE);

	GetParent()->SendMessage(WM_COMMAND, MAKEWPARAM(GetDlgCtrlID(), BN_CLICKED));
}

void CCherryCheckBox::EnableAutoResize(BOOL bEnable)
{
	if (bEnable)
		ModifyCherryStyle(0, STYLE_AUTORESIZE);
	else
		ModifyCherryStyle(STYLE_AUTORESIZE, 0);
}

void CCherryCheckBox::EnableAutoCheck(BOOL bEnable)
{
	if (bEnable)
		ModifyCherryStyle(0, STYLE_AUTOCHECKBOX);
	else
		ModifyCherryStyle(STYLE_AUTOCHECKBOX, 0);
}

void CCherryCheckBox::SetCherryFont(LPCTSTR lpszFontName, float fFontSize, Color fontColor, DWORD dwFontStyle)
{
	CCherryFontByStatus::SetCherryFont(lpszFontName, fFontSize, fontColor, dwFontStyle);

	// CherryCheckBox::STYLE_AUTORESIZE �����Ǿ� �ִٸ� ������ Resize
	if (GetCherryStyle() & STYLE_AUTORESIZE)
	{
		if (m_images[STATUS_NORMAL].IsLoadedImage())
			ResizeWindow(m_images[STATUS_NORMAL].GetWidth(), m_images[STATUS_NORMAL].GetHeight());
	}
}

BOOL CCherryCheckBox::SetCherryFont(CCherryFont *pCherryFont)
{
	if (!CCherryFontByStatus::SetCherryFont(pCherryFont))
		return FALSE;

	// CherryCheckBox::STYLE_AUTORESIZE �����Ǿ� �ִٸ� ������ Resize
	if (GetCherryStyle() & STYLE_AUTORESIZE)
	{
		if (m_images[STATUS_NORMAL].IsLoadedImage())
			ResizeWindow(m_images[STATUS_NORMAL].GetWidth(), m_images[STATUS_NORMAL].GetHeight());
	}

	return TRUE;
}

void CCherryCheckBox::SetNormalFont(LPCTSTR lpszFontName, float fFontSize, Color fontColor, DWORD dwFontStyle)
{
	CCherryFontByStatus::SetNormalFont(lpszFontName, fFontSize, fontColor, dwFontStyle);
		
	// CherryCheckBox::STYLE_AUTORESIZE �����Ǿ� �ִٸ� ������ Resize
	if (GetCherryStyle() & STYLE_AUTORESIZE)
	{
		if (m_images[STATUS_NORMAL].IsLoadedImage())
			ResizeWindow(m_images[STATUS_NORMAL].GetWidth(), m_images[STATUS_NORMAL].GetHeight());
	}
}

BOOL CCherryCheckBox::SetNormalFont(CCherryFont *pNormalFont)
{
	if (!CCherryFontByStatus::SetNormalFont(pNormalFont))
		return FALSE;

	// CherryCheckBox::STYLE_AUTORESIZE �����Ǿ� �ִٸ� ������ Resize
	if (GetCherryStyle() & STYLE_AUTORESIZE)
	{
		if (m_images[STATUS_NORMAL].IsLoadedImage())
			ResizeWindow(m_images[STATUS_NORMAL].GetWidth(), m_images[STATUS_NORMAL].GetHeight());
	}

	return TRUE;
}

void CCherryCheckBox::SetNormalFontName(LPCTSTR lpszFontName)
{
	CCherryFontByStatus::SetNormalFontName(lpszFontName);
	
	// CherryCheckBox::STYLE_AUTORESIZE �����Ǿ� �ִٸ� ������ Resize
	if (GetCherryStyle() & STYLE_AUTORESIZE)
	{
		if (m_images[STATUS_NORMAL].IsLoadedImage())
			ResizeWindow(m_images[STATUS_NORMAL].GetWidth(), m_images[STATUS_NORMAL].GetHeight());
	}
}

void CCherryCheckBox::SetNormalFontSize(float fFontSize)
{
	CCherryFontByStatus::SetNormalFontSize(fFontSize);

	// CherryCheckBox::STYLE_AUTORESIZE �����Ǿ� �ִٸ� ������ Resize
	if (GetCherryStyle() & STYLE_AUTORESIZE)
	{
		if (m_images[STATUS_NORMAL].IsLoadedImage())
			ResizeWindow(m_images[STATUS_NORMAL].GetWidth(), m_images[STATUS_NORMAL].GetHeight());
	}
}

void CCherryCheckBox::SetNormalFontColor(Color fontColor)
{
	CCherryFontByStatus::SetNormalFontColor(fontColor);
	// ������ ������ ������ �̷������ �������� �������� �� �ʿ䰡 ����.
}

void CCherryCheckBox::SetNormalFontStyle(DWORD dwFontStyle)
{
	CCherryFontByStatus::SetNormalFontStyle(dwFontStyle);

	// CherryCheckBox::STYLE_AUTORESIZE �����Ǿ� �ִٸ� ������ Resize
	if (GetCherryStyle() & STYLE_AUTORESIZE)
	{
		if (m_images[STATUS_NORMAL].IsLoadedImage())
			ResizeWindow(m_images[STATUS_NORMAL].GetWidth(), m_images[STATUS_NORMAL].GetHeight());
	}
}

void CCherryCheckBox::SetHoverFont(LPCTSTR lpszFontName, float fFontSize, Color fontColor, DWORD dwFontStyle)
{
	CCherryFontByStatus::GetHoverFont()->SetCherryFont(lpszFontName, fFontSize, fontColor, dwFontStyle);
		
	// CherryCheckBox::STYLE_AUTORESIZE �����Ǿ� �ִٸ� ������ Resize
	if (GetCherryStyle() & STYLE_AUTORESIZE)
	{
		if (m_images[STATUS_NORMAL].IsLoadedImage())
			ResizeWindow(m_images[STATUS_NORMAL].GetWidth(), m_images[STATUS_NORMAL].GetHeight());
	}
}

BOOL CCherryCheckBox::SetHoverFont(CCherryFont *pHoverFont)
{
	if (!CCherryFontByStatus::SetHoverFont(pHoverFont))
		return FALSE;

	// CherryCheckBox::STYLE_AUTORESIZE �����Ǿ� �ִٸ� ������ Resize
	if (GetCherryStyle() & STYLE_AUTORESIZE)
	{
		if (m_images[STATUS_NORMAL].IsLoadedImage())
			ResizeWindow(m_images[STATUS_NORMAL].GetWidth(), m_images[STATUS_NORMAL].GetHeight());
	}

	return TRUE;
}

void CCherryCheckBox::SetHoverFontName(LPCTSTR lpszFontName)
{
	CCherryFontByStatus::SetHoverFontName(lpszFontName);
	
	// CherryCheckBox::STYLE_AUTORESIZE �����Ǿ� �ִٸ� ������ Resize
	if (GetCherryStyle() & STYLE_AUTORESIZE)
	{
		if (m_images[STATUS_NORMAL].IsLoadedImage())
			ResizeWindow(m_images[STATUS_NORMAL].GetWidth(), m_images[STATUS_NORMAL].GetHeight());
	}
}

void CCherryCheckBox::SetHoverFontSize(float fFontSize)
{
	CCherryFontByStatus::SetHoverFontSize(fFontSize);

	// CherryCheckBox::STYLE_AUTORESIZE �����Ǿ� �ִٸ� ������ Resize
	if (GetCherryStyle() & STYLE_AUTORESIZE)
	{
		if (m_images[STATUS_NORMAL].IsLoadedImage())
			ResizeWindow(m_images[STATUS_NORMAL].GetWidth(), m_images[STATUS_NORMAL].GetHeight());
	}
}

void CCherryCheckBox::SetHoverFontColor(Color fontColor)
{
	CCherryFontByStatus::SetHoverFontColor(fontColor);
	// ������ ������ ������ �̷������ �������� �������� �� �ʿ䰡 ����.
}

void CCherryCheckBox::SetHoverFontStyle(DWORD dwFontStyle)
{
	CCherryFontByStatus::SetHoverFontStyle(dwFontStyle);

	// CherryCheckBox::STYLE_AUTORESIZE �����Ǿ� �ִٸ� ������ Resize
	if (GetCherryStyle() & STYLE_AUTORESIZE)
	{
		if (m_images[STATUS_NORMAL].IsLoadedImage())
			ResizeWindow(m_images[STATUS_NORMAL].GetWidth(), m_images[STATUS_NORMAL].GetHeight());
	}
}

void CCherryCheckBox::SetDownFont(LPCTSTR lpszFontName, float fFontSize, Color fontColor, DWORD dwFontStyle)
{
	CCherryFontByStatus::SetDownFont(lpszFontName, fFontSize, fontColor, dwFontStyle);
		
	// CherryCheckBox::STYLE_AUTORESIZE �����Ǿ� �ִٸ� ������ Resize
	if (GetCherryStyle() & STYLE_AUTORESIZE)
	{
		if (m_images[STATUS_NORMAL].IsLoadedImage())
			ResizeWindow(m_images[STATUS_NORMAL].GetWidth(), m_images[STATUS_NORMAL].GetHeight());
	}
}

BOOL CCherryCheckBox::SetDownFont(CCherryFont *pDownFont)
{
	if (!CCherryFontByStatus::SetDownFont(pDownFont))
		return FALSE;

	// CherryCheckBox::STYLE_AUTORESIZE �����Ǿ� �ִٸ� ������ Resize
	if (GetCherryStyle() & STYLE_AUTORESIZE)
	{
		if (m_images[STATUS_NORMAL].IsLoadedImage())
			ResizeWindow(m_images[STATUS_NORMAL].GetWidth(), m_images[STATUS_NORMAL].GetHeight());
	}

	return TRUE;
}

void CCherryCheckBox::SetDownFontName(LPCTSTR lpszFontName)
{
	CCherryFontByStatus::SetDownFontName(lpszFontName);
	
	// CherryCheckBox::STYLE_AUTORESIZE �����Ǿ� �ִٸ� ������ Resize
	if (GetCherryStyle() & STYLE_AUTORESIZE)
	{
		if (m_images[STATUS_NORMAL].IsLoadedImage())
			ResizeWindow(m_images[STATUS_NORMAL].GetWidth(), m_images[STATUS_NORMAL].GetHeight());
	}
}

void CCherryCheckBox::SetDownFontSize(float fFontSize)
{
	CCherryFontByStatus::SetDownFontSize(fFontSize);

	// CherryCheckBox::STYLE_AUTORESIZE �����Ǿ� �ִٸ� ������ Resize
	if (GetCherryStyle() & STYLE_AUTORESIZE)
	{
		if (m_images[STATUS_NORMAL].IsLoadedImage())
			ResizeWindow(m_images[STATUS_NORMAL].GetWidth(), m_images[STATUS_NORMAL].GetHeight());
	}
}

void CCherryCheckBox::SetDownFontColor(Color fontColor)
{
	CCherryFontByStatus::SetDownFontColor(fontColor);
	// ������ ������ ������ �̷������ �������� �������� �� �ʿ䰡 ����.
}

void CCherryCheckBox::SetDownFontStyle(DWORD dwFontStyle)
{
	CCherryFontByStatus::SetDownFontStyle(dwFontStyle);

	// CherryCheckBox::STYLE_AUTORESIZE �����Ǿ� �ִٸ� ������ Resize
	if (GetCherryStyle() & STYLE_AUTORESIZE)
	{
		if (m_images[STATUS_NORMAL].IsLoadedImage())
			ResizeWindow(m_images[STATUS_NORMAL].GetWidth(), m_images[STATUS_NORMAL].GetHeight());
	}
}

void CCherryCheckBox::SetDisableFont(LPCTSTR lpszFontName, float fFontSize, Color fontColor, DWORD dwFontStyle)
{
	CCherryFontByStatus::SetDisableFont(lpszFontName, fFontSize, fontColor, dwFontStyle);
		
	// CherryCheckBox::STYLE_AUTORESIZE �����Ǿ� �ִٸ� ������ Resize
	if (GetCherryStyle() & STYLE_AUTORESIZE)
	{
		if (m_images[STATUS_NORMAL].IsLoadedImage())
			ResizeWindow(m_images[STATUS_NORMAL].GetWidth(), m_images[STATUS_NORMAL].GetHeight());
	}
}

BOOL CCherryCheckBox::SetDisableFont(CCherryFont *pDisableFont)
{
	if (!CCherryFontByStatus::SetDisableFont(pDisableFont))
		return FALSE;

	// CherryCheckBox::STYLE_AUTORESIZE �����Ǿ� �ִٸ� ������ Resize
	if (GetCherryStyle() & STYLE_AUTORESIZE)
	{
		if (m_images[STATUS_NORMAL].IsLoadedImage())
			ResizeWindow(m_images[STATUS_NORMAL].GetWidth(), m_images[STATUS_NORMAL].GetHeight());
	}

	return TRUE;
}

void CCherryCheckBox::SetDisableFontName(LPCTSTR lpszFontName)
{
	CCherryFontByStatus::SetDisableFontName(lpszFontName);
	
	// CherryCheckBox::STYLE_AUTORESIZE �����Ǿ� �ִٸ� ������ Resize
	if (GetCherryStyle() & STYLE_AUTORESIZE)
	{
		if (m_images[STATUS_NORMAL].IsLoadedImage())
			ResizeWindow(m_images[STATUS_NORMAL].GetWidth(), m_images[STATUS_NORMAL].GetHeight());
	}
}

void CCherryCheckBox::SetDisableFontSize(float fFontSize)
{
	CCherryFontByStatus::SetDisableFontSize(fFontSize);

	// CherryCheckBox::STYLE_AUTORESIZE �����Ǿ� �ִٸ� ������ Resize
	if (GetCherryStyle() & STYLE_AUTORESIZE)
	{
		if (m_images[STATUS_NORMAL].IsLoadedImage())
			ResizeWindow(m_images[STATUS_NORMAL].GetWidth(), m_images[STATUS_NORMAL].GetHeight());
	}
}

void CCherryCheckBox::SetDisableFontColor(Color fontColor)
{
	CCherryFontByStatus::SetDisableFontColor(fontColor);
	// ������ ������ ������ �̷������ �������� �������� �� �ʿ䰡 ����.
}

void CCherryCheckBox::SetDisableFontStyle(DWORD dwFontStyle)
{
	CCherryFontByStatus::SetDisableFontStyle(dwFontStyle);

	// CherryCheckBox::STYLE_AUTORESIZE �����Ǿ� �ִٸ� ������ Resize
	if (GetCherryStyle() & STYLE_AUTORESIZE)
	{
		if (m_images[STATUS_NORMAL].IsLoadedImage())
			ResizeWindow(m_images[STATUS_NORMAL].GetWidth(), m_images[STATUS_NORMAL].GetHeight());
	}
}