#pragma once

#include "CherryException.h"
#include "CherryMemDC.h"
#include "CherryFontByStatus.h"
#include "CherryImage.h"
#include "CherryWnd.h"
#include "CherryRect.h"

using namespace Gdiplus;


///////////////////////////////////////////////////////////////////////////
///
/// \class		CCherryButton
/// \brief		�Ϲ����� �̹��� ��ư�� �����Ѵ�. (���� ����)\n
///				��ư�� ���� = Normal, Hover, Down, Disable\n
///				1x4 ������ �̹����� ��ư�� �Ҵ��Ͽ� �� ���¸� ǥ���Ѵ�.
/// \author		ogoons
/// \date		2012-11-05
/// \remark		
/// \section	
///
///////////////////////////////////////////////////////////////////////////

class AFX_EXT_CLASS CCherryButton : public CButton, public CCherryFontByStatus
{
	DECLARE_DYNAMIC(CCherryButton)

	// ��ư ���� ����
	enum STATUS
	{
		STATUS_NORMAL,
		STATUS_HOVER,
		STATUS_DOWN,
		STATUS_DISABLE,
		STATUS_MAX_COUNT
	};

	// ��ư ��Ÿ�� ����
	enum STYLE
	{
		STYLE_AUTORESIZE = 0x0001,
	};

// Constructors
public:
	CCherryButton();
	virtual CHERRY_RET Create(
		LPCTSTR lpszCaption, 
		LPCTSTR lpszImagePath, 
		DWORD dwCherryStyle, 
		DWORD dwStyle, 
		CCherryRect &cherryRect,
		CWnd *pParentWnd, 
		UINT nID);	// ��ư ����(�ʼ� ���)
	
// Attributes
protected:
	CCherryMemDC	*m_pBackMemDC;
	CCherryImage	m_images[STATUS_MAX_COUNT];		// 4���� ����(Normal, Hover, Down, Disable)�� �̹��� ����
	
	BOOL			m_bTracking;	// Ʈ��ŷ ����
	BOOL			m_bHover;		// ���콺 Hover ���� ����
	BOOL			m_bEnableHoverHandCursor;
	DWORD			m_dwCherryStyle;

	CRect			m_textRect;
	int				m_nOffsetTextLeft;
	int				m_nOffsetTextTop;

// Operations
public:
	CHERRY_RET								SetImage(LPCTSTR lpszImagePath);		// ��ư �̹��� ����
	void									OffsetText(int nLeft, int nTop);		// �ؽ�Ʈ ��ġ �̵�
	void									EnableHoverHandCursor(BOOL bEnable);
	void									SetBackground(CDC *pDC);

	DWORD									GetCherryStyle() const;
	void									ModifyCherryStyle(DWORD dwRemove, DWORD dwAdd);
	void									EnableAutoResize(BOOL bEnable);

protected:
	void									ResizeWindow(UINT nWidth, UINT nHeight);

// Implementation
public:
	virtual ~CCherryButton();

protected:
	DECLARE_MESSAGE_MAP()

	virtual BOOL	PreTranslateMessage(MSG* pMsg);
	virtual void	DrawItem(LPDRAWITEMSTRUCT lpDrawItemStruct);	// Owner draw �� ���Ǹ� OnPaint�� ������ �ʴ´�.

	afx_msg void	OnMouseMove(UINT nFlags, CPoint point);
	afx_msg void	OnMouseHover(UINT nFlags, CPoint point);
	afx_msg void	OnMouseLeave();
	afx_msg void	OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void	OnLButtonUp(UINT nFlags, CPoint point);
	afx_msg BOOL	OnEraseBkgnd(CDC* pDC);
	afx_msg void	OnEnable(BOOL bEnable);

	virtual void	OnDrawCherry(CCherryMemDC *pDC);
};
	