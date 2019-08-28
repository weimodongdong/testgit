/*******************************************************************************
** File	      : ResourceBsm.h
** Author     : XuYun
** Version    :	1.0
** Environment: VS2013
** Description:
** History    :	18/12/04    XuYun    Created
*******************************************************************************/
/*==== DECLARATION CONTROL ===================================================*/
#pragma once

/*==== INCLUDES ==============================================================*/
#include "TcDef.h"

/*==== CONST =================================================================*/
/* bsm interlock define*/


/*==== ENUM ==================================================================*/
//BSM Intelocks Bit define
/*enum
{
	BSM_INIT_FAILED = 0x0001,				//bit0
	BSM_MLC_COMM_FAILURE = 0x0002,			//bit1
	BSM_CALIBRATION_CRC_ERROR = 0x0004,		//bit2
	BSM_MLC_STATE_SYNC_FAILED = 0x0008,		//bit3
	BSM_WATCHDOG_FAILED = 0x0010,			//bit4
	JAWY_OUT_OF_TOLERANCE = 0x0020,			//bit5
	MLC_OUT_OF_TOLERANCE = 0x0040,			//bit6
	HEAD_OUT_OF_TOLERANCE = 0x0080,			//bit7
	HEAD_DRIVER_ERROR = 0x0100,			//bit8
	HEAD_ENCODER_UNTRUSTED = 0x0200,	    //bit9
	JAW_ENCODER_UNTRUSTED = 0x0400,			//bit10
	MLC_PCB_POWER_ERROR = 0x0800,			//bit11
	MLC_PCB_FAN_ERROR = 0x1000,				//bit12
	LEAF_ENCODER_UNTRUSTED = 0x2000,			//bit13
	CARRIER_ENCODER_UNTRUSTED = 0x4000,		//bit14
	CARRIER_DRIVER_ERROR = 0x8000,			//bit15
};*/

#define BSM_INIT_FAILED							0x0000000000000001							//bit0    
#define BSM_PARAMETER_CRC_ERROR					0x0000000000000002							//bit1
#define BSM_WATCHDOG_FAILED						0x0000000000000004							//bit2
#define BSM_MLC_COMM_FAILURE					0x0000000000000100							//bit8
#define BSM_MLC_SUBSYS_SYNC_FAILED				0x0000000000000200							//bit9
#define BSM_MLC_PCB_POWER_ERROR					0x0000000000000400							//bit10
#define BSM_MLC_PCB_FAN_ERROR					0x0000000000000800							//bit11
#define BSM_LEAF_OUT_OF_TOLERANCE				0x0000000000008000							//bit15
#define BSM_LEAF_DRIVER_ERROR					0x0000000000010000							//bit16
#define BSM_LEAF_LOCKED_ERROR					0x0000000000020000							//bit17
#define BSM_LEAF_Encoder_UNTRUSTED				0x0000000000040000							//bit18
#define BSM_CARRIER_DRIVE_ERROR					0x0000000000100000							//bit20
#define BSM_CARRIER_LOCKED_ROTOR				0x0000000000200000							//bit21
#define BSM_CARRIER_ENCODER_UNTRUSTED			0x0000000000400000							//bit22
#define BSM_JAWX_OUT_OF_TOLERANCE				0x0000000001000000							//bit24
#define BSM_JAWX_DERIVE_ERROR					0x0000000002000000							//bit25
#define BSM_JAWX_LOCKED_ROTOR					0x0000000004000000							//bit26
#define BSM_JAWX_ENCODER_UNTRUSTED				0x0000000008000000							//bit27
#define BSM_JAWY_OUT_OF_TOLERANCE				0x0000000100000000							//bit32
#define BSM_JAWY_DRIVE_ERROR					0x0000000200000000							//bit33
#define BSM_JAWYLOCKE_ROTOR						0x0000000400000000							//bit34
#define BSM_JAWY_ENCODEDR_UNTRUSTED				0x0000000800000000							//bit35
#define BSM_COLLIMATOR_OUT_OF_TOLERANCE			0x0000010000000000							//bit40
#define BSM_COLLIMATOR_POWER_ERROR				0x0000020000000000							//bit41
#define BSM_COLLIMATOR_DRIVE_ERROR				0x0000040000000000							//bit42
#define BSM_COLLIMATOR_LOCKED_ROTOR				0x0000080000000000							//bit43
#define BSM_COLLIMATOR_ENCODEDR_UNTRUSTED		0x0000100000000000							//bit44
#define BSM_PRIMARYCOLLIMATOR_OUT_OF_POSITION	0x0100000000000000							//bit56
#define BSM_PRIMARYCOLLIMATOR_MOTION_TIMEOUT	0x0200000000000000							//bit57
#define BSM_PRIMARYCOLLIMATOR_SIGNAL_ERROR		0x0400000000000000							//bit58/

enum BsmFsmState_u
{
	BSM_NO_STATE = 0,
	BSM_INITIAL = 1,		//BSM Initial state
	BSM_STANDBY = 3,		//BSM Standby state			                  
	BSM_PREPARE = 6,		//BSM Prepare state
	BSM_SERVO = 9,			//BSM Servo	state                
	BSM_FAULT = 10,         //BSM Fault state                           
	BSM_MANUAL = 11,        //BSM Manual state                           
	BSM_OFF = 12            //BSM Off state                           
};

enum BsmUnreadys_u{
	BSM_NOT_READY		 = 0x01, //Bsm not ready
	BSM_UNREADY_JAWX_OOT = 0x02, //jaw x out of tolerance
	BSM_UNREADY_JAWY_OOT = 0x04, //jaw y out of tolerance
	BSM_UNREADY_LEAF_OOT = 0x08, //leaf out of tolerance
	BSM_UNREADY_HEAD_OOT = 0x10, //head out of tolerance
};

/*==== CLASS =================================================================*/
class CResourceBsm
{
public:
	CResourceBsm();
	~CResourceBsm();
	
	VOID	SetPlanTransCompleted(BOOL Completed);						//set state of dose transition		TRUE:completed  FALSE:not completed
	BOOL	PlanTransCompleted(VOID);				//get state of plan transition
	BOOL	SetInterLock(UINT64 InterLock);							//set Bsm Interlock
	UINT64	GetInterlock(VOID);										//get Bsm Interlock
	BOOL	SetUnready(UINT16 Unready);
	UINT16	GetUnready(VOID);
	VOID	SetBsmInitCompleteFlag(BOOL Flag);
	BOOL	GetBSMInitCompleteFlag(VOID);								//check Bsm Initialization complete or not

private:	
	BOOL m_PlanTransFlag;											//plan transition completed flag
	UINT64 m_Interlock;
	UINT16 m_Unready;
	BOOL m_BsmInitCompleteFlag;
};