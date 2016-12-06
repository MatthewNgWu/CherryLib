// FlyoutDlg.cpp : ���� �����Դϴ�.
//

#include "stdafx.h"
#include "CherryCtrlExam.h"
#include "FlyoutDlg.h"


// CFlyoutDlg ��ȭ �����Դϴ�.

IMPLEMENT_DYNAMIC(CFlyoutDlg, CDialogEx)

CFlyoutDlg::CFlyoutDlg(CWnd* pParent /*=NULL*/)
	: CCherryNotificationIconFlyout(CFlyoutDlg::IDD, pParent)
{
	
}

CFlyoutDlg::~CFlyoutDlg()
{
}

void CFlyoutDlg::DoDataExchange(CDataExchange* pDX)
{
	CCherryNotificationIconFlyout::DoDataExchange(pDX);
}


BEGIN_MESSAGE_MAP(CFlyoutDlg, CCherryNotificationIconFlyout)
	ON_WM_CREATE()
	ON_BN_CLICKED(1000, &CFlyoutDlg::OnBnClickedButton1)
END_MESSAGE_MAP()


// CFlyoutDlg �޽��� ó�����Դϴ�.

void CFlyoutDlg::PostNcDestroy()
{
	//delete this;

	CCherryNotificationIconFlyout::PostNcDestroy();
}

void CFlyoutDlg::OnDrawCherry(CCherryMemDC *pDC)
{
}

void CFlyoutDlg::OnBnClickedButton1()
{
	AfxMessageBox(_T("�׽�Ʈ"));
}


int CFlyoutDlg::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CCherryNotificationIconFlyout::OnCreate(lpCreateStruct) == -1)
		return -1;

	m_cherryButton.Create(_T("[CherryButton]\n�⺻ ������ ��ư(&A)"), _T("button.png"), CCherryButton::STYLE_AUTORESIZE, WS_CHILD | WS_VISIBLE, CCherryRect(0, 0, 0, 0), this, 1000);

	//m_button1.Create(_T("�׽�Ʈ"), WS_CHILD | WS_VISIBLE, CRect(0, 0, 50, 50), this, 1001);

	return 0;
}
