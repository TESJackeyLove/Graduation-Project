
// TestPID.h : PROJECT_NAME Ӧ�ó������ͷ�ļ�
//

#pragma once

#ifndef __AFXWIN_H__
	#error "�ڰ������ļ�֮ǰ������stdafx.h�������� PCH �ļ�"
#endif

#include "resource.h"		// ������


// CTestPIDApp: 
// �йش����ʵ�֣������ TestPID.cpp
//

class CTestPIDApp : public CWinApp
{
public:
	CTestPIDApp();

// ��д
public:
	virtual BOOL InitInstance();

// ʵ��

	DECLARE_MESSAGE_MAP()
};

extern CTestPIDApp theApp;