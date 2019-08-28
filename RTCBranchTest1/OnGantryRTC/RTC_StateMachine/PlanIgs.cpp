/*******************************************************************************
** File	      : PlanIgs.cpp
** Author:    : ChenLong
** Version    :	1.0
** Environment: VS2013
** Description:
** History    :	18/10/16    Chenlong    Created
				19/02/28    Chenlong    Modify fixed speed with CBCT case
*******************************************************************************/
/* INCLUDE ===================================================================*/
#include "PlanIgs.h"

/* METHODS===================================================================*/
/******************************************************************************
* Function: CPlanIgs(HandlerInputs_t* HandlerInputs)
*
* Describe: construct function
*
* Input   : None
*
* Output  : None
*******************************************************************************/
CPlanIgs::CPlanIgs(HandlerInputs_t* HandlerInputs)
{
	m_PlanBufferPtr = HandlerInputs;
	m_PlanType = PLAN_TYPE_NONE;
	m_IntervalStep = 0;
}

CPlanIgs::~CPlanIgs()
{
}

/******************************************************************************
* Function: GetDeltaGantry(VOID)
*
* Describe: get delta value of gantry
*
* Input   : None
*
* Output  : None
*******************************************************************************/
VOID CPlanIgs::GetDeltaGantry(VOID)
{
	for (size_t i = 0; i < m_PlanBufferPtr->BeamPlanPtr->TotalCP - 1; i++)
	{
		//calculate delta value of Gantry wiht gantry direction
		FLOAT TempGantryPosition = 0;
		if (m_PlanBufferPtr->OriginalPlanPtr[i].GantryDir == CW)
		{
			TempGantryPosition = static_cast<FLOAT>(fmod_((m_PlanBufferPtr->OriginalPlanPtr[i + 1].GantryAngle - m_PlanBufferPtr->OriginalPlanPtr[i].GantryAngle + 360.0), 360.0));
		}
		else if (m_PlanBufferPtr->OriginalPlanPtr[i].GantryDir == CCW)
		{
			TempGantryPosition = static_cast<FLOAT>(360.0 - (fmod_((m_PlanBufferPtr->OriginalPlanPtr[i + 1].GantryAngle - m_PlanBufferPtr->OriginalPlanPtr[i].GantryAngle + 360.0), 360.0)));
		}
		else
		{ 
			//do nothing
		}

		if (fabs_(TempGantryPosition - 360.0) <= EPSINON)
		{
			m_PlanBufferPtr->DeltaPtr->GantryAngle[i] = 0;
		}
		else
		{
			m_PlanBufferPtr->DeltaPtr->GantryAngle[i] = TempGantryPosition;
		}
	}
}

/******************************************************************************
* Function: HandlePlanData(VOID)
*
* Describe: handle plan of gantry
*
* Input   : None
*
* Output  : None
*******************************************************************************/
BOOL CPlanIgs::HandlePlanData(VOID)
{
	if (NULL != m_PlanBufferPtr)
	{
		//Extend radiation index
		ExtendRadiationIndex();
		
		//Get delta value of gantry angle
		GetDeltaGantry();
		
		//Get current plan type and set fixed gantry speed
		PlanType_eu planIGS = GetCurrentPlanType();
		if (PLAN_TYPE_KV_KV == planIGS)
		{
			m_PlanType = PLAN_TYPE_KV_KV;
			memset(m_PlanBufferPtr->BackCalSpeedPtr->Gantry, 0, sizeof(m_PlanBufferPtr->BackCalSpeedPtr->Gantry)); //set gantry all speed 0
		}
		else if (PLAN_TYPE_CBCT == planIGS)
		{
			m_PlanType = PLAN_TYPE_CBCT;
			for (UINT16 i = 0; i < m_PlanBufferPtr->BeamPlanPtr->TotalCP - 1; i++)
			{
				m_PlanBufferPtr->BackCalSpeedPtr->Gantry[i] = CBCT_FIXED_SPEED; //used with fixed valie
			}
		}
	}

	return TRUE;
}

/******************************************************************************
* Function: ClearPlanData(VOID)
*
* Describe: clear all plan data
*
* Input   : None
*
* Output  : None
*******************************************************************************/
VOID CPlanIgs::ClearPlanData(VOID)
{
	m_PlanType = PLAN_TYPE_NONE;

	return;
}

/******************************************************************************
* Function: GetCurrentPlanType(VOID)
*
* Describe: get current igs plan type
*
* Input   : None
*
* Output  : None
*******************************************************************************/
PlanType_eu CPlanIgs::GetCurrentPlanType(VOID)
{
	for (UINT16 i = 0; i < m_PlanBufferPtr->BeamPlanPtr->TotalCP - 1; i++)
	{
		if (fabs_(m_PlanBufferPtr->DeltaPtr->GantryAngle[i] - 0.0) <= EPSINON) //equal 0
		{
			continue;
		}
		else
		{
			return PLAN_TYPE_CBCT;
		}
	}
		
	return PLAN_TYPE_KV_KV;
}

/******************************************************************************
* Function: ExtendRadiationIndex(VOID)
*
* Describe: extend radiation index
*
* Input   : None
*
* Output  : None
*******************************************************************************/
VOID CPlanIgs::ExtendRadiationIndex(VOID)
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

