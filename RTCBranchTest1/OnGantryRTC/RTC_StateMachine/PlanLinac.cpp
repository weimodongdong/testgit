/*******************************************************************************
** File	      : PlanHandler.cpp
** Author:    : ChenLong
** Version    :	1.0
** Environment: VS2013
** Description:
** History    :	18/10/16    Chenlong    Created
*******************************************************************************/
/* INCLUDE ===================================================================*/
#include "PlanLinac.h"

/* METHODS===================================================================*/
/******************************************************************************
* Function: CPlanLinac()
*
* Describe: construct function
*
* Input   : None
*
* Output  : None
*******************************************************************************/
CPlanLinac::CPlanLinac(HandlerInputs_t* HandlerInputs)
{
	m_PlanBufferPtr = HandlerInputs;
	//memset(&m_MaxTime, 0, sizeof(m_MaxTime));
	//m_Tmax = 0;
	m_IntervalStep = 0;
	m_MinServoAxisSpeed = 0;
	memset(m_ServoAxisTime, 0, sizeof(m_ServoAxisTime));
	memset(m_AllAxisStepMaxTime, 0, sizeof(m_AllAxisStepMaxTime));
	m_CurrentServoType = None;
	m_TreatmentPlanType = PLAN_TYPE_NONE;
}

CPlanLinac::~CPlanLinac()
{
}

/******************************************************************************
* Function: HandlePlanData(HandlerInputs_t DataBuffer)
*
* Describe: 
*
* Input   : None
*
* Output  : None
*******************************************************************************/
BOOL CPlanLinac::HandlePlanData(VOID)
{
	if (NULL == m_PlanBufferPtr->BeamPlanPtr || \
		NULL == m_PlanBufferPtr->OriginalPlanPtr || \
		NULL == m_PlanBufferPtr->BackCalSpeedPtr || \
		NULL == m_PlanBufferPtr->DeltaPtr || \
		NULL == m_PlanBufferPtr->ResComponentPtr)
	{
		return FALSE;	//intput parameter is invaild
	}
	else
	{
		ExtendRadiationIndex();
		GetDeltaCP(); //calculate delta value of all cp
		GetMaxTime(); //calculate max time of all cp
		GetCurrentServoAxis(); //find current servo axis
		GetServoAxisDeltaTime(); //calculate servo axis delta time
		GetSpeed(); //match speed to all axis
			
		return TRUE;
	}
}

/******************************************************************************
* Function: GetDeltaCP(VOID)
*
* Describe: figure out CP delta value
*
* Input   : None
*
* Output  : None
*******************************************************************************/
BOOL CPlanLinac::GetDeltaCP(VOID)
{
	for (size_t i = 0; i < m_PlanBufferPtr->BeamPlanPtr->TotalCP - 1; i++)
	{
		//calculate delta value of Dose
		m_PlanBufferPtr->DeltaPtr->DoseWeight[i] = (m_PlanBufferPtr->OriginalPlanPtr[i + 1].DoseWeight * m_PlanBufferPtr->BeamPlanPtr->Dose) - (m_PlanBufferPtr->OriginalPlanPtr[i].DoseWeight * m_PlanBufferPtr->BeamPlanPtr->Dose);
		
		//calculate delta value of Mlc
		for (size_t j = 0; j < MLC_LEAF_NUM; j++)		//120 mlc leafs
		{
			m_PlanBufferPtr->DeltaPtr->MlcPos[i][j] = static_cast<FLOAT>(fabs_(m_PlanBufferPtr->OriginalPlanPtr[i + 1].MlcPos[j] - m_PlanBufferPtr->OriginalPlanPtr[i].MlcPos[j]));
		}

		//calculate delta value of Gantry
		FLOAT TempGantryPosition = 0;
		if (m_PlanBufferPtr->OriginalPlanPtr[i].GantryDir == CW)
			TempGantryPosition = static_cast<FLOAT>(fmod_((m_PlanBufferPtr->OriginalPlanPtr[i + 1].GantryAngle - m_PlanBufferPtr->OriginalPlanPtr[i].GantryAngle + 360.0), 360.0));
		else if (m_PlanBufferPtr->OriginalPlanPtr[i].GantryDir == CCW)
			TempGantryPosition = static_cast<FLOAT>(360.0 - (fmod_((m_PlanBufferPtr->OriginalPlanPtr[i + 1].GantryAngle - m_PlanBufferPtr->OriginalPlanPtr[i].GantryAngle + 360.0), 360.0)));
		else{ ; }
		if (fabs_(TempGantryPosition - 360.0) <= EPSINON)
		{
			m_PlanBufferPtr->DeltaPtr->GantryAngle[i] = 0;
		}
		else
		{
			m_PlanBufferPtr->DeltaPtr->GantryAngle[i] = TempGantryPosition;
		}

	}
		
	//GetDeltaDose();				//figure out dose weight delta value
	//GetDeltaMlc();				//figure out mlc position delta value
	//GetDeltaGantry();				//figure out gantry delta value

	return TRUE;
}

#if 0
/******************************************************************************
* Function: GetDeltaGantry(VOID)
*
* Describe: figure out gantry delta value
*
* Input   : None
*
* Output  : None
*******************************************************************************/
VOID CPlanLinac::GetDeltaGantry(VOID)
{
	//figure out delta value
	for (size_t i = 0; i < m_PlanBufferPtr->BeamPlanPtr->TotalCP - 1; i++)
	{
		if (m_PlanBufferPtr->OriginalPlanPtr[i].GantryDir == CW)
		{
			m_PlanBufferPtr->DeltaPtr->GantryAngle[i] = static_cast<FLOAT>(fmod_((m_PlanBufferPtr->OriginalPlanPtr[i + 1].GantryAngle - m_PlanBufferPtr->OriginalPlanPtr[i].GantryAngle + 360.0), 360.0));
		}
		else if (m_PlanBufferPtr->OriginalPlanPtr[i].GantryDir == CCW)
		{
			m_PlanBufferPtr->DeltaPtr->GantryAngle[i] = static_cast<FLOAT>(360.0 - (fmod_((m_PlanBufferPtr->OriginalPlanPtr[i + 1].GantryAngle - m_PlanBufferPtr->OriginalPlanPtr[i].GantryAngle + 360.0), 360.0)));
		}
		else{ ; }
	}

	return;
}

/******************************************************************************
* Function: GetDeltaDose(VOID)
*
* Describe: figure out dose weight delta value
*
* Input   : None
*
* Output  : None
*******************************************************************************/
VOID CPlanLinac::GetDeltaDose(VOID)
{
	//convert dose weight to dose value and calculate delta value
	for (size_t i = 0; i < m_PlanBufferPtr->BeamPlanPtr->TotalCP - 1; i++)
		m_PlanBufferPtr->DeltaPtr->DoseWeight[i] = (m_PlanBufferPtr->OriginalPlanPtr[i + 1].DoseWeight * m_PlanBufferPtr->BeamPlanPtr->Dose) - (m_PlanBufferPtr->OriginalPlanPtr[i].DoseWeight * m_PlanBufferPtr->BeamPlanPtr->Dose);

	return;
}

/******************************************************************************
* Function: GetDeltaMlcPos(VOID)
*
* Describe: figure out mlc position delta value
*
* Input   : None
*
* Output  : None
*******************************************************************************/
VOID CPlanLinac::GetDeltaMlc(VOID)
{
	//figure out mlc position delta value
	for (size_t i = 0; i < m_PlanBufferPtr->BeamPlanPtr->TotalCP - 1; i++)
	{
		for (size_t j = 0; j < MLC_LEAF_NUM; j++)		//120 mlc leafs
		{
			m_PlanBufferPtr->DeltaPtr->MlcPos[i][j] = static_cast<FLOAT>(fabs_(m_PlanBufferPtr->OriginalPlanPtr[i + 1].MlcPos[j] - m_PlanBufferPtr->OriginalPlanPtr[i].MlcPos[j]));
		}
	}

	return;
}
#endif
/******************************************************************************
* Function: GetMaxTime(VOID)
*
* Describe: figure out max time
*
* Input   : None
*
* Output  : None
*******************************************************************************/
BOOL CPlanLinac::GetMaxTime(VOID)
{
	//Check inputs parameter for calculate
	if (!InputsParameterIsReady() )
	{
		return FALSE; //stop run
	}
	else{ ; }//continue run

	//Calculate max time of all axis
	FLOAT CalculateValue = 0;
	for (size_t i = 0; i < m_PlanBufferPtr->BeamPlanPtr->TotalCP - 1; i++)
	{
		//figure out dose max time
		CalculateValue = m_PlanBufferPtr->DeltaPtr->DoseWeight[i] / (m_PlanBufferPtr->ResComponentPtr->InputsVMAT->DoseMaxSpeed / 60);		//convert minute to unit s
		if (m_AllAxisStepMaxTime[i] < CalculateValue)
			m_AllAxisStepMaxTime[i] = CalculateValue;

		//figure out gantry max time
		CalculateValue = m_PlanBufferPtr->DeltaPtr->GantryAngle[i] / m_PlanBufferPtr->ResComponentPtr->InputsVMAT->GantryMaxSpeed;
		if (m_AllAxisStepMaxTime[i] < CalculateValue)
			m_AllAxisStepMaxTime[i] = CalculateValue;

		//figure out mlc max time
		for (size_t j = 0; j < MLC_LEAF_NUM; j++)
		{
			CalculateValue = m_PlanBufferPtr->DeltaPtr->MlcPos[i][j] / m_PlanBufferPtr->ResComponentPtr->InputsVMAT->MlcMaxSpeed;
			if (m_AllAxisStepMaxTime[i] < CalculateValue)
				m_AllAxisStepMaxTime[i] = CalculateValue;
		}
	}

	//Find Tmax value
/*	FLOAT MaxValue = 0;
	FLOAT MaxArray[10] = { 0 };
	memcpy(MaxArray, &m_MaxTime, sizeof(m_MaxTime));
	for (size_t i = 0; i < 10; i++)
	{
		if (MaxValue < MaxArray[i])
			MaxValue = MaxArray[i];
	}

	m_Tmax = MaxValue;
*/
	return TRUE;
}
#if 0
/******************************************************************************
* Function: GetMaxTimeDose(VOID)
*
* Describe: figure out dose max time
*
* Input   : None
*
* Output  : None
*******************************************************************************/
BOOL CPlanLinac::GetMaxTimeDose(VOID)
{
	FLOAT CalculateValue;
	
	//figure out dose max time
	if (0 != m_PlanBufferPtr->ResComponentPtr->InputsVMAT->DoseMaxSpeed)
	{
		for (size_t i = 0; i < m_PlanBufferPtr->BeamPlanPtr->TotalCP - 1; i++)
		{
			//CalculateValue = m_DeltaCP.DoseWeight[i] / (m_SpeedSetting.DoseMaxSpeed/60);		//convert minute to unit s
			CalculateValue = m_PlanBufferPtr->DeltaPtr->DoseWeight[i] / (m_PlanBufferPtr->ResComponentPtr->InputsVMAT->DoseMaxSpeed / 60);		//convert minute to unit s

			if (m_MaxTime.DoseMaxTime < CalculateValue)
				m_MaxTime.DoseMaxTime = CalculateValue;
		}

		return TRUE;
	}
	else
	{
		m_PlanBufferPtr->ResComponentPtr->Trace->Log(tlInfo, "Line:%d::CPlanLinac::<GetMaxTimeDose>::Error->DoseMaxSpeed is 0", __LINE__);
		return FALSE;//wait for parameter ready
	}
}

/******************************************************************************
* Function: GetMaxTimeGantry(VOID)
*
* Describe: figure out gantry max time
*
* Input   : None
*
* Output  : None
*******************************************************************************/
BOOL CPlanLinac::GetMaxTimeGantry(VOID)
{
	FLOAT CalculateValue;
	
	//figure out dose max time
	if (0 != m_PlanBufferPtr->ResComponentPtr->InputsVMAT->GantryMaxSpeed)
	{
		for (size_t i = 0; i < m_PlanBufferPtr->BeamPlanPtr->TotalCP - 1; i++)
		{
			CalculateValue = m_PlanBufferPtr->DeltaPtr->GantryAngle[i] / m_PlanBufferPtr->ResComponentPtr->InputsVMAT->GantryMaxSpeed;

			if (m_MaxTime.GantryAngleMaxTime < CalculateValue)
				m_MaxTime.GantryAngleMaxTime = CalculateValue;
		}

		return TRUE;
	}
	else
	{
		m_PlanBufferPtr->ResComponentPtr->Trace->Log(tlInfo, "Line:%d::CPlanLinac::<GetMaxTimeGantry>::Error->GantryMaxSpeed is 0", __LINE__);
		return FALSE;//wait for parameter ready
	}
}

/******************************************************************************
* Function: GetMaxTimeMlc(VOID)
*
* Describe: figure out mlc max time
*
* Input   : None
*
* Output  : None
*******************************************************************************/
BOOL CPlanLinac::GetMaxTimeMlc(VOID)
{
	FLOAT CalculateValue;

	//figure out dose max time
	if (0 != m_PlanBufferPtr->ResComponentPtr->InputsVMAT->MlcMaxSpeed)
	{
		for (size_t i = 0; i < m_PlanBufferPtr->BeamPlanPtr->TotalCP - 1; i++)
		{
			for (size_t j = 0; j < MLC_LEAF_NUM; j++)
			{
				CalculateValue = m_PlanBufferPtr->DeltaPtr->MlcPos[i][j] / m_PlanBufferPtr->ResComponentPtr->InputsVMAT->MlcMaxSpeed;

				if (m_MaxTime.MlcPosMaxTime < CalculateValue)
					m_MaxTime.MlcPosMaxTime = CalculateValue;
			}
		}

		return TRUE;
	}
	else
	{
		m_PlanBufferPtr->ResComponentPtr->Trace->Log(tlInfo, "Line:%d::CPlanLinac::<GetMaxTimeMlc>::Error->MlcMaxSpeed is 0", __LINE__);
		return FALSE;//wait for parameter ready
	}
}
#endif
/******************************************************************************
* Function: ExtendRadiationIndex(VOID)
*
* Describe: extend radiation index
*
* Input   : None
*
* Output  : None
*******************************************************************************/
VOID CPlanLinac::ExtendRadiationIndex(VOID)
{
	/*Calculate interval step*/
	if (0 != m_PlanBufferPtr->BeamPlanPtr->TotalCP)
	{
		m_PlanBufferPtr->OriginalPlanPtr[0].RadiationIndex = RADIATION_INDEX_INIT;
		m_IntervalStep = 4095 / (m_PlanBufferPtr->BeamPlanPtr->TotalCP - 1);
		for (UINT32 i = 0; i < m_PlanBufferPtr->BeamPlanPtr->TotalCP; i++)
			m_PlanBufferPtr->OriginalPlanPtr[i].RadiationIndex = 1 + m_IntervalStep*i;
	}
	else
		m_PlanBufferPtr->ResComponentPtr->Trace->Log(tlInfo, "Line%d::CPlanLinac::<RecvPlanData>::m_BeamPlan.TotalCP is 0", __LINE__);
}

/******************************************************************************
* Function: GetSpeed(VOID)
*
* Describe: figure out speed of all axis
*
* Input   : None
*
* Output  : None
*******************************************************************************/
BOOL CPlanLinac::GetSpeed(VOID)
{
	/*BOOL ret = FALSE;
	ret = GetSpeedDose();
	ret = GetServoAxisMinSpeed();
	ret = GetServoAxisTime();
	ret = GetSpeedGantry();
	ret = GetSpeedMlc();
	if (FALSE == ret)
	{
		m_PlanBufferPtr->ResComponentPtr->Trace->Log(tlInfo, "Line%d::CPlanLinac::<GetSpeed>::Calculate has error", __LINE__);
		return FALSE;
	}
	else
		return TRUE;*/

	for (size_t i = 0; i < m_PlanBufferPtr->BeamPlanPtr->TotalCP - 1; i++)
	{
		if (fabs_(m_ServoAxisTime[i]) > EPSINON)
		{
			//figure out dose speed
			m_PlanBufferPtr->BackCalSpeedPtr->Dose[i] = m_PlanBufferPtr->DeltaPtr->DoseWeight[i] / m_ServoAxisTime[i];		//MU/s

			//figure out gantry speed
			UINT16 TempResult = 0;
			TempResult = (UINT16)((m_PlanBufferPtr->DeltaPtr->GantryAngle[i] / m_ServoAxisTime[i]) * 100);
			if ((TempResult %= 10) >= 5) //normalization
				m_PlanBufferPtr->BackCalSpeedPtr->Gantry[i] = static_cast<FLOAT>(m_PlanBufferPtr->DeltaPtr->GantryAngle[i] / m_ServoAxisTime[i] + 0.1);	//carry bit
			else
				m_PlanBufferPtr->BackCalSpeedPtr->Gantry[i] = static_cast<FLOAT>(m_PlanBufferPtr->DeltaPtr->GantryAngle[i] / m_ServoAxisTime[i]);		//truncation bit
			
			//figure out mlc speed
			for (size_t j = 0; j < MLC_LEAF_NUM; j++)
			{
				m_PlanBufferPtr->BackCalSpeedPtr->Mlc[i][j] = m_PlanBufferPtr->DeltaPtr->MlcPos[i][j] / m_ServoAxisTime[i];
			}
		}
		else
			return FALSE;
	}

	return TRUE;
}

#if 0
/******************************************************************************
* Function: GetGantrySpeed(VOID)
*
* Describe: figure out gantry setting speed
*
* Input   : None
*
* Output  : None
*******************************************************************************/
BOOL CPlanLinac::GetSpeedGantry(VOID)
{
	//figure out Gantry speed
	UINT16 TempResult = 0;
	for (size_t i = 0; i < m_PlanBufferPtr->BeamPlanPtr->TotalCP - 1; i++)
	{
		if (0 != m_ServoAxisTime[i])
		{
			TempResult = (UINT16)((m_PlanBufferPtr->DeltaPtr->GantryAngle[i] / m_ServoAxisTime[i]) * 100);
			//normalization
			if ((TempResult %= 10) >= 5)
				m_PlanBufferPtr->BackCalSpeedPtr->Gantry[i] = static_cast<FLOAT>(m_PlanBufferPtr->DeltaPtr->GantryAngle[i] / m_ServoAxisTime[i] + 0.1);	//carry bit
			else
				m_PlanBufferPtr->BackCalSpeedPtr->Gantry[i] = static_cast<FLOAT>(m_PlanBufferPtr->DeltaPtr->GantryAngle[i] / m_ServoAxisTime[i]);		//truncation bit
		}
		else
			return FALSE;
	}

	return TRUE;
}

/******************************************************************************
* Function: MatchDoseSpeed(VOID)
*
* Describe: match dose system speed
*
* Input   : None
*
* Output  : None
*******************************************************************************/
BOOL CPlanLinac::GetSpeedDose(VOID)
{
	if (0 != m_Tmax)
	{
		//figure out dose speed
		for (size_t i = 0; i < m_PlanBufferPtr->BeamPlanPtr->TotalCP - 1; i++)
		{
			m_PlanBufferPtr->BackCalSpeedPtr->Dose[i] = m_PlanBufferPtr->DeltaPtr->DoseWeight[i] / m_Tmax;		//MU/s
		}

		return TRUE;
	}
	else
	{
		return FALSE;
	}
}

/******************************************************************************
* Function: MatchMlcSpeed(VOID)
*
* Describe: match mlc speed
*
* Input   : None
*
* Output  : None
*******************************************************************************/
BOOL CPlanLinac::GetSpeedMlc(VOID)
{
	//figure out mlc speed
	for (size_t i = 0; i < m_PlanBufferPtr->BeamPlanPtr->TotalCP - 1; i++)
	{
		for (size_t j = 0; j < MLC_LEAF_NUM; j++)
		{
			if (0 != m_ServoAxisTime[i])
				m_PlanBufferPtr->BackCalSpeedPtr->Mlc[i][j] = m_PlanBufferPtr->DeltaPtr->MlcPos[i][j] / m_ServoAxisTime[i];
			else
				return FALSE;
		}
	}

	return TRUE;
}
#endif

/******************************************************************************
* Function: GetServoAxisDeltaTime(VOID)
*
* Describe: get servo axis delta time
*
* Input   : None
*
* Output  : None
*******************************************************************************/
BOOL CPlanLinac::GetServoAxisDeltaTime(VOID)
{
	if (Dose == m_CurrentServoType)
	{
		GetServoDoseDeltaTime();
	}
	else if (Gantry == m_CurrentServoType)
	{
		GetServoGantryDeltaTime();
	}
	else
	{
		return FALSE;
	}

	return TRUE;
}

#if 0
/******************************************************************************
* Function: GetServoAxisMinSpeed(VOID)
*
* Describe: get servo axis min speed
*
* Input   : None
*
* Output  : None
*******************************************************************************/
BOOL CPlanLinac::GetServoAxisMinSpeed(VOID)
{
	FLOAT MinSpeed = 0;
	//Find dose min speed
	if (m_PlanBufferPtr->BeamPlanPtr->TotalCP > 3)
	{
		MinSpeed = m_PlanBufferPtr->BackCalSpeedPtr->Dose[1];
		for (size_t i = 1; i < m_PlanBufferPtr->BeamPlanPtr->TotalCP - 2; i++)
		{
			if (m_PlanBufferPtr->BackCalSpeedPtr->Dose[i] <= MinSpeed)
				MinSpeed = m_PlanBufferPtr->BackCalSpeedPtr->Dose[i];
			else
				continue;
		}
	}
	else
	{
		MinSpeed = m_PlanBufferPtr->BeamPlanPtr->DoseRateSet/60;
	}
	
	//Set to all control point
	for (size_t i = 0; i < m_PlanBufferPtr->BeamPlanPtr->TotalCP - 1; i++)
		m_PlanBufferPtr->BackCalSpeedPtr->Dose[i] = MinSpeed;

	return TRUE;
}
#endif
/******************************************************************************
* Function: ClearPlanData(VOID)
*
* Describe: clear all plan data
*
* Input   : None
*
* Output  : None
*******************************************************************************/
VOID CPlanLinac::ClearPlanData(VOID)
{
	//memset(&m_MaxTime, 0, sizeof(m_MaxTime));
	//m_Tmax = 0;
	m_IntervalStep = 0;
	m_MinServoAxisSpeed = 0;
	memset(m_ServoAxisTime, 0, sizeof(m_ServoAxisTime));
	memset(m_AllAxisStepMaxTime, 0, sizeof(m_AllAxisStepMaxTime));
	m_CurrentServoType = None;
	m_TreatmentPlanType = PLAN_TYPE_NONE;
}

/******************************************************************************
* Function: InputsParameterIsReady(VOID)
*
* Describe: input parameters is ready
*
* Input   : None
*
* Output  : None
*******************************************************************************/
BOOL CPlanLinac::InputsParameterIsReady(VOID)
{
	if ((fabs_(m_PlanBufferPtr->ResComponentPtr->InputsVMAT->DoseMaxSpeed) <= EPSINON) || \
		(fabs_(m_PlanBufferPtr->ResComponentPtr->InputsVMAT->GantryMaxSpeed) <= EPSINON) || \
		(fabs_(m_PlanBufferPtr->ResComponentPtr->InputsVMAT->MlcMaxSpeed) <= EPSINON))
	{
		m_PlanBufferPtr->ResComponentPtr->Trace->Log(tlInfo, "Line:%d::CPlanLinac::<GetMaxTimeDose>::Parameter is 0:Dose:%f Gantry:%f Mlc:%f", \
			__LINE__, m_PlanBufferPtr->ResComponentPtr->InputsVMAT->DoseMaxSpeed, \
			m_PlanBufferPtr->ResComponentPtr->InputsVMAT->GantryMaxSpeed, \
			m_PlanBufferPtr->ResComponentPtr->InputsVMAT->MlcMaxSpeed);

		return FALSE;	//has invaild parameter
	}
	else
	{
		return TRUE;	//input parameter is ready
	}
}

/******************************************************************************
* Function: GetCurrentServoAxis(VOID)
*
* Describe: get current servo axis
*
* Input   : None
*
* Output  : None
*******************************************************************************/
BOOL CPlanLinac::GetCurrentServoAxis(VOID)
{
	//Figure out dose and gantry speed with Tmax
	
	for (size_t i = 0; i < m_PlanBufferPtr->BeamPlanPtr->TotalCP - 1; i++)
	{
		if (fabs_(m_AllAxisStepMaxTime[i]) > EPSINON)
		{
			//figure out dose speed
			m_PlanBufferPtr->BackCalSpeedPtr->Dose[i] = m_PlanBufferPtr->DeltaPtr->DoseWeight[i] / m_AllAxisStepMaxTime[i];		//MU/s

			//figure out gantry speed
			//UINT16 TempResult = 0;
			//TempResult = (UINT16)((m_PlanBufferPtr->DeltaPtr->GantryAngle[i] / m_AllAxisStepMaxTime[i]) * 100);
			//if ((TempResult %= 10) >= 5) //normalization
			//	m_PlanBufferPtr->BackCalSpeedPtr->Gantry[i] = static_cast<FLOAT>(m_PlanBufferPtr->DeltaPtr->GantryAngle[i] / m_AllAxisStepMaxTime[i]+0.1 );	//carry bit
			//else
			m_PlanBufferPtr->BackCalSpeedPtr->Gantry[i] = static_cast<FLOAT>(m_PlanBufferPtr->DeltaPtr->GantryAngle[i] / m_AllAxisStepMaxTime[i]);		//truncation bit
		}
	}
	

	//Find out current servo axis and treatment plan type
	UINT16 TempCounter = 0;
	for (size_t i = 0; i < m_PlanBufferPtr->BeamPlanPtr->TotalCP - 1; i++)
		TempCounter += static_cast<UINT16>(m_PlanBufferPtr->BackCalSpeedPtr->Gantry[i]);
	if (0 == TempCounter)
	{
		m_CurrentServoType = Dose;
		m_TreatmentPlanType = PLAN_TYPE_SLIDING_WINDOW;
	}
	else
	{
		m_CurrentServoType = Gantry;
		m_TreatmentPlanType = PLAN_TYPE_VMAT;
	}
		

	return TRUE;
}

/******************************************************************************
* Function: GetServoDoseDeltaTime(VOID)
*
* Describe: get dose servo axis delta time
*
* Input   : None
*
* Output  : None
*******************************************************************************/
BOOL CPlanLinac::GetServoDoseDeltaTime(VOID)
{	//Figure out min dose speed
	m_MinServoAxisSpeed = m_PlanBufferPtr->BackCalSpeedPtr->Dose[0];
	if (m_PlanBufferPtr->BeamPlanPtr->TotalCP > 3)
	{
		m_MinServoAxisSpeed = m_PlanBufferPtr->BackCalSpeedPtr->Dose[1];
		for (size_t i = 0; i < m_PlanBufferPtr->BeamPlanPtr->TotalCP - 1; i++) //first and last cp not include
		{
			if (m_MinServoAxisSpeed >= m_PlanBufferPtr->BackCalSpeedPtr->Dose[i])
				m_MinServoAxisSpeed = m_PlanBufferPtr->BackCalSpeedPtr->Dose[i];
			else
				continue;
		}
	}
	else
	{
		if (fabs_(m_PlanBufferPtr->BeamPlanPtr->DoseRateSet) <= EPSINON)
		{
			return FALSE;
		}
		else
		{
			m_MinServoAxisSpeed = m_PlanBufferPtr->BeamPlanPtr->DoseRateSet / 60; //support QuickBeam, use plan dose rate
		}
	}

	//Set min speed to all dose cp
	for (size_t i = 0; i < m_PlanBufferPtr->BeamPlanPtr->TotalCP - 1; i++)
		m_PlanBufferPtr->BackCalSpeedPtr->Dose[i] = m_MinServoAxisSpeed;

	//Figure out delta time
	for (size_t i = 0; i < m_PlanBufferPtr->BeamPlanPtr->TotalCP - 1; i++)
	{
		if (fabs_(m_PlanBufferPtr->BackCalSpeedPtr->Dose[i]) > EPSINON)
			m_ServoAxisTime[i] = m_PlanBufferPtr->DeltaPtr->DoseWeight[i] / m_PlanBufferPtr->BackCalSpeedPtr->Dose[i];
		else
			return FALSE;
	}

	return TRUE;
}

/******************************************************************************
* Function: GetServoGantryDeltaTime(VOID)
*
* Describe: get gantry servo axis delta time
*
* Input   : None
*
* Output  : None
*******************************************************************************/
BOOL CPlanLinac::GetServoGantryDeltaTime(VOID)
{
	//Figure out min Gantry speed
	m_MinServoAxisSpeed = m_PlanBufferPtr->BackCalSpeedPtr->Gantry[0];
	for (size_t i = 0; i < m_PlanBufferPtr->BeamPlanPtr->TotalCP - 1; i++) //first and last cp not include
	{
		if (m_MinServoAxisSpeed >= m_PlanBufferPtr->BackCalSpeedPtr->Gantry[i])
			m_MinServoAxisSpeed = m_PlanBufferPtr->BackCalSpeedPtr->Gantry[i];
			else
				continue;
	}
	//Set min speed to all dose cp
	for (size_t i = 0; i < m_PlanBufferPtr->BeamPlanPtr->TotalCP - 1; i++)
		m_PlanBufferPtr->BackCalSpeedPtr->Gantry[i] = m_MinServoAxisSpeed;

	//Figure out delta time
	for (size_t i = 0; i < m_PlanBufferPtr->BeamPlanPtr->TotalCP - 1; i++)
	{
		if (fabs_(m_PlanBufferPtr->BackCalSpeedPtr->Gantry[i]) > EPSINON)
			m_ServoAxisTime[i] = m_PlanBufferPtr->DeltaPtr->GantryAngle[i] / m_PlanBufferPtr->BackCalSpeedPtr->Gantry[i];
		else
			return FALSE;
	}

	return TRUE;
}

/******************************************************************************
* Function: GetServoAxisType
*
* Describe: get gantry servo axis delta time
*
* Input   : None
*
* Output  : None
*******************************************************************************/
ServoType_eu CPlanLinac::GetServoAxisType( VOID)
{
	return m_CurrentServoType;
}

/******************************************************************************
* Function: GetProtectTime(VOID)
*
* Describe: 
*
* Input   : None
*
* Output  : None
*******************************************************************************/
LONGLONG CPlanLinac::GetProtectTime(VOID)
{
	FLOAT TotalTime = 0;
	for (size_t i = 0; i < m_PlanBufferPtr->BeamPlanPtr->TotalCP - 1; i++)
	{
		TotalTime += m_ServoAxisTime[i];
	}

	TotalTime *= static_cast<FLOAT>(1.2);		//120%   unit:s
	
	if (TotalTime < 6)
		TotalTime = 6;
	else{ ; }

	return static_cast<LONGLONG>(TotalTime*1000);
}

/******************************************************************************
* Function: GetDeltaTimeCp
*
* Describe: get gantry servo axis delta time
*
* Input   : None
*
* Output  : None
*******************************************************************************/
FLOAT* CPlanLinac::GetDeltaTimeCp(VOID)
{
	return m_ServoAxisTime;
}
