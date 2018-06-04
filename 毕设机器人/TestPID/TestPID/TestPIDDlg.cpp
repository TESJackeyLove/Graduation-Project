
// TestPIDDlg.cpp : ʵ���ļ�
//

#include "stdafx.h"
#include "TestPID.h"
#include "TestPIDDlg.h"
#include "afxdialogex.h"
#include <conio.h> //ʹ�������п���
#include <mmsystem.h>
#pragma comment(lib, "winmm.lib")

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


void CALLBACK TimerCallback(UINT TimerID, UINT msg, DWORD dwUser, DWORD dwa, DWORD dwb);  //������ý�嶨ʱ���Ļص�����

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
	, m_c_arrayLength(1000)
	
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
	ON_WM_TIMER()
	ON_BN_CLICKED(IDC_BUTTON_STOP, &CTestPIDDlg::OnBnClickedButtonStop)
	ON_BN_CLICKED(IDC_BUTTON_RESTART, &CTestPIDDlg::OnBnClickedButtonRestart)
	ON_BN_CLICKED(IDC_BUTTON_STEP_RESPONSE, &CTestPIDDlg::OnBnClickedButtonStepResponse)
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
	//pAxis->SetAutomatic(true);
	pAxis->SetMinMax(-1,55);
	///////��������
	TChartString str1;
	str1 = _T("������ʾ");
	m_ChartCtrl1.GetTitle()->AddString(str1);
	str1 = _T("��������");
	CChartAxisLabel *pLabel=m_ChartCtrl1.GetBottomAxis()->GetLabel();
	pLabel->SetText(str1);
	str1 = _T("λ��deg���ٶ�deg/s");
	pLabel = m_ChartCtrl1.GetLeftAxis()->GetLabel();
	pLabel->SetText(str1);

	m_pLineSerie1 = m_ChartCtrl1.CreateLineSerie();
	m_pLineSerie2 = m_ChartCtrl1.CreateLineSerie();
	m_pLineSerie3 = m_ChartCtrl1.CreateLineSerie();

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
	//short rtn;
	//long pos[4];
	//double vel[4];
	//unsigned short sta[4];

	//for (int i = 1; i <= 4; i++)
	//{
	//	GT_Axis(i);///ѡ���i����
	//	rtn = GT_GetAtlPos(&pos[i - 1]);   //��ȡ��i�����ʵ��λ��
	//	if (rtn != 0)
	//	{
	//		AfxMessageBox(_T("���ú���GT_GetAtlPos��ȡʵ��λ��ʧ��!"), MB_OK);
	//	}
	//	rtn = GT_GetAtlVel(&vel[i - 1]);    //��ȡ��i�����ʵ���ٶ�
	//	if (rtn != 0)
	//	{
	//		AfxMessageBox(_T("���ú���GT_GetAtlVel��ȡʵ���ٶ�ʧ��!"), MB_OK);
	//	}
	//	rtn = GT_GetSts(&sta[i - 1]);        //��ȡ��i�����ʵ��״̬
	//	if (rtn != 0)
	//	{
	//		AfxMessageBox(_T("���ú���GT_GetSts��ȡʵ��״̬ʧ��!"), MB_OK);
	//	}
	//}
	//AllocConsole();//ע���鷵��ֵ
	//_cprintf("pos0=%d\n", pos[0]);
	//_cprintf("pos1=%d\n", pos[1]);
	//_cprintf("pos2=%d\n", pos[2]);
	//_cprintf("pos3=%d\n", pos[3]);
	//	TRACE("x   =   %d   and   y   =   %d/n", pos[0], pos[1]);
	//	FreeConsole();
	Robot->m_pController->AxisCaptHomeWithoutLimit(4, 10);	//��צ��Ҫ�޸�������
}


void CTestPIDDlg::OnBnClickedButtonStartshow()
{
	// TODO:  �ڴ���ӿؼ�֪ͨ����������
	//m_ChartCtrl1.RemoveAllSeries();//�����
	//KillTimer(2);
	ZeroMemory(&m_HightSpeedChartArray1, sizeof(double)*m_c_arrayLength);
	m_pLineSerie1->SetSeriesOrdering(poNoOrdering);//����Ϊ����
	for (size_t i = 0; i<m_c_arrayLength; ++i)
	{
		m_X[i] = i;   //X��Ĳ�����ʼ��
	}
	m_count = m_c_arrayLength;
	m_pLineSerie1->ClearSerie();  //��յ�
	TChartStringStream strs1;
	strs1 << _T("����IDC_ChartCtrl1�ĵ�")
		<< m_ChartCtrl1.GetSeriesCount()
		<< _T("������");
	m_pLineSerie1->SetName(strs1.str());//SetName�����ý��ں��潲��

	SetTimer(2, 50, NULL);
}



///   
/// \brief ��������  
/// \param ptr ����ָ��  
/// \param data ����ֵ  
///  
void LeftMoveArray(double* ptr, size_t length, double data)
{
	for (size_t i = 1; i<length; ++i)
	{
		ptr[i - 1] = ptr[i];
	}
	ptr[length - 1] = data;
}

//void CTestPIDDlg::drawMoving()
//{
//	m_pLineSerie->ClearSerie();
//	RandArray(m_HightSpeedChartArray, m_c_arrayLength);
//	LeftMoveArray(m_X, m_c_arrayLength, m_count);
//	m_pLineSerie->AddPoints(m_X, m_HightSpeedChartArray, m_c_arrayLength);
//}
//
void CTestPIDDlg::OnTimer(UINT_PTR nIDEvent)
{
	// TODO:  �ڴ������Ϣ�����������/�����Ĭ��ֵ
	Robot->UpdateJointArray();
	if (nIDEvent == 2)
	{
		++m_count;
		m_pLineSerie1->ClearSerie();
		LeftMoveArray(m_HightSpeedChartArray1, m_c_arrayLength, randf(0, 10));  //Y�������
		LeftMoveArray(m_X, m_c_arrayLength, m_count);                          //X�������
		m_pLineSerie1->AddPoints(m_X, m_HightSpeedChartArray1, m_c_arrayLength);
		CDialogEx::OnTimer(nIDEvent);
	}
	if (nIDEvent == 0)
	{
		++m_count;
		m_pLineSerie1->ClearSerie();
		m_pLineSerie2->ClearSerie();
		m_pLineSerie3->ClearSerie();
		LeftMoveArray(m_HightSpeedChartArray1, m_c_arrayLength, 50);  //Y�������
		LeftMoveArray(m_HightSpeedChartArray2, m_c_arrayLength, Robot->m_JointArray[0].CurrentJointPositon);  //Y�������
		LeftMoveArray(m_HightSpeedChartArray3, m_c_arrayLength, Robot->m_JointArray[0].CurrentJointVelocity);  //Y�������
		LeftMoveArray(m_X, m_c_arrayLength, m_count);
		m_pLineSerie1->AddPoints(m_X, m_HightSpeedChartArray1, m_c_arrayLength);
		m_pLineSerie2->AddPoints(m_X, m_HightSpeedChartArray2, m_c_arrayLength);
		m_pLineSerie3->AddPoints(m_X, m_HightSpeedChartArray3, m_c_arrayLength);

	}

}

double CTestPIDDlg::randf(double min, double max)
{
	int minInteger = (int)(min * 10000);
	int maxInteger = (int)(max * 10000);
	int randInteger = rand()*rand();
	int diffInteger = maxInteger - minInteger;
	int resultInteger = randInteger % diffInteger + minInteger;
	return resultInteger / 10000.0;
}

void CTestPIDDlg::OnBnClickedButtonStop()
{
	// TODO:  �ڴ���ӿؼ�֪ͨ����������
	KillTimer(2);
	KillTimer(0);
	timeKillEvent(TimerID);
}


void CTestPIDDlg::OnBnClickedButtonRestart()
{
	// TODO:  �ڴ���ӿؼ�֪ͨ����������
	//SetTimer(2, 50, NULL); 
	SetTimer(0, 50, NULL);
}


void CTestPIDDlg::OnBnClickedButtonStepResponse()
{
	// TODO:  �ڴ���ӿؼ�֪ͨ����������
	CWaitCursor wc;	
	ZeroMemory(&m_HightSpeedChartArray1, sizeof(double)*m_c_arrayLength);
	ZeroMemory(&m_HightSpeedChartArray2, sizeof(double)*m_c_arrayLength);
	ZeroMemory(&m_HightSpeedChartArray3, sizeof(double)*m_c_arrayLength);
	m_pLineSerie1->SetSeriesOrdering(poNoOrdering);//����Ϊ����
	m_pLineSerie2->SetSeriesOrdering(poNoOrdering);//����Ϊ����
	m_pLineSerie3->SetSeriesOrdering(poNoOrdering);//����Ϊ����
	for (size_t i = 0; i<m_c_arrayLength; ++i)
	{
		m_X[i] = i;   //X��Ĳ�����ʼ��
	}
	m_count = m_c_arrayLength;
	m_pLineSerie1->ClearSerie();  //��յ�  
	m_pLineSerie2->ClearSerie();  //��յ� 
	m_pLineSerie3->ClearSerie();  //��յ� 
	TChartStringStream strs1,strs2,strs3;
	strs1 << _T("�趨ֵ");
	m_pLineSerie1->SetName(strs1.str());//SetName�����ý��ں��潲��
	strs2 << _T("�ɼ�ֵ");
	m_pLineSerie2->SetName(strs2.str());
	strs3 << _T("�ٶ�ֵ");
	m_pLineSerie3->SetName(strs3.str());
	m_ChartCtrl1.GetLegend()->SetVisible(true);
	SetTimer(0, 20, NULL);    //������ʱ��0����ʱ������50ms
    //TimerID=timeSetEvent(50, 1, (LPTIMECALLBACK)TimerCallback, (DWORD)this, TIME_PERIODIC);
	if (Robot->JointJog(1, 50, 1) == -1)  //��1����ģ��˶���1�ȣ��˶�����Ϊ1
		AfxMessageBox(_T("�˶�������Χ!"), MB_OK);

}

void CALLBACK TimerCallback(UINT TimerID, UINT msg, DWORD dwUser, DWORD dwa, DWORD dwb)
{
	CTestPIDDlg* pDlg = (CTestPIDDlg*)dwUser;
	pDlg->OnTimer(0);
}