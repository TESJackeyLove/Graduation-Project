
// MyRobotDlg.cpp : ʵ���ļ�
//

#include "stdafx.h"
#include "MyRobot.h"
#include "MyRobotDlg.h"
#include "afxdialogex.h"
#include <conio.h> //ʹ�������п���

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

SOCKET sockClient; //ȫ�ֱ������ͻ��˵��׽���
void CString2Char(CString str, char ch[]);//�˺��������ַ�ת��������ʵ�ִ��룬����ԭ��˵��
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
	m_ImpedanceButtonflag = false;

}

void CMyRobotDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_OPENCONTROLLER, m_opendevice);
	DDX_Control(pDX, IDC_SERVO_ON, m_servo);
	DDX_Control(pDX, IDC_BUTTON_IMPEDANCE, m_ImpedanceButton);
	DDX_Control(pDX, IDC_SERVER_IPADDRESS1, m_ServerIPAddr);
	DDX_Control(pDX, IDC_EDIT1_PORT, m_ServerPort);
	DDX_Control(pDX, IDC_LIST1, m_TCPMessage);
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
ON_BN_CLICKED(IDC_BUTTON_JOINT1_NEGATIVE, &CMyRobotDlg::OnBnClickedButtonJoint1Negative)
ON_BN_CLICKED(IDC_BUTTON_JOINT1_POSITIVE, &CMyRobotDlg::OnBnClickedButtonJoint1Positive)
ON_BN_CLICKED(IDC_BUTTON_JOINT2_POSITIVE, &CMyRobotDlg::OnBnClickedButtonJoint2Positive)
ON_BN_CLICKED(IDC_BUTTON_JOINT2_NEGATIVE, &CMyRobotDlg::OnBnClickedButtonJoint2Negative)
ON_BN_CLICKED(IDC_BUTTON_JOINT3_NEGATIVE, &CMyRobotDlg::OnBnClickedButtonJoint3Negative)
ON_BN_CLICKED(IDC_BUTTON_JOINT3_POSITIVE, &CMyRobotDlg::OnBnClickedButtonJoint3Positive)
ON_BN_CLICKED(IDC_BUTTON_JOINT4_POSITIVE, &CMyRobotDlg::OnBnClickedButtonJoint4Positive)
ON_BN_CLICKED(IDC_BUTTON_JOINT4_NEGATIVE, &CMyRobotDlg::OnBnClickedButtonJoint4Negative)
ON_BN_CLICKED(IDC_BUTTON_GOHOME, &CMyRobotDlg::OnBnClickedButtonGohome)
ON_BN_CLICKED(IDC_BUTTON_CONNECTSERVER, &CMyRobotDlg::OnBnClickedButtonConnectserver)
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
	//SetTimer(2, 100, NULL);  //���ö�ʱ������ʱ����Ϊ100ms,����TCP/IP������

	//*************TCP/IP�ĵ�ַ�����ó�ʼ����TCP/IP��ַ
	CString  strIP = _T("192.168.56.1");  //����Ĭ�ϵ�ַ
	DWORD dwAddress;
	char ch_ip[20];
	CString2Char(strIP, ch_ip);//ע�⣡����������ַ���ʽת���������˺������ܣ�CString����ת��ΪChar���ͣ�ʵ�ִ����ں������
	dwAddress = inet_addr(ch_ip);
	unsigned  char  *pIP = (unsigned  char*)&dwAddress;
	m_ServerIPAddr.SetAddress(*pIP, *(pIP + 1), *(pIP + 2), *(pIP + 3));
	this->SetDlgItemText(IDC_EDIT1_PORT, _T("8888"));



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
		GetDlgItem(IDC_BUTTON_GOHOME)->EnableWindow(true);
		GetDlgItem(IDC_BUTTON_TEST)->EnableWindow(true);
		m_servo.SetWindowText(_T("�ŷ��ϵ�"));
		m_servoflag = true;  //�ŷ��ϵ��־λ��
	}
	else
	{
		Robot->m_pController->ServoOff();
		m_opendevice.EnableWindow(true);
		GetDlgItem(IDC_BUTTON_HOME)->EnableWindow(false);
		GetDlgItem(IDC_BUTTON_IMPEDANCE)->EnableWindow(false);
		GetDlgItem(IDC_BUTTON_GOHOME)->EnableWindow(false);
		m_servo.SetWindowText(_T("�ŷ��ϵ�"));
		m_servoflag = false;
	}

}


void CMyRobotDlg::OnBnClickedButtonHome()   //Ѱ����㲢�ص����
{
	// TODO:  �ڴ���ӿؼ�֪ͨ����������
	//�������ù��ػ��޸ĵ�
	CWaitCursor wc;
	Robot->Home();
	Robot->UpdateJointArray();      //ˢ�»��������еı���
	OnJointsDataShow();
	OnToolDataShow();
	for (int i = 0; i<Robot->m_JointNumber; i++)
		Robot->m_JointArray[i].LastJointPosition = 0;

}
void CMyRobotDlg::OnBnClickedButtonGohome()    //���Ѿ���֪��������ĵ������ֱ�ӻ������
{
	// TODO:  �ڴ���ӿؼ�֪ͨ����������
	CWaitCursor wc;
	double homepos[4] = { 0, 0, 0, 0 };
	double homevel[4];
	for (int i = 0; i < Robot->m_JointNumber; i++)
	{
		homevel[i] = Robot->m_JointArray[i].NormalJointVelocity;
	}
	Robot->JointsTMove(homepos,homevel);
	Robot->m_pController->wait_motion_finished(1);  //�ȴ����˶���ɺ�ֹͣ
	Robot->UpdateJointArray();      //ˢ�»��������еı���
	OnJointsDataShow();
	OnToolDataShow();
}


void CMyRobotDlg::OnBnClickedButtonTest()
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
		rtn = GT_GetAtlPos(&pos[i-1]);   //��ȡ��i�����ʵ��λ��
		if (rtn != 0)
		{
			AfxMessageBox(_T("���ú���GT_GetAtlPos��ȡʵ��λ��ʧ��!"), MB_OK);
		}
		rtn = GT_GetAtlVel(&vel[i-1]);    //��ȡ��i�����ʵ���ٶ�
		if (rtn != 0)
		{
			AfxMessageBox(_T("���ú���GT_GetAtlVel��ȡʵ���ٶ�ʧ��!"), MB_OK);
		}
		rtn = GT_GetSts(&sta[i-1]);        //��ȡ��i�����ʵ��״̬
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


//�����￪���迹���ƵĴ���
extern bool ImpedenceControllerStopflag;  //���迹�����ڲ�����һ���̣߳��˱������ڱ���߳��Ƿ�ִ����ȥ
void CMyRobotDlg::OnBnClickedButtonImpedance()
{
	// TODO:  �ڴ���ӿؼ�֪ͨ����������
	if (ImpedanceController == NULL)
	{
		ImpedanceController = new CImpedance(Robot);
	}

	if (m_ImpedanceButtonflag == false)   //�����û�д��迹���Ƶİ�ť����ô�����ť���Ǵ򿪵���˼
	{
		ImpedanceController->StartImpedanceController();
		ImpedenceControllerStopflag = false;
		m_ImpedanceButton.SetWindowText(_T("�迹���ƹر�"));
		m_ImpedanceButtonflag = true;
	}
	else
	{                                                                                                                                                                                                                                                                                                                                                                                                                  
		ImpedenceControllerStopflag = true;
		ImpedanceController->StopImpedanceController();
		m_ImpedanceButton.SetWindowText(_T("�迹���ƿ���"));
		m_ImpedanceButtonflag = false;
	}


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
//////////////////////////////////////////////��ǰ�ؽڵ�λ��
	CString str;
	str.Format(_T("%f"), Robot->m_JointArray[0].CurrentJointPositon);  //doubleת�����ַ�����
	SetDlgItemText(IDC_STATIC_61, str);
	str.Format(_T("%f"), Robot->m_JointArray[1].CurrentJointPositon);  
	SetDlgItemText(IDC_STATIC_62, str);
	str.Format(_T("%f"), Robot->m_JointArray[2].CurrentJointPositon);  
	SetDlgItemText(IDC_STATIC_63, str);
	str.Format(_T("%f"), Robot->m_JointArray[3].CurrentJointPositon); 
	SetDlgItemText(IDC_STATIC_64, str);
//////////////////////////////////////////////�ؽ�����λ�Ƕ�,����С�����һλС��
	str.Format(_T("%.1f"), Robot->m_JointArray[0].PositiveJointLimit);  //doubleת�����ַ�����
	SetDlgItemText(IDC_STATIC_71, str);
	str.Format(_T("%.1f"), Robot->m_JointArray[1].PositiveJointLimit);  
	SetDlgItemText(IDC_STATIC_72, str);
	str.Format(_T("%.1f"), Robot->m_JointArray[2].PositiveJointLimit); 
	SetDlgItemText(IDC_STATIC_73, str);
	str.Format(_T("%.1f"), Robot->m_JointArray[3].PositiveJointLimit);  
	SetDlgItemText(IDC_STATIC_74, str);
////////////////////////////////////////////�ؽڸ���λ�Ƕ�,����С�����һλС��
	str.Format(_T("%.1f"), Robot->m_JointArray[0].NegativeJointLimit);  //doubleת�����ַ�����
	SetDlgItemText(IDC_STATIC_81, str);
	str.Format(_T("%.1f"), Robot->m_JointArray[1].NegativeJointLimit);  
	SetDlgItemText(IDC_STATIC_82, str);
	str.Format(_T("%.1f"), Robot->m_JointArray[2].NegativeJointLimit);  
	SetDlgItemText(IDC_STATIC_83, str);
	str.Format(_T("%.1f"), Robot->m_JointArray[3].NegativeJointLimit);  
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
	//ֱ������������Ϣ��ʾ������С�����4λ��Ч����
	str.Format(_T("%.4f"), Robot->m_HandCurrTn[0][3]);   //ȡ�����˻���������ֱ������ϵ��ǰ��X������
	SetDlgItemText(IDC_STATIC_XPOS, str);
	str.Format(_T("%.4f"), Robot->m_HandCurrTn[1][3]);   //ȡ�����˻���������ֱ������ϵ��ǰ��Y������
	SetDlgItemText(IDC_STATIC_YPOS, str);
	str.Format(_T("%.4f"), Robot->m_HandCurrTn[2][3]);       //ȡ�����˻���������ֱ������ϵ��ǰ��Z������
	SetDlgItemText(IDC_STATIC_ZPOS, str);
	str.Format(_T("%.4f"), Robot->m_JointArray[3].CurrentJointPositon);
	SetDlgItemText(IDC_STATIC_GRIPPER_JOINT, str);
}

//��ʱ����������OnInitDialog�������������ö�ʱ���Ķ�ʱʱ����100ms
void CMyRobotDlg::OnTimer(UINT_PTR nIDEvent)
{
	// TODO:  �ڴ������Ϣ�����������/�����Ĭ��ֵ
	if (nIDEvent == 1)
	{
		if (m_servoflag&&ImpedenceControllerStopflag==true)   //����迹���������ڹ�������ôִ��ˢ�º���
		{
			Robot->UpdateJointArray();
			OnJointsDataShow();
			OnToolDataShow();
		}
		if (m_servoflag&&ImpedenceControllerStopflag==false)   //����迹���ڹ�������ô����ִ��ˢ�º���
		{
			OnJointsDataShow();
			OnToolDataShow();
		}
	}
	if (nIDEvent == 2)   //�����õķ��ͺ���
	{
		//������
		//RobotData MyRobotData;
		//memset(&MyRobotData, 0, sizeof(MyRobotData));
		//MyRobotData.JointsNow[0] = 5;
		//MyRobotData.JointsNow[1] = 10;
		//char buff[sizeof(MyRobotData)];
		//memset(buff, 0, sizeof(MyRobotData));
		//memcpy(buff, &MyRobotData, sizeof(MyRobotData));
		//send(sockClient, buff, sizeof(buff), 0);
	}

	CDialogEx::OnTimer(nIDEvent);
}


////***************************************************//������˸�������Ӧ�ŵ������ԣ�ֵ��һ˵������Щ��������ȵ���Щ�˶����֮��Ż�ֹͣ
void CMyRobotDlg::OnBnClickedButtonJoint1Negative()
{
	// TODO:  �ڴ���ӿؼ�֪ͨ����������
	CWaitCursor wc;
	if (Robot->JointJog(1,-1,1)==-1)  //��1����ģ��˶���1�ȣ��˶�����Ϊ1
		AfxMessageBox(_T("�˶�������Χ!"), MB_OK);
	Robot->m_pController->wait_motion_finished(1);  //�ȴ����˶���ɺ�ֹͣ
	Robot->UpdateJointArray();      //ˢ�»��������еı���
	OnJointsDataShow();
	OnToolDataShow();
}


void CMyRobotDlg::OnBnClickedButtonJoint1Positive()
{
	// TODO:  �ڴ���ӿؼ�֪ͨ����������
	CWaitCursor wc;
	if (Robot->JointJog(1, 1, 1) == -1)  //��1����ģ��˶���1�ȣ��˶�����Ϊ1
		AfxMessageBox(_T("�˶�������Χ!"), MB_OK);
	Robot->m_pController->wait_motion_finished(1);  //�ȴ����˶���ɺ�ֹͣ
	Robot->UpdateJointArray();      //ˢ�»��������еı��������ڼ����϶���������������е��˶����������ڽ������֮��ˢ��
	OnJointsDataShow();
	OnToolDataShow();
}


void CMyRobotDlg::OnBnClickedButtonJoint2Positive()
{
	// TODO:  �ڴ���ӿؼ�֪ͨ����������
	CWaitCursor wc;
	if (Robot->JointJog(2, 1, 1) == -1)  //��2��ģ��˶���1�ȣ��˶�����Ϊ1
		AfxMessageBox(_T("�˶�������Χ!"), MB_OK);
	Robot->m_pController->wait_motion_finished(2);  //�ȴ����˶���ɺ�ֹͣ
	Robot->UpdateJointArray();      //ˢ�»��������еı���
	OnJointsDataShow();
	OnToolDataShow();
}


void CMyRobotDlg::OnBnClickedButtonJoint2Negative()
{
	// TODO:  �ڴ���ӿؼ�֪ͨ����������
	CWaitCursor wc;
	if (Robot->JointJog(2, -1, 1) == -1)  //��2��ģ��˶���1�ȣ��˶�����Ϊ1
		AfxMessageBox(_T("�˶�������Χ!"), MB_OK);
	Robot->m_pController->wait_motion_finished(2);  //�ȴ����˶���ɺ�ֹͣ
	Robot->UpdateJointArray();      //ˢ�»��������еı���
	OnJointsDataShow();
	OnToolDataShow();
}


void CMyRobotDlg::OnBnClickedButtonJoint3Negative()  //*****ע���������ķ����붨��ķ������෴��
{
	// TODO:  �ڴ���ӿؼ�֪ͨ����������
	CWaitCursor wc;
	if (Robot->JointJog(3, -1, 1) == -1)  //��3��ģ��˶���1�ȣ��˶�����Ϊ1
		AfxMessageBox(_T("�˶�������Χ!"), MB_OK);
	Robot->m_pController->wait_motion_finished(3);  //�ȴ����˶���ɺ�ֹͣ
	Robot->UpdateJointArray();      //ˢ�»��������еı���
	OnJointsDataShow();
	OnToolDataShow();
}


void CMyRobotDlg::OnBnClickedButtonJoint3Positive()  //*****ע���������ķ����붨��ķ������෴��
{
	// TODO:  �ڴ���ӿؼ�֪ͨ����������
	CWaitCursor wc;
	if (Robot->JointJog(3, 1, 1) == -1)  //��3��ģ��˶���1�ȣ��˶�����Ϊ1
		AfxMessageBox(_T("�˶�������Χ!"), MB_OK);
	Robot->m_pController->wait_motion_finished(3);  //�ȴ����˶���ɺ�ֹͣ
	Robot->UpdateJointArray();      //ˢ�»��������еı���
	OnJointsDataShow();
	OnToolDataShow();
}


void CMyRobotDlg::OnBnClickedButtonJoint4Positive()
{
	// TODO:  �ڴ���ӿؼ�֪ͨ����������
	CWaitCursor wc;
	if (Robot->JointJog(4, 1, 1) == -1)  //��4��ģ��˶���1�ȣ��˶�����Ϊ1
		AfxMessageBox(_T("�˶�������Χ!"), MB_OK);
	Robot->m_pController->wait_motion_finished(4);  //�ȴ����˶���ɺ�ֹͣ
	Robot->UpdateJointArray();      //ˢ�»��������еı���
	OnJointsDataShow();
	OnToolDataShow();
}


void CMyRobotDlg::OnBnClickedButtonJoint4Negative()
{
	// TODO:  �ڴ���ӿؼ�֪ͨ����������
	CWaitCursor wc;
	if (Robot->JointJog(4, -1, 1) == -1)  //��4��ģ��˶���1�ȣ��˶�����Ϊ1
		AfxMessageBox(_T("�˶�������Χ!"), MB_OK);
	Robot->m_pController->wait_motion_finished(4);  //�ȴ����˶���ɺ�ֹͣ
	Robot->UpdateJointArray();      //ˢ�»��������еı���
	OnJointsDataShow();
	OnToolDataShow();
}
//*******************************************************//




void CString2Char(CString str, char ch[])//�˺��������ַ�ת��������ʵ�ִ���
{
	int i;
	char *tmpch;
	int wLen = WideCharToMultiByte(CP_ACP, 0, str, -1, NULL, 0, NULL, NULL);//�õ�Char�ĳ���
	tmpch = new char[wLen + 1];                                             //��������ĵ�ַ��С
	WideCharToMultiByte(CP_ACP, 0, str, -1, tmpch, wLen, NULL, NULL);       //��CStringת����char*


	for (i = 0; tmpch[i] != '\0'; i++) ch[i] = tmpch[i];
	ch[i] = '\0';
}



void CMyRobotDlg::OnBnClickedButtonConnectserver()
{
	// TODO:  �ڴ���ӿؼ�֪ͨ����������
	//�����׽��ֿ�2.0
	WSADATA wsaData;
	int err = WSAStartup(MAKEWORD(2, 2), &wsaData);   //�����׽��ֿ�2.0�汾
	if (err != 0)
	{
		update(_T("�����׽��ֿ�2.0ʧ��"));
	}
	///IP�ŵĻ�ȡ
	unsigned char *pIP;
	CString strIP;
	DWORD dwIP;
	m_ServerIPAddr.GetAddress(dwIP);
	pIP = (unsigned char*)&dwIP;
	strIP.Format(_T("%u.%u.%u.%u"), *(pIP + 3), *(pIP + 2), *(pIP + 1), *pIP);
	update(_T("������IP��ַΪ��") + strIP);
	//////�˿ںŵĻ�ȡ
	UpdateData(TRUE);   //�ӱ༭���ȡ���ݵ���������
	int ServerPort;
	ServerPort = GetDlgItemInt(IDC_EDIT1_PORT);
	CString a;
	a.Format(_T("%d"), ServerPort);
	update(_T("�������˿ں�Ϊ��") + a);
	///////////////////////
	//--------�����׽���---------------
	sockClient = socket(AF_INET, SOCK_STREAM, 0);
	//--------�������������������------
	//����Ҫ���ӵķ���������Ϣ
	SOCKADDR_IN addrSrv;
	char ch_ip[20];
	CString2Char(strIP, ch_ip);//ע�⣡����������ַ���ʽת���������˺������ܣ�CString����ת��ΪChar���ͣ�ʵ�ִ����ں������
	addrSrv.sin_addr.S_un.S_addr = inet_addr(ch_ip);//�����������Ϳͻ��˶��ڱ��أ��̿���ʹ�ñ��ػ�·��ַ127.0.0.1
	addrSrv.sin_family = AF_INET;
	addrSrv.sin_port = htons(ServerPort);
	if (connect(sockClient, (SOCKADDR*)&addrSrv, sizeof(SOCKADDR)) == INVALID_SOCKET)   //���connect�����Ƿ�����ģʽ
	{
		update(_T("���ӷ�����ʧ��"));
	}
	else
	{
		update(_T("���ӷ������ɹ�"));
	}
	//������
	//RobotData MyRobotData;
	//memset(&MyRobotData, 0, sizeof(MyRobotData));
	//char buff[sizeof(MyRobotData)];
	//memset(buff, 0, sizeof(MyRobotData));
	//memcpy(buff, &MyRobotData, sizeof(MyRobotData));
	//send(sockClient, buff, sizeof(buff), 0);


}
