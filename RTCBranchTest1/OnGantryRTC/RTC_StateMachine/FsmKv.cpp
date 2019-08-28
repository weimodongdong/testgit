/*******************************************************************************
** File	      : Crc32.cpp
** Author:    : ChenLong
** Version    :	1.0
** Environment: VS2013
** Description:
** History    :	18/01/30    Chenlong    Created
*******************************************************************************/
/* INCLUDE ===================================================================*/
#include "FsmKv.h"

/* METHODS====================================================================*/
/******************************************************************************
* Function: FsmKv()
*
* Describe: construct function
*
* Input   : None
*
* Output  : None
*******************************************************************************/
CFsmKv::CFsmKv()
{

}

CFsmKv::CFsmKv(CResourceMngr* Resource)
{
	m_ResourceMngr = Resource;
	m_CurrentState = RTC_NO_STATE;
	m_LastState = RTC_NO_STATE;
	m_RequiredState = RTC_NO_STATE;
	m_ResComponent = Resource->GetResourceCom();
	m_RunState = FSM_HOLDING;
	m_InitEn = TRUE;
	InitSubFsmReqState();
}

CFsmKv::~CFsmKv()
{
}

/******************************************************************************
* Function: CheckBgmFsmState(VOID)
*
* Describe: construct function
*
* Input   : None
*
* Output  : None
*******************************************************************************/
VOID CFsmKv::CheckBgmFsmState(VOID)
{
	BOOL SyncFlag = FALSE;
	if (m_ResComponent->InputsSubFsm->BgmCurrentState == RTC_PREPARATORY || \
		m_ResComponent->InputsSubFsm->BgmCurrentState == RTC_STANDBY)
	{
		SyncFlag = TRUE;
	}
	else
	{
		SyncFlag = FALSE;
	}

	//Set sync interlock
	if (SyncFlag)
		ResetInterlock(INTL_BGM_SUBSYSTEM_NOT_SYNC);
	else
		SetInterlock(INTL_BGM_SUBSYSTEM_NOT_SYNC);

	return;
}

/******************************************************************************
* Function: CheckBSMFsmState(VOID)
*
* Describe: construct function
*
* Input   : None
*
* Output  : None
*******************************************************************************/
VOID CFsmKv::CheckBSMFsmState(VOID)
{
	BOOL SyncFlag = FALSE;

	//Get bsm current state
	BYTE BsmCurrentState = m_ResComponent->InputsBsm->CurrentState & 0x7f;
	if (BSM_STANDBY == BsmCurrentState)
		SyncFlag = TRUE;
	else
		SyncFlag = FALSE;
	
	//Set sync interlock
	if (SyncFlag)
		ResetInterlock(INTL_BSM_SUBSYSTEM_NOT_SYNC);
	else
		SetInterlock(INTL_BSM_SUBSYSTEM_NOT_SYNC);

	return;
}

/******************************************************************************
* Function: CheckIgsFsmState(VOID)
*
* Describe: 
*
* Input   : None
*
* Output  : None
*******************************************************************************/
VOID CFsmKv::CheckIgsFsmState(VOID)
{
	BOOL SyncFlag = FALSE;

	IgsState_eu igsState = m_ResourceMngr->IgsResource.GetCurrentState();
	switch (m_CurrentState)
	{
		case RTC_INIT:
		{
			if (IGS_INITIAL == igsState)
			{
				SyncFlag = TRUE;
				break;
			}
			else
			{
				SyncFlag = FALSE;
				break;
			}
		}
		case RTC_STANDBY:
		{
			if (IGS_STANDBY == igsState)
			{
				SyncFlag = TRUE;
				break;
			}
			else
			{
				SyncFlag = FALSE;
				break;
			}
		}
		case RTC_PREPARATORY:
		{
			if (IGS_STANDBY == igsState)
			{
				SyncFlag = TRUE;
				break;
			}
			else
			{
				SyncFlag = FALSE;
				break;
			}
		}
		case RTC_PREPARE:
		{
			if (IGS_PREPARE == igsState || IGS_READY == igsState)
			{
				SyncFlag = TRUE;

				break;
			}
			else
			{
				SyncFlag = FALSE;
				break;
			}

		}
		case RTC_READY:
		{
			if (IGS_READY == igsState)
			{
				SyncFlag = TRUE;
				break;
			}
			else
			{
				SyncFlag = FALSE;
				break;
			}
		}
		case RTC_RADIATION:
		{
			if (IGS_EXPOSE == igsState || IGS_FINISH == igsState)
			{
				SyncFlag = TRUE;
				break;
			}
			else
			{
				SyncFlag = FALSE;
				
				break;
			}
		}
		case RTC_REPOSITION:
		{
				SyncFlag = TRUE;
				m_ResComponent->Trace->Log(tlInfo, "Line%d::CFsmKv::<StateIsSync>::unexcepted state", __LINE__);
				break;
		}
		case RTC_COMPLETE:
		{
			if (   IGS_FINISH == igsState  )
			{
				SyncFlag = TRUE;
				break;
			}
			else
			{
				SyncFlag = FALSE;
				break;
			}
		}
		case RTC_INTERRUPT:
		{
			SyncFlag = TRUE;
			m_ResComponent->Trace->Log(tlInfo, "Line%d::CFsmKv::<StateIsSync>::unexcepted state", __LINE__);
			break;
		}
		case RTC_INTERRUPTREADY:
		{
			SyncFlag = TRUE;
			m_ResComponent->Trace->Log(tlInfo, "Line%d::CFsmKv::<StateIsSync>::unexcepted state", __LINE__);
			break;
		}
		case RTC_TERMINATE:
		{
			if (IGS_FAULT == igsState)
			{
				SyncFlag = TRUE;
				break;
			}
			else
			{
				SyncFlag = FALSE;
				break;
			}
		}
		case RTC_OFF:
		{

		}
		case RTC_SHUTDOWN:
		{

		}
		default:
		{
			m_ResComponent->Trace->Log(tlInfo, "Line%d::CFsm::<StateIsSync>::error state", __LINE__);
			return;
		}
	}

	if (SyncFlag)
		ResetInterlock(INTL_IGS_SUBSYSTEM_NOT_SYNC);
	else
		SetInterlock(INTL_IGS_SUBSYSTEM_NOT_SYNC);

	return;
}

/******************************************************************************
* Function: RadiationMain(VOID)
*
* Describe: construct function
*
* Input   : None
*
* Output  : None
*******************************************************************************/
FsmState_u CFsmKv::RadiationMain(VOID)
{
	m_CurrentState = RTC_RADIATION;

	//Start state sync timer
	if (m_StateSyncTimer.IsReady())
	{
		m_StateSyncTimer.StartTimer(m_ResourceMngr->GetCurrentSysTime(), RADIATION_WAIT_TIME);
		return RTC_RADIATION;					//hold on current state
	}
	else if (!m_StateSyncTimer.IsTimeOut(m_ResourceMngr->GetCurrentSysTime()))
	{
		ResetInterlock(INTL_BSM_SUBSYSTEM_NOT_SYNC);			//reset state bsm sync interlock
		ResetInterlock(INTL_BGM_SUBSYSTEM_NOT_SYNC);			//reset state  bgm sync interlock
		ResetInterlock(INTL_IGS_SUBSYSTEM_NOT_SYNC);			//reset state  Igs sync interlock
		ResetInterlock(INTL_OGR_NOT_SYNC);
		return RTC_RADIATION;					//hold on current state
	}
	else
	{
		CheckSubFsmState();
		CheckOGRFsmState();

		if (HasTerminate() || HasInterlock())
		{
			#if FSM_DEBUG_FALL_EN
			m_ResComponent->Trace->Log(tlInfo, "Line%d::CFsmKv::<RadiationMain>::Jump to RTC_TERMINATE::Has Intl:: BGM H32=0x%lx L32=0x%lx, NGR =0x%lx , IGS =0x%lx , BSM =0x%lx", \
				__LINE__, m_ResourceMngr->Interlocks.BgmInterlock >> 32, m_ResourceMngr->Interlocks.BgmInterlock,  m_ResourceMngr->Interlocks.NGRInterlock, \
			 m_ResourceMngr->Interlocks.IgsInterlock,  m_ResourceMngr->Interlocks.BsmInterlock);

			m_ResComponent->Trace->Log(tlInfo, "Line%d::CFsmKv::<RadiationMain>::Jump to RTC_TERMINATE:: unreadys ::BGM=0x%x,NGR=0x%x,Igs=0x%x,BSM=0x%x", \
			__LINE__, m_ResourceMngr->Unreadys.BgmUnready, m_ResourceMngr->Unreadys.NGRUnready, m_ResourceMngr->Unreadys.IgsUnready, m_ResourceMngr->Unreadys.BsmUnready);
			#endif


			m_RadiationProtTimer.ResetTimer();

			return JumpToNextState(RTC_TERMINATE);		//go to complete state
		}
		else if (IGS_FINISH == m_ResourceMngr->IgsResource.GetCurrentState())
		{
			m_RadiationProtTimer.ResetTimer();

			return JumpToNextState(RTC_COMPLETE);		//go to complete state
		}
		else
		{
			return RTC_RADIATION;
		}
	}
}

/******************************************************************************
* Function: PrepareMain(VOID)
*
* Describe: construct function
*
* Input   : None
*
* Output  : None
*******************************************************************************/
FsmState_u CFsmKv::PrepareMain(VOID)
{
	m_CurrentState = RTC_PREPARE;

	if (m_ResourceMngr->FsmResource.GuiCmd.ClosePatient || m_ResourceMngr->FsmResource.GuiCmd.CleanPlan)
	{
		#if FSM_DEBUG_FALL_EN
		m_ResComponent->Trace->Log(tlInfo, "Line%d::CFsm::<PrepareMain>::Jump to RTC_PREPARATORY::GUI Close Patient Command", __LINE__);
		#endif

		return JumpToNextState(RTC_PREPARATORY);	//go to preparatory state
	}
	else
	{
		if (m_StateSyncTimer.IsReady())
		{
			m_StateSyncTimer.StartTimer(m_ResourceMngr->GetCurrentSysTime(), PREPARE_WAIT_TIME);
			return RTC_PREPARE;					//hold on current state
		}
		else if (!m_StateSyncTimer.IsTimeOut(m_ResourceMngr->GetCurrentSysTime()))
		{
			ResetInterlock(INTL_BSM_SUBSYSTEM_NOT_SYNC);			//reset state bsm sync interlock
			ResetInterlock(INTL_BGM_SUBSYSTEM_NOT_SYNC);			//reset state  bgm sync interlock
			ResetInterlock(INTL_IGS_SUBSYSTEM_NOT_SYNC);			//reset state  Igs sync interlock
			ResetInterlock(INTL_OGR_NOT_SYNC);
			return RTC_PREPARE;					//hold on current state
		}
		else
		{
			CheckSubFsmState();
			CheckOGRFsmState();

			if (HasTerminate())
			{
				#if FSM_DEBUG_FALL_EN
				m_ResComponent->Trace->Log(tlInfo, "Line%d::CFsm::<PrepareMain>::Jump to RTC_PREPARATORY::Has Terminate NGR Terminate:%u", __LINE__, m_ResourceMngr->Terminates.NGRTerminate);
				#endif

				return JumpToNextState(RTC_PREPARATORY);	//go to preparatory state
			}
			else if (HasInterlock())
			{
				#if FSM_DEBUG_FALL_EN
				m_ResComponent->Trace->Log(tlInfo, "Line%d::CFsmKv::<PrepareMain>::Jump to RTC_PREPARATORY::Has Intl:: BGM H32=0x%lx L32=0x%lx, NGR =0x%lx , IGS =0x%lx , BSM =0x%lx ", \
					__LINE__, m_ResourceMngr->Interlocks.BgmInterlock >> 32, m_ResourceMngr->Interlocks.BgmInterlock, m_ResourceMngr->Interlocks.NGRInterlock, \
				 m_ResourceMngr->Interlocks.IgsInterlock, m_ResourceMngr->Interlocks.BsmInterlock);

				m_ResComponent->Trace->Log(tlInfo, "Line%d::CFsmKv::<PrepareMain>::Jump to RTC_PREPARATORY:: unreadys ::BGM=0x%x,NGR=0x%x,Igs=0x%x,BSM=0x%x", \
				__LINE__, m_ResourceMngr->Unreadys.BgmUnready, m_ResourceMngr->Unreadys.NGRUnready, m_ResourceMngr->Unreadys.IgsUnready, m_ResourceMngr->Unreadys.BsmUnready);
				#endif

				return JumpToNextState(RTC_PREPARATORY);	//go to preparatory state
			}
			else if (m_ResourceMngr->IgsResource.IsReady())
			{
				return JumpToNextState(RTC_READY);	//go to ready state
			}
			else
			{
				return RTC_PREPARE;			//hold on prepare state
			}
		}
	}
}

/******************************************************************************
* Function: UpdateOutputResource(VOID)
*
* Describe: update FSM output resource
*
* Input   :
*
* Output  :
*******************************************************************************/
BOOL CFsmKv::UpdateOutputResource(VOID)
{
	//Output FSM state to BGM
	m_ResComponent->OutputsSubFsm->BgmRequiredState = GetBgmRequiredState();

	//Ouput FSM state to OGR
	m_ResComponent->OutputsOGR->CurrentState = m_CurrentState;

	//Output FSM state to Tcp Thread
	m_ResComponent->OutputsFsm->required_state = m_RequiredState;
	m_ResComponent->OutputsFsm->rtc_current_state = m_CurrentState;
	m_ResComponent->OutputsFsm->rtc_last_state = m_LastState;

	//Output FSM state to resource manager
	m_ResourceMngr->FsmResource.SetCurrentState(m_CurrentState);

	//Motion Enable
	if (m_CurrentState == RTC_PREPARATORY || m_CurrentState == RTC_INTERRUPT)
	{
		m_ResComponent->OutputsFsm->bSwManualMotionEn = TRUE;
	}
	else
	{
		m_ResComponent->OutputsFsm->bSwManualMotionEn = FALSE;
	}

	UpdateKvEnableChain();

	return TRUE;
}

/******************************************************************************
* Function: UpdateKvEnableChain(VOID)
*
* Describe: update kv enable chain
*
* Input   :
*
* Output  :
*******************************************************************************/
VOID CFsmKv::UpdateKvEnableChain(VOID)
{
	if (RTC_COMPLETE == m_CurrentState)
	{
		if (m_KvChainDelayTimer.IsReady())
		{
			m_KvChainDelayTimer.StartTimer(m_ResourceMngr->GetCurrentSysTime(), KV_CHAIN_DELAY_TIME);
			return;
		}
		else if (!m_KvChainDelayTimer.IsTimeOut(m_ResourceMngr->GetCurrentSysTime()))
		{
			m_ResComponent->OutputsOnGantryHD->SwKvEn = TRUE;
			return;
		}
		else
		{
			m_ResComponent->OutputsOnGantryHD->SwKvEn = FALSE;
		}
	}
	else if (RTC_PREPARE == m_CurrentState || RTC_READY == m_CurrentState || RTC_RADIATION == m_CurrentState)
	{
		m_ResComponent->OutputsOnGantryHD->SwKvEn = TRUE;
		m_KvChainDelayTimer.ResetTimer();
	}
	else
	{
		m_ResComponent->OutputsOnGantryHD->SwKvEn = FALSE;
	}

	return;
}


