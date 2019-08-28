/*******************************************************************************
** File	      : SoftTimer.h
** Author     : ChenLong
** Version    :	2.2
** Environment: VS2013
** Description:
** History    :	17/11/17 Chenlong -Created
			  :	18/01/30 Chenlong -Convert read_value to ReadValue
			  :	18/07/19 LiBo Add -GetRunTimer()
			  :	18/07/30 ChenLong -Modify TimerReady() to IsReady()
			  : 18/08/03 ChenLong -InitEn to m_InitEn
*******************************************************************************/
/* INCLUDE ===================================================================*/
#include "SoftTimer.h"

/* GLOBAL VARS================================================================*/
/* FUNCTIONS =================================================================*/
/******************************************************************************
* Function: CSoftTimer()
*
* Describe:
*
* Input   : None
*
* Output  : None
*******************************************************************************/
CSoftTimer::CSoftTimer()
{
	m_StartTime = 0;
	m_SettingTime = 0;
	m_InitEn = TRUE;
}

CSoftTimer::~CSoftTimer()
{

}

/******************************************************************************
* Function: ResetTimer(VOID)
*
* Describe:
*
* Input   : None
*
* Output  : None
*******************************************************************************/
BOOL CSoftTimer::ResetTimer(VOID)
{
	m_SettingTime = 0;
	m_StartTime = 0;
	m_InitEn = TRUE;

	return TRUE;
}

/******************************************************************************
* Function: StartTimer(VOID)
*
* Describe: start timer
*
* Input   : None
*
* Output  : None
*******************************************************************************/
BOOL CSoftTimer::StartTimer(const LONGLONG CurrentSysTime, const LONGLONG Duration)
{
	m_StartTime = CurrentSysTime;
	m_SettingTime = Duration;
	m_InitEn = FALSE;

	return TRUE;
}

/******************************************************************************
* Function: TimerReady(VOID)
*
* Describe:
*
* Input   : None
*
* Output  : None
*******************************************************************************/
BOOL CSoftTimer::IsReady(VOID)
{
	return m_InitEn;	//ready if init_en is TRUE
}

/******************************************************************************
* Function: TimeOut(const LONGLONG current_sys_time)
*
* Describe:
*
* Input   : None
*
* Output  : None
*******************************************************************************/
BOOL CSoftTimer::IsTimeOut(const LONGLONG CurrentSysTime)
{
	if (FALSE == m_InitEn && CurrentSysTime - m_StartTime >= m_SettingTime)
		return TRUE;
	else
		return FALSE;
}

/******************************************************************************
* Function: GetRunTime(VOID)
*
* Describe: Getting time is current time minus setting time
*
* Input   : None
*
* Output  : None
*******************************************************************************/
LONGLONG CSoftTimer::GetRunTime(const LONGLONG CurrentSysTime)
{
	return CurrentSysTime - m_StartTime;
		
}