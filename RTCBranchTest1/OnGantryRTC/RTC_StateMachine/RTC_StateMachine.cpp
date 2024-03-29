///////////////////////////////////////////////////////////////////////////////
// RTC_StateMachine.cpp
#include "TcPch.h"
#pragma hdrstop

#include "RTC_StateMachine.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif
DEFINE_THIS_FILE()

///////////////////////////////////////////////////////////////////////////////
// CRTC_StateMachine
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
// Collection of interfaces implemented by module CRTC_StateMachine
BEGIN_INTERFACE_MAP(CRTC_StateMachine)
	INTERFACE_ENTRY_ITCOMOBJECT()
	INTERFACE_ENTRY(IID_ITcADI, ITcADI)
	INTERFACE_ENTRY(IID_ITcWatchSource, ITcWatchSource)
///<AutoGeneratedContent id="InterfaceMap">
	INTERFACE_ENTRY(IID_ITcCyclic, ITcCyclic)
///</AutoGeneratedContent>
END_INTERFACE_MAP()

IMPLEMENT_ITCOMOBJECT(CRTC_StateMachine)
IMPLEMENT_ITCOMOBJECT_SETSTATE_LOCKOP2(CRTC_StateMachine)
IMPLEMENT_ITCADI(CRTC_StateMachine)
IMPLEMENT_ITCWATCHSOURCE(CRTC_StateMachine)

///////////////////////////////////////////////////////////////////////////////
// Set parameters of CRTC_StateMachine 
BEGIN_SETOBJPARA_MAP(CRTC_StateMachine)
	SETOBJPARA_DATAAREA_MAP()
///<AutoGeneratedContent id="SetObjectParameterMap">
	SETOBJPARA_VALUE(PID_TcTraceLevel, m_TraceLevelMax)
	SETOBJPARA_ITFPTR(PID_Ctx_TaskOid, m_spCyclicCaller)
///</AutoGeneratedContent>
END_SETOBJPARA_MAP()

///////////////////////////////////////////////////////////////////////////////
// Get parameters of CRTC_StateMachine 
BEGIN_GETOBJPARA_MAP(CRTC_StateMachine)
	GETOBJPARA_DATAAREA_MAP()
///<AutoGeneratedContent id="GetObjectParameterMap">
	GETOBJPARA_VALUE(PID_TcTraceLevel, m_TraceLevelMax)
	GETOBJPARA_ITFPTR(PID_Ctx_TaskOid, m_spCyclicCaller)
///</AutoGeneratedContent>
END_GETOBJPARA_MAP()

///////////////////////////////////////////////////////////////////////////////
// Get watch entries of CRTC_StateMachine
BEGIN_OBJPARAWATCH_MAP(CRTC_StateMachine)
	OBJPARAWATCH_DATAAREA_MAP()
///<AutoGeneratedContent id="ObjectParameterWatchMap">
///</AutoGeneratedContent>
END_OBJPARAWATCH_MAP()

///////////////////////////////////////////////////////////////////////////////
// Get data area members of CRTC_StateMachine
BEGIN_OBJDATAAREA_MAP(CRTC_StateMachine)
///<AutoGeneratedContent id="ObjectDataAreaMap">
	OBJDATAAREA_VALUE(ADI_RTC_StateMachineInputsFsm, m_InputsFsm)
	OBJDATAAREA_VALUE(ADI_RTC_StateMachineOutputsFsm, m_OutputsFsm)
	OBJDATAAREA_VALUE(ADI_RTC_StateMachineInputsSubFsm, m_InputsSubFsm)
	OBJDATAAREA_VALUE(ADI_RTC_StateMachineOutputsSubFsm, m_OutputsSubFsm)
	OBJDATAAREA_VALUE(ADI_RTC_StateMachineInputsOGR, m_InputsOGR)
	OBJDATAAREA_VALUE(ADI_RTC_StateMachineOutputsOGR, m_OutputsOGR)
	OBJDATAAREA_VALUE(ADI_RTC_StateMachineInputsBeam, m_InputsBeam)
	OBJDATAAREA_VALUE(ADI_RTC_StateMachineOutputsBeam, m_OutputsBeam)
	OBJDATAAREA_VALUE(ADI_RTC_StateMachineInputsDose, m_InputsDose)
	OBJDATAAREA_VALUE(ADI_RTC_StateMachineOutputsDose, m_OutputsDose)
	OBJDATAAREA_VALUE(ADI_RTC_StateMachinePlanInputs, m_PlanInputs)
	OBJDATAAREA_VALUE(ADI_RTC_StateMachinePlanOutputs, m_PlanOutputs)
	OBJDATAAREA_VALUE(ADI_RTC_StateMachineInputsBgmPlan, m_InputsBgmPlan)
	OBJDATAAREA_VALUE(ADI_RTC_StateMachineOutputsBgmPlan, m_OutputsBgmPlan)
	OBJDATAAREA_VALUE(ADI_RTC_StateMachineInputsIgsSlv, m_InputsIgsSlv)
	OBJDATAAREA_VALUE(ADI_RTC_StateMachineOutputsIgsSlv, m_OutputsIgsSlv)
	OBJDATAAREA_VALUE(ADI_RTC_StateMachineInputsBsm, m_InputsBsm)
	OBJDATAAREA_VALUE(ADI_RTC_StateMachineOutputsBsm, m_OutputsBsm)
	OBJDATAAREA_VALUE(ADI_RTC_StateMachineInputsVMAT, m_InputsVMAT)
	OBJDATAAREA_VALUE(ADI_RTC_StateMachineOutputsVMAT, m_OutputsVMAT)
	OBJDATAAREA_VALUE(ADI_RTC_StateMachineInputsCtrlState, m_InputsCtrlState)
	OBJDATAAREA_VALUE(ADI_RTC_StateMachineOutputsCtrlState, m_OutputsCtrlState)
	OBJDATAAREA_VALUE(ADI_RTC_StateMachineInputsOnGantryHD, m_InputsOnGantryHD)
	OBJDATAAREA_VALUE(ADI_RTC_StateMachineOutputsOnGantryHD, m_OutputsOnGantryHD)
	OBJDATAAREA_VALUE(ADI_RTC_StateMachineInputsBsmFmGUI, m_InputsBsmFmGUI)
	OBJDATAAREA_VALUE(ADI_RTC_StateMachineOutptsBsmToGUI, m_OutptsBsmToGUI)
	OBJDATAAREA_VALUE(ADI_RTC_StateMachineInputsIgs, m_InputsIgs)
	OBJDATAAREA_VALUE(ADI_RTC_StateMachineOutputsIgs, m_OutputsIgs)
	OBJDATAAREA_VALUE(ADI_RTC_StateMachineFmDevices, m_FmDevices)
	OBJDATAAREA_VALUE(ADI_RTC_StateMachineIUTMonitor, m_IUTMonitor)
///</AutoGeneratedContent>
END_OBJDATAAREA_MAP()

///////////////////////////////////////////////////////////////////////////////
CRTC_StateMachine::CRTC_StateMachine()
	: m_Trace(m_TraceLevelMax, m_spSrv)
{
///<AutoGeneratedContent id="MemberInitialization">
	m_TraceLevelMax = tlAlways;
	memset(&m_InputsFsm, 0, sizeof(m_InputsFsm));
	memset(&m_OutputsFsm, 0, sizeof(m_OutputsFsm));
	memset(&m_InputsSubFsm, 0, sizeof(m_InputsSubFsm));
	memset(&m_OutputsSubFsm, 0, sizeof(m_OutputsSubFsm));
	memset(&m_InputsOGR, 0, sizeof(m_InputsOGR));
	memset(&m_OutputsOGR, 0, sizeof(m_OutputsOGR));
	memset(&m_InputsBeam, 0, sizeof(m_InputsBeam));
	memset(&m_OutputsBeam, 0, sizeof(m_OutputsBeam));
	memset(&m_InputsDose, 0, sizeof(m_InputsDose));
	memset(&m_OutputsDose, 0, sizeof(m_OutputsDose));
	memset(&m_PlanInputs, 0, sizeof(m_PlanInputs));
	memset(&m_PlanOutputs, 0, sizeof(m_PlanOutputs));
	memset(&m_InputsBgmPlan, 0, sizeof(m_InputsBgmPlan));
	memset(&m_OutputsBgmPlan, 0, sizeof(m_OutputsBgmPlan));
	memset(&m_InputsIgsSlv, 0, sizeof(m_InputsIgsSlv));
	memset(&m_OutputsIgsSlv, 0, sizeof(m_OutputsIgsSlv));
	memset(&m_InputsBsm, 0, sizeof(m_InputsBsm));
	memset(&m_OutputsBsm, 0, sizeof(m_OutputsBsm));
	memset(&m_InputsVMAT, 0, sizeof(m_InputsVMAT));
	memset(&m_OutputsVMAT, 0, sizeof(m_OutputsVMAT));
	memset(&m_InputsCtrlState, 0, sizeof(m_InputsCtrlState));
	memset(&m_OutputsCtrlState, 0, sizeof(m_OutputsCtrlState));
	memset(&m_InputsOnGantryHD, 0, sizeof(m_InputsOnGantryHD));
	memset(&m_OutputsOnGantryHD, 0, sizeof(m_OutputsOnGantryHD));
	memset(&m_InputsBsmFmGUI, 0, sizeof(m_InputsBsmFmGUI));
	memset(&m_OutptsBsmToGUI, 0, sizeof(m_OutptsBsmToGUI));
	memset(&m_InputsIgs, 0, sizeof(m_InputsIgs));
	memset(&m_OutputsIgs, 0, sizeof(m_OutputsIgs));
	memset(&m_FmDevices, 0, sizeof(m_FmDevices));
	memset(&m_IUTMonitor, 0, sizeof(m_IUTMonitor));
///</AutoGeneratedContent>
	/*== Custom Add ==========================================================*/
	/*== End =================================================================*/
}

///////////////////////////////////////////////////////////////////////////////
CRTC_StateMachine::~CRTC_StateMachine() 
{

}

///////////////////////////////////////////////////////////////////////////////
// State Transitions 
///////////////////////////////////////////////////////////////////////////////
IMPLEMENT_ITCOMOBJECT_SETOBJSTATE_IP_PI(CRTC_StateMachine)

///////////////////////////////////////////////////////////////////////////////
// State transition from PREOP to SAFEOP
//
// Initialize input parameters 
// Allocate memory
HRESULT CRTC_StateMachine::SetObjStatePS(PTComInitDataHdr pInitData)
{
	m_Trace.Log(tlVerbose, FENTERA);

	HRESULT hr = S_OK;
	IMPLEMENT_ITCOMOBJECT_EVALUATE_INITDATA(pInitData);

	/*== Custom Add ==========================================================*/
	//init file access ptr
	hr = m_spSrv->TcCreateInstance(CID_TcFileAccess, IID_ITcFileAccess, (PPVOID)&ResourceMngr.m_PlanTxtPtr);
	if (SUCCEEDED(hr))
	{
		m_Trace.Log(tlInfo, "Line:%d::Creat file access ptr succeed", __LINE__);
		ITComObjectPtr spFileAccessObj = ResourceMngr.m_PlanTxtPtr;
		hr = spFileAccessObj->TcSetObjState(TCOM_STATE_PREOP, m_spSrv, pInitData);
	}
	/*== End =================================================================*/
	m_Trace.Log(tlVerbose, FLEAVEA "hr=0x%08x", hr);
	return hr;
}

///////////////////////////////////////////////////////////////////////////////
// State transition from SAFEOP to OP
//
// Register with other TwinCAT objects
HRESULT CRTC_StateMachine::SetObjStateSO()
{
	m_Trace.Log(tlVerbose, FENTERA);
	HRESULT hr = S_OK;

	// TODO: Add any additional initialization
	// If following call is successful the CycleUpdate method will be called, 
	// possibly even before this method has been left.
	hr = FAILED(hr) ? hr : AddModuleToCaller(); 

	// Cleanup if transition failed at some stage
	if ( FAILED(hr) )
	{
		RemoveModuleFromCaller(); 
	}
	m_Trace.Log(tlVerbose, FLEAVEA "hr=0x%08x", hr);

	/*== Custom Add ==========================================================*/
	//Init resource of manager
	InitResource();

	#if OUTPUT_PLAN_ENABLE
	hr = ResourceMngr.m_PlanTxtPtr->FileOpen("%TC_TARGETPATH%PlanGantry.txt", static_cast<TcFileAccessMode>(amText | amAppend), &ResourceMngr.hFile);
	if (FAILED(hr))
	{
		ResourceMngr.m_PlanTxtPtr = NULL;
		m_Trace.Log(tlInfo, "%d::CRTC_StateMachine::<SetObjStateSO>:: Opened PlanGantry.txt", __LINE__);
		return hr;
	}
	else
	{
		m_Trace.Log(tlInfo, "%d::CRTC_StateMachine::<SetObjStateSO>:: Fail to Open PlanGantry.txt", __LINE__);
	}	
	#endif
	/*== End =================================================================*/
	return hr;
}

///////////////////////////////////////////////////////////////////////////////
// State transition from OP to SAFEOP
HRESULT CRTC_StateMachine::SetObjStateOS()
{
	m_Trace.Log(tlVerbose, FENTERA);
	HRESULT hr = S_OK;

	RemoveModuleFromCaller(); 

	m_Trace.Log(tlVerbose, FLEAVEA "hr=0x%08x", hr);
	return hr;
}

///////////////////////////////////////////////////////////////////////////////
// State transition from SAFEOP to PREOP
HRESULT CRTC_StateMachine::SetObjStateSP()
{
	m_Trace.Log(tlVerbose, FENTERA);
	HRESULT hr = S_OK;
	/*== Custom Add ==========================================================*/
	if (ResourceMngr.m_PlanTxtPtr != NULL)
	{
		ITComObjectPtr spFileAccessObj = ResourceMngr.m_PlanTxtPtr;
		spFileAccessObj->TcSetObjState(TCOM_STATE_INIT, m_spSrv, NULL);
		spFileAccessObj = NULL;
		ResourceMngr.m_PlanTxtPtr = NULL;
		safe_delete_a(ResourceMngr.m_PlanTxtPtr);
	}
	/*== End =================================================================*/
	m_Trace.Log(tlVerbose, FLEAVEA "hr=0x%08x", hr);
	return hr;
}

///<AutoGeneratedContent id="ImplementationOf_ITcCyclic">
HRESULT CRTC_StateMachine::CycleUpdate(ITcTask* ipTask, ITcUnknown* ipCaller, ULONG_PTR context)
{
	HRESULT hr = S_OK;

	/*== Custom Add ==========================================================*/
	//Get current system time
	GetCurrentSysTime(ipTask);

	//Switch current fsm
	SwitchCurrentFsm();

	//Update Resource Manager
	ResourceMngr.CycleUpdate();

	//Update FSM
	FsmPtr->CycleUpdate();

	//Update Plan Manager
	PlanMngr.CycleUpdate();

	//Update Bgm control object
	Bgm.CycleUpdate();

	//Update Bsm control object
	Bsm.CycleUpdate();
	
	//Update Igs control object
	Igs.CycleUpdate();
	/*== End =================================================================*/

	return hr;
}
///</AutoGeneratedContent>

///////////////////////////////////////////////////////////////////////////////
HRESULT CRTC_StateMachine::AddModuleToCaller()
{
	m_Trace.Log(tlVerbose, FENTERA);
	HRESULT hr = S_OK;
	if ( m_spCyclicCaller.HasOID() )
	{
		if ( SUCCEEDED_DBG(hr = m_spSrv->TcQuerySmartObjectInterface(m_spCyclicCaller)) )
		{
			if ( FAILED(hr = m_spCyclicCaller->AddModule(m_spCyclicCaller, THIS_CAST(ITcCyclic))) )
			{
				m_spCyclicCaller = NULL;
			}
		}
	}
	else
	{
		hr = ADS_E_INVALIDOBJID; 
		SUCCEEDED_DBGT(hr, "Invalid OID specified for caller task");
	}
	m_Trace.Log(tlVerbose, FLEAVEA "hr=0x%08x", hr);
	return hr;
}

///////////////////////////////////////////////////////////////////////////////
VOID CRTC_StateMachine::RemoveModuleFromCaller()
{
	m_Trace.Log(tlVerbose, FENTERA);
	if ( m_spCyclicCaller )
	{
		m_spCyclicCaller->RemoveModule(m_spCyclicCaller);
	}
	m_spCyclicCaller	= NULL;
	m_Trace.Log(tlVerbose, FLEAVEA);
}

/*== Custom Add =============================================================*/
/******************************************************************************
* Function: InitResource(VOID)
*
* Describe:
*
* Input   :
*
* Output  : None
*
* Return  : TRUE or FALSE
*
* Others  :
*******************************************************************************/
BOOL CRTC_StateMachine::InitResource(VOID)
{
	ResComponent_t Resource = {0};
	Resource.Trace = &m_Trace;

	Resource.InputsFsm = &m_InputsFsm;
	Resource.OutputsFsm = &m_OutputsFsm;
	Resource.InputsSubFsm = &m_InputsSubFsm;
	Resource.OutputsSubFsm = &m_OutputsSubFsm;
	Resource.InputsOGR = &m_InputsOGR;
	Resource.OutputsOGR = &m_OutputsOGR;
	Resource.InputsBeam = &m_InputsBeam;
	Resource.OutputsBeam = &m_OutputsBeam;
	Resource.InputsDose = &m_InputsDose;
	Resource.OutputsDose = &m_OutputsDose;
	Resource.PlanInputs = &m_PlanInputs;
	Resource.PlanOutputs = &m_PlanOutputs;
	Resource.InputsBgmPlan = &m_InputsBgmPlan;
	Resource.OutputsBgmPlan = &m_OutputsBgmPlan;
	Resource.InputsIgsSlv = &m_InputsIgsSlv;
	Resource.OutputsIgsSlv = &m_OutputsIgsSlv;
	Resource.OutputsBsm = &m_OutputsBsm;
	Resource.InputsBsm = &m_InputsBsm;
	Resource.InputsVMAT = &m_InputsVMAT;
	Resource.OutputsVMAT = &m_OutputsVMAT;
	Resource.OutputsCtrlState = &m_OutputsCtrlState;
	Resource.InputsCtrlState = &m_InputsCtrlState;
	Resource.InputsOnGantryHD = &m_InputsOnGantryHD;
	Resource.OutputsOnGantryHD = &m_OutputsOnGantryHD;
	Resource.OutputsBsmToGUI = &m_OutptsBsmToGUI;
	Resource.InputsBsmFmGUI = &m_InputsBsmFmGUI;
	Resource.InputsIgs = &m_InputsIgs;
	Resource.OutputsIgs = &m_OutputsIgs;
	Resource.FmDevices = &m_FmDevices;
	Resource.IUTMonitor = &m_IUTMonitor;
	
	ResourceMngr.SetResourceCom(Resource);
	
	return TRUE;
}

/******************************************************************************
* Function: GetCurrentSysTime(ITcTask* ipTask)
*
* Describe:
*
* Input   :
*
* Output  : None
*
* Return  : TRUE or FALSE
*
* Others  :
*******************************************************************************/
BOOL CRTC_StateMachine::GetCurrentSysTime(ITcTask* ipTask)
{
	LONGLONG CurrentTimer = 0;
	ipTask->GetCurrentSysTime( &CurrentTimer);	//get current sys time
	CurrentTimer /= 10000;
	ResourceMngr.SetCurrentSysTime(CurrentTimer);
	return TRUE;
}

/******************************************************************************
* Function: GetCurrentSysTime(VOID)
*
* Describe:
*
* Input   : None
*
* Output  : None
*******************************************************************************/
VOID CRTC_StateMachine::SwitchCurrentFsm(VOID)
{
	PlanEvent_eu fsmEvent = ResourceMngr.PlanResource.GetPlanEvent();
	FsmState_u fsmState = ResourceMngr.FsmResource.GetCurrentState();
	
	
	if (KV_PLAN_RECV == fsmEvent && RTC_PREPARATORY == fsmState)
	{
		FsmPtr->ResetStateSyncTimer();
		//run kv fsm
		FsmPtr = &FsmKv; //fsm for kv image
		FsmKv.SetRequiredState(RTC_PREPARATORY); //start state after switch
		FsmKv.SetRunState(FSM_RUNNING); //set fsm running state

		//holding fsm
		Fsm.SetRunState(FSM_HOLDING);

		ResourceMngr.PlanResource.SetPlanEvent(PLAN_EVT_NONE); //clear event
	}
	else if ((MV_PLAN_RECV == fsmEvent || CLOSE_PATIENT == fsmEvent) && RTC_PREPARATORY == fsmState)
	{
		FsmPtr->ResetStateSyncTimer();
		//run  fsm
		FsmPtr = &Fsm; //fsm for  mv
		Fsm.SetRequiredState(RTC_PREPARATORY); //start state after switch
		Fsm.SetRunState(FSM_RUNNING); //set fsm running state

		//holding fsm
		FsmKv.SetRunState(FSM_HOLDING);

		ResourceMngr.PlanResource.SetPlanEvent(PLAN_EVT_NONE); //clear event
	}
	else
	{
		//do nothing
	}

	return;
}

