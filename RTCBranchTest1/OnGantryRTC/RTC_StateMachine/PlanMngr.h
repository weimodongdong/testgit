/*******************************************************************************
** File	      : PlanMngr.h
** Author     : ChenLong
** Version    :	1.0
** Environment: VS2013
** Description:
** History    :	18/01/05    Chenlong    Created
				18/11/06	Chenlong    Modif :: Sync file with OnGantryRTC
*******************************************************************************/
/*==== DECLARATION CONTROL ===================================================*/
#pragma once

/*==== INCLUDES ==============================================================*/
#include "PlanLinac.h"
#include "PlanIgs.h"
#include "TcMath.h"

/*==== MARCOS ================================================================*/
//marcos for ouput calculate value
#define OUTPUT_PLAN_ENABLE		0
#if OUTPUT_PLAN_ENABLE
#define OUTPUT_DELTA_GANTRY_EN	0
#define OUTPUT_DELTA_DOSE_EN	0
#define OUTPUT_DELTA_MLC_EN		0

#define OUTPUT_TMAX_GANTRY_EN	0
#define OUTPUT_TMAX_DOSE_EN		0
#define OUTPUT_TMAX_MLC_EN		1

#define OUTPUT_SPEED_GANTRY_EN	0
#define OUTPUT_SPEED_DOSE_EN	0
#define OUTPUT_SPEED_MLC_EN		1
#endif

/*==== CLASS =================================================================*/
class CPlanMngr
{
public:
	CPlanMngr();
	CPlanMngr(CResourceMngr *Resource);
	~CPlanMngr();
	//Methods
	BOOL CycleUpdate(VOID);
	#if OUTPUT_PLAN_ENABLE
		VOID PrintFileInWrite(PCCH szFileName);
		BOOL FileBite;
	#endif
private:
	//Data members
	BeamPlan_t			m_BeamPlan;			
	OriginalCP_t		m_OriginalCP[MAX_CP_NUM];
	DeltaCP_t			m_DeltaCP;
	BackCalSpeedCP_t	m_BackCalSpeedCP;

	CResourceMngr*		m_ResourceMngr;
	ResComponent_t*		m_ResComponentPtr;
	BOOL				m_PlanDataRecvEn;
	BOOL				m_InitEn;

	HandlerInputs_t		m_HandlerInputs;
	CPlanLinac			Linac{ &m_HandlerInputs};
	CPlanIgs			Igs{ &m_HandlerInputs };
	//Methods members
	BOOL  PlanIsReceived(VOID); //judge TcpServer thread receive plan data is complete
	BOOL  StorePlanData(VOID); //store plan data in local
	BOOL  SetPlanReady(VOID); //notify plan ready to other control objects with resource manager
	BOOL  ClearALLPlan(VOID);
	VOID  Initialize(VOID);
	VOID  BindingResourceToMngr(VOID);
};

/*==== END OF FILE ===========================================================*/




