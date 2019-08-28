/*******************************************************************************
** File	      : Igs.cpp
** Author : : libo
** Version : 1.0
** Environment : VS2013
** Description :
** History : 18 / 08 / 04    libo    Created
******************************************************************************* /
/* INCLUDE ===================================================================*/
#include "Igs.h"
/* METHODS====================================================================*/
/******************************************************************************
* Function: CIgs()
*
* Describe : construct function
*
* Input : None
*
* Output : None
*
* Return :
*
* Others :
*******************************************************************************/
CIgs::CIgs()
{
	m_ResourceMngr = NULL;
	m_ResComponent = NULL;
	m_RequiredState = IGS_INITIAL;
	m_CurrentState = IGS_INITIAL;
	m_LastState = IGS_INITIAL; 
	m_SimulationBite = 0;
	m_RtcCurrentState = 0;
	m_IgsWorkMode = 0;                                    
	m_IgsCntlWorkMode = IGSCNTL_NO_MODE;
	m_RequiredWorktimes = 0;                              
    m_WorkSuccessfultimes = 0;   
	m_Protocal = 0;
	m_Interlock = 0;
	m_Unreadys = 0;
	m_KvReady = FALSE;

	m_SimulationBite = 0;
	memset(&m_PlanGantry, 0, sizeof(m_PlanGantry));
	

	memset(&m_IcuPlanData, 0, sizeof(m_IcuPlanData) );
	memset(&m_IcuActualData,0,sizeof(m_IcuActualData) );
	memset(m_CollimatorData, 0, sizeof(m_CollimatorData)); 
	memset(m_KvPosition, 0, sizeof(m_KvPosition));
	memset(&m_KvActualData, 0, sizeof(m_KvActualData));

	memset(m_MvPosition, 0, sizeof(m_MvPosition));
	memset(&m_MvActualData, 0, sizeof(m_MvActualData));
	memset(&m_ExposeSequence, 0, sizeof(m_ExposeSequence));
	memset(&m_Parameter, 0, sizeof(m_Parameter));
}
/******************************************************************************
* Function: CIgs(CResourceMngr* ResourceMngr)
*
* Describe : construct function
*
* Input : None
*
* Output : None
*
* Return :
*
* Others :
*******************************************************************************/
CIgs::CIgs(CResourceMngr* ResourceMngr)
{
	m_ResourceMngr = ResourceMngr;
	m_ResComponent = ResourceMngr->GetResourceCom();
	m_RequiredState = IGS_INITIAL;
	m_CurrentState = IGS_INITIAL;
	m_LastState = IGS_INITIAL;
	m_SimulationBite = 0;
	m_RtcCurrentState = 0;
	m_IgsWorkMode = 0;
	m_IgsCntlWorkMode = IGSCNTL_NO_MODE;
	m_RequiredWorktimes = 0;
	m_WorkSuccessfultimes = 0;
	m_Protocal = 0;
	m_Interlock = 0; 
	m_Unreadys = 0;
	m_KvReady = FALSE;
	m_SimulationBite = 0;
	memset(&m_PlanGantry, 0, sizeof(m_PlanGantry));
	
	memset(&m_IcuPlanData, 0, sizeof(m_IcuPlanData));
	memset(&m_IcuActualData, 0, sizeof(m_IcuActualData));
	memset(m_CollimatorData, 0, sizeof(m_CollimatorData));
	memset(m_KvPosition, 0, sizeof(m_KvPosition));
	memset(&m_KvActualData, 0, sizeof(m_KvActualData));

	memset(m_MvPosition, 0, sizeof(m_MvPosition));
	memset(&m_MvActualData, 0, sizeof(m_MvActualData));
	memset(&m_ExposeSequence, 0, sizeof(m_ExposeSequence));
	memset(&m_Parameter, 0, sizeof(m_Parameter));
}

/******************************************************************************
* Function: ~CIgs()
*
* Describe : Destructor
*
* Input : None
*
* Output : None
*
* Return :
*
* Others :
*******************************************************************************/
CIgs::~CIgs()
{

}
/******************************************************************************
* Function: CycleUpdate()
*
* Describe : Destructor
*
* Input : None
*
* Output : None
*
* Return :
*
* Others :
*******************************************************************************/
BOOL CIgs::CycleUpdate()
{
	if ((m_ResourceMngr == NULL) || (m_ResComponent == NULL) || (m_ResComponent->Trace == NULL))
	{
		return FALSE;
	}
	else if ((m_ResComponent->InputsIgsSlv == NULL) || (m_ResComponent->OutputsIgs == NULL))
	{
        #if RTC_DEBUG_EXCEPTION
		m_ResComponent->Trace->Log(tlInfo, "Line:%d::CycleUpdate::Resource is not ready::%d", __LINE__);
        #endif
		return FALSE;

	}
	else
	{
		GetRtcCurrentState();
		GetState();
		SetState();
		SendRequireState();
		GetRTFieldData();
		GetInterlock();
		GetNotready();
		OutputReady();
	}

	return  TRUE;
}
/******************************************************************************
* Function: SetState()
*
* Describe : Destructor
*
* Input : None
*
* Output : None
*
* Return :
*
* Others :
*******************************************************************************/
VOID CIgs::SetState()
{
	if (m_CurrentState == IGS_FAULT )    // igs  currentstate  is fault
	{
		SendResetSigial();
		KVExposeTimer.ResetTimer();
		m_RequiredState = IGS_STANDBY;
	}
	

	switch (m_RequiredState)
	{
		case IGS_INITIAL:
	    {
			InitialMain();   // initial  main
			break;
	    }
		case IGS_STANDBY:
		{
			StandbyMain();  // standby main
			break;
		}
		case IGS_PREPARE:
		{
			PrepareMain();  //prepare  main
			break;
		}
		case IGS_READY:
		{
			ReadyMain();   // ready  main
			break;
		}
		case IGS_EXPOSE:
		{
			ExposeMain();   // expose main
			break;
		}
		case IGS_FINISH:
		{
			FinishMain();   // finish main
			break;
		}
		case IGS_FAULT:
		{
			break;
		}
		default:
		{
            #if RTC_DEBUG_EXCEPTION
			m_ResComponent->Trace->Log(tlInfo, "Line:%d::CIgs::SetState::next_state::Unknown state::%d", __LINE__, m_RequiredState);
            #endif

			break;
		}

		

	}
	
}

/******************************************************************************
* Function: GetState()
*
* Describe : Destructor
*
* Input : None
*
* Output : None
*
* Return :
*
* Others :
*******************************************************************************/
VOID CIgs::GetState()
{
	m_CurrentState = (IgsState_eu) (m_ResComponent->InputsIgsSlv->CurrentState & 0x7f);
	m_LastState = (IgsState_eu )(m_ResComponent->InputsIgsSlv->LastState & 0x7f);
	m_ResourceMngr->IgsResource.SetCurrentState(m_CurrentState);

}

/******************************************************************************
* Function: InitialMain()
*
* Describe : Destructor
*
* Input : None
*
* Output : None
*
* Return :
*
* Others :
*******************************************************************************/
VOID CIgs::InitialMain(VOID)
{
	if (m_CurrentState == IGS_STANDBY)
	{
		m_RequiredState = IGS_STANDBY;
	}
}
/******************************************************************************
* Function: StandbyMain()
*
* Describe : Destructor
*
* Input : None
*
* Output : None
*
* Return :
*
* Others :
*******************************************************************************/
VOID CIgs::StandbyMain(VOID)
{
	OutputInitialstate();
	SendResetSigial();
	GetParameter();
	SetParameter();
	if (m_ResourceMngr->PlanResource.PlanIsReady())
	{
		GetPlan();
		GetGantryTarPosition();
		SetWorkMode();
		SendPlan();
		IsJumpToPrePare();
	}
	else
	{
		CleanPlan();
	}
	//IsJumpToFault();
}
/******************************************************************************
* Function: PrepareMain()
*
* Describe : Destructor
*
* Input : None
*
* Output : None
*
* Return :
*
* Others :
*******************************************************************************/
VOID CIgs::PrepareMain(VOID)
{
	if (m_CurrentState == IGS_READY)
	{
		m_RequiredState = IGS_READY;
	}



}
/******************************************************************************
* Function: ReadyMain()
*
* Describe : Destructor
*
* Input : None
*
* Output : None
*
* Return :
*
* Others :
*******************************************************************************/
VOID CIgs::ReadyMain(VOID)
{
	
	if (m_CurrentState == IGS_READY)
	{
        
		if (m_RtcCurrentState == RTC_PREPARATORY)
		{
			m_RequiredState = IGS_STANDBY;
		}
		else if (m_RtcCurrentState == RTC_RADIATION)
		{
			m_RequiredState = IGS_EXPOSE;
		}
	}

}
/******************************************************************************
* Function: ExposeMain()
*
* Describe : Destructor
*
* Input : None
*
* Output : None
*
* Return :
*
* Others :
*******************************************************************************/
VOID CIgs::ExposeMain(VOID)
{
	if (m_CurrentState == IGS_EXPOSE)
	{
		IsJumpToFault();
		
		
		if (m_IgsCntlWorkMode == IGSCNTL_CBCT)
		{
			m_ResComponent->OutputsIgs->ExpolistLong = m_PlanGantry.CbctExposeNum;
			GetExposeList();
		}
		else if (m_IgsCntlWorkMode == IGSCNTL_KV)
		{
			m_ResComponent->OutputsIgs->ExpolistLong = 1;
			GetKvExposeList();
			if (KVExposeTimer.IsReady())
			{
				KVExposeTimer.StartTimer(m_ResourceMngr->GetCurrentSysTime(), KV_EXPOSE_TIME);
			}
			else if (KVExposeTimer.IsTimeOut(m_ResourceMngr->GetCurrentSysTime()))
			{
                #if IGS_DEBUG_TRACE
				m_ResComponent->Trace->Log(tlInfo, "Line:%d::CIgs::<ExposeMain>:: kv is timeout ::m_RequiredState = %d", __LINE__, m_RequiredState);
                #endif
				KVExposeTimer.ResetTimer();
				m_RequiredState = IGS_FAULT;
			}
			

		}

	}
	IsJumpToFinish();
}
/******************************************************************************
* Function: FinishMain()
*
* Describe : Destructor
*
* Input : None
*
* Output : None
*
* Return :
*
* Others :
*******************************************************************************/
VOID CIgs::FinishMain(VOID)
{
	if (m_CurrentState == IGS_FINISH)
	{
		if (m_RtcCurrentState == RTC_PREPARATORY)
		{
			m_RequiredState = IGS_STANDBY;
		}
		else
		{

		}

	}


}


/******************************************************************************
* Function: GetPlan()
*
* Describe : Destructor
*
* Input : None
*
* Output : None
*
* Return :
*
* Others :
*******************************************************************************/
BOOL CIgs::GetPlan()
{
	/* get ICU  plan From GUI  */
	m_IcuPlanData.FocusExp = m_ResComponent->InputsIgs->IcuFocusExp;
	m_IcuPlanData.FrequencyExp = m_ResComponent->InputsIgs->IcuFrequencyExp;
	m_IcuPlanData.KVExp = m_ResComponent->InputsIgs->IcuKVExp;
	m_IcuPlanData.MAExp = m_ResComponent->InputsIgs->IcuMAExp;
	m_IcuPlanData.MSExp = m_ResComponent->InputsIgs->IcuMSExp;
	m_IgsWorkMode = m_ResComponent->InputsIgs->WorkMode;
	/*get Collimator  plan From GUI */
	m_CollimatorData[COLLIMATOR_CROSS] = m_ResComponent->InputsIgs->CollimatorCross;
	m_CollimatorData[COLLIMATOR_LONG] = m_ResComponent->InputsIgs->CollimatorLong;
	m_CollimatorData[COLLIMATOR_OFFSET_CROSS] = m_ResComponent->InputsIgs->CollimatorOffsetCross;
	m_CollimatorData[COLLIMATOR_OFFSET_LONG] = m_ResComponent->InputsIgs->CollimatorOffsetLong;

	m_Protocal = m_ResComponent->InputsIgs->Protocal;

	return TRUE;
}
/******************************************************************************
* Function: SetParameter()
*
* Describe : Destructor
*
* Input : None
*
* Output : None
*******************************************************************************/
VOID CIgs::SetParameter()
{
	m_ResComponent->OutputsIgsSlv->KVOffset1 = m_Parameter.KvOffset1;
	m_ResComponent->OutputsIgsSlv->KVOffset2 = m_Parameter.KvOffset2;
	m_ResComponent->OutputsIgsSlv->KvTimeout = m_Parameter.KvTimeOut;
	m_ResComponent->OutputsIgsSlv->MVOffset1 = m_Parameter.MvOffset1;
	m_ResComponent->OutputsIgsSlv->MVOffset2 = m_Parameter.MvOffset2;
	m_ResComponent->OutputsIgsSlv->MvTimeout = m_Parameter.MvTimeOut;
	
}

/******************************************************************************
* Function: SetWorkMode()
*
* Describe : Destructor
*
* Input : None
*
* Output : None
*******************************************************************************/
BOOL CIgs::SetWorkMode()
{
	/*if (m_WorkSuccessfultimes ==  0)
	{
		if (m_IgsWorkMode == IGS_CBCT)
		{
			m_RequiredWorktimes = 1;
			m_IgsCntlWorkMode = IGSCNTL_CBCT;
		}
		else
		{
			m_RequiredWorktimes = 2;
		}

		if (m_IgsWorkMode == IGS_KV_KV || m_IgsWorkMode == IGS_KV_MV)
		{
			m_IgsCntlWorkMode = IGSCNTL_KV;
		}
		else if (m_IgsWorkMode == IGS_MV_MV )
		{
			m_IgsCntlWorkMode = IGSCNTL_MV;
		}
	}
	else if (m_WorkSuccessfultimes == m_RequiredWorktimes)
	{
		return TRUE;
	}
	else if (m_WorkSuccessfultimes == 1)
	{
		if (m_IgsWorkMode == IGS_KV_KV )
		{
			m_IgsCntlWorkMode = IGSCNTL_KV;
		}
		else if (m_IgsWorkMode == IGS_MV_MV || m_IgsWorkMode == IGS_KV_MV)
		{
			m_IgsCntlWorkMode = IGSCNTL_MV;
		}
	}
	else
	{
		// do nothing 
	}*/

	if (m_IgsWorkMode == IGS_CBCT)
	{
		m_IgsCntlWorkMode = IGSCNTL_CBCT;
	}
	else if (m_IgsWorkMode == IGS_KV_KV)
	{
		m_IgsCntlWorkMode = IGSCNTL_KV;
	}
	else if (m_IgsWorkMode == IGS_MV_MV)
	{
		m_IgsCntlWorkMode = IGSCNTL_MV;
	}
	
	m_ResComponent->OutputsIgsSlv->WorkMode = (BYTE)m_IgsCntlWorkMode;
	return FALSE;
}
/******************************************************************************
* Function:SendPlan();                                  
*
* Describe : Destructor
*
* Input : None
*
* Output : None
*******************************************************************************/
BOOL CIgs::SendPlan(VOID)                                     // send Plan
{
	// work mode
	m_ResComponent->OutputsIgsSlv->WorkMode = m_IgsCntlWorkMode;

	// ICU  data
	m_ResComponent->OutputsIgsSlv->IcuFocusExp = m_IcuPlanData.FocusExp;
	m_ResComponent->OutputsIgsSlv->IcuFrequencyExp = m_IcuPlanData.FrequencyExp;
	m_ResComponent->OutputsIgsSlv->IcuKVExp = m_IcuPlanData.KVExp;
	m_ResComponent->OutputsIgsSlv->IcuMAExp = m_IcuPlanData.MAExp;
	m_ResComponent->OutputsIgsSlv->IcuMSExp = m_IcuPlanData.MSExp;
	
	// Collimator data
	m_ResComponent->OutputsIgsSlv->CollimatorCross = m_CollimatorData[COLLIMATOR_CROSS];
	m_ResComponent->OutputsIgsSlv->CollimatorLong = m_CollimatorData[COLLIMATOR_LONG];
	m_ResComponent->OutputsIgsSlv->CollimatorOffsetCross = m_CollimatorData[COLLIMATOR_OFFSET_CROSS];
	m_ResComponent->OutputsIgsSlv->CollimatorOffsetLong = m_CollimatorData[COLLIMATOR_OFFSET_LONG];
	// panel position
	SendPanelPosition();

	return TRUE;

}
/******************************************************************************
* Function:SendPlan();
*
* Describe : Destructor
*
* Input : None
*
* Output : None
*******************************************************************************/
VOID CIgs::GetParameter(VOID)
{
	m_Parameter.KvOffset1 = m_ResComponent->InputsIgs->KVOffset1;
	m_Parameter.KvOffset2 = m_ResComponent->InputsIgs->KVOffset2;
	m_Parameter.KvTimeOut = m_ResComponent->InputsIgs->KVTimeOut;
	m_Parameter.MvOffset1 = m_ResComponent->InputsIgs->MVOffset1;
	m_Parameter.MvOffset2 = m_ResComponent->InputsIgs->MVOffset2;
	m_Parameter.MvTimeOut = m_ResComponent->InputsIgs->MVTimeOut;
	/* get panel position  plan From GUI */
	m_KvPosition[HALF_WILD] = m_ResComponent->InputsIgs->KvWorkPosH;
	m_KvPosition[FULL_WILD] = m_ResComponent->InputsIgs->KvWorkPosF;
	m_MvPosition[0] = m_ResComponent->InputsIgs->MvWorkPosX;
	m_MvPosition[1] = m_ResComponent->InputsIgs->MvWorkPosY;
}
/******************************************************************************
* Function:SendRequireState();
*
* Describe : Destructor
*
* Input : None
*
* Output : None
*******************************************************************************/
BOOL CIgs::SendRequireState(VOID)                             // send requirestate to Igs
{
	m_ResComponent->OutputsIgsSlv->RequiredState = m_RequiredState;
	if( m_ResComponent->InputsCtrlState == NULL)
	{
       #if IGS_DEBUG_TRACE
		m_ResComponent->Trace->Log(tlInfo, "Line%d::CIgs ::<SendRequireState>::get  Source  m_CusResource->m_InputsCtrlState parameter is invaild", __LINE__);
        #endif
		return FALSE;
	}
	else
	{


		UINT8 TempState = 0;
		TempState = SetSimulationState();
		
		if (m_LastRequireState != TempState)
		{
			if (m_StateTimer.IsReady() && m_RequiredState == IGS_FINISH && m_IgsCntlWorkMode == IGSCNTL_CBCT)
			{
				m_ResComponent->OutputsIgsSlv->RequiredState = TempState;  //  chenge  state
				m_StateTimer.StartTimer(m_ResourceMngr->GetCurrentSysTime(), IGS_CBCT_EXPOSE_REQUIRESTATE_TIMEOUT);
				m_LastRequireState = TempState;
			}
			else if (m_StateTimer.IsReady())
			{
				m_ResComponent->OutputsIgsSlv->RequiredState = TempState;  //  chenge  state
				m_StateTimer.StartTimer(m_ResourceMngr->GetCurrentSysTime(), IGS_REQUIRESTATE_TIMEOUT);
				m_LastRequireState = TempState;
			}

		}

		if (m_LastRequireState != m_ResComponent->InputsIgsSlv->CurrentState)
		{
			if (m_StateTimer.IsTimeOut(m_ResourceMngr->GetCurrentSysTime()))
			{
				m_Unreadys |= IGS_UNREADY;
				m_RequiredState = IGS_FAULT;
				m_StateTimer.ResetTimer();
                #if RTC_DEBUG_TRACE
				m_ResComponent->Trace->Log(tlInfo, "Line%u::CIgs::<SendRequireState>::RequireState = %u send IsTimeOut CurrentState = %d  ", \
					__LINE__, m_LastRequireState, m_ResComponent->InputsIgsSlv->CurrentState);
                #endif
			}
			else
			{
				// do nothing
			}


		}
		else
		{
			m_StateTimer.ResetTimer();
			m_Unreadys &= ~IGS_UNREADY;
		}

		if (m_ResourceMngr->FsmResource.GuiCmd.Reset)
		{
			m_StateTimer.ResetTimer();
			m_Unreadys &= ~IGS_UNREADY;
		}
		else
		{
			// do nothing
		}
	}
	
	
	return TRUE;
}
/******************************************************************************
* Function:GetRTFieldData();
*
* Describe : Destructor
*
* Input : None
*
* Output : None
*******************************************************************************/

VOID  CIgs::GetRTFieldData(VOID)                               // Get RT Field to GUI
{
	m_ResComponent->OutputsIgs->KvPosReady = m_ResComponent->InputsIgsSlv->KVPosReady;
	m_ResComponent->OutputsIgs->MvPosReady = m_ResComponent->InputsIgsSlv->MVPosReady;
	m_ResComponent->OutputsIgs->CollimatorPosReady = m_ResComponent->InputsIgsSlv->CollimatorPosReady;
	m_ResComponent->OutputsIgs->KvPosition = m_ResComponent->InputsIgsSlv->KVPosActual;
	m_ResComponent->OutputsIgs->MvPosition = m_ResComponent->InputsIgsSlv->MVPosActual;
	m_ResComponent->OutputsIgs->KVOffset1 = m_ResComponent->InputsIgsSlv->KVEncode1;
	m_ResComponent->OutputsIgs->KVOffset2 = m_ResComponent->InputsIgsSlv->KVEncode2;
	m_ResComponent->OutputsIgs->MVOffset1 = m_ResComponent->InputsIgsSlv->MVEncode1;
	m_ResComponent->OutputsIgs->MVOffset2 = m_ResComponent->InputsIgsSlv->MVEncode2;
	m_ResComponent->OutputsIgs->IcuFocusExp = m_ResComponent->InputsIgsSlv->IcuFocusExpActual;
	m_ResComponent->OutputsIgs->IcuFrequencyExp = m_ResComponent->InputsIgsSlv->IcuFrequencyExpActual;
	m_ResComponent->OutputsIgs->IcuKVExp = m_ResComponent->InputsIgsSlv->IcuKVExpActual;
	m_ResComponent->OutputsIgs->IcuMAExp = m_ResComponent->InputsIgsSlv->IcuMAExpActual;
	m_ResComponent->OutputsIgs->IcuMSExp= m_ResComponent->InputsIgsSlv->IcuMSExpActual;

	m_ResComponent->OutputsIgs->CollimatorCross = m_ResComponent->InputsIgsSlv->CollimatorCross;
	m_ResComponent->OutputsIgs->CollimatorLong = m_ResComponent->InputsIgsSlv->CollimatorLong;
	m_ResComponent->OutputsIgs->CollimatorOffsetCross = m_ResComponent->InputsIgsSlv->CollimatorOffsetCross;
	m_ResComponent->OutputsIgs->CollimatorOffsetLong = m_ResComponent->InputsIgsSlv->CollimatorOffsetLong;

}
/******************************************************************************
* Function:IsJumpToPrePare();
*
* Describe : Destructor
*
* Input : None
*
* Output : None
*******************************************************************************/
BOOL CIgs:: IsJumpToPrePare(VOID)
{
		BeamPlan_t* BeamPlan = m_ResourceMngr->PlanResource.GetBeamPlanData();

		 if (BeamPlan->RadiationType == RadiationKv)
		{
			if ((m_IgsCntlWorkMode == IGSCNTL_CBCT || m_IgsCntlWorkMode == IGSCNTL_KV) && (m_RtcCurrentState == RTC_PREPARE) && KvIsReady())
			{
				m_RequiredState = IGS_PREPARE;
			}
			else if (m_IgsCntlWorkMode == IGSCNTL_MV && (m_RtcCurrentState == RTC_PREPARE) && m_ResComponent->OutputsIgs->MvPosReady)
			{
				m_RequiredState = IGS_PREPARE;
			}
			else
			{
				m_RequiredState = IGS_STANDBY;
				return FALSE;
			}

		}
		 else
		 {
			 m_RequiredState = IGS_STANDBY;
			 // do nothing
		 }

	return TRUE;
}
/******************************************************************************
* Function:IsJumpToFault();
*
* Describe : Destructor
*
* Input : None
*
* Output : None
*******************************************************************************/

BOOL CIgs::IsJumpToFault(VOID)
{
	if (m_RtcCurrentState == RTC_OFF || m_RtcCurrentState == RTC_TERMINATE)
	{
         #if IGS_DEBUG_TRACE
	     m_ResComponent->Trace->Log(tlInfo, "Line:%d::CIgs::<IsJumpToFaul>::Rtc  let  jump fault ::m_RequiredState = %d", __LINE__, m_RequiredState);
         #endif
		m_RequiredState = IGS_FAULT;
	}
	else
	{
		return  FALSE;
	}

	return  TRUE;
}
/******************************************************************************
* Function:IsJumpToFinish();
*
* Describe : Destructor
*
* Input : None
*
* Output : None
*******************************************************************************/

BOOL CIgs::IsJumpToFinish(VOID)
{
	// CBCT  jump to finish
	

	UINT16  TempPosition = m_ResComponent->InputsOGR->GantryAngle;
	if (m_ResComponent->InputsOGR->GantryAngle == 36000)
	{
		TempPosition = 0;
	}
	
	
	if ((abs(m_PlanGantry.GantryTarPosition - TempPosition) % 36000 <= 10) && (m_IgsCntlWorkMode == IGSCNTL_CBCT)  \
		&& m_PlanGantry.LastRadiationIndex == m_ResComponent->InputsOGR->RadiationIndex)  // 
	{
		m_RequiredState = IGS_FINISH;
	}
	else if (m_IgsCntlWorkMode == IGSCNTL_KV && m_CurrentState == IGS_FINISH)   // KV_KV jump to finish
	{
		KVExposeTimer.ResetTimer();
		m_RequiredState = IGS_FINISH;
	}
	else
	{
		return   FALSE;
	}


	return  TRUE;
}
/******************************************************************************
* Function:SendResetSigial();
*
* Describe : Destructor
*
* Input : None
*
* Output : None
*******************************************************************************/
VOID CIgs::SendResetSigial(VOID)                                  // send  reset Sigial from  GUI to Igs
{
	m_ResComponent->OutputsIgsSlv->Reset = (bool)m_ResourceMngr->FsmResource.GuiCmd.Reset;
}
/******************************************************************************
* Function:SendResetSigial();
*
* Describe : Destructor
*
* Input : None
*
* Output : None
*******************************************************************************/
VOID CIgs::GetRtcCurrentState(VOID)
{
	m_RtcCurrentState = m_ResourceMngr->FsmResource.GetCurrentState();
}
/******************************************************************************
* Function::GetExposeList();
*
* Describe : Destructor
*
* Input : None
*
* Output : None
*******************************************************************************/

VOID CIgs::GetExposeList(VOID)     // 
{
	/*
	if (m_ResComponent->OutputsIgs->ExpolistLong != 0)
	{
		INT32 i = 0, j = 0;
		for (i = 0; i < SIZE_EXPOSE_LIST_GROUP; i++)
		{
			for (j = 0; j < SIZE_EXPOSE_LIST; j++ )
			{
				m_ResComponent->OutputsIgs->ExposeList[i][j].GantryPosition = (FLOAT)i*SIZE_EXPOSE_LIST_GROUP + j;
				m_ResComponent->OutputsIgs->ExposeList[i][j].Dose = (FLOAT)i*SIZE_EXPOSE_LIST_GROUP + j + 1;
			}
		}
	}
	else
	{
		memset(m_ResComponent->OutputsIgs->ExposeList, 0, sizeof(m_ResComponent->OutputsIgs->ExposeList));
	}
	*/
	/*
	UINT16 ArrayNum1 = 0, ArrayNum2 = 0;
	if (m_ExposeSequence.RecordNum < SIZE_EXPOSE_LIST * SIZE_EXPOSE_LIST_GROUP )
	{
		ArrayNum1 = m_ExposeSequence.RecordNum % SIZE_EXPOSE_LIST;
		ArrayNum2 = m_ExposeSequence.RecordNum / SIZE_EXPOSE_LIST;

		m_ExposeSequence.GantryExposeList[ArrayNum2][ArrayNum1].GantryPosition = m_ResComponent->InputsOGR->GantryAngle*0.01;
		m_ExposeSequence.GantryExposeList[ArrayNum2][ArrayNum1].GantrySpeed = m_ResComponent->InputsOGR->GantrySpeed*0.01;
		SYSTEMTIME stTaskSysTime;
		TcFileTimeToSystemTime(m_ResourceMngr->GetCurrentSysTime() * 10000, &stTaskSysTime);

		m_ExposeSequence.GantryExposeList[ArrayNum2][ArrayNum1].Hour = (BYTE)(stTaskSysTime.wHour + 8) % 24;
		m_ExposeSequence.GantryExposeList[ArrayNum2][ArrayNum1].Minute = (BYTE)stTaskSysTime.wMinute;
		m_ExposeSequence.GantryExposeList[ArrayNum2][ArrayNum1].Second = (BYTE)stTaskSysTime.wSecond;
		m_ExposeSequence.GantryExposeList[ArrayNum2][ArrayNum1].Millisecond = stTaskSysTime.wMilliseconds;
		m_ExposeSequence.GantryExposeList[ArrayNum2][ArrayNum1].CollimatorType = 2;
		m_ExposeSequence.GantryExposeList[ArrayNum2][ArrayNum1].PanelType = 1;
		m_ExposeSequence.GantryExposeList[ArrayNum2][ArrayNum1].PanelPosition = m_ExposeSequence.RecordNum;
		m_ExposeSequence.GantryExposeList[ArrayNum2][ArrayNum1].OpenSourceTimer = 0;

		m_ResComponent->OutputsIgs->GantryExposeList[ArrayNum2][ArrayNum1].GantryPosition = m_ExposeSequence.GantryExposeList[ArrayNum2][ArrayNum1].GantryPosition;
		m_ResComponent->OutputsIgs->GantryExposeList[ArrayNum2][ArrayNum1].GantrySpeed = m_ExposeSequence.GantryExposeList[ArrayNum2][ArrayNum1].GantrySpeed;
		m_ResComponent->OutputsIgs->GantryExposeList[ArrayNum2][ArrayNum1].Hour = m_ExposeSequence.GantryExposeList[ArrayNum2][ArrayNum1].Hour;
		m_ResComponent->OutputsIgs->GantryExposeList[ArrayNum2][ArrayNum1].Minute = m_ExposeSequence.GantryExposeList[ArrayNum2][ArrayNum1].Minute;
		m_ResComponent->OutputsIgs->GantryExposeList[ArrayNum2][ArrayNum1].Second = m_ExposeSequence.GantryExposeList[ArrayNum2][ArrayNum1].Second;
		m_ResComponent->OutputsIgs->GantryExposeList[ArrayNum2][ArrayNum1].Millisecond = m_ExposeSequence.GantryExposeList[ArrayNum2][ArrayNum1].Millisecond;
		m_ResComponent->OutputsIgs->GantryExposeList[ArrayNum2][ArrayNum1].CollimatorType = m_ExposeSequence.GantryExposeList[ArrayNum2][ArrayNum1].CollimatorType;
		m_ResComponent->OutputsIgs->GantryExposeList[ArrayNum2][ArrayNum1].PanelType = m_ExposeSequence.GantryExposeList[ArrayNum2][ArrayNum1].PanelType;
		m_ResComponent->OutputsIgs->GantryExposeList[ArrayNum2][ArrayNum1].PanelPosition = m_ExposeSequence.GantryExposeList[ArrayNum2][ArrayNum1].PanelPosition;
		m_ResComponent->OutputsIgs->GantryExposeList[ArrayNum2][ArrayNum1].OpenSourceTimer = m_ExposeSequence.GantryExposeList[ArrayNum2][ArrayNum1].OpenSourceTimer;

		m_ExposeSequence.RecordNum++;
		m_ResComponent->OutputsIgs->ExposeListNum = ArrayNum1 + 1;
		m_ResComponent->OutputsIgs->ExposeListGroup = ArrayNum2 + 1;


	}



*/
	
	

	
	

	if (m_ResComponent->InputsIgsSlv->RecordCounter > 0 && m_ResComponent->InputsIgsSlv->RecordCounter < SIZE_EXPOSE_LIST * SIZE_EXPOSE_LIST_GROUP)
	{
		UINT16 ArrayNum1 = 0, ArrayNum2 = 0;

		if (!m_ExposeSequence.CurrentRecordFlag)
		{
			ArrayNum1 = m_ExposeSequence.RecordCounter %  SIZE_EXPOSE_LIST;
			ArrayNum2 = m_ExposeSequence.RecordCounter / SIZE_EXPOSE_LIST;
			m_ExposeSequence.RecordNum = m_ResComponent->InputsIgsSlv->RecordCounter;
			m_ExposeSequence.GantryExposeList[ArrayNum2][ArrayNum1].GantryPosition = static_cast<FLOAT>(m_ResComponent->InputsOGR->GantryAngle*0.01);
			m_ExposeSequence.GantryExposeList[ArrayNum2][ArrayNum1].GantrySpeed =    static_cast<FLOAT> (m_ResComponent->InputsOGR->GantrySpeed*0.01);
			SYSTEMTIME stTaskSysTime;
			TcFileTimeToSystemTime(m_ResourceMngr->GetCurrentSysTime() * 10000, &stTaskSysTime);

			m_ExposeSequence.GantryExposeList[ArrayNum2][ArrayNum1].Hour = (BYTE)(stTaskSysTime.wHour + 8) % 24;
			m_ExposeSequence.GantryExposeList[ArrayNum2][ArrayNum1].Minute = (BYTE)stTaskSysTime.wMinute;
			m_ExposeSequence.GantryExposeList[ArrayNum2][ArrayNum1].Second = (BYTE)stTaskSysTime.wSecond;
			m_ExposeSequence.GantryExposeList[ArrayNum2][ArrayNum1].Millisecond = stTaskSysTime.wMilliseconds;
			m_ExposeSequence.GantryExposeList[ArrayNum2][ArrayNum1].CollimatorType = 1;
			m_ExposeSequence.GantryExposeList[ArrayNum2][ArrayNum1].PanelType = 0;
			m_ExposeSequence.GantryExposeList[ArrayNum2][ArrayNum1].PanelPosition = m_ResComponent->InputsIgsSlv->KVPosActual;
			m_ExposeSequence.GantryExposeList[ArrayNum2][ArrayNum1].OpenSourceTimer = 0;

			m_ResComponent->OutputsIgs->GantryExposeList[ArrayNum2][ArrayNum1].GantryPosition = m_ExposeSequence.GantryExposeList[ArrayNum2][ArrayNum1].GantryPosition;
			m_ResComponent->OutputsIgs->GantryExposeList[ArrayNum2][ArrayNum1].GantrySpeed = m_ExposeSequence.GantryExposeList[ArrayNum2][ArrayNum1].GantrySpeed;
			m_ResComponent->OutputsIgs->GantryExposeList[ArrayNum2][ArrayNum1].Hour = m_ExposeSequence.GantryExposeList[ArrayNum2][ArrayNum1].Hour;
			m_ResComponent->OutputsIgs->GantryExposeList[ArrayNum2][ArrayNum1].Minute = m_ExposeSequence.GantryExposeList[ArrayNum2][ArrayNum1].Minute;
			m_ResComponent->OutputsIgs->GantryExposeList[ArrayNum2][ArrayNum1].Second = m_ExposeSequence.GantryExposeList[ArrayNum2][ArrayNum1].Second;
			m_ResComponent->OutputsIgs->GantryExposeList[ArrayNum2][ArrayNum1].Millisecond = m_ExposeSequence.GantryExposeList[ArrayNum2][ArrayNum1].Millisecond;
			m_ResComponent->OutputsIgs->GantryExposeList[ArrayNum2][ArrayNum1].CollimatorType = m_ExposeSequence.GantryExposeList[ArrayNum2][ArrayNum1].CollimatorType;
			m_ResComponent->OutputsIgs->GantryExposeList[ArrayNum2][ArrayNum1].PanelType = m_ExposeSequence.GantryExposeList[ArrayNum2][ArrayNum1].PanelType;
			m_ResComponent->OutputsIgs->GantryExposeList[ArrayNum2][ArrayNum1].PanelPosition = m_ExposeSequence.GantryExposeList[ArrayNum2][ArrayNum1].PanelPosition;
			m_ResComponent->OutputsIgs->GantryExposeList[ArrayNum2][ArrayNum1].OpenSourceTimer = m_ExposeSequence.GantryExposeList[ArrayNum2][ArrayNum1].OpenSourceTimer;
			
			m_ExposeSequence.RecordCounter++;
			//m_ResComponent->Trace->Log(tlInfo, "Line:%d::CIgs::SetState::next_state::m_ExposeSequence.RecordNum  = %d", __LINE__, m_ExposeSequence.RecordCounter);
			m_ResComponent->OutputsIgs->ExposeListNum = ArrayNum1 + 1;
			m_ResComponent->OutputsIgs->ExposeListGroup = ArrayNum2 + 1;


			m_ExposeSequence.CurrentRecordFlag = TRUE;
		}

		if (m_ExposeSequence.RecordNum != m_ResComponent->InputsIgsSlv->RecordCounter)
		{
			m_ExposeSequence.CurrentRecordFlag = FALSE;
		}

	}
	
}


/******************************************************************************
* Function::GetExposeList();
*
* Describe : Destructor
*
* Input : None
*
* Output : None
*******************************************************************************/

VOID CIgs::GetKvExposeList(VOID)     // 
{
	if (m_ResComponent->InputsIgsSlv->RecordFlag  &&  m_ExposeSequence.RecordNum < 1)
	{
		UINT16 ArrayNum1 = 0, ArrayNum2 = 0;
		m_ExposeSequence.GantryExposeList[ArrayNum2][ArrayNum1].GantryPosition = static_cast<FLOAT>(m_ResComponent->InputsOGR->GantryAngle*0.01);
		m_ExposeSequence.GantryExposeList[ArrayNum2][ArrayNum1].GantrySpeed = static_cast<FLOAT>(m_ResComponent->InputsOGR->GantrySpeed*0.01);
		SYSTEMTIME stTaskSysTime;
		TcFileTimeToSystemTime(m_ResourceMngr->GetCurrentSysTime() * 10000, &stTaskSysTime);

		m_ExposeSequence.GantryExposeList[ArrayNum2][ArrayNum1].Hour = (BYTE)(stTaskSysTime.wHour + 8) % 24;
		m_ExposeSequence.GantryExposeList[ArrayNum2][ArrayNum1].Minute = (BYTE)stTaskSysTime.wMinute;
		m_ExposeSequence.GantryExposeList[ArrayNum2][ArrayNum1].Second = (BYTE)stTaskSysTime.wSecond;
		m_ExposeSequence.GantryExposeList[ArrayNum2][ArrayNum1].Millisecond = stTaskSysTime.wMilliseconds;
		m_ExposeSequence.GantryExposeList[ArrayNum2][ArrayNum1].CollimatorType = 2;
		m_ExposeSequence.GantryExposeList[ArrayNum2][ArrayNum1].PanelType = 1;
		m_ExposeSequence.GantryExposeList[ArrayNum2][ArrayNum1].PanelPosition = m_ResComponent->InputsIgsSlv->KVPosActual;;
		m_ExposeSequence.GantryExposeList[ArrayNum2][ArrayNum1].OpenSourceTimer = 0;

		m_ResComponent->OutputsIgs->GantryExposeList[ArrayNum2][ArrayNum1].GantryPosition = m_ExposeSequence.GantryExposeList[ArrayNum2][ArrayNum1].GantryPosition;
		m_ResComponent->OutputsIgs->GantryExposeList[ArrayNum2][ArrayNum1].GantrySpeed = m_ExposeSequence.GantryExposeList[ArrayNum2][ArrayNum1].GantrySpeed;
		m_ResComponent->OutputsIgs->GantryExposeList[ArrayNum2][ArrayNum1].Hour = m_ExposeSequence.GantryExposeList[ArrayNum2][ArrayNum1].Hour;
		m_ResComponent->OutputsIgs->GantryExposeList[ArrayNum2][ArrayNum1].Minute = m_ExposeSequence.GantryExposeList[ArrayNum2][ArrayNum1].Minute;
		m_ResComponent->OutputsIgs->GantryExposeList[ArrayNum2][ArrayNum1].Second = m_ExposeSequence.GantryExposeList[ArrayNum2][ArrayNum1].Second;
		m_ResComponent->OutputsIgs->GantryExposeList[ArrayNum2][ArrayNum1].Millisecond = m_ExposeSequence.GantryExposeList[ArrayNum2][ArrayNum1].Millisecond;
		m_ResComponent->OutputsIgs->GantryExposeList[ArrayNum2][ArrayNum1].CollimatorType = m_ExposeSequence.GantryExposeList[ArrayNum2][ArrayNum1].CollimatorType;
		m_ResComponent->OutputsIgs->GantryExposeList[ArrayNum2][ArrayNum1].PanelType = m_ExposeSequence.GantryExposeList[ArrayNum2][ArrayNum1].PanelType;
		m_ResComponent->OutputsIgs->GantryExposeList[ArrayNum2][ArrayNum1].PanelPosition = m_ExposeSequence.GantryExposeList[ArrayNum2][ArrayNum1].PanelPosition;
		m_ResComponent->OutputsIgs->GantryExposeList[ArrayNum2][ArrayNum1].OpenSourceTimer = m_ExposeSequence.GantryExposeList[ArrayNum2][ArrayNum1].OpenSourceTimer;

		m_ExposeSequence.RecordNum++;
		//m_ResComponent->Trace->Log(tlInfo, "Line:%d::CIgs::SetState::next_state::m_ExposeSequence.RecordNum  = %d", __LINE__, m_ExposeSequence.RecordNum);
		m_ResComponent->OutputsIgs->ExposeListNum = ArrayNum1 + 1;
		m_ResComponent->OutputsIgs->ExposeListGroup = ArrayNum2 + 1;

	}
}

/******************************************************************************
* Function : sendPanelPosition()
*
* Describe : Destructor
*
* Input : None
*
* Output : None
*******************************************************************************/

VOID CIgs::SendPanelPosition(VOID)
{
	if (m_IgsCntlWorkMode == IGSCNTL_CBCT || m_IgsCntlWorkMode == IGSCNTL_KV)
	{
		if (m_ResComponent->InputsIgs->FullHalfShape == FULL_WILD)
		{
			m_ResComponent->OutputsIgsSlv->KVPosTarget = m_KvPosition[FULL_WILD];
		}
		else if (m_ResComponent->InputsIgs->FullHalfShape == HALF_WILD)
		{
			m_ResComponent->OutputsIgsSlv->KVPosTarget = m_KvPosition[HALF_WILD];
		}
	}
	else
	{
		//  do nothing 
	}

	m_ResComponent->OutputsIgsSlv->MVPosTarget = m_MvPosition[0];
}
/******************************************************************************
* Function : Getinterlock()
*
* Describe : Destructor
*
* Input : None
*
* Output : None
*******************************************************************************/

VOID CIgs::GetInterlock(VOID)
{
	BeamPlan_t* BeamPlan = m_ResourceMngr->PlanResource.GetBeamPlanData();
	m_Interlock = m_ResComponent->InputsIgsSlv->Interlock;

	if (BeamPlan->RadiationType == RadiationMv || (!m_ResourceMngr->PlanResource.PlanIsReady()))
	{
		m_Interlock &= ~KV_TOLERANCE_BIT;
	}
	m_ResourceMngr->IgsResource.SetInterlock(m_Interlock);

}

/******************************************************************************
* Function : Getinterlock()
*
* Describe : Destructor
*
* Input : None
*
* Output : None
*******************************************************************************/

BOOL CIgs::OutputInitialstate(VOID)
{
	if (m_ResComponent->InputsIgsSlv->Interlock & 0x00000001)
	{
		m_ResourceMngr->IgsResource.SetInitialState(FALSE);
		return  FALSE;
	}
	else
	{
		m_ResourceMngr->IgsResource.SetInitialState(TRUE);
	}

	return  TRUE;
}

/******************************************************************************
* Function : IsReady()
*
* Describe : Destructor
*
* Input : None
*
* Output : None
*******************************************************************************/
BOOL CIgs::KvIsReady(VOID)
{
	
	if (m_ResComponent->OutputsIgs->KvPosReady && \
		m_ResComponent->OutputsIgs->CollimatorPosReady && SetIcuParameterIsOk())
	{
		m_KvReady = TRUE;
		return   TRUE;
	}
	else
	{
		m_KvReady = FALSE;
		return FALSE;
	}

}


/******************************************************************************
* Function : IsReady()
*
* Describe : Destructor
*
* Input : None
*
* Output : None
*******************************************************************************/
BOOL CIgs::SetIcuParameterIsOk(VOID)
{
	if (

		(m_IcuPlanData.FocusExp == m_ResComponent->InputsIgsSlv->IcuFocusExpActual) && \
		(m_IcuPlanData.FrequencyExp == m_ResComponent->InputsIgsSlv->IcuFrequencyExpActual) && \
		(m_IcuPlanData.KVExp == m_ResComponent->InputsIgsSlv->IcuKVExpActual) && \
		(m_IcuPlanData.MAExp == m_ResComponent->InputsIgsSlv->IcuMAExpActual) && \
		(m_IcuPlanData.MSExp == m_ResComponent->InputsIgsSlv->IcuMSExpActual)

		)
	{
		return  TRUE;
	}
	else
	{
		return FALSE;
	}
		

}
/******************************************************************************
* Function : GetGantryTarPosition()
*
* Describe : Destructor
*
* Input : None
*
* Output : None
*******************************************************************************/

BOOL  CIgs::GetGantryTarPosition(VOID)                          // get gantry  Target position 
{
	if (m_ResourceMngr->PlanResource.PlanIsReady ())
	{
		OriginalCP_t* m_OriginalCP = m_ResourceMngr->PlanResource.GetOriginalPlan();
		BackCalSpeedCP_t*TempSpeedAddr = m_ResourceMngr->PlanResource.GetSpeedData();
		BeamPlan_t* TempBeamPlan = m_ResourceMngr->PlanResource.GetBeamPlanData();
		if (m_OriginalCP == NULL || TempSpeedAddr == NULL || TempBeamPlan == NULL)
		{
			//m_ResComponent->Trace->Log(tlInfo, " CIgs:Line:%d::GetGantryTarPosition::address is NULL::%d", __LINE__);
			return  FALSE;
		}
		else
		{
			m_PlanGantry.LastRadiationIndex = m_OriginalCP[m_PlanGantry.CpTotal -1 ].RadiationIndex;
			m_PlanGantry.CpTotal = TempBeamPlan->TotalCP;
			m_PlanGantry.GantryTarPosition = (UINT16)(m_OriginalCP[m_PlanGantry.CpTotal - 1].GantryAngle * 100);
			if (m_PlanGantry.GantryTarPosition == 36000)
			{
				m_PlanGantry.GantryTarPosition = 0;
			}
			m_PlanGantry.GantryDir = m_OriginalCP[m_PlanGantry.CpTotal - 1].GantryDir;

			int  i = 0;
			FLOAT TempTarGantryPosition = 0;
			FLOAT TempGantryPosition = 0;
			for (i = 0; i < m_PlanGantry.CpTotal - 1; i++)
			{

				if (m_OriginalCP[i].GantryDir == CW)
					TempGantryPosition = static_cast<FLOAT>(fmod_((m_OriginalCP[i + 1].GantryAngle - m_OriginalCP[i].GantryAngle + 360.0), 360.0));
				else if (m_OriginalCP[i].GantryDir == CCW)
				{
				   TempGantryPosition = static_cast<FLOAT>(360.0 - (fmod_((m_OriginalCP[i + 1].GantryAngle - m_OriginalCP[i].GantryAngle + 360.0), 360.0)));
				   if (fabs_(TempGantryPosition - 360.0) <= EPSINON)
				   {
					   TempGantryPosition = 0;
				   }
			    }
			    else{ ; }
				TempTarGantryPosition += TempGantryPosition;
		    }                         
			
			
			if (fabs_(TempSpeedAddr->Gantry[0]) <= EPSINON)
			{
				//m_ResComponent->Trace->Log(tlInfo, " CIgs:Line:%d::GetGantryTarPosition::address is NULL::%d", __LINE__);
			}
			else
			{
				m_PlanGantry.CbctExposeNum = static_cast<UINT16> (ceil_(TempTarGantryPosition / TempSpeedAddr->Gantry[0] * m_IcuPlanData.FrequencyExp) );
				m_ResComponent->OutputsIgsSlv->PlanTotalFrame = m_PlanGantry.CbctExposeNum;

			}
			

		}

		return  TRUE;
	}
	return  FALSE;
}
/******************************************************************************
* Function : GetGantryTarPosition()
*
* Describe : Destructor
*
* Input : None
*
* Output : None
*******************************************************************************/
VOID  CIgs::OutputReady()
{
	if (m_KvReady)
	{
		m_ResourceMngr->IgsResource.SetReady(TRUE);
	}
	else
	{
		m_ResourceMngr->IgsResource.SetReady(FALSE);
	}

}

/******************************************************************************
* Function : SetSwKvEnable()
*
* Describe : Destructor
*
* Input : None
*
* Output : None
*******************************************************************************/
VOID  CIgs::SetSwKvEnable(BOOL Enable)
{
	m_ResComponent->OutputsOnGantryHD->SwKvEn = Enable;

}
/******************************************************************************
* Function : CleanPlan
*
* Describe : Destructor
*
* Input : None
*
* Output : None
*******************************************************************************/
VOID  CIgs::CleanPlan()
{

	memset(&m_IcuPlanData, 0, sizeof(m_IcuPlanData));
	memset(m_CollimatorData, 0, sizeof(m_CollimatorData));
	memset(&m_ExposeSequence, 0, sizeof(m_ExposeSequence));
	memset(&m_PlanGantry, 0, sizeof(m_PlanGantry));
	memset(m_ResComponent->OutputsIgs->GantryExposeList, 0, sizeof(m_ResComponent->OutputsIgs->GantryExposeList));
	m_ResComponent->OutputsIgs->ExpolistLong = 0;
	m_ResComponent->OutputsIgs->ExposeListNum = 0;
	m_ResComponent->OutputsIgs->ExposeListGroup = 0;
	m_ResComponent->OutputsIgsSlv->PlanTotalFrame = 0;

}
/******************************************************************************
* Function : CleanPlan
*
* Describe : Destructor
*
* Input : None
*
* Output : None
*******************************************************************************/

UINT8  CIgs::SetSimulationState()                             // set Simulation State
{
	UINT8 TempState = m_RequiredState;

	switch (m_RequiredState)
	{
	    case IGS_INITIAL:
	    {
	    	break;
	    }
	    case IGS_STANDBY:
	    {
	    	m_SimulationBite = 0;
	    	if (m_ResComponent->InputsCtrlState->ControllerEmuMode.IgsCtrl)
	    	{
	    		m_SimulationBite |= IGS_STANDBY_BITE;
	    		TempState |= 0X80;
	    	}
	    	else
	    	{
	    		// do nothing
	    
	    	}
	    	break;
	    }
	    case IGS_PREPARE:
	    {
	    	if (m_ResComponent->InputsCtrlState->ControllerEmuMode.IgsCtrl && (m_SimulationBite & IGS_STANDBY_BITE))
	    	{
	    		m_SimulationBite |= IGS_PREPAR_BITE;
	    		TempState |= 0X80;
	    	}
	    	else if (m_SimulationBite & IGS_STANDBY_BITE)
	    	{
	    		TempState = IGS_FAULT;
	    		m_SimulationBite &= ~IGS_PREPAR_BITE;
	    
	    	}
	    
	    	break;
	    }
	    case IGS_READY:
	    {
	    	if (m_ResComponent->InputsCtrlState->ControllerEmuMode.IgsCtrl && (m_SimulationBite & IGS_PREPAR_BITE))
	    	{
	    		m_SimulationBite |= IGS_READY_BITE;
	    		TempState |= 0X80;
	    	}
	    	else if (m_SimulationBite & IGS_PREPAR_BITE)
	    	{
	    		TempState = IGS_FAULT;
	    		m_SimulationBite &= ~IGS_READY_BITE;
	    
	    	}
	    	break;
	    }
	    case IGS_EXPOSE:
	    {
	    	if (m_ResComponent->InputsCtrlState->ControllerEmuMode.IgsCtrl && (m_SimulationBite & IGS_READY_BITE))
	    	{
	    		m_SimulationBite |= IGS_EXPOSE_BITE;
	    		TempState |= 0X80;
	    	}
	    	else if (m_SimulationBite & IGS_READY_BITE)
	    	{
	    		TempState = IGS_FAULT;
	    		m_SimulationBite &= ~IGS_EXPOSE_BITE;
	    
	    	}
	    	break;
	    }
	    case IGS_FINISH:
	    {
	    	if (m_ResComponent->InputsCtrlState->ControllerEmuMode.IgsCtrl && (m_SimulationBite & IGS_EXPOSE_BITE))
	    	{
	    		m_SimulationBite |= IGS_FINISH_BITE;
	    		TempState |= 0X80;
	    	}
	    	else if (m_SimulationBite & IGS_EXPOSE_BITE)
	    	{
	    		TempState = IGS_FAULT;
	    		m_SimulationBite &= ~IGS_FINISH_BITE;
	    
	    	}
	    	break;
	    }
	    case IGS_FAULT:
	    {
	    	if (m_ResComponent->InputsCtrlState->ControllerEmuMode.IgsCtrl && (m_SimulationBite != 0))
	    	{
	    		m_SimulationBite |= IGS_FAULT_BITE;
	    		TempState |= 0X80;
	    	}
	    	else if (m_SimulationBite != 0)
	    	{
	    		TempState = IGS_FAULT;
	    		m_SimulationBite &= ~IGS_FAULT_BITE;
	    
	    	}
	    
	    	break;
	    }
	    default:
	    {
           #if RTC_DEBUG_EXCEPTION
	    	m_ResComponent->Trace->Log(tlInfo, "Line:%d::CIgs::SetState::next_state::Unknown state::%d", __LINE__, m_RequiredState);
           #endif
	    
	    	break;
	    }
	}
	return  TempState;

}
/******************************************************************************
* Function : GetNotready
*
* Describe : Destructor
*
* Input : None
*
* Output : None
*******************************************************************************/
VOID CIgs::GetNotready(VOID)
{
	if (m_ResourceMngr->PlanResource.PlanIsReady())
	{

		BeamPlan_t* BeamPlan = m_ResourceMngr->PlanResource.GetBeamPlanData();
		if (BeamPlan->RadiationType == RadiationKv)
		{
			if ( m_ResourceMngr->FsmResource.GuiCmd.IgsCTNotReady)
			{
				m_Unreadys |= IGS_CT_NOTREADY;
			}
			else if (m_ResourceMngr->FsmResource.GuiCmd.IgsCTReady)
			{
				m_Unreadys &= ~IGS_CT_NOTREADY;
			}
		}
		else
		{
			m_Unreadys &= ~IGS_CT_NOTREADY;
		}
	}
	else
	{
		m_Unreadys &= IGS_CT_NOTREADY;
	}
	m_ResourceMngr->IgsResource.SetNotReady(m_Unreadys);
}