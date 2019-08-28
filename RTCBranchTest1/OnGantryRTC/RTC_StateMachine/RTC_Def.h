/*******************************************************************************
** File	      : RTC_Def.h
** Author     : ChenLong
** Version    :	1.0
** Environment: VS2013
** Description:
** History    :	17/04/24    Chenlong    Created
*******************************************************************************/
/*==== DECLARATION CONTROL ===================================================*/
#ifndef __RTC_DEF_H__
#define __RTC_DEF_H__

/*==== INCLUDES ==============================================================*/
/*==== GLOBAL VARS============================================================*/
/*==== STRUCT ================================================================*/
/*struct SeriousInhibit_s{
	BOOL RadiationTimer;		//radiation protect time
	BOOL ComStatusGui;			//communication status between gui and rtc
};

struct MinorInhibit_s{
	BOOL MachinePara;			//TRUE:machine parametwre
	BOOL ModStandbyIntl;
	BOOL ModHvIntl;
	BOOL ModTrigIntl;
	BOOL ModCom;
};*/

/*==== MACROS ================================================================*/
/*Function State Identifier Macros*/
#define		RTC_OK						0
#define		RTC_FAIL					1

/*bgm interlock enable macros*/
#define		BGM_ENABLE					1					//bgm interlock enable
#define		BGM_DISABLE					0					//bgm interlock disable

/*subsystem status wait time macros*/

/*bgm interlock define*/
//#define		INTL_MOD_TRIG				0x00000008				//machine parameter interlock
#define		INTL_MOD_TRIG				0x00000008				//interlock >> modulator trig
#define		INTL_MOD_HV					0x00000004				//interlock >> modulator hv
#define		INTL_HV_CONTACTOR_ERROR		0x00020000				//interlock >> hv contactor error		<<< bgm plc
#define		INTL_SLIPRING_HV			0x00080000				//interlock >> sliping hv				<<< bgm plc

/*==== FUNCTIONS =============================================================*/
/*==== CLASS =================================================================*/
#endif
/*==== END OF FILE ===========================================================*/
