#pragma once
#include "afx.h"
#include "DAQSys.h"
#include "GRB4Robot.h"
#include <utility>
#define OPENVITUAL

struct RobotData
{
	double JointsNow[4];
	double JointsNext[4];
	double JointsVelNow[4];
	double JointsVelNext[4];
	double Origin6axisForce[6];
	double JointsTorque[4];
	double CartesianPositionNow[4];
	double CartesianPositionNext[4];
	double CartesianVelNow[4];
	double CartesianVelNext[4];
};

struct MiniRecvData
{
	double JointsNow[4];
	double JointsVelNow[4];
	double Origin6axisForce[6];
};

typedef double(*FG)(int);  //force generate ������������ָ������ָ��
class CForceSensor :
	public CObject
{
private:
	CForceSensor();  //����ģʽ�����ع��캯��
#ifdef OPENVITUAL

#else
	DAQSys* NIDataCard;
	double m_GainCorrectionFactor[6];
	double m_CalibrationMatrix[6][6];
#endif
	static CForceSensor* pForceSense;
public:
	
	static CForceSensor* getForceSensorInstance();
	
	virtual ~CForceSensor();
	virtual void ForceBaseAxia(CRobotBase *Robot);

#ifdef OPENVITUAL
	/*ʹ�ú�������������ʽ*/
	bool m_isBias;   ////���isBias=true,��ʹ��ƫ��
	double m_ForceScrew[6];
	double m_ForceScrewBase[6];
	/*ʹ����������ʱ�򣬸���ʹ��һϵ�е����У�����ʼ��һֱ����ֹ�㣬���е���ֹ��֮����ֹͣ*/
	int T_start;  //ʹ����������ʱ��Ŀ�ʼ��
	int T_end;    //ʹ����������ʱ�����ֹ��
	int T_head;   //����ָ��ĵ�
	int interval; //ָ��ÿ�����еļ��
	FG FGFunc;
	std::pair<int, FG> fchannelANDfunc[6];
	bool bind(int ForceChannel,CString funcName);
	void getNextPoint(void);
	void getForceDataUseFunc(void);   //ʹ�ú���
	void getForceDataUse_JS_Key(void);   //ʹ��ҡ��

#else
	bool m_isBias;   ////���isBias=true,��ʹ��ƫ��
	double m_StainVoltage[7];
	double m_ForceScrew[6];
	double m_ForceScrewBase[6];
	double m_Bias[6];   ///����ƫ��ֵ
#endif
	void InitForceSensor(void);
	int UpdataForceData(void);
	void CalculateForceData(void);
	void GetBias(void);
	void CloseBias(void);
	void OpenBias(void);
};


#ifdef OPENVITUAL

//ʹ�ú����������ķ�ʽ��ͬ�ĺ��������������к���ָ�룬ָ��ͬһ������
double Mode_1(int T_Head);
double Mode_2(int T_Head);
double NOTFUNC(int T_Head);
double Mode_Zero(int T_Head);
double Mode_ForceControl(int T_Head);

#endif