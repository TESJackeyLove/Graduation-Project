#include "stdafx.h"
#include "Impedance.h"
#include <conio.h> //ʹ�������п���

//���Ա��֮��ķ�֧

#define DEBUG

#define ATIForce   //ʹ��ʵ�ʵ���������
///////////////////////////////////////////////////////////


extern SOCKET sockClient; //ȫ�ֱ������ͻ��˵��׽���
//////���嶨ʱ����
#define Tms (15)   
#define T (Tms*0.001)
////////////////////////
int testNUM = 0;
int timenum = 0;
bool UpOrDown = 0;  //0Ϊup,1Ϊdown
HANDLE hSyncEvent;//ͬ���¼����
bool ImpedenceControllerStopflag = true; //�߳̽�����־,ȫ�ֱ�����ʼ��Ϊһ��ʼ����ֹͣ
//�迹���ƴ�Home�㿪ʼ�˶�����Home���ʱ��GaptoPositive=0;

HANDLE ImpedanceStopMutex; //��֤
DWORD WINAPI ThreadProc(LPVOID lpParam)
{
	CImpedance *pImpedence = (CImpedance *)lpParam;  //��ȡ��ָ��
	RobotData MyRobotData;
	ResetEvent(hSyncEvent);  //ȷ���¼��������ź�״̬
//	AllocConsole();//ע���鷵��ֵ   ����ʹ������̨����Ϊ����ں�����ʹ������ǳ�������ʱ�䣬������ʾ�����11ms
	LARGE_INTEGER litmp;
	LONGLONG qt1=0, qt2=0,qt1last;
	double dft, dfm1, dfm2, dff;
	//���ʱ��Ƶ��  
	QueryPerformanceFrequency(&litmp);//���ʱ��Ƶ��  
	dff = (double)litmp.QuadPart;

	ImpedanceStopMutex = CreateMutex(NULL, false, NULL); //���߳�������
	while (1)
	{	
		WaitForSingleObject(ImpedanceStopMutex, INFINITE);
		if (ImpedenceControllerStopflag)
		{
			ReleaseMutex(ImpedanceStopMutex);
			break;
		}   
		else
		{   
			WaitForSingleObject(hSyncEvent, INFINITE);
			////////////////////////////////////////////////����ʱ����뿪ʼ
#ifdef DEBUG
			QueryPerformanceCounter(&litmp);
			qt1last = qt1;
			qt1 = litmp.QuadPart;//��õ�ǰʱ��t2��ֵ 
			//��ö�Ӧ��ʱ��ֵ��ת�����뵥λ��  
			dfm1 = (double)(qt1 - qt2);
			dft = dfm1 / dff;
			TRACE("waited time:t1-t2=%.3f\n", dft * 1000);
			//		TRACE("testNUM=%d\n", testNUM);	
			dfm1 = (double)(qt1 - qt1last);
			dft = dfm1 / dff;
			TRACE("One cycle time:t1-t1last=%.3f\n", dft * 1000);
#endif
	
			if (!(pImpedence->m_Robot->m_isOnGap))
			{
				pImpedence->GetCurrentState();//��ȡ��ǰ��ʱ�̵Ĺؽڿռ���ٶȣ�λ�ú�ֱ������ռ��λ�ã��ٶ�
#ifdef DEBUG
				for (int i = 0; i < 4; i++)
				{
					TRACE("the %d��axis theta is: %.3f\n", i, pImpedence->m_thetaImpedPara[i].Now);
					TRACE("the %d' axis angelVel is: %.3f\n", i, pImpedence->m_angularVelImpedPara[i].Now);
				}

#endif
				pImpedence->GetNextStateUsingJointSpaceImpendenceWithSpeedWithTProfile();  //������һ��ʱ�̵ĹؽڵĽǶȺͽ��ٶȲ�ִ��

				////������TCP/IP����
				memset(&MyRobotData, 0, sizeof(MyRobotData));
				for (int i = 0; i < 4; i++)
				{
					MyRobotData.JointsNow[i] = pImpedence->m_thetaImpedPara[i].Now;
					MyRobotData.JointsVelNow[i] = pImpedence->m_angularVelImpedPara[i].Now;
					MyRobotData.JointsTorque[i] = pImpedence->ExtTorque[i];
				}
				for (int i = 0; i < 6; i++)
				{
					MyRobotData.Origin6axisForce[i] = pImpedence->ForceSensor[i];
				}

				char buff[sizeof(MyRobotData)];
				memset(buff, 0, sizeof(MyRobotData));
				memcpy(buff, &MyRobotData, sizeof(MyRobotData));
				send(sockClient, buff, sizeof(buff), 0);
			}
			ReleaseMutex(ImpedanceStopMutex);

#ifdef DEBUG  ////////////////////////////////����ʱ����뿪ʼ
			QueryPerformanceCounter(&litmp);
			qt2 = litmp.QuadPart;//��õ�ǰʱ��t3��ֵ 
			dfm2 = (double)(qt2 - qt1);
			dft = dfm2 / dff;
			TRACE("The program running time:t2-t1=%.3f\n", dft * 1000);
#endif
			ResetEvent(hSyncEvent);//��λͬ���¼�
		}
	}

	GT_SetIntSyncEvent(NULL);//֪ͨ�豸ISR �ͷ��¼�
	CloseHandle(hSyncEvent); //�ر�ͬ���¼����
	ExitThread(0);
	return 0;
}

CImpedance::CImpedance(CRobotBase *Robot)
{
	m_Robot = Robot;
	m_RunningFlag = false;
	for (int i = 0; i < 4; i++)
	{
		if (i == 0 || i == 1 )
		{
			m_M[i]= 0;
			m_K[i]= 0.05;   //��λ�� N/mm  0.2
			m_B[i] = 0.03;
		}
		else if (i == 2)
		{
			m_M[i] = 0;
			m_K[i] = 0.05;   //��λ�� N/mm  0.2
			m_B[i] = 0.1;
		}
		else
		{
			m_M[i] = 0;
			m_K[i] = 0.001;   //��λ�� N/mm  0.2
			m_B[i] = 0.02;
		}

	}

	m_FImpedPara.Last = 0;
	m_FImpedPara.Now = 0;
	m_FImpedPara.Next = 0;

	for (int i = 0; i < m_Robot->m_JointNumber; i++)
	{
		m_xImpedPara[i].Last = 0;
		m_xImpedPara[i].Now = 0;
		m_xImpedPara[i].Next = 0;

		m_vImpedPara[i].Last = 0;
		m_vImpedPara[i].Now = 0;
		m_vImpedPara[i].Next = 0;
	}

	for (int i = 0; i < m_Robot->m_JointNumber; i++)    //����ֻ�������ؽ�
	{
		m_thetaImpedPara[i].Last = 0;
		m_thetaImpedPara[i].Now = 0;
		m_thetaImpedPara[i].Next = 0;
	}
	for (int i = 0; i < m_Robot->m_JointNumber; i++)    //����ֻ�������ؽ�
	{
		m_angularVelImpedPara[i].Last = 0;
		m_angularVelImpedPara[i].Now = 0;
		m_angularVelImpedPara[i].Next = 0;
	}

	m_hControlThread = NULL;
	ATIForceSensor = NULL;

}


CImpedance::~CImpedance()
{
	//if (ATIForceSensor != NULL);
	//{
	//	delete ATIForceSensor;
	//	ATIForceSensor = NULL;
	//}
}


//���Ǿ��ðѻ����˴��ݸ��迹�������ȽϺã���Ϊ�迹������һ�ֱȻ���������ϲ�Ŀ��ƣ����Ի����˲�һ�����迹���ƣ������迹����һ���л�����
bool CImpedance::StartImpedanceController()
{
	////////////////////////////////��ʼ���������˲���
	
	for (int i = 0; i < m_Robot->m_JointNumber; i++)
	{
		JointFilter[i].Init_Kalman(m_K[i], m_B[i], T);
	}
	//////////////////////////////////////
	m_Robot->UpdateJointArray(); //ˢ�¸����ؽڵ�ֵ

	for (int i = 0; i < m_Robot->m_JointNumber; i++)   //�õ������ؽڵĽǶ�,���������õ�����ֱ������λ�ã��ǶȻ���mm
	{	
		m_thetaImpedPara[i].Last = m_Robot->m_JointArray[i].LastJointPosition;
		m_thetaImpedPara[i].Now = m_Robot->m_JointArray[i].CurrentJointPositon;
	}

	for (int i = 0; i < m_Robot->m_JointNumber; i++)  //�õ������ؽڵĽ��ٶ�  �˿��������ٶ�
	{
		m_angularVelImpedPara[i].Last = m_Robot->m_JointArray[i].LastJointVelocity;
		m_angularVelImpedPara[i].Now = m_Robot->m_JointArray[i].CurrentJointVelocity;
	}

	//Ҫֱ�Ӳɼ�������Ϣ
	ATIForceSensor = CForceSensor::getForceSensorInstance();  //�����ĳ�ʼ��
	ATIForceSensor->InitForceSensor();
	ATIForceSensor->GetBias();
	ATIForceSensor->OpenBias();
	m_FImpedPara.Last = 0;   //����1N  ����λ��N
	m_FImpedPara.Now = 0;   
	m_FImpedPara.Next = 0;
	for (int i = 0; i < 6; i++)
	{
		ForceSensor[i] = 0;
	}
	for (int i = 0; i < 4; i++)
		ExtTorque[i] = 0;
	//ATIForceSensor->UpdataForceData();
	//ForceSensor[2] = ATIForceSensor->m_ForceScrew[2];

/////////////////ֱ������ϵ����Ϣ
	m_xImpedPara[0].Last = m_Robot->m_HandCurrTn[0][3];   //��X���ߵ�λ��
	m_xImpedPara[1].Last = m_Robot->m_HandCurrTn[1][3];   //��Y���ߵ�λ��
	m_xImpedPara[2].Last = m_Robot->m_HandCurrTn[2][3];   //��z���ߵ�λ��

//�˴�Ӧ�û����ٶȣ���Ҫ���


//����һ���µ��̣߳�������߳�����ʹ��һ����ʱ��һֱˢ��
	unsigned short Status;
	hSyncEvent = CreateEvent(NULL, true, false, NULL); //WIN32 API ����
	if (hSyncEvent == INVALID_HANDLE_VALUE)
	{
		AfxMessageBox(_T("������ʱ�����ʧ��!"), MB_OK);
	}
	GT_SetIntrTm(75);  //���ö�ʱ���Ķ�ʱ����Ϊ75*200us = 15ms
	GT_TmrIntr();   //���������붨ʱ�ж�
	//GT_GetIntr(&Status);   //���windows������������������ 
//	if (&Status != 0)
	//{
	//	AfxMessageBox(_T("��ʱ���ж����ó���!"), MB_OK);
//		exit(1);
//	}
	GT_SetIntSyncEvent(hSyncEvent);//ΪPCI���ƿ������ж�ͬ���¼������ò���ΪNULLʱ���ú�����λ��ǰ������ֵ
	ImpedenceControllerStopflag = false;
	m_hControlThread = CreateThread(NULL, 0, ThreadProc, (LPVOID)(this), CREATE_SUSPENDED, NULL);  //���춨ʱ����������������ú���,���迹���ƶ����ָ�븳��Ϊ���̺߳����Ĳ���
	SetThreadPriority(m_hControlThread, THREAD_PRIORITY_HIGHEST);
	ResumeThread(m_hControlThread);
	if (m_hControlThread == NULL)
	{
		AfxMessageBox(_T("�����߳�ʧ��!"), MB_OK);
		return false;
	}

	return true;
}

//�ر��迹����������Ҫ�ǽ����϶�����⣬��֤��ֹͣ��ʱ�򣬱�֤��϶GaptoPositive=0;
bool CImpedance::StopImpedanceController()
{
	ImpedenceControllerStopflag = true;
	WaitForSingleObject(ImpedanceStopMutex, INFINITE);
	this->m_Robot->m_pController->wait_motion_finished(1);
	if (this->m_Robot->m_JointGap[0].GapToPositive != 0)
	{
		if (this->m_Robot->m_isGapCorrespond == false)     //�ڸ�-->��ת�۵㴦
		{
			this->m_Robot->m_isOnGap = true;   //��ʼ������϶�ˣ��������ϲ��迹��������ʱʲô������
			long pos;
			double vel1, acc;

			//���ؽ�ֵת��Ϊ����ֵ
			pos = (long)((this->m_Robot->m_JointArray[0].CurrentJointWithoutGapPosition + this->m_Robot->m_JointGap[0].GapLength)* this->m_Robot->m_JointArray[0].PulsePerMmOrDegree);  //�߹���-->��ת�۵㴦
			//���ٶ�תΪ�忨���ܵ��ٶ�,vel�ǽǶ�ÿ�룬������ÿ����   Ĭ�ϳ������������200us,deg/s = 
			vel1 = this->m_Robot->m_JointArray[0].NormalJointVelocity;
			//���ٶ�ֱ�Ӵ���ȥ����λһֱ��Pulse/ST^2
			acc = this->m_Robot->m_JointArray[0].NormalJointAcc;
			if (this->m_Robot->m_pController->AxisMoveToWithTProfile(1, pos, vel1, acc) != 0)  //���������˶�ģʽ
				return false;
			this->m_Robot->m_pController->wait_motion_finished(1);  //�ȴ����˶���ɺ�ֹͣ
			this->m_Robot->m_isGapCorrespond = true;     //����ƥ������
			this->m_Robot->m_JointGap[0].GapToPositive = 0;
			this->m_Robot->m_JointGap[0].GapToNegative = this->m_Robot->m_JointGap[0].GapLength - this->m_Robot->m_JointGap[0].GapToPositive;
			this->m_Robot->UpdateJointArray();			//@wqqʦ��������ӵ�
			this->m_Robot->m_isOnGap = false;   //��ɼ�϶����ʱ��������迹�����м�������
#ifdef DEBUG
			TRACE("pass the negetive to positive!\n");
#endif
		}
	}
	ReleaseMutex(ImpedanceStopMutex);
	CloseHandle(ImpedanceStopMutex);
}

bool CImpedance::GetCurrentState(void)
{
	m_Robot->UpdateJointArray(); //ˢ�¸����ؽڵ�ֵ
	for (int i = 0; i < m_Robot->m_JointNumber; i++)   //�õ������ؽڵĽǶ�,���������õ�����ֱ������λ�ã��ǶȻ���mm
	{
		m_thetaImpedPara[i].Last = m_thetaImpedPara[i].Now;
		m_thetaImpedPara[i].Now = m_Robot->m_JointArray[i].CurrentJointPositon;
	}

	for (int i = 0; i < m_Robot->m_JointNumber; i++)  //�õ������ؽڵĽ��ٶ�  �˿��������ٶ�
	{
		m_angularVelImpedPara[i].Last = m_angularVelImpedPara[i].Now;

		//������ٶ�ֱ��ʹ�ð忨���õĽǶȺ����ǲ��õ�
		//m_angularVelImpedPara[i].Now = m_Robot->m_JointArray[i].CurrentJointVelocity;
		m_angularVelImpedPara[i].Now = (m_thetaImpedPara[i].Now - m_thetaImpedPara[i].Last) / T;
	}
	/////////////////////////////ʹ�ÿ������˲���


	//�������������ã��������õ�ʱ����Ҫֱ�Ӳɼ�������Ϣ
	ATIForceSensor->UpdataForceData();
	ATIForceSensor->ForceBaseAxia(m_Robot);
	ForceSensor[0] = ATIForceSensor->m_ForceScrewBase[0];
	ForceSensor[1] = ATIForceSensor->m_ForceScrewBase[1];
	ForceSensor[2] = ATIForceSensor->m_ForceScrewBase[2];
	ForceSensor[3] = ATIForceSensor->m_ForceScrewBase[3];
	ForceSensor[4] = ATIForceSensor->m_ForceScrewBase[4];
	ForceSensor[5] = ATIForceSensor->m_ForceScrewBase[5];
	for (int i = 0; i < 6; i++)
	{
		switch (i)
		{
		case 0:
		case 1:
		case 2:
				{
					if (abs(ForceSensor[i]) < 0.1)
						ForceSensor[i] = 0;
					break;
				}
		case 3:
		case 4:
		case 5:
				{
					if (abs(ForceSensor[i]) < 0.008)
						ForceSensor[i] = 0;
					break;
				}
		}
	}

	//m_FImpedPara.Last = 10;   //����1N  ����λ��N
	//m_FImpedPara.Now = 10;
	//m_FImpedPara.Next = 10;

	/////////////////ֱ������ϵ����Ϣ
	m_xImpedPara[0].Last = m_Robot->m_HandCurrTn[0][3];   //��X���ߵ�λ��
	m_xImpedPara[1].Last = m_Robot->m_HandCurrTn[1][3];   //��Y���ߵ�λ��
	m_xImpedPara[2].Last = m_Robot->m_HandCurrTn[2][3];   //��z���ߵ�λ��


	//�˴�Ӧ�û���ֱ������ռ���ٶȣ���Ҫ�õ��ſ˱Ⱦ��󣬵��ǻ�����������ӣ����������ﻹû�ж���

	return true;
}

bool CImpedance::GetNextStateUsingJointSpaceImpendence(void)
{
	//double Torque[3] = { 10, 10, 10 };   //�����ǲ����ã����ÿ���ؽڵ����أ�ֻʹ��ǰ�����ؽڵĲ���
	//for (int i = 0; i < 3; i++)
	//{
	//	m_angularVelImpedPara[i].Next = (Torque[i] - m_K[i]*m_thetaImpedPara[i].Now) / (m_K[i]*0.01 + m_B[i]);
	//	m_thetaImpedPara[i].Next = m_thetaImpedPara[i].Now + m_angularVelImpedPara[i].Next*0.01;
	//}
	return true;
	
}


bool CImpedance::CalculateTorque(void)
{
	m_Robot->CalculateJacobiMatrix();  ///�����ſ˱Ⱦ���
	
	for (int i = 0; i < 4; i++)
		ExtTorque[i] = 0;

	double InverseJacobiTn[4][6];
	for (int i = 0; i < 4; i++)
		for (int j = 0; j < 6; j++)
			InverseJacobiTn[i][j] = m_Robot->m_JacobiTn[j][i];

	for (int i = 0; i < 4; i++)    // t=J^T * F
		for (int j = 0; j < 6; j++)
			ExtTorque[i] = ExtTorque[i] + InverseJacobiTn[i][j] * ForceSensor[j];

	if (abs(ExtTorque[1]) < 0.2) ExtTorque[1] = 0;
	if (abs(ExtTorque[2]) < 0.2) ExtTorque[2] = 0;
	if (abs(ExtTorque[3]) < 0.05) ExtTorque[3] = 0;

	return true;
}


extern double States[4][2];

bool CImpedance::GetNextStateUsingJointSpaceImpendenceWithSpeedWithTProfile(void)
{
	double RealAcc[4];
	for (int i = 0; i < this->m_Robot->m_JointNumber; i++)
	{
		RealAcc[i] = (this->m_Robot->m_JointArray[i].MaxJointAcceleration) / ((this->m_Robot->m_JointArray[i].PulsePerMmOrDegree) * (2 * 0.0001)*(2 * 0.0001));
		//TRACE("the %d��RealAcc is: %.3f\n", i, RealAcc[i]);
	}

	CalculateTorque();
	double Torque[4] = { 0, 0, 0, 0 };   
	Torque[0] = 0;
	Torque[1] = ExtTorque[1];
	Torque[2] = ExtTorque[2];
	Torque[3] = ExtTorque[3];
#ifdef DEBUG
	for (int i = 0; i < 4; i++)
	{
		TRACE("the %d��Torque is: %.3f\n", i, Torque[i]);
	}
#endif
	//////////ʹ�ÿ������˲���
	for (int i = 0; i < 4; i++)
	{
		JointFilter[i].GetKalmanStates(m_thetaImpedPara[i].Now, m_angularVelImpedPara[i].Now, Torque[i],i);
		m_thetaImpedPara[i].Now = States[i][0];
		//m_angularVelImpedPara[i].Now = States[1];
		m_angularVelImpedPara[i].Now = (m_thetaImpedPara[i].Now - m_thetaImpedPara[i].Last) / T;

		//////ʹ�û��˲���
		m_angularVelImpedPara[i].Now = JointVelFilter[i].GetVelStates(m_angularVelImpedPara[i].Next, m_angularVelImpedPara[i].Now);
	}

#ifdef DEBUG
	for (int i = 0; i < 4; i++)
	{
		TRACE("the %d��axis Kalmantheta is: %.3f\n", i, m_thetaImpedPara[i].Now);
		TRACE("the %d' axis KalmanangelVel is: %.3f\n", i, m_angularVelImpedPara[i].Now);
	}
#endif

	for (int i = 0; i < 4; i++)
	{
		if ( i==1 || i==2 ||i == 3)
		{
			m_angularVelImpedPara[i].Next = (Torque[i] - m_K[i]*m_thetaImpedPara[i].Now) / (m_K[i]*T + m_B[i]);
			//m_thetaImpedPara[i].Next = m_thetaImpedPara[i].Now + m_angularVelImpedPara[i].Next*T;
			m_thetaImpedPara[i].Next = m_B[i]*m_thetaImpedPara[i].Now / (m_K[i]*T + m_B[i]) + T*Torque[i] / (m_K[i]*T + m_B[i]);
		}
		
	}

#ifdef DEBUG
	for (int i = 0; i < 4; i++)
	{
		TRACE("the %d��axis next theta is: %.3f\n", i, this->m_thetaImpedPara[i].Next);
		TRACE("the %d��axis next angelVel is: %.3f\n", i, this->m_angularVelImpedPara[i].Next);
	}
#endif

	double GoalPos[4] = { 0, 0, 0, 0 }, GoalVel[4] = { 0, 0, 0, 0 };
	for (int i = 0; i < this->m_Robot->m_JointNumber; i++)
	{
		if (i==1||i==2||i == 3)
		{
			GoalVel[i] = this->m_angularVelImpedPara[i].Next;
	#ifdef DEBUG
			TRACE("the %d��GoalVel is: %.6lf\n", i, GoalVel[i]);
	#endif
			if (GoalVel[i] >= 0)   //��������˶�
			{
				GoalPos[i] = (this->m_thetaImpedPara[i].Next) + GoalVel[i] * GoalVel[i] / (2 * RealAcc[i]) + 0.4;   //������ֱ�Ӽ���һ�����Ķ�������ֹ����
				//GoalPos[i] = (this->m_thetaImpedPara[i].Next) ;   //������ֱ�Ӽ���һ�����Ķ�������ֹ����
	#ifdef DEBUG
				TRACE("the %d��extro theta is: %.6lf\n", i, GoalVel[i] * GoalVel[i] / (2 * RealAcc[i]));
	#endif
			}
			else   //��������˶�
			{
				GoalPos[i] = (this->m_thetaImpedPara[i].Next) - GoalVel[i] * GoalVel[i] / (2 * RealAcc[i]) - 0.4;   //������ֱ�Ӽ���һ�����Ķ�������ֹ����
				//GoalPos[i] = (this->m_thetaImpedPara[i].Next) ;   //������ֱ�Ӽ���һ�����Ķ�������ֹ����
				GoalVel[i] = -GoalVel[i];
			}
			//if (GoalVel[i] < 0) GoalVel[i] = -GoalVel[i];
		//	GoalVel[i] = fabs(GoalVel[i]);   //������ȡ����ֵ
			this->m_Robot->JointsTMove(GoalPos, GoalVel);
		}
		
	}

	return true;
}

//////////////////��������һ��ʱ�̵�λ�ò������Ǹ�λ�ã������ٶȵĹ滮
bool CImpedance::GetNextStateUsingJointSpaceImpendenceWithoutSpeedWithTProfile(void)
{
	
	double Torque[3] = { 0, 0, 0 };   //�����ǲ����ã����ÿ���ؽڵ����أ�ֻʹ��ǰ�����ؽڵĲ���


	//	Torque[0] = timenum / 100.0;
	//	Torque[1] = timenum / 100.0;
	Torque[2] = ForceSensor[2];

	for (int i = 0; i < 3; i++)  //ʹ������ֵ���ʽ
	{
		m_thetaImpedPara[i].Next = 1.0 / (m_B[i] / T + m_K[i])*Torque[i] + 1.0 / (m_B[i] / T + m_K[i])*(m_B[i] / T)*m_thetaImpedPara[i].Now;
	}
	//for (int i = 0; i < 3; i++)   //ֱ��ʹ��΢�ַ���
	//{
	//	m_thetaImpedPara[i].Next = 1.0 / (m_B[i] / T + m_K[i])*Torque[i] + 1.0 / (m_B[i] / T + m_K[i])*(m_B[i] / T)*m_thetaImpedPara[i].Now;
	//}
#ifdef DEBUG
	for (int i = 0; i < 4; i++)
	{
		TRACE("the %d��axis next theta is: %.3f\n", i, this->m_thetaImpedPara[i].Next);
	}
#endif
	double GoalPos[4];
	for (int i = 0; i < this->m_Robot->m_JointNumber; i++)
	{
		GoalPos[i] = (this->m_thetaImpedPara[i].Next);  
	}
	this->m_Robot->JointSynTMove(GoalPos,T);
	return true;
}

bool CImpedance::GetNextStateUsingJointSpaceImpendenceWithoutSpeedWithSProfile(void)
{
	double Torque[3] = { 10, 10, 10 };   //�����ǲ����ã����ÿ���ؽڵ����أ�ֻʹ��ǰ�����ؽڵĲ���
	for (int i = 0; i < 3; i++)
	{
		m_thetaImpedPara[i].Next = 1.0 / (m_B[i] / T + m_K[i])*Torque[i] + 1.0 / (m_B[i] / T + m_K[i])*(m_B[i] / T)*m_thetaImpedPara[i].Now;
	}
#ifdef DEBUG
	for (int i = 0; i < 4; i++)
	{
		TRACE("the %d��axis next theta is: %.3f\n", i, this->m_thetaImpedPara[i].Next);
	}
#endif
	double GoalPos[4];
	for (int i = 0; i < this->m_Robot->m_JointNumber; i++)
	{
		GoalPos[i] = (this->m_thetaImpedPara[i].Next);
	}
	return true;
}


CForceSensor* CImpedance::DeliverForceSensor(void)
{
	if (ATIForceSensor != NULL)
	{
		return ATIForceSensor;
	}
}