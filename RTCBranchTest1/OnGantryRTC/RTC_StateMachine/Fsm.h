/*******************************************************************************
** File	      : FSM.h
** Author     : ChenLong
** Version    :	1.0
** Environment: VS2013
** Description:
** History    :	18/01/30    Chenlong    Created
*******************************************************************************/
/*==== DECLARATION CONTROL ===================================================*/
#pragma once

/*==== INCLUDES ==============================================================*/
#include "ResourceMngr.h"

/*==== MACROS ================================================================*/
#define RTC_DEBUG_EN			1
#if RTC_DEBUG_EN
#define	RTC_DEBUG_EXCEPTION		1
#define FSM_DEBUG_FALL_EN		1
#define FSM_DEBUG_RAISE_EN		0
#endif

//FSM state sync time define : unit ms
#define		INIT_WAIT_TIME				400
#define		STANDBY_WAIT_TIME			400
#define		PREPARATORY_WAIT_TIME		400
#define		PREPARE_WAIT_TIME			600
#define		READY_WAIT_TIME				4000
#define		RADIATION_WAIT_TIME			2000
#define		REPOSITION_WAIT_TIME		400
#define		INTERRUPT_WAIT_TIME			400
#define		INTERRUPTREADY_WAIT_TIME	4000
#define		TERMINATE_WAIT_TIME			400

/*==== STRUCTS ===============================================================*/
struct SubFsmRequiredState_t{
	BYTE BgmReqState[2][17];
};

/*==== ENUMS =================================================================*/
enum FsmStateType_t{
	FsmTypeMv = 0,
	FsmTypeKv = 1,
};

enum SyncCMD_u{
	FKP_CMD_NONE = 0,
	FKP_CMD_BEAM_REVIEW = 1,
};

/*==== CLASS =================================================================*/
class CFsm
{
public:
	CFsm();
	CFsm(CResourceMngr* Resource);
	virtual ~CFsm();
	//methods
	VOID CycleUpdate(VOID); //invoke interface
	VOID SetRunState(FsmRunState_u RunState); //set fsm current run state
	VOID SetRequiredState(FsmState_u RequiredState); //set fsm required state
	VOID ResetStateSyncTimer(VOID);
protected:
	//Genernal methods 
	VOID  Initialize(VOID);
	VOID  ExecuteBusiness(VOID);
	BOOL  UpdateInputResource(VOID); //update compoent outputs
	virtual BOOL  UpdateOutputResource(VOID);

	//Methods for execute business
	FsmState_u PowerUpMain(VOID);
	FsmState_u InitMain(VOID);
	FsmState_u StandbyMain(VOID);
	FsmState_u PreparatoryMain(VOID);
	virtual FsmState_u PrepareMain(VOID);
	virtual FsmState_u ReadyMain(VOID); //subclass redefine
	virtual FsmState_u RadiationMain(VOID); //subclass redefine
	FsmState_u RepositionMain(VOID);
	FsmState_u CompleteMain(VOID);
	FsmState_u InterruptMain(VOID);
	FsmState_u InterruptReadyMain(VOID);
	FsmState_u TerminateMain(VOID);
	FsmState_u OffMain(VOID);
	FsmState_u ShutDownMain(VOID);
	
	BOOL  HardwareIsReady(VOID);
	FsmState_u  JumpToNextState(FsmState_u RequiredState);

	VOID  CheckSubFsmState(VOID); //check all subfsm state
	BOOL  CheckOGRFsmState(VOID);
	virtual VOID  CheckBSMFsmState(VOID); //check bsm fsm state
	virtual VOID  CheckBgmFsmState(VOID); //check bgm fsm state
	virtual VOID  CheckIgsFsmState(VOID); //check igs fsm state

	BOOL  HasInterlock(VOID);
	BOOL  SetInterlock(UINT16 Intl);
	BOOL  ResetInterlock(UINT16 Intl);
	
	BOOL  HasUnready(VOID);
	BOOL  SetUnready(UINT32 Unready);
	BOOL  ResetUnready(UINT32 Unready);

	BOOL  HasTerminate(VOID);
	BOOL  SetTerminate(UINT32 Terminate);
	BOOL  ResetTerminate(UINT32 Terminate);

	BOOL  OGRIsOk(VOID);
	VOID  InitSubFsmReqState(VOID);
	BYTE  GetBgmRequiredState(VOID);
	
	//Data members
	FsmState_u		m_CurrentState;
	FsmState_u		m_LastState;
	FsmState_u		m_RequiredState;
	CSoftTimer		m_StateSyncTimer; //timer for FSM state sync delay 


	CResourceMngr*	m_ResourceMngr; //customer resource pointer
	ResComponent_t*	m_ResComponent;
	CSoftTimer		m_RadiationProtTimer; //radiation protect timer
	FsmRunState_u   m_RunState; //fsm run state
	SubFsmRequiredState_t m_SubFsmReqState;
	BOOL			m_InitEn;
};

/*==== END OF FILE ===========================================================*/
