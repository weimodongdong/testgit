/*******************************************************************************
** File	      : ResourceMngr.cpp
** Author:    : ChenLong
** Version    :	1.0
** Environment: VS2013
** Description:
** History    :	17/12/13    Chenlong    Created
                18/08/07    LiBo       Modify 
*******************************************************************************/
/* INCLUDE ===================================================================*/
#include "ResourceMngr.h"

/* FUNCTIONS =================================================================*/
/******************************************************************************
* Function: CResourceMngr()
*
* Describe:
*
* Input   :
*
* Output  : None
*******************************************************************************/
CResourceMngr::CResourceMngr()
{
	//Public resource init
	m_PlanTxtPtr = NULL;
	hFile = NULL;
	memset(&Interlocks, 0, sizeof(Interlocks));
	memset(&Unreadys, 0, sizeof(Unreadys));
	memset(&Terminates, 0, sizeof(Terminates));
	OGR_IUT = 0;
	
	//Private resource init
	m_CurrentSysTime = 0;
	memset(&m_ResComponent, 0, sizeof(m_ResComponent));
	m_InitEn = TRUE;
	m_GuiComOk = FALSE;
}

CResourceMngr::~CResourceMngr()
{
	//variables
}

/******************************************************************************
* Function: CycleUpdate()
*
* Describe:
*
* Input   :
*
* Output  : None
*******************************************************************************/
BOOL CResourceMngr::CycleUpdate()
{
	Initialize();

	if (ResourceIsReady())
	{
		UpdateInputResource();

		UpdateOutputResource();
	}
	else
	{
		#if RTC_DEBUG_EXCEPTION
		m_ResComponent.Trace->Log(tlInfo, "Line:%d::CResourceMngr::Resource is not ready::%d", __LINE__);
		#endif
	}

	return TRUE;
}

/******************************************************************************
* Function: GetCmd(UINT8 cmd)
*
* Describe:
*
* Input   :
*
* Output  :
*******************************************************************************/
BOOL CResourceMngr::SetCmdStruct(VOID)
{
	memset(&FsmResource.GuiCmd, 0, sizeof(FsmResource.GuiCmd));

	GetCmd(m_ResComponent.InputsFsm->gui_cmd1);
	GetCmd(m_ResComponent.InputsFsm->gui_cmd2);
	GetCmd(m_ResComponent.InputsFsm->gui_cmd3);
	GetCmd(m_ResComponent.InputsFsm->gui_cmd4);
	GetCmd(m_ResComponent.InputsFsm->gui_cmd5);
	GetCmd(m_ResComponent.InputsFsm->gui_cmd6);
	GetCmd(m_ResComponent.InputsFsm->gui_cmd7);
	GetCmd(m_ResComponent.InputsFsm->gui_cmd8);

	return TRUE;
}

/******************************************************************************
* Function: GetCmd(UINT8 cmd)
*
* Describe:
*
* Input   :
*
* Output  :
*******************************************************************************/
UINT8 CResourceMngr::GetCmd(UINT8 cmd)
{
	switch (cmd)
	{
		case GUI_CMD_RESET:
		{
			FsmResource.GuiCmd.Reset = TRUE;	//reset command
			break;
		}
		case GUI_CMD_CLINICAL:
		{
			FsmResource.GuiCmd.Clinical = TRUE;	//clinical command
			FsmResource.GuiCurrentMode = GUI_CMD_CLINICAL;		//cmd identifer is mode identifer
			break;
		}
		case GUI_CMD_PHYSICS:
		{
			FsmResource.GuiCmd.Physics = TRUE;	//physics command
			break;
		}
		case GUI_CMD_SERVICE:
		{
			FsmResource.GuiCmd.Service = TRUE;	//service command
			FsmResource.GuiCurrentMode = GUI_CMD_SERVICE;		//cmd identifer is mode identifer
			break;
		}
		case GUI_CMD_DAILY_QA:
		{
			FsmResource.GuiCmd.DailyQA = TRUE;	//daily qa command
			break;
		}
		case GUI_CMD_LOGIN:
		{
			FsmResource.GuiCmd.Login = TRUE;	//login command
			break;
		}
		case GUI_CMD_LOGOUT:
		{
			FsmResource.GuiCmd.Logout = TRUE;	//logout command
			FsmResource.GuiCurrentMode = 0;		//reset mode identifer
			break;
		}
		case GUI_CMD_NEXT_BEAM:
		{
			FsmResource.GuiCmd.NextBeam = TRUE;	//next beam command
			break;
		}
		case GUI_CMD_CONTINUE:
		{
			FsmResource.GuiCmd.Continue = TRUE;	//continue command
			break;
		}
		case GUI_CMD_STOP_RADIATION:
		{
			FsmResource.GuiCmd.StopRadiation = TRUE;	//stop radiation command
			break;
		}
		case GUI_CMD_CLOSE_PATIENT:
		{
			FsmResource.GuiCmd.ClosePatient = TRUE;	//close patient command
			break;
		}
		case GUI_CMD_EXCEPTION_BEAMOFF:
		{
			FsmResource.GuiCmd.ExceptionBeamOff = TRUE;			//exception beam off command
			break;
		}
		case GUI_CMD_PREPARE:
		{
			FsmResource.GuiCmd.PrepareState = TRUE;			//off state patient command
			break;
		}
		case GUI_CMD_RADIATION:
		{
			FsmResource.GuiCmd.RadiationState = TRUE;			//off state patient command
			break;
		}
		case GUI_CMD_KV_IMAGE:
		{
			FsmResource.GuiCmd.KvImage = TRUE;			//off state patient command
			break;
		}
		case GUI_CMD_TCSNOTREADY:
		{
			FsmResource.GuiCmd.TCSNotready = TRUE;			//off state patient command
			break;
		}
		case GUI_CMD_TCSREADY:
		{
			FsmResource.GuiCmd.TCSReady = TRUE;			//off state patient command
			break;
		}
		case GUI_CMD_OFFSTATE:
		{
			FsmResource.GuiCmd.OffState = TRUE;			//off state command
			break;
		}
		case GUI_CMD_SHUTDOWN:
		{
			FsmResource.GuiCmd.ShutDown = TRUE;			//off state patient command
			break;
		}
		case GUI_CMD_ClEANPLAN:
		{
			FsmResource.GuiCmd.CleanPlan = TRUE;			//off state patient command
			break;
		}
		case GUI_CMD_IGSCT_NOTREADY:
		{
			FsmResource.GuiCmd.IgsCTNotReady = TRUE;			//off state patient command
			break;
		}
		case GUI_CMD_IGSCT_READY:
		{
			FsmResource.GuiCmd.IgsCTReady = TRUE;			//off state patient command
			break;
		}
		default:
		{
			break;
		}
	}

	return TRUE;
}

/******************************************************************************
* Function: ResourceIsReady(VOID)
*
* Describe:
*
* Input   :
*
* Output  :
*******************************************************************************/
BOOL CResourceMngr::ResourceIsReady(VOID)
{
	if(m_ResComponent.InputsFsm == NULL || \
		m_ResComponent.OutputsFsm == NULL || \
		m_ResComponent.InputsSubFsm == NULL || \
		m_ResComponent.OutputsSubFsm == NULL || \
		m_ResComponent.InputsOGR == NULL || \
		m_ResComponent.OutputsOGR == NULL || \
		m_ResComponent.InputsBeam == NULL || \
		m_ResComponent.OutputsBeam == NULL || \
		m_ResComponent.InputsDose == NULL || \
		m_ResComponent.OutputsDose == NULL || \
		m_ResComponent.PlanInputs == NULL || \
		m_ResComponent.PlanOutputs == NULL || \
		m_ResComponent.InputsBgmPlan == NULL || \
		m_ResComponent.OutputsBgmPlan == NULL || \
		m_ResComponent.InputsIgsSlv == NULL || \
		m_ResComponent.OutputsIgsSlv == NULL || \
		m_ResComponent.InputsBsm == NULL || \
		m_ResComponent.OutputsBsm == NULL || \
		m_ResComponent.InputsVMAT == NULL || \
		m_ResComponent.OutputsVMAT == NULL || \
		m_ResComponent.InputsCtrlState == NULL || \
		m_ResComponent.OutputsCtrlState == NULL || \
		m_ResComponent.InputsOnGantryHD == NULL || \
		m_ResComponent.OutputsOnGantryHD == NULL || \
		m_ResComponent.InputsBsmFmGUI == NULL || \
		m_ResComponent.OutputsBsmToGUI == NULL || \
		m_ResComponent.InputsIgs == NULL || \
		m_ResComponent.OutputsIgs == NULL || \
		m_ResComponent.FmDevices == NULL || \
		m_ResComponent.IUTMonitor == NULL)
		return FALSE;
	else
		return TRUE;
}

/******************************************************************************
* Function: UpdateMngrResource(VOID)
*
* Describe:
*
* Input   :
*
* Output  :
*******************************************************************************/
BOOL CResourceMngr::UpdateInputResource(VOID)
{
	//Get BGM input interlock
	Interlocks.BgmInterlock = m_ResComponent.InputsSubFsm->BgmInterlocks;		//get BGM interlock
	Interlocks.BsmInterlock = BsmResource.GetInterlock();
	Interlocks.IgsInterlock = IgsResource.Getinterlock();
	if (m_ResComponent.OutputsFsm->rtc_current_state == 3 || m_ResComponent.OutputsFsm->rtc_current_state == 9)	//3->Prepatory     9->Interrupt
	{
		Interlocks.BgmInterlock &= ~INTL_MOD_HV;	//override modulator hv interlock
		Interlocks.BgmInterlock &= ~INTL_MOD_TRIG;	//override modulator trig interlock
		Interlocks.BgmInterlock &= ~INTL_HV_CONTACTOR_ERROR;	//override modulator hv contactor interlock
		Interlocks.BgmInterlock &= ~INTL_SLIPRING_HV;	//override slipring hv interlock
	}

	//Get BGM input unready
	Unreadys.BgmUnready = static_cast<UINT8>(m_ResComponent.InputsSubFsm->BgmUnreadys);
	Unreadys.BsmUnready = static_cast<UINT8>(BsmResource.GetUnready());
	Unreadys.IgsUnready = static_cast<UINT8>(IgsResource.GetNotReady());

	//Get OffGantryRTC input IUT
	OGR_IUT = m_ResComponent.InputsOGR->IUT;

	//Get GUI command
	SetCmdStruct();

	//Check hardware state and set interlock
	CheckHardwareState();

	//Check GUI case and set interlock
	CheckGuiCase();

	//check  TCs  ready
	CheckTcsReady();

	//Reset radiation timer
	if (FsmResource.GuiCmd.Reset)
		FsmResource.SetRadiationTimeOut(FALSE);	//reset beam timer interlock
	else{ ; }

	//Check Input Terminates
	UpdateInputTerminates();
		
	return TRUE;
}

/******************************************************************************
* Function: UpdateOutputResource(VOID)
*
* Describe:
*
* Input   :
*
* Output  :
*******************************************************************************/
BOOL CResourceMngr::UpdateOutputResource(VOID)
{
	//Ouput dose value to GUI & OGR
	m_ResComponent.OutputsOGR->DoseTotal = static_cast<UINT>(m_ResComponent.InputsDose->DoseTotal);
	m_ResComponent.OutputsOGR->DoseSegment = static_cast<UINT>(m_ResComponent.InputsDose->DoseSegment);

	//Ouput beam setting value to BGM
	m_ResComponent.OutputsBeam->beam_total_set1 = m_ResComponent.InputsBeam->beam_total_set1;

	//Output interlock to GUI Thread
	m_ResComponent.OutputsFsm->Interlocks.BgmInterlock1 = static_cast<UINT32>(Interlocks.BgmInterlock);
	m_ResComponent.OutputsFsm->Interlocks.BgmInterlock2 = static_cast<UINT32>(Interlocks.BgmInterlock>>32);
	
	m_ResComponent.OutputsFsm->Interlocks.NGRInterlock = Interlocks.NGRInterlock;
	m_ResComponent.OutputsFsm->Interlocks.IgsInterlock = Interlocks.IgsInterlock;

	if (Interlocks.BgmInterlock & (static_cast<UINT64>(0x01) << 36))
		m_ResComponent.OutputsDose->LowDoseRateInterlock = TRUE;
	else
		m_ResComponent.OutputsDose->LowDoseRateInterlock = FALSE;

	if (Interlocks.BgmInterlock & (static_cast<UINT64>(0x01) << 35))
		m_ResComponent.OutputsDose->HighDoseRateInterlock = TRUE;
	else
		m_ResComponent.OutputsDose->HighDoseRateInterlock = FALSE;
		
	//m_ResComponent.OutputsFsm->Interlocks.BsmInterlock = Interlocks.BsmInterlock;

	m_ResComponent.OutputsFsm->Interlocks.BsmInterlock1 = static_cast<UINT32>(Interlocks.BsmInterlock);
	m_ResComponent.OutputsFsm->Interlocks.BsmInterlock2 = static_cast<UINT32>(Interlocks.BsmInterlock >> 32);
	//Output unready to GUI Thread
	m_ResComponent.OutputsFsm->Unreadys.NGRUnready = Unreadys.NGRUnready;
	m_ResComponent.OutputsFsm->Unreadys.BgmUnready = Unreadys.BgmUnready;
	m_ResComponent.OutputsFsm->Unreadys.BsmUnready = Unreadys.BsmUnready;
	m_ResComponent.OutputsFsm->Unreadys.IgsUnready = Unreadys.IgsUnready;

	//Check parameter crc
	CheckParameterFile();

	//sync ITU to OGR
	SyncItuToOGR();

	//Output IUT to monitor
	OutputToMonitor();

	OutputRadiationIndexToGUI();
	//Ouput radiation to BGM from OGR
	m_ResComponent.OutputsOGR->RadiationIndex = m_ResComponent.InputsOGR->RadiationIndex;

	//Outputs dose interlock enable to BGM
	m_ResComponent.OutputsDose->DoseInterlockEnable = m_ResComponent.InputsDose->DoseInterlockEnable;
	
	//Output controller emulator state 
	m_ResComponent.OutputsCtrlState->ControllerEmuSet.RgsCtrl = m_ResComponent.InputsCtrlState->ControllerEmuMode.RgsCtrl;
	m_ResComponent.OutputsCtrlState->ControllerEmuSet.BsmCtrl = m_ResComponent.InputsCtrlState->ControllerEmuMode.BsmCtrl;
	m_ResComponent.OutputsCtrlState->ControllerEmuSet.IgsCtrl = m_ResComponent.InputsCtrlState->ControllerEmuMode.IgsCtrl;

	//Out watch dog to GUI
	BYTE WatchDogState = 0;
	WatchDogState |= m_ResComponent.FmDevices->OnGantryRTCWdOk ? WD_NGR_OK : 0x00;
	WatchDogState |= static_cast<BOOL>(m_ResComponent.InputsBsm->Status&0x0001) ? WD_BSM_OK : 0x00;
	WatchDogState |= m_ResComponent.FmDevices->BgmWdOk ? WD_BGM_OK : 0x00;
	WatchDogState |= m_ResComponent.FmDevices->IgsWdOk ? WD_IGS_OK : 0x00;
	WatchDogState |= m_ResComponent.FmDevices->RgsWdOk ? WD_RGS_OK : 0x00;
	m_ResComponent.OutputsCtrlState->WatchDogState = WatchDogState;

	return TRUE;
}

/******************************************************************************
* Function: SyncItuToOGR(VOID)
*
* Describe:
*
* Input   :
*
* Output  :
*******************************************************************************/
BOOL CResourceMngr::SyncItuToOGR(VOID)
{
	if (Interlocks.BgmInterlock == 0 && \
		Interlocks.NGRInterlock == 0 && \
		Interlocks.BsmInterlock == 0 && \
		Interlocks.IgsInterlock == 0 && \
		Unreadys.BgmUnready == 0 && \
		Unreadys.NGRUnready == 0 && \
		Unreadys.BsmUnready == 0 && \
		Unreadys.IgsUnready == 0 && \
		Terminates.NGRTerminate == 0)
	{
		m_ResComponent.OutputsOGR->IUT = 0;
	}
	else
	{
		m_ResComponent.OutputsOGR->IUT = 1;
	}

	return TRUE;
}

/******************************************************************************
* Function: CheckHardwareState(VOID)
*
* Describe:
*
* Input   :
*
* Output  :
*******************************************************************************/
BOOL CResourceMngr::CheckHardwareState(VOID)
{
	//Check Gui communication state
	if (m_GuiComCheckTimer.IsTimeOut(m_CurrentSysTime))
	{
		if (TRUE == m_ResComponent.InputsFsm->com_status_gui)
		{
			m_GuiComOk = TRUE; //set gui communication interlock
		}
		else
		{
			m_GuiComOk = FALSE;
		}

		m_GuiComCheckTimer.ResetTimer();
		m_GuiComCheckTimer.StartTimer(m_CurrentSysTime, GUI_COM_CHECK_INTERVAL);
	}
	else
	{
		//do nothing
	}

	//m_ResComponent.IUTMonitor->InterlockBGM.GunHeater = Interlocks.BgmInterlock & INTL_GUN_HEATER ? TRUE : FALSE;
	m_GuiComOk ? Interlocks.NGRInterlock &= ~INTL_GUI_COMM_ERROR : Interlocks.NGRInterlock |= INTL_GUI_COMM_ERROR;
	(m_ResComponent.FmDevices->CANOpen == CANOPEN_STATE) ? Interlocks.NGRInterlock &= ~INTL_CANOPEN_MASTER_COMM_ERROR : Interlocks.NGRInterlock |= INTL_CANOPEN_MASTER_COMM_ERROR;
	((m_ResComponent.FmDevices->BGMSlave & 0x08) == ETHERCAT_OP_STATE) ? Interlocks.NGRInterlock &= ~INTL_ETHERCAT_BGM_COMM_ERROR : Interlocks.NGRInterlock |= INTL_ETHERCAT_BGM_COMM_ERROR;
	((m_ResComponent.FmDevices->BSMSlave & 0x08) == ETHERCAT_OP_STATE) ? Interlocks.NGRInterlock &= ~INTL_ETHERCAT_BSM_COMM_ERROR : Interlocks.NGRInterlock |= INTL_ETHERCAT_BSM_COMM_ERROR;
	((m_ResComponent.FmDevices->IGSSlave & 0x08) == ETHERCAT_OP_STATE) ? Interlocks.NGRInterlock &= ~INTL_ETHERCAT_IGS_COMM_ERROR : Interlocks.NGRInterlock |= INTL_ETHERCAT_IGS_COMM_ERROR;

	return TRUE;

}

/******************************************************************************
* Function: CheckGuiCase(VOID)
*
* Describe:
*
* Input   :
*
* Output  :
*******************************************************************************/
VOID CResourceMngr::CheckGuiCase(VOID)
{
	if (PlanResource.PlanIsReady() && \
		BsmResource.PlanTransCompleted() && \
		BgmResource.PlanTransCompleted())
	{
		Unreadys.NGRUnready &= ~UNREADY_CASE;	//reset interlock
	}
	else
	{
		Unreadys.NGRUnready |= UNREADY_CASE;	//set interlock
	}

	return;
}
/******************************************************************************
* Function: CheckTcsReady(VOID)
*
* Describe :
*
* Input :
*
* Output :
*******************************************************************************/
BOOL CResourceMngr::CheckTcsReady(VOID)
{
	if (FsmResource.GuiCmd.TCSNotready)
	{
		Unreadys.NGRUnready |= TCS_NOTREADY;				//set  tcs not ready

		return TRUE;
	}
	else if (FsmResource.GuiCmd.TCSReady )
	{
		Unreadys.NGRUnready &= ~TCS_NOTREADY;				//reset  tcs not ready

		return TRUE;
	}
	else
	{
		return FALSE;
	}

}

/******************************************************************************
* Function: CheckParameterFile(VOID)
*
* Describe :
*
* Input :
*
* Output :
*******************************************************************************/
VOID CResourceMngr::CheckParameterFile(VOID)
{
	UINT32 unreadyTemp = 0;

	if (m_ResComponent.InputsFsm->ParaCrcStatus.ModulatorCrcOk && \
		m_ResComponent.InputsFsm->ParaCrcStatus.GunCrcOk && \
		m_ResComponent.InputsFsm->ParaCrcStatus.AfcCrcOk && \
		m_ResComponent.InputsFsm->ParaCrcStatus.AfcCtrlCrcOk && \
		m_ResComponent.InputsFsm->ParaCrcStatus.DoseCrcOk && \
		m_ResComponent.InputsFsm->ParaCrcStatus.PrfCrcOk && \
		m_ResComponent.InputsFsm->ParaCrcStatus.EmulatorCrcOk && \
		m_ResComponent.InputsFsm->ParaCrcStatus.BsmCrcOk && \
		m_ResComponent.InputsFsm->ParaCrcStatus.IgsCrcOk)

		Unreadys.NGRUnready &= ~(static_cast<UINT8>(PARAMETER_NOT_READY));
	else
		Unreadys.NGRUnready |= static_cast<UINT8>(PARAMETER_NOT_READY);
}

/******************************************************************************
* Function: OutputToMonitor(VOID)
*
* Describe:
*
* Input   :
*
* Output  :
*******************************************************************************/
BOOL CResourceMngr::OutputToMonitor(VOID)
{
	//Bgm interlock
	m_ResComponent.IUTMonitor->InterlockBGM.MachineParameter	= Interlocks.BgmInterlock & INTL_MACHINE_PARA ? TRUE : FALSE;
	m_ResComponent.IUTMonitor->InterlockBGM.ModStanby			= Interlocks.BgmInterlock & INTL_MOD_STANDBY ? TRUE : FALSE;
	m_ResComponent.IUTMonitor->InterlockBGM.ModHV				= Interlocks.BgmInterlock & INTL_MOD_HV ? TRUE : FALSE;
	m_ResComponent.IUTMonitor->InterlockBGM.ModTrig				= Interlocks.BgmInterlock & INTL_MOD_TRIG ? TRUE : FALSE;
	m_ResComponent.IUTMonitor->InterlockBGM.ModCom				= Interlocks.BgmInterlock & INTL_COM_MOD ? TRUE : FALSE;
	m_ResComponent.IUTMonitor->InterlockBGM.CoolingIntl1		= Interlocks.BgmInterlock & INTL_COOLING_INTL1 ? TRUE : FALSE;
	m_ResComponent.IUTMonitor->InterlockBGM.CoolingIntl2		= Interlocks.BgmInterlock & INTL_COOLING_INTL2 ? TRUE : FALSE;
	m_ResComponent.IUTMonitor->InterlockBGM.WaterFlow			= Interlocks.BgmInterlock & INTL_WATER_FLOW ? TRUE : FALSE;
	m_ResComponent.IUTMonitor->InterlockBGM.CoolingCom			= Interlocks.BgmInterlock & INTL_COM_WATER ? TRUE : FALSE;
	m_ResComponent.IUTMonitor->InterlockBGM.GunHeater			= Interlocks.BgmInterlock & INTL_GUN_HEATER ? TRUE : FALSE;
	m_ResComponent.IUTMonitor->InterlockBGM.GunHV				= Interlocks.BgmInterlock & INTL_GUN_HV ? TRUE : FALSE;
	m_ResComponent.IUTMonitor->InterlockBGM.GunTrig				= Interlocks.BgmInterlock & INTL_GUN_TRIG ? TRUE : FALSE;
	m_ResComponent.IUTMonitor->InterlockBGM.Vac					= Interlocks.BgmInterlock & INTL_VAC ? TRUE : FALSE;
	m_ResComponent.IUTMonitor->InterlockBGM.GasLow				= Interlocks.BgmInterlock & INTL_GAS_LOW ? TRUE : FALSE;
	m_ResComponent.IUTMonitor->InterlockBGM.GasHigh				= Interlocks.BgmInterlock & INTL_GAS_HIGH ? TRUE : FALSE;
	m_ResComponent.IUTMonitor->InterlockBGM.RadiationTime		= Interlocks.BgmInterlock & INTL_RADIATION_TIMER ? TRUE : FALSE;
	m_ResComponent.IUTMonitor->InterlockBGM.BgmHV				= Interlocks.BgmInterlock & INTL_BGM_HV ? TRUE : FALSE;
	m_ResComponent.IUTMonitor->InterlockBGM.HvContactorError	= Interlocks.BgmInterlock & INTL_HV_CONTACTOR_ERROR ? TRUE : FALSE;
	m_ResComponent.IUTMonitor->InterlockBGM.ControllerOk		= Interlocks.BgmInterlock & INTL_CONTROLLER_OK ? TRUE : FALSE;
	m_ResComponent.IUTMonitor->InterlockBGM.SlipringHV			= Interlocks.BgmInterlock & INTL_SLIPRING_HV ? TRUE : FALSE;
	m_ResComponent.IUTMonitor->InterlockBGM.SlipringTreatEN		= Interlocks.BgmInterlock & INTL_SLIPRING_TREAT_EN ? TRUE : FALSE;
	m_ResComponent.IUTMonitor->InterlockBGM.Dose1WriteTimeOut	= Interlocks.BgmInterlock & INTL_DOSE1_WR_TM ? TRUE : FALSE;
	m_ResComponent.IUTMonitor->InterlockBGM.Dose1Symmetry		= Interlocks.BgmInterlock & INTL_DOSE1_SYMMETRY ? TRUE : FALSE;
	m_ResComponent.IUTMonitor->InterlockBGM.Dose1EtherCAT		= Interlocks.BgmInterlock & INTL_DOSE1_EtherCAT ? TRUE : FALSE;
	m_ResComponent.IUTMonitor->InterlockBGM.Dose1DarkExceed		= Interlocks.BgmInterlock & INTL_DOSE1_DK_EXD ? TRUE : FALSE;
	m_ResComponent.IUTMonitor->InterlockBGM.Dose1TotalExceed	= Interlocks.BgmInterlock & INTL_DOSE1_TOTAL_EXD ? TRUE : FALSE;
	m_ResComponent.IUTMonitor->InterlockBGM.Dose2WriteTimeOut	= Interlocks.BgmInterlock & INTL_DOSE2_WR_TM ? TRUE : FALSE;
	m_ResComponent.IUTMonitor->InterlockBGM.Dose2Symmetry		= Interlocks.BgmInterlock & INTL_DOSE2_SYMMETRY ? TRUE : FALSE;
	m_ResComponent.IUTMonitor->InterlockBGM.Dose2EtherCAT		= Interlocks.BgmInterlock & INTL_DOSE2_EtherCAT ? TRUE : FALSE;
	m_ResComponent.IUTMonitor->InterlockBGM.Dose2DarkExceed		= Interlocks.BgmInterlock & INTL_DOSE2_DK_EXD ? TRUE : FALSE;
	m_ResComponent.IUTMonitor->InterlockBGM.Dose2TotalExceed	= Interlocks.BgmInterlock & INTL_DOSE2_TOTAL_EXD ? TRUE : FALSE;
	m_ResComponent.IUTMonitor->InterlockBGM.DoseDualChannelExceed = Interlocks.BgmInterlock & INTL_DOSE_DUAL_CH_EXD ? TRUE : FALSE;
	m_ResComponent.IUTMonitor->InterlockBGM.DoseInformation		= Interlocks.BgmInterlock & INTL_INFORMATION ? TRUE : FALSE;
	m_ResComponent.IUTMonitor->InterlockBGM.DoseDummyTimeOut	= Interlocks.BgmInterlock & INTL_DOSE_DUMMY_TIME_OUT ? TRUE : FALSE;
	m_ResComponent.IUTMonitor->InterlockBGM.DoseHardware		= Interlocks.BgmInterlock & INTL_DOSE_HARDWARE ? TRUE : FALSE;

	//Bgm unready
	m_ResComponent.IUTMonitor->UnreadyBGM.ModulatorUnready		= Unreadys.BgmUnready & BGM_UNREADY_MODULATOR ? TRUE : FALSE;
	m_ResComponent.IUTMonitor->UnreadyBGM.GunUnready			= Unreadys.BgmUnready & BGM_UNREADY_GUN ? TRUE : FALSE;
	m_ResComponent.IUTMonitor->UnreadyBGM.DoseBoardUnready		= Unreadys.BgmUnready & BGM_UNREADY_DOSE ? TRUE : FALSE;

	//NGR interlock
	//m_ResComponent.IUTMonitor->InterlockNGR.SlaveNotSync		= Interlocks.NGRInterlock & INTL_SLAVE_NOT_SYNC ? TRUE : FALSE;
	//m_ResComponent.IUTMonitor->InterlockNGR.OgrNotSync			= Interlocks.NGRInterlock & INTL_OGR_NOT_SYNC ? TRUE : FALSE;

	//NGR unready
	m_ResComponent.IUTMonitor->UnreadyNGR.Case = Unreadys.NGRUnready & UNREADY_CASE ? TRUE : FALSE;

	return TRUE;
}

/******************************************************************************
* Function: SetCurrentSysTime(LONGLONG Time)
*
* Describe:
*
* Input   :
*
* Output  :
*******************************************************************************/
VOID  CResourceMngr::SetCurrentSysTime(LONGLONG Timer)
{
	m_CurrentSysTime = Timer;
}
/******************************************************************************
* Function: GetCurrentSysTime( )
*
* Describe:
*
* Input   :
*
* Output  :
*
* Return  :
*
* Others  :
*******************************************************************************/
LONGLONG CResourceMngr::GetCurrentSysTime(VOID )
{
	return m_CurrentSysTime ;
}

/******************************************************************************
* Function: SetComResource(ComponentRes_t Resource)
*
* Describe:
*
* Input   :
*
* Output  : BOOL
*******************************************************************************/
BOOL CResourceMngr::SetResourceCom(ResComponent_t Resource)
{
	m_ResComponent = Resource;

	return TRUE;
}

/******************************************************************************
* Function: GetComResource(VOID)
*
* Describe:
*
* Input   :
*
* Output  : BOOL
*******************************************************************************/
ResComponent_t* CResourceMngr::GetResourceCom(VOID)
{
	return &m_ResComponent;
}

/******************************************************************************
* Function: GOutputRadiationIndexToGUI(VOID)
*
* Describe:
*
* Input   :
*
* Output  : BOOL
*******************************************************************************/
VOID CResourceMngr::OutputRadiationIndexToGUI()
{
	//Output radiationIndex to GUI
	ServoType_eu ServoAxisType = PlanResource.GetServoAxisType();
	if (ServoAxisType == Dose)
	{
		m_ResComponent.OutputsBeam->RadiationIndex = m_ResComponent.InputsBeam->RadiationIndex;
	}
	else if (ServoAxisType == Gantry)
	{
		m_ResComponent.OutputsBeam->RadiationIndex = m_ResComponent.InputsOGR->RadiationIndex;
	}
	else
	{
		 // do nothing
	}
}

/******************************************************************************
* Function: CResourceMngr::Initialize(VOID)
*
* Describe:
*
* Input   :
*
* Output  : BOOL
*******************************************************************************/
VOID CResourceMngr::Initialize(VOID)
{
	if (m_InitEn)
	{
		m_GuiComCheckTimer.ResetTimer();
		m_GuiComCheckTimer.StartTimer(m_CurrentSysTime, GUI_COM_CHECK_INTERVAL);
		m_InitEn = FALSE;
	}
	else
	{
		//do nothing
	}

	return;
}

/******************************************************************************
* Function: UpdateInputTerminates(VOID)
*
* Describe:
*
* Input   :
*
* Output  : BOOL
*******************************************************************************/
VOID CResourceMngr::UpdateInputTerminates(VOID)
{
	BOOL HasTerminateFlag = FALSE;

	//Check radiation timer
	if (FsmResource.RadiationIsTimeOut())
	{
		Interlocks.BgmInterlock |= INTL_RADIATION_TIMER; //set beam timer interlock
		HasTerminateFlag = TRUE;
	}
	else
		Interlocks.BgmInterlock &= ~INTL_RADIATION_TIMER;

	//Check Bgm terminate
	Interlocks.BgmInterlock & INTL_DOSE_INTER1		? HasTerminateFlag = TRUE : NULL;
	Interlocks.BgmInterlock & INTL_DOSE_INTER6		? HasTerminateFlag = TRUE : NULL;
	Interlocks.BgmInterlock & INTL_DOSE_INTER10		? HasTerminateFlag = TRUE : NULL;

	//Check Bsm terminate
	Unreadys.BsmUnready & BSM_UNREADY_JAWX_OOT ? HasTerminateFlag = TRUE : NULL;	//JAWY_OUT_OF_TOLERANCE
	Unreadys.BsmUnready & BSM_UNREADY_LEAF_OOT ? HasTerminateFlag = TRUE : NULL;	//LEAF_OUT_OF_TOLERANCE
	Unreadys.BsmUnready & BSM_UNREADY_HEAD_OOT ? HasTerminateFlag = TRUE : NULL;	//HEAD_OUT_OF_TOLRANCE

	if (HasTerminateFlag)
		Terminates.NGRTerminate = 1;
	else
		Terminates.NGRTerminate = 0;
	
	return;
}