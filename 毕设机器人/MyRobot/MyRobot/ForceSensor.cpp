#include "stdafx.h"
#include "ForceSensor.h"


CForceSensor::CForceSensor()
{
	NIDataCard = NULL;
	m_isBias = false;
	double CalibrationMatrix[6][6] = {
			{ 0.19862, 0.00298, -0.07043, -35.91298, -0.82461, -35.09399 },
			{ 0.17567, 41.68855, 0.46880, -20.5979, 0.23460, 20.39329 },
			{ -21.86787, 1.39626, -22.67093, 0.56102, 21.91165, -0.99413 },
			{ 0.13422, 0.31087, -38.94425, 0.44128, -39.55710, 1.51217 },
			{ 44.16989, -2.42101, -23.49982, 0.48488, 22.34983, -0.69161 },
			{ 0.42935, 20.55373, -0.29313, 20.60041, -0.16424, -20.0283 } };
	double GainCorrectionFactor[6] = { 6.10060103362181, 6.10060103362181, 2.09459278308425,
		261.233947464091, 261.233947464091, 249.766888041799 };
	double tmp[6] = { 0, 0, 0, 0, 0, 0 };
	memcpy(m_CalibrationMatrix, CalibrationMatrix,sizeof(double)*36);
	memcpy(m_GainCorrectionFactor, GainCorrectionFactor, sizeof(double) * 6);
	memcpy(m_StainVoltage, tmp, sizeof(double) * 6);
	memcpy(m_ForceScrew, tmp, sizeof(double) * 6);
	memcpy(m_Bias, tmp, sizeof(double) * 6);
}


CForceSensor::~CForceSensor()
{
	delete NIDataCard;
}

void CForceSensor::InitForceSensor(void)
{
	NIDataCard = new DAQSys();
	int iSaturated = NIDataCard->ScanGauges(m_StainVoltage, 1);
	if (iSaturated)  //�жϰ忨�Ƿ񱥺�
	{
		AfxMessageBox(_T("�忨��ѹ�ɼ��ӽ�����!"), MB_OK);
	}
	CalculateForceData();
}

///�������1�����ѹ������ͣ���������㣬���ѹ����Ǳ���
int CForceSensor::UpdataForceData(void)  //����������ϲ㶨ʱ���ã����Խ���ˢ�º���
{
	int iSaturated = NIDataCard->ScanGauges(m_StainVoltage, 1);
	CalculateForceData();
	return iSaturated;
}

void CForceSensor::CalculateForceData(void)
{
	if (m_isBias)  //����ƫ��
	{
		for (int i = 0; i < 6; i++)   //��ȥƫ��ֵ
		{
			m_StainVoltage[i] = m_StainVoltage[i] - m_Bias[i];
		}
	}
	for (int i = 0; i < 6; i++)
	{
		for (int j = 0; j < 6; j++)
		{
			m_ForceScrew[i] += m_CalibrationMatrix[i][j] * m_StainVoltage[j];
		}
	}
	for (int i = 0; i < 6; i++)
	{
		m_ForceScrew[i] = m_ForceScrew[i] / m_GainCorrectionFactor[i];
	}
}

void CForceSensor::GetBias(void)
{
	int iSaturated = NIDataCard->ScanGauges(m_StainVoltage, 1);
	if (iSaturated)  //�жϰ忨�Ƿ񱥺�
	{
		AfxMessageBox(_T("�忨��ѹ�ɼ��ӽ�����!"), MB_OK);
	}
	for (int i = 0; i < 6; i++)
	{
		m_Bias[i] = m_StainVoltage[i];
	}
//	m_isBias = false;
}

void CForceSensor::CloseBias(void)
{
	m_isBias = false;
}

void CForceSensor::OpenBias(void)
{
	m_isBias = true;
}