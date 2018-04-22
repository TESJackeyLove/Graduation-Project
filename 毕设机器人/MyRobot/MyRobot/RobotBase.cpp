#include "stdafx.h"
#include "RobotBase.h"
#include <math.h>

CRobotBase::CRobotBase()
{
	m_pController = NULL;
	m_pPlanner = NULL;
	m_JointArray = NULL;  //�ؽ����飬��Źؽڲ���  ����SCARA��������˵һ�����ĸ��ؽ�
	OverallVelocityRate = 1;//����
	m_NormalCartesianVelocity = 100;   //�̸߹��������õ���20000,�о����� *********@wqq ��дΪ100mm/s  ����������
	m_MacroSamplePeriod = 0.01;  //����������0.01s,�滮���������������������ƶ�ʱ����0.01s
	//	UpdateJointArray();		// �����ʦ�ܱ�д��ˢ�º���

}


CRobotBase::~CRobotBase()
{
	if (m_pController != NULL)  //ɾ��������
		delete m_pController;
	if (m_pPlanner != NULL)    //ɾ���滮��
		delete m_pPlanner;
	if (m_JointArray != NULL)   //ɾ���ؽ�����
		delete[]m_JointArray;
}

/*
	�����˻������ӿ�����
	*/
void CRobotBase::AttachController(CGTController *pController)
{
	if (m_pController != NULL)
	{
		delete m_pController;
	}
	m_pController = pController;  //ע������ֱ��ʹ�õ��Ǹ�ֵ��䣬m_pControllerֱ��ָ��pController������ڴ�ռ�
//	UpdateJointArray();			//@wqqʦ��������ӵ�

}

/*
	�����˻��� ���ӹ滮��
	*/
void CRobotBase::AttachPlanner(CPlanner *pPlanner)
{
	if (m_pPlanner != NULL)
		delete m_pPlanner;

	m_pPlanner = pPlanner;
}

/*
	�����˻���Ͽ�������
	*/
void CRobotBase::DetachController(void)
{
	if(m_pController != NULL)
	{
		delete m_pController;
	}
	m_pController = NULL;
}

/*
	�����ؽڰ���ָ����Ŀ��λ�ú��ٶ��ƶ�
	���룺	jointNo��  
			goalPos��  �������ת�ؽھ���deg,�����ֱ�߹ؽھ���mm  ,������ʹ�õ��˹ؼ�����PulsePerMmOrDegree
			vel  ��   �ٶȣ���deg/s)
	����� -1��ʧ��
		    0���ɹ�
*/
short CRobotBase::JointDrive(short jointNo, double goalPos, double vel)    //###################ע�����
{
	unsigned short flag = 0;

	// ��ǰλ��С����С��λλ�û���������λλ��ʱ�������������˶���Χ��
	if (goalPos<m_JointArray[jointNo - 1].NegativeJointLimit || goalPos>m_JointArray[jointNo - 1].PositiveJointLimit)
		return -1;

	long pos;
	double vel1, acc;

	//���ؽ�ֵת��Ϊ����ֵ
	pos = (long)(goalPos * m_JointArray[jointNo - 1].PulsePerMmOrDegree);  //λ������=����Ŀ��ؽ�ֵת��Ϊ������
	//���ٶ�תΪ�忨���ܵ��ٶ�,vel�ǽǶ�ÿ�룬������ÿ����   Ĭ�ϳ������������200us,deg/s = 
	vel1 = vel * m_JointArray[jointNo - 1].PulsePerMmOrDegree * 2 * 0.0001;  //******@wqq�Ҹо��̸�д���˹̸���0.000001
	//���ٶ�ֱ�Ӵ���ȥ����λһֱ��Pulse/ST^2
	acc = m_JointArray[jointNo - 1].NormalJointAcc;
	if (m_pController->AxisMoveToWithTProfile(jointNo, pos, vel1, acc) != 0)  //���������˶�ģʽ
		return -1;
	//	UpdateJointArray();			//@wqqʦ��������ӵ�
	return 0;
}

/*
	�ؽڵĵ����˶�������ģʽ
	���룺	axisNo��  �ؽں�
			step :  ����  ��������deg������mm)
			velRatio��  �ٶȱ���   
*/
short CRobotBase::JointJog(short axisNo, double step, double velRatio)       //###################ע�����
{
	double pos1, vel;

	pos1 = m_JointArray[axisNo - 1].CurrentJointPositon + step;
	//��ǰλ�ã����ȡ����ߡ����ס����Ӳ��������ȡ����ߡ����ס�����
	//��ת�ؽڵ�λΪ���ȡ����ƶ��ؽڵ�λΪ�����ס���

	vel = OverallVelocityRate * m_JointArray[axisNo - 1].NormalJointVelocity * velRatio;
	//m_JointArray[axisNo-1].NormalJointVelocity:�忨�������ٶȡ�����λΪ����/�롱�򡰺���/�롱��
	//m_JointArray[axisNo-1].AxisRatio������ٱȣ�
	//velRatio����ǰ�ᡰ�����ٶȡ��ı��ʣ����˶��ٶȱ��ʣ������Ǿ�����ֵ���������ٶȡ���ֵ�ڰ忨��ʼ��ʱ�趨��û���������һ�㲻�ڸı䣬
	//          �û�ֻ��ı��ٶȱ��ʵ����ٶȵĴ�С���ٶȱ���Ĭ��ֵΪ��1����
	//OverallVelocityRate����������������˶��ٶȱ�ֵ��
	return JointDrive(axisNo, pos1, vel);
}

/*
	ֱ������ϵ�����˶���ע���������ֻ�����ŵ���������ķ����ƶ������еĵ��������漰�������˶�����OverallVelocityRate
	���룺CartesianAxis 1-->x 2-->y  3-->z
		  step  ����������
*/
short CRobotBase::CartesianJog(short CartesianAxis, double step)
{
	//�ڵѿ����ռ�ʾ��ʱ��ֻ�ܲ�����X,Y,Z�����˶�
	if (CartesianAxis>3 || CartesianAxis<1) return -1;
	double handGoal[3][4];   //����λ�˾���
	for (int i = 0; i<3; i++)
		for (int j = 0; j<4; j++)
			handGoal[i][j] = m_HandCurrTn[i][j];
	handGoal[CartesianAxis - 1][3] += step;   //��P�����Ĳ���
	double vel;
	vel = m_NormalCartesianVelocity * OverallVelocityRate;
	return SMoveWithLProfile(handGoal, vel, 1);    //����ʹ���ٶȹ滮
}

/*
	��ؽ�������ģʽͬʱ�˶�   PulsePerMmOrDegree�ͼ������ļ��ٱ��й�
	���룺
			goalPos: Ŀ��λ������ ��λ�� deg
			vel    : �˶��ٶ����� ��λ�� deg/s
*/
short CRobotBase::JointsTMove(double goalPos[], double goalVel[])
{
	double acc[4];
	long pos[4];
	double vel[4];

	int i;

	for (i = 0; i<m_JointNumber; i++)
	{
		pos[i] = (long)(goalPos[i] * m_JointArray[i].PulsePerMmOrDegree);
		vel[i] = goalVel[i] * m_JointArray[i].PulsePerMmOrDegree * 2 * 0.0001; //ת��Ϊ�忨ʶ����ٶȣ�PLUSE/ST  ******@wqq �Ҿ��ù̸�д���� �̸�д����0.000001
		acc[i] = m_JointArray[i].NormalJointAcc;
	}
	m_pController->MoveToWithTProfile(pos, vel, acc);
	//	UpdateJointArray();   // @wqq  �����ʦ�ܱ�д��ˢ�º���
	return 0;

}

/*
	��ؽ�ͬʱ�˶�������ģʽ��ͬʱ����
	  ���룺 goalPos  Ŀ������λ��
	         moveTime  �˶�ʱ��  s
*/
short CRobotBase::JointSynTMove(double goalPos[], double moveTime)
{
	//���Ŀ��ؽ�ֵ�Ƿ��ڻ����˹����ռ���
	//......
	if (moveTime<0.001) return -1;
	double vel[6];
	int i;
	for (i = 0; i<m_JointNumber; i++)
	{
		///???  //������ٶȿ����Ǹ��ģ�������
		vel[i] = fabs((goalPos[i] - m_JointArray[i].CurrentJointPositon)) / moveTime;
		///???
	}
	return JointsTMove(goalPos, vel);
}

/*
	�ռ������Ķ���ͬ��ֱ���˶�������ģʽ
	���룺 goalTn[][] :Ŀ��λ�˾���
	       VelOrTime  ������ʱ������ٶȹ滮�Ĺ滮����
		   flag     1���ٶ�  ;  0: ʱ��
*/
short CRobotBase::SMoveWithLProfile(double goalTn[3][4], double VelOrTime, short flag)
{
	short rtn;
	if (m_pController == NULL)
	{
		AfxMessageBox(_T("���������ӿ�����!"), MB_OK);
		return -1; //error
	}

	if (m_pPlanner == NULL)
	{
		AfxMessageBox(_T("���������ӹ滮��!"), MB_OK);
		return -1; //error
	}

	///��������
	for (int i = 0; i<3; i++)
		for (int j = 0; j<4; j++)
			m_HandGoalTn[i][j] = goalTn[i][j];//���ĩ��Ŀ��λ��

	if (InverseKinematics() == false) 			//������������ж��Ƿ����
	{
		AfxMessageBox(_T("�˶������˹����ռ�!"), MB_OK);
		return -1;
	}

	//���ù滮������
	m_pPlanner->SetPlanningFlag(flag);
	if (flag)
		m_pPlanner->SetDesiredVelocity(VelOrTime);//���ٶȹ滮
	else
		m_pPlanner->SetDesiredTime(VelOrTime);	//��ʱ��滮

	//�����6���ؽڣ�����ֻ��Ҫ�õ�4��
	double vel[6];
	double preJointGoal[6];   //ǰһ���ؽ�Ŀ��

	//����ֱ�߹滮����û��ʵ�����У���������������Ѿ��õ��滮���б���ɼ�������
	rtn = m_pPlanner->SMovePlaningWithLProfile(m_HandCurrTn, m_HandGoalTn);

	if (rtn == 0)
	{
		int k;
		for (k = 0; k <= m_JointNumber - 1; k++)
			preJointGoal[k] = m_JointArray[k].CurrentJointPositon;//ȡ��ǰλ��ÿ���ؽڵ���ʼλ��

		//��ȡ·����
		while (m_pPlanner->GetNextViaTn(m_HandGoalTn)) //�ѹ滮���еĹ滮���б����һ��Ԫ�ظ�ֵ��m_HandGoalTn
		{
			if (InverseKinematics())
			{ //�˶�ѧ���⽫m_HandGoalTnת��Ϊm_JointGoal

				//���µĴ���ʵ�ֹؽ�ͬ���˶�����Ŀ��㣬�ؼ�����
				for (k = 0; k<6; k++)
					vel[k] = 0;
				for (k = 0; k <= m_JointNumber - 1; k++)
				{
					vel[k] = (m_JointGoal[k] - preJointGoal[k]) / m_MacroSamplePeriod; //���ʹ�ò�ͬ������ٶȲ�һ�������ǵ���ʱ����һ���ģ�����m_MacroSamplePeriod=0.01s
					if (vel[k]<0)vel[k] = -vel[k];   /////////////////*************@wqq  �忨���˶�����ֻ��POS�йأ��ٶȶ�������
				}
				//ֻ��x,y,z��ֻ��ǰ����
				m_JointGoal[3] = 0; vel[3] = 0;

				JointsTMove(m_JointGoal, vel);//�������ݸ� JointsTMove

				for (k = 0; k <= m_JointNumber - 1; k++)
					preJointGoal[k] = m_JointGoal[k];
			}
		}

	}
//	UpdateJointArray();		// @wqq�����ʦ�ܱ�д��ˢ�º���
	return 0;
}

/*
	����������麯�� ��������ʵ�ִ򿪼о߲������麯���У�
*/
bool  CRobotBase::ExecuteToolOperation(int toolIndex, int toolOperation)
{
	return ToolOperation(toolIndex, toolOperation);
}

/*
	��ʱ����
*/
void  CRobotBase::ExecuteDelayOperation(int ms)
{
	Sleep(ms);
}

/********************************************************/
/*		��������UpdateJointArray                        */
/*		  ���ܣ�ˢ�¹ؽ�����                            */
/*		  ���룺                                        */
/*		  �����CurrentJointPositon,                    */
/*				JointStatus,                            */
/*				m_HandCurrTn,                           */
/*	  ע�����λ�ø���һ�ε���һ��                    */
/********************************************************/
void CRobotBase::UpdateJointArray()
{
	long pos[4];					//������
	double vel[4];
	unsigned short status[4];

	for (int i = 0; i < m_JointNumber; i++)   //����һ�ε�λ�ø�ֵ��һ�ε�λ��(�ؽڿռ�)
	{
		m_JointArray[i].LastJointPosition = m_JointArray[i].CurrentJointPositon;
		m_JointArray[i].LastJointVelocity = m_JointArray[i].CurrentJointVelocity;
	}
	for (int i = 0; i<3; i++)         //����һ�ε�λ�ø�ֵ��һ�ε�λ��(ֱ������ռ䣩 
		for (int j = 0; j<4; j++)
			m_HandLastTn[i][j] = m_HandCurrTn[i][j];


	if (m_pController != NULL&&m_pController->m_ServoIsOn)
	{
		//m_pController->UpdateAxisArray();
		m_pController->GetAxisPositionAndVelocityAndState(pos,vel,status);
		for (int i = 0; i<m_JointNumber; i++)
		{
			m_JointArray[i].CurrentJointPositon = (double)pos[i] / m_JointArray[i].PulsePerMmOrDegree;
			m_JointArray[i].JointStatus = status[i];
			m_JointArray[i].CurrentJointVelocity = (double)vel[i] / (m_JointArray[i].PulsePerMmOrDegree * 0.0002);
		}
		//ĩ��λ���˶�ѧ��������m_HandCurrTn[3][4]; 
		ForwardKinematics();
	}
//	Sleep(5);
}