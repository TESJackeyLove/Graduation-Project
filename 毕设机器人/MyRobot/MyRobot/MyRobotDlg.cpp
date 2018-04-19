
// MyRobotDlg.cpp : ʵ���ļ�
//

#include "stdafx.h"
#include "MyRobot.h"
#include "MyRobotDlg.h"
#include "afxdialogex.h"


#ifdef _DEBUG
#define new DEBUG_NEW
#endif



// ����Ӧ�ó��򡰹��ڡ��˵���� CAboutDlg �Ի���

class CAboutDlg : public CDialogEx
{
public:
	CAboutDlg();

// �Ի�������
	enum { IDD = IDD_ABOUTBOX };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV ֧��

// ʵ��
protected:
	DECLARE_MESSAGE_MAP()
};

CAboutDlg::CAboutDlg() : CDialogEx(CAboutDlg::IDD)
{
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialogEx)
END_MESSAGE_MAP()


// CMyRobotDlg �Ի���


//����ȫ�ֱ��� 
CRobotBase *gRobotPtr = NULL;

//�Ի���Ĺ��캯��
CMyRobotDlg::CMyRobotDlg(CWnd* pParent /*=NULL*/)
	: CDialogEx(CMyRobotDlg::IDD, pParent)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
	ImpedanceController = NULL;
	Robot = new CGRB4Robot();
	m_deviceflag = false;
	m_servoflag = false;

}

void CMyRobotDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_OPENCONTROLLER, m_opendevice);
	DDX_Control(pDX, IDC_SERVO_ON, m_servo);
	DDX_Control(pDX, IDC_BUTTON_IMPEDANCE, m_ImpedanceButton);
}

BEGIN_MESSAGE_MAP(CMyRobotDlg, CDialogEx)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_BN_CLICKED(IDC_OPENCONTROLLER, &CMyRobotDlg::OnBnClickedOpencontroller)
	ON_BN_CLICKED(IDC_SERVO_ON, &CMyRobotDlg::OnBnClickedServoOn)
//	ON_WM_LBUTTONUP()
ON_BN_CLICKED(IDC_BUTTON_HOME, &CMyRobotDlg::OnBnClickedButtonHome)
ON_BN_CLICKED(IDC_BUTTON_TEST, &CMyRobotDlg::OnBnClickedButtonTest)
ON_BN_CLICKED(IDC_BUTTON_IMPEDANCE, &CMyRobotDlg::OnBnClickedButtonImpedance)
ON_WM_TIMER()
END_MESSAGE_MAP()

  
// CMyRobotDlg ��Ϣ�������

BOOL CMyRobotDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// ��������...���˵�����ӵ�ϵͳ�˵��С�

	// IDM_ABOUTBOX ������ϵͳ���Χ�ڡ�
	ASSERT((IDM_ABOUTBOX & 0xFFF0) == IDM_ABOUTBOX);
	ASSERT(IDM_ABOUTBOX < 0xF000);

	CMenu* pSysMenu = GetSystemMenu(FALSE);
	if (pSysMenu != NULL)
	{
		BOOL bNameValid;
		CString strAboutMenu;
		bNameValid = strAboutMenu.LoadString(IDS_ABOUTBOX);
		ASSERT(bNameValid);
		if (!strAboutMenu.IsEmpty())
		{
			pSysMenu->AppendMenu(MF_SEPARATOR);
			pSysMenu->AppendMenu(MF_STRING, IDM_ABOUTBOX, strAboutMenu);
		}
	}

	// ���ô˶Ի����ͼ�ꡣ  ��Ӧ�ó��������ڲ��ǶԻ���ʱ����ܽ��Զ�
	//  ִ�д˲���
	SetIcon(m_hIcon, TRUE);			// ���ô�ͼ��
	SetIcon(m_hIcon, FALSE);		// ����Сͼ��

	// TODO:  �ڴ���Ӷ���ĳ�ʼ������
	SetTimer(1, 100, NULL);  //���ö�ʱ������ʱ����Ϊ100ms
	return TRUE;  // ���ǽ��������õ��ؼ������򷵻� TRUE
}

void CMyRobotDlg::OnSysCommand(UINT nID, LPARAM lParam)
{
	if ((nID & 0xFFF0) == IDM_ABOUTBOX)
	{
		CAboutDlg dlgAbout;
		dlgAbout.DoModal();
	}
	else
	{
		CDialogEx::OnSysCommand(nID, lParam);
	}
}

// �����Ի��������С����ť������Ҫ����Ĵ���
//  �����Ƹ�ͼ�ꡣ  ����ʹ���ĵ�/��ͼģ�͵� MFC Ӧ�ó���
//  �⽫�ɿ���Զ���ɡ�

void CMyRobotDlg::OnPaint()
{
	if (IsIconic())
	{
		CPaintDC dc(this); // ���ڻ��Ƶ��豸������

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// ʹͼ���ڹ����������о���
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// ����ͼ��
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialogEx::OnPaint();
	}
}

//���û��϶���С������ʱϵͳ���ô˺���ȡ�ù��
//��ʾ��
HCURSOR CMyRobotDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}



void CMyRobotDlg::OnBnClickedOpencontroller()
{
	// TODO:  �ڴ���ӿؼ�֪ͨ����������
	if (!m_deviceflag)
	{
		CGTController *Controller = new CGTController;
		Robot->AttachController(Controller);

		Robot->m_pController->EnableController();
		Robot->m_pController->InitCard();

		CPlanner *Planner = new CPlanner;
		Robot->AttachPlanner(Planner);

		m_servo.EnableWindow(true);
		m_opendevice.SetWindowText(_T("�رտ�����"));
		m_deviceflag = true;

	}
	else
	{
		Robot->DetachController();
		m_opendevice.SetWindowText(_T("�򿪿�����"));
		m_servo.EnableWindow(false);
		m_deviceflag = false;
	}
}


void CMyRobotDlg::OnBnClickedServoOn()
{
	// TODO:  �ڴ���ӿؼ�֪ͨ����������
	if (!m_servoflag)
	{
		Robot->m_pController->ServoOn();

		m_opendevice.EnableWindow(false);
		GetDlgItem(IDC_BUTTON_HOME)->EnableWindow(true);
		GetDlgItem(IDC_BUTTON_IMPEDANCE)->EnableWindow(true);
		m_servo.SetWindowText(_T("�ŷ��ϵ�"));
		m_servoflag = true;  //�ŷ��ϵ��־λ��
	}
	else
	{
		Robot->m_pController->ServoOff();
		m_opendevice.EnableWindow(true);
		GetDlgItem(IDC_BUTTON_HOME)->EnableWindow(false);
		m_servo.SetWindowText(_T("�ŷ��ϵ�"));
		m_servoflag = false;
	}

}


void CMyRobotDlg::OnBnClickedButtonHome()
{
	// TODO:  �ڴ���ӿؼ�֪ͨ����������
	//�������ù��ػ��޸ĵ�


	CWaitCursor wc;
	Robot->Home();
	for (int i = 0; i<Robot->m_JointNumber; i++)
		Robot->m_JointArray[i].LastJointPosition = 0;

}


void CMyRobotDlg::OnBnClickedButtonTest()
{
	// TODO:  �ڴ���ӿؼ�֪ͨ����������
	CWaitCursor wc;
	Robot->JointsTest();
}


//�����￪���迹���ƵĴ���
void CMyRobotDlg::OnBnClickedButtonImpedance()
{
	// TODO:  �ڴ���ӿؼ�֪ͨ����������
	ImpedanceController = new CImpedance(Robot);
	ImpedanceController->StartImpedanceController();

}
/*
��״̬�Ĵ�����
λ��	0 ->�˶���ɱ�־λ          1 ->�ŷ�������־λ	 	 2 ->�ϵ㵽���־λ		  3 ->Index/home�����־λ
		4 ->�˶������־λ����	5 ->������λ���ش���	 6 ->������λ����		  7 ->��������־λ
		8 ->�������/�ջ�״̬		9 ->���ʹ��/��ֹ״̬	 10->�Ƿ����˶���־λ	  11->��λ����ʹ��/��ֹ��־λ
		12&13->��ǰ����ţ�1234��   14->�趨Home�����źŲ����־                      15->�趨Index�źŲ����־   
*/

long statusStatusIDArray[8][4] = 
{ 
	    { IDC_STATIC_11, IDC_STATIC_12, IDC_STATIC_13, IDC_STATIC_14 },   //�ŷ�������־λ
	    { IDC_STATIC_21, IDC_STATIC_22, IDC_STATIC_23, IDC_STATIC_24 },   //�˶������־λ
		{ IDC_STATIC_31, IDC_STATIC_32, IDC_STATIC_33, IDC_STATIC_34 },   //����λ������־λ
		{ IDC_STATIC_41, IDC_STATIC_42, IDC_STATIC_43, IDC_STATIC_44 },   //����λ������־λ
		{ IDC_STATIC_51, IDC_STATIC_52, IDC_STATIC_53, IDC_STATIC_54 },   //�ŷ��ϵ��־λ
		{ IDC_STATIC_61, IDC_STATIC_62, IDC_STATIC_63, IDC_STATIC_64 },   //��ǰ�ؽڵ�λ��
		{ IDC_STATIC_71, IDC_STATIC_72, IDC_STATIC_73, IDC_STATIC_74 },   //�ؽ�����λ�Ƕ�
		{ IDC_STATIC_81, IDC_STATIC_82, IDC_STATIC_83, IDC_STATIC_84 }    //�ؽڸ���λ�Ƕ�
};
unsigned statusBitsMask[5] = { 0x02, 0x10, 0x20, 0x40, 0x200 };//�ֱ����ŷ��������˶�����������λ���أ�������λ���أ��ŷ��ϵ�
void CMyRobotDlg::OnJointsDataShow()  //�ؽڿռ��е�״̬��ʾ
{
/////////////////////////////////��ǰ�ؽڵ�λ��
	CString str;
	str.Format(_T("%f"), Robot->m_JointArray[0].CurrentJointPositon);  //doubleת�����ַ�����
	SetDlgItemText(IDC_STATIC_61, str);
	str.Format(_T("%f"), Robot->m_JointArray[1].CurrentJointPositon);  //doubleת�����ַ�����
	SetDlgItemText(IDC_STATIC_62, str);
	str.Format(_T("%f"), Robot->m_JointArray[2].CurrentJointPositon);  //doubleת�����ַ�����
	SetDlgItemText(IDC_STATIC_63, str);
	str.Format(_T("%f"), Robot->m_JointArray[3].CurrentJointPositon);  //doubleת�����ַ�����
	SetDlgItemText(IDC_STATIC_64, str);
//////////////////////////////////////////////�ؽ�����λ�Ƕ�
	str.Format(_T("%f"), Robot->m_JointArray[0].PositiveJointLimit);  //doubleת�����ַ�����
	SetDlgItemText(IDC_STATIC_71, str);
	str.Format(_T("%f"), Robot->m_JointArray[1].PositiveJointLimit);  //doubleת�����ַ�����
	SetDlgItemText(IDC_STATIC_72, str);
	str.Format(_T("%f"), Robot->m_JointArray[2].PositiveJointLimit);  //doubleת�����ַ�����
	SetDlgItemText(IDC_STATIC_73, str);
	str.Format(_T("%f"), Robot->m_JointArray[3].PositiveJointLimit);  //doubleת�����ַ�����
	SetDlgItemText(IDC_STATIC_74, str);
////////////////////////////////////////////�ؽڸ���λ�Ƕ�
	str.Format(_T("%f"), Robot->m_JointArray[0].NegativeJointLimit);  //doubleת�����ַ�����
	SetDlgItemText(IDC_STATIC_81, str);
	str.Format(_T("%f"), Robot->m_JointArray[1].NegativeJointLimit);  //doubleת�����ַ�����
	SetDlgItemText(IDC_STATIC_82, str);
	str.Format(_T("%f"), Robot->m_JointArray[2].NegativeJointLimit);  //doubleת�����ַ�����
	SetDlgItemText(IDC_STATIC_83, str);
	str.Format(_T("%f"), Robot->m_JointArray[3].NegativeJointLimit);  //doubleת�����ַ�����
	SetDlgItemText(IDC_STATIC_84, str);

////////////////////////////////////////////////
	unsigned short sts;
	for (int i = 0; i < Robot->m_JointNumber; i++)
	{
		sts = Robot->m_JointArray[i].JointStatus;
		for (int j = 0; j < 5; j++)
		{
			if ((sts&statusBitsMask[j]) == statusBitsMask[j])
			{
				SetDlgItemText(statusStatusIDArray[j][i], _T("1"));
			}
			else
				SetDlgItemText(statusStatusIDArray[j][i], _T("0"));
		}
	}

}
void CMyRobotDlg::OnToolDataShow()    //ֱ������ϵ�е�״̬��ʾ
{
	CString str;
	str.Format(_T("%f"), Robot->m_HandCurrTn[0][3]);   //ȡ�����˻���������ֱ������ϵ��ǰ��X������
	SetDlgItemText(IDC_STATIC_XPOS, str);
	str.Format(_T("%f"), Robot->m_HandCurrTn[1][3]);   //ȡ�����˻���������ֱ������ϵ��ǰ��Y������
	SetDlgItemText(IDC_STATIC_YPOS, str);
	str.Format(_T("%f"), Robot->m_HandCurrTn[2][3]);       //ȡ�����˻���������ֱ������ϵ��ǰ��Z������
	SetDlgItemText(IDC_STATIC_ZPOS, str);
	str.Format(_T("%f"), Robot->m_JointArray[3].CurrentJointPositon);
	SetDlgItemText(IDC_STATIC_GRIPPER_JOINT, str);
}

//��ʱ����������OnInitDialog�������������ö�ʱ���Ķ�ʱʱ����100ms
void CMyRobotDlg::OnTimer(UINT_PTR nIDEvent)
{
	// TODO:  �ڴ������Ϣ�����������/�����Ĭ��ֵ
	if (m_servoflag)
	{
		OnJointsDataShow();
		OnToolDataShow();
	}
	CDialogEx::OnTimer(nIDEvent);
}
