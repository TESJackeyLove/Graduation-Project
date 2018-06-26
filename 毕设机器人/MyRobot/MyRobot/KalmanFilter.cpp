#include "stdafx.h"
#include "KalmanFilter.h"



CKalmanFilter::CKalmanFilter()
{

}


CKalmanFilter::~CKalmanFilter()
{
}

void CKalmanFilter::Init_Kalman(double k, double b, double dt)
{
	//��ʼ���������Ĳ���
	m_dt = dt;
	m_k = k;
	m_b = b;
#if Order==1   ///////һ�׿������˲���

		A= b / (k * m_dt + b);
		H= 1;
		Q = 0.0075;
		R = 0.05;
		P = 0.1;

#elif Order==2   ///////���׿������˲���

	A[0][0] = b / (k * dt + b);
	A[0][1] = 0;
	A[1][0] = (-k) / (k * dt + b);
	A[1][1] = 0;


	H[0][0] = 1;
	H[0][1] = 0;
	H[1][0] = 0;
	H[1][1] = 1;

	Q[0][0] = 0;
	Q[0][1] = 0;
	Q[1][0] = 0;
	Q[0][1] = 0.5;

	R[0][0] = 0.1;
	R[0][1] = 0.5;
	R[1][0] = 0.5;
	R[1][1] = 1;

	P[0][0] = 0.1;
	P[1][0] = 0;
	P[0][1] = 0;
	P[1][1] = 0.5;


	/////////һ�׿�����
	A[0][0] = b / (k * dt + b);
	A[0][1] = 0;
	A[1][0] = (-k) / (k * dt + b);
	A[1][1] = 0;
#endif

}


double States[2];  //ȫ�ֱ���


void CKalmanFilter::GetKalmanStates(double ObserveTheta, double ObserveVel, double torque)
{
#if Order==1   ///////һ�׿������˲���
	////////////��һ��
	Bu = m_dt * torque / (m_k * m_dt + m_b);
	double StatesEstimate;
	StatesEstimate = A * States[0] + Bu;
	///////////�ڶ���
	double StatesP;
	StatesP = A * P * A + Q;
	/////////������
	K=StatesP*H/(H*StatesP*H+R);
	////////���Ĳ�
	States[0]=StatesEstimate+K*(ObserveTheta-H*StatesEstimate);
	////////���岽
	P=(1-K*H)*StatesP;
#elif Order==2

	//////////////////////////��һ��
	Bu[0] = m_dt * torque / (m_k * m_dt + m_b);
	Bu[1] = torque / (m_k * m_dt + m_b);
	double StatesEstimate[2];
	StatesEstimate[0] = A[0][0] * States[0] + A[0][1] * States[1] + Bu[0];
	StatesEstimate[1] = A[1][0] * States[0] + A[1][1] * States[1] + Bu[1];

	///////////////////////���ڶ���
	double StatesP[2][2];
	double Step1[2][2];
	Step1[0][0] = A[0][0] * P[0][0] + A[0][1] * P[1][0];
	Step1[0][1] = A[0][0] * P[0][1] + A[0][1] * P[1][1];
	Step1[1][0] = A[1][0] * P[0][0] + A[1][1] * P[1][0];
	Step1[1][1] = A[1][0] * P[0][1] + A[1][1] * P[1][1];

	double Step2[2][2];
	Step2[0][0] = A[0][0];
	Step2[0][1] = A[1][0];
	Step2[1][0] = A[0][1];
	Step2[1][1] = A[1][1];

	double Step3[2][2];
	Step3[0][0] = Step1[0][0] * Step2[0][0] + Step1[0][1] * Step2[1][0];
	Step3[0][1] = Step1[0][0] * Step2[0][1] + Step1[0][1] * Step2[1][1];
	Step3[1][0] = Step1[1][0] * Step2[0][0] + Step1[1][1] * Step2[1][0];
	Step3[1][1] = Step1[1][0] * Step2[0][1] + Step1[1][1] * Step2[1][1];

	StatesP[0][0] = Step3[0][0] + Q[0][0];
	StatesP[0][1] = Step3[0][1] + Q[0][1];
	StatesP[1][0] = Step3[1][0] + Q[1][0];
	StatesP[1][1] = Step3[1][1] + Q[1][1];

	////////////////////////������
	Step1[0][0] = H[0][0] * StatesP[0][0] + H[0][1] * StatesP[1][0];
	Step1[0][1] = H[0][0] * StatesP[0][1] + H[0][1] * StatesP[1][1];
	Step1[1][0] = H[1][0] * StatesP[0][0] + H[1][1] * StatesP[1][0];
	Step1[1][1] = H[1][0] * StatesP[0][1] + H[1][1] * StatesP[1][1];

	Step2[0][0] = H[0][0];
	Step2[0][1] = H[1][0];
	Step2[1][0] = H[0][1];
	Step2[1][1] = H[1][1];

	Step3[0][0] = Step1[0][0] * Step2[0][0] + Step1[0][1] * Step2[1][0];
	Step3[0][1] = Step1[0][0] * Step2[0][1] + Step1[0][1] * Step2[1][1];
	Step3[1][0] = Step1[1][0] * Step2[0][0] + Step1[1][1] * Step2[1][0];
	Step3[1][1] = Step1[1][0] * Step2[0][1] + Step1[1][1] * Step2[1][1];

	double Step4[2][2];
	Step4[0][0] = Step3[0][0] + R[0][0];
	Step4[0][1] = Step3[0][1] + R[0][1];
	Step4[1][0] = Step3[1][0] + R[1][0];
	Step4[1][1] = Step3[1][1] + R[1][1];

	double Inverse[2][2];
	double det;
	det = Step4[0][0] * Step4[1][1] - Step4[0][1] * Step4[1][0];
	if (det == 0)
	{
		AfxMessageBox(_T("���㿨�����˲����������ʱ�����������ʽΪ��!"), MB_OK);
		exit(1);
	}
	Inverse[0][0] = Step4[1][1] / det;
	Inverse[0][1] = -Step4[0][1] / det;
	Inverse[1][0] = -Step4[1][0] / det;
	Inverse[1][1] = Step4[0][0] / det;

	double Step5[2][2];
	Step5[0][0] = StatesP[0][0] * Step2[0][0] + StatesP[0][1] * Step2[1][0];
	Step5[0][1] = StatesP[0][0] * Step2[0][1] + StatesP[0][1] * Step2[1][1];
	Step5[1][0] = StatesP[1][0] * Step2[0][0] + StatesP[1][1] * Step2[1][0];
	Step5[1][1] = StatesP[1][0] * Step2[0][1] + StatesP[1][1] * Step2[1][1];

	K[0][0] = Step5[0][0] * Inverse[0][0] + Step5[0][1] * Inverse[1][0];
	K[0][1] = Step5[0][0] * Inverse[0][1] + Step5[0][1] * Inverse[1][1];
	K[1][0] = Step5[1][0] * Inverse[0][0] + Step5[1][1] * Inverse[1][0];
	K[1][1] = Step5[1][0] * Inverse[0][1] + Step5[1][1] * Inverse[1][1];

	/////////////////////���Ĳ�
	double New1[2];
	New1[0] = H[0][0] * StatesEstimate[0] + H[0][1] * StatesEstimate[1];
	New1[1] = H[1][0] * StatesEstimate[0] + H[1][1] * StatesEstimate[1];

	double New2[2];
	New2[0] = ObserveTheta - New1[0];
	New2[1] = ObserveVel - New1[1];

	double New3[2];
	New3[0] = K[0][0] * New2[0] + K[0][1] * New2[1];
	New3[1] = K[1][0] * New2[0] + K[1][1] * New2[1];

	States[0] = StatesEstimate[0] + New3[0];
	States[1] = StatesEstimate[1] + New3[1];

	//////////////////////���岽
	Step1[0][0] = K[0][0] * H[0][0] + K[0][1] * H[1][0];
	Step1[0][1] = K[0][0] * H[0][1] + K[0][1] * H[1][1];
	Step1[1][0] = K[1][0] * H[0][0] + K[1][1] * H[1][0];
	Step1[1][1] = K[1][0] * H[0][1] + K[1][1] * H[1][1];

	Step2[0][0] = 1;
	Step2[0][1] = 0;
	Step2[1][0] = 0;
	Step2[1][1] = 1;

	Step3[0][0] = Step2[0][0] - Step1[0][0];
	Step3[0][1] = Step2[0][1] - Step1[0][1];
	Step3[1][0] = Step2[1][0] - Step1[1][0];
	Step3[1][1] = Step2[1][1] - Step1[1][1];

	P[0][0] = Step3[0][0] * StatesP[0][0] + Step3[0][1] * StatesP[1][0];
	P[0][1] = Step3[0][0] * StatesP[0][1] + Step3[0][1] * StatesP[1][1];
	P[1][0] = Step3[1][0] * StatesP[0][0] + Step3[1][1] * StatesP[1][0];
	P[1][1] = Step3[1][0] * StatesP[0][1] + Step3[1][1] * StatesP[1][1];

#endif

}