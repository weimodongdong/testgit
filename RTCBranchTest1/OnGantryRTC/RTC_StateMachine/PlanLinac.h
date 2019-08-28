/*******************************************************************************
** File	      : PlanLinac.h
** Author     : ChenLong
** Version    :	1.0
** Environment: VS2013
** Description:
** History    :	18/11/1    Chenlong    Created
*******************************************************************************/
/*==== DECLARATION CONTROL ===================================================*/
#pragma once

/*==== INCLUDES ==============================================================*/
#include "TcDef.h"
#include "ResourceMngr.h"



/*==== MARCOS ================================================================*/
#define		BEAMENERGY_NUM			360
#define		GANTRY_CP_NUM			360
#define		MLC_CP_NUM				360
#define		HEAD_CP_NUM				360
#define		PSS_CP_NUM				360
#define		DOSE_CP_NUM				360
#define		MLC_LEAF_NUM			120

#define		RADIATION_INDEX_INIT	1

/*==== STRUCTS ===============================================================*/
struct DeltaCP_t{
	FLOAT DoseWeight[MAX_CP_NUM];
	FLOAT MlcPos[MAX_CP_NUM][MLC_POS_NUM];
	FLOAT HeadAngle[MAX_CP_NUM];					//x-ray or gamma
	FLOAT GantryAngle[MAX_CP_NUM];
	FLOAT PssAngle[MAX_CP_NUM];
	FLOAT TableVerPos[MAX_CP_NUM];
	FLOAT TableLonPos[MAX_CP_NUM];
	FLOAT TableLatPos[MAX_CP_NUM];
	FLOAT TablePitchAngle[MAX_CP_NUM];
	FLOAT TableRollAngle[MAX_CP_NUM];
};

struct SpeedSetting_t{
	FLOAT GantryMaxSpeed;
	FLOAT GantryMinSpeed;
	FLOAT MlcMaxSpeed;
	FLOAT MlcMinSpeed;
	FLOAT HeadMaxSpeed;
	FLOAT HeadMinSpeed;
	FLOAT PssMaxSpeed[6];
	FLOAT PssMinSpeed[6];
	FLOAT DoseMaxSpeed;
	FLOAT DoseMinSpeed;
};

struct MaxTime_t{
	FLOAT DoseMaxTime;
	FLOAT MlcPosMaxTime;
	FLOAT HeadAngleMaxTime;
	FLOAT GantryAngleMaxTime;
	FLOAT PssAngleMaxTime;
	FLOAT TableVerPosMaxTime;
	FLOAT TableLonPosMaxTime;
	FLOAT TableLatPosMaxTime;
	FLOAT TablePitchAngleMaxTime;
	FLOAT TableRollAngleMaxTime;
};

struct MinTime_t{
	FLOAT DoseMinTime;
	FLOAT MlcPosMinTime;
	FLOAT HeadAngleMinTime;
	FLOAT GantryAngleMinTime;
	FLOAT PssAngleMinTime;
	FLOAT TableVerPosMinTime;
	FLOAT TableLonPosMinTime;
	FLOAT TableLatPosMinTime;
	FLOAT TablePitchAngleMinTime;
	FLOAT TableRollAngleMinTime;
};

struct HandlerInputs_t{
	const BeamPlan_t*	BeamPlanPtr;
	OriginalCP_t*		OriginalPlanPtr;
	DeltaCP_t*			DeltaPtr;
	BackCalSpeedCP_t*	BackCalSpeedPtr;
	ResComponent_t*		ResComponentPtr;
};

/*==== CLASS ================================================================*/
class CPlanLinac
{
public:
	CPlanLinac(HandlerInputs_t* HandlerInputs);
	~CPlanLinac();
	//Interfaces
	BOOL	HandlePlanData(VOID);
	VOID	ClearPlanData(VOID);
	ServoType_eu GetServoAxisType();
	LONGLONG GetProtectTime(VOID);						//total delta time of servo axis
	FLOAT* GetDeltaTimeCp(VOID);
private:
	//Data members
	HandlerInputs_t*	m_PlanBufferPtr;				//pointer for input parameters
	//MaxTime_t			m_MaxTime;						//max cost time of all axis
	//FLOAT				m_Tmax;							//max value of struct m_MaxTime
	FLOAT               m_MinServoAxisSpeed;            // min Servo Axis Speed
	UINT16				m_IntervalStep;					//interval step after extend radiation index
	FLOAT				m_AllAxisStepMaxTime[MAX_CP_NUM];	//all Axis every Step need max time 
    FLOAT				m_ServoAxisTime[MAX_CP_NUM];	//delta value/servo axis min speed 
	ServoType_eu		m_CurrentServoType;				//current servo axis type
	PlanType_eu			m_TreatmentPlanType;

	//Method members
	VOID  ExtendRadiationIndex(VOID);					//extend radiation index
	
	BOOL  GetDeltaCP(VOID);								//figure out delta value of all axis
	//VOID  GetDeltaDose(VOID);
	//VOID  GetDeltaMlc(VOID);
	//VOID  GetDeltaGantry(VOID);
	
	BOOL  GetMaxTime(VOID);								//figure out max time of all axis within all control point
	//BOOL  GetMaxTimeDose(VOID);
	//BOOL  GetMaxTimeMlc(VOID);
	//BOOL  GetMaxTimeGantry(VOID);

	BOOL  GetSpeed(VOID);								//figure out speed of all axis
	//BOOL  GetSpeedGantry(VOID);
	//BOOL  GetSpeedDose(VOID);
	//BOOL  GetSpeedMlc(VOID);

	BOOL  GetCurrentServoAxis(VOID);					//find out current servo axis
	BOOL  GetServoAxisDeltaTime(VOID);					//figure out servo axis delta time
	BOOL  GetServoDoseDeltaTime(VOID);					//figure out delta time when servo axis is dose
	BOOL  GetServoGantryDeltaTime(VOID);				//figure out delta time when servo axis is gantry
	//BOOL  GetServoAxisMinSpeed(VOID);					//find min speed of servo axis 

	BOOL  InputsParameterIsReady(VOID);					//judge input parameters is ready		
};
