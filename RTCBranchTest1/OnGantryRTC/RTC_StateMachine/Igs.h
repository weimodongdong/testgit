/*******************************************************************************
** File	      : Igs.h
** Author     : libo
** Version    :	1.0
** Environment: VS2013
** Description:
** History    :	18/08/01    libo   Created
*******************************************************************************/
/*==== DECLARATION CONTROL ===================================================*/
#pragma once

/*==== INCLUDES ==============================================================*/
#include "ResourceMngr.h"
/*= == = CONST == == == == == == == == == == == == == == == == == == == == == == == == == == == == == == == == = */
const UINT16 SIZE_EXPOSE_LIST = 20;                                   // the buffer of exposelist
const UINT8  SIZE_EXPOSE_LIST_GROUP = 30;                             //  the buffer of exposelist
const LONGLONG KV_EXPOSE_TIME = 10000;                                // expose  timer
const LONGLONG IGS_REQUIRESTATE_TIMEOUT = 600;                        // send requiresate timeout time  ms
const LONGLONG IGS_CBCT_EXPOSE_REQUIRESTATE_TIMEOUT = 30000;          // send requiresate timeout time  ms
const UINT32   KV_TOLERANCE_BIT = 0X20000000;                                   // the buffer of exposelist



/*==== STRUCTS ===============================================================*/
struct PanelActualData_t
{
	FLOAT Position;                                                   // the actual position of panel   unit: mm
	INT32 ENcode1;                                                    // the original value of the first code 
	INT32 ENcode2;                                                    // the original value of the second time code 
	BOOL  PosReady;                                                   // panel in place signal 
};
struct GantryExposeList_t
{
	FLOAT  GantryPosition;                                           // gantry position
	FLOAT  GantrySpeed;                                              // Gantry Speed
	BYTE   Hour;                                                     // system time :h
	BYTE   Minute;                                                   // system time :min
	BYTE   Second;													 // system time :s
	UINT16 Millisecond;                                        		 // system time :ms
	BYTE   CollimatorType;                                           // collimator type ::  0：NO  1：φ6  2：φ9  3：φ12  4：φ16  5：φ20  6：φ25  7：φ35  255：φMAX
	BYTE   PanelType;                                                // 0：KV Panel     1：MV Panel
	FLOAT  PanelPosition;                                            // panel position                                  
	UINT32 OpenSourceTimer;                                          // open source time

};

struct CExposeSequence_t
{
	BOOL CurrentRecordFlag;                                             // current flag record gantry position and dose
	BOOL LastRecordFlag;                                                // last flag record gantry position and dose
	UINT16 RecordNum;
	UINT16 RecordCounter;
	//Exposelist_t Exposelist[SIZE_EXPOSE_LIST_GROUP][SIZE_EXPOSE_LIST];                                    // buffer gantry position and dose
	GantryExposeList_t GantryExposeList[SIZE_EXPOSE_LIST_GROUP][SIZE_EXPOSE_LIST];
	//BOOL m_GantryStart;
	//BOOL m_GantryReady;
};

struct CPlanGantry_t
{
	UINT16 GantryTarPosition;                               // CBCT  gantry target Positio
	UINT16 CpTotal;                                         // cp total
	UINT16 LastRadiationIndex;                              // last radiation Index
	UINT16 CbctExposeNum;                                   // 
	UINT8  GantryDir;                                       // CBCT  gantry run director
};



struct Parameters_t
{
	INT32 KvOffset1;                                                    // Set the kv deviation value at one time of panel  range :0-33554432
	INT32 KvOffset2;                                                    // Set the kv deviation value at the two time of panel range :0-33554432
	INT32 MvOffset1;                                                    // Set the mv deviation value at one time of panel  range :0-33554432
	INT32 MvOffset2;                                                    // Set the mv deviation value at the two time of panel range :0-33554432
	UINT16 KvTimeOut;                                                   // set kv panel time for move
	UINT16 MvTimeOut;                                                   // set kv panel time for move
};

enum  IgsCntlWorkMode_eu
{
	IGSCNTL_NO_MODE = 0,
	IGSCNTL_KV      = 1,
	IGSCNTL_MV      = 2,
	IGSCNTL_CBCT    = 3,
};

enum  
{
	COLLIMATOR_CROSS = 0,
	COLLIMATOR_LONG = 1,
	COLLIMATOR_OFFSET_CROSS = 2,
	COLLIMATOR_OFFSET_LONG = 3,
};
enum
{
	IGS_STANDBY_BITE = 0x01,
	IGS_PREPAR_BITE = 0x02,
	IGS_READY_BITE  = 0x04,
	IGS_EXPOSE_BITE = 0x08,
	IGS_FINISH_BITE = 0x10,
	IGS_FAULT_BITE  = 0x20,
};

/*==== FUNCTIONS =============================================================*/
/*==== CLASS =================================================================*/

class CIgs
{
public:
	CIgs();
	CIgs(CResourceMngr* ResourceMngr);
	~CIgs();
	BOOL CycleUpdate();
private:
	//Data members
	CResourceMngr* m_ResourceMngr;                           // address of  ResourceMngr
	ResComponent_t* m_ResComponent;                          // address of  ResComponent
	IgsState_eu m_RequiredState;                             // Igs require state
	IgsState_eu m_CurrentState;                              // Igs last  state
	IgsState_eu m_LastState;                                 // Igs last  state
	UINT16    m_LastRequireState;                            //  Igs last  Last
	UINT8 m_RtcCurrentState;                                 // Rtc  Current stste
	UINT8 m_IgsWorkMode;                                     // Igs WorkMode
	IgsCntlWorkMode_eu m_IgsCntlWorkMode;                     // Igs controller  WorkMode
	UINT8 m_Protocal;                                         // body  head
	UINT8 m_RequiredWorktimes;                                // one plan need work times
	UINT8 m_WorkSuccessfultimes;                              // one plan work successfully times
	UINT16 m_SimulationBite;                                  // the bite of  Simulation state                                  
	CSoftTimer KVExposeTimer;                                 //  timer for kv  expose mode 
	CSoftTimer m_StateTimer;                                  //  timer for change state 
	// Icu  data								                         
	IcuData_t m_IcuPlanData;                                  //ICU  Plan Data
	IcuData_t m_IcuActualData;                               // ICU actual data
	INT16 m_CollimatorData[SIZE_WILD_DATA];                  // m_CollimatorData[0]:The cross of the field,m_CollimatorData[1]:The Long of the field
	// kv data                                               // m_CollimatorData[2]:The offset cross of the field,m_CollimatorData[3]:The offset long of the field
	FLOAT m_KvPosition[SIZE_PANEL_POS];                      //  position  of KV  two Workstations 
	PanelActualData_t m_KvActualData;                        // kv actual data
	// mv data
	FLOAT m_MvPosition[SIZE_PANEL_POS];                      //  position  of MV  two Workstations 
	PanelActualData_t m_MvActualData;                        // Mv actual data
	//expose sequence
	CExposeSequence_t m_ExposeSequence;                      // expose sequence data
	//Parameters
	Parameters_t m_Parameter;                                // Parameter from GUI
	UINT32 m_Interlock;                                      // Igs  Interlock
	UINT32 m_Unreadys;                                        // Igs unready
	BOOL  m_KvReady;                                         // kv is ready
	CPlanGantry_t m_PlanGantry;                                 // plan Gantry Data;
	



	VOID GetState();                                          // get igs current data
	VOID SetState(VOID);                                      // set igs state
	VOID InitialMain(VOID);                                  
	VOID StandbyMain(VOID);
	VOID PrepareMain(VOID);
	VOID ReadyMain(VOID);
	VOID ExposeMain(VOID);
	VOID FinishMain(VOID);
	BOOL GetPlan(VOID);                                      // get plan
	BOOL SendPlan(VOID);                                     // send Plan
	VOID CleanPlan(VOID);                                    // clean plan
	VOID GetInterlock(VOID);                                 // get interlock
	VOID GetNotready(VOID);                                  // get notready

	VOID GetParameter(VOID);                                 // get parameter
	BOOL GetExposFrameData(VOID);                            // get exposFrameData
	VOID GetRtcCurrentState(VOID);                           // get rtc current state
	VOID SetParameter(VOID);                                 // set parameter to Igs
	BOOL SetWorkMode(VOID);                                  // set workmode to Igs
	BOOL SendRequireState(VOID);                             // send requirestate to Igs
	VOID GetRTFieldData(VOID);                               // Get RT Field to GUI
	BOOL IsJumpToPrePare(VOID);                              // Igs jump Prepare
	BOOL IsJumpToFault(VOID);                                //  Igs jump to FAULT
	BOOL IsJumpToFinish(VOID);                               //  is jump to finish
	VOID SendResetSigial(VOID);                              // send  reset cmd from  GUI to Igs
	VOID GetExposeList(VOID);                                //  get expose  List 
	VOID GetKvExposeList(VOID);


	BOOL RecordExposeListData(VOID);                         // record  exposelist data :gantry Position and dose
	VOID SendPanelPosition(VOID);                            // send  panel postion to igs controller
	BOOL OutputInitialstate(VOID);                           // output initial state to ResourceIgs
	BOOL KvIsReady(VOID);                                     // IGS IS ready
	BOOL SetIcuParameterIsOk(VOID);                           // set Icu parameter is ok
	BOOL GetGantryTarPosition(VOID);                          // get gantry  Target position 
	VOID OutputReady(VOID);                                   // output Ready to ResourceIgs
	VOID SetSwKvEnable( BOOL Enable);                         // output Ready to ResourceIgs
	UINT8 SetSimulationState();                              // set Simulation State
	

};