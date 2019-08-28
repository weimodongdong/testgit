/*******************************************************************************
** File	      : Crc32.cpp
** Author:    : ChenLong
** Version    :	1.0
** Environment: VS2013
** Description:
** History    :	18/01/30    Chenlong    Created
*******************************************************************************/
/* INCLUDE ===================================================================*/
#include "Fsm.h"

/* METHODS====================================================================*/
/******************************************************************************
* Function: FSM()
*
* Describe: construct function
*
* Input   : None
*
* Output  : None
*******************************************************************************/
CFsm::CFsm()
{
	m_CurrentState = RTC_NO_STATE;
	m_LastState = RTC_NO_STATE;
	m_RequiredState = RTC_NO_STATE;
	m_ResComponent = NULL;
	m_ResourceMngr = NULL;
	m_InitEn = TRUE;
}

CFsm::CFsm(CResourceMngr* Resource)
{
	m_ResourceMngr = Resource;
	m_CurrentState = RTC_NO_STATE;
	m_LastState = RTC_NO_STATE;
	m_RequiredState = RTC_NO_STATE;
	m_ResComponent = Resource->GetResourceCom();
	m_RunState = FSM_RUNNING;
	m_InitEn = TRUE;
}

CFsm::~CFsm()
{
}

/******************************************************************************
* Function: CycleUpdate(VOID)
*
* Describe:
*
* Input   :
*
* Output  :
*******************************************************************************/
VOID CFsm::CycleUpdate(VOID)
{
	Initialize();

	UpdateInputResource();

	ExecuteBusiness();

	UpdateOutputResource();
	
	return;
}

/******************************************************************************
* Function: PowerUpMain(VOID)
*
* Describe: power up actions
*
* Input   :
*
* Output  :
*******************************************************************************/
VOID CFsm::SetRunState(FsmRunState_u State)
{
	m_RunState = State;

	return;
}

/******************************************************************************
* Function: PowerUpMain(VOID)
*
* Describe: power up actions
*
* Input   :
*
* Output  :
*******************************************************************************/
VOID CFsm::SetRequiredState(FsmState_u State)
{
	m_RequiredState = State;

	return;
}

/******************************************************************************
* Function: PowerUpMain(VOID)
*
* Describe: power up actions
*
* Input   :
*
* Output  :
*******************************************************************************/
FsmState_u CFsm::PowerUpMain(VOID)
{
	m_CurrentState = RTC_NO_STATE;
	m_LastState = RTC_NO_STATE;

	return JumpToNextState(RTC_INIT);
}

/******************************************************************************
* Function: InitMain(VOID)
*
* Describe: init actions
*
* Input   :
*
* Output  :
*******************************************************************************/
FsmState_u CFsm::InitMain(VOID)
{
	m_CurrentState = RTC_INIT;

	if (HardwareIsReady() && m_ResourceMngr->BsmResource.GetBSMInitCompleteFlag())
	{
		//if (CheckSubFsmState())
			return JumpToNextState(RTC_STANDBY);			//go to standby state
		//else
		//	return RTC_INIT;
	}
	else
	{
		return RTC_INIT;	//waiting hardware is ready
	}
}

/******************************************************************************
* Function: StandbyMain(VOID)
*
* Describe: standby actions
*
* Input   :
*
* Output  :
*******************************************************************************/
FsmState_u CFsm::StandbyMain(VOID)
{
	m_CurrentState = RTC_STANDBY;

	if (m_ResourceMngr->FsmResource.GuiCmd.ShutDown)
	{
		#if FSM_DEBUG_FALL_EN
			m_ResComponent->Trace->Log(tlInfo, "Line%d::CFsm::<StandbyMain>::Jump to RTC_SHUTDOWN::GUI Shut Down Command", __LINE__);
		#endif

		return JumpToNextState(RTC_SHUTDOWN);
	}
	else if (m_ResourceMngr->FsmResource.GuiCmd.OffState)
	{
		#if FSM_DEBUG_FALL_EN
			m_ResComponent->Trace->Log(tlInfo, "Line%d::CFsm::<StandbyMain>::Jump to RTC_OFF::GUI Off Command");
		#endif

		return JumpToNextState(RTC_OFF);
	}
	else
	{
		if (m_StateSyncTimer.IsReady())					//start state sync timer
		{
			m_StateSyncTimer.StartTimer(m_ResourceMngr->GetCurrentSysTime(), STANDBY_WAIT_TIME);
			return RTC_STANDBY;								//hold on current state
		}
		else if (!m_StateSyncTimer.IsTimeOut(m_ResourceMngr->GetCurrentSysTime()))
		{
			ResetInterlock(INTL_BSM_SUBSYSTEM_NOT_SYNC);			//reset state bsm sync interlock
			ResetInterlock(INTL_BGM_SUBSYSTEM_NOT_SYNC);			//reset state  bgm sync interlock
			ResetInterlock(INTL_IGS_SUBSYSTEM_NOT_SYNC);			//reset state  Igs sync interlock
		
			return RTC_STANDBY;								//hold on current state
		}
		else
		{
			CheckSubFsmState();								//check sub FSM state

			if (m_ResourceMngr->FsmResource.GuiCmd.Login)
			{
				return JumpToNextState(RTC_PREPARATORY);	//go to preparatory state
			}
			else
			{
				return RTC_STANDBY;							//hold on current state
			}
		}
	}
}

/******************************************************************************
* Function: PreparatoryMain(VOID)
*
* Describe: preparatory actions
*
* Input   :
*
* Output  :
*******************************************************************************/
FsmState_u CFsm::PreparatoryMain(VOID)
{
	m_CurrentState = RTC_PREPARATORY;

	if (m_ResourceMngr->FsmResource.GuiCmd.Logout)
	{
		#if FSM_DEBUG_FALL_EN
			m_ResComponent->Trace->Log(tlInfo, "Line%d::CFsm::<PreparatoryMain>::Jump to RTC_STANDBY::GUI Log Out Command", __LINE__);
		#endif

		return JumpToNextState(RTC_STANDBY);	//go to standby state
	}
	else
	{
		if (m_StateSyncTimer.IsReady())
		{
			m_StateSyncTimer.StartTimer(m_ResourceMngr->GetCurrentSysTime(), PREPARATORY_WAIT_TIME);
			return RTC_PREPARATORY;					//hold on current state
		}
		else if (!m_StateSyncTimer.IsTimeOut(m_ResourceMngr->GetCurrentSysTime()))
		{
			ResetInterlock(INTL_BSM_SUBSYSTEM_NOT_SYNC);			//reset state bsm sync interlock
			ResetInterlock(INTL_BGM_SUBSYSTEM_NOT_SYNC);			//reset state  bgm sync interlock
			ResetInterlock(INTL_IGS_SUBSYSTEM_NOT_SYNC);			//reset state  Igs sync interlock
			ResetInterlock(INTL_OGR_NOT_SYNC);
			return RTC_PREPARATORY;					//hold on current state
		}
		else
		{
			CheckSubFsmState();
			CheckOGRFsmState();
			
			if (m_ResourceMngr->FsmResource.GuiCmd.PrepareState && \
				!HasInterlock() && !HasUnready() && !HasTerminate() && OGRIsOk())
			{
				return JumpToNextState(RTC_PREPARE);//go to prepare state
			}
			else
			{
				return RTC_PREPARATORY;				//hold on current state
			}
		}
	}
}

/******************************************************************************
* Function: PrepareMain(VOID)
*
* Describe: prepare actions
*
* Input   :
*
* Output  :
*******************************************************************************/
FsmState_u CFsm::PrepareMain(VOID)
{
	m_CurrentState = RTC_PREPARE;

	if (m_ResourceMngr->FsmResource.GuiCmd.ClosePatient || m_ResourceMngr->FsmResource.GuiCmd.CleanPlan)
	{

		#if FSM_DEBUG_FALL_EN
			m_ResComponent->Trace->Log(tlInfo, "Line%d::CFsm::<PrepareMain>::Jump to RTC_PREPARATORY::GUI Close Patient Command", __LINE__);
		#endif

		return JumpToNextState(RTC_PREPARATORY);	//go to preparatory state
	}
	else if (FKP_CMD_BEAM_REVIEW == m_ResComponent->InputsOGR->FkpButton)
	{
		#if FSM_DEBUG_FALL_EN
			m_ResComponent->Trace->Log(tlInfo, "Line%d::CFsm::<PrepareMain>::Jump to RTC_PREPARATORY::Press beam review button", __LINE__);
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

			if (HasInterlock())
			{
				#if FSM_DEBUG_FALL_EN
				m_ResComponent->Trace->Log(tlInfo, \
					"Line%d::CFsm::<PrepareMain>::Jump to RTC_PREPARATORY::Has Intl::BGM H32=0x%lx L32=0x%lx, NGR =0x%lx , IGS =0x%lx , BSM =0x%lx ", \
					__LINE__, m_ResourceMngr->Interlocks.BgmInterlock>>32, m_ResourceMngr->Interlocks.BgmInterlock, m_ResourceMngr->Interlocks.NGRInterlock, \
					 m_ResourceMngr->Interlocks.IgsInterlock,  m_ResourceMngr->Interlocks.BsmInterlock);
				
				m_ResComponent->Trace->Log(tlInfo, "Line%d::CFsm::<PrepareMain>::Jump to RTC_PREPARATORY:: unreadys ::BGM=0x%x,NGR=0x%x,Igs=0x%x,BSM=0x%x" ,\
					__LINE__, m_ResourceMngr->Unreadys.BgmUnready, m_ResourceMngr->Unreadys.NGRUnready, m_ResourceMngr->Unreadys.IgsUnready, m_ResourceMngr->Unreadys.BsmUnready);

				m_ResComponent->Trace->Log(tlInfo, \
					"Line%d::CFsm::<PrepareMain>::GuiCom:%d CanOpen:%d EthercatBGM:%d EthercatBSM:%d", __LINE__,\
					m_ResComponent->InputsFsm->com_status_gui, m_ResComponent->FmDevices->CANOpen, m_ResComponent->FmDevices->BGMSlave, m_ResComponent->FmDevices->BSMSlave);

				#endif

				return JumpToNextState(RTC_PREPARATORY);	//go to preparatory state
			}
			else if (m_ResComponent->InputsSubFsm->BgmCurrentState == RTC_READY && m_ResComponent->InputsBsm->BsmReady) 
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
* Function: ReadyMain(VOID)
*
* Describe: ready actions
*
* Input   :
*
* Output  :
*******************************************************************************/
FsmState_u CFsm::ReadyMain(VOID)
{
	m_CurrentState = RTC_READY;

	if (m_StateSyncTimer.IsReady())
	{
		m_StateSyncTimer.StartTimer(m_ResourceMngr->GetCurrentSysTime(), READY_WAIT_TIME);
		return RTC_READY;					//hold on current state
	}
	else if (!m_StateSyncTimer.IsTimeOut(m_ResourceMngr->GetCurrentSysTime()))
	{
		ResetInterlock(INTL_BSM_SUBSYSTEM_NOT_SYNC);			//reset state bsm sync interlock
		ResetInterlock(INTL_BGM_SUBSYSTEM_NOT_SYNC);			//reset state  bgm sync interlock
		ResetInterlock(INTL_IGS_SUBSYSTEM_NOT_SYNC);			//reset state  Igs sync interlock
		ResetInterlock(INTL_OGR_NOT_SYNC);
		return RTC_READY;					//hold on current state
	}
	else
	{
		CheckSubFsmState();
		CheckOGRFsmState();

		if (m_ResComponent->InputsOGR->CurrentState == RTC_RADIATION)
		{
			return JumpToNextState(RTC_RADIATION);	//go to radiation state
		}
		else if (HasTerminate())
		{
			#if FSM_DEBUG_FALL_EN
			m_ResComponent->Trace->Log(tlInfo, \
				"Line%d::CFsm::<ReadyMain>::Jump to RTC_PREPARATORY::Has Terminate",__LINE__);
			#endif

			return JumpToNextState(RTC_PREPARATORY);	//go to preparatory state
		}
		else if (HasInterlock() || HasUnready())
		{
			#if FSM_DEBUG_FALL_EN
			m_ResComponent->Trace->Log(tlInfo, "Line%d::CFsm::<ReadyMain>::Jump to RTC_PREPARATORY::Has Intl:: BGM H32=0x%lx L32=0x%lx, NGR =0x%lx , IGS =0x%lx , BSM =0x%lx ", \
				__LINE__, m_ResourceMngr->Interlocks.BgmInterlock >> 32, m_ResourceMngr->Interlocks.BgmInterlock,  m_ResourceMngr->Interlocks.NGRInterlock, \
			 m_ResourceMngr->Interlocks.IgsInterlock,  m_ResourceMngr->Interlocks.BsmInterlock);

			m_ResComponent->Trace->Log(tlInfo, "Line%d::CFsm::<ReadyMain>::Jump to RTC_PREPARATORY:: unreadys ::BGM=0x%x,NGR=0x%x,Igs=0x%x,BSM=0x%x", \
				__LINE__, m_ResourceMngr->Unreadys.BgmUnready, m_ResourceMngr->Unreadys.NGRUnready, m_ResourceMngr->Unreadys.IgsUnready, m_ResourceMngr->Unreadys.BsmUnready);
			
			m_ResComponent->Trace->Log(tlInfo, \
				"Line%d::CFsm::<ReadyMain>::GuiCom:%d CanOpen:%d EthercatBGM:%d EthercatBSM:%d", __LINE__,\
				m_ResComponent->InputsFsm->com_status_gui, m_ResComponent->FmDevices->CANOpen, m_ResComponent->FmDevices->BGMSlave, m_ResComponent->FmDevices->BSMSlave);
			#endif

			return JumpToNextState(RTC_PREPARATORY);	//go to preparatory state
		}
		/*else if (m_ResourceMngr->FsmResource.GuiCmd.RadiationState)
		{
			return JumpToNextState(RTC_RADIATION);	//go to radiation state
		}*/
		else
		{
			return RTC_READY;					//hold on current state
		}
	}
}

/******************************************************************************
* Function: RadiationMain(VOID)
*
* Describe: radiation actions
*
* Input   :
*
* Output  :
*******************************************************************************/
FsmState_u CFsm::RadiationMain(VOID)
{
	m_CurrentState = RTC_RADIATION;

	//Start radiation protect timer
	if (m_RadiationProtTimer.IsReady())
		m_RadiationProtTimer.StartTimer(m_ResourceMngr->GetCurrentSysTime(), m_ResourceMngr->PlanResource.GetRadiationProtTime());
	else if (!m_RadiationProtTimer.IsTimeOut(m_ResourceMngr->GetCurrentSysTime()))
		m_ResComponent->OutputsBeam->RadiationTime = static_cast<UINT32>((m_RadiationProtTimer.GetRunTime(m_ResourceMngr->GetCurrentSysTime())) / 1000); //output radiation time
	else if (m_RadiationProtTimer.IsTimeOut(m_ResourceMngr->GetCurrentSysTime()))
	{
		m_ResComponent->OutputsBeam->RadiationTime = static_cast<UINT32>((m_RadiationProtTimer.GetRunTime(m_ResourceMngr->GetCurrentSysTime())) / 1000); //output radiation time
		m_ResourceMngr->FsmResource.SetRadiationTimeOut(TRUE); //set radiation timer interlock

		#if FSM_DEBUG_FALL_EN
			m_ResComponent->Trace->Log(tlInfo, "Line%d::CFsm::<RadiationMain>::Jump to RTC_TERMINATE::Radiation timeout", __LINE__);
		#endif

		return JumpToNextState(RTC_TERMINATE);		//go to complete state
	}
		
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

		if (HasTerminate() || m_ResComponent->InputsSubFsm->BgmCurrentState == RTC_TERMINATE || m_ResComponent->InputsOGR->CurrentState == RTC_TERMINATE)
		{
			#if FSM_DEBUG_FALL_EN
				m_ResComponent->Trace->Log(tlInfo, "Line%d::CFsm::<RadiationMain>::Jump to RTC_TERMINATE::Has Terminate::NGR Terminate:%u \
										BgmState:%u::OGRState:%u", __LINE__, m_ResourceMngr->Terminates.NGRTerminate, \
										m_ResComponent->InputsSubFsm->BgmCurrentState, m_ResComponent->InputsOGR->CurrentState);
			#endif
			m_RadiationProtTimer.ResetTimer();

			return JumpToNextState(RTC_TERMINATE);		//go to complete state
		}
		else if (HasInterlock() || HasUnready())
		{
			#if FSM_DEBUG_FALL_EN
			m_ResComponent->Trace->Log(tlInfo, "Line%d::CFsm::<RadiationMain>::Jump to RTC_INTERRUPT::Has Intl:: BGM H32=0x%lx L32=0x%lx, NGR =0x%lx , IGS =0x%lx , BSM =0x%lx ", \
				__LINE__, m_ResourceMngr->Interlocks.BgmInterlock >> 32, m_ResourceMngr->Interlocks.BgmInterlock,  m_ResourceMngr->Interlocks.NGRInterlock, \
		    m_ResourceMngr->Interlocks.IgsInterlock, m_ResourceMngr->Interlocks.BsmInterlock);

			m_ResComponent->Trace->Log(tlInfo,"Line%d::CFsm::<RadiationMain>::Jump to RTC_INTERRUPT:: unreadys ::BGM=0x%x,NGR=0x%x,Igs=0x%x,BSM=0x%x", \
			__LINE__, m_ResourceMngr->Unreadys.BgmUnready, m_ResourceMngr->Unreadys.NGRUnready, m_ResourceMngr->Unreadys.IgsUnready, m_ResourceMngr->Unreadys.BsmUnready);
			
			m_ResComponent->Trace->Log(tlInfo, \
				"Line%d::CFsm::<RadiationMain>::GuiCom:%d CanOpen:%d EthercatBGM:%d EthercatBSM:%d", __LINE__,\
				m_ResComponent->InputsFsm->com_status_gui, m_ResComponent->FmDevices->CANOpen, m_ResComponent->FmDevices->BGMSlave, m_ResComponent->FmDevices->BSMSlave);
			#endif
			m_RadiationProtTimer.ResetTimer();

			return JumpToNextState(RTC_INTERRUPT);		//go to interrupt state
		}
		else if (m_ResComponent->InputsSubFsm->BgmCurrentState == RTC_COMPLETE)
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
* Function: RepositionMain(VOID)
*
* Describe: teposition actions
*
* Input   :
*
* Output  :
*******************************************************************************/
FsmState_u CFsm::RepositionMain(VOID)
{
	return RTC_NO_STATE;
}

/******************************************************************************
* Function: CompleteMain(VOID)
*
* Describe: complete actions
*
* Input   :
*
* Output  :
*******************************************************************************/
FsmState_u CFsm::CompleteMain(VOID)
{
	m_CurrentState = RTC_COMPLETE;
	CheckBSMFsmState();		//check Bsm FSM state

	if (m_ResourceMngr->FsmResource.GuiCmd.NextBeam)
	{
		return JumpToNextState(RTC_PREPARATORY);	//go to preparatory state
	}
	else
	{
		return RTC_COMPLETE;						//hold on current state
	}
}

/******************************************************************************
* Function: InterruptMain(VOID)
*
* Describe: interrupt actions
*
* Input   :
*
* Output  :
*******************************************************************************/
FsmState_u CFsm::InterruptMain(VOID)
{
	m_CurrentState = RTC_INTERRUPT;

	if (m_ResourceMngr->FsmResource.GuiCmd.StopRadiation || m_ResComponent->InputsOGR->CurrentState == RTC_TERMINATE)
	{
		#if FSM_DEBUG_FALL_EN
			m_ResComponent->Trace->Log(tlInfo, "Line%d::CFsm::<InterruptMain>::Jump to RTC_TERMINATE::Gui Stop Radiation Command or OffgantryRtc  terminate", __LINE__);
		#endif

		return JumpToNextState(RTC_TERMINATE);	//go to terminate state
	}
	else
	{
		if (m_StateSyncTimer.IsReady())
		{
			m_StateSyncTimer.StartTimer(m_ResourceMngr->GetCurrentSysTime(), INTERRUPT_WAIT_TIME);
			return RTC_INTERRUPT;					//hold on current state
		}
		else if (!m_StateSyncTimer.IsTimeOut(m_ResourceMngr->GetCurrentSysTime()))
		{
			ResetInterlock(INTL_BSM_SUBSYSTEM_NOT_SYNC);			//reset state bsm sync interlock
			ResetInterlock(INTL_BGM_SUBSYSTEM_NOT_SYNC);			//reset state  bgm sync interlock
			ResetInterlock(INTL_IGS_SUBSYSTEM_NOT_SYNC);			//reset state  Igs sync interlock
			ResetInterlock(INTL_OGR_NOT_SYNC);
			return RTC_INTERRUPT;					//hold on current state
		}
		else
		{
			CheckSubFsmState();
			CheckOGRFsmState();

			if (m_ResourceMngr->FsmResource.GuiCmd.Continue && \
				!HasInterlock() && !HasUnready() )
			{
				return JumpToNextState(RTC_INTERRUPTREADY);	//go to interruptready state
			}
			else if (HasTerminate() || (m_ResComponent->InputsOGR->CurrentState == RTC_TERMINATE))
			{
				return JumpToNextState(RTC_TERMINATE);	//go to terminate state
			}
			else
			{
				return RTC_INTERRUPT;					//hold on current state
			}
		}
	} 
}

/******************************************************************************
* Function: InterruptReadyMain(VOID)
*
* Describe: interrupt ready actions
*
* Input   :
*
* Output  :
*******************************************************************************/
FsmState_u CFsm::InterruptReadyMain(VOID)
{
	m_CurrentState = RTC_INTERRUPTREADY;

	if (m_ResourceMngr->FsmResource.GuiCmd.ClosePatient || m_ResourceMngr->FsmResource.GuiCmd.CleanPlan)
	{
		#if FSM_DEBUG_FALL_EN
			m_ResComponent->Trace->Log(tlInfo, "Line%d::CFsm::<InterruptReadyMain>::Jump to RTC_TERMINATE::Gui Close Patient Command", __LINE__);
		#endif

			return JumpToNextState(RTC_INTERRUPT);	//go to interrupt state
	}
	else if (FKP_CMD_BEAM_REVIEW == m_ResComponent->InputsOGR->FkpButton)
	{
		#if FSM_DEBUG_FALL_EN
			m_ResComponent->Trace->Log(tlInfo, "Line%d::CFsm::<InterruptReadyMain>::Jump to RTC_INTERRUPT::Press beam review button", __LINE__);
		#endif

		return JumpToNextState(RTC_INTERRUPT);	//go to interrupt state
	}
	else
	{
		if (m_StateSyncTimer.IsReady())
		{
			m_StateSyncTimer.StartTimer(m_ResourceMngr->GetCurrentSysTime(), INTERRUPTREADY_WAIT_TIME);
			return RTC_INTERRUPTREADY;					//hold on current state
		}
		else if (!m_StateSyncTimer.IsTimeOut(m_ResourceMngr->GetCurrentSysTime()))
		{
			ResetInterlock(INTL_BSM_SUBSYSTEM_NOT_SYNC);			//reset state bsm sync interlock
			ResetInterlock(INTL_BGM_SUBSYSTEM_NOT_SYNC);			//reset state  bgm sync interlock
			ResetInterlock(INTL_IGS_SUBSYSTEM_NOT_SYNC);			//reset state  Igs sync interlock
			ResetInterlock(INTL_OGR_NOT_SYNC);
			return RTC_INTERRUPTREADY;					//hold on current state
		}
		else
		{
			CheckSubFsmState();
			CheckOGRFsmState();
			
			if (m_ResourceMngr->FsmResource.GuiCmd.RadiationState && \
				!HasInterlock() && !HasUnready())
			{
				return JumpToNextState(RTC_RADIATION);	//go to radiation state
			}
			else if (HasInterlock() || HasUnready())
			{
				#if FSM_DEBUG_FALL_EN
				m_ResComponent->Trace->Log(tlInfo, "Line%d::CFsm::<InterruptReadyMain>::Jump to RTC_INTERRUPT::Has Intl::BGM H32=0x%lx L32=0x%lx, NGR =0x%lx , IGS =0x%lx , BSM =0x%lx ", \
					__LINE__, m_ResourceMngr->Interlocks.BgmInterlock >> 32, m_ResourceMngr->Interlocks.BgmInterlock,  m_ResourceMngr->Interlocks.NGRInterlock, \
				 m_ResourceMngr->Interlocks.IgsInterlock,  m_ResourceMngr->Interlocks.BsmInterlock);

				m_ResComponent->Trace->Log(tlInfo, "Line%d::CFsm::<InterruptReadyMain>::Jump to RTC_INTERRUPT:: unreadys ::BGM=0x%x,NGR=0x%x,Igs=0x%x,BSM=0x%x", \
					__LINE__, m_ResourceMngr->Unreadys.BgmUnready, m_ResourceMngr->Unreadys.NGRUnready, m_ResourceMngr->Unreadys.IgsUnready, m_ResourceMngr->Unreadys.BsmUnready);
				
				m_ResComponent->Trace->Log(tlInfo, \
					"Line%d::CFsm::<InterruptReadyMain>::GuiCom:%d CanOpen:%d EthercatBGM:%d EthercatBSM:%d", __LINE__,\
					m_ResComponent->InputsFsm->com_status_gui, m_ResComponent->FmDevices->CANOpen, m_ResComponent->FmDevices->BGMSlave, m_ResComponent->FmDevices->BSMSlave);
				#endif

				return JumpToNextState(RTC_INTERRUPT);	//go to interrupt state
			}
			else if (HasTerminate())
			{
				#if FSM_DEBUG_FALL_EN
					m_ResComponent->Trace->Log(tlInfo, "Line%d::CFsm::<InterruptReadyMain>::Jump to RTC_TERMINATE::Has Terminate::NGR Terminate:%u", __LINE__, m_ResourceMngr->Terminates.NGRTerminate);
				#endif

				return JumpToNextState(RTC_TERMINATE);	//go to terminate state
			}
			else
			{
				return RTC_INTERRUPTREADY;					//hold on current state
			}
		}
	}
}

/******************************************************************************
* Function: TerminateMain(VOID)
*
* Describe: terminate actions
*
* Input   :
*
* Output  :
*******************************************************************************/
FsmState_u CFsm::TerminateMain(VOID)
{
	m_CurrentState = RTC_TERMINATE;

	CheckSubFsmState();

	CheckOGRFsmState();

	if (m_ResourceMngr->FsmResource.GuiCmd.NextBeam)
	{
		#if FSM_DEBUG_FALL_EN
			m_ResComponent->Trace->Log(tlInfo, "Line%d::CFsm::<TerminateMain>::Jump to RTC_PREPARATORY::GUI Next Beam Command", __LINE__);
		#endif

		return JumpToNextState(RTC_PREPARATORY);	//go to preparatory state
	}
	else
	{
		return RTC_TERMINATE;
	}
}

/******************************************************************************
* Function: OffMain(VOID)
*
* Describe: off actions
*
* Input   :
*
* Output  :
*******************************************************************************/
FsmState_u CFsm::OffMain(VOID)
{
	m_CurrentState = RTC_OFF; //Off state
	
	if (m_ResourceMngr->FsmResource.GuiCmd.ShutDown)
		return JumpToNextState(RTC_SHUTDOWN); //go to shutdown state
	else if (m_ResourceMngr->FsmResource.GuiCmd.Login)
		return JumpToNextState(RTC_PREPARATORY); //go to preparatory state
	else
		return RTC_OFF; //Hold on Shut down state
}

/******************************************************************************
* Function: ShutDownMain(VOID)
*
* Describe: shut down actions
*
* Input   :
*
* Output  :
*******************************************************************************/
FsmState_u CFsm::ShutDownMain(VOID)
{
	m_CurrentState = RTC_SHUTDOWN; //Shut down state

	return RTC_SHUTDOWN; //Hold on Shut down state
}

/******************************************************************************
* Function: UpdateInputResource(VOID)
*
* Describe: update FSM input resource
*
* Input   :
*
* Output  :
*******************************************************************************/
BOOL CFsm::UpdateInputResource(VOID)
{
	//Reset radiation time out interlock
	if (m_ResourceMngr->FsmResource.GuiCmd.ClosePatient || m_ResourceMngr->FsmResource.GuiCmd.CleanPlan)
	{
		m_RadiationProtTimer.ResetTimer();
		m_ResComponent->OutputsBeam->RadiationTime = 0; //reset GUI radiation time display
	}
	else
	{ 
		; //do nothing
	}
	
	return TRUE;
}

/******************************************************************************
* Function: UpdateInputResource(VOID)
*
* Describe: update FSM input resource
*
* Input   :
*
* Output  :
*******************************************************************************/
BOOL CFsm::UpdateOutputResource(VOID)
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

	return TRUE;
}

/******************************************************************************
* Function: JumpToNextState(UINT8 RequiredState)
*
* Describe: 
*
* Input   :
*
* Output  :
*******************************************************************************/
FsmState_u CFsm::JumpToNextState(FsmState_u RequiredState)
{
	m_LastState = m_CurrentState;		//update last state
	m_StateSyncTimer.ResetTimer();		//reset timer

	return RequiredState;				//go to standby state
}

/******************************************************************************
* Function: CheckSubFsmState(VOID)
*
* Describe:
*
* Input   :
*
* Output  :
*******************************************************************************/
VOID CFsm::CheckBgmFsmState(VOID)
{
	BOOL SyncFlag = FALSE;

	switch (m_CurrentState)
	{
		case RTC_INIT:
		{
			if ((m_ResComponent->InputsSubFsm->BgmCurrentState == RTC_INIT) || \
				(m_ResComponent->InputsSubFsm->BgmCurrentState == RTC_STANDBY))
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
			if (m_ResComponent->InputsSubFsm->BgmCurrentState == RTC_STANDBY)
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
			if (m_ResComponent->InputsSubFsm->BgmCurrentState == RTC_PREPARATORY || m_ResComponent->InputsSubFsm->BgmCurrentState == RTC_PREPARE)
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
			if ((m_ResComponent->InputsSubFsm->BgmCurrentState == RTC_PREPARE) || (m_ResComponent->InputsSubFsm->BgmCurrentState == RTC_READY))
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
			if (m_ResComponent->InputsSubFsm->BgmCurrentState == RTC_READY)
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
			if (m_ResComponent->InputsSubFsm->BgmCurrentState == RTC_RADIATION || m_ResComponent->InputsSubFsm->BgmCurrentState == RTC_COMPLETE)
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
			if (m_ResComponent->InputsSubFsm->BgmCurrentState == RTC_REPOSITION)
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
		case RTC_COMPLETE:
		{
			if (m_ResComponent->InputsSubFsm->BgmCurrentState == RTC_COMPLETE)
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
			if (m_ResComponent->InputsSubFsm->BgmCurrentState == RTC_INTERRUPT)
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
		case RTC_INTERRUPTREADY:
		{
			if (m_ResComponent->InputsSubFsm->BgmCurrentState == RTC_INTERRUPTREADY)
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
		case RTC_TERMINATE:
		{
			if (m_ResComponent->InputsSubFsm->BgmCurrentState == RTC_TERMINATE)
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
		ResetInterlock(INTL_BGM_SUBSYSTEM_NOT_SYNC);
	else
		SetInterlock(INTL_BGM_SUBSYSTEM_NOT_SYNC);

	return;
}

/******************************************************************************
* Function: CheckIgsFsmState(VOID)
*
* Describe:
*
* Input   :
*
* Output  :
*******************************************************************************/
VOID CFsm::CheckIgsFsmState(VOID)
{
	//Check igs subfsm state
	BOOL SyncFlag = FALSE;
	if (IGS_STANDBY == m_ResourceMngr->IgsResource.GetCurrentState())
		SyncFlag = TRUE;
	else
		SyncFlag = FALSE;

	//Set interlock
	if (SyncFlag)
		ResetInterlock(INTL_IGS_SUBSYSTEM_NOT_SYNC);
	else
		SetInterlock(INTL_IGS_SUBSYSTEM_NOT_SYNC);

	return;
}

/******************************************************************************
* Function: CheckSubFsmState(VOID)
*
* Describe:
*
* Input   :
*
* Output  :
*******************************************************************************/
VOID CFsm::CheckSubFsmState(VOID)
{
	CheckBgmFsmState();
	CheckBSMFsmState();
	CheckIgsFsmState();

	return;
}

/******************************************************************************
* Function: CheckOGRFsmState(VOID)
*
* Describe:
*
* Input   :
*
* Output  :
*******************************************************************************/
BOOL CFsm::CheckOGRFsmState(VOID)
{
	switch (m_CurrentState)
	{
		case RTC_PREPARATORY:
		{
			if ((m_ResComponent->InputsOGR->CurrentState == RTC_PREPARATORY) || (m_ResComponent->InputsOGR->CurrentState == RTC_PREPARE))
				ResetInterlock(INTL_OGR_NOT_SYNC);
			else
				SetInterlock(INTL_OGR_NOT_SYNC);
			break;
		}
		case RTC_PREPARE:
		{
			if ((m_ResComponent->InputsOGR->CurrentState == RTC_PREPARE) || (m_ResComponent->InputsOGR->CurrentState == RTC_READY))
				ResetInterlock(INTL_OGR_NOT_SYNC);
			else
				SetInterlock(INTL_OGR_NOT_SYNC);
			break;
		}
		case RTC_READY:
		{
			if ((m_ResComponent->InputsOGR->CurrentState == RTC_READY) || (m_ResComponent->InputsOGR->CurrentState == RTC_RADIATION))
				ResetInterlock(INTL_OGR_NOT_SYNC);
			else
				SetInterlock(INTL_OGR_NOT_SYNC);
			break;
		}
		case RTC_INTERRUPT:
		{
			if ((m_ResComponent->InputsOGR->CurrentState == RTC_INTERRUPT) || (m_ResComponent->InputsOGR->CurrentState == RTC_INTERRUPTREADY))
				ResetInterlock(INTL_OGR_NOT_SYNC);
			else
				SetInterlock(INTL_OGR_NOT_SYNC);
			break;
		}
		case RTC_INTERRUPTREADY:
		{
			if ((m_ResComponent->InputsOGR->CurrentState == RTC_INTERRUPTREADY) || (m_ResComponent->InputsOGR->CurrentState == RTC_RADIATION))
				ResetInterlock(INTL_OGR_NOT_SYNC);
			else
				SetInterlock(INTL_OGR_NOT_SYNC);
			break;
		}
		default:
		{
			if (m_ResComponent->InputsOGR->CurrentState == m_CurrentState)
				ResetInterlock(INTL_OGR_NOT_SYNC);
			else
				SetInterlock(INTL_OGR_NOT_SYNC);
			break;
		}
	}
	
	return TRUE;
}

/******************************************************************************
* Function: SetInterlock(UINT16 Intl)
*
* Describe:
*
* Input   :
*
* Output  :
*******************************************************************************/
BOOL CFsm::SetInterlock(UINT16 Intl)
{
	m_ResourceMngr->Interlocks.NGRInterlock |= Intl;
	
	return TRUE;
}

/******************************************************************************
* Function: ResetInterlock(UINT16 Intl)
*
* Describe:
*
* Input   :
*
* Output  :
*******************************************************************************/
BOOL CFsm::ResetInterlock(UINT16 Intl)
{
	m_ResourceMngr->Interlocks.NGRInterlock &= ~Intl;

	return TRUE;
}

/******************************************************************************
* Function: SetUnready(UINT32 Intl)
*
* Describe:
*
* Input   :
*
* Output  :
*******************************************************************************/
BOOL CFsm::SetUnready(UINT32 Intl)
{
	m_ResourceMngr->Unreadys.NGRUnready |= static_cast<UINT32>(Intl);

	return TRUE;
}

/******************************************************************************
* Function: ResetUnready(UINT32 Intl)
*
* Describe:
*
* Input   :
*
* Output  :
*******************************************************************************/
BOOL CFsm::ResetUnready(UINT32 Intl)
{
	m_ResourceMngr->Unreadys.NGRUnready &= ~(static_cast<UINT32>(Intl));

	return TRUE;
}

/******************************************************************************
* Function: NoInterlock(VOID)
*
* Describe:
*
* Input   :
*
* Output  :
*******************************************************************************/
BOOL CFsm::HasInterlock(VOID)
{
	UINT64 TempInterlock = m_ResourceMngr->Interlocks.BgmInterlock;
	if (m_ResComponent->InputsDose->DoseInterlockEnable & DOSE_RATE_LOW_INTL_EN)
	{
		; //do nothing
	}
	else
	{
		TempInterlock &= ~INTL_DOSE_OVER_LOWLIMIT; //override dose rate low interlock
	}

	if (m_ResComponent->InputsDose->DoseInterlockEnable & DOSE_RATE_HIGH_INTL_EN)
	{
		; //do nothing
	}
	else
	{
		TempInterlock &= ~INTL_DOSE_OVER_HIGHLIMIT; //override dose rate high interlock
	}

	if (TempInterlock == 0 && m_ResourceMngr->Interlocks.NGRInterlock == 0 &&\
		m_ResourceMngr->Interlocks.BsmInterlock == 0 && m_ResourceMngr->Interlocks.IgsInterlock == 0)
		return FALSE;
	else
		return TRUE;
}

/******************************************************************************
* Function: HasUnready(VOID)
*
* Describe:
*
* Input   :
*
* Output  :
*******************************************************************************/
BOOL CFsm::HasUnready(VOID)
{
	if (m_ResourceMngr->Unreadys.BgmUnready == 0 && \
		m_ResourceMngr->Unreadys.NGRUnready == 0 && \
		m_ResourceMngr->Unreadys.BsmUnready == 0)
		return FALSE;
	else
		return TRUE;
}

/******************************************************************************
* Function: HasTerminate(VOID)
*
* Describe:
*
* Input   :
*
* Output  :
*******************************************************************************/
BOOL CFsm::HasTerminate(VOID)
{
	BOOL HasTerminateFlag = FALSE;

	m_ResourceMngr->FsmResource.GuiCmd.TCSNotready	? HasTerminateFlag = TRUE : NULL;
	m_ResourceMngr->Terminates.NGRTerminate			? HasTerminateFlag = TRUE : NULL;

	if (HasTerminateFlag)
		return TRUE;
	else
		return FALSE;
}

/******************************************************************************
* Function: OGRIsOk(VOID)
*
* Describe:
*
* Input   :
*
* Output  :
*******************************************************************************/
BOOL CFsm::OGRIsOk(VOID)
{
	return !m_ResourceMngr->OGR_IUT;
}

/******************************************************************************
* Function: HardwareIsReady(VOID)
*
* Describe:
*
* Input   :
*
* Output  :
*******************************************************************************/
BOOL CFsm::HardwareIsReady(VOID)
{
	if ((m_ResourceMngr->Interlocks.NGRInterlock &INTL_CANOPEN_MASTER_COMM_ERROR) || (m_ResourceMngr->Interlocks.NGRInterlock &INTL_ETHERCAT_BSM_COMM_ERROR)\
		|| (m_ResourceMngr->Interlocks.NGRInterlock &INTL_ETHERCAT_BGM_COMM_ERROR) || (m_ResourceMngr->Interlocks.NGRInterlock &INTL_ETHERCAT_IGS_COMM_ERROR))
	{
		return FALSE;
	}
	else
	{
		return TRUE;
	}
}

/******************************************************************************
* Function: CheckBSMFsmState(VOID)
*
* Describe:
*
* Input   :
*
* Output  :
*******************************************************************************/
VOID CFsm::CheckBSMFsmState(VOID)
{
	BOOL SyncFlag = FALSE;
	BYTE BsmCurrentState = m_ResComponent->InputsBsm->CurrentState & 0x7f;
	
	switch (m_CurrentState)
	{
		case RTC_INIT:
		{
			if (BsmCurrentState == BSM_INITIAL)
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
			if (BsmCurrentState == BSM_STANDBY)
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
			if (BsmCurrentState == BSM_STANDBY || BsmCurrentState == BSM_MANUAL)
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
			if (BsmCurrentState == BSM_PREPARE)
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
			if (BsmCurrentState == BSM_SERVO)
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
			if (BsmCurrentState == BSM_SERVO)
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
			if (BsmCurrentState == BSM_PREPARE)
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
		case RTC_COMPLETE:
		{
			if (BsmCurrentState == BSM_STANDBY )
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
			if (BsmCurrentState == BSM_SERVO)
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
		case RTC_INTERRUPTREADY:
		{
			if (BsmCurrentState == BSM_SERVO)
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
		case RTC_TERMINATE:
		{
			if (BsmCurrentState == BSM_STANDBY)
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
			if (BsmCurrentState == BSM_OFF)
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
		case RTC_SHUTDOWN:
		{
			if (BsmCurrentState == BSM_OFF)
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
		default:
		{
			m_ResComponent->Trace->Log(tlInfo, "Line%d::CFsm::<StateIsSync>::error state", __LINE__);
			return;
		}
	}

	//Set interlock
	if (SyncFlag)
		ResetInterlock(INTL_BSM_SUBSYSTEM_NOT_SYNC);
	else
		SetInterlock(INTL_BSM_SUBSYSTEM_NOT_SYNC);

	return;
}

/******************************************************************************
* Function: InitSubFsmReqState(VOID)
*
* Describe:
*
* Input   :
*
* Output  :
*******************************************************************************/
VOID CFsm::InitSubFsmReqState(VOID)
{
	//Init bgm required state
	m_SubFsmReqState.BgmReqState[static_cast<BYTE>(FsmTypeMv)][0] = static_cast<BYTE>(RTC_NO_STATE);
	m_SubFsmReqState.BgmReqState[static_cast<BYTE>(FsmTypeMv)][1] = static_cast<BYTE>(RTC_INIT);
	m_SubFsmReqState.BgmReqState[static_cast<BYTE>(FsmTypeMv)][2] = static_cast<BYTE>(RTC_STANDBY);
	m_SubFsmReqState.BgmReqState[static_cast<BYTE>(FsmTypeMv)][3] = static_cast<BYTE>(RTC_PREPARATORY);
	m_SubFsmReqState.BgmReqState[static_cast<BYTE>(FsmTypeMv)][4] = static_cast<BYTE>(RTC_PREPARE);
	m_SubFsmReqState.BgmReqState[static_cast<BYTE>(FsmTypeMv)][5] = static_cast<BYTE>(RTC_READY);
	m_SubFsmReqState.BgmReqState[static_cast<BYTE>(FsmTypeMv)][6] = static_cast<BYTE>(RTC_RADIATION);
	m_SubFsmReqState.BgmReqState[static_cast<BYTE>(FsmTypeMv)][7] = static_cast<BYTE>(RTC_REPOSITION);
	m_SubFsmReqState.BgmReqState[static_cast<BYTE>(FsmTypeMv)][8] = static_cast<BYTE>(RTC_COMPLETE);
	m_SubFsmReqState.BgmReqState[static_cast<BYTE>(FsmTypeMv)][9] = static_cast<BYTE>(RTC_INTERRUPT);
	m_SubFsmReqState.BgmReqState[static_cast<BYTE>(FsmTypeMv)][10] = static_cast<BYTE>(RTC_INTERRUPTREADY);
	m_SubFsmReqState.BgmReqState[static_cast<BYTE>(FsmTypeMv)][11] = static_cast<BYTE>(RTC_TERMINATE);
	m_SubFsmReqState.BgmReqState[static_cast<BYTE>(FsmTypeMv)][12] = static_cast<BYTE>(RTC_OFF);
	m_SubFsmReqState.BgmReqState[static_cast<BYTE>(FsmTypeMv)][13] = static_cast<BYTE>(RTC_SHUTDOWN);

	m_SubFsmReqState.BgmReqState[static_cast<BYTE>(FsmTypeKv)][0] = static_cast<BYTE>(RTC_NO_STATE);
	m_SubFsmReqState.BgmReqState[static_cast<BYTE>(FsmTypeKv)][1] = static_cast<BYTE>(RTC_INIT);
	m_SubFsmReqState.BgmReqState[static_cast<BYTE>(FsmTypeKv)][2] = static_cast<BYTE>(RTC_STANDBY);
	m_SubFsmReqState.BgmReqState[static_cast<BYTE>(FsmTypeKv)][3] = static_cast<BYTE>(RTC_PREPARATORY);
	m_SubFsmReqState.BgmReqState[static_cast<BYTE>(FsmTypeKv)][4] = static_cast<BYTE>(RTC_PREPARATORY);
	m_SubFsmReqState.BgmReqState[static_cast<BYTE>(FsmTypeKv)][5] = static_cast<BYTE>(RTC_PREPARATORY);
	m_SubFsmReqState.BgmReqState[static_cast<BYTE>(FsmTypeKv)][6] = static_cast<BYTE>(RTC_PREPARATORY);
	m_SubFsmReqState.BgmReqState[static_cast<BYTE>(FsmTypeKv)][7] = static_cast<BYTE>(RTC_PREPARATORY);
	m_SubFsmReqState.BgmReqState[static_cast<BYTE>(FsmTypeKv)][8] = static_cast<BYTE>(RTC_PREPARATORY);
	m_SubFsmReqState.BgmReqState[static_cast<BYTE>(FsmTypeKv)][9] = static_cast<BYTE>(RTC_PREPARATORY);
	m_SubFsmReqState.BgmReqState[static_cast<BYTE>(FsmTypeKv)][10] = static_cast<BYTE>(RTC_PREPARATORY);
	m_SubFsmReqState.BgmReqState[static_cast<BYTE>(FsmTypeKv)][11] = static_cast<BYTE>(RTC_PREPARATORY);
	m_SubFsmReqState.BgmReqState[static_cast<BYTE>(FsmTypeKv)][12] = static_cast<BYTE>(RTC_PREPARATORY);
	m_SubFsmReqState.BgmReqState[static_cast<BYTE>(FsmTypeKv)][13] = static_cast<BYTE>(RTC_PREPARATORY);

	return;
}

/******************************************************************************
* Function: GetBgmRequiredState(VOID)
*
* Describe:
*
* Input   :
*
* Output  :
*******************************************************************************/
BYTE CFsm::GetBgmRequiredState(VOID)
{
	BeamPlan_t* beamPlanPtr = m_ResourceMngr->PlanResource.GetBeamPlanData();
	//Look up bgm requird state
	if (m_ResourceMngr->PlanResource.PlanIsReady())
	{
		if (RadiationKv == beamPlanPtr->RadiationType)
			return static_cast<BYTE>(m_SubFsmReqState.BgmReqState[static_cast<BYTE>(FsmTypeKv)][static_cast<BYTE>(m_CurrentState)]);
		else
			return static_cast<BYTE>(m_SubFsmReqState.BgmReqState[static_cast<BYTE>(FsmTypeMv)][static_cast<BYTE>(m_CurrentState)]);
	}
	else
	{
		return static_cast<BYTE>(m_SubFsmReqState.BgmReqState[static_cast<BYTE>(FsmTypeMv)][static_cast<BYTE>(m_CurrentState)]);
	}
}

/******************************************************************************
* Function: Initialize(VOID)
*
* Describe: 
*
* Input   : None
*
* Output  : None
*******************************************************************************/
VOID CFsm::Initialize(VOID)
{
	if (m_InitEn)
	{
		InitSubFsmReqState();
		m_InitEn = FALSE;
	}
	else
	{
		//do nothing
	}

	return;
}

/******************************************************************************
* Function: ExecuteBusiness(VOID)
*
* Describe:
*
* Input   : None
*
* Output  : None
*******************************************************************************/
VOID CFsm::ExecuteBusiness(VOID)
{
	switch (m_RequiredState)
	{
	case RTC_NO_STATE:
	{
		m_RequiredState = PowerUpMain();			//power up
		break;
	}
	case RTC_INIT:
	{
		m_RequiredState = InitMain();				//initialization
		break;
	}
	case RTC_STANDBY:
	{
		m_RequiredState = StandbyMain();			//standby
		break;
	}
	case RTC_PREPARATORY:
	{
		m_RequiredState = PreparatoryMain();		//preparatory
		break;
	}
	case RTC_PREPARE:
	{
		m_RequiredState = PrepareMain();			//prepare
		break;
	}
	case RTC_READY:
	{
		m_RequiredState = ReadyMain();				//ready
		break;
	}
	case RTC_RADIATION:
	{
		m_RequiredState = RadiationMain();			//radiation
		break;
	}
	case RTC_REPOSITION:
	{
		m_RequiredState = RepositionMain();			//reposition
		break;
	}
	case RTC_COMPLETE:
	{
		m_RequiredState = CompleteMain();			//complete
		break;
	}
	case RTC_INTERRUPT:
	{
		m_RequiredState = InterruptMain();			//interrupt
		break;
	}
	case RTC_INTERRUPTREADY:
	{
		m_RequiredState = InterruptReadyMain();		//interruptready
		break;
	}
	case RTC_TERMINATE:
	{
		m_RequiredState = TerminateMain();			//terminate
		break;
	}
	case RTC_OFF:
	{
		m_RequiredState = OffMain();				//off
		break;
	}
	case RTC_SHUTDOWN:
	{
		m_RequiredState = ShutDownMain();			//shut down
		break;
	}
	default:
	{
		#if RTC_DEBUG_EXCEPTION
		m_ResComponent->Trace->Log(tlInfo, "Line:%d::CRTC_StateMachine::next_state::Unknown state::%d", __LINE__, m_RequiredState);
		#endif

		break;
	}
	}
}

/******************************************************************************
* Function: ExecuteBusiness(VOID)
*
* Describe:
*
* Input   : None
*
* Output  : None
*******************************************************************************/

VOID CFsm::ResetStateSyncTimer(VOID)
{
	m_StateSyncTimer.ResetTimer();
}