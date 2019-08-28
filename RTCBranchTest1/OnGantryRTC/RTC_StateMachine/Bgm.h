/*******************************************************************************
** File	      : Bgm.h
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

/*==== CONST =================================================================*/
const UINT32 DATA_BUS_IDLE = 0;
const UINT32 DATA_BUS_FB_OFFSET = 16;
const UINT32 DATA_BUS_FB_RESET = 0;
const UINT32 ACK_INTERVAL = 1500;		//unit ms
const UINT32 MAX_RESEND_TIME = 3;
const UINT32 DATA_BUS_DATA_LEN = 15;	//number of bank

/*==== STRUCTS ===============================================================*/
struct TransControl_t{	//signals for transition control
	UINT16 BeamTransID;	//beam transition id
	BYTE   BankCounter;
	UINT32 Crc32;
};

struct TransState_t{	//variables for transition state
	UINT16 RetransCounter;
	BOOL PlanSendFinish;//plan data send to bgm finished
	BOOL AckTimeOut;
	BOOL SendFailed;
};

struct DebugControl_t{
	BOOL DummyEnable;	//enable construct dummy data for debug
	BOOL TransIDEn;		//enable construct error transition id
	BOOL Crc32En;		//enable construct error crc32
};

struct DoseCP_t{
	UINT16 RadiationIndex;
	FLOAT  OriginalData;
	FLOAT  Speed;
};

struct DoseData_t
{
	DoseCP_t Data[MAX_CP_TOTAL];
	FLOAT	 DoseTotal;
	UINT16	 CPTotal;
};

/*==== ENUMS =================================================================*/
enum{	//error id
	DOSE_ERROR_CRC = 0xfd,
	DOSE_ERROR_BEAM_ID = 0xfe,
};

enum BgmPlanState_eu{	//BGM plan state
	TRANS_STATE_IDLE = 0,
	TRANS_STATE_SEND = 1,
	TRANS_STATE_ERROR = 2,
	TRANS_STATE_FINISH = 3,
};

/*==== CLASS =================================================================*/
class CBgm
{
public:
	CBgm(CResourceMngr* ResourceMngr);
	virtual ~CBgm();
	VOID CycleUpdate(VOID); //invoke interface

private:
	//Genernal methods 
	VOID			Initialize(VOID);
	VOID			ExecuteBusiness(VOID);
	BOOL			UpdateInputResource(VOID); //update compoent outputs
	BOOL			UpdateOutputResource(VOID);

	//Methods for execute business
	BOOL			TransHasError(VOID);
	VOID			ResetCommunication(VOID); //reset communication
	BOOL			InputBusIsIdle(VOID); //check input data bus is idle or not
	BOOL			OutputBusIsIdle(VOID); //check output data bus is idle or not
	BOOL			BusIsReset(VOID); //check data bus is reset or not
	BgmPlanState_eu	RunIdleState(VOID); //idle state
	BgmPlanState_eu RunSendState(VOID); //send state
	BgmPlanState_eu RunErrorState(VOID); //error state
	BgmPlanState_eu RunFinishState(VOID); //finish state
	VOID			SetBusIdle(VOID);
	VOID			SendDosePlanToBGM(VOID); //send one dose plan data to BGM
	BOOL			SendBankDataToBGM(VOID); //send one bank data to BGM
	VOID			LoadDataToBus(VOID);
	VOID			LoadControlToBus(VOID);
	BOOL			RecvAckOk(VOID);
	VOID			GetPlanData(VOID); //get plan data from bgm resource
	VOID			ConstructDummyData(VOID); //construct dummy data for debug
	VOID			ClearPlan(VOID);
	VOID			LoadServoAxisTygeToBus(BYTE ServoAxisTyge);
	VOID			CleanServoAxisBusData(VOID);

	//Data members
	CResourceMngr*	m_ResourceMngr;
	ResComponent_t* m_ResComponent;
	BgmPlanState_eu	m_PlanState; //bgm plan current state
	TransControl_t	m_TransControl;				
	TransState_t	m_TransState;
	DoseData_t		m_DoseData;
	DebugControl_t	m_DebugControl;
	UINT16			m_BankNumber; //total number of bank
	BOOL			m_InitEn;
	//CSoftTimer m_AckTimer;	  //ack singal time out timer
};


