/*******************************************************************************
** File	      : ParameterDataModel.h
** Author     : ChenLong
** Version    :	1.0
** Environment: VS2013
** Description:
** History    :	19/04/14    ChenLong    Created
*******************************************************************************/
/*==== DECLARATION CONTROL ===================================================*/
#pragma once

/*==== INCLUDES ==============================================================*/
#include "TcDef.h"

/*==== MACROS ================================================================*/
/*==== STRUCTS ===============================================================*/
struct ModulatorSetting_t{
	FLOAT ModulatorFilamentVoltage;
	FLOAT ModulatorPrimaryDCVoltage;
	FLOAT ModulatorPulseWidth;
};

struct GunSetting_t{
	FLOAT GunHV;
	FLOAT GunWarmUp;
	FLOAT GunFilamentCurrentSetTolerance;
	FLOAT GunFilamentVoltageMin;
	FLOAT GunFilamentVoltageMax;
	FLOAT GunHeaterJudgeTime;
	FLOAT GunCathodeCurrentMin;
	FLOAT GunCathodeCurrentMax;
	FLOAT GunDoseRateAtFullPower;
	FLOAT GunRundownPRFK;
	FLOAT GunRundownDoseRateK;
	FLOAT GunHVSetRatio1;
	FLOAT GunHVSetRatio2;
	FLOAT GunHVSetRatio3;
	FLOAT GunHVSetRatio4;
	FLOAT GunFilamentVoltageSetRatio1;
	FLOAT GunFilamentVoltageSetRatio2;
	FLOAT GunFilamentVoltageSetRatio3;
	FLOAT GunFilamentVoltageSetRatio4;
	FLOAT GunFilamentVoltageRatio1;
	FLOAT GunFilamentVoltageRatio2;
	FLOAT GunFilamentVoltageRatio3;
	FLOAT GunFilamentVoltageRatio4;
	FLOAT GunFilamentCurrentRatio1;
	FLOAT GunFilamentCurrentRatio2;
	FLOAT GunFilamentCurrentRatio3;
	FLOAT GunFilamentCurrentRatio4;
	FLOAT GunCathodeCurrentRatio1;
	FLOAT GunCathodeCurrentRatio2;
	FLOAT GunCathodeCurrentRatio3;
	FLOAT GunCathodeCurrentRatio4;
	FLOAT GunHVReadRatio1;
	FLOAT GunHVReadRatio2;
	FLOAT GunHVReadRatio3;
	FLOAT GunHVReadRatio4;
	UINT32 GunWarmupTime;
};

struct AFCSetting_t{
	UINT32	AFCMotorMaximumSpeed;
	UINT32	AFCDirectionSwap;
	FLOAT	AFCLinearLow;
	FLOAT	AFCLinearHigh;
	FLOAT	AFCPreset;
	FLOAT	RFSet;
};

struct AFCCTRL_t{
	UINT32	AFCModeSet;
	UCHAR	AFCManualDirection;
	UCHAR	AFCManualMove;
};

struct DoseSetting_t{
	UINT16	DoseRateCycleTime;
	UINT16	DoseRateAccTime;
	UINT16	DoseCalFactorA1;
	UINT16	DoseCalFactorB1;
	UINT16	DoseCalFactorA2;
	UINT16	DoseCalFactorB2;
	UINT16	DarkCurrentLimit;
	UINT16	DummyDoseRateSet;
	UINT16	DoseInterlockEnable;
	UINT32	Symmetry_Threshold1;
	UINT32	Symmetry_Threshold2;
	FLOAT	Symmetry_Tolerance1;
	FLOAT	Symmetry_Tolerance2;
	FLOAT	DualChannelRatio;
	FLOAT	DualChannelTolerance;
	FLOAT	SegmentOverangeRatio;
	FLOAT	DoseRateLimit;
};

struct PRFSetting_t{
	FLOAT	PRF_Set;
	FLOAT	PrfRatio;
	FLOAT	PrfDeltaRatio;
	FLOAT	DPP;
	FLOAT	DoseDeltaActionRange;
	FLOAT	PrfCatchCurveD1;
	FLOAT	PrfCatchCurveR1;
	FLOAT	PrfCatchCurveD2;
	FLOAT	PrfCatchCurveR2;
	FLOAT	PrfCatchCurveP1;
	FLOAT	PrfCatchCurveQ1;
	FLOAT	PrfCatchCurveP2;
	FLOAT	PrfCatchCurveQ2;
	UINT16	PrfCatchCycle;
};

struct EmulatorSetting_t{
	UINT16	PLCEmulate;
	UINT16	ControllerEmulator;
};

struct BsmSetting_t{
	BYTE	BSMOk;
	FLOAT	HeadSpeed;
	FLOAT	UpJaw1Speed;
	FLOAT	UpJaw2Speed;
	FLOAT	HeadAngle;
	FLOAT	HeadAngleOffset;
	UINT16	BSMEnable;
	FLOAT	CalibrationJaw[16];
	BYTE	BSMInitMode;
	FLOAT	HeadAngleK;
	BYTE	CollimatorPosition;
	FLOAT	HeadAngleSecondaryK;
	UINT32	HeadLimitEncoder;
};

/*==== ENUMS =================================================================*/
enum{
	PARA_MODULATOR_ERROR = 0x0001,
	PARA_GUN_ERROR		 = 0x0002,
	PARA_AFC_ERROR		 = 0x0004,
	PARA_AFC_CTRL_ERROR	 = 0x0008,
	PARA_DOSE_ERROE		 = 0x0010,
	PARA_PRF_ERROR		 = 0x0020,
	PARA_EMULATOR_ERROR	 = 0x0040,
	PARA_BSM_ERROR		 = 0x0080,
};
/*==== CLASS =================================================================*/
/*==== END OF FILE ===========================================================*/