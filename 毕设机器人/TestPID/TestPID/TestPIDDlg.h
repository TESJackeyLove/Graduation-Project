
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
	CChartLineSerie* m_pLineSerie1;
	CChartLineSerie* m_pLineSerie2;
	CChartLineSerie* m_pLineSerie3;
	double m_HightSpeedChartArray1[2096];
	double m_HightSpeedChartArray2[2096];
	double m_HightSpeedChartArray3[2096];
	double m_X[2096];
	unsigned int m_count;
	const size_t m_c_arrayLength;
//	void LeftMoveArray(double* ptr, size_t length, double data);
//	void RandArray(double* ptr, size_t length);
	double randf(double min, double max);
// �Ի�������
	enum { IDD = IDD_TESTPID_DIALOG };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV ֧��
	void CTestPIDDlg::drawMoving();
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


	afx_msg void OnTimer(UINT_PTR nIDEvent);
	afx_msg void OnBnClickedButtonStop();
	afx_msg void OnBnClickedButtonRestart();
	afx_msg void OnBnClickedButtonStepResponse();
};
