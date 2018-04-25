#pragma once
#include "afx.h"

#include "GT400.h"
#define LIMIT_SENSE 255  //������λ���ص͵�ƽ����

#define PAxisReduce 1.0   //@by wqq  ��������˶����Ƶ�ʱ��ı��������֡�΢������
//��ṹ��
struct t_Axis{
	unsigned short AxisNo;//���
	long   CurrentPosition; //��ǰ��λ�ã�����
	double   CurrentVelocity;//*******************************************@wqq  Firmware �汾ΪVer2.50 �������ϵ�֧��
	unsigned short CurrentState; //��ǰ���״̬
	/*
	long   GoalPosition;    //����Ŀ��λ�ã�����
	double GoalVelocity;    //�����ٶ� ������/�ŷ�����
	double GoalAcc; //������ٶȣ�����/�ŷ�����^2
	double GoalMaxAcc;//���������ٶȣ�����/�ŷ�����^2 (0~0.5)
	double GoalJerk;  //����Ӽ��ٶȣ�����/�ŷ�����^3   (0~0.5)
	*/
};

class CGTController :
	public CObject
{
public :
	t_Axis *m_AxisArray;  //������
	int    m_AxisNumber;  //��ĸ�������SCARA��˵��һ�����ĸ���
	bool  m_ServoIsOn;  ////�ŷ�������־ 
public:
	CGTController();
	virtual ~CGTController();
	short ServoOn(void);
	short ServoOff(void);
	short UpdateAxisArray(void);
	short InitCard(void); //��ʼ������
	short EnableController(void);
	short GetAxisPositionAndVelocityAndState(long pos[4], double vel[4], unsigned short state[4]);
	short AxisCaptHomeWithHome(int axisno, long maxNegativeOffset, long maxPositiveOffset, double vel);
	short AxisCaptHomeWithLimit(int axisno, long offset, double vel);
	short AxisCaptLimit(int axisno, long offset, double vel);
	short AxisCaptHomeWithoutLimit(int axisno, double vel);
	short AxisMoveToWithTProfile(int axisno, long pos, double vel, double acc);
	bool  StartUsingSProfile();
	short MoveToWithSProfile(long pos[4]);
	short CGTController::AxisMoveToWithSProfile(int axisno, long pos);
	short MoveToWithTProfile(long pos[4], double vel[4], double acc[4]);
	short DigitalOut(unsigned int dout);
	void  wait_motion_finished(int AxisNo);
};

