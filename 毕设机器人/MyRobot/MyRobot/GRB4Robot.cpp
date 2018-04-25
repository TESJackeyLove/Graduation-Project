#include "stdafx.h"
#include "GRB4Robot.h"

CGRB4Robot::CGRB4Robot()
{
	InitJoints();
//	UpdateJointArray();			//ˢ�µ�ǰ����λ��//@wqq ʦ�ܼӵ�
	m_OutportState = 0xffff;    //����˿�ȫ����1
	//��ʼ����������,��׼4���ɶȻ�е��GRB2014��һ�������ץ��ʹ��bit1
	for (int i = 0; i<16; i++)
	{
		m_toolArray[i].toolIndex = i;
		m_toolArray[i].IsOpen = false;
		strcpy_s(m_toolArray[i].toolName, "");
	}
	strcpy_s(m_toolArray[0].toolName, "��צ");  //ʹ��bits0 ,��һλ����ץ����
}


CGRB4Robot::~CGRB4Robot()
{
}

/*
	��ʼ�������ؽڵĲ���,��Ҫ�ǳ�ʼ��������ļ��ٱȣ�
*/
void CGRB4Robot::InitJoints(void)
{
	m_JointNumber = 4;  //�����︳ֵ�˻���Ĺؽڸ���
	m_JointArray = new t_Joint[m_JointNumber];  //�����ﴴ���˻���ĸ����ؽڵľ������

	int i = 0;
	for (i = 0; i<m_JointNumber; i++)
	{
		m_JointArray[i].JointNo = i + 1;  //  ���  �ֱ�Ϊ 1,2,3,4
		m_JointArray[i].LastJointPosition = 0.0;
		m_JointArray[i].CurrentJointPositon = 0.0;   //���õ�ǰ��λ����0
		m_JointArray[i].NormalJointAcc = 0.05;      //������ٶȿ���ֱ�Ӹ�ֵ���忨 GT_SetAcc  ********@wqq ���Լ���Ӧ�ÿ��Ը�д�����Լ���Ӧ�ô�һ���
		m_JointArray[i].LastJointPosition = 0.0;
		m_JointArray[i].LastJointVelocity = 0.0;
		m_JointArray[i].MaxJointVelocity = 15;     //��λ�� deg/s    
		m_JointArray[i].MaxJointAcceleration = 0.1;  //���嵫��û���õ�
	}

	ForwardKinematics();
	for (int i = 0; i<3; i++)
		for (int j = 0; j<4; j++)
			m_HandLastTn[i][j] =m_HandCurrTn[i][j] ;


	/*
	�������ڣ�ST=200us
	���ÿת��������p=2500(Pulse/r)
	SV���ƿ��ı�Ƶ������m=4

	3��˿���ݾࣺL=5(mm/r)
	1�᳤�ȣ�l1=250mm
	2�᳤�ȣ�l2=150mm

	1����ٱȣ�n1=80
	2����ٱ�: n2=80
    ����ļ��ٱ���24

	�û�λ�Ƶ�λΪ:deg �� mm
	�û��ٶȵ�λΪ��deg/s �� mm/s
	�û����ٶȵ�λΪ: deg/s^2 �� mm/s^2

	���ƿ�λ�Ƶ�λΪ:Pulse
	���ƿ��ٶȵ�λΪ: Pulse/ST
	���ƿ����ٶȵ�λΪ: Pulse/ST^2
	*/
	m_JointArray[0].AxisRatio = 80;   //��һ��ļ��ٱ���80��PulsePerMmOrDegree ��NormalJointVelocity
	m_JointArray[0].JointType = 1;     //��ת�ؽ�        
	m_JointArray[0].PulsePerMmOrDegree = 10000 * m_JointArray[0].AxisRatio / 360.;   //ÿתһ����Ҫ���ٸ�������2222.222 PULSE
//	m_JointArray[0].NormalJointVelocity = 2. / (m_JointArray[0].PulsePerMmOrDegree * 2 * 0.000001);  //450 �ٶ�Ҳ̫���˰� ��λ�� deg/s �̸�
	m_JointArray[0].NormalJointVelocity = 4;  //**************@wqq�Լ����ó�4 deg/s
	m_JointArray[0].NegativeJointLimit = -110.0;
	m_JointArray[0].PositiveJointLimit = 110.0;

	m_JointArray[1].AxisRatio = 80;    //��ת�ؽ� 
	m_JointArray[1].JointType = 1;
	m_JointArray[1].PulsePerMmOrDegree = 10000.* m_JointArray[1].AxisRatio / 360.; 
//	m_JointArray[1].NormalJointVelocity = 2. / (m_JointArray[1].PulsePerMmOrDegree * 2 * 0.000001);
	m_JointArray[1].NormalJointVelocity = 6;  //**************@wqq�Լ����ó�6 deg/s
	m_JointArray[1].NegativeJointLimit = -120.0;
	m_JointArray[1].PositiveJointLimit = 120.0;

	m_JointArray[2].AxisRatio = 1;   //�������ؽ����ƶ���������û�м�������ֱ������˿����ĸ
	m_JointArray[2].JointType = 0;   
	m_JointArray[2].PulsePerMmOrDegree = 10000 * m_JointArray[2].AxisRatio / 5.;  //ÿ�˶�1mm ��Ҫ���ٸ�������  2000 PLUSE  3��˿���ݾࣺL=5(mm/r)
//	m_JointArray[2].NormalJointVelocity = 5. / (m_JointArray[2].PulsePerMmOrDegree * 2 * 0.000001);  //500Pulse/ST=1250mm/s
	m_JointArray[2].NormalJointVelocity = 12;  //**************@wqq�Լ����ó�12 mm/s
	m_JointArray[2].NegativeJointLimit = -110;
	m_JointArray[2].PositiveJointLimit = 110.0;

	m_JointArray[3].AxisRatio = 24;     //���ĸ���ļ��ٱ���24
	m_JointArray[3].JointType = 1;      //���ĸ�����ת���� 
	m_JointArray[3].PulsePerMmOrDegree = 10000.* m_JointArray[3].AxisRatio / 360.;   //ÿתһ����Ҫ���ٸ�������666.667 PULSE
//	m_JointArray[3].NormalJointVelocity = 2. / (m_JointArray[3].PulsePerMmOrDegree * 2 * 0.000001); //1500 deg/s �̸ߵ�����Ҳ̫���˰�
	m_JointArray[3].NormalJointVelocity = 15;  //**************@wqq�Լ����ó�15 deg/s
	m_JointArray[3].NegativeJointLimit = -360.0;
	m_JointArray[3].PositiveJointLimit = 360.0;
}

/*
	����צ�Ĳ���
	���룺 toolIndex ���ڼ������ߣ�����צ��m_toolArray[0].toolIndex
		   toolOperation ��  1:����צ   0���ر���צ
*/
bool CGRB4Robot::ToolOperation(int toolIndex, int toolOperation)
{
	if (m_pController == NULL)
	{
		AfxMessageBox(_T("���ȴ򿪿�����!"), MB_OK);
		return false;
	}
	if (toolOperation == 0)//����1�պ�
	{
		m_OutportState = m_OutportState&CloseMask[toolIndex];  //CloseMask[0]=0xfffe=1111111111111110

		m_pController->DigitalOut(m_OutportState); //bit1 ��0
	}
	else   //���ߴ�
	{
		m_OutportState = m_OutportState | OpenMask[toolIndex]; //CloseMask[0]=0x0001=0000000000000001

		m_pController->DigitalOut(m_OutportState); //bit1 ��1

	}
	return true;
}

/*
	������Ķ���  ��ǰ3����ȥ�����޺͸�����λ��
*/
short CGRB4Robot::JointsTest(void)
{
	if (m_pController == NULL)
	{
		AfxMessageBox(_T("���ȴ򿪿�����!"), MB_OK);
		return -1;
	}
	for (int axis = 1; axis <= 3; axis++)
		for (int number = 1; number <= 2; number++)
		{
			m_pController->AxisCaptLimit(axis, 8888888, 3 * number);  //3 * number3 �����ֱ�Ӹ��忨���ٶ�
			m_pController->AxisCaptLimit(axis, -8888888, 3 * number);
		}
	return 1;
}

/*
	�ص����λ�á�  ���λ��ȫ�������ˡ�
*/
short CGRB4Robot::Home()
{
	if (m_pController == NULL){
		AfxMessageBox(_T("���ȴ򿪿�����!"), MB_OK);
		return -1;
	}
	//��ЩӲ������������Գ����İ�
	//��10Pulse/ST ���ٶ���ת33�ȣ�����˵��10~20Pulse/ST �ǱȽ��������ٶ�
	m_pController->AxisCaptHomeWithLimit(1, long(/*120*/(33.)*m_JointArray[0].PulsePerMmOrDegree), 10);
	m_pController->AxisCaptHomeWithLimit(2, long(/*185.*/(103.)*m_JointArray[1].PulsePerMmOrDegree), 15);
	m_pController->AxisCaptHomeWithLimit(3, long(100.*m_JointArray[2].PulsePerMmOrDegree), 20);
	m_pController->AxisCaptHomeWithoutLimit(4, 20);	//��צ��Ҫ�޸�������

	return 1;
}

/*
	SCARA�������˶�ѧ���� ������Ŀǰֻ�Ǽ�����ǰ�����ؽڵĲ�������û��ʹ�����һ����ת�ؽڵĽǶ�ֵ
	���룺	m_JointArray[0].CurrentJointPositon
			m_JointArray[1].CurrentJointPositon
			m_JointArray[2].CurrentJointPositon
	�����  m_HandCurrTn[][]
	��UpdateJointArray()�����е������������  ����û������̬�������
*/
bool CGRB4Robot::ForwardKinematics()
{
	double t0, t1, t2;

	t0 = m_JointArray[0].CurrentJointPositon * pi / 180.; //ת���ɻ���
	t1 = m_JointArray[1].CurrentJointPositon * pi / 180.; //ת���ɻ���
	t2 = m_JointArray[2].CurrentJointPositon;


	//������theta
	///	t3=m_JointArray[3].CurrentJointPositon*pi/180.;

	for (int i = 0; i<3; i++)
		for (int j = 0; j<4; j++)
			m_HandCurrTn[i][j] = 0;

	m_HandCurrTn[0][0] = cos(t0 + t1);
	m_HandCurrTn[1][1] = m_HandCurrTn[0][0];
	m_HandCurrTn[2][2] = -1;

	m_HandCurrTn[1][0] = sin(t0 + t1);
	m_HandCurrTn[0][1] = -m_HandCurrTn[1][0];

	m_HandCurrTn[0][3] = l2 * cos(t0 + t1) + l1 * cos(t0);////��Xλ��
	m_HandCurrTn[1][3] = l2 * sin(t0 + t1) + l1 * sin(t0);////��Yλ��
	m_HandCurrTn[2][3] = t2;


	////m_HandCurrTn[3][3]=t3;

	//x=m_HandCurrTn[0][3];  //��X��λ��
	//y=m_HandCurrTn[1][3];  //��Y��λ��
	//z=m_HandCurrTn[2][3];  //��z��λ��
	//theta=m_HandCurrTn[3][3];  //��theta��λ��

	return true;

}

/*
	SCRAR���������˶�ѧ:��ֱ������ϵ���˶�ת�����ؽ�����ռ�
	���룺	m_HandGoalTn[3][4]
	�����  m_JointGoal[4]
	ע����� ����false˵�������˶���Χ
	ֻʹ��ִ����ĩ��λ�ã�û��ʹ��ĩ����̬���������������ʱ����Ҫ��̬��
*/
bool CGRB4Robot::InverseKinematics(void)
{
	//
	double x, y, z, r; //a;
	double theta2;
	//��ʱʹ�õ�����צ����
	x = m_HandGoalTn[0][3];
	y = m_HandGoalTn[1][3];
	z = m_HandGoalTn[2][3];
	m_JointGoal[2] = z;   //��ΪZ��ֱ���ǽ���ģ�����ֱ�Ӹ�ֵ��m_JointGoal[2]�ĵ��

	///a=m_HandGoalTn[3][3];

	r = x * x + y * y;  //����뾶

	double cc2;

	cc2 = (l1*l1 + l2*l2 - r) / (2 * l1*l2);

	if (fabs(cc2)>1.0000001)
	{
		///AfxMessageBox(_T("���������ռ�"));
		return false;
	}

	//�����������
	if (cc2>0.99999999)
	{
		m_JointGoal[1] = 180;
		m_JointGoal[0] = atan2(y, x)*180. / pi;
		//	m_JointGoal[2]=z;
		//	m_JointGoal[3]=m_JointGoal[0]+m_JointGoal[1]
		//				-atan2(m_HandGoalTn[1][0],m_HandGoalTn[0][0])*180./pi;

		return true;
	}
	if (cc2<-0.9999999)
	{
		m_JointGoal[1] = 0;
		m_JointGoal[0] = atan2(y, x)*180. / pi;
		//	m_JointGoal[2]=z;
		//	m_JointGoal[3]=m_JointGoal[0]+m_JointGoal[1]
		//				  -atan2(m_HandGoalTn[1][0],m_HandGoalTn[0][0])*180./pi;

		return true;
	}

	//�����������,���ؽ������˶���Χ��
	//	-150<=theta1<=150
	//	-145<=theta2<=145
	//	|theta3|<=280;
	//  -180<=theta4<=180

	//����theta1-------------------------------------
	double sc11, cc11, sc2, theta11, theta12;
	sc2 = sqrt(1 - cc2*cc2);
	sc11 = l2*sc2 / sqrt(r);

	///cc11=(l2*l2-r-l1*l1)/(2*sqrt(r)*l1);
	cc11 = (r + l1*l1 - l2*l2) / (2 * sqrt(r)*l1);

	// 0< theta11 < pi/2
	theta11 = atan2(sc11, cc11);
	theta12 = atan2(y, x);

	//	theta1=theta11+theta12;
	theta2 = theta12 - theta11;

	//ȡֵ׼��Ϊ�ؽ��˶���С
	//	if(fabs(Qo[0]-theta1)>fabs(Qo[0]-theta2))
	m_JointGoal[0] = theta2*180. / pi;   //ת��Ϊ�Ƕ���Ϣ
	//	else
	//		InvResult[0]=theta1;

	//-----------------------------------------------------

	//��theta2-------------------------------------
	double sc1, cc1, theta1_2;

	sc1 = y - l1*sin(m_JointGoal[0] * pi / 180.);
	cc1 = x - l1*cos(m_JointGoal[0] * pi / 180.);
	theta1_2 = atan2(sc1, cc1);
	m_JointGoal[1] = (theta1_2)*180. / pi - m_JointGoal[0];
	//---------------------------------------------

	//��theta3 �� theta4
	//double theta4;
	//theta4=atan2(m_HandGoalTn[1][0],m_HandGoalTn[0][0]);
	//m_JointGoal[3]=-(m_JointGoal[0]+m_JointGoal[1]-theta4*180./pi);
	return true;
}

/*
	���������˶�ѧ
*/
bool CGRB4Robot::FullForwardKinematics(void)
{
	return true;
}

/*
	�����ĸ��˶�ѧ����
*/
bool CGRB4Robot::FullInverseKinematics(void)
{
	return true;
}