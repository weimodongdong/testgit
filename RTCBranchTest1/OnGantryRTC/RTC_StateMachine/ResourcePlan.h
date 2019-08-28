/*******************************************************************************
** File	       : ResourcePlan.h
** Author      : ChenLong
** Version     : 1.0
** Environment : VS2013
** Description :
** History     : 18 / 08 / 07    LiBo    Created
*******************************************************************************/
/*==== DECLARATION CONTROL ===================================================*/
#pragma once

/*==== INCLUDES ==============================================================*/
#include "TcDef.h"

/*==== MACROS ================================================================*/
#define		MAX_CP_NUM				360
#define		MLC_POS_NUM				128

/*==== STRUCT ================================================================*/
struct OriginalCP_t
{
	//BeamEnergy
	FLOAT BeamEnergy;
	FLOAT DoseWeight;
	//Mlc&Head
	FLOAT MlcPos[MLC_POS_NUM];
	FLOAT MlcAngle;
	BYTE  MlcDir;
	//Gantry
	FLOAT GantryAngle;
	BYTE  GantryDir;
	//Pss
	FLOAT PssAngle;
	BYTE  PssDir;
	FLOAT TableVerPos;
	FLOAT TableLonPos;
	FLOAT TableLatPos;
	FLOAT TablePitchAngle;
	BYTE  TablePitchDir;
	FLOAT TableRollAngle;
	BYTE  TableRollDir;
	UINT16 RadiationIndex;
};


/*==== ENUMS =================================================================*/
enum{
	CW = 1, //clockwise
	CCW = 2
};


struct BackCalSpeedCP_t
{
	FLOAT Mlc[MAX_CP_NUM][MLC_POS_NUM];
	FLOAT Head[MAX_CP_NUM];
	FLOAT Pss[MAX_CP_NUM];
	FLOAT Dose[MAX_CP_NUM];
	FLOAT Gantry[MAX_CP_NUM];
};

struct BeamPlan_t{
	BYTE   BeamType;
	BYTE   RadiationType;
	UINT16 BeamNumber;
	UINT16 TotalCP;
	FLOAT  DoseRateSet;
	FLOAT  Dose;
	FLOAT  GantryAngleTolerance;
	FLOAT  BeamLimitingAngleTolerance;
	FLOAT  AsymxPositionTolerance;
	FLOAT  AsymyPositionTolerance;
	FLOAT  MlcPositionTolerance;
	FLOAT  TableTopVerPositionTolerance;
	FLOAT  TableTopLonPositionTolerance;
	FLOAT  TableTopLatPositionTolerance;
};

/*==== ENUMS ================================================================*/
enum ServoType_eu{
	None = 0,
	Gantry = 1, //used for VMAT
	Dose = 2, //used for IMRT
};

enum PlanEvent_eu{
	PLAN_EVT_NONE = 0,
	KV_PLAN_RECV = 1,
	MV_PLAN_RECV = 2,
	CLOSE_PATIENT = 3,
};

enum RadiationType_eu{
	RadiationKv = 1,
	RadiationMv = 2,
};

enum PlanType_eu{
	PLAN_TYPE_NONE = 0,
	PLAN_TYPE_VMAT = 1,
	PLAN_TYPE_SLIDING_WINDOW = 2,

	PLAN_TYPE_KV_KV = 10,
	PLAN_TYPE_CBCT = 11,
};

/*==== CLASS =================================================================*/
class CPlanResource
{
public:
	CPlanResource();
	virtual ~CPlanResource();

	//Provider & User interface
	BOOL				SetPlanReady(BOOL Ready);
	BOOL				PlanIsReady(VOID);
	BOOL				SetPlanCPTotal(UINT16 PlanCPTotal);
	UINT16				GetPlanCPTotal(VOID);
	VOID				SetDoseTotal(FLOAT Dose);
	FLOAT				GetDoseTotal(VOID);
	
	VOID				BindingPtrOriginalPlan(OriginalCP_t* StructArray);
	OriginalCP_t*		GetOriginalPlan(VOID);
	VOID				BindingPtrSpeedData(BackCalSpeedCP_t* StructPtr);
	BackCalSpeedCP_t*	GetSpeedData(VOID);
	VOID				BindingPtrBeamPlanData(BeamPlan_t* BeamPlan); //set beam
	BeamPlan_t*         GetBeamPlanData(VOID); //get beam plan data,beam number,cp total,etc
	VOID				BindingPtrDeltaTimeCp(FLOAT* ptr);
	FLOAT*				GetDeltaTimeCp(VOID);
	VOID				SetServoAxisType(ServoType_eu ServoAxisTyge);
	ServoType_eu		GetServoAxisType(VOID);
	VOID				SetRadiationProtTime(LONGLONG Time);
	LONGLONG			GetRadiationProtTime(VOID); //get radiation protect time
	VOID				SetPlanEvent(PlanEvent_eu Event);
	PlanEvent_eu		GetPlanEvent(VOID);
		
private:
	OriginalCP_t*		m_OriginalPlanPtr;
	BackCalSpeedCP_t*	m_SpeedPtr;
	BeamPlan_t*			m_BeamPlanPtr;
	FLOAT*				m_DeltaTimeCp;

	BOOL				m_PlanReadyFlag;
	UINT16				m_PlanCPTotal;
	FLOAT				m_DoseTotal;
	ServoType_eu		m_ServoAxisType;
	LONGLONG			m_RadiationProtTime;
	PlanEvent_eu		m_PlanEvent;
};
