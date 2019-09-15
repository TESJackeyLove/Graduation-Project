#pragma once
#include "afx.h"
#define pi 3.141592653589793238

struct t_Coordinate{
	double Coordinate[3][4];
};//λ�˾���

class CPlanner :
	public CObject
{
	public:
		CPlanner();
		virtual ~CPlanner();
	public:
		double JointGoal[6];				//�ؽ�Ŀ��λ�ã����6��
		double m_PlanningSamplePeriod;		//�岹���ڣ���ָ��������΢Сֱ�߶�֮��Ĳ岹ʱ����
		double m_DesiredMoveTime;			//�����˶�ʱ��
		double m_DesiredMoveVelocity;		//�����˶��ٶ�
		int    m_PlanningFlag;					//0: ��ʱ��滮, 1:���ٶȹ滮
		CList <t_Coordinate, t_Coordinate&> m_InterpolationList; //��ֵ���б��滮�õ�·���������
		int   m_CurrentListPosition;			//����ǰλ��

	public:
		bool SetPlanningFlag(int flag);
		void SetDesiredTime(double VelOrTime);
		bool GetNextViaTn(double HandGoal[3][4]);
		void SetDesiredVelocity(double VelOrTime);
		int SMovePlaningWithLProfile(double PositonFrom[3][4], double PositionTo[3][4]);
    public: //ʦ���Լ�����ĺ���
		int ArcMovePlaning(double start_tn[3][4], double center[2], double deg);

	public:
		//��ѧ����ԭ��
		  static short Judge_Quadrant(double x, double y);
		  static void matrix_multiply(double r[3][3], double r1[3][3], double r2[3][3]);
		  static bool is_matrix_same(double r1[3][3], double r2[3][3]);
		  static double vector_dot_mult(double v1[3], double v2[3]);
		  static double RL_atan2(double sy, double sx);
};

