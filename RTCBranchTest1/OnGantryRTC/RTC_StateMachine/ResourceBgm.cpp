/*******************************************************************************
** File	      : ResourceBgm.cpp
** Author:    : ChenLong
** Version    :	1.0
** Environment: VS2013
** Description:
** History    :	18/01/30    Chenlong    Created
*******************************************************************************/
/* INCLUDE ===================================================================*/
#include "ResourceBgm.h"

/* METHODS===================================================================*/
/******************************************************************************
* Function: CResourceBgm()
*
* Describe: construct function
*
* Input   : None
*
* Output  : None
*******************************************************************************/
CResourceBgm::CResourceBgm()
{
	m_DoseTransFlag = FALSE;
}


CResourceBgm::~CResourceBgm()
{
}

/******************************************************************************
* Function: PlanTransCompleted(VOID)
*
* Describe: dose transition is completed
*
* Input   : None
*
* Output  : None
*******************************************************************************/
BOOL CResourceBgm::PlanTransCompleted(VOID)
{
	return m_DoseTransFlag;
}

/******************************************************************************
* Function: SetPlanBgmIsReady(BOOL Completed)
*
* Describe: set dose transition is completed or not
*
* Input   : None
*
* Output  : None
*******************************************************************************/
VOID CResourceBgm::SetPlanBgmIsReady(BOOL Completed)
{
	m_DoseTransFlag = Completed;
}