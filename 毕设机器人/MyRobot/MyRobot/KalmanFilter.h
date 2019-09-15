#pragma once
#include "afx.h"

////���忨�����˲����Ľ���
#define Order 1

class CKalmanFilter : public CObject
{
	double m_dt;
	double m_k;
	double m_b;
	//int m_kalmanmode;    //�����0��λ�ÿ������������1�����ٶȿ�����;����Ƕ��׿�������������Ч
 //////һ�׿������˲���    
#if Order==2  
	double Q[2][2]; //ϵͳ�Ĺ���������Э�������
	double R[2][2]; // ϵͳ�Ĺ۲�������Э�������
	double A[2][2];   //ϵͳ��״̬����
	double Bu[2];
	double H[2][2];

	double P[2][2]; // ���Э�������- This is a 2x2 matrix������
	double K[2][2]; //���������� - This is a 2x1 vector��������

	
	///////���׿������˲���
#elif Order==1   

	double Q; //ϵͳ�Ĺ���������Э�������
	double R; // ϵͳ�Ĺ۲�������Э�������
	double A;   //ϵͳ��״̬����
	double Bu;
	double H;
	double P; // ���Э�������- This is a 2x2 matrix������
	double K; //���������� - This is a 2x1 vector��������

#endif



public:
	CKalmanFilter();
	virtual ~CKalmanFilter();

	void Init_Kalman(double k, double b, double dt);

	void GetKalmanStates(double ObserveTheta, double ObserveVel, double torque,int i);

private:
	
};
