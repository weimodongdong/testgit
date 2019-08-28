/*******************************************************************************
** File	      : ResourceBsm.cpp
** Author:    : XuYun
** Version    :	1.0
** Environment: VS2013
** Description:
** History    :	18/12/04    XuYun    Created
*******************************************************************************/
/* INCLUDE ===================================================================*/
#include "ResourceBsm.h"

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
CResourceBsm::CResourceBsm()
{
	m_PlanTransFlag = FALSE;
	m_Interlock = 0;
	m_Unready = 0;
}


CResourceBsm::~CResourceBsm()
{
}

/******************************************************************************
* Function: SetPlanTransCompleted(BOOL Completed)
*
* Describe: set plan transition is completed or not
*
* Input   : None
*
* Output  : None
*******************************************************************************/
VOID CResourceBsm::SetPlanTransCompleted(BOOL Completed)
{
	m_PlanTransFlag = Completed;
}

/******************************************************************************
* Function: PlanTransCompleted(VOID)
*
* Describe: plan transition is completed or not
*
* Input   : None
*
* Output  : None
*******************************************************************************/
BOOL CResourceBsm::PlanTransCompleted(VOID)
{
	return m_PlanTransFlag;
}

/******************************************************************************
* Function: SetInterlock(BOOL Completed)
*
* Describe: set Bsm Interlock
*
* Input   : None
*
* Output  : None
*******************************************************************************/
BOOL CResourceBsm::SetInterLock(UINT64 Interlock)
{
	m_Interlock = Interlock;

	return TRUE;
}

/******************************************************************************
* Function: GetInterlock(BOOL Completed)
*
* Describe: set Bsm Interlock
*
* Input   : None
*
* Output  : None
*******************************************************************************/
UINT64 CResourceBsm::GetInterlock(VOID)
{
	return m_Interlock;
}

/******************************************************************************
* Function: SetUnready(BOOL Completed)
*
* Describe: set Bsm not ready
*
* Input   : None
*
* Output  : None
*******************************************************************************/
BOOL CResourceBsm::SetUnready(UINT16 Unready)
{
	m_Unready = Unready;

	return TRUE;
}

/******************************************************************************
* Function: GetInterlock(BOOL Completed)
*
* Describe: set Bsm Interlock
*
* Input   : None
*
* Output  : None
*******************************************************************************/
UINT16 CResourceBsm::GetUnready(VOID)
{
	return m_Unready;
}

/******************************************************************************
* Function: GetBSMInitCompleteFlag(BOOL Completed)
*
* Describe: Get Bsm Init complete flat
*
* Input   : None
*
* Output  : None
*******************************************************************************/
BOOL CResourceBsm::GetBSMInitCompleteFlag(VOID)
{
	return m_BsmInitCompleteFlag;
}
/******************************************************************************
* Function: SetBsmInitCompleteFlag(BOOL Completed)
*
* Describe: Get Bsm Init complete flat
*
* Input   : None
*
* Output  : None
*******************************************************************************/
VOID CResourceBsm::SetBsmInitCompleteFlag(BOOL flag)
{
	m_BsmInitCompleteFlag = flag;
}
