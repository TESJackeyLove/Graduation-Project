#pragma once
#include "afx.h"
class CVelFliter :
	public CObject
{
public:
	double a1;  //�������ϵ��1
	double a2;  //�������ϵ��2
public:
	CVelFliter();
	virtual ~CVelFliter();
	double GetVelStates(double predict,double differential);  //��������������Ԥ�����΢����
};

