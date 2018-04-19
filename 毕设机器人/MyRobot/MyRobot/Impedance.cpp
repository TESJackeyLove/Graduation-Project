#include "stdafx.h"
#include "Impedance.h"

///////////////////////////////////////////////////////////
HANDLE hSyncEvent;//ͬ���¼����
bool stopflag; //�߳̽�����־
DWORD WINAPI ThreadProc(LPVOID lpParam)
{
	CImpedance *pImpedence = (CImpedance *)lpParam;  //��ȡ��ָ��
	ResetEvent(hSyncEvent);  //ȷ���¼��������ź�״̬
	while (1)
	{
		WaitForSingleObject(hSyncEvent, INFINITE);
		if (stopflag)
		{
			break;
		}
		///////////����������Ҫ����Ĵ���
		pImpedence->GetCurrentState();//��ȡ��ǰ��ʱ�̵Ĺؽڿռ���ٶȣ�λ�ú�ֱ������ռ��λ�ã��ٶ�
		pImpedence->GetNextStateUsingJointSpaceImpendence();  //������һ��ʱ�̵ĹؽڵĽǶȺͽ��ٶ�
		double GoalPos[4],GoalVel[4];
		for (int i = 0; i < pImpedence->m_Robot->m_JointNumber; i++)
		{
			GoalPos[i] = pImpedence->m_thetaImpedPara[i].Next+2;   //������ֱ�Ӽ���һ�����Ķ�������ֹ����
			GoalVel[i] = pImpedence->m_angularVelImpedPara[i].Next;
		}
		pImpedence->m_Robot->JointsTMove(GoalPos, GoalVel);

		////////////����������
		ResetEvent(hSyncEvent);//��λͬ���¼�
	}
	GT_SetIntSyncEvent(NULL);//֪ͨ�豸ISR �ͷ��¼�
	CloseHandle(hSyncEvent); //�ر�ͬ���¼����
	ExitThread(0);
	return 0;
}

CImpedance::CImpedance(CRobotBase *Robot)
{
	m_M = 0;
	m_K = 0.5;   //��λ�� N/mm
	m_B = 0.01;
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
	m_Robot = Robot;   //���ӻ����˶���

}


CImpedance::~CImpedance()
{

}


//���Ǿ��ðѻ����˴��ݸ��迹�������ȽϺã���Ϊ�迹������һ�ֱȻ���������ϲ�Ŀ��ƣ����Ի����˲�һ�����迹���ƣ������迹����һ���л�����
bool CImpedance::StartImpedanceController()
{

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

	//�������������ã��������õ�ʱ����Ҫֱ�Ӳɼ�������Ϣ
	m_FImpedPara.Last = 10;   //����1N  ����λ��N
	m_FImpedPara.Now = 10;   
	m_FImpedPara.Next = 10;

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

	GT_SetIntrTm(250);  //���ö�ʱ���Ķ�ʱ����Ϊ250*200us = 50ms
	GT_TmrIntr();   //���������붨ʱ�ж�
	GT_GetIntr(&Status);
	if (&Status != 0)
	{
		AfxMessageBox(_T("��ʱ���ж����ó���!"), MB_OK);
		exit(1);
	}
	GT_SetIntSyncEvent(hSyncEvent);//ΪPCI���ƿ������ж�ͬ���¼������ò���ΪNULLʱ���ú�����λ��ǰ������ֵ
	stopflag = false;
	m_hControlThread = CreateThread(NULL, 0, ThreadProc, (LPVOID)(this), 0, NULL);  //���춨ʱ����������������ú���,���迹���ƶ����ָ�븳��Ϊ���̺߳����Ĳ���
	SetThreadPriority(m_hControlThread, THREAD_PRIORITY_HIGHEST);
	if (m_hControlThread == NULL)
	{
		AfxMessageBox(_T("�����߳�ʧ��!"), MB_OK);
		return false;
	}


	return true;
}

bool CImpedance::GetCurrentState(void)
{
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

	//�������������ã��������õ�ʱ����Ҫֱ�Ӳɼ�������Ϣ
	m_FImpedPara.Last = 10;   //����1N  ����λ��N
	m_FImpedPara.Now = 10;
	m_FImpedPara.Next = 10;

	/////////////////ֱ������ϵ����Ϣ
	m_xImpedPara[0].Last = m_Robot->m_HandCurrTn[0][3];   //��X���ߵ�λ��
	m_xImpedPara[1].Last = m_Robot->m_HandCurrTn[1][3];   //��Y���ߵ�λ��
	m_xImpedPara[2].Last = m_Robot->m_HandCurrTn[2][3];   //��z���ߵ�λ��


	//�˴�Ӧ�û���ֱ������ռ���ٶȣ���Ҫ�õ��ſ˱Ⱦ��󣬵��ǻ�����������ӣ����������ﻹû�ж���

	return true;
}

bool CImpedance::GetNextStateUsingJointSpaceImpendence(void)
{
	double Torque[3] = { 10, 10, 0 };   //�����ǲ����ã����ÿ���ؽڵ����أ�ֻʹ��ǰ�����ؽڵĲ���
	for (int i = 0; i < 3; i++)
	{
		m_angularVelImpedPara[i].Next = (Torque[i] - m_K*m_thetaImpedPara[i].Now) / (m_K*0.05 + m_B);
	}
	return true;
	
}