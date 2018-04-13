
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

	Robot = new CGRB4Robot();
	m_deviceflag = false;
	m_servoflag = false;

}

void CMyRobotDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_OPENCONTROLLER, m_opendevice);
	DDX_Control(pDX, IDC_SERVO_ON, m_servo);
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
