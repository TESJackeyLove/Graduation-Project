#include "stdafx.h"
#include "VelFliter.h"


CVelFliter::CVelFliter()
{
	a1 = 0.9;
	a2 = 0.1;
}


CVelFliter::~CVelFliter()
{
}


double CVelFliter::GetVelStates(double predict, double differential) //��������������Ԥ�����΢����
{
	double Vel;
	Vel = a1 * predict + a2 * differential;
	return Vel;
}