// CherrySliderCtrl.cpp : ���� �����Դϴ�.
//

#include "stdafx.h"
#include "CherrySliderCtrl.h"


// CCherrySliderCtrlThumbButton

IMPLEMENT_DYNAMIC(CCherrySliderCtrlThumbButton, CCherryButton)

CCherrySliderCtrlThumbButton::CCherrySliderCtrlThumbButton()
{
	m_bLButtonDown = FALSE;
}

CCherrySliderCtrlThumbButton::~CCherrySliderCtrlThumbButton()
{
}

BEGIN_MESSAGE_MAP(CCherrySliderCtrlThumbButton, CCherryButton)
	ON_WM_MOUSEMOVE()
	ON_WM_LBUTTONDOWN()
	ON_WM_LBUTTONUP()
END_MESSAGE_MAP()

void CCherrySliderCtrlThumbButton::UseCachedImage(BOOL bUse)
{
	for (int i = STATUS_NORMAL; i < STATUS_MAX_COUNT; i++)
		m_images[i].UseCachedImage(bUse);
}

void CCherrySliderCtrlThumbButton::OnMouseMove(UINT nFlags, CPoint point)
{
	((CCherrySliderCtrl *)GetParent())->OnMouseMove(nFlags, point);

	//GetParent()->SendMessage(WM_MOUSEMOVE);
	CCherryButton::OnMouseMove(nFlags, point);
}

void CCherrySliderCtrlThumbButton::OnLButtonDown(UINT nFlags, CPoint point)
{
	m_bLButtonDown = TRUE;

	CCherryButton::OnLButtonDown(nFlags, point);
}

void CCherrySliderCtrlThumbButton::OnLButtonUp(UINT nFlags, CPoint point)
{
	m_bLButtonDown = FALSE;

	CCherryButton::OnLButtonUp(nFlags, point);
}


// CCherrySliderCtrl

IMPLEMENT_DYNAMIC(CCherrySliderCtrl, CCherryProgressCtrl)

CCherrySliderCtrl::CCherrySliderCtrl()
{
}

CCherrySliderCtrl::~CCherrySliderCtrl()
{
}

BEGIN_MESSAGE_MAP(CCherrySliderCtrl, CCherryProgressCtrl)
	ON_WM_LBUTTONDOWN()
	ON_WM_MOUSEMOVE()
	ON_WM_LBUTTONUP()
END_MESSAGE_MAP()


// CCherrySliderCtrl �޽��� ó�����Դϴ�.

CHERRY_RET CCherrySliderCtrl::Create(LPCTSTR lpszThumbImagePath, LPCTSTR lpszTrackImagePath, const int nMaxPos, DWORD dwCherryStyle, DWORD dwStyle, const CCherryRect &cherryRect, CWnd *pParentWnd, UINT nID)
{	
	CHERRY_RET cherryRet = CCherryException::ERROR_CHERRY_SUCCESS;

	try
	{
		switch (dwCherryStyle)
		{
			// 2x1 �̹����� ����ϴ� ��Ÿ���̸�
		case STYLE_HORIZONTAL_LEFT_TO_RIGHT:
		case STYLE_HORIZONTAL_RIGHT_TO_LEFT:
		case STYLE_VERTICAL_BOTTOM_TO_TOP:
		case STYLE_VERTICAL_TOP_TO_BOTTOM:
			{
				// ���� CherryProgressCtrl �ڵ带 ����Ѵ�.
				if ((cherryRet = CCherryProgressCtrl::Create(lpszTrackImagePath, nMaxPos, dwCherryStyle,
					dwStyle, cherryRect, pParentWnd, nID)) != CCherryException::ERROR_CHERRY_SUCCESS)
					throw cherryRet;

				// ��ư ����
				if ((cherryRet = m_thumbButton.Create(NULL, lpszThumbImagePath, STYLE_AUTORESIZE,
					WS_VISIBLE | WS_CHILD, CCherryRect(0, 0, 0, 0), this, nID + 1)) != CCherryException::ERROR_CHERRY_SUCCESS)
					throw cherryRet;

				m_thumbButton.UseCachedImage(); // ��ư�� ���� �������� ĳ�õ� �̹����� �������.

				CRect clientRect;
				GetClientRect(&clientRect);

				// �ݵ�� ��ư ���� �ڿ� ȣ��
				RepositionThumbButton(clientRect.Width(), clientRect.Height());
			}
			break;
			// 1x1 ��Ÿ���� �̹������
		case STYLE_HORIZONTAL_LEFT_TO_RIGHT_NOT_FOLLOW:
		case STYLE_HORIZONTAL_RIGHT_TO_LEFT_NOT_FOLLOW:
		case STYLE_VERTICAL_BOTTOM_TO_TOP_NOT_FOLLOW:
		case STYLE_VERTICAL_TOP_TO_BOTTOM_NOT_FOLLOW:
			{
				if (!CWnd::Create(NULL, NULL, dwStyle, cherryRect, pParentWnd, nID))
					throw CCherryException::ERROR_SLIDERCTRL_CREATE_FAIL;

				SetMaxPos(nMaxPos);	// CWnd�� �����߱⿡ �������� ����
				SetPos(nMaxPos);

				// CCherrySliderCtrl ���� ��Ÿ�� bit �÷���
				ModifyCherryStyle(0, dwCherryStyle);

				// ��ư ����
				if ((cherryRet = m_thumbButton.Create(NULL, lpszThumbImagePath, STYLE_AUTORESIZE,
					WS_VISIBLE | WS_CHILD, CCherryRect(0, 0, 0, 0), this, nID + 1)) != CCherryException::ERROR_CHERRY_SUCCESS)
					throw cherryRet;

				if ((cherryRet = SetTrackImage(lpszTrackImagePath)) != CCherryException::ERROR_CHERRY_SUCCESS)
					throw cherryRet;
			}
			break;
		default:
			ASSERT(0);
			break;
		}
	}
	catch (const CHERRY_RET &errorRet)
	{
		cherryRet = errorRet;
	}

	return cherryRet;
}

CHERRY_RET CCherrySliderCtrl::SetThumbImage(LPCTSTR lpszImagePath)
{
	return m_thumbButton.SetImage(lpszImagePath);
}

// NOT_FOLLOW �� �̹��� �� �Լ�
CHERRY_RET CCherrySliderCtrl::SetTrackImage(LPCTSTR lpszImagePath)
{
	CHERRY_RET cherryRet = CCherryException::ERROR_CHERRY_SUCCESS;

	try
	{
		// ��� �̹��� �ε�
		if ((cherryRet = m_trackImage.LoadImage(lpszImagePath, TRUE)) != CCherryException::ERROR_CHERRY_SUCCESS)
			throw cherryRet;

		UINT nRawWidth = m_trackImage.GetWidth();
		UINT nRawHeight = m_trackImage.GetHeight();

		if (GetCherryStyle() & STYLE_AUTORESIZE)
		{
			// SLIDER_STYLE_AUTORESIZE �����Ǿ� �ִٸ� ������ Resize
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
				nWidth = 0;

			if (windowRect.Height() > 0)
				nHeight = windowRect.Height();
			else
				nHeight = 0;

			if (nWidth > 0 && nHeight > 0)
			{
				windowRect.right = windowRect.left + nWidth;
				windowRect.bottom = windowRect.top + nHeight;

				MoveWindow(windowRect);

				// Thumb ��ư ���̸� ����� �̵�
				RepositionThumbButton(windowRect.Width(), windowRect.Height());
			}
			else
			{
				// nWidth || nHeight ��� �ϳ��� 0�̸� Auto Resizing
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

void CCherrySliderCtrl::OnDrawCherry(CCherryMemDC *pDC)
{
	// 2013-07-19 ogoons: 
	// CherryProgressCtrl�� �ڵ� �ߺ������� dc�� ��Ȱ���ϱ⿣ ���ŷο�Ƿ�
	// �׳� ���� �ڵ带 �и��Ͽ� �������̵� �Ѵ�.
	CRect clientRect;
	GetClientRect(&clientRect);

	Graphics graphics(pDC->GetSafeHdc());
	graphics.SetInterpolationMode(InterpolationModeDefault);
	graphics.SetPixelOffsetMode(PixelOffsetModeNone);

	DWORD dwCherryStyle = GetCherryStyle();

	switch (dwCherryStyle)
	{
	// 2x1 �̹����� ����ϴ� ��Ÿ���̸�
	case STYLE_HORIZONTAL_LEFT_TO_RIGHT:
	case STYLE_HORIZONTAL_RIGHT_TO_LEFT:
	case STYLE_VERTICAL_BOTTOM_TO_TOP:
	case STYLE_VERTICAL_TOP_TO_BOTTOM:
	{
		// ��� �׸���
		if (m_images[DRAW_TYPE_BACKGROUND].GetBitmapLastStatus() == Ok)
		{
			if ((UINT)clientRect.Width() > m_images[DRAW_TYPE_BACKGROUND].GetWidth() && 
				(UINT)clientRect.Height() > m_images[DRAW_TYPE_BACKGROUND].GetHeight())
				// Source ���� ū ��� 3x3 Ȯ���Ͽ� ����Ѵ�.
				m_images[DRAW_TYPE_BACKGROUND].Draw9PatchImage(&graphics, clientRect);
			else
				// Source ũ�⺸�� Client�� �۰ų� ���� ���� Client ũ��� ����Ѵ�.
				m_images[DRAW_TYPE_BACKGROUND].DrawImage(&graphics, clientRect);
		}

		// ���� ���¹� �׸���
		if (m_images[DRAW_TYPE_FILL].GetBitmapLastStatus() == Ok)
		{
			int nSourceLeft = 0, nSourceTop = 0;
			int nSourceWidth = 0, nSourceHeight = 0;
			CRect fillRect(clientRect);

			// �ٿ��� ���� ������ ���
			switch (dwCherryStyle)
			{
			case STYLE_HORIZONTAL_RIGHT_TO_LEFT:
				if ((UINT)(nSourceLeft = m_images[DRAW_TYPE_FILL].GetWidth() - ((m_images[DRAW_TYPE_FILL].GetWidth() * m_nPos) / m_nMaxPos)) >= m_images[DRAW_TYPE_FILL].GetWidth())
					goto done; // 0�϶��� �׸��� ����

				nSourceWidth = (int)(m_images[DRAW_TYPE_FILL].GetWidth() - nSourceLeft);
				nSourceHeight = (int)m_images[DRAW_TYPE_FILL].GetHeight();
				fillRect.left = (fillRect.right - ((fillRect.right * m_nPos) / m_nMaxPos));
				break;
			case STYLE_HORIZONTAL_LEFT_TO_RIGHT:
				if ((nSourceWidth = (int)((m_images[DRAW_TYPE_FILL].GetWidth() * m_nPos) / m_nMaxPos)) <= 0)
					goto done; // 0�϶��� �׸��� ����

				nSourceHeight = (int)m_images[DRAW_TYPE_FILL].GetHeight();
				fillRect.right = (fillRect.right * m_nPos) / m_nMaxPos;
				break;
			case STYLE_VERTICAL_BOTTOM_TO_TOP:
				if ((UINT)(nSourceTop = m_images[DRAW_TYPE_FILL].GetHeight() - ((m_images[DRAW_TYPE_FILL].GetHeight() * m_nPos) / m_nMaxPos)) >= m_images[DRAW_TYPE_FILL].GetHeight())
					goto done; // 0�϶��� �׸��� ����

				nSourceWidth = (int)m_images[DRAW_TYPE_FILL].GetWidth();
				nSourceHeight = (int)(m_images[DRAW_TYPE_FILL].GetHeight() - nSourceTop);
				fillRect.top = (fillRect.bottom - ((fillRect.bottom * m_nPos) / m_nMaxPos));
				break;
			case STYLE_VERTICAL_TOP_TO_BOTTOM:
				if ((nSourceHeight = (int)((m_images[DRAW_TYPE_FILL].GetHeight() * m_nPos) / m_nMaxPos)) <= 0)
					goto done; // 0�϶��� �׸��� ����

				nSourceWidth = (int)m_images[DRAW_TYPE_FILL].GetWidth();
				fillRect.bottom = (fillRect.bottom * m_nPos) / m_nMaxPos;
				break;
			default:
				ASSERT(0);
				break;
			}

			CCherryImage fillImage(m_images[DRAW_TYPE_FILL].GetBitmap(), nSourceLeft, nSourceTop, nSourceWidth, nSourceHeight, TRUE);
			
			if ((UINT)clientRect.Width() > m_images[DRAW_TYPE_FILL].GetWidth() &&
				(UINT)clientRect.Height() > m_images[DRAW_TYPE_FILL].GetHeight())
				// Source ���� ū ��� 3x3 Ȯ���Ͽ� ����Ѵ�.
				fillImage.Draw9PatchImage(&graphics, fillRect);
			else
				// Source ũ�⺸�� Client�� �۰ų� ���� ���� Client ũ��� ����Ѵ�.
				fillImage.DrawImage(&graphics, fillRect);
		}
	}
	// 1x1 ��Ÿ���� �̹������
	case STYLE_HORIZONTAL_LEFT_TO_RIGHT_NOT_FOLLOW:
	case STYLE_HORIZONTAL_RIGHT_TO_LEFT_NOT_FOLLOW:
	case STYLE_VERTICAL_BOTTOM_TO_TOP_NOT_FOLLOW:
	case STYLE_VERTICAL_TOP_TO_BOTTOM_NOT_FOLLOW:
		if (m_trackImage.IsLoadedImage())
			m_trackImage.Draw9PatchImage(&graphics, clientRect);
		break;
	default:
		ASSERT(0);
		break;
	}

done:
	m_thumbButton.SetBackground(pDC);
}

void CCherrySliderCtrl::ResizeWindow(UINT nWidth, UINT nHeight)
{
	CCherryProgressCtrl::ResizeWindow(nWidth, nHeight);

	// Thumb ��ư�� ���̸� ����� �̵�
	RepositionThumbButton(nWidth, nHeight);
}

// Pos = 0�� �������� Reposition�Ѵ�.
void CCherrySliderCtrl::RepositionThumbButton(int nTrackWidth, int nTrackHeight)
{
	CRect thumbButtonRect;
	m_thumbButton.GetClientRect(&thumbButtonRect);

	int nThumbButtonWidth = thumbButtonRect.Width();
	int nThumbButtonHeight = thumbButtonRect.Height();

	// Horizontal
	DWORD dwCherryStyle = GetCherryStyle();

	switch (dwCherryStyle)
	{
	case STYLE_HORIZONTAL_RIGHT_TO_LEFT:
	case STYLE_HORIZONTAL_LEFT_TO_RIGHT:
	case STYLE_HORIZONTAL_RIGHT_TO_LEFT_NOT_FOLLOW:
	case STYLE_HORIZONTAL_LEFT_TO_RIGHT_NOT_FOLLOW:
		{
			int nThumbButtonHeight = thumbButtonRect.Height();
			LONG lTop = (nTrackHeight - nThumbButtonHeight) / 2;	// ���� ��� ����

			thumbButtonRect.top = lTop;
			thumbButtonRect.bottom = lTop + nThumbButtonHeight;

			switch (dwCherryStyle)
			{
			// Left to right
			case STYLE_HORIZONTAL_RIGHT_TO_LEFT:
			case STYLE_HORIZONTAL_RIGHT_TO_LEFT_NOT_FOLLOW:
				thumbButtonRect.left = nTrackWidth - nThumbButtonWidth;
				thumbButtonRect.right = nTrackWidth;
				break;
			// Right to left
			case STYLE_HORIZONTAL_LEFT_TO_RIGHT:
			case STYLE_HORIZONTAL_LEFT_TO_RIGHT_NOT_FOLLOW:
				thumbButtonRect.left = 0;
				thumbButtonRect.right = nThumbButtonWidth;
				break;
			default:
				ASSERT(0);
				break;
			}
		}
		break;
	// Vertical
	case STYLE_VERTICAL_BOTTOM_TO_TOP:
	case STYLE_VERTICAL_TOP_TO_BOTTOM:
	case STYLE_VERTICAL_BOTTOM_TO_TOP_NOT_FOLLOW:
	case STYLE_VERTICAL_TOP_TO_BOTTOM_NOT_FOLLOW:
		{
			int nThumbButtonWidth = thumbButtonRect.Width();
			LONG lLeft = (nTrackWidth - nThumbButtonWidth) / 2;

			thumbButtonRect.left = lLeft;
			thumbButtonRect.right = lLeft + nThumbButtonWidth;
			
			switch (dwCherryStyle)
			{
			// Bottom to top
			case STYLE_VERTICAL_BOTTOM_TO_TOP:
			case STYLE_VERTICAL_BOTTOM_TO_TOP_NOT_FOLLOW:
				thumbButtonRect.top = nTrackHeight - nThumbButtonHeight;
				thumbButtonRect.bottom = nTrackHeight;
				break;
			// Top to bottom
			case STYLE_VERTICAL_TOP_TO_BOTTOM:
			case STYLE_VERTICAL_TOP_TO_BOTTOM_NOT_FOLLOW:
				thumbButtonRect.top = 0;
				thumbButtonRect.bottom = nThumbButtonHeight;
				break;
			}
		}
		break;
	default:
		ASSERT(0);
		break;
	}

	m_thumbButton.MoveWindow(thumbButtonRect);
}

void CCherrySliderCtrl::OnLButtonDown(UINT nFlags, CPoint point)
{
	if (!IsWindowEnabled())
		return;

	CRect clientRect;
	GetClientRect(clientRect);

	int nWidth = clientRect.Width();
	int nHeight = clientRect.Height();
	int nCurrentPos = ConvertCursorPosToCurrentPos(point);

	/*
	if (nPos > GetPos())
	nPos = GetPos() + 1;
	else if (nPos < GetPos())
	nPos = GetPos() - 1;
	else
	nPos -= 1;
	*/

	CRect thumbButtonRect = ConvertCurrentPosToThumbButtonRect(nCurrentPos);

	m_thumbButton.MoveWindow(thumbButtonRect);

	// ��ư ��ġ �̵��� ���ÿ� drag�� �����ϵ��� thumb ��ư�� WM_LBUTTONDOWN �޽��� �߻� ��Ŵ
	m_thumbButton.SendMessage(WM_LBUTTONDOWN, 0, MAKELPARAM(point.x, point.y));

	SetPos(nCurrentPos);

	GetParent()->SendMessage(WM_COMMAND, MAKEWPARAM(GetDlgCtrlID(), WM_CHERRY_SLIDERCTRL_POS_CHANGED), (WPARAM)GetSafeHwnd());

	CCherryProgressCtrl::OnLButtonDown(nFlags, point);
}

void CCherrySliderCtrl::OnMouseMove(UINT nFlags, CPoint point)
{
	if (!IsWindowEnabled())
		return;

	if (m_thumbButton.IsLButtonDown())
	{
		CPoint cursorPoint;
		GetCursorPos(&cursorPoint);
		ScreenToClient(&cursorPoint);

		int nCurrentPos = ConvertCursorPosToCurrentPos(cursorPoint);
		CRect thumbButtonRect = ConvertCurrentPosToThumbButtonRect(nCurrentPos);

		m_thumbButton.MoveWindow(thumbButtonRect, FALSE);

		if (GetPos() != nCurrentPos) // ��ġ�� ��ȭ�� ����ٸ�
		{
			SetPos(nCurrentPos);
			GetParent()->SendMessage(WM_COMMAND, MAKEWPARAM(GetDlgCtrlID(), WM_CHERRY_SLIDERCTRL_POS_CHANGED), (LPARAM)GetSafeHwnd());
		}
	}

	CCherryProgressCtrl::OnMouseMove(nFlags, point);
}

void CCherrySliderCtrl::OnLButtonUp(UINT nFlags, CPoint point)
{
	// thumb ��ư�� WM_LBUTTONUP �޽��� �߻� ��Ŵ
	m_thumbButton.SendMessage(WM_LBUTTONUP, 0, MAKELPARAM(point.x, point.y));

	CCherryProgressCtrl::OnLButtonUp(nFlags, point);
}

int CCherrySliderCtrl::ConvertCursorPosToCurrentPos(CPoint point)
{
	CRect clientRect;
	GetClientRect(clientRect);

	int nClientWidth = clientRect.Width();
	int nClientHeight = clientRect.Height();

	float fPos = 0.0;
	DWORD dwCherryStyle = GetCherryStyle();

	switch (dwCherryStyle)
	{
	// To left
	case STYLE_HORIZONTAL_RIGHT_TO_LEFT:
	case STYLE_HORIZONTAL_RIGHT_TO_LEFT_NOT_FOLLOW:
		fPos = (((float)(nClientWidth - point.x) / nClientWidth) * GetMaxPos()) - 1;	// rect���� Ŀ�� ��ġ�� ������� ���� pos ������ ��ȯ
		break;
	// To right
	case STYLE_HORIZONTAL_LEFT_TO_RIGHT:
	case STYLE_HORIZONTAL_LEFT_TO_RIGHT_NOT_FOLLOW:
		fPos = (((float)point.x / nClientWidth) * GetMaxPos()) + 1;						// rect���� Ŀ�� ��ġ�� ������� ���� pos ������ ��ȯ
		break;
	// To top
	case STYLE_VERTICAL_BOTTOM_TO_TOP:
	case STYLE_VERTICAL_BOTTOM_TO_TOP_NOT_FOLLOW:
		fPos = (((float)(nClientHeight - point.y) / nClientHeight) * GetMaxPos()) - 1;	// rect���� Ŀ�� ��ġ�� ������� ���� pos ������ ��ȯ
		break;
	// To bottom
	case STYLE_VERTICAL_TOP_TO_BOTTOM:
	case STYLE_VERTICAL_TOP_TO_BOTTOM_NOT_FOLLOW:
		fPos = (((float)point.y / nClientHeight) * GetMaxPos()) + 1;					// rect���� Ŀ�� ��ġ�� ������� ���� pos ������ ��ȯ
		break;
	default:
		ASSERT(0);
		break;
	}

	int nPos;

	if ((int)(fPos * 10) % 10 < 5)
		nPos = (int)floor(fPos);
	else
		nPos = (int)ceil(fPos);

	switch (dwCherryStyle)
	{
	case STYLE_HORIZONTAL_RIGHT_TO_LEFT:
	case STYLE_HORIZONTAL_RIGHT_TO_LEFT_NOT_FOLLOW:
	case STYLE_VERTICAL_BOTTOM_TO_TOP:
	case STYLE_VERTICAL_BOTTOM_TO_TOP_NOT_FOLLOW:
		nPos += 1;
		break;
	case STYLE_HORIZONTAL_LEFT_TO_RIGHT:
	case STYLE_HORIZONTAL_LEFT_TO_RIGHT_NOT_FOLLOW:
	case STYLE_VERTICAL_TOP_TO_BOTTOM:
	case STYLE_VERTICAL_TOP_TO_BOTTOM_NOT_FOLLOW:
		nPos -= 1;
		break;
	default:
		ASSERT(0);
		break;
	}

	return nPos;
}

CRect CCherrySliderCtrl::ConvertCurrentPosToThumbButtonRect(int nCurrentPos)
{
	CRect clientRect;
	GetClientRect(clientRect);

	int nClientWidth = clientRect.Width();
	int nClientHeight = clientRect.Height();

	CRect thumbButtonRect;
	m_thumbButton.GetWindowRect(thumbButtonRect);
	ScreenToClient(thumbButtonRect);

	int nThumbButtonWidth = thumbButtonRect.Width();
	int nThumbButtonHeight = thumbButtonRect.Height();

	int nThumbButtonCenter = 0;
	int nThumbButtonPos = 0;

	DWORD dwCherryStyle = GetCherryStyle();
	
	/////////////////////////////////////////////////////
	// 1. ��ư �߾� ��ġ ��������
	switch (dwCherryStyle)
	{
	// Horizontal
	case STYLE_HORIZONTAL_RIGHT_TO_LEFT:
	case STYLE_HORIZONTAL_RIGHT_TO_LEFT_NOT_FOLLOW:
	case STYLE_HORIZONTAL_LEFT_TO_RIGHT:
	case STYLE_HORIZONTAL_LEFT_TO_RIGHT_NOT_FOLLOW:
		nThumbButtonCenter = thumbButtonRect.Width() / 2;
		break;
	// Vertical
	case STYLE_VERTICAL_BOTTOM_TO_TOP:
	case STYLE_VERTICAL_BOTTOM_TO_TOP_NOT_FOLLOW:
	case STYLE_VERTICAL_TOP_TO_BOTTOM:
	case STYLE_VERTICAL_TOP_TO_BOTTOM_NOT_FOLLOW:
		nThumbButtonCenter = thumbButtonRect.Height() / 2;
		break;
	default:
		ASSERT(0);
		break;
	}

	/////////////////////////////////////////////////////
	// 2. ��ư�� ��ġ ���
	switch (dwCherryStyle)
	{
	// To left
	case STYLE_HORIZONTAL_RIGHT_TO_LEFT:
	case STYLE_HORIZONTAL_RIGHT_TO_LEFT_NOT_FOLLOW:
		nThumbButtonPos = ((nClientWidth * (GetMaxPos() - nCurrentPos)) / GetMaxPos()) - nThumbButtonCenter;
		break;
	// To right
	case STYLE_HORIZONTAL_LEFT_TO_RIGHT:
	case STYLE_HORIZONTAL_LEFT_TO_RIGHT_NOT_FOLLOW:
		nThumbButtonPos = ((nClientWidth * nCurrentPos) / GetMaxPos()) - nThumbButtonCenter;
		break;
	// To top
	case STYLE_VERTICAL_BOTTOM_TO_TOP:
	case STYLE_VERTICAL_BOTTOM_TO_TOP_NOT_FOLLOW:
		nThumbButtonPos = ((nClientHeight * (GetMaxPos() - nCurrentPos)) / GetMaxPos()) - nThumbButtonCenter;
		break;
	// To bottom
	case STYLE_VERTICAL_TOP_TO_BOTTOM:
	case STYLE_VERTICAL_TOP_TO_BOTTOM_NOT_FOLLOW:
		nThumbButtonPos = ((nClientHeight * nCurrentPos) / GetMaxPos()) - nThumbButtonCenter;
		break;
	default:
		ASSERT(0);
		break;
	}

	/////////////////////////////////////////////////////
	// 3. ���� ��ư ��ġ�� ��ǥ ����
	switch(dwCherryStyle)
	{
	// Horizontal
	case STYLE_HORIZONTAL_RIGHT_TO_LEFT:
	case STYLE_HORIZONTAL_RIGHT_TO_LEFT_NOT_FOLLOW:
	case STYLE_HORIZONTAL_LEFT_TO_RIGHT:
	case STYLE_HORIZONTAL_LEFT_TO_RIGHT_NOT_FOLLOW:
		thumbButtonRect.MoveToX(nThumbButtonPos);

		// ���� ���� �����
		if (0 > thumbButtonRect.left)
		{
			thumbButtonRect.left = 0;
			thumbButtonRect.right = nThumbButtonWidth;
		}
		// ������ ���� �����
		else if (nClientWidth < thumbButtonRect.right)
		{
			thumbButtonRect.left = nClientWidth - nThumbButtonWidth;
			thumbButtonRect.right = nClientWidth;
		}
		break;
	// Vertical
	case STYLE_VERTICAL_BOTTOM_TO_TOP:
	case STYLE_VERTICAL_BOTTOM_TO_TOP_NOT_FOLLOW:
	case STYLE_VERTICAL_TOP_TO_BOTTOM:
	case STYLE_VERTICAL_TOP_TO_BOTTOM_NOT_FOLLOW:
		thumbButtonRect.MoveToY(nThumbButtonPos);

		// ��� ���� �����
		if (0 > thumbButtonRect.top)
		{
			thumbButtonRect.top = 0;
			thumbButtonRect.bottom = nThumbButtonHeight;
		}
		// �ϴ� ���� �����
		else if (nClientHeight < thumbButtonRect.bottom)
		{
			thumbButtonRect.top = nClientHeight - nThumbButtonHeight;
			thumbButtonRect.bottom = nClientHeight;
		}
		break;
	default:
		ASSERT(0);
		break;
	}

	return thumbButtonRect;
}