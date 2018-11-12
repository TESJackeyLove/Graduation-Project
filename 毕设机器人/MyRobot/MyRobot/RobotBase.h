#pragma once
#include "afx.h"
#include "GTController.h"
#include "Planner.h"
/*
ע�����������ǳ����࣬����ֱ�����ɶ��� �麯��Ϊ���˶�ѧ�����˶�ѧ����ʼ���ؽڣ��ؽڲ��ԣ��ص�Home���λ��
*/
struct t_Joint
{
	int 	 JointNo;				//�ؽڱ��
	int 	 JointType; 		    //0���ƶ��ؽڣ�1����ת�ؽ�
	double	 CurrentJointPositon; 	//�ؽڵ�ǰλ��
	double   CurrentJointWithoutGapPosition;
	double   CurrentJointVelocity;
	double	 LastJointPosition;  	//�ؽ���һ��λ��
	double   LastJointVelocity;    //�ؽ���һ�ε��ٶ�

	double	 NormalJointVelocity;	//�����ؽ��ٶ�
	double	 NormalJointAcc;		//�����ؽڼ��ٶ�
	double	 AxisRatio; 			//��ļ��ٱȣ�����õļ������ļ��ٱ�
	double	 PulsePerMmOrDegree;    //JointType==0,����/���ף�JointType==1,����/��

	double	 PositiveJointLimit;	//�ؽ�������λ�ã��Ȼ����
	double	 NegativeJointLimit;	//�ؽڸ�����λ��

	double	 MaxJointVelocity;		//MaxJointVelocity=5*NormalJointVelocity
	double	 MaxJointAcceleration;  //

	unsigned  short  JointStatus;	//�ؽ�״̬��0����ֹ��1���˶�
};
struct t_JointGap
{
	double GapLength;
	double GapToPositive;
	double GapToNegative;
};
//���߽ṹ��
struct t_Tools{
	unsigned int toolIndex;			//����
	bool IsOpen;			 		//״̬{0��1}={Off,On}
	char toolName[20];				//���ߵ�����
};
class CRobotBase :
	public CObject
{
	double   OverallVelocityRate;	//�����ٶȱ���
	HANDLE   m_ThreadHandle;   //The Update thread
public:
	CGTController *m_pController;	//����������
	CPlanner *m_pPlanner;			//�滮������
	double	m_MacroSamplePeriod;	//��������  Macro΢���

	t_Tools 	 m_toolArray[16];   //�ɿ���16·�����ʵ��ֻ��һ�������צ�������Ժ�����

	double   m_HandLastTn[3][4];    //��һ�ιؽڵ�λ�ã�
	double	 m_HandCurrTn[3][4];  	//��ǰĩ�˵�ǰλ��,��ʱ����  ///�˶�ѧ�������������
	double   m_JacobiTn[6][4];    //�ſɱȾ���
	double	 m_HandGoalTn[3][4];	//ĩ��Ŀ��λ�ˣ�		 ///��������
	double	 m_JointGoal[4];	    //�ؽ�λ�ã���ʱ���� 	///�˶�ѧ�������������

	double	 m_NormalCartesianVelocity;//ֱ������ռ������˶��ٶ�

	t_Joint *m_JointArray;	  		//�ؽ����飬��Źؽڲ���  ����SCARA��������˵һ�����ĸ��ؽ�
	int 	 m_JointNumber; 		//�ؽڸ���
	t_JointGap *m_JointGap;        //����ؽڵļ�϶
	bool m_isGapCorrespond;     //true���Ӧ�����򲻶�Ӧ
	bool m_isOnGap;     //�ж��ǲ������迹���Ƶ�ʱ��  
	
public:
	CRobotBase();
	virtual ~CRobotBase();
public:
	void AttachController(CGTController *pController);  
	void DetachController(void);
	void AttachPlanner(CPlanner *pPlanner);

	void SetOverallVelocityRatio(double ratio){ OverallVelocityRate = ratio; }

	///You must impletement these 6 virtual function acrrording to a special Robot
	virtual bool ForwardKinematics(void) = 0;
	virtual bool FullForwardKinematics(void) = 0;  //�Լ�������������˶�ѧ�������漰�������ɶȵĽ���
	virtual bool InverseKinematics(void) = 0;
	virtual bool FullInverseKinematics(void) = 0;
	virtual void InitJoints(void) = 0;
	virtual short JointsTest(void) = 0;
	virtual short Home(void) = 0; //First position	
	virtual bool ToolOperation(int toolIndex, int toolOperation) = 0;//���߲���ʵ�ֺ���������ض��Ļ�����ʵ��,һ�㲻ֱ�ӵ��ã�����ExecuteToolOperation()
	virtual bool CalculateJacobiMatrix() = 0;
	/// 

	short JointsTMove(double goalPos[], double vel[]);
	short JointDrive(short jointNo, double goalPos, double vel,double step);
	short JointJog(short axisNo, double step, double velRatio);
	short CartesianJog(short CartesianAxis, double step);
	short JointSynTMove(double goalPos[], double moveTime = 1.);
	short SMoveWithLProfile(double goal[3][4], double VelOrTime, short flag);

	//
	bool  ExecuteToolOperation(int toolIndex, int toolOperation);
	void  ExecuteDelayOperation(int ms);
	void  UpdateJointArray();
	 
	//
	short JointJogGapDeal(short axisNo, double& goalPos, const double& step);


};

