/*******************************************************************************
** File	      : PlanIgs.h
** Author     : ChenLong
** Version    :	1.0
** Environment: VS2013
** Description:
** History    :	19/01/29    Chenlong    Created
*******************************************************************************/
/*==== DECLARATION CONTROL ===================================================*/
#pragma once

/*==== INCLUDES ==============================================================*/
#include "PlanLinac.h"

/*==== ENUMS =================================================================*/
/*==== CONSTS ================================================================*/
const FLOAT CBCT_FIXED_SPEED = 6;

/*==== STRUCTS ===============================================================*/
/*==== CLASS =================================================================*/
class CPlanIgs
{
public:
	CPlanIgs(HandlerInputs_t* HandlerInputs);
	virtual ~CPlanIgs();
	//Interfaces
	BOOL	HandlePlanData(VOID);
	VOID	ClearPlanData(VOID);

private:
	//Data
	HandlerInputs_t*	m_PlanBufferPtr;
	PlanType_eu			m_PlanType;
	UINT16				m_IntervalStep;					//interval step after extend radiation index
	//Methods
	VOID				GetDeltaGantry(VOID); //figure out delta value of gantry
	PlanType_eu			GetCurrentPlanType(VOID); //get IGS current plan type
	VOID				ExtendRadiationIndex(VOID); //extend radiation index
};

