
// ShowRobotDataDlg.cpp : ʵ���ļ�
//

#include "stdafx.h"
#include "ShowRobotData.h"
#include "ShowRobotDataDlg.h"
#include "afxdialogex.h"


#ifdef _DEBUG
#define new DEBUG_NEW
#endif

// ����Ӧ�ó��򡰹��ڡ��˵���� CAboutDlg �Ի���

UINT server_thd(LPVOID p);   //�����̺߳���
CString IP;  //����Ϊȫ�ֱ���
SOCKET listen_sock;
SOCKET sock;
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


// CShowRobotDataDlg �Ի���



CShowRobotDataDlg::CShowRobotDataDlg(CWnd* pParent /*=NULL*/)
	: CDialogEx(CShowRobotDataDlg::IDD, pParent)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CShowRobotDataDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);

	DDX_Control(pDX, IDC_ChartCtrl1, m_ChartCtrl1);
	DDX_Control(pDX, IDC_ChartCtrl2, m_ChartCtrl2);
	DDX_Control(pDX, IDC_ChartCtrl3, m_ChartCtrl3);
	DDX_Control(pDX, IDC_ChartCtrl4, m_ChartCtrl4);
	DDX_Control(pDX, IDC_LIST1_SHOWMESSAGE, m_recvMessage);
	DDX_Control(pDX, IDC_EDIT2_TCPPort, m_TCPPort);
	DDX_Control(pDX, IDC_IPADDRESS1, m_TCPIPaddr);
}

BEGIN_MESSAGE_MAP(CShowRobotDataDlg, CDialogEx)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_BN_CLICKED(IDC_STARTSERVER, &CShowRobotDataDlg::OnBnClickedStartserver)
	ON_LBN_SELCHANGE(IDC_LIST1_SHOWMESSAGE, &CShowRobotDataDlg::OnLbnSelchangeList1Showmessage)
END_MESSAGE_MAP()


// CShowRobotDataDlg ��Ϣ�������

BOOL CShowRobotDataDlg::OnInitDialog()
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
	CChartAxis *pAxis = NULL;
	pAxis = m_ChartCtrl1.CreateStandardAxis(CChartCtrl::BottomAxis);
	pAxis->SetAutomatic(true);
	pAxis = m_ChartCtrl1.CreateStandardAxis(CChartCtrl::LeftAxis);
	//pAxis->SetAutomatic(true);
	pAxis->SetMinMax(-120, 120);

	pAxis = m_ChartCtrl2.CreateStandardAxis(CChartCtrl::BottomAxis);
	pAxis->SetAutomatic(true);
	pAxis = m_ChartCtrl2.CreateStandardAxis(CChartCtrl::LeftAxis);
	//pAxis->SetAutomatic(true);
	pAxis->SetMinMax(-120, 120);

	pAxis = m_ChartCtrl3.CreateStandardAxis(CChartCtrl::BottomAxis);
	pAxis->SetAutomatic(true);
	pAxis = m_ChartCtrl3.CreateStandardAxis(CChartCtrl::LeftAxis);
	//pAxis->SetAutomatic(true);
	pAxis->SetMinMax(-120, 120);

	pAxis = m_ChartCtrl4.CreateStandardAxis(CChartCtrl::BottomAxis);
	pAxis->SetAutomatic(true);
	pAxis = m_ChartCtrl4.CreateStandardAxis(CChartCtrl::LeftAxis);
	//pAxis->SetAutomatic(true);
	pAxis->SetMinMax(-360, 360);
	///////��������
	TChartString str1;

	str1 = _T("#1�ĽǶȣ��ٶȣ�����");
	m_ChartCtrl1.GetTitle()->AddString(str1);
	str1 = _T("��������");
	CChartAxisLabel *pLabel = m_ChartCtrl1.GetBottomAxis()->GetLabel();
	pLabel->SetText(str1);
	str1 = _T("λ��deg���ٶ�deg/s������N��m");
	pLabel = m_ChartCtrl1.GetLeftAxis()->GetLabel();
	pLabel->SetText(str1);

	str1 = _T("#2�ĽǶȣ��ٶȣ�����");
	m_ChartCtrl2.GetTitle()->AddString(str1);
	str1 = _T("��������");
	pLabel = m_ChartCtrl2.GetBottomAxis()->GetLabel();
	pLabel->SetText(str1);
	str1 = _T("λ��deg���ٶ�deg/s������N��m");
	pLabel = m_ChartCtrl2.GetLeftAxis()->GetLabel();
	pLabel->SetText(str1);

	str1 = _T("#3�ĽǶȣ��ٶȣ�����");
	m_ChartCtrl3.GetTitle()->AddString(str1);
	str1 = _T("��������");
	pLabel = m_ChartCtrl3.GetBottomAxis()->GetLabel();
	pLabel->SetText(str1);
	str1 = _T("λ��deg���ٶ�deg/s������N��m");
	pLabel = m_ChartCtrl3.GetLeftAxis()->GetLabel();
	pLabel->SetText(str1);

	str1 = _T("#4�ĽǶȣ��ٶȣ�����");
	m_ChartCtrl4.GetTitle()->AddString(str1);
	str1 = _T("��������");
	pLabel = m_ChartCtrl4.GetBottomAxis()->GetLabel();
	pLabel->SetText(str1);
	str1 = _T("λ��deg���ٶ�deg/s������N��m");
	pLabel = m_ChartCtrl4.GetLeftAxis()->GetLabel();
	pLabel->SetText(str1);

	m_pLineSerie1 = m_ChartCtrl1.CreateLineSerie();
	m_pLineSerie2 = m_ChartCtrl1.CreateLineSerie();
	m_pLineSerie3 = m_ChartCtrl1.CreateLineSerie();

	/////////////////////////////////////////////////


	return TRUE;  // ���ǽ��������õ��ؼ������򷵻� TRUE
}

void CShowRobotDataDlg::OnSysCommand(UINT nID, LPARAM lParam)
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

void CShowRobotDataDlg::OnPaint()
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
HCURSOR CShowRobotDataDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}


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

void CShowRobotDataDlg::OnBnClickedStartserver()
{
	// TODO:  �ڴ���ӿؼ�֪ͨ����������
	WSADATA wsaData;
	int err = WSAStartup(MAKEWORD(2, 2), &wsaData);   //�����׽��ֿ�2.0�汾
	if (err!=0)
	{
		update(_T("�����׽��ֿ�2.0ʧ��"));
	}
	send_edit = (CEdit *)GetDlgItem(IDC_EDIT1);
	send_edit->SetFocus();
	char name[128];
	hostent* pHost;
	gethostname(name, 128);//��������� 
	pHost = gethostbyname(name);//��������ṹ 
	IP = inet_ntoa(*(struct in_addr*)pHost->h_addr);    //��ȡ������IP��ַ
	DWORD dwIP;
	char ch_ip1[20];
	CString2Char(IP, ch_ip1);//ע�⣡����������ַ���ʽת���������˺������ܣ�CString����ת��ΪChar���ͣ�ʵ�ִ����ں������
	dwIP = inet_addr(ch_ip1);
	unsigned char *pIP = (unsigned char*)&dwIP;
	m_TCPIPaddr.SetAddress(*(pIP), *(pIP + 1), *(pIP + 2), *(pIP+3));
	
	update(_T("��������IP��ַ��") + IP);

	AfxBeginThread(server_thd, NULL);//�����߳�
}


void CShowRobotDataDlg::OnLbnSelchangeList1Showmessage()
{
	// TODO:  �ڴ���ӿؼ�֪ͨ����������
}

//��list���ܿ��д�ӡ��Ϣ
void CShowRobotDataDlg::update(CString s)
{
	m_recvMessage.AddString(s);
}



UINT server_thd(LPVOID p)//�߳�Ҫ���õĺ���
{
	SOCKADDR_IN local_addr;
	SOCKADDR_IN client_addr;
	int iaddrSize = sizeof(SOCKADDR_IN);
	int res;
	char msg[1024];
	CShowRobotDataDlg * dlg = (CShowRobotDataDlg *)AfxGetApp()->GetMainWnd();
	char ch_ip[20];
	CString2Char(IP, ch_ip);//ע�⣡����������ַ���ʽת���������˺������ܣ�CString����ת��ΪChar���ͣ�ʵ�ִ����ں������
	//local_addr.sin_addr.s_addr = htonl(INADDR_ANY);//��ȡ����IP��ַ
	local_addr.sin_addr.s_addr = inet_addr(ch_ip);
	local_addr.sin_family = AF_INET;
	local_addr.sin_port = htons(8888);   //�˿ں�Ϊ8888
	dlg->SetDlgItemText(IDC_EDIT2_TCPPort, _T("8888"));     //д��

	if ((listen_sock = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP)) == INVALID_SOCKET)//�����׽���
	{
		dlg->update(_T("���������׽���ʧ��"));
	}
	if (bind(listen_sock, (struct sockaddr*) &local_addr, sizeof(SOCKADDR_IN)))//���׽���
	{
		dlg->update(_T("�󶨴���"));
	}

    listen(listen_sock, 1);     //��ʼ����,����ʹ�õ�������ģʽ
	if ((sock = accept(listen_sock, (struct sockaddr *)&client_addr, &iaddrSize)) == INVALID_SOCKET)//�����׽���
	{
		dlg->update(_T("accept ʧ��"));
	}
	else
	{
		CString port;
		port.Format(_T("%d"), int(ntohs(client_addr.sin_port)));
		dlg->update(_T("�����ӿͻ��ˣ�") + CString(inet_ntoa(client_addr.sin_addr)) + "  �˿ڣ�" + port);
	}
	////////////��������
	while (1)
	{		
		if ((res = recv(sock, msg, 1024, 0)) == -1)    //���rev����Ҳ������ģʽ
		{
			dlg->update(_T("ʧȥ�ͻ��˵�����"));
			break;
		}
		else
		{
		//	msg[res] = '\0';
			dlg->update(_T("client:") + CString(msg));
		}

	}
	WSACleanup();
	return 0;
}

