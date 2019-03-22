#pragma once
#include "afx.h"
#include "DAQSys.h"
#include "GRB4Robot.h"

#define OPENVITUAL
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
	bool m_isBias;   ////���isBias=true,��ʹ��ƫ��
	double m_ForceScrew[6];
	double m_ForceScrewBase[6];
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

