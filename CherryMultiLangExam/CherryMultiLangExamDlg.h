
// CherryMultiLangExamDlg.h : ��� ����
//

#pragma once
#include "afxwin.h"


// CCherryMultiLangExamDlg ��ȭ ����
class CCherryMultiLangExamDlg : public CDialogEx
{
// �����Դϴ�.
public:
	CCherryMultiLangExamDlg(CWnd* pParent = NULL);	// ǥ�� �������Դϴ�.

// ��ȭ ���� �������Դϴ�.
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_CHERRYMULTILANGEXAM_DIALOG };
#endif

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV �����Դϴ�.


// �����Դϴ�.
protected:
	HICON m_hIcon;

	// ������ �޽��� �� �Լ�
	virtual BOOL OnInitDialog();
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	CString GetCurrentPath();
	DECLARE_MESSAGE_MAP()
public:
	CEdit m_edit;
};
