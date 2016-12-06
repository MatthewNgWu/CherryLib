#pragma once

#include "CherryException.h"
#include "CherryImage.h"
#include "CherryWnd.h"
#include "CherryFont.h"
#include "CherryMemDC.h"
#include "CherryRect.h"

///////////////////////////////////////////////////////////////////////////
///
/// \class		CCherryProgressCtrl
/// \brief		Progress ��Ʈ�� Ŭ����\n
///				��Ų�� ���� �� �ִ� Progress ��Ʈ���̴�.\n
///				2x1 ������ ���α׷��� �̹����� �Ҵ��Ͽ� ���� ���� ���¸� ���� ǥ���Ѵ�.\n
///				1x1�� �ش��ϴ� �̹����� ���� ���� �̹��� 1x2�� ��� �̹����� ������ �־�� �Ѵ�.\n
///				Status: 4������ ����� ���α׷��� �̹��� ���� ������ ������ �� �ִ�.\n
///				Range: 1 ~ 32768 (short)
/// \author		ogoons
/// \date		2013-09-24
/// \remark		
/// \section	
///
///////////////////////////////////////////////////////////////////////////

class AFX_EXT_CLASS CCherryProgressCtrl : public CCherryWnd, public CCherryFont
{
	DECLARE_DYNAMIC(CCherryProgressCtrl)

public:
	enum DRAW_TYPE
	{
		DRAW_TYPE_FILL,
		DRAW_TYPE_BACKGROUND,
		DRAW_TYPE_COUNT
	};

	enum STYLE
	{
		STYLE_AUTORESIZE = 0x0001,

		STYLE_HORIZONTAL_LEFT_TO_RIGHT = 0x0002,
		STYLE_HORIZONTAL_RIGHT_TO_LEFT = 0x0004,
		STYLE_VERTICAL_BOTTOM_TO_TOP = 0x0008,
		STYLE_VERTICAL_TOP_TO_BOTTOM = 0x0010,
	};

// Constructors
public:
	CCherryProgressCtrl();
	virtual CHERRY_RET Create(
		LPCTSTR lpszImagePath, 
		const short nMaxRange, 
		DWORD dwCherryStyle,
		DWORD dwStyle, 
		const CCherryRect &cherryRect,
		CWnd *pParentWnd, UINT nID);

// Attributes
protected:
	CCherryImage	m_images[DRAW_TYPE_COUNT];
	
	int m_nMaxPos;
	int m_nPos;

	DWORD m_dwCherryStyle;
	
// Operations
public:
	CHERRY_RET			SetImage(LPCTSTR lpszImagePath, int nWidth, int nHeight);
	void				SetPos(int nPos);
	CHERRY_RET			SetMaxPos(int nMaxPos);

	//void								SetStyle(CherryProgressCtrl::STYLE style);

	DWORD								GetCherryStyle() const;
	void								ModifyCherryStyle(DWORD dwRemove, DWORD dwAdd);
	void								EnableAutoResize(BOOL bEnable);

	int									GetPos() const;
	int									GetMaxPos() const;

protected:
	void								ResizeWindow(UINT nWidth, UINT nHeight);

// Implementation
public:
	virtual ~CCherryProgressCtrl();

protected:
	DECLARE_MESSAGE_MAP()

	virtual void OnDrawCherry(CCherryMemDC *pDC);
};
