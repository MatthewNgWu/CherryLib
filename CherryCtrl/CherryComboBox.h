#pragma once

#include "CherryException.h"
#include "CherryMemDC.h"
#include "CherryFont.h"
#include "CherryImage.h"
#include "CherryButton.h"
#include "CherryFontByStatus.h"

///////////////////////////////////////////////////////////////////////////
///
/// \class		CCherryComboBox
/// \brief		�޺� �ڽ� Ŭ����
/// \author		ogoons
/// \date		2013-07-10
/// \remark		
/// \section	
///
///////////////////////////////////////////////////////////////////////////

class AFX_EXT_CLASS CCherryComboBox : public CComboBox, public CCherryFont, public CCherryFontByStatus
{
	DECLARE_DYNAMIC(CCherryComboBox)

	// ��ư ���� ����
	enum STATUS
	{
		STATUS_NORMAL,
		STATUS_HOVER,
		STATUS_DOWN,
		STATUS_DISABLE,
		STATUS_MAX_COUNT
	};

	enum STYLE
	{
		STYLE_AUTORESIZE = 0x0001,
	};

// Constructors
public:
	CCherryComboBox();
	virtual CHERRY_RET Create(
		LPCTSTR lpszImagePath,
		DWORD dwCherryStyle,
		DWORD dwStyle, 
		const CCherryRect &cherryRect,
		CWnd *pParentWnd, 
		UINT nID);

// Attributes
protected:
	CCherryMemDC	*m_pBackMemDC;
	CCherryImage	*m_pCurrentImage;
	CCherryImage	m_images[STATUS_MAX_COUNT];		// 4���� ����(Normal, Hover, Down, Disable)�� �̹��� ����

	BOOL			m_bTracking;	// Ʈ��ŷ ����
	BOOL			m_bHover;		// ���콺 Hover ���� ����
	BOOL			m_bDropDown;
	BOOL			m_bEnableHoverHandCursor;
	DWORD			m_dwCherryStyle;

	CRect			m_textRect;
	int				m_nOffsetTextLeft;
	int				m_nOffsetTextTop;

	CFont			m_font;
	COLORREF		m_textColor;

// Operations
public:
	CHERRY_RET								SetImage(LPCTSTR lpszImagePath);		// ��ư �̹��� ����
	void									OffsetText(int nLeft, int nTop);				// �ؽ�Ʈ ��ġ �̵�
	void									EnableHoverHandCursor(BOOL bEnable);
	void									SetBackground(CDC *pDC);

	DWORD									GetCherryStyle() const;
	void									ModifyCherryStyle(DWORD dwRemove, DWORD dwAdd);
	void									EnableAutoResize(BOOL bEnable);

	// ComboBox Button ��Ʈ ����
	void									SetCherryFont(LPCTSTR lpszFontName, float fFontSize, Color fontColor, DWORD dwFontStyle);
	BOOL									SetCherryFont(CCherryFont *pCherryFont);

	// DropDownList ��Ʈ ����
	void									SetDropDownListFont(LPCTSTR lpszFontName, LONG lFontSize, Color fontColor, DWORD dwFontStyle);
	BOOL									SetDropDownListFont(CCherryFont *pCherryFont);
	void									SetDropDownListFontName(LPCTSTR lpszFontName);
	void									SetDropDownListFontSize(LONG lFontSize);
	void									SetDropDownListFontColor(Color fontColor);
	void									SetDropDownListFontStyle(DWORD dwFontStyle);

// Overridables
	int										SetCurSel(int nSelect);

protected:
	void									ResizeWindow(UINT nWidth, UINT nHeight);

// Implementation
public:
	virtual ~CCherryComboBox();

protected:
	DECLARE_MESSAGE_MAP()

	virtual void	DrawItem(LPDRAWITEMSTRUCT /*lpDrawItemStruct*/);
	virtual void	MeasureItem(LPMEASUREITEMSTRUCT /*lpMeasureItemStruct*/);
	virtual BOOL	PreTranslateMessage(MSG* pMsg);

	afx_msg void	OnPaint();
	afx_msg void	OnLButtonDown();
	afx_msg void	OnMouseMove(UINT nFlags, CPoint point);
	afx_msg void	OnMouseHover(UINT nFlags, CPoint point);
	afx_msg void	OnMouseLeave();
	afx_msg void	OnCbnDropDown();
	afx_msg void	OnCbnCloseUp();
	afx_msg BOOL	OnEraseBkgnd(CDC* pDC);
	afx_msg void	OnEnable(BOOL bEnable);
	afx_msg void	OnDestroy();
};


