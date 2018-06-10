#pragma once
#include "afx.h"


class CKalmanFilter : public CObject
{
	double m_dt;
	double m_k;
	double m_b;

	double Q[2][2]; //ϵͳ�Ĺ���������Э�������
	double R[2][2]; // ϵͳ�Ĺ۲�������Э�������
	double A[2][2];   //ϵͳ��״̬����
	double Bu[2];
	double H[2][2];

	double P[2][2]; // ���Э�������- This is a 2x2 matrix������
	double K[2][2]; //���������� - This is a 2x1 vector��������

public:
	CKalmanFilter();
	virtual ~CKalmanFilter();

	void Init_Kalman(double k, double b, double dt);

	void GetKalmanStates(double ObserveTheta, double ObserveVel, double torque);

private:
	
};
