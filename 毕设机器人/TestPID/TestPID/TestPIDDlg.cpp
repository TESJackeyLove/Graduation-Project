
// TestPIDDlg.cpp : ʵ���ļ�
//

#include "stdafx.h"
#include "TestPID.h"
#include "TestPIDDlg.h"
#include "afxdialogex.h"
#include <conio.h> //ʹ�������п���

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


// CTestPIDDlg �Ի���



CTestPIDDlg::CTestPIDDlg(CWnd* pParent /*=NULL*/)
	: CDialogEx(CTestPIDDlg::IDD, pParent)
	
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
	Robot = new CGRB4Robot();
	m_deviceflag = false;
	m_servoflag = false;
}

void CTestPIDDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_OPENCONTROLLER, m_opendevice);
	DDX_Control(pDX, IDC_BUTTON_SERVO_ON, m_servo);
	DDX_Control(pDX, IDC_ChartCtrl1, m_ChartCtrl1);
}

BEGIN_MESSAGE_MAP(CTestPIDDlg, CDialogEx)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_BN_CLICKED(IDC_OPENCONTROLLER, &CTestPIDDlg::OnBnClickedOpencontroller)
	ON_BN_CLICKED(IDC_BUTTON_HOME, &CTestPIDDlg::OnBnClickedButtonHome)
	ON_BN_CLICKED(IDC_BUTTON_SERVO_ON, &CTestPIDDlg::OnBnClickedButtonServoOn)
	ON_BN_CLICKED(IDC_BUTTON_TEST, &CTestPIDDlg::OnBnClickedButtonTest)
	ON_BN_CLICKED(IDC_BUTTON_STARTSHOW, &CTestPIDDlg::OnBnClickedButtonStartshow)
END_MESSAGE_MAP()


// CTestPIDDlg ��Ϣ�������

BOOL CTestPIDDlg::OnInitDialog()
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
	////////����������Ϊ��ֵ
	CChartAxis *pAxis = NULL;
	pAxis = m_ChartCtrl1.CreateStandardAxis(CChartCtrl::BottomAxis);
	pAxis->SetAutomatic(true);
	pAxis = m_ChartCtrl1.CreateStandardAxis(CChartCtrl::LeftAxis);
	pAxis->SetAutomatic(true);
	///////��������
	TChartString str1;
	str1 = _T("λ��������ʾ");
	m_ChartCtrl1.GetTitle()->AddString(str1);
	str1 = _T("����λ�ô�С");
	CChartAxisLabel *pLabel=m_ChartCtrl1.GetBottomAxis()->GetLabel();
	pLabel->SetText(str1);
	str1 = _T("ʱ�����");
	pLabel = m_ChartCtrl1.GetLeftAxis()->GetLabel();
	pLabel->SetText(str1);



	/////////
	return TRUE;  // ���ǽ��������õ��ؼ������򷵻� TRUE
}

void CTestPIDDlg::OnSysCommand(UINT nID, LPARAM lParam)
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

void CTestPIDDlg::OnPaint()
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
HCURSOR CTestPIDDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}





void CTestPIDDlg::OnBnClickedOpencontroller()
{
	// TODO:  �ڴ���ӿؼ�֪ͨ����������
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


void CTestPIDDlg::OnBnClickedButtonHome()
{
	//�������ù��ػ��޸ĵ�
	CWaitCursor wc;
	Robot->Home();
	for (int i = 0; i<Robot->m_JointNumber; i++)
		Robot->m_JointArray[i].LastJointPosition = 0;
}


void CTestPIDDlg::OnBnClickedButtonServoOn()
{
	// TODO:  �ڴ���ӿؼ�֪ͨ����������
	if (!m_servoflag)
	{
		Robot->m_pController->ServoOn();

		m_opendevice.EnableWindow(false);
		GetDlgItem(IDC_BUTTON_HOME)->EnableWindow(true);
		GetDlgItem(IDC_BUTTON_TEST)->EnableWindow(true);
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


void CTestPIDDlg::OnBnClickedButtonTest()
{
	// TODO:  �ڴ���ӿؼ�֪ͨ����������
	CWaitCursor wc;
	short rtn;
	long pos[4];
	double vel[4];
	unsigned short sta[4];

	for (int i = 1; i <= 4; i++)
	{
		GT_Axis(i);///ѡ���i����
		rtn = GT_GetAtlPos(&pos[i - 1]);   //��ȡ��i�����ʵ��λ��
		if (rtn != 0)
		{
			AfxMessageBox(_T("���ú���GT_GetAtlPos��ȡʵ��λ��ʧ��!"), MB_OK);
		}
		rtn = GT_GetAtlVel(&vel[i - 1]);    //��ȡ��i�����ʵ���ٶ�
		if (rtn != 0)
		{
			AfxMessageBox(_T("���ú���GT_GetAtlVel��ȡʵ���ٶ�ʧ��!"), MB_OK);
		}
		rtn = GT_GetSts(&sta[i - 1]);        //��ȡ��i�����ʵ��״̬
		if (rtn != 0)
		{
			AfxMessageBox(_T("���ú���GT_GetSts��ȡʵ��״̬ʧ��!"), MB_OK);
		}
	}
	AllocConsole();//ע���鷵��ֵ
	_cprintf("pos0=%d\n", pos[0]);
	_cprintf("pos1=%d\n", pos[1]);
	_cprintf("pos2=%d\n", pos[2]);
	_cprintf("pos3=%d\n", pos[3]);
	//	TRACE("x   =   %d   and   y   =   %d/n", pos[0], pos[1]);
	//	FreeConsole();
}


void CTestPIDDlg::OnBnClickedButtonStartshow()
{
	// TODO:  �ڴ���ӿؼ�֪ͨ����������
	m_ChartCtrl1.EnableRefresh(false);
	double x[1000], y[1000];
	for (int i = 0; i<1000; i++)
	{
		x[i] = i;
		y[i] = sin(float(i));
	}
	CChartLineSerie *pLineSerie1;
	m_ChartCtrl1.RemoveAllSeries();//�����
	pLineSerie1 = m_ChartCtrl1.CreateLineSerie();
	pLineSerie1->SetSeriesOrdering(poNoOrdering);//����Ϊ����
	pLineSerie1->AddPoints(x, y, 1000);
	pLineSerie1->SetName(_T("����IDC_ChartCtrl1�ĵ�һ����"));//SetName�����ý��ں��潲��
}


