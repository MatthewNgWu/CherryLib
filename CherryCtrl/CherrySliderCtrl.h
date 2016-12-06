#pragma once

#include "CherryException.h"
#include "CherryButton.h"
#include "CherryMemDC.h"
#include "CherryImage.h"
#include "CherryProgressCtrl.h"

///////////////////////////////////////////////////////////////////////////
///
/// \class		CCherrySliderCtrlThumbButton
/// \brief		�����̴� ��Ʈ�� Thumb ��ư
/// \author		ogoons
/// \date		2014-01-01
/// \remark		
/// \section	
///
///////////////////////////////////////////////////////////////////////////

class CCherrySliderCtrlThumbButton : public CCherryButton
{
	DECLARE_DYNAMIC(CCherrySliderCtrlThumbButton)

// Constructors
public:
	CCherrySliderCtrlThumbButton();

// Attributes
protected:
	BOOL m_bLButtonDown;

// Operations
public:
	BOOL IsLButtonDown() { return m_bLButtonDown; };
	void UseCachedImage(BOOL bUse = TRUE);

// Overridables
protected:

// Implementation
public:
	virtual ~CCherrySliderCtrlThumbButton();

protected:
	DECLARE_MESSAGE_MAP()

	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
};

///////////////////////////////////////////////////////////////////////////
///
/// \class		CCherrySliderCtrl
/// \brief		�����̴� ��Ʈ��
/// \author		ogoons
/// \date		2013-07-10
/// \remark		
/// \section	
///
///////////////////////////////////////////////////////////////////////////

// Message
#define WM_CHERRY_SLIDERCTRL_POS_CHANGED WM_USER + 1

// Register Message Map
#define ON_WM_CHERRY_SLIDERCTRL_POS_CHANGED(id, memberFxn) \
	ON_CONTROL(WM_CHERRY_SLIDERCTRL_POS_CHANGED, id, memberFxn)

class AFX_EXT_CLASS CCherrySliderCtrl : public CCherryProgressCtrl
{
	DECLARE_DYNAMIC(CCherrySliderCtrl)

public:
	// �����̴� ��Ÿ�� ����
	enum STYLE
	{
		// 2013-07-17 ogoons: ����!
		// CherryProgressCtrl�� style ���� ��ġ��Ų��.
		STYLE_AUTORESIZE = 0x0001,

		// Thumb ��ġ ��ŭ ��� �̹����� ä������.
		STYLE_HORIZONTAL_LEFT_TO_RIGHT		= 0x0002,
		STYLE_HORIZONTAL_RIGHT_TO_LEFT		= 0x0004,
		STYLE_VERTICAL_BOTTOM_TO_TOP		= 0x0008,
		STYLE_VERTICAL_TOP_TO_BOTTOM		= 0x0010,

		// ��� �̹����� Thumb ��ġ�� ���� ä������ �ʴ´�.
		STYLE_HORIZONTAL_LEFT_TO_RIGHT_NOT_FOLLOW		= 0x0020,
		STYLE_HORIZONTAL_RIGHT_TO_LEFT_NOT_FOLLOW		= 0x0040,
		STYLE_VERTICAL_BOTTOM_TO_TOP_NOT_FOLLOW			= 0x0080,
		STYLE_VERTICAL_TOP_TO_BOTTOM_NOT_FOLLOW			= 0x0100,
	};

// Constructors
public:
	CCherrySliderCtrl();
	virtual CHERRY_RET Create(
		LPCTSTR lpszThumbImagePath,
		LPCTSTR lpszTrackImagePath, 
		const int nMaxRange,
		DWORD dwCherryStyle, 
		DWORD dwStyle,
		const CCherryRect &cherryRect,
		CWnd *pParentWnd, 
		UINT nID);	// ��ư ����(�ʼ� ���)
	

// Attributes
protected:
	CCherryImage					m_trackImage;
	CCherrySliderCtrlThumbButton	m_thumbButton;

// Operations
public:
	CHERRY_RET			SetThumbImage(LPCTSTR lpszImagePath);
	CHERRY_RET			SetTrackImage(LPCTSTR lpszImagePath);
	
protected:
	void									ResizeWindow(UINT nWidth, UINT nHeight);
	void									RepositionThumbButton(int nTrackWidth, int nTrackHeight);
	int										ConvertCursorPosToCurrentPos(CPoint point);
	CRect									ConvertCurrentPosToThumbButtonRect(int nCurrentPos);

// Implementation
public:
	virtual ~CCherrySliderCtrl();

	afx_msg void OnMouseMove(UINT nFlags, CPoint point);

protected:
	DECLARE_MESSAGE_MAP()

	virtual void OnDrawCherry(CCherryMemDC *pDC);

	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
};