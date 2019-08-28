/*******************************************************************************
** File	       : ResourceFsm.h
** Author      : LiBo
** Version     : 1.0
** Environment : VS2013
** Description :
** History     : 18 / 08 / 07    LiBo    Created
********************************************************************************/
/*==== DECLARATION CONTROL ====================================================*/
#pragma once

/*==== INCLUDES ==============================================================*/
#include "TcDef.h"
#include "SoftTimer.h"
/*==== ENUMS =================================================================*/
//State machine define
enum FsmState_u{
	RTC_NO_STATE = 0,
	RTC_INIT = 1,
	RTC_STANDBY = 2,
	RTC_PREPARATORY = 3,
	RTC_PREPARE = 4,
	RTC_READY = 5,
	RTC_RADIATION = 6,
	RTC_REPOSITION = 7,
	RTC_COMPLETE = 8,
	RTC_INTERRUPT = 9,
	RTC_INTERRUPTREADY = 10,
	RTC_TERMINATE = 11,
	RTC_OFF = 12,
	RTC_SHUTDOWN = 13,
	SERVO_STATE = 14,
	MANUAL_STATE = 15,
	FAULT_STATE = 99
};

enum FsmRunState_u{
	FSM_RUNNING = 1,
	FSM_HOLDING = 2,
};

/*==== STRUCT ================================================================*/
//struct define for GUI command------------------------------------------------
struct GuiCmd_t{
	BOOL Reset;
	BOOL Clinical;
	BOOL Physics;
	BOOL Service;
	BOOL DailyQA;
	BOOL ClosePatient;
	BOOL Login;
	BOOL Logout;
	BOOL NextBeam;
	BOOL Continue;
	BOOL StopRadiation;
	BOOL ExceptionBeamOff;
	BOOL PrepareState;
	BOOL RadiationState;
	BOOL KvImage;
	BOOL TCSNotready;
	BOOL TCSReady;
	BOOL OffState;
	BOOL ShutDown;
	BOOL CleanPlan;
	BOOL IgsCTNotReady;
	BOOL IgsCTReady;
};

class CFsmResource
{
public:
	CFsmResource();
	virtual ~CFsmResource();
	VOID	SetCurrentState(FsmState_u CurrentState);
	FsmState_u	GetCurrentState(VOID);

	GuiCmd_t GuiCmd;
	BYTE     GuiCurrentMode;
	BOOL IsReady(VOID);
	BOOL RadiationIsTimeOut(VOID);
	VOID SetRadiationTimeOut(BOOL State);
	FsmState_u m_CurrentState;
	
private:
	BOOL m_RadiationTimeOut;
	
};
