/*******************************************************************************
** File	      : Bsm.h
** Author     : XuYun
** Version    :	1.0
** Environment: VS2013
** Description:
** History    :	18/12/04    XuYun    Created
*******************************************************************************/
/*==== DECLARATION CONTROL ===================================================*/
#pragma once

/*==== INCLUDES ==============================================================*/
#include "ResourceMngr.h"

/*==== CONST =================================================================*/
const UINT32 BSM_DATA_BUS_IDLE = 0;
const UINT32 BSM_DATA_BUS_FB_OFFSET = 64;
const UINT32 BSM_DATA_BUS_FB_RESET = 0;
const UINT32 BSM_ACK_INTERVAL = 2000;	//time out test, ms
const UINT32 BSM_MAX_RESEND_TIME = 3;
const UINT32 BSM_DATA_BUS_DATA_LEN = 64;	//number of bank
const UINT32 BSM_BANK1_REQ = 1;			//transmit bank1 data requirement
const UINT32 BSM_BANK1_RSP = 17;		//bank1 succeed response signal :0x11
const UINT32 BSM_BANK2_REQ = 2;			//transmit bank1 data requirement
const UINT32 BSM_BANK2_RSP = 18;		//bank1 succeed response signal :0x12
const UINT32 BSM_STATE_INTERVAL = 1000;		//getting to required state max time

//cmd 

const UINT8 BSM_RESET = 0x01;
/*==== STRUCTS ===============================================================*/
struct BsmTransControl_t{	//signals for transition control
	UINT16 RadiationIndex;		//radiation step
	UINT16 BeamID;				//transition id:used for distinguish two plan transmission
	UINT16 CPIndex;				//control point index to be send
	UINT8  BankIndex;			//bank index of one cp
	UINT32 Crc32;
};

struct BsmTransState_t{	//variables for transition state
	UINT16 RetransCounter;
	BOOL PlanSendFinish;//plan data send to bgm finished
	BOOL AckTimeOut;
	BOOL SendFailed;
};

//struct DebugControl_t{
//	BOOL DummyEnable;	//enable construct dummy data for debug
//	BOOL TransIDEn;		//enable construct error transition id
//	BOOL Crc32En;		//enable construct error crc32
//};

//struct DoseCP_t{
//	UINT16 RadiationIndex;
//	FLOAT  OriginalData;
//	FLOAT  Speed;
//};

struct BsmPlanData_t
{	
	INT32	PlanData[MAX_CP_TOTAL][MLC_AXIS_NUM];//Bsm Plan Data	
	INT32	DeltaTime[MAX_CP_TOTAL];
	INT32	TotalTreatmentTime;
	UINT16	CPTotal;
	UINT16  LeafTolerance;
	UINT16  JawXTolerance;
	UINT16  JawYTolerance;
	UINT16  HeadTolerance;
};

/*==== ENUMS =================================================================*/
enum{	//error id
	BSM_ERROR_CRC = 0x13,
	BSM_ERROR_BEAM_ID = 0x14,
};

enum BsmPlanState_eu{	//BSM plan state
	BSM_TRANS_STATE_IDLE = 0,
	BSM_TRANS_STATE_SEND = 1,
	BSM_TRANS_STATE_ERROR = 2,
	BSM_TRANS_STATE_FINISH = 3,
};

/*==== CLASS =================================================================*/
class CBsm
{
public:
	CBsm(CResourceMngr* ResourceMngr);
	virtual ~CBsm();
	VOID CycleUpdate(VOID);						//invoke interface

private:
	//Data members
	CResourceMngr* m_ResourceMngr;
	ResComponent_t* m_ResComponent;
	BsmPlanState_eu m_PlanState;				//bgm plan current state
	BsmTransControl_t m_TransControl;
	BsmTransState_t m_TransState;
	BsmPlanData_t m_BsmData;
	//DebugControl_t m_DebugControl;	
	CSoftTimer m_AckTimerBank1; 
	CSoftTimer m_AckTimerBank2;
	CSoftTimer m_AckTimer;						//ack singal time out timer
	
	INT16 m_PositionFeedBack[MLC_AXIS_NUM];		//MLC actual posiion 
	OriginalCP_t *m_OriginalCpPtr;
	OriginalCP_t m_Original[360];
	FLOAT m_DeltaTimeCp[360];
	BeamPlan_t *m_BeamPlanPtr;
	FsmState_u			m_RtcCurrentState;
	BsmFsmState_u		m_CurrentState;				//Bsm current Fsm State
	BsmFsmState_u		m_LastState;				//Bsm Last time Fsm State
	BsmFsmState_u		m_RequiredState;			//Bsm requiredState from RTC
	UINT64				m_Interlock;				//Bsm Interlock
	UINT16				m_RadiationIndexFrBsm;		//Bsm current radiation Index
	CSoftTimer			m_StateTimer;				//Bsm Fsm timeout check	
	UINT16				m_Unready;
	UINT8				m_CommandToBsm;				//bit0:reset
	FLOAT				m_TotalTreatmentTime;
	BOOL				m_BsmInitCompleteFlag;	
	VOID			Initialize(VOID);
	VOID			ExecuteBusiness(VOID);
	BOOL			UpdateInputResource(VOID); //update compoent outputs
	BOOL			UpdateOutputResource(VOID);
	//Function members
	BOOL TransHasError(VOID);
	VOID ResetCommunication(VOID);				//reset communication
	BOOL InputBusIsIdle(VOID);					//check input data bus is idle or not
	BOOL OutputBusIsIdle(VOID);					//check output data bus is idle or not
	BOOL BusIsReset(VOID);						//check data bus is reset or not
	BsmPlanState_eu RunIdleState(VOID);			//idle state
	BsmPlanState_eu RunSendState(VOID);			//send state
	BsmPlanState_eu RunErrorState(VOID);		//error state
	BsmPlanState_eu RunFinishState(VOID);		//finish state
	VOID SetBusIdle(VOID);						//
	VOID SendPlanToBSM(VOID);					//send one  plan data to BSM
	BOOL SendOneCpDataToBSM(UINT16 CpIndex);				//send one Cp data to BSM
	BOOL SendFirstBankDataToBSM(UINT16 CpIndex);
	BOOL SendSecondBankDataToBSM(UINT16 CpIndex);
	VOID LoadDataToBus(UINT16 CpIndex,UINT16 BankIndex);
	VOID LoadControlToBus(VOID);
	BOOL RecvAckOk(UINT16 BankIndex);						//
	VOID GetPlanData(VOID);						//get plan data from bgm resource	
	VOID ClearPlan(VOID);	
	VOID GetRTFiedData(VOID);					//get feedback data from PLC
	VOID GetPosFeedBackData(VOID);				//get actual position from MLC
	VOID GetInterlock(VOID);					//get Bsm interlock
	VOID GetRtcCurrentState(VOID);				//get RTC current FSM state
	VOID GetState(VOID);						//get Bsm Fsm State
	VOID SetState(VOID);						//set Bsm Fsm State
	VOID RunState(VOID);						//detail doings of every state
	VOID InitialMain(VOID);
	VOID StandbyMain(VOID);
	VOID PrepareMain(VOID);
	VOID ServoMain(VOID);
	VOID ManualMain(VOID);
	VOID OffMain(VOID);
	VOID FaultMain(VOID);
	BOOL SendPlanMain(VOID);					//sending plan data main function
	BsmFsmState_u JumpToNextState(BsmFsmState_u RequiredState);
	VOID SendRadiationIndexToBSM(VOID);				//send radiation index to BSM
	VOID GetRadiationIndexFrBSM(VOID);				//get BSM current radiationIndex
	VOID OutputRequiredState(VOID);					//put requiredstate on bus
	VOID SendCmdToBSM(UINT8 Command);
	BOOL EmulatorMode(VOID);
	VOID ClearResetCmd(VOID);						//clear reset command to BSM
	VOID UpdateInputsFmGUI(VOID);
	VOID UpdateOutputsToGUI(VOID);
	
};


