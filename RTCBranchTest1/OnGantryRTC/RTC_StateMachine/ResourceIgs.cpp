/*******************************************************************************
** File	      : ResourceIgs.cpp
** Author : : libo
** Version : 1.0
** Environment : VS2013
** Description :
** History : 18 / 08 / 04    libo   Created
******************************************************************************* /
/* INCLUDE ===================================================================*/
#include "ResourceIgs.h"
/* METHODS===================================================================*/
/******************************************************************************
* Function: CResourceIGs()
*
* Describe: construct function
*
* Input   : None
*
* Output  : None
*
* Return  :
*
* Others  :
*******************************************************************************/
CResourceIgs::CResourceIgs()
{
	m_CurrentState = IGS_INITIAL;
	m_Interlock = 0;
	m_InitialState = FALSE;
	m_Notready = 0;
	m_Ready = FALSE;
	
}
/******************************************************************************
* Function: ~CResourceIGs()
*
* Describe: Destructor
*
* Input   : None
*
* Output  : None
*
* Return  :
*
* Others  :
*******************************************************************************/
CResourceIgs::~CResourceIgs()
{
	
}
/******************************************************************************
* Function:SetCurrentState()
*
* Describe: 
*
* Input   : None
*
* Output  : None
*
* Return  :
*
* Others  :
*******************************************************************************/
VOID CResourceIgs::SetCurrentState(IgsState_eu CurrentState)
{
	m_CurrentState = CurrentState;
}

/******************************************************************************
* Function:GetCurrentState()
*
* Describe: 
*
* Input   : None
*
* Output  : None
*
* Return  :
*
* Others  :
*******************************************************************************/
IgsState_eu CResourceIgs::GetCurrentState(VOID)
{
	return m_CurrentState;
}

/******************************************************************************
* Function : Getinterlock()
*
* Describe : Destructor
*
* Input : None
*
* Output : None
*******************************************************************************/
UINT32  CResourceIgs::Getinterlock()
{
	return m_Interlock;
}
/******************************************************************************
* Function : Getinterlock()
*
* Describe : Destructor
*
* Input : None
*
* Output : None
*******************************************************************************/

VOID   CResourceIgs::SetInterlock(UINT32 Interelock)
{
	m_Interlock = Interelock;
}
/******************************************************************************
* Function : SetInitialState()
*
* Describe : Destructor
*
* Input : None
*
* Output : None
*******************************************************************************/
VOID CResourceIgs::SetInitialState(BOOL InitialState)
{
	m_InitialState = InitialState;
}
/******************************************************************************
* Function : InitialIsOk()
*
* Describe : Destructor
*
* Input : None
*
* Output : None
*******************************************************************************/
BOOL   CResourceIgs::InitialIsOk()                                             // judge  initial  is  OK
{
	return m_InitialState;
}
/******************************************************************************
* Function : SetFinished()
*
* Describe : Destructor
*
* Input : None
*
* Output : None
*******************************************************************************/


/******************************************************************************
* Function : SetReady()
*
* Describe : Destructor
*
* Input : None
*
* Output : None
*******************************************************************************/
VOID   CResourceIgs::SetReady(BOOL Ready)
{
	m_Ready = Ready;
}

/******************************************************************************
* Function : SetReady()
*
* Describe : Destructor
*
* Input : None
*
* Output : None
*******************************************************************************/
BOOL   CResourceIgs::IsReady()                                               // igs is ready
{
	return  m_Ready;
}
/******************************************************************************
* Function : Getinterlock()
*
* Describe : Destructor
*
* Input : None
*
* Output : None
*******************************************************************************/
UINT32  CResourceIgs::GetNotReady()
{
	return m_Notready;
}
/******************************************************************************
* Function : Getinterlock()
*
* Describe : Destructor
*
* Input : None
*
* Output : None
*******************************************************************************/

VOID   CResourceIgs::SetNotReady(UINT32 Notready)
{
	m_Notready = Notready;
}
