/*******************************************************************************
** File	       : ResourcePlan.cpp
** Author      : LiBo
** Version     : 1.0
** Environment : VS2013
** Description :
** History     : 18 / 08 / 07    LiBo    Created
********************************************************************************/
/* INCLUDE ===================================================================*/
#include "ResourceFsm.h"

/* FUNCTIONS =================================================================*/
/******************************************************************************
* Function: CFsmResource()
*
* Describe:
*
* Input   :
*
* Output  : None
*******************************************************************************/
CFsmResource::CFsmResource()
{
	//init com resources
	memset(&GuiCmd, 0, sizeof(GuiCmd));
	GuiCurrentMode = 0;
	m_RadiationTimeOut = FALSE;
}

CFsmResource::~CFsmResource()
{
	//variables
}

/******************************************************************************
* Function: IsReady(VOID)
*
* Describe:
*
* Input   :
*
* Output  :
*******************************************************************************/
BOOL CFsmResource::IsReady(VOID)
{
	return TRUE;
}

/******************************************************************************
* Function: RadiationIsTimeOut(VOID)
*
* Describe:
*
* Input   :
*
* Output  :
*******************************************************************************/
BOOL CFsmResource::RadiationIsTimeOut(VOID)
{
	return m_RadiationTimeOut;
}

/******************************************************************************
* Function: SetRadiationTimeOut(BOOL State)
*
* Describe:
*
* Input   :
*
* Output  :
*******************************************************************************/
VOID CFsmResource::SetRadiationTimeOut(BOOL State)
{
	m_RadiationTimeOut = State;
	return;
}

/******************************************************************************
* Function: SetCurrentState(VOID)
*
* Describe:
*
* Input   :
*
* Output  :
*******************************************************************************/
VOID CFsmResource::SetCurrentState(FsmState_u CurrentState)
{
	m_CurrentState = CurrentState;
}

/******************************************************************************
* Function: GetCurrentState(VOID)
*
* Describe:
*
* Input   :
*
* Output  :
*******************************************************************************/
FsmState_u CFsmResource::GetCurrentState(VOID)
{
	return m_CurrentState;
}

