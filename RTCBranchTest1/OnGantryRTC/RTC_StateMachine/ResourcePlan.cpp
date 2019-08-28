/*******************************************************************************
** File	       : ResourcePlan.cpp
** Author      : LiBo
** Version     : 1.0
** Environment : VS2013
** Description :
** History     : 18 / 08 / 07    LiBo    Created
********************************************************************************/
/* INCLUDE ===================================================================*/
#include "ResourcePlan.h"

/* FUNCTIONS =================================================================*/
/******************************************************************************
* Function: CPlanResource()
*
* Describe:
*
* Input   :
*
* Output  : None
*******************************************************************************/
CPlanResource::CPlanResource()
{
	m_PlanReadyFlag = FALSE;
	m_PlanCPTotal = 0;
	m_OriginalPlanPtr = NULL;
	m_SpeedPtr = NULL;
	m_BeamPlanPtr = NULL;
	m_DoseTotal = 0;
	m_ServoAxisType = None;
	m_DeltaTimeCp = NULL;
	m_RadiationProtTime = 0;
}

CPlanResource::~CPlanResource()
{
	//variables

}

/******************************************************************************
* Function: PlanIsReady()
*
* Describe:
*
* Input   :
*
* Output  : None
*******************************************************************************/
BOOL CPlanResource::PlanIsReady(VOID)
{
	return m_PlanReadyFlag;
}

/******************************************************************************
* Function: SetPlanCPTotal(UINT16 PlanCPTotal)
*
* Describe:
*
* Input   :
*
* Output  : None
*******************************************************************************/
BOOL CPlanResource::SetPlanCPTotal(UINT16 PlanCPTotal)
{
	m_PlanCPTotal = PlanCPTotal;

	return TRUE;
}

/******************************************************************************
* Function: SetPlanReady(BOOL Ready)
*
* Describe:
*
* Input   :
*
* Output  : None
*******************************************************************************/
BOOL CPlanResource::SetPlanReady(BOOL Ready)
{
	m_PlanReadyFlag = Ready;			//set flag

	return TRUE;
}

/******************************************************************************
* Function: GetPlanCPTotal(BOOL Ready)
*
* Describe:
*
* Input   :
*
* Output  : None
*******************************************************************************/
UINT16 CPlanResource::GetPlanCPTotal(VOID)
{
	return m_PlanCPTotal;
}

/******************************************************************************
* Function: GetOriginalPlan(VOID)
*
* Describe:
*
* Input   :
*
* Output  : None
*******************************************************************************/
OriginalCP_t* CPlanResource::GetOriginalPlan(VOID)
{
	return m_OriginalPlanPtr;
}

/******************************************************************************
* Function: BindingPtrOriginalPlan(OriginalCP_t* StructArray)
*
* Describe:
*
* Input   :
*
* Output  : None
*******************************************************************************/
VOID CPlanResource::BindingPtrOriginalPlan(OriginalCP_t* StructArray)
{
	m_OriginalPlanPtr = StructArray;
	return;
}

/******************************************************************************
* Function: GetSpeedData(VOID)
*
* Describe:
*
* Input   :
*
* Output  : None
*******************************************************************************/
BackCalSpeedCP_t* CPlanResource::GetSpeedData(VOID)
{
	return m_SpeedPtr;
}

/******************************************************************************
* Function:  SetSpeedData(FLOAT* SpeedArray)
*
* Describe:
*
* Input   :
*
* Output  : None
*******************************************************************************/
VOID CPlanResource::BindingPtrSpeedData(BackCalSpeedCP_t* StructPtr)
{
	m_SpeedPtr = StructPtr;
	return;
}

/******************************************************************************
* Function:  SetDoseTotal(FLOAT Dose)
*
* Describe:
*
* Input   :
*
* Output  : None
*******************************************************************************/
VOID CPlanResource::SetDoseTotal(FLOAT Dose)
{
	m_DoseTotal = Dose;
	return;
}

/******************************************************************************
* Function:  GetDoseTotal(VOID)
*
* Describe:
*
* Input   :
*
* Output  : None
*******************************************************************************/
FLOAT CPlanResource::GetDoseTotal(VOID)
{
	 return m_DoseTotal;
}
/******************************************************************************
* Function:  SetServoAxisTyge(BYTE ServoAxisTyge)
* Describe:
*
* Input   :
*
* Output  : None
*******************************************************************************/

VOID CPlanResource::SetServoAxisType(ServoType_eu ServoAxisTyge)
{
	m_ServoAxisType = ServoAxisTyge;
}

/******************************************************************************
* Function:  GetServoAxisTyge(VOID)
* Describe:
*
* Input   :
*
* Output  : None
*******************************************************************************/
ServoType_eu CPlanResource::GetServoAxisType(VOID)
{
	return m_ServoAxisType ;
}

/******************************************************************************
* Function:  GetRadiationProtTime(VOID)
* Describe:
*
* Input   :
*
* Output  : None
*******************************************************************************/
LONGLONG CPlanResource::GetRadiationProtTime(VOID)
{
	return m_RadiationProtTime;
}

/******************************************************************************
* Function:  SetRadiationProtTime(FLOAT Time)
* Describe:
*
* Input   :
*
* Output  : None
*******************************************************************************/
VOID CPlanResource::SetRadiationProtTime(LONGLONG Time)
{
	m_RadiationProtTime = Time;
	return;
}


/******************************************************************************
* Function:  GetDeltaTimeCp(VOID)
* Describe:
*
* Input   :
*
* Output  : None
*******************************************************************************/
FLOAT* CPlanResource::GetDeltaTimeCp(VOID)
{
	
	return m_DeltaTimeCp ;
}

/******************************************************************************
* Function: SetDeltaTimeCp(FLOAT Ptr)
*
* Describe:
*
* Input   :
*
* Output  : None
*******************************************************************************/
VOID CPlanResource::BindingPtrDeltaTimeCp(FLOAT* Ptr)
{
	m_DeltaTimeCp = Ptr;	
}

/******************************************************************************
* Function:  SetBeamPlanData(BeamPlan_t* BeamPlan)
* Describe:
*
* Input   :
*
* Output  : None
*******************************************************************************/
VOID CPlanResource::BindingPtrBeamPlanData(BeamPlan_t* BeamPlan)
{
	m_BeamPlanPtr = BeamPlan;
}

/******************************************************************************
* Function:  GetBeamPlanData(VOID)
* Describe:
*
* Input   :
*
* Output  : None
*******************************************************************************/
BeamPlan_t* CPlanResource::GetBeamPlanData(VOID)
{
	return m_BeamPlanPtr;
}

/******************************************************************************
* Function:  GetBeamPlanData(VOID)
*
* Describe:
*
* Input   :
*
* Output  : None
*******************************************************************************/
PlanEvent_eu CPlanResource::GetPlanEvent(VOID)
{
	return m_PlanEvent;
}

/******************************************************************************
* Function:  SetPlanEvent(PlanEvent_eu Evt)
*
* Describe:
*
* Input   :
*
* Output  : None
*******************************************************************************/
VOID CPlanResource::SetPlanEvent(PlanEvent_eu Evt)
{
	m_PlanEvent = Evt;
	return;
}
