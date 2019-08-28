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
/*==== DECLARATION CONTROL ===================================================*/
#ifndef __SOFTTIMER_H__
#define __SOFTTIMER_H__

/*==== INCLUDES ==============================================================*/
#include "TcDef.h"
/*==== MACROS ================================================================*/
/*==== STRUCTS ===============================================================*/
/*==== CLASS =================================================================*/
class CSoftTimer{
public:
	CSoftTimer();
	virtual ~CSoftTimer();

	//functions
	BOOL ResetTimer(VOID);
	BOOL StartTimer(const LONGLONG CurrentSysTime, const LONGLONG Duration);
	BOOL IsReady(VOID);
	BOOL IsTimeOut(const LONGLONG CurrentSysTime);
	LONGLONG GetRunTime(const LONGLONG CurrentSysTime);    //Getting time is current time minus setting time 

protected:
	//variables
	BOOL m_InitEn;				//initialize timer
	LONGLONG m_StartTime;
	LONGLONG m_SettingTime;
};

#endif
/*==== END OF FILE ===========================================================*/