
// CherryUpdateExamDlg.h : ��� ����
//

#pragma once

#include "CherryUpdate.h"
#ifdef _DEBUG
#pragma comment(lib, "CherryUpdateD.lib")
#else
#pragma comment(lib, "CherryUpdate.lib")
#endif

// CCherryUpdateExamDlg ��ȭ ����
class CCherryUpdateExamDlg : public CDialogEx
{
// �����Դϴ�.
public:
	CCherryUpdateExamDlg(CWnd* pParent = NULL);	// ǥ�� �������Դϴ�.

// ��ȭ ���� �������Դϴ�.
	enum { IDD = IDD_CHERRYUPDATEEXAM_DIALOG };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV �����Դϴ�.


	CCherryUpdate m_updater;

// �����Դϴ�.
protected:
	HICON m_hIcon;

	// ������ �޽��� �� �Լ�
	virtual BOOL OnInitDialog();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnBnClickedOk();
	afx_msg void OnBnClickedCancel();
	afx_msg void OnBnClickedButton1();
};
