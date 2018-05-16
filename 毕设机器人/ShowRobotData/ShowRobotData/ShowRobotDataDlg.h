
// ShowRobotDataDlg.h : ͷ�ļ�
//

#pragma once
#include "afxwin.h"
#include "ChartCtrl/ChartCtrl.h" 
#include "ChartCtrl/ChartTitle.h"
#include "ChartCtrl/ChartAxisLabel.h"
#include "ChartCtrl/ChartLineSerie.h"
#include "ChartCtrl/ChartAxis.h"
#include "afxcmn.h"



// CShowRobotDataDlg �Ի���
class CShowRobotDataDlg : public CDialogEx
{
// ����
public:
	CShowRobotDataDlg(CWnd* pParent = NULL);	// ��׼���캯��

// �Ի�������
	enum { IDD = IDD_SHOWROBOTDATA_DIALOG };

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

	CChartCtrl m_ChartCtrl1;
	CChartCtrl m_ChartCtrl2;
	CChartCtrl m_ChartCtrl3;
	CChartCtrl m_ChartCtrl4;

	CChartLineSerie* m_pLineSerie1;
	CChartLineSerie* m_pLineSerie2;
	CChartLineSerie* m_pLineSerie3;
	afx_msg void OnBnClickedStartserver();
	// //��������Ϊ���������ܵ���������Ϣ
	CListBox m_recvMessage;
	afx_msg void OnLbnSelchangeList1Showmessage();

public:
	void update(CString s); 	

	
private: CEdit* send_edit;

public:
	CEdit m_TCPPort;
	CIPAddressCtrl m_TCPIPaddr;
};


