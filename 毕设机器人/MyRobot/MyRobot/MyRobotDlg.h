
// MyRobotDlg.h : ͷ�ļ�
//

#pragma once

#include "GRB4Robot.h"
#include "afxwin.h"

// CMyRobotDlg �Ի���
class CMyRobotDlg : public CDialogEx
{
// ����
public:
	CMyRobotDlg(CWnd* pParent = NULL);	// ��׼���캯��
	CRobotBase *Robot;
	bool m_deviceflag;
	bool m_servoflag;

// �Ի�������
	enum { IDD = IDD_MYROBOT_DIALOG };

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
	afx_msg void OnBnClickedServoOn();
//	afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
	afx_msg void OnBnClickedButtonHome();
	// �򿪿������ؼ���ť�Ķ���
	CButton m_opendevice;
	// �ŷ���������
	CButton m_servo;
	afx_msg void OnBnClickedButtonTest();
};
