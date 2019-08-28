/*******************************************************************************
** File	      : ResourceBgm.h
** Author     : ChenLong
** Version    :	1.0
** Environment: VS2013
** Description:
** History    :	18/01/30    Chenlong    Created
*******************************************************************************/
/*==== DECLARATION CONTROL ===================================================*/
#pragma once

/*==== INCLUDES ==============================================================*/
#include "TcDef.h"

/*==== CONST =================================================================*/
/* bgm interlock define*/
const UINT64 INTL_NO_SET				= 0x0000000000000000;				//machine parameter interlock

const UINT64 INTL_MACHINE_PARA			= 0x0000000000000001;				//Bit0 >> machine parameter
const UINT64 INTL_MOD_STANDBY			= 0x0000000000000002;				//Bit1 >> modulator standby
const UINT64 INTL_MOD_HV				= 0x0000000000000004;				//Bit2 >> modulator hv
const UINT64 INTL_MOD_TRIG				= 0x0000000000000008;				//Bit3 >> modulator trig
const UINT64 INTL_COM_MOD				= 0x0000000000000010;				//Bit4 >> modulator communication
const UINT64 INTL_COOLING_INTL1			= 0x0000000000000020;				//Bit5 >> cooling interlock 1
const UINT64 INTL_COOLING_INTL2			= 0x0000000000000040;				//Bit6 >> cooling interlock 2
const UINT64 INTL_WATER_FLOW			= 0x0000000000000080;				//Bit7 >> cooling water flow
const UINT64 INTL_COM_WATER				= 0x0000000000000100;				//Bit8 >> cooling communication
const UINT64 INTL_GUN_HEATER			= 0x0000000000000200;				//Bit9 >> gun heater
const UINT64 INTL_GUN_HV				= 0x0000000000000400;				//Bit10 >> gun hv
const UINT64 INTL_GUN_TRIG				= 0x0000000000000800;				//Bit11 >> gun trig
const UINT64 INTL_VAC					= 0x0000000000001000;				//Bit12 >> vacuum ok
const UINT64 INTL_GAS_LOW				= 0x0000000000002000;				//Bit13 >> gas low
const UINT64 INTL_GAS_HIGH				= 0x0000000000004000;				//Bit14 >> gas high
const UINT64 INTL_RADIATION_TIMER		= 0x0000000000008000;				//Bit15 >> radiation timer
const UINT64 INTL_BGM_HV				= 0x0000000000010000;				//Bit16 >> hv interlock				<<< bgm plc
const UINT64 INTL_HV_CONTACTOR_ERROR	= 0x0000000000020000;				//Bit17 >> hv contactor error		<<< bgm plc
const UINT64 INTL_CONTROLLER_OK			= 0x0000000000040000;				//Bit18 >> controller ok			<<< bgm plc
const UINT64 INTL_SLIPRING_HV			= 0x0000000000080000;				//Bit19 >> sliping hv				<<< bgm plc
const UINT64 INTL_SLIPRING_TREAT_EN		= 0x0000000000100000;				//Bit20 >> sliping treatment enable <<< bgm plc
const UINT64 INTL_DOSE1_WR_TM			= 0x0000000000200000;				//Bit21 >> dose1 write time out
const UINT64 INTL_DOSE1_SYMMETRY		= 0x0000000000400000;				//Bit22 >> dose1 symmetry
const UINT64 INTL_DOSE1_EtherCAT		= 0x0000000000800000;				//Bit23 >> dose1 EtherCAT
const UINT64 INTL_DOSE1_DK_EXD			= 0x0000000001000000;				//Bit24 >> dose1 dark exceed
const UINT64 INTL_DOSE1_TOTAL_EXD		= 0x0000000002000000;				//Bit25 >> dose1 total exceed
const UINT64 INTL_DOSE2_WR_TM			= 0x0000000004000000;				//Bit26 >> dose2 write time out
const UINT64 INTL_DOSE2_SYMMETRY		= 0x0000000008000000;				//Bit27 >> dose2 symmetry
const UINT64 INTL_DOSE2_EtherCAT		= 0x0000000010000000;				//Bit28 >> dose2 EtherCAT
const UINT64 INTL_DOSE2_DK_EXD			= 0x0000000020000000;				//Bit29 >> dose2 dark exceed
const UINT64 INTL_DOSE2_TOTAL_EXD		= 0x0000000040000000;				//Bit30 >> dose2 total exceed
const UINT64 INTL_DOSE_DUAL_CH_EXD		= 0x0000000080000000;				//Bit31 >> dose dual channel exceed
const UINT64 INTL_INFORMATION			= 0x0000000100000000;				//Bit32 >> dose error information
const UINT64 INTL_DOSE_DUMMY_TIME_OUT	= 0x0000000200000000;				//Bit33 >> dose board dummy time out
const UINT64 INTL_DOSE_HARDWARE			= 0x0000000400000000;				//Bit33 >> dose board hardware

/*==== ENUM ==================================================================*/
//BGM Unreadys Bit Define
enum{
	BGM_UNREADY_MODULATOR	= 0x00000001,		//Bit0
	BGM_UNREADY_GUN			= 0x00000002,		//Bit1
	BGM_UNREADY_DOSE		= 0x00000004,		//Bit2
};

/*==== CLASS =================================================================*/
class CResourceBgm
{
public:
	CResourceBgm();
	~CResourceBgm();

	//Interfaces
	BOOL	PlanTransCompleted(VOID); //dose data transition is completed
	VOID	SetPlanBgmIsReady(BOOL Completed); //set state of dose transition		TRUE:completed  FALSE:not completed

private:
	BOOL	m_DoseTransFlag; //dose transition completed flag
};

