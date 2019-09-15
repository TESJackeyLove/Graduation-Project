#pragma once
#include "afx.h"
#include "GRB4Robot.h"

struct ImpedancePara
{
	double Last;
	double Now;
	double Next;
};

class CImpedance : public CObject
{
public:
	HANDLE m_hControlThread;
	CRobotBase *m_Robot;    //�����˶���
	double m_M, m_K, m_B;
	ImpedancePara m_FImpedPara, m_xImpedPara[3], m_vImpedPara[3];   //�ýṹ�����洢�������迹����ʱ����Ҫ�õ��Ĳ��������������ѿ�������ռ��λ�ú�����
	ImpedancePara m_thetaImpedPara[4];  //�����鴢�����ĸ��ؽڿռ�ĽǶ�
	ImpedancePara m_angularVelImpedPara[4];   //�����鴢�����ĸ��ؽڵĽ��ٶ�
	

public:
	CImpedance(CRobotBase *Robot);
	virtual ~CImpedance();
public:
	bool StartImpedanceController();  //��ʼ�迹���������������˿����ഫ�ݸ��迹������
    bool GetCurrentState(void);
	bool GetNextStateUsingJointSpaceImpendence(void);
	bool GetNextStateUsingJointSpaceImpendenceWithSpeedWithTProfile(void);
	bool GetNextStateUsingJointSpaceImpendenceWithoutSpeedWithTProfile(void);
	bool GetNextStateUsingJointSpaceImpendenceWithoutSpeedWithSProfile(void);

};

