#include "stdafx.h"
#include "GTController.h"


/*
������˵������ڹ̸ߵ�CGTController��Ĺ���ԭ��
1���ڳ�ʼ��CGTController֮���������߳�ͬʱ�������ˣ�threadProc_UpdateAxisArray��m_AxisArray����ThreadProc����
threadProc_UpdateAxisArray�������ڸ��¶���ĳ�Ա����������λ�ã��ٶȣ������״̬�Ȳ�����ThreadProc������һ����ѭ����
ѭ��֮����һ��case������ж����ڵ�ǰ����ִ���Ǹ������ġ�
2��ͬʱ��������һ��������ghEventCanVisit�������¼���ghEventAxisMotionFinised[4]��ghComandFinishedEvent_Controller
������������ִ�в�ͬ����ʱ����֮�䲻��ͻ��ghEventAxisMotionFinised[4]�������û�����Ƿ��˶���ɣ��ڿ������˶���ʱ��
�����ȴ����˶���λ,�������뿪ThreadProc������һ��ѭ�����ڣ�ghComandFinishedEvent_Controller������ServoOn/ServoOff
���������йء�
*/

//��ʼ����ĸ���Ϊ4��Ȼ��newһ���ĸ�������飬Ȼ����������г�ʼ��
//����Ϊ��һ���ᣬ�ڶ�����,�������ᣬ���ĸ���
CGTController::CGTController()              //###################ע�����
{
	m_AxisNumber = 4;
	m_AxisArray = new t_Axis[m_AxisNumber];
	for (int num = 1; num <= m_AxisNumber; num++)
	{
		m_AxisArray[num - 1].AxisNo = num;
		m_AxisArray[num - 1].CurrentPosition = 0;
		m_AxisArray[num - 1].CurrentVelocity = 0;   //**********************@wqq  �����ٶ���
		/*
		m_AxisArray[num-1].GoalPosition=0;
		m_AxisArray[num-1].GoalVelocity=0;
		m_AxisArray[num-1].GoalAcc=0;
		m_AxisArray[num-1].GoalJerk=0;
		m_AxisArray[num-1].GoalMaxAcc=0;
		*/
		m_AxisArray[num - 1].CurrentState = 0;

	}
	m_ServoIsOn = false;  //��ʼ���ŷ���־λΪ���ϵ�
   //�̸������ﻹ������ͬ���Ķ��󣬻�����ghEventCanVisit���¼�ghEventAxisMotionFinised���¼�ghComandFinishedEvent_Controller



	//�̸������ﻹ�ж࿪�������߳�m_ThreadHandle����m_hControlThread�������߳�
}


CGTController::~CGTController()        //###################ע�����
{
	if (m_AxisArray != NULL)
	{
		delete[] m_AxisArray;
		m_AxisArray = NULL;
	}
	
	GT_ExOpt(0xffff);    //�ú��������˶�������ͨ�������������״̬��Bit(i)----->EXO(i)  i=0~15

	//������̹߳ر��̵߳ľ��
	//����л������������¼����ر��¼����ľ��
	ServoOff();    //�ŷ��ر�
}

//������ʹ��  ���ǳ�ʼ���忨�ĵ�һ��
short CGTController::EnableController(void)   //###################ע�����
{
	short Rtn;

	Rtn = GT_Close(); //�ȹر�
	Sleep(100);
	Rtn = GT_Open();	//�ٴ�  ��
	if (Rtn != 0)
	{
		AfxMessageBox(_T("���˶����ƿ�����"));
		exit(1);  //�˳�Ӧ�ó��򣬹رյ�ǰ����
	}

	Rtn = GT_Reset();	//���λ�˶����ƿ�
	Sleep(300);
	if (Rtn != 0)
	{
		AfxMessageBox(_T("����GT_Reset����"));
		exit(1);    //�˳�Ӧ�ó��򣬹رյ�ǰ����
	}

	Rtn = GT_SetSmplTm(200);  //���ÿ���������200us ****************************���������Ը��ĵĸ�С
	if (Rtn != 0)
	{
		AfxMessageBox(_T("����GT_SetSmplTm����"));
		exit(1);    //�˳�Ӧ�ó��򣬹رյ�ǰ����
	}

	Rtn = GT_LmtSns(LIMIT_SENSE); //������λ���ص͵�ƽ��������ĳ�������ϵ���λ���ش���ʱ���˶�����������
	//��ֹͣ�÷����ϵ��˶����Ա���ϵͳ��ȫ�����뿪��λ�����Ժ󣬱������ָ��GT_ClrSts 
	//	��������������λ״̬,Ĭ���ǳ��տ��أ��������ǵ͵�ƽ���������ó�255��Ϊ��
	if (Rtn != 0)
	{
		AfxMessageBox(_T("����GT_LmtSns����"));
		exit(1);    //�˳�Ӧ�ó��򣬹رյ�ǰ����
	}

	//*********�̸��˶������ṩ��ԭ���ź�����ӿڡ�Ĭ��������˶����ƿ���ԭ���ź����½��ش�������Ȼ����
	//*********����ָ��GT_HomeSns�ܹ��ı��˶����ƿ��ı��ش�����ʽ     

	//*********�̸��˶������ṩ�˱������ĵ���ı�����������������ã�ֻ�е����ת��������ͱ��������������������һ�£����ܱ�֤
	//*********�˶�����������������GT_EncSns���Ըı������ı�������������ȡ1�����Ӧ�Ŀ�����ı������ļ�������ȡ����

	return 0;
}
//�忨��ʼ��  �˶��������ʼ��
short CGTController::InitCard(void)            //###################ע�����
{
	/*���ﲻ֪��ΪʲôҪ��100����ת������������*/
	double	m_nAcc = 15 / PAxisReduce;		//�������ٶ�x100  /  PAxisReduce
	double  m_MAcc = 30 / PAxisReduce;		//�����ٶ�x100  /  PAxisReduce
	double  m_Jerk = 10 / PAxisReduce;		//�Ӽ��ٶ�x100    /  PAxisReduce
	short	m_nILimit = 1000 ;
	long	m_nPos = 0;		//����λ��x100  /  PAxisReduce
	double	m_nVel = 500 / PAxisReduce;		//�����ٶ�x100  /  PAxisReduce
	int		i;


	for (i = 1; i <= 4; i++)  
	{
		GT_Axis(i);   //���õ�һ����Ϊ��ǰ��
		GT_ClrSts();  //�����ǰ�᲻��ȷ״̬
		//*********GT_CtrlMode��mode�� ���Ŀ�������Ĭ�����ģʽ�����������ģ����(mode=0)�������������mode=1)��Ĭ�����ģ����

		//*********����Ϊ�ջ��Ŀ��Ʒ�ʽ�����ֿ���ʵ��׼ȷ��λ�ÿ��ƣ�SV������Ĭ�ϵĿ��Ʒ�ʽΪ�ջ����ƣ���������GT_OpenLp()���ÿ���

        //PID�����������ã��̸߹ٷ��ֲ����ֵ
		if (i == 1)
		{
			GT_SetKp(11);  
			GT_SetKi(2);
			GT_SetKd(8);
		}
		if (i == 2)
		{
			GT_SetKp(13);
			GT_SetKi(5);
			GT_SetKd(5);
		}
		if (i == 3)   //�ص��޸��⼸��ֵ����ʹ���迹����ʱ��ͬ��
		{
			GT_SetKp(5);
			GT_SetKi(5);
			GT_SetKd(8);
		}
		if (i == 4)
		{
			GT_SetKp(13);
			GT_SetKi(4);
			GT_SetKd(5);
		}

		//*********�����������ٶ�ǰ��GT_SetKvff(0~32767),���ٶ�ǰ��GT_SetKaff(0~32767)�������GT_SetMtrBias(-32768~32768)��
		//*********��������ȡֵ��Χ+/2^15����Ӧ��ģ�������Ϊ+/-10v ,ʵ�ʿ��ƿ�����GT_SetILmt(0~32767)������Ч�����Χ��Ĭ��Ϊ32767

		GT_PrflT();  //�����˶�ģʽ����������ģʽ����ΪT������������ʱ�̶����Ըı��ٶȺ�λ�ã���S������ֻ�ܸı�λ��,��������Ը�ǿ
		GT_SetVel(m_nVel / 100);   //��������ٶ�   ��λ��Pulse /ST
		GT_SetAcc(m_nAcc / 100);   //���������ٶ�   ��λ��Pulse /ST^2
	//	GT_SetJerk(m_Jerk / 100);    @wqq  ����Ϊ����д���ˣ��������ֻ����������S����ģʽ�µ����Ӽ��ٶȡ�
	//	GT_SetMAcc(m_MAcc / 100);    @wqq  ����Ϊ����д���ˣ��������ֻ����������S����ģʽ�µ������ٶȡ�
		GT_SetPos(m_nPos / 100);   //����Ŀ��λ�ã���������0.
		GT_LmtsOn();               //ʹ��ǰ�����λ������Ч
		GT_SetIntrMsk(0);          //���ε�ǰ����ж������֣�0 ʱ���������У����򡢸��򿪹ء�Home���񡢶ϵ��������㡢���������������˶���ɵ��ж��¼������������жϡ�
		GT_Update();               //ʹ��Щ��������һ������������Ч
	}
	return 1;
}
//�ŷ��ϵ�
short CGTController::ServoOn(void)       //###################ע�����
{
	//����1~4�ᣬ���״̬
	for (int i = 1; i <= 4; i++)
	{
		GT_Axis(i);  
		GT_AxisOn();   //����ʹ��   
		GT_ClrSts();   //�����ǰ���¼�״̬��־λ�����Bit0~Bit7;
		Sleep(80);
	}
	//���ŷ�������־λ
	m_ServoIsOn = true;
	//�̸��˶��������ﻹ�ж������̵߳Ļָ������о��ǽ�ghComandFinishedEvent_Controller�ĳɼ���״̬
	
   //���������ʦ����ӵģ������Ǹ������λ��״̬���ظ�������
	UpdateAxisArray();//ˢ�������� 	 ����ĵ�ǰ��λ��ȡ����ĳ�Ա����֮��

	return 0;
}

//�������λ��״̬���ظ�������   ����ĵ�ǰ��λ��ȡ����ĳ�Ա����֮��
//��ÿ��Ԫ�������֮��
short CGTController::UpdateAxisArray()      //###################ע�����
{
	t_Axis *axisArray = m_AxisArray;  //��仰����һ�������ԣ���ʵ����axisArray��m_AxisArray��ָ��ͬһ���ڴ�
	int axisNumber = 4;

	for (int axisIndex = 1; axisIndex <= axisNumber; axisIndex++)
	{
		GT_Axis(axisIndex);   
		GT_GetAtlPos(&axisArray[axisIndex - 1].CurrentPosition);//��ȡʵ��λ�ø�ֵ��CurrentPosition
		GT_GetAtlVel(&axisArray[axisIndex - 1].CurrentVelocity);  //**********************@wqq  �����ٶ���
		GT_GetSts(&axisArray[axisIndex - 1].CurrentState);//��ȡ��ǰ��ֵ��CurrentState
	}
	return 0;
}
//�ŷ��ϵ�
short CGTController::ServoOff(void)             //###################ע�����
{
	//�̸������ﻹʹ���˵ȴ����������ȴ�ghEventCanVisit�ͷţ��ں���������ͷ����������
	//�̸��˶��������ﻹ�ж������̵߳���ͣ�����о��ǽ�ghComandFinishedEvent_Controller�ĳ�δ����״̬

	int i;
	for (i = 1; i <= 4; i++)
	{
		GT_Axis(i);
		GT_StpMtn(); //��������ֹͣ�˶�
	}
	GT_MltiUpdt(0xf); //ͬ��ֹͣ�˶�
	Sleep(100);

	for (i = 1; i <= 4; i++)
	{
		GT_Axis(i);
		GT_AxisOff(); //�ر�����
	}

	m_ServoIsOn = false; //�ŷ��򿪱�־λ��Ϊ��

	return 0;
}

/*
    ��ʵ����������˵����
	    1���ڹ̸߹�˾�ĳ���������������Ǹ������˻���Ķ�����õģ���RotbotBase,���ǿ��Կ�����GetAxisPositionAndState�������
		��û��ˢ���Լ��ĳ�Ա����m_AxisArray[]������
		2��Ϊ�˽��ˢ�µ����⣬������̸ߵĺ���������threadProc_UpdateAxisArray��m_AxisArray������̺߳���������̺߳�����
		��һ��while(true������ѭ�����������ѭ������һֱ���ĸ����ˢ��״̬�Ĺ�����	
*/
//��������ᱻ�ϲ�Ļ��������еĺ�������,���ﴫ���ָ�룬����������ʵ�ı������������ֵ
short CGTController::GetAxisPositionAndVelocityAndState(long pos[4], double vel[4],unsigned short state[4])    //###################ע�����
{
	short rtn;
	//���������ʱ��ֱ��ˢ�����������ֵ
	for (int i = 1; i <= m_AxisNumber; i++)
	{
			GT_Axis(i);///ѡ���i����
			rtn=GT_GetAtlPos(&m_AxisArray[i-1].CurrentPosition);   //��ȡ��i�����ʵ��λ��
			if (rtn!=0)
			{
				AfxMessageBox(_T("���ú���GT_GetAtlPos��ȡʵ��λ��ʧ��!"), MB_OK);
			}
			rtn=GT_GetAtlVel(&m_AxisArray[i-1].CurrentVelocity);    //��ȡ��i�����ʵ���ٶ�
			if (rtn != 0)
			{
				AfxMessageBox(_T("���ú���GT_GetAtlVel��ȡʵ���ٶ�ʧ��!"), MB_OK);
			}
			rtn=GT_GetSts(&m_AxisArray[i-1].CurrentState);        //��ȡ��i�����ʵ��״̬
			if (rtn != 0)
			{
				AfxMessageBox(_T("���ú���GT_GetSts��ȡʵ��״̬ʧ��!"), MB_OK);
			}
	}
	for (int i = 0; i < m_AxisNumber; i++)
	{
		pos[i] = m_AxisArray[i].CurrentPosition;    //��ȡ��ǰ��λ��
		vel[i] = m_AxisArray[i].CurrentVelocity;     //**********************@wqq  �����ٶ���
		state[i] = m_AxisArray[i].CurrentState;     //��ȡ��ǰ��״̬
	}
	return 0;
}

/*����homeλ��ʹ��Home���أ����������λ���˶���Home���λ�ã�������λ����ΪPos��0λ�á�
//****************************������������һ��bug,�������򸺷����˶���ʱ���п��ܻ���������λ���ء���δ�޸���
 ���룺
    axisno   :  ���number��,SCARA������ֻ����[1:4]
	maxNegativeOffset  :  ���԰����򵽴�homeλ�õ����������
	maxPositiveOffset  :  �������û�е��︺����λ��ʱû������ԭ��λ�ã���ת������������λ��ȥ��ȡHomeλ��
 �����
    1  :  �ɹ�
	-1 �� ʧ��
*/
short CGTController::AxisCaptHomeWithHome(int axisno, long maxNegativeOffset, long maxPositiveOffset, double vel)   //###################ע�����
{
	//�̸������ﻹʹ���˵ȴ����������ȴ�ghEventCanVisit�ͷţ��ں���������ͷ����������
	short Rtn;
	long pos;
	unsigned short str = 0;
	bool bFound = false;

	Rtn = GT_Axis(axisno);  
	Rtn = GT_ClrSts();

	Rtn = GT_CaptHome();//���øú�����λ�ò���Ĵ�������¼Home�źŵ���ʱ��ʵ��λ��,����״̬�Ĵ����е�bit14��Ϊ1��

	Rtn = GT_GetAtlPos(&pos);  //��ȡ��ǰ���ʵ��λ��
	if (Rtn != 0) return -1;
	//������������
	Rtn = GT_SetPos(pos + maxNegativeOffset);    //long(185.*10000/2/180.* 80));
	if (Rtn != 0) return -1;
	GT_SetVel(vel);
	Rtn = GT_SetAcc(0.1);
	GT_Update();
	// Sleep(10);

	GT_GetSts(&str);  
	while ((str & 0x400)) //��״̬�Ĵ�����ʮλ����������˶�����Ϊ1.����Ϊ0��
	{//�������˶�ʱ
		if (str & 0x08) {	//��״̬�Ĵ���bit3 , �ڿ�������⵽Ҫ���home��������֮�󣬸�λ��Ϊ1���������bit14 
			bFound = true; //��bfound��־
			break;
		}
		GT_GetSts(&str);
	}

	if (!bFound) //�������ֹͣ�˶��˻�û�ҵ�Home
	{
		Rtn = GT_Axis(axisno);
		Rtn = GT_ClrSts();
		//������������
		Rtn = GT_SetPos(pos + maxPositiveOffset);//-long(185.*10000/2/180.* 80));
		Rtn = GT_SetVel(vel);
		Rtn = GT_SetAcc(0.1);
		Rtn = GT_Update();

		GT_GetSts(&str);
		while ((str & 0x400))
		{		
			if (str & 0x08)  //��״̬�Ĵ���bit3 , �ڿ�������⵽Ҫ���home��������֮�󣬸�λ��Ϊ1���������bit14 
			{
				bFound = true;
				break;
			}
			GT_GetSts(&str);
		}
	}
/*	//��ʱ�ҵ���Home������ֹͣ
	Rtn = GT_SmthStp();
	Rtn = GT_Update();
	Sleep(500);

	GT_Axis(axisno);
	GT_ClrSts();
	GT_ZeroPos();
	*/       //       @wqq   ����Ϊ����д�Ĳ��ԣ���Ӧ������ֹͣ��Ӧ��ȥȡ����λ�õ�Ŀ��ֵ����ʹ����ֵΪĿ��ֵ��
//   @wqq    ���ﰴ�����Լ����뷨��д
	Rtn = GT_SmthStp();
	Rtn = GT_Update();
	GT_GetSts(&str);
	while ((str & 400))  //�ȴ������ֹͣ
	{
		GT_GetSts(&str);
	}
	GT_Axis(axisno);
	GT_ClrSts();
	Rtn = GT_GetCapt(&pos);  //��ȡ����λ��
	Rtn = GT_SetPos(pos);   //���ò���λ��ΪĿ��λ��
	Rtn = GT_Update();     //ˢ��״̬
	Rtn = GT_GetSts(&str);
	while ((str & 0x400))
	{
		Rtn = GT_GetSts(&str);  //������û���˶���ԭ��λ��
	}
	Rtn = GT_ClrSts();       //���״̬��ɱ�־λ
	Rtn = GT_ZeroPos();       //����ǰ���ʵ��λ�üĴ�����Ŀ��λ���Լ���ǰ�������ڵĹ滮λ�üĴ�����Ϊ��ֵ��
//  @wqq
	return 1;
}
/*
���ø�����λ�ò���ԭ�㣬��ͨ���������嵽��ԭ��λ�ã�������λ������Ϊ��㡣������Ϊ�������������û��ԭ�㿪���뵽��ӽ�ԭ��������
	���룺
		 axisno   :  ���number��,SCARA������ֻ����[1:4]
		 offset  : �Ӹ�����λ�õ�Homeλ�õ�����ƫ����
		 vel     �� �˶������е��ٶ�ֵ
   �����
		 1  :  �ɹ�
		 -1 �� ʧ��
*/
short CGTController::AxisCaptHomeWithLimit(int axisno, long offset, double vel)                  //###################ע�����
{
	long pos;
	unsigned short str = 0;
	bool flag = false;

	///first go to the negative limit
	if (AxisCaptLimit(axisno, -999999, vel) != 1)  //������︺����λ��û�гɹ�����������-1��ʧ�ܣ�
		return -1;

	////�̸������ﻹʹ���˵ȴ����������ȴ�ghEventCanVisit�ͷţ��ں���������ͷ����������
	GT_Axis(axisno);
	GT_ClrSts();
	Sleep(50);
	GT_GetAtlPos(&pos);   //��õ�ǰ���ʵ��λ��ֵ   //*************�ڵ���AxisCaptLimitʱ��ʵ�Ѿ�������λ��ʱ�ĺ���������0�����˾��ô�ʱPos=0;
	///GT_PrflT();
	GT_SetPos(pos + offset);
	GT_SetVel(vel);
	GT_SetAcc(0.01);
	GT_Update();
	GT_Axis(axisno);
	while (!flag)  //�ж��Ƿ��˶���λ
	{
		GT_GetSts(&str);
		if ((str & 0x1) || (!(str & 0x400))) flag = true;   
	}

	GT_Axis(axisno);
	GT_ClrSts();
	GT_ZeroPos();

	////�̸������ﻹʹ���˵ȴ����������ȴ�ghEventCanVisit�ͷţ��ں���������ͷ����������

	return 1;
}

/*
������λ�� ���������˶�������λ�ã�������ø�λ��Ϊ��㡣
     ���룺
		axisno   :  ���number��,SCARA������ֻ����[1:4]
		offset  :  offset �����һ��������ʱ����������ᵽ��������λ�ã�Ϊ������ʱ��������λ�á�
		vel     �� ȥ����λ��ʱ���ٶ�ֵ
�����
		1  :  �ɹ�
		-1 �� ʧ��
*/

short CGTController::AxisCaptLimit(int axisno, long offset, double vel)             //###################ע�����
{
	////�̸������ﻹʹ���˵ȴ����������ȴ�ghEventCanVisit�ͷţ��ں���������ͷ����������

	unsigned short str = 0;
	unsigned short limitFlag = offset>0 ? 0x20 : 0x40;  //�ж�offset���������Ǹ���������limitFlag=0x20��������limitFlag=0x40��
	long pos;

	GT_Axis(axisno);
	GT_ClrSts();

	GT_GetAtlPos(&pos);    //��ȡ��ǰ���ʵ��λ��
	GT_SetPos(pos + offset);   //������/������λ��
	GT_SetVel(vel);
	GT_Update();

	GT_GetSts(&str);
	while (str & 0x400)   //�ж����Ƿ����˶�
	{		
		if (str&limitFlag)   //0x20����״̬�Ĵ�����������λ�����Ƿ񱻴�����0x40����״̬�Ĵ����ĸ�����λ�����Ƿ񱻴�����
			break;
		GT_GetSts(&str);

	}
	GT_SmthStp();     	//����ֹͣ�����˶� 
	GT_Update();    

	GT_Axis(axisno);  
	GT_ClrSts();       //���״̬��־λ
	GT_ZeroPos();      //���Ŀ��λ�ú�ʵ��λ��Ϊ0��ֻ�е���ֹͣʱ��Ч

	return 1;
}

/*
���ڻ�ȡ�ĺ����ԭ�㣬�ĺ�����û����λ���صģ��˶���ָ����λ�ã�������λ�����ó���㡣���������Գ���ԭ��λ�ã�

*/
short CGTController::AxisCaptHomeWithoutLimit(int axisno, double vel)     //###################ע�����
{

	////�̸������ﻹʹ���˵ȴ����������ȴ�ghEventCanVisit�ͷţ��ں���������ͷ����������

	short Rtn;
	long pos;
	unsigned short str = 0;
	bool bFound = false;

	Rtn = GT_Axis(axisno);
	Rtn = GT_ClrSts();

	Rtn = GT_CaptHome();  //����Home����

	if (Rtn != 0) return -1;
	Rtn = GT_GetAtlPos(&pos);   //��ȡ��ǰ���λ��
	if (Rtn != 0) return -1;   
	Rtn = GT_Axis(axisno);
	Rtn = GT_ClrSts();
	Rtn = GT_SetPos(pos - long(180.*10000.*24. / 360.));   //����ת��180��    24�Ǽ��ٱ�
	Rtn = GT_SetVel(vel);
	Rtn = GT_SetAcc(0.1);
	Rtn = GT_Update();

	GT_GetSts(&str);
	while ((str & 0x400))
	{
		GT_GetSts(&str);
	}
	//}
	Sleep(10);

	Rtn = GT_Axis(axisno);
	Rtn = GT_ClrSts();
	Sleep(10);
	Rtn = GT_CaptHome();
	Sleep(10);
	Rtn = GT_GetAtlPos(&pos);
	Rtn = GT_SetPos(pos + long(360.*10000.*24. / 360.));
	GT_SetVel(vel);
	Rtn = GT_SetAcc(0.1);
	GT_Update();
	GT_GetSts(&str);
	while ((str & 0x400))
	{	
		if (str & 0x08)     //Home����λ�����ź�
		{
			bFound = true;  //�ҵ���Home
			break;
		}
		GT_GetSts(&str);
	}

	Rtn = GT_SmthStp();  //����ֹͣ
	Rtn = GT_Update();   
	Sleep(200);
	GT_Axis(axisno);
	GT_ClrSts();
	GT_ZeroPos();     //���ø�������ֹͣʱ��λ��Ϊ��㡣
	GT_Update();

	//***********************************�����Ҿ�����Ҫ�޸ģ�Ϊʲô��ֱ�Ӷ�ԭ�㿪�ص�λ���ˣ�������ԭ��λ�ÿ��ذ�װλ�ñȽ�ƫ��
	GT_SetVel(vel);
	GT_SetAcc(0.01);
	GT_SetPos(long((-68)*10000.*24. / 360.));		//�������������ĽǶ�ΪY+ ��ǰ��
	//GT_SetPos(long(/*-17.*/(-51.+1.7415)*10000.*24./360.));
	GT_Update();
	GT_GetSts(&str);
	while (!(str & 0x01))  //��״̬�Ĵ�����һλ���˶���ɱ�־λ
	{
		GT_GetSts(&str);
	}
	GT_ZeroPos();    //�����λ�����ó����


	////�̸������ﻹʹ���˵ȴ����������ȴ�ghEventCanVisit�ͷţ��ں���������ͷ����������

	return 0;
}

//��������ģʽ�˶�
/*
   ���룺  
   axisno ��   ���number��,SCARA������ֻ����[1:4]
   pos    ��   �趨�˶�����Ŀ��λ��
   vel    ��   �趨�˶���������ٶ�
   acc    ��   �趨�˶����������ٶ�
*/
short CGTController::AxisMoveToWithTProfile(int axisno, long pos, double vel, double acc)   //###################ע�����
{
	GT_Axis(axisno); //���õ�ǰ��
	GT_ClrSts();	//���״̬λ
//	GT_PrflT();		//����ģʽ************************@wqq   �����ڳ�ʼ���忨��ʱ��ͽ��˶����ó�T�����ߵ�ģʽ����������������������Ϊ�ı��˶�ģʽ�������������
	GT_SetPos(pos); //����Ŀ��λ��
	GT_SetVel(vel);	//���������ٶ�
	GT_SetAcc(acc); //����������ٶ�
	GT_Update();	//ˢ�¸���
	/*  ʦ���޸ĵģ�@wqq����������Ͳ�����
//	wait_motion_finished(axisno); //�ȴ������˶�����
//	UpdateAxisArray(); //����������   *///@wqq����������Ͳ�����
	return 0;
}

/*
���룺
	pos    ��   �趨�˶�����Ŀ��λ��
	vel    ��   �趨�˶���������ٶ�
	acc    ��   �趨�˶����������ٶ�
*/
//����ͬʱ����ģʽ�˶�
//ʵ��һϵ���˶��Ļ�����///ʵ���ĸ����������ٶȺͼ��ٶ��˶���Ŀ��λ��
short CGTController::MoveToWithTProfile(long pos[4], double vel[4], double acc[4])       //###################ע�����
{
	for (int i = 0; i < 4; i++)
	{
		if (vel[i]>10) vel[i] = 10;
	}
	for (int i = 1; i <= 4; i++) //������������״̬
	{
		GT_Axis(i);
		GT_ClrSts();
//		GT_PrflT();   //����ģʽ************************@wqq   �����ڳ�ʼ���忨��ʱ��ͽ��˶����ó�T�����ߵ�ģʽ����������������������Ϊ�ı��˶�ģʽ�������������
		GT_SetPos(pos[i - 1]);
		GT_SetVel(vel[i - 1]);
		GT_SetAcc(acc[i - 1]);
	}
	GT_MltiUpdt(0xF); //ͬʱˢ�¶���״̬
/*	for (int i = 1; i <= 4; i++)
		wait_motion_finished(i);
	return 0;
	UpdateAxisArray();   *///@wqq   ʦ���޸ĵģ����������ﲻ���ˡ�
	return 0;
}

/*
  ����
	dout  �� bitλ
*/
//���������  ֱ�ӿ��Ƶ�ŷ��Ŀ���
short CGTController::DigitalOut(unsigned int dout)      //###################ע�����
{
	GT_ExOpt(dout);  //�ú��������˶�������ͨ�������������״̬��Bit(i)----->EXO(i)
	Sleep(20);
	return 0;
}

/*
���룺
	AxisNo  :  �ڼ����� ��NO. 1~4
*/
//�ȴ�ֱ������ֹͣ�˶�,�ú�����һֱ�ȴ�ֱ��������˶�
void CGTController::wait_motion_finished(int AxisNo)     //###################ע�����
{
	bool flag = false;
	unsigned short str = 0;

	GT_Axis(AxisNo);
	while (!flag)  //�ж��Ƿ��˶���λ
	{
		GT_GetSts(&str); //��ȡ�˶�״̬�Ĵ���
		if ((str & 0x1) || (!(str & 0x400))) flag = true;   //�᲻���˶��������˶���ɱ�־λ��1��һ������ʱ
	}
	GT_ClrSts(); //***************@wqq �Լ���ӵ�
}

/////////////////////////////////////////////////2018.4.24 ���
//��ʼʹ��S�������滮
bool CGTController::StartUsingSProfile()
{
	short rtn;
	for (int i = 0; i < 4; i++)
	{
		rtn = GT_PrflS();
		rtn = GT_SetJerk(0.000002);
		rtn = GT_SetMAcc(0.004);
		rtn = GT_SetVel(4);
	}
	GT_MltiUpdt(0xF); //ͬʱˢ�¶���״̬
	return true;
}

//�����S�������˶�
short CGTController::MoveToWithSProfile(long pos[4])
{
	for (int i = 1; i <= 4; i++) //������������״̬
	{
		GT_Axis(i);
		GT_ClrSts();
		GT_SetPos(pos[i - 1]);
	}
	GT_MltiUpdt(0xF); //ͬʱˢ�¶���״̬
	return 0;
}

//�����S�������˶�	
short CGTController::AxisMoveToWithSProfile(int axisno, long pos)
{
	GT_Axis(axisno); //���õ�ǰ��
	GT_ClrSts();	//���״̬λ
	GT_SetPos(pos); //����Ŀ��λ��
	GT_Update();	//ˢ�¸���
	return 0;
}
///////////////////////////////////////////////////2018.4.24 ��ӽ���