#pragma once


#include "CherryMemDC.h"
#include "CherryImageList.h"
#include "CherryHeaderCtrl.h"
#include "CherryRect.h"

///////////////////////////////////////////////////////////////////////////
///
/// \class		CCherryListCtrl
/// \brief		REPORT ������ List control
/// \author		ogoons
/// \date		2014-03-20
/// \remark		
/// \section	
///
///////////////////////////////////////////////////////////////////////////

class AFX_EXT_CLASS CCherryListCtrl : public CListCtrl
{
	DECLARE_DYNAMIC(CCherryListCtrl)

	enum ITEM_STATUS
	{
		ITEM_STATUS_NORMAL,
		ITEM_STATUS_SELECTED,
		ITEM_STATUS_MAX_COUNT
	};

// Constructors
public:
	CCherryListCtrl();
	CHERRY_RET Create(
		LPCTSTR lpszHeaderImagePath,
		LPCTSTR lpszItemImagePath,
		UINT nHeaderHeight,
		UINT nItemHeight,
		DWORD dwCherryStyle,
		DWORD dwStyle,
		const CCherryRect &cherryRect,
		CWnd *pParentWnd,
		UINT nID);

// Attributes
protected:
	CCherryHeaderCtrl m_headerCtrl;

	CCherryImage m_itemImage[ITEM_STATUS_MAX_COUNT];
	
	// Font style
	CCherryFont		*m_pCurrentFont;
	CCherryFont		m_normalFont;
	CCherryFont		m_selectedFont;
	CCherryFont		m_disableFont;

// Operations
public:
	CHERRY_RET		SetImage(LPCTSTR lpszHeaderImagePath, LPCTSTR lpszItemImagePath);
	CHERRY_RET		SetHeaderImage(LPCTSTR lpszImagePath);
	CHERRY_RET		SetItemImage(LPCTSTR lpszImagePath);

	CCherryHeaderCtrl *GetCherryHeaderCtrl() const { return (CCherryHeaderCtrl *)GetHeaderCtrl(); };

	// Font style
	// ��� ���� ��Ʈ�� �����Ѵ�.(Disable Į��� �⺻ �׷��� ����)
	void SetCherryFont(LPCTSTR lpszFontName, float fFontSize, Color fontColor, DWORD dwFontStyle);
	BOOL SetCherryFont(CCherryFont *pCherryFont);

	// ���º� ��Ʈ ����
	void SetFontByStatus(CCherryFont *pNormalFont, CCherryFont *pSelectedFont, CCherryFont *pDisableFont);

	// Normal ��Ʈ ����
	void SetNormalFont(LPCTSTR lpszFontName, float fFontSize, Color fontColor, DWORD dwFontStyle);
	BOOL SetNormalFont(CCherryFont *pNormalFont);
	void SetNormalFontName(LPCTSTR lpszFontName);
	void SetNormalFontSize(float fFontSize);
	void SetNormalFontColor(Color fontColor);
	void SetNormalFontStyle(DWORD dwFontStyle);

	// Selected ��Ʈ ����
	void SetSelectedFont(LPCTSTR lpszFontName, float fFontSize, Color fontColor, DWORD dwFontStyle);
	BOOL SetSelectedFont(CCherryFont *pSelectedFont);
	void SetSelectedFontName(LPCTSTR lpszFontName);
	void SetSelectedFontSize(float fFontSize);
	void SetSelectedFontColor(Color fontColor);
	void SetSelectedFontStyle(DWORD dwFontStyle);

	// Disable ��Ʈ ����
	void SetDisableFont(LPCTSTR lpszFontName, float fFontSize, Color fontColor, DWORD dwFontStyle);
	BOOL SetDisableFont(CCherryFont *pDisableFont);
	void SetDisableFontName(LPCTSTR lpszFontName);
	void SetDisableFontSize(float fFontSize);
	void SetDisableFontColor(Color fontColor);
	void SetDisableFontStyle(DWORD dwFontStyle);

	void GetFontByStatus(CCherryFont &normalFont, CCherryFont &selectedFont, CCherryFont &disableFont) const;

protected:
	BOOL			SetHeaderHeight(UINT nHeight);
	CHERRY_RET		SetItemHeight(UINT nHeight);

	// Font style
	CCherryFont	*GetNormalFont();
	CCherryFont	*GetSelectedFont();
	CCherryFont	*GetDisableFont();

	void SetCurrentFont(CCherryFont *pStatusFont);
	CCherryFont	*GetCurrentFont() const;
	
// Implementation
public:
	virtual ~CCherryListCtrl();

protected:
	DECLARE_MESSAGE_MAP()

	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnNMCustomdraw(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
	afx_msg virtual void PreSubclassWindow();
};
