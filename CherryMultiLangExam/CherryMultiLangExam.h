
// CherryMultiLangExam.h : PROJECT_NAME ���� ���α׷��� ���� �� ��� �����Դϴ�.
//

#pragma once

#ifndef __AFXWIN_H__
	#error "PCH�� ���� �� ������ �����ϱ� ���� 'stdafx.h'�� �����մϴ�."
#endif

#include "resource.h"		// �� ��ȣ�Դϴ�.


// CCherryMultiLangExamApp:
// �� Ŭ������ ������ ���ؼ��� CherryMultiLangExam.cpp�� �����Ͻʽÿ�.
//

class CCherryMultiLangExamApp : public CWinApp
{
public:
	CCherryMultiLangExamApp();

// �������Դϴ�.
public:
	virtual BOOL InitInstance();

// �����Դϴ�.

	DECLARE_MESSAGE_MAP()
};

extern CCherryMultiLangExamApp theApp;