/*******************************************************************************
** File	      : ResourceIgs.h
** Author     : libo
** Version    :	1.0
** Environment: VS2013
** Description:
** History    :	18/08/04    libo    Created
*******************************************************************************/
/*==== DECLARATION CONTROL ===================================================*/
#pragma once

/*==== INCLUDES ==============================================================*/
#include "TcDef.h"
/*==== CONST =================================================================*/
const UINT32 SIZE_WILD_DATA   = 4;                                     // wild has four datas
const UINT32 SIZE_PANEL_POS   = 2;                                     // panel have two workplaces

/*==== ENUMS =================================================================*/
// IGS Intelocks Bit define
enum{						   
	INTL_INITIAL_FAILED           = 0x00000001,                            // IGS initialization failure
	INTL_SERIAL_COM_FAILED        = 0x00000002,                            // Igs serial commmunication failed
	INTI_KVETHERCAT_COM_FAILED    = 0x00000004,                            // Igs kv panel etherCat communication failed
	INTI_MVETHERCAT_COM_FAILED    = 0x00000008,                            // Igs Mv panel ethercat communication failed
	INTI_ONGANTRYRTC_COM_FAILED   = 0x00000010,                            // Igs with ongantryrtc ethercat communication failed
	INTI_CANOPEN_COM_FAILED       = 0x00000020,                            // Igs canopen communication failed
	INTI_KV_DRIVER_FAULT          = 0x00000040,                            // kv driver fault
	INTI_KV_PANEL_TIMEOUT         = 0x00000080,                            // KV paneal move timeout
	INTI_KV_PANEL_EN_FAILED       = 0x00000100,                            // KV paneal move enable failed
	INTI_KV_PANEL_MIS_MOVE        = 0x00000200,                            // KV paneal mis move
	INTI_KV_FBPOS_SUPERDIFFEREVCE = 0x00000400,                            // KV feedback position super difference
	INTI_MV_DRIVER_FAULT          = 0x00000800,                            // MV driver fault
	INTI_MV_PANEL_TIMEOUT         = 0x00001000,                            // MV paneal move timeout
	INTI_MV_PANEL_EN_FAILED       = 0x00002000,                            // MV paneal move enable failed
	INTI_MV_PANEL_MIS_MOVE        = 0x00004000,                            // MV paneal mis move
	INTI_MV_FBPOS_SUPERDIFFEREVCE = 0x00008000,                            // MV feedback position super difference
	INTI_ICU_ERROR                = 0x00010000,                            // KV paneal move timeout
	INTI_HV_GENERATOR_ERROR       = 0x00020000,                            // KV paneal move timeout
	INTI_COLLIMATOR_MOTOR_ALARM   = 0x00040000,                            // KV paneal move timeout
	INTI_HVEN_SLIPRING            = 0x00080000,                            // KV paneal move timeout
	INTI_TRIG_EPID2IGS            = 0x00100000,                            // KV paneal move timeout
	INTI_IKVEN_RTC2IGS            = 0x00200000,                            // KV paneal move timeout
};


//Igs unready define in Gui&Rtc protocal
enum{
	IGS_UNREADY     = 0x00000001,
	IGS_CT_NOTREADY = 0x00000002,
};
enum IgsState_eu
{
	IGS_INITIAL = 0,
	IGS_STANDBY = 3,
	IGS_PREPARE = 6,
	IGS_READY = 9,
	IGS_EXPOSE = 10,
	IGS_FINISH = 11,
	IGS_FAULT = 12,

};
enum 
{
	IGS_NO_MODE   = 0,
	IGS_CBCT      = 1,
	IGS_KV_KV     = 2,
	IGS_KV_MV     = 3,
	IGS_MV_MV     = 4,

};

enum
{
	FULL_WILD = 0,
	HALF_WILD = 1,


};
/*==== STRUCTS ===============================================================*/
struct Exposelist_t
{
	FLOAT GantryPosition;                                                    // record gantry position
	FLOAT  Dose;                                                              // record dose
};

struct IcuData_t
{
	UINT8  FrequencyExp;                                              // exposure frequency     unit: Hz
	UINT8  KVExp;                                                     // exposure voltage       unit: Kv
	UINT8  FocusExp;                                                  // exposure focus state   state :Large focus and small focus       
	UINT16 MAExp;                                                     // Exposure curremt       unit: mA
	UINT16 MSExp;                                                     // time of exposure       unit: ms
};
struct PanelSetData_t
{
	INT32 Offset1;                                                    // Set the deviation value at one time of panel  range :0-33554432
	INT32 Offset2;                                                    // Set the deviation value at the two time of panel range :0-33554432
};

/*==== CLASS =================================================================*/
class CResourceIgs
{
public:
	CResourceIgs();
	~CResourceIgs();

	//Provider interface
	VOID SetCurrentState(IgsState_eu CurrentState);
	IgsState_eu GetCurrentState(VOID);
	VOID   SetInterlock(UINT32 Interelock);
	UINT32 Getinterlock();                                            // get igs  interlock
	VOID   SetNotReady(UINT32 Notready);
	UINT32 GetNotReady();                                            // get igs  Notreadys

	VOID   SetInitialState(BOOL InitialState);
	BOOL   InitialIsOk();                                             // judge  initial  is  OK
	VOID   SetReady(BOOL Ready);                            
	BOOL   IsReady();                                                 // igs is ready
	
	


private:
	UINT8 m_WorkMode;                                                  // the working mode of ICU : CBCT   KV-Kv KV-MV  MV-MV 
	IgsState_eu m_CurrentState;
	UINT32      m_Interlock;
	UINT32      m_Notready;                                            //  igs  Not ready
	BOOL        m_InitialState;
	BOOL        m_Ready;
};
