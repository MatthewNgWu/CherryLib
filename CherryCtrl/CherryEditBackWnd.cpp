// CherryEditBackWnd.cpp : ���� �����Դϴ�.
//

#include "stdafx.h"
#include "CherryEditBackWnd.h"

// CCherryEditBackWnd

IMPLEMENT_DYNAMIC(CCherryEditBackWnd, CCherryWnd)

CCherryEditBackWnd::CCherryEditBackWnd()
{
}

CCherryEditBackWnd::~CCherryEditBackWnd()
{
}

BEGIN_MESSAGE_MAP(CCherryEditBackWnd, CCherryWnd)
	//ON_WM_CTLCOLOR()
END_MESSAGE_MAP()

// CCherryEditBackWnd �޽��� ó�����Դϴ�.

CHERRY_RET CCherryEditBackWnd::Create(LPCTSTR lpszBackImagePath, DWORD dwStyle, const CCherryRect &cherryRect, CWnd* pParentWnd, UINT nID)
{
	CHERRY_RET cherryRet = CCherryException::ERROR_CHERRY_SUCCESS;

	try
	{
		if ((cherryRet = SetImage(lpszBackImagePath)) != CCherryException::ERROR_CHERRY_SUCCESS)
			throw cherryRet;

		CRect backWndRect(cherryRect);

		if ((cherryRect.right - cherryRect.left) <= 0)
			backWndRect.right = cherryRect.left + m_backImage.GetWidth();

		if ((cherryRect.bottom - cherryRect.top) <= 0)
			backWndRect.bottom = cherryRect.top + m_backImage.GetHeight();

		if (!CCherryWnd::Create(NULL, NULL, dwStyle, backWndRect, pParentWnd, nID + 1000))
			throw CCherryException::ERROR_EDITBACKWND_CREATE_FAIL;
	}
	catch (const CHERRY_RET &errorRet)
	{
		cherryRet = errorRet;
	}

	return cherryRet;
}

CHERRY_RET CCherryEditBackWnd::SetImage(LPCTSTR lpszImagePath)
{
	return m_backImage.LoadImage(lpszImagePath);
}

void CCherryEditBackWnd::OnDrawCherry(CCherryMemDC *pDC)
{
	if (FALSE == m_backImage.IsLoadedImage())
		return;

	CRect clientRect;
	GetClientRect(&clientRect);

	Graphics graphics(pDC->GetSafeHdc());

	m_backImage.DrawImage(&graphics, clientRect);
}

//HBRUSH CCherryEditBackWnd::OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor)
//{
//	HBRUSH hbr = CWnd::OnCtlColor(pDC, pWnd, nCtlColor);
//
//	//switch (nCtlColor)
//	//{
//	//case CTLCOLOR_EDIT:
//
//	//	//if (pWnd->GetDlgCtrlID() == this->GetDlgCtrlID() + 1000)
//	//	//{
//	//		pDC->SetTextColor(RGB(255, 255, 255));
//	//		pDC->SetBkMode(TRANSPARENT);
//	//		hbr = (HBRUSH)GetStockObject(NULL_BRUSH);
//	//	//}
//	//	break;
//	//}
//
//	return hbr;
//}
