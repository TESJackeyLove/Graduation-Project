#include "stdafx.h"
#include "Planner.h"
#include <math.h>

/*
	���ǻ����˵Ĺ滮��
*/
/*
������˵������ڹ̸ߵ�CController��Ĺ���ԭ��
1�������Ĺ����ǰ����ٶȻ���ʱ��滮��һ��ֱ�߻���Բ������ɢ�㣬Ȼ��������ɢ��浽�Լ��۵ĳ�Ա������������
���ⲿ������������������ɢ���ʱ���ú���GetNextViaTn(double HandGoal[3][4])����ɢ�õ�λ�˾��󴫵ݽ�ȥ
2����������ṩ�ܶຯ���Ų����Լ���Ա������ֵ���Ӷ����Ĺ滮ģʽ���趨���ٶȻ����趨�Ĺ滮ʱ��
3������Ϊʲô��Ҫ��������������ɢ���أ���ʵԭ��ܼ򵥣����ֱ��ʹ��start_tn-->end_tn ���������Ϣ��
ת�����ؽ�����ռ���ʵ���ǹؽڿռ�Ĺ滮����ֱ������ռ���ʵ�Ƕ�λ��ߵ���ϣ�������ֱ����ɢ֮����Ȼ��ÿ����
���ڵ���ɢ��֮�������ǹؽڿռ�Ĺ滮��������������������ĩ��ִ������ֱ������ռ��Ѿ���ֱ���ˡ���ʵ���������·��
ֱ����start_tn-->end_tn��Ҳ����


*/

CPlanner::CPlanner()     //###################ע�����
{
	m_DesiredMoveTime = 0;
	m_PlanningFlag = 0;			//��ʱ��滮
	m_PlanningSamplePeriod = 0.5; //500����
}


CPlanner::~CPlanner()    
{

}

/* 
	ֱ�߲岹��ֱ�߹滮,������滮������       �Ҹо��̸ߵĳ��򲻶ԣ���д//*******@wqq
	����ֵ�� 0�� OK
		   100:  m_PlanningFlag��Ч
		   101�� Planning set ����Ч��  ����˵�滮�ľ���̫���ˣ�һ����ɢ�ĵ㶼�ò����������������Ӧ�ò����ܷ���
*/
//***********************wqq   ����Ϊ����������ܻ���������ģ�Dx=Dx = Dx / dscrt_num ��һ�������������һ���ܵ�
//endpoint  double�ľ�������
int CPlanner::SMovePlaningWithLProfile(double start_tn[3][4], double end_tn[3][4])  //###################ע�����
{
	double Ns[3], Os[3], As[3];  		// [N,O,A] of start point----orientation   Ns;Os;As ���е�s��ʾstart��ʼ��
	double Ps[3];					// P vector of start point----position
	double Ne[3], Oe[3], Ae[3];  		// [N,O,A] of end point----orientation     Ne;Oe;Ae ���е�e��ʾend��ֹ��
	double Pe[3];					// P vector of end point----position
	double psi, theta, phai;       //ŷ���ǣ��צȦգ���Ӣ�Ķ���
	double pp[3], tp[3], coeff[3];
	double sp, cp, vt, st, ct, sin_phai, cos_phai;
	double sTe[3][3];

	double via_Tn[3][4], via_Rot[3][3], del_Rot[3][3], del_P[3];
	double strt_Rot[3][3], end_Rot[3][3];

	double x1, y1, z1, x2, y2, z2;
	int i, j, k;
	bool rotmtr_same;

	double Dx, Dy, Dz, Dpsi, Dtheta, Dphai;   //����D������ɢ����˼
	double MotionDistance;         //������ֱ�߾���
	int dscrt_num;                 //��ɢ��ĸ���

	for (i = 0; i<3; i++)
		for (j = 0; j<3; j++)
			strt_Rot[i][j] = start_tn[i][j];//�����ת����  3*3�������ת����

	for (i = 0; i<3; i++) Ns[i] = start_tn[i][0];  // N����
	for (i = 0; i<3; i++) Os[i] = start_tn[i][1];  // O����
	for (i = 0; i<3; i++) As[i] = start_tn[i][2];  // A����
	for (i = 0; i<3; i++) Ps[i] = start_tn[i][3];//�������   P����

	for (i = 0; i<3; i++)
		for (j = 0; j<3; j++)
			end_Rot[i][j] = end_tn[i][j];//�յ���ת���� 

	for (i = 0; i<3; i++) Ne[i] = end_tn[i][0];
	for (i = 0; i<3; i++) Oe[i] = end_tn[i][1];
	for (i = 0; i<3; i++) Ae[i] = end_tn[i][2];
	for (i = 0; i<3; i++) Pe[i] = end_tn[i][3];//�յ������

	//3������XYZ�����ϵ�λ�ò����
	Dx = Pe[0] - Ps[0];   
	Dy = Pe[1] - Ps[1];
	Dz = Pe[2] - Ps[2];

	//������ֱ�߾���
	MotionDistance = sqrt(Dx*Dx + Dy*Dy + Dz*Dz);

	//�滮ģʽ��0������ʱ��滮��1:�����ٶȹ滮
	switch (m_PlanningFlag)
	{
	case 0:
		// �����˶�ʱ��/�滮���� �� �ܵĹ滮����  dscrt_num��ɢ�������
		dscrt_num = (int)ceil(m_DesiredMoveTime / m_PlanningSamplePeriod);  //ceil���ش��ڻ��ߵ�������ֵ����С����
		break;

	case 1:
		// ֱ�߾���/���滮���ٶ�*�岹���ڣ�=ֱ�߾���/�滮����λ�� �� �ܵĹ滮����
		dscrt_num = (int)ceil(MotionDistance / m_DesiredMoveVelocity / m_PlanningSamplePeriod);
		//m_DesiredMoveVelocity ֱ�ǿռ���ʵ�ٶ� mm/s
		break;
	default:
		return 100;
	}

	if (dscrt_num<1)
		return 101;

	//������XYZ�����ϵĵ�������
	Dx = Dx / dscrt_num; Dy = Dy / dscrt_num; Dz = Dz / dscrt_num;

	/*************����ŷ����**************/
	/*
		UTs=[Ns,Os,As]  U���ϱ���������ϵ s�ǿ�ʼ��
		UTe=[Ne,Oe,Ae]  U���ϱ���������ϵ e����ֹ��
		����   UTe=UTs * sTe  ����sTe��e�����s����ת����
		����   sTe = (UTs)^T * UTe = [Ns,Os,As]^T * [Ne,Oe,Ae]
		                           =[Ns*Ne Ns*Oe Ns*Ae
								     Os*Ne Os*Oe Os*Ae
									 As*Ne As*Oe As*Ae]                  
	*/
	rotmtr_same = is_matrix_same(strt_Rot, end_Rot);//�ж�����յ���ת�����Ƿ�һ�£�����bool����
	if (!rotmtr_same)  //�����ʼ���ŷ��������ֹ���ŷ���ǲ����
	{
		sTe[0][0] = vector_dot_mult(Ns, Ne);  
		sTe[1][0] = vector_dot_mult(Os, Ne);
		sTe[2][0] = vector_dot_mult(As, Ne);

		sTe[0][1] = vector_dot_mult(Ns, Oe);
		sTe[1][1] = vector_dot_mult(Os, Oe);
		sTe[2][1] = vector_dot_mult(As, Oe);

		sTe[0][2] = vector_dot_mult(Ns, Ae);
		sTe[1][2] = vector_dot_mult(Os, Ae);
		sTe[2][2] = vector_dot_mult(As, Ae);

		// psi
		psi = RL_atan2(sTe[1][2], sTe[0][2]);
		// theta
		theta = RL_atan2(sTe[0][2] * cos(psi) + sTe[1][2] * sin(psi),sTe[2][2]);    //***********@wqq

		// phai
		phai = RL_atan2(-1.0 * sTe[0][0] * sin(psi) + sTe[1][0] * cos(psi), -1.0 * sTe[1][0] * sin(psi) + sTe[1][1] * cos(psi));
	}

	Dphai = phai / dscrt_num; Dtheta = theta / dscrt_num; Dpsi = psi / dscrt_num;  //ÿ��С����ŷ����ԽǶ�

	double DeltaPhai, DeltaTheta, DeltaPsi;

	del_P[0] = 0; del_P[1] = 0; del_P[2] = 0;
	DeltaPhai = 0; DeltaTheta = 0; DeltaPsi = 0;    //�ܵĽǶ�

	//��չ滮���б�
	t_Coordinate coordinatePoint;
	m_InterpolationList.RemoveAll();

	//�滮��ʼ
	int ii, jj;
	for (i = 1; i <= dscrt_num; i++)  
	{
		for (ii = 0; ii<3; ii++)
			for (jj = 0; jj<4; jj++)
				via_Tn[ii][jj] = 0;//�м���ʱλ�˾����ʼ����0


		if (rotmtr_same) //�����ֹ��ת������ͬ���м���ת���󲻱�
		{
			for (j = 0; j<3; j++)
				for (k = 0; k<3; k++)
					via_Rot[j][k] = strt_Rot[j][k];
		}
		else 			//��������м���ת����
		{
			DeltaPhai += Dphai;   //�ܵĽǶ�ÿ�μ�����ɢ�ĽǶ�
			DeltaTheta += Dtheta;    //�ܵĽǶ�ÿ�μ�����ɢ�ĽǶ�
			DeltaPsi += Dpsi;      //�ܵĽǶ�ÿ�μ�����ɢ�ĽǶ�
//*********************************@wqq   ��������Ϊ�̸���ȫ������Ϊд����
			del_Rot[0][0] = cos(DeltaPsi)*cos(DeltaTheta)*cos(DeltaPhai) - sin(DeltaPsi)*sin(DeltaPhai);
			del_Rot[1][0] = sin(DeltaPsi)*cos(DeltaTheta)*cos(DeltaPhai) + cos(DeltaPsi)*sin(DeltaPhai);
			del_Rot[2][0] = -1 * sin(DeltaTheta)*cos(DeltaPhai);
			del_Rot[1][0] = -1 * cos(DeltaPsi)*cos(DeltaTheta)*sin(DeltaPhai) - sin(DeltaPsi)*cos(DeltaPhai);
			del_Rot[1][1] = -1 * sin(DeltaPsi)*cos(DeltaTheta)*sin(DeltaPhai) + cos(DeltaPsi)*cos(DeltaPhai);
			del_Rot[1][2] = sin(DeltaTheta)*sin(DeltaPhai);
			del_Rot[2][0] = cos(DeltaPsi)*sin(DeltaTheta);
			del_Rot[2][1] = sin(DeltaPsi)*sin(DeltaTheta);
			del_Rot[2][2] = cos(DeltaTheta);
//*********************************@wqq   ���ŷ����ʽ
			matrix_multiply(via_Rot, strt_Rot, del_Rot);  //������ҳ�
		}

		for (j = 0; j<3; j++)
			for (k = 0; k<3; k++)
				via_Tn[j][k] = via_Rot[j][k];//�� �м���ת���� ��ֵ���м���ʱλ�˾���

		del_P[0] += Dx;
		del_P[1] += Dy;
		del_P[2] += Dz;

		//�м�̬��ʱλ�˾��� �����µ�λ��ʸ��
		via_Tn[0][3] = Ps[0] + del_P[0];
		via_Tn[1][3] = Ps[1] + del_P[1];
		via_Tn[2][3] = Ps[2] + del_P[2];
		//���˵õ��˹滮�� λ�˾���Via_Tn

		//����coordinatePoint������󣬽�Via_Tn��������
		for (ii = 0; ii<3; ii++)
			for (jj = 0; jj<4; jj++)
				coordinatePoint.Coordinate[ii][jj] = via_Tn[ii][jj];

		//���滮���������β
		m_InterpolationList.AddTail(coordinatePoint);
	}
	/* add the dest as the last interplotation point
	for(ii=0;ii<3;ii++)
	for(jj=0;jj<4;jj++)
	coordinatePoint.Coordinate[ii][jj]=end_tn[ii][jj];
	m_InterpolationList.AddTail(coordinatePoint);
	//////
	*/
	m_CurrentListPosition = 0;//��ǰ����������0
	//////

	return 0;
}

/*
			HandGoal[i][j] = coordinatePoint.Coordinate[i][j];  //ע�����ﲢû��ֱ����coordinatePoint�����ֵ�����Ǹ�ֵ���������
	��ȡ�滮���������һ���㣬��HandGoal��û�е�ͷ���false �������й滮���б�ֵһ��һ����ֵ��HandGoal[3][4]
*/
bool CPlanner::GetNextViaTn(double HandGoal[3][4])     //###################ע�����
{
	POSITION pos = m_InterpolationList.FindIndex(m_CurrentListPosition);
	if (pos == NULL)return false;
	t_Coordinate coordinatePoint;

	coordinatePoint = m_InterpolationList.GetAt(pos);  //����������IndexΪpos��Ԫ��

	for (int i = 0; i<3; i++)
		for (int j = 0; j<4; j++)
			HandGoal[i][j] = coordinatePoint.Coordinate[i][j];  //ע�����ﲢû��ֱ����coordinatePoint�����ֵ�����Ǹ�ֵ���������

	m_CurrentListPosition++;

	return true;
}
/*
	���ù滮����,���flag=0����ʱ��滮�����flag=1�������ٶȹ滮
*/
bool CPlanner::SetPlanningFlag(int flag)      //###################ע�����
{
	if (flag != 0 && flag != 1)  //flag==0 for time planning;flag==1 for velocity planning
		return false; // flag incorrect
	m_PlanningFlag = flag;
	return true;
}

/*
	���������˶�ʱ��
*/
void CPlanner::SetDesiredTime(double Time)    //###################ע�����
{
	m_DesiredMoveTime = Time;
}

/*
	���������˶��ٶ�
*/
void CPlanner::SetDesiredVelocity(double Vel)
{
	m_DesiredMoveVelocity = Vel;
}
/*
	������㣬Բ�ģ��Ƕȹ滮��Բ��·����
	���룺start_tn[][] :  ��ʼ��
		  center[] :   Բ��
		  deg    :    ��ʱ��Ϊ��  �Ƕ���
*/
int CPlanner::ArcMovePlaning(double start_tn[3][4], double center[2], double deg)
{
	double via_Tn[3][4];
	double FT, a, xCur, yCur, xCur1;
	double xSt, ySt, xEnd, yEnd;
	double es, xdir, ydir;//�����ж��յ�����
	double radius;
	int full_circle = 0, judge = 1;
	int ndir;
	int i, j;

	/*ƽ��Բ�������õ���ת����Ϊ����ʽһ�£��������ת����ֵ�����е�*/
	for (i = 0; i<3; i++)
		for (j = 0; j<4; j++)
			via_Tn[i][j] = start_tn[i][j];

	xSt = start_tn[0][3] - center[0];
	ySt = start_tn[1][3] - center[1];		//����������Բ������

	if (fabs(xSt)<0.000001&&fabs(ySt)<0.000001) //����Բ�Ĳ���Ϊһ�㣬����滮ʧ��
		return -1;

	xCur = xSt;
	yCur = ySt;							//����ǰ���Ƶ���� 

	double d = deg * pi / 180.;     //ת��Ϊ������
	xEnd = xSt*cos(d) - ySt*sin(d); 		//�����յ����Բ������
	yEnd = ySt*cos(d) + xSt*sin(d);
	/*ֱ�Ӱ��ٶȹ滮��������ʱ��滮*/
	m_PlanningFlag = 1;
	FT = m_DesiredMoveVelocity*m_PlanningSamplePeriod;	//����FT

	radius = sqrt(xSt*xSt + ySt*ySt);//�뾶
	es = FT / 2;
	a = 2 * asin(FT / 2 / radius);		//ÿ���岹���������������߶�Ӧ��Բ�Ľ� 

	/*�������,������������*/
	t_Coordinate coordinatePoint;
	m_InterpolationList.RemoveAll();
	for (i = 0; i<3; i++)
		for (j = 0; j<4; j++)
			coordinatePoint.Coordinate[i][j] = via_Tn[i][j];
	m_InterpolationList.AddTail(coordinatePoint);
	if (deg == 360 || deg == -360)    		//�������յ�
		full_circle = 1;						//��Բ�ж�
	if (deg>0)							//��Բ�岹  
	{
		while (judge == 1 || full_circle == 1)		//�յ��ж� ����Բ�ж�
		{
			xCur1 = xCur*cos(a) - yCur*sin(a);	//���Ǻ���Բ���岹�ĵ�����ʽ       
			yCur = yCur*cos(a) + xCur*sin(a);
			xCur = xCur1;
			via_Tn[0][3] = xCur + center[0]; //����ƽ�� �������������
			via_Tn[1][3] = yCur + center[1];
			for (i = 0; i<3; i++)
				for (j = 0; j<4; j++)
					coordinatePoint.Coordinate[i][j] = via_Tn[i][j];
			m_InterpolationList.AddTail(coordinatePoint);

			full_circle = 0;
			xdir = xCur - xEnd;
			ydir = yCur - yEnd;
			ndir = Judge_Quadrant(xdir, ydir);
			switch (ndir)
			{
			case 1:judge = xdir >= es || ydir >= es; break;
			case 2:judge = (-xdir) >= es || ydir >= es; break;
			case 3:judge = (-xdir) >= es || (-ydir) >= es; break;
			case 4:judge = xdir >= es || (-ydir) >= es; break;
			}
		}
	}
	else									//˳Բ�岹 
	{
		while (judge == 1 || full_circle == 1)		//�յ��ж�    
		{
			xCur1 = xCur*cos(a) + yCur*sin(a);  //���Ǻ���Բ���岹�ĵ�����ʽ    
			yCur = yCur*cos(a) - xCur*sin(a);
			xCur = xCur1;
			via_Tn[0][3] = xCur + center[0];
			via_Tn[1][3] = yCur + center[1];
			for (i = 0; i<3; i++)
				for (j = 0; j<4; j++)
					coordinatePoint.Coordinate[i][j] = via_Tn[i][j];
			m_InterpolationList.AddTail(coordinatePoint);
			full_circle = 0;

			xdir = xCur - xEnd;
			ydir = yCur - yEnd;
			ndir = Judge_Quadrant(xdir, ydir);
			switch (ndir)
			{
			case 1:judge = xdir >= es || ydir >= es; break;
			case 2:judge = (-xdir) >= es || ydir >= es; break;
			case 3:judge = (-xdir) >= es || (-ydir) >= es; break;
			case 4:judge = xdir >= es || (-ydir) >= es; break;
			}
		}
	}
	m_CurrentListPosition = 0;
	return 0;
}
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////��ѧ����ԭ�ͺ���
/*
	�жϲ���������������޲�������Ӧ����ֵ
	���룺	(x,y) ����
	����ֵ��   1-4����

*/
short CPlanner::Judge_Quadrant(double x, double y)
{
	if (x >= 0)
	{
		if (y >= 0) return 1;
		else return 4;
	}
	else
	{
		if (y >= 0) return 2;
		else return 3;
	}

}

/*
	����ĳ˷�������3*3�ľ�����ˣ����������r[3][3]
*/
void CPlanner::matrix_multiply(double r[3][3], double r1[3][3], double r2[3][3])
{
	int i, j, k;
	for (i = 0; i<3; i++)
		for (j = 0; j<3; j++)
		{
			r[i][j] = 0.0;
			for (k = 0; k<3; k++)
				r[i][j] += r1[i][k] * r2[k][j];
		}
}

/*
	�Ƚ����������ľ����Ƿ���ͬ�������ͬ������true
*/
bool CPlanner::is_matrix_same(double r1[3][3], double r2[3][3])
{
	int i, j;
	for (i = 0; i<3; i++)
		for (j = 0; j<3; j++)
		{
			if (fabs(r1[i][j] - r2[i][j])<0.0000001)
				continue;
			else
				return(false);
		}
	return true;
}

/*
	����֮��ĵ�ˣ�����һ������
*/
double CPlanner::vector_dot_mult(double v1[3], double v2[3])
{
	return(v1[0] * v2[0] + v1[1] * v2[1] + v1[2] * v2[2]);
}

/*
	���� sy/sx�� arctan�������к����� (-pi<=arctg(sy,sx)<=pi)  ,���Դ��������ĸ����޵����
*/

double CPlanner::RL_atan2(double sy, double sx)
{
	//double  mp;
	//double  e1 = 1E-16, e2 = -1E-16;
	//if (fabs(sx)>e1) 
	//{
	//	mp = atan(fabs(sy) / fabs(sx));
	//	if ((sx>e1) && (sy >= e1))  return(mp);
	//	if ((sx<e2) && (sy >= e1))  return(pi - mp);
	//	if ((sx>e1) && (sy<e1))   return(-mp);
	//	if ((sx<e2) && (sy<e1))   return(-pi + mp);
	//}
	//else 
	//{
	//	if (fabs(sy) <= e1) return(0);
	//	if (sy>e1)      return(pi / 2);
	//	if (sy<e2)      return(-pi / 2);
	//}

	//return 0;
	return atan2(sy,sx);       //********************************@wqq
}
//
////Straight line planning by linear interpolation
//int CPlanner::SMovePlaningWithLProfile(double start_tn[3][4], double end_tn[3][4])
////return 0: ok;
////		100: m_PlanningFlag is invalid
////		101: planning set is invalid
//{
//	double Ns[3], Os[3], As[3];  // [N,O,A] of start point----orientation
//	double Ps[3]; // P vector of start point----position
//	double Ne[3], Oe[3], Ae[3];  // [N,O,A] of end point----orientation
//	double Pe[3]; // P vector of end point----position
//	double psi, theta, phai;
//	double pp[3], tp[3], coeff[3];
//	double sp, cp, vt, st, ct, sin_phai, cos_phai;
//
//	double via_Tn[3][4], via_Rot[3][3], del_Rot[3][3], del_P[3];
//	double strt_Rot[3][3], end_Rot[3][3];
//
//	double x1, y1, z1, x2, y2, z2;
//	int i, j, k;
//	BOOL rotmtr_same;
//
//	//---------------------------------------------------
//	double Dx, Dy, Dz, Dpsi, Dtheta, Dphai;
//	double MotionDistance;
//	int dscrt_num;
//
//	for (i = 0; i<3; i++)
//		for (j = 0; j<3; j++)
//			strt_Rot[i][j] = start_tn[i][j];
//
//	for (i = 0; i<3; i++) Ns[i] = start_tn[i][0];
//	for (i = 0; i<3; i++) Os[i] = start_tn[i][1];
//	for (i = 0; i<3; i++) As[i] = start_tn[i][2];
//	for (i = 0; i<3; i++) Ps[i] = start_tn[i][3];
//
//	for (i = 0; i<3; i++)
//		for (j = 0; j<3; j++)
//			end_Rot[i][j] = end_tn[i][j];
//
//	for (i = 0; i<3; i++) Ne[i] = end_tn[i][0];
//	for (i = 0; i<3; i++) Oe[i] = end_tn[i][1];
//	for (i = 0; i<3; i++) Ae[i] = end_tn[i][2];
//	for (i = 0; i<3; i++) Pe[i] = end_tn[i][3];
//
//	Dx = Pe[0] - Ps[0];
//	Dy = Pe[1] - Ps[1];
//	Dz = Pe[2] - Ps[2];
//
//	//Computer the motion distance in Cartisan
//	MotionDistance = sqrt(Dx*Dx + Dy*Dy + Dz*Dz);
//
//	switch (m_PlanningFlag)
//	{
//	case 0:
//		dscrt_num = (int)ceil(m_DesiredMoveTime / m_PlanningSamplePeriod);
//		break;
//
//	case 1:
//		dscrt_num = (int)ceil(MotionDistance / m_DesiredMoveVelocity / m_PlanningSamplePeriod);
//		break;
//	default:
//		return 100;
//	}
//
//	if (dscrt_num<1)
//		return 101;
//
//	//Move step in XYZ coordinates
//	Dx = Dx / dscrt_num; Dy = Dy / dscrt_num; Dz = Dz / dscrt_num;
//
//	//Compute Euler angle of rotation
//	rotmtr_same = is_matrix_same(strt_Rot, end_Rot);
//	if (!rotmtr_same) {
//		tp[0] = vector_dot_mult(Ns, Ae);
//		tp[1] = vector_dot_mult(Os, Ae);
//		tp[2] = vector_dot_mult(As, Ae);
//		// psi
//		psi = RL_atan2(tp[1], tp[0]);
//		// theta
//		theta = RL_atan2(sqrt(tp[0] * tp[0] + tp[1] * tp[1]), tp[2]);
//		sp = sin(psi);      cp = cos(psi);
//		st = sin(theta);    ct = cos(theta);
//		vt = 1.0 - ct;
//		coeff[0] = -sp*cp*vt;
//		coeff[1] = cp*cp*vt + ct;
//		coeff[2] = -sp*st;
//		tp[0] = vector_dot_mult(Ns, Ne);
//		tp[1] = vector_dot_mult(Os, Ne);
//		tp[2] = vector_dot_mult(As, Ne);
//		sin_phai = coeff[0] * tp[0] + coeff[1] * tp[1] + coeff[2] * tp[2];
//		tp[0] = vector_dot_mult(Ns, Oe);
//		tp[1] = vector_dot_mult(Os, Oe);
//		tp[2] = vector_dot_mult(As, Oe);
//		cos_phai = coeff[0] * tp[0] + coeff[1] * tp[1] + coeff[2] * tp[2];
//		// phai
//		phai = RL_atan2(sin_phai, cos_phai);
//	}
//
//	Dphai = phai / dscrt_num; Dtheta = theta / dscrt_num; Dpsi = psi / dscrt_num;
//
//	double DeltaPhai, DeltaTheta, DeltaPsi;
//
//	del_P[0] = 0; del_P[1] = 0; del_P[2] = 0;
//	DeltaPhai = 0; DeltaTheta = 0; DeltaPsi = 0;
//
//	/////empty the interpolation list
//	t_Coordinate coordinatePoint;
//	m_InterpolationList.RemoveAll();
//	int ii, jj;
//	/////
//
//	//�滮��ʼ
//	for (i = 1; i <= dscrt_num; i++)
//	{
//		for (ii = 0; ii<3; ii++)
//			for (jj = 0; jj<4; jj++)
//				via_Tn[ii][jj] = 0;
//		///�����ת������ͬ��	
//		if (rotmtr_same)
//		{
//			for (j = 0; j<3; j++)
//				for (k = 0; k<3; k++)
//					via_Rot[j][k] = strt_Rot[j][k];
//		}
//		else
//		{
//			DeltaPhai += Dphai;
//			DeltaTheta += Dtheta;
//			DeltaPsi += Dpsi;
//			pp[0] = sin(DeltaPhai);
//			pp[1] = cos(DeltaPhai);
//			tp[0] = sin(DeltaTheta);
//			tp[1] = cos(DeltaTheta);
//
//			///I modified here
//			//////}
//
//			tp[2] = 1 - tp[1]; // vers(lamda*theta)
//
//			// delta O
//			x1 = del_Rot[0][1] = -pp[0] * (sp*sp*tp[2] + tp[1]) + pp[1] * (-sp*cp*tp[2]);
//			y1 = del_Rot[1][1] = -pp[0] * (-sp*cp*tp[2]) + pp[1] * (cp*cp*tp[2] + tp[1]);
//			z1 = del_Rot[2][1] = -pp[0] * (-cp*tp[0]) + pp[1] * (-sp*tp[0]);
//			// delta A
//			x2 = del_Rot[0][2] = cp*tp[0];
//			y2 = del_Rot[1][2] = sp*tp[0];
//			z2 = del_Rot[2][2] = tp[1];
//			// delta N
//			del_Rot[0][0] = y1*z2 - z1*y2;
//			del_Rot[1][0] = z1*x2 - x1*z2;
//			del_Rot[2][0] = x1*y2 - y1*x2;
//			matrix_multiply(via_Rot, strt_Rot, del_Rot);
//
//		}
//
//		for (j = 0; j<3; j++)
//			for (k = 0; k<3; k++)
//				via_Tn[j][k] = via_Rot[j][k];
//
//		del_P[0] += Dx;
//		del_P[1] += Dy;
//		del_P[2] += Dz;
//
//		via_Tn[0][3] = Ps[0] + del_P[0];
//		via_Tn[1][3] = Ps[1] + del_P[1];
//		via_Tn[2][3] = Ps[2] + del_P[2];
//
//		//����������󣬽�Via_Tn��������
//		//////
//		for (ii = 0; ii<3; ii++)
//			for (jj = 0; jj<4; jj++)
//				coordinatePoint.Coordinate[ii][jj] = via_Tn[ii][jj];
//
//		m_InterpolationList.AddTail(coordinatePoint);
//		//////   
//	}
//	/* add the dest as the last interplotation point
//	for(ii=0;ii<3;ii++)
//	for(jj=0;jj<4;jj++)
//	coordinatePoint.Coordinate[ii][jj]=end_tn[ii][jj];
//	m_InterpolationList.AddTail(coordinatePoint);
//	//////
//	*/
//	m_CurrentListPosition = 0;
//	//////
//
//	return 0;
//}