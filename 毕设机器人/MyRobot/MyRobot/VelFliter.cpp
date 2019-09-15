#include "stdafx.h"
#include "VelFliter.h"


CVelFliter::CVelFliter()
{
	a1 = 0.95;
	a2 = 0.05;
}


CVelFliter::~CVelFliter()
{
}


double CVelFliter::GetVelStates(double predict, double differential) //��������������Ԥ�����΢����
{
	double Vel;
	Vel = a1 * predict + a2 * differential;   //ѡ���ڶ���ʱ�������Ԥ���ֵ������ʱ���������΢��ֵ
	return Vel;
}