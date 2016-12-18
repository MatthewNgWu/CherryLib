#include "stdafx.h"
#include "CherryRadioButton.h"


// CCherryRadioButton

IMPLEMENT_DYNAMIC(CCherryRadioButton, CCherryCheckBox)

CCherryRadioButton::CCherryRadioButton()
{
	m_pHeadRadioButton = NULL;
}

CCherryRadioButton::~CCherryRadioButton()
{
}

BEGIN_MESSAGE_MAP(CCherryRadioButton, CCherryCheckBox)
	ON_WM_LBUTTONUP()
	ON_WM_DESTROY()
END_MESSAGE_MAP()

// CCherryRadioButton �޽��� ó�����Դϴ�.
CHERRY_RET CCherryRadioButton::Create(LPCTSTR lpszCaption, LPCTSTR lpszImagePath, DWORD dwCherryStyle, CCherryRadioButton *pHeadRadioButton, DWORD dwStyle, const CCherryRect &cherryRect, CWnd *pParentWnd, UINT nID)
{
	CHERRY_RET cherryRet = CCherryException::ERROR_CHERRY_SUCCESS;

	try
	{
		dwStyle |= BS_OWNERDRAW;	// �߰�

		if (!CButton::Create(lpszCaption, dwStyle, cherryRect, pParentWnd, nID))
			throw CCherryException::ERROR_RADIOBUTTON_CREATE_FAIL;

		// CherryCheckBox ���� ��Ÿ�� bit �÷���
		ModifyCherryStyle(0, dwCherryStyle);

		if (dwCherryStyle & STYLE_BUTTONTYPE)
		{
			GetNormalFont()->SetFontStyle(CCherryFont::STYLE_CENTER | CCherryFont::STYLE_VCENTER);
			GetHoverFont()->SetFontStyle(CCherryFont::STYLE_CENTER | CCherryFont::STYLE_VCENTER);
			GetDownFont()->SetFontStyle(CCherryFont::STYLE_CENTER | CCherryFont::STYLE_VCENTER);
			GetDisableFont()->SetFontStyle(CCherryFont::STYLE_CENTER | CCherryFont::STYLE_VCENTER);
		}
		else
		{
			// �⺻ ���� ����
			GetNormalFont()->SetFontStyle(CCherryFont::STYLE_VCENTER);
			GetHoverFont()->SetFontStyle(CCherryFont::STYLE_VCENTER);
			GetDownFont()->SetFontStyle(CCherryFont::STYLE_VCENTER);
			GetDisableFont()->SetFontStyle(CCherryFont::STYLE_VCENTER);
		}

		// �⺻ ��Ȱ��ȭ �ؽ�Ʈ �Ӽ�
		COLORREF disableColor = GetSysColor(COLOR_GRAYTEXT);
		GetDisableFont()->SetFontColor(Color(GetRValue(disableColor), GetGValue(disableColor), GetBValue(disableColor)));

		// ����� �׷����� �ڵ��� ������ �ִ´�.
		m_pHeadRadioButton = pHeadRadioButton;

		if (pHeadRadioButton)
		{
			pHeadRadioButton->AddRadioVector(this);
		}
		else
		{
			AddRadioVector(this);
			SetCheck(STATUS_CHECKED);
		}

		cherryRet = SetImage(lpszImagePath);
	}
	catch (const CHERRY_RET &errorRet)
	{
		cherryRet = errorRet;
	}

	return cherryRet;
}

void CCherryRadioButton::SetCheck(STATUS checkStatus)
{
	if (!GetSafeHwnd())
		return;

	// �ڱ� �ڽ��� üũ
	m_checkStatus = checkStatus;

	Invalidate(FALSE);

	//if (checkStatus == STATUS_CHECKED)
		// �θ� �����쿡�� �� ��Ʈ���� Ŭ�� �޽��� �߻� ��Ŵ
		//GetParent()->SendMessage(WM_COMMAND, MAKEWPARAM(GetDlgCtrlID(), BN_CLICKED));

	if (GetCherryStyle() & STYLE_AUTOCHECKBOX)
	{
		vector<CCherryRadioButton *> *pRadioVector = GetHeadRadioVector();
		for (vector<CCherryRadioButton *>::iterator it = pRadioVector->begin(); it != pRadioVector->end(); ++it)
		{
			// ���� �ڽ� �� ������ ���
			if (((CCherryRadioButton *)*it)->GetSafeHwnd() && GetSafeHwnd() != ((CCherryRadioButton *)*it)->GetSafeHwnd())
			{
				// �������� ���� üũ ����
				((CCherryRadioButton *)*it)->m_checkStatus = STATUS_UNCHECKED;
				((CCherryRadioButton *)*it)->Invalidate(FALSE);
			}
		}
	}
}

void CCherryRadioButton::AddRadioVector(CCherryRadioButton *pRadioButton)
{
	m_radioVector.push_back(pRadioButton);
}

vector<CCherryRadioButton *> *CCherryRadioButton::GetRadioVector()
{
	return &m_radioVector;
}

vector<CCherryRadioButton *> *CCherryRadioButton::GetHeadRadioVector()
{
	vector<CCherryRadioButton *> *pRadioVector = NULL;

	if (m_pHeadRadioButton) // �׷���� ���
		pRadioVector = m_pHeadRadioButton->GetRadioVector();
	else // �׷����� ���
		pRadioVector = &m_radioVector;

	return pRadioVector;
}

CCherryRadioButton *CCherryRadioButton::GetHeadRadioButton() const
{
	return m_pHeadRadioButton;
}

void CCherryRadioButton::SetHeadRadioButton(CCherryRadioButton *pRadioButton)
{
	m_pHeadRadioButton = pRadioButton;
}

void CCherryRadioButton::OnLButtonUp(UINT nFlags, CPoint point)
{
	SetCheck(STATUS_CHECKED);

	CCherryButton::OnLButtonUp(nFlags, point); // warning: CCherryCheckBox�� call���� �ʴ´�.
}

LRESULT CCherryRadioButton::DefWindowProc(UINT message, WPARAM wParam, LPARAM lParam)
{
	/*
	if (message == WM_COMMAND && HIWORD(wParam) == BN_CLICKED)
	{
		UINT nCtrlID = LOWORD(wParam);
		
	}
	*/

	return CCherryCheckBox::DefWindowProc(message, wParam, lParam);
}

void CCherryRadioButton::OnDestroy()
{
	CCherryCheckBox::OnDestroy();

	vector<CCherryRadioButton *> *pRadioVector = GetHeadRadioVector();

	for (vector<CCherryRadioButton *>::iterator it = pRadioVector->begin(); it != pRadioVector->end(); ++it)
	{
		// �ڱ� �ڽ��� ���Ϳ��� ��������
		if (GetSafeHwnd() == ((CCherryRadioButton *)*it)->GetSafeHwnd())
		{
			pRadioVector->erase(it);

			break;
		}
	}

	// destroy �Ǵ� �ڽ��� �׷����� ��� (�׷��� �����ϱ�)
	if (NULL == m_pHeadRadioButton)
	{
		for (vector<CCherryRadioButton *>::iterator it = pRadioVector->begin(); it != pRadioVector->end(); ++it)
		{
			// �׷����� ���ŵ� ���� �߿� �����ִ� �� �߿� ù ��° ���� �׷������� ��������.
			if (it == pRadioVector->begin())
			{
				((CCherryRadioButton *)*it)->GetRadioVector()->assign(pRadioVector->begin(), pRadioVector->end()); // �׷��忡�� ���� ����
				((CCherryRadioButton *)*it)->SetHeadRadioButton(NULL); // NULL �� �׷��� ���� �Ӹ�

				continue;
			}

			((CCherryRadioButton *)*it)->SetHeadRadioButton(pRadioVector->at(0)); // ������ �׷������ �׷����� �����͸� ������ �־�߰ڴ�.
		}
	}
}
