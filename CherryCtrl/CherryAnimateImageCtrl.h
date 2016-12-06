#pragma once

#include "CherryWnd.h"
#include "CherryImageList.h"
#include "CherryMemDC.h"
#include "CherryRect.h"

///////////////////////////////////////////////////////////////////////////
///
/// \class		CCherryAnimateImageCtrl
/// \brief		���ϸ��̼� �̹��� Ŭ����
/// \author		ogoons
/// \date		2013-07-10
/// \remark		
/// \section	
///
///////////////////////////////////////////////////////////////////////////

class AFX_EXT_CLASS CCherryAnimateImageCtrl : public CCherryWnd
{
	DECLARE_DYNAMIC(CCherryAnimateImageCtrl)

// Constructors
public:
	CCherryAnimateImageCtrl();
	virtual CHERRY_RET Create(
		CCherryImageList& imageList, 
		UINT nInterval, 
		UINT nAnimateNum,
		DWORD dwStyle,
		const CCherryRect& cherryRect, 
		CWnd* pParentWnd, 
		UINT nID);

// Attributes
protected:
	CCherryImageList m_imageList;	// �̹��� ����Ʈ
	INT_PTR m_nAnimateTimerRet;

	INT_PTR m_nImage;				// ���� �������� �̹����� Index
	UINT m_nInterval;				// �̹��� ���� Interval
	UINT m_nAnimateNum;			// �ݺ� Ƚ�� 0�� ��� ����
	UINT m_nAnimateCount;				// ���� �ݺ��� Ƚ��
	
// Operations
public:
	void		SetImageList(CCherryImageList& imageList);
	CHERRY_RET	ShowImage(INT_PTR nImage);
	
	void StartAnimate(UINT nInterval, UINT nAnimateNum = 0);
	void PauseAnimate();
	void StopAnimate();
	void SetAnimateInterval(UINT nInternval);
	void SetAnimateCount(UINT nAnimateNum);

// Implementation
public: 
	virtual ~CCherryAnimateImageCtrl();

protected:
	DECLARE_MESSAGE_MAP()

	virtual void OnDrawCherry(CCherryMemDC *pDC);

	afx_msg void OnTimer(UINT_PTR nIDEvent);	
};


