
// TestPIDDlg.h : ͷ�ļ�
//

#pragma once

#include "GRB4Robot.h"
#include "afxwin.h"
#include "ChartCtrl/ChartCtrl.h"
#include "ChartCtrl/ChartTitle.h"
#include "ChartCtrl/ChartAxisLabel.h"
#include "ChartCtrl/ChartLineSerie.h"
#include "ChartCtrl/ChartAxis.h"

// CTestPIDDlg �Ի���
class CTestPIDDlg : public CDialogEx
{
// ����
public:
	CTestPIDDlg(CWnd* pParent = NULL);	// ��׼���캯��
	CRobotBase *Robot;
	bool m_deviceflag;
	bool m_servoflag;
	CChartCtrl m_ChartCtrl1;
// �Ի�������
	enum { IDD = IDD_TESTPID_DIALOG };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV ֧��

// ʵ��
protected:
	HICON m_hIcon;

	// ���ɵ���Ϣӳ�亯��
	virtual BOOL OnInitDialog();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnBnClickedOpencontroller();
	CButton m_opendevice;
	CButton m_servo;
	afx_msg void OnBnClickedButtonHome();
	afx_msg void OnBnClickedButtonServoOn();
	afx_msg void OnBnClickedButtonTest();
	afx_msg void OnBnClickedButtonStartshow();


};
