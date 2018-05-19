
// MyRobotDlg.h : ͷ�ļ�
//

#pragma once

#include "GRB4Robot.h"
#include "Impedance.h"
#include "afxwin.h"
#include "afxcmn.h"

struct RobotData
{
	double JointsNow[4];
	double JointsNext[4];
	double JointsVelNow[4];
	double JointsVelNext[4];
	double Origin6axisForce[6];
	double JointsTorque[4];
	double CartesianPositionNow[4];
	double CartesianPositionNext[4];
	double CartesianVelNow[4];
	double CartesianVelNext[4];
};
// CMyRobotDlg �Ի���
class CMyRobotDlg : public CDialogEx
{
// ����
public:
	CMyRobotDlg(CWnd* pParent = NULL);	// ��׼���캯��
	CRobotBase *Robot;
	CImpedance *ImpedanceController;
	bool m_deviceflag;
	bool m_servoflag;
	bool m_ImpedanceButtonflag;
	void OnJointsDataShow();	
	void OnToolDataShow();

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

	CButton m_opendevice;//���ư�ť�ܷ񱻵���������ؼ���ť�Ķ���
	CButton m_servo;//���ư�ť�ܷ񱻵��  �ŷ���������
	CButton m_ImpedanceButton;   //���ư�ť�ܷ񱻵��

	afx_msg void OnBnClickedButtonTest();
	afx_msg void OnBnClickedButtonImpedance();
	afx_msg void OnTimer(UINT_PTR nIDEvent);

	afx_msg void OnBnClickedButtonJoint1Negative();
	afx_msg void OnBnClickedButtonJoint1Positive();
	afx_msg void OnBnClickedButtonJoint2Positive();
	afx_msg void OnBnClickedButtonJoint2Negative();
	afx_msg void OnBnClickedButtonJoint3Negative();
	afx_msg void OnBnClickedButtonJoint3Positive();
	afx_msg void OnBnClickedButtonJoint4Positive();
	afx_msg void OnBnClickedButtonJoint4Negative();
	afx_msg void OnBnClickedButtonGohome();
	CIPAddressCtrl m_ServerIPAddr;
	CEdit m_ServerPort;
	afx_msg void OnBnClickedButtonConnectserver();

	void CMyRobotDlg::update(CString s)
	{
		m_TCPMessage.AddString(s);
	}
	CListBox m_TCPMessage;
};
