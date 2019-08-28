/*******************************************************************************
** File	      : Bsm.cpp
** Author:    : XuYun
** Version    :	1.0
** Environment: VS2013
** Description:
** History    :	18/12/04    XuYun    Created
*******************************************************************************/
/* INCLUDE ===================================================================*/
#include "Bsm.h"

/* METHODS===================================================================*/
/******************************************************************************
* Function: CBgm(CResourceMngr& ResourceMngr)
*
* Describe: construct function
*
* Input   : None
*
* Output  : None
*******************************************************************************/
CBsm::CBsm(CResourceMngr* ResourceMngr)
{
	m_ResourceMngr = ResourceMngr;
	m_PlanState = BSM_TRANS_STATE_IDLE;
	memset(&m_TransControl, 0, sizeof(m_TransControl));
	memset(&m_TransState, 0, sizeof(m_TransState));
	memset(&m_BsmData, 0, sizeof(m_BsmData));
	memset(m_PositionFeedBack, 0,sizeof(m_PositionFeedBack));
	//memset(&m_DebugControl, 0, sizeof(m_DebugControl));		
	m_ResComponent = ResourceMngr->GetResourceCom();
	m_OriginalCpPtr = NULL;
	m_BeamPlanPtr = NULL;
	memset(m_Original, 0, sizeof(m_Original));
	memset(m_DeltaTimeCp, 0, sizeof(m_DeltaTimeCp));	
	m_CurrentState = BSM_NO_STATE;
	m_RequiredState = BSM_NO_STATE;
	m_LastState = BSM_NO_STATE;	
	m_RadiationIndexFrBsm = 0;
	m_Interlock = 0;
	m_Unready = 0;
	m_CommandToBsm = 0;
	m_TotalTreatmentTime = 0;	
}

CBsm::~CBsm()
{
}

/******************************************************************************
* Function: CycleUpdate(VOID)
*
* Describe:
*
* Input   : None
*
* Output  : None
*******************************************************************************/
VOID CBsm::CycleUpdate(VOID)
{	
	Initialize();

	UpdateInputResource();

	ExecuteBusiness();

	UpdateOutputResource();
	
}

/******************************************************************************
* Function: RunIdleState(VOID)
*
* Describe:
*
* Input   : None
*
* Output  : BOOL
*******************************************************************************/
BsmPlanState_eu CBsm::RunIdleState(VOID)
{

	m_ResourceMngr->BsmResource.SetPlanTransCompleted(FALSE);

	SetBusIdle();		//set data bus to idle state	
	if ((m_TransState.RetransCounter-1) == BSM_MAX_RESEND_TIME)
	{
		m_TransState.SendFailed = TRUE;		
		return BSM_TRANS_STATE_IDLE;		//hold on idle state
	}
	if (InputBusIsIdle())
	{
		if (m_ResourceMngr->PlanResource.PlanIsReady() || m_TransState.RetransCounter != 0)
		{
			GetPlanData();			
	
			return BSM_TRANS_STATE_SEND;
		}
		else
		{
			return BSM_TRANS_STATE_IDLE;		//hold on idle state
		}
	}
	else
	{
	return BSM_TRANS_STATE_IDLE;		//hold on idle state
	}
}

/******************************************************************************
* Function: SetBusIdle(VOID)
*
* Describe:
*
* Input   : None
*
* Output  : BOOL
*******************************************************************************/
VOID CBsm::SetBusIdle(VOID)
{
	m_TransControl.BankIndex = 0;	
	m_ResComponent->OutputsBsm->BankIndex = m_TransControl.BankIndex;	
	m_ResComponent->OutputsBsm->CPIndex = 0;
	m_ResComponent->OutputsBsm->CPTotal = 0;
	m_ResComponent->OutputsBsm->Crc32 = 0;
	m_ResComponent->OutputsBsm->ToleranceJawX = 0;
	m_ResComponent->OutputsBsm->ToleranceJawY = 0;
	m_ResComponent->OutputsBsm->ToleranceLeaf = 0;

	return;
}

/******************************************************************************
* Function: InputBusIsIdle(VOID)
*
* Describe:
*
* Input   : None
*
* Output  : BOOL
*******************************************************************************/
BOOL CBsm::InputBusIsIdle(VOID)
{
	if (m_ResComponent->InputsBsm->BankIndex == BSM_DATA_BUS_IDLE)
		return TRUE;
	else
		return FALSE;
}

/******************************************************************************
* Function: GetPlanData(VOID)
*
* Describe:
*
* Input   : None
*
* Output  : BOOL
*******************************************************************************/
VOID CBsm::GetPlanData(VOID)
{
	//Get CP total
	m_BsmData.CPTotal = m_ResourceMngr->PlanResource.GetPlanCPTotal();	
	
	//checking Cp total availablity
	if (m_BsmData.CPTotal > MAX_CP_TOTAL)
		#if RTC_DEBUG_EXCEPTION
			m_ResComponent->Trace->Log(tlInfo, "Line:%d::CBsm::<LoadResource>::unavailable total cp number::%d", __LINE__);
		#endif
	else;		
	
	//get Original plan data pointer,and save MLC plan data
	m_OriginalCpPtr = m_ResourceMngr->PlanResource.GetOriginalPlan();
	
	for (UINT32 i = 0; i < m_BsmData.CPTotal; i++)
	{
		m_Original[i] = m_OriginalCpPtr[i];
		for (UINT32 j = 0; j < (MLC_AXIS_NUM-1); j++)
		{			
			m_BsmData.PlanData[i][j] = static_cast<INT32>(m_Original[i].MlcPos[j]*1000);	//transform Mlc Position to 0.01mm				
		}		
		m_BsmData.PlanData[i][124] = static_cast<INT32>(m_Original[i].MlcAngle * 100);	//Collimator
		m_BsmData.PlanData[i][127] = m_Original[i].RadiationIndex;
	}
	
	//Get delta time of Cp
	FLOAT* DeltaTimeCp = m_ResourceMngr->PlanResource.GetDeltaTimeCp();
	m_TotalTreatmentTime = 0;
	//get total treatment time
	for (UINT16 i = 0; i < (m_BsmData.CPTotal - 1); i++)
	{
		m_DeltaTimeCp[i] = DeltaTimeCp[i];
		m_TotalTreatmentTime += m_DeltaTimeCp[i];					
	}
	m_BsmData.TotalTreatmentTime = static_cast<INT32>(m_TotalTreatmentTime * 1000);//unit:ms
	m_BsmData.PlanData[0][126] = m_BsmData.TotalTreatmentTime;

	//Get Tolerance
	m_BeamPlanPtr= m_ResourceMngr->PlanResource.GetBeamPlanData();
	m_BsmData.LeafTolerance = static_cast<UINT16> (m_BeamPlanPtr->MlcPositionTolerance * 100);//uint:cm*1000,from GUI uint:mm
	m_BsmData.JawXTolerance = static_cast<UINT16>(m_BeamPlanPtr->AsymxPositionTolerance * 100);
	m_BsmData.JawYTolerance = static_cast<UINT16>(m_BeamPlanPtr->AsymyPositionTolerance * 100);
	m_BsmData.HeadTolerance = static_cast<UINT16>(m_BeamPlanPtr->BeamLimitingAngleTolerance * 100);


	//Add transition beam number
	m_TransControl.BeamID++;	//add transition number		
	m_TransControl.BeamID = (m_TransControl.BeamID == 65535 ? 1 : m_TransControl.BeamID);

	//Get crc32 value	
	//m_TransControl.Crc32 = m_ResourceMngr->ToolCrc32.Crc32Buffer((UCHAR*)(m_BsmData.PlanData), sizeof(m_BsmData.PlanData)); //for crc error test	
	m_TransControl.Crc32 = m_ResourceMngr->ToolCrc32.Crc32Buffer((UCHAR*)(m_BsmData.PlanData), m_BsmData.CPTotal*MLC_AXIS_NUM * sizeof(INT32));
	
		
	return;
}

/******************************************************************************
* Function: RunSendState(VOID)
*
* Describe:
*
* Input   : None
*
* Output  : BOOL
*******************************************************************************/
BsmPlanState_eu CBsm::RunSendState(VOID)
{
	m_ResourceMngr->BsmResource.SetPlanTransCompleted(FALSE);	

	SendPlanToBSM();

	if (TransHasError())
	{
		m_TransState.RetransCounter++;		//record retransition number of times
		m_AckTimerBank1.ResetTimer();			//reset check timer	
		m_AckTimerBank2.ResetTimer();			//reset check timer			

		return BSM_TRANS_STATE_ERROR;			//go to error state
	}
	else if (m_ResourceMngr->FsmResource.GuiCmd.ClosePatient || !m_ResourceMngr->PlanResource.PlanIsReady() || m_ResourceMngr->FsmResource.GuiCmd.CleanPlan)	//close patient or new plan
	{
		return BSM_TRANS_STATE_ERROR;			//go to error state
	}
	else if (m_TransState.PlanSendFinish)
	{
		m_TransState.RetransCounter = 0;
		return BSM_TRANS_STATE_FINISH;			//go to finish state
	}
	else
	{
		return BSM_TRANS_STATE_SEND;			//hold on send state
	}
}

/******************************************************************************
* Function: SendPlanToBSM(VOID)
*
* Describe:
*
* Input   : None
*
* Output  : None
*******************************************************************************/
VOID CBsm::SendPlanToBSM(VOID)
{
	if (m_BsmData.CPTotal == m_TransControl.CPIndex)
	{
		#if BSM_DEBUG_TRACE
			m_ResComponent->Trace->Log(tlInfo, "Line:%d::<send_one_case>::end send case", __LINE__);
		#endif
		m_TransState.PlanSendFinish = TRUE;		//plan data send completed
	}
	else if (FALSE==SendOneCpDataToBSM(m_TransControl.CPIndex))
	{
		m_TransState.PlanSendFinish = FALSE;	//continue send	
	}
	else
	{
		m_TransControl.CPIndex++;	//next cp
		m_TransState.PlanSendFinish = FALSE;	//continue send	
	}

}

/******************************************************************************
* Function: SendOneCpDataToBSM(VOID)
*
* Describe:
*
* Input   : None
*
* Output  : None
*******************************************************************************/
BOOL CBsm::SendOneCpDataToBSM(UINT16 CpIndex)
{
	//Checking timeout
	if (m_AckTimerBank1.IsTimeOut(m_ResourceMngr->GetCurrentSysTime()) || m_AckTimerBank2.IsTimeOut(m_ResourceMngr->GetCurrentSysTime()))
	{				
		m_TransState.AckTimeOut = TRUE;		//resend plan data	
	
		#if BSM_DEBUG_TRACE
			m_ResComponent->Trace->Log(tlInfo, "Line:%d::CBsm::<SendOneCP>::start cp %d   ack TIMEOUT %d", __LINE__, m_TransControl.CPIndex, m_TransState.AckTimeOut);
		#endif
	
		return FALSE;
	}
	else ;	
	
	//for BeamId error test
	//if (CpIndex == 1)
	//{
	//	m_TransControl.BeamID++;
	//}		

	/*transmit bank1 value*/
	if (InputBusIsIdle() && OutputBusIsIdle())
	{	
		//output bank1 value to ethercat	
		SendFirstBankDataToBSM(CpIndex);

		#if BSM_DEBUG_TRACE
			m_ResComponent->Trace->Log(tlInfo, "Line:%d::CBsm::<SendOneCP>::send cp %d bank1", __LINE__, m_TransControl.CPIndex);
		#endif	
		
		//start AckTimer of Bank1
		if (m_AckTimerBank1.IsReady())	
		{
			m_AckTimerBank1.StartTimer(m_ResourceMngr->GetCurrentSysTime(), BSM_ACK_INTERVAL);			
		}
		else ;

		return FALSE;	//wait bsm response
	}
	else if (RecvAckOk(BSM_BANK1_REQ))	//received same cp bank1 response
	{
		//reset AckTimer of Bank1, for having received Bank1 BankIndexAck
		m_AckTimerBank1.ResetTimer();

		//output bank2 value to ethercat
		SendSecondBankDataToBSM(CpIndex);

		#if BSM_DEBUG_TRACE
			m_ResComponent->Trace->Log(tlInfo, "Line:%d::<SendOneCP>::send cp %d bank2", __LINE__, m_TransControl.CPIndex);
		#endif	
		
		//start AckTimer of Bank2
		if (m_AckTimerBank2.IsReady())	
		{
			m_AckTimerBank2.StartTimer(m_ResourceMngr->GetCurrentSysTime(), BSM_ACK_INTERVAL);			
		}
		else ;	

		return FALSE;	//wait bsm response
	}
	else if (RecvAckOk(BSM_BANK2_REQ))	//received same cp bank2 response
	{
		//reset AckTimer of Bank2, for having received Bank2 BankIndexAck
		m_AckTimerBank2.ResetTimer();

		//change bank to end transmit
		SetBusIdle();

		#if BSM_DEBUG_TRACE
			m_ResComponent->Trace->Log(tlInfo, "Line:%d::<SendOneCP>::send %d cp completed", __LINE__, m_TransControl.CPIndex);
		#endif	

		return TRUE;	//one cp transmit completed
	}
	else
	{
		return FALSE;
	}

}

/******************************************************************************
* Function: OutputBusIsIdle(VOID)
*
* Describe:
*
* Input   : None
*
* Output  : BOOL
*******************************************************************************/
BOOL CBsm::OutputBusIsIdle(VOID)
{
	if (m_TransControl.BankIndex == BSM_DATA_BUS_IDLE)
		return TRUE;
	else
		return FALSE;
}

/******************************************************************************
* Function: LoadDataToBus(VOID)
*
* Describe:
*
* Input   : None
*
* Output  : None
*******************************************************************************/
VOID CBsm::LoadDataToBus(UINT16 CpIndex,UINT16 BankIndex)
{	
	//output bank1 value to bsm
	if (BSM_BANK1_REQ == BankIndex)
	{
		for (UINT16 i = 0; i < BSM_DATA_BUS_DATA_LEN; i++)
		{
			m_ResComponent->OutputsBsm->Data[i] = m_BsmData.PlanData[CpIndex][i];	//output bank1
		}
	}
	//output bank2 value to bsm
	else if (BSM_BANK2_REQ == BankIndex)
	{
		for (UINT16 i = 0; i < BSM_DATA_BUS_DATA_LEN; i++)
		{
			m_ResComponent->OutputsBsm->Data[i] = m_BsmData.PlanData[CpIndex][i+64];	//output bank1
		}
	}
	
	//Output Jaw1,Jaw2 tolerance and MLC tolerance
	m_ResComponent->OutputsBsm->ToleranceJawX = m_BsmData.JawXTolerance;
	m_ResComponent->OutputsBsm->ToleranceJawY = m_BsmData.JawYTolerance;
	m_ResComponent->OutputsBsm->ToleranceLeaf = m_BsmData.LeafTolerance;
	m_ResComponent->OutputsBsm->HeadTolerance = m_BsmData.HeadTolerance;

	m_ResComponent->OutputsBsm->CPTotal = m_BsmData.CPTotal;	
	//m_ResComponent->Trace->Log(tlInfo, "CBsm::<RunSendState>::Outputs.CpTotal=%d", m_ResComponent->OutputsBsm->CPTotal);
	return;
}

/******************************************************************************
* Function: SendFirstBankDataToBSM(VOID)
*
* Describe:
*
* Input   : None
*
* Output  : None
*******************************************************************************/
BOOL CBsm::SendFirstBankDataToBSM(UINT16 CpIndex)
{
	m_TransControl.BankIndex = BSM_BANK1_REQ;

	//load data to EtherCAT bus
	LoadDataToBus(CpIndex,BSM_BANK1_REQ);

	//lod control data to EtherCAT bus
	LoadControlToBus();	

	return TRUE;

}
/******************************************************************************
* Function: SendSecondBankDataToBSM(VOID)
*
* Describe:
*
* Input   : None
*
* Output  : None
*******************************************************************************/
BOOL CBsm::SendSecondBankDataToBSM(UINT16 CpIndex)
{
	m_TransControl.BankIndex = BSM_BANK2_REQ;

	//load data to  EtherCAT bus
	LoadDataToBus(CpIndex,BSM_BANK2_REQ);

	//load control data to EtherCAT bus
	LoadControlToBus();

	return TRUE;

}

/******************************************************************************
* Function: LoadControlToBus(VOID)
*
* Describe:
*
* Input   : None
*
* Output  : None
*******************************************************************************/
VOID CBsm::LoadControlToBus(VOID)
{
	m_ResComponent->OutputsBsm->BeamID = m_TransControl.BeamID;
	m_ResComponent->OutputsBsm->CPIndex = m_TransControl.CPIndex;	
	m_ResComponent->OutputsBsm->BankIndex = m_TransControl.BankIndex ;			//counter start with 0

	//output whole case crc32 value to bsm(last cp used it)
	m_ResComponent->OutputsBsm->Crc32 = m_TransControl.Crc32;

	return;
}

/******************************************************************************
* Function: RecvAckOk(UINT16 BankIndex)
*
* Describe:
*
* Input   : None
*
* Output  : None
*******************************************************************************/
BOOL CBsm::RecvAckOk(UINT16 BankIndex)
{
	//check if receiving bank1 data
	if (BankIndex == BSM_BANK1_REQ)
	{
		if ((BSM_BANK1_RSP == m_ResComponent->InputsBsm->BankIndex) && (m_ResComponent->InputsBsm->CPIndex == m_TransControl.CPIndex))
		{
			return TRUE;
		}
		else
		{
			return FALSE;
		}
	}
	//check if receiving bank2 data
	else if ((BankIndex == BSM_BANK2_REQ) && (m_ResComponent->InputsBsm->CPIndex == m_TransControl.CPIndex))
	{
		if ((BSM_BANK2_RSP == m_ResComponent->InputsBsm->BankIndex) /*&& (m_ResComponent->InputsBsm->CPIndex == m_TransControl.CPIndex)*/)
		{
			return TRUE;
		}
		else
		{
			return FALSE;
		}
	}	
	else
	{
		return FALSE;
	}
}
/******************************************************************************
* Function: TransitionIsError(VOID)
*
* Describe:
*
* Input   : None
*
* Output  : None
*******************************************************************************/
VOID	CBsm::Initialize(VOID)
{

	return;
}
/******************************************************************************
* Function: TransitionIsError(VOID)
*
* Describe:
*
* Input   : None
*
* Output  : None
*******************************************************************************/
VOID	CBsm::ExecuteBusiness(VOID)
{
	GetInterlock();
	SetState();
	GetState();
	RunState();

	GetRTFiedData();

	return;
}
/******************************************************************************
* Function: TransitionIsError(VOID)
*
* Describe:
*
* Input   : None
*
* Output  : None
*******************************************************************************/
BOOL	CBsm::UpdateInputResource(VOID) //update compoent outputs
{
	
	return TRUE;
}
/******************************************************************************
* Function: TransitionIsError(VOID)
*
* Describe:
*
* Input   : None
*
* Output  : None
*******************************************************************************/
BOOL	CBsm::UpdateOutputResource(VOID)
{
	m_ResComponent->OutputsBsm->CollimatorDegree = m_ResComponent->InputsBsm->PosFeedback[124];
	m_ResComponent->OutputsBsm->HHCWheel1 = m_ResComponent->InputsBsm->WheelBsm1;
	m_ResComponent->OutputsBsm->HHCWheel2 = m_ResComponent->InputsBsm->WheelBsm2;
	m_ResComponent->OutputsBsm->HHCWheel3 = m_ResComponent->InputsBsm->WheelBsm3;
	return TRUE;

}

/******************************************************************************
* Function: TransitionIsError(VOID)
*
* Describe:
*
* Input   : None
*
* Output  : None
*******************************************************************************/
BOOL CBsm::TransHasError(VOID)
{
	if (m_ResComponent->InputsBsm->BankIndex == BSM_ERROR_CRC ||
		m_ResComponent->InputsBsm->BankIndex == BSM_ERROR_BEAM_ID ||
		m_TransState.AckTimeOut)
	{
		#if BSM_DEBUG_TRACE
			m_ResComponent->Trace->Log(tlInfo, "CBsm::<RunSendState>::Has Error:BankIndex:%d,Ack Time out:%d", m_ResComponent->InputsBsm->BankIndex, m_TransState.AckTimeOut);
		#endif

		return TRUE;
	}
	else
		return FALSE;
}

/******************************************************************************
* Function: RunErrorState(VOID)
*
* Describe:
*
* Input   : None
*
* Output  : BOOL
*******************************************************************************/
BsmPlanState_eu CBsm::RunErrorState(VOID)
{
	m_ResourceMngr->BsmResource.SetPlanTransCompleted(FALSE);	//set transition not complete
	ResetCommunication();

	if (BusIsReset())
	{
		ClearPlan();

		#if BSM_DEBUG_TRACE
			m_ResComponent->Trace->Log(tlInfo, "Line::%d::CBsm::<RunErrorState>::retransmit %d starts", __LINE__, m_TransState.RetransCounter);
		#endif

		return BSM_TRANS_STATE_IDLE;								//go to idle state

	}
	else
		return BSM_TRANS_STATE_ERROR;								//hold on error state
}

/******************************************************************************
* Function: ResetCommunication(VOID)
*
* Describe:
*
* Input   : None
*
* Output  : None
*******************************************************************************/
VOID CBsm::ResetCommunication(VOID)
{	
	m_ResComponent->OutputsBsm->BeamID = 0;			//reset singal
	m_ResComponent->OutputsBsm->BankIndex = 0;
	m_ResComponent->OutputsBsm->CPIndex = 0;
	m_ResComponent->OutputsBsm->CPTotal = 0;
	m_ResComponent->OutputsBsm->Crc32 = 0;
	memset(&m_ResComponent->OutputsBsm->Data, 0, sizeof(m_ResComponent->OutputsBsm->Data));

	return;
}

/******************************************************************************
* Function: BusIsReset(VOID)
*
* Describe:
*
* Input   : None
*
* Output  : BOOL
*******************************************************************************/
BOOL CBsm::BusIsReset(VOID)
{
	if (m_ResComponent->InputsBsm->BeamID == BSM_DATA_BUS_FB_RESET)
		return TRUE;
	else
		return FALSE;
}

/******************************************************************************
* Function: ConstructDummyData(VOID)
*
* Describe:
*
* Input   : None
*
* Output  : BOOL
*******************************************************************************/
VOID CBsm::ClearPlan(VOID)
{
	//clear data buffer
	memset(&m_BsmData, 0, sizeof(m_BsmData));
	//clear control sigal
	m_TransControl.BankIndex = 0;
	m_TransControl.CPIndex = 0;
	m_TransControl.Crc32 = 0;	
	m_TransState.AckTimeOut = FALSE;
	m_TransState.PlanSendFinish = FALSE;	
}

/******************************************************************************
* Function: RunFinishState(VOID)
*
* Describe:
*
* Input   : None
*
* Output  : BOOL
*******************************************************************************/
BsmPlanState_eu CBsm::RunFinishState(VOID)
{
	m_ResourceMngr->BsmResource.SetPlanTransCompleted(TRUE);

	SetBusIdle();

	m_TransState.RetransCounter = 0;	//reset resend counter
	m_TransControl.CPIndex = 0;

	if (m_ResourceMngr->FsmResource.GuiCmd.ClosePatient || !m_ResourceMngr->PlanResource.PlanIsReady() || m_ResourceMngr->FsmResource.GuiCmd.CleanPlan)
	{				
		return BSM_TRANS_STATE_IDLE;
	}
	else
	{
		return BSM_TRANS_STATE_FINISH;			//hold on finish state
	}
}

/******************************************************************************
* Function:GetRTFiedData(VOID)
*
* Describe:
*
* Input   : None
*
* Output  : BOOL
*******************************************************************************/
VOID CBsm::GetRTFiedData(VOID)
{
	//update inputs and outputs
	UpdateInputsFmGUI();
	UpdateOutputsToGUI();

	GetPosFeedBackData();
}

/******************************************************************************
* Function:GetPosFeedBackData(VOID)
*
* Describe:
*
* Input   : None
*
* Output  : BOOL
*******************************************************************************/
VOID CBsm::GetPosFeedBackData(VOID)
{
	memcpy(m_PositionFeedBack, m_ResComponent->InputsBsm->PosFeedback, sizeof(m_PositionFeedBack));
	for (UINT16 i = 0; i < MLC_AXIS_NUM; i++)
	{
		if (i == 124)
		{
			m_ResComponent->OutputsBsmToGUI->MLCActPos[i] = static_cast<FLOAT>(m_PositionFeedBack[i] * 0.1);
		}
		else
		{
			m_ResComponent->OutputsBsmToGUI->MLCActPos[i] = static_cast<FLOAT>(m_PositionFeedBack[i] * 0.001);
		}
		
	}	
}

/******************************************************************************
* Function:GetRtcCurrentState(VOID)
*
* Describe:
*
* Input   : None
*
* Output  : BOOL
*******************************************************************************/
VOID CBsm::GetRtcCurrentState(VOID)
{
	m_RtcCurrentState = m_ResourceMngr->FsmResource.GetCurrentState();
}

/******************************************************************************
* Function:GetState(VOID)
*
* Describe:
*
* Input   : None
*
* Output  : BOOL
*******************************************************************************/
VOID CBsm::GetState(VOID)
{
	m_CurrentState = (BsmFsmState_u)(m_ResComponent->InputsBsm->CurrentState & 0x7F);	//get Bsm current state
	m_LastState = (BsmFsmState_u)(m_ResComponent->InputsBsm->LastState & 0x7F);			//get Bsm last state	

	//set Bsm init complete flag
	if (m_CurrentState == BSM_STANDBY)
		m_BsmInitCompleteFlag = TRUE;
	else
		m_BsmInitCompleteFlag = FALSE;

	m_ResourceMngr->BsmResource.SetBsmInitCompleteFlag(m_BsmInitCompleteFlag);
}

/******************************************************************************
* Function:SetState(VOID)
*
* Describe:
*
* Input   : None
*
* Output  : BOOL
*******************************************************************************/
VOID CBsm::SetState(VOID)
{	
	GetRtcCurrentState();
	ClearResetCmd();

	switch (m_RtcCurrentState)
	{
	case RTC_INIT:	
	{	
		//checking BSM received correct Init mode		
		if ((m_ResComponent->OutputsBsm->BSMInitMode == m_ResComponent->InputsBsm->BSMInitModeAck) && m_ResComponent->OutputsBsm->BSMInitMode != 0)
			JumpToNextState(BSM_INITIAL);
		else
			JumpToNextState(BSM_NO_STATE);

		break;
	}
	case RTC_STANDBY:
	{
		JumpToNextState(BSM_STANDBY);		
		break;
	}
	case RTC_PREPARATORY:
	{
		if (m_ResComponent->InputsBsm->BsmMotionEnable != 0)
		{
			JumpToNextState(BSM_MANUAL);
		}
		else
		{
			JumpToNextState(BSM_STANDBY);
		}
				
		//if HHC effective
		//JumpToNextState(BSM_MANUAL);
		break;
	}
	case RTC_PREPARE:
	{
		if (m_BeamPlanPtr->RadiationType == RadiationKv)
		{
			JumpToNextState(BSM_STANDBY);

		}
		else
		{
			JumpToNextState(BSM_PREPARE);
		}		
		break;
	}
	case RTC_READY:
	{
		if (m_BeamPlanPtr->RadiationType == RadiationKv)
		{
			JumpToNextState(BSM_STANDBY);

		}
		else
		{
			JumpToNextState(BSM_SERVO);
		}			
		break;
	}
	case RTC_RADIATION:
	{
		if (m_BeamPlanPtr->RadiationType == RadiationKv)
		{
			JumpToNextState(BSM_STANDBY);
		}
		else
		{
			JumpToNextState(BSM_SERVO);
		}		
		break;
	}
	case RTC_REPOSITION:
	{
		JumpToNextState(BSM_PREPARE);
		break;
	}
	case RTC_COMPLETE:
	{
		JumpToNextState(BSM_STANDBY);
		break;
	}
	case RTC_INTERRUPT:
	{
		JumpToNextState(BSM_SERVO);
		break;
	}
	case RTC_INTERRUPTREADY:
	{
		JumpToNextState(BSM_SERVO);
		break;
	}
	case RTC_TERMINATE:
	{
		JumpToNextState(BSM_STANDBY);
		break;
	}
	case RTC_OFF:
	{
		JumpToNextState(BSM_OFF);
		break;
	}
	default:
		;
		break;
	}	
		
	OutputRequiredState();	

	//state timeout check
	if (m_StateTimer.IsReady())
	{
		m_StateTimer.StartTimer(m_ResourceMngr->GetCurrentSysTime(), BSM_STATE_INTERVAL);
	}
	else if (m_StateTimer.IsTimeOut(m_ResourceMngr->GetCurrentSysTime()) && m_RequiredState != m_CurrentState)
	{
		m_Unready |= 0x0001;
	}	
	
	//get to required state ,then clear Bsm not ready information
	if (m_RequiredState == m_CurrentState)
	{
		m_Unready &= ~0x0001;
		m_StateTimer.ResetTimer();
	}
	
	m_ResourceMngr->BsmResource.SetUnready(m_Unready);

	return;
}

/******************************************************************************
* Function:GetInterlock(VOID)
*
* Describe:
*
* Input   : None
*
* Output  : BOOL
*******************************************************************************/
VOID CBsm::GetInterlock(VOID)
{
	//00000000
	//Get Interlock
	
	(m_ResComponent->InputsBsm->Interlock & BSM_INIT_FAILED) ? m_Interlock |= 0x0000000000000001 : m_Interlock &= ~0x0000000000000001;//

	(m_ResComponent->InputsBsm->Interlock & BSM_PARAMETER_CRC_ERROR) ? m_Interlock |= 0x0000000000000002 : m_Interlock &= ~0x0000000000000002;//

	(m_ResComponent->InputsBsm->Interlock & BSM_WATCHDOG_FAILED) ? m_Interlock |= 0x0000000000000004 : m_Interlock &= ~0x0000000000000004;//

	(m_ResComponent->InputsBsm->Interlock & BSM_MLC_COMM_FAILURE) ? m_Interlock |= 0x0000000000000008 : m_Interlock &= ~0x0000000000000008;//

	(m_ResComponent->InputsBsm->Interlock & BSM_MLC_SUBSYS_SYNC_FAILED) ? m_Interlock |= 0x0000000000000010 : m_Interlock &= ~0x0000000000000010;//

	(m_ResComponent->InputsBsm->Interlock & BSM_MLC_PCB_POWER_ERROR) ? m_Interlock |= 0x0000000000000020 : m_Interlock &= ~0x0000000000000020;//

	(m_ResComponent->InputsBsm->Interlock & BSM_MLC_PCB_FAN_ERROR) ? m_Interlock |= 0x0000000000000040 : m_Interlock &= ~0x0000000000000040;//

	(m_ResComponent->InputsBsm->Interlock & BSM_LEAF_DRIVER_ERROR) ? m_Interlock |= 0x0000000000000080 : m_Interlock &= ~0x0000000000000080;//

	(m_ResComponent->InputsBsm->Interlock & BSM_LEAF_LOCKED_ERROR) ? m_Interlock |= 0x0000000000000100 : m_Interlock &= ~0x0000000000000100;//

	(m_ResComponent->InputsBsm->Interlock & BSM_LEAF_Encoder_UNTRUSTED) ? m_Interlock |= 0x0000000000000200 : m_Interlock &= ~0x0000000000000200;//

	(m_ResComponent->InputsBsm->Interlock & BSM_CARRIER_DRIVE_ERROR) ? m_Interlock |= 0x0000000000000400 : m_Interlock &= ~0x0000000000000400;//

	(m_ResComponent->InputsBsm->Interlock & BSM_CARRIER_LOCKED_ROTOR) ? m_Interlock |= 0x0000000000000800 : m_Interlock &= ~0x0000000000000800;//

	(m_ResComponent->InputsBsm->Interlock & BSM_CARRIER_ENCODER_UNTRUSTED) ? m_Interlock |= 0x0000000000001000 : m_Interlock &= ~0x0000000000001000;//

	(m_ResComponent->InputsBsm->Interlock & BSM_JAWX_DERIVE_ERROR) ? m_Interlock |= 0x0000000000002000 : m_Interlock &= ~0x0000000000002000;//

	(m_ResComponent->InputsBsm->Interlock & BSM_JAWX_LOCKED_ROTOR) ? m_Interlock |= 0x0000000000004000 : m_Interlock &= ~0x0000000000004000;//

	(m_ResComponent->InputsBsm->Interlock & BSM_JAWX_ENCODER_UNTRUSTED) ? m_Interlock |= 0x0000000000008000 : m_Interlock &= ~0x0000000000008000;//

	(m_ResComponent->InputsBsm->Interlock & BSM_JAWY_DRIVE_ERROR) ? m_Interlock |= 0x0000000000010000 : m_Interlock &= ~0x0000000000010000;//

	(m_ResComponent->InputsBsm->Interlock & BSM_JAWYLOCKE_ROTOR) ? m_Interlock |= 0x0000000000020000 : m_Interlock &= ~0x0000000000020000;//

	(m_ResComponent->InputsBsm->Interlock & BSM_JAWY_ENCODEDR_UNTRUSTED) ? m_Interlock |= 0x0000000000040000 : m_Interlock &= ~0x0000000000040000;//

	(m_ResComponent->InputsBsm->Interlock & BSM_COLLIMATOR_POWER_ERROR) ? m_Interlock |= 0x0000000000080000 : m_Interlock &= ~0x0000000000080000;//

	(m_ResComponent->InputsBsm->Interlock & BSM_COLLIMATOR_DRIVE_ERROR) ? m_Interlock |= 0x0000000000100000 : m_Interlock &= ~0x0000000000100000;//

	(m_ResComponent->InputsBsm->Interlock & BSM_COLLIMATOR_LOCKED_ROTOR) ? m_Interlock |= 0x0000000000200000 : m_Interlock &= ~0x0000000000200000;//

	(m_ResComponent->InputsBsm->Interlock & BSM_COLLIMATOR_ENCODEDR_UNTRUSTED) ? m_Interlock |= 0x0000000000400000 : m_Interlock &= ~0x0000000000400000;//

	(m_ResComponent->InputsBsm->Interlock & BSM_PRIMARYCOLLIMATOR_OUT_OF_POSITION) ? m_Interlock |= 0x0000000100000000 : m_Interlock &= ~0x0000000100000000;//

	(m_ResComponent->InputsBsm->Interlock & BSM_PRIMARYCOLLIMATOR_MOTION_TIMEOUT) ? m_Interlock |= 0x0000000200000000 : m_Interlock &= ~0x0000000200000000;//

	(m_ResComponent->InputsBsm->Interlock & BSM_PRIMARYCOLLIMATOR_SIGNAL_ERROR) ? m_Interlock |= 0x0000000400000000 : m_Interlock &= ~0x0000000400000000;//

	//Get unready
	if (m_ResComponent->InputsBsm->Interlock & BSM_JAWX_OUT_OF_TOLERANCE)
	{
		m_Unready |= 0x0002;//bit1:JAWY_OUT_OF_TOLERANCE
	}
	else
	{
		m_Unready &= ~0x0002;
	}
	if (m_ResComponent->InputsBsm->Interlock & BSM_JAWY_OUT_OF_TOLERANCE)
	{
		m_Unready |= 0x0004;//bit2:MLC_OUT_OF_TOLERANCE
	}
	else
	{
		m_Unready &= ~0x0004;
	}

	if (m_ResComponent->InputsBsm->Interlock & BSM_LEAF_OUT_OF_TOLERANCE)
	{
		m_Unready |= 0x0008;//bit2:MLC_OUT_OF_TOLERANCE
	}
	else
	{
		m_Unready &= ~0x0008;
	}
	
	if (m_ResComponent->InputsBsm->Interlock & BSM_COLLIMATOR_OUT_OF_TOLERANCE)
	{
		m_Unready |= 0x0010;//bit3:HEAD_OUT_OF_TOLRANCE
	}
	else
	{
		m_Unready &= ~0x0010;
	}
	
	

	m_ResourceMngr->BsmResource.SetInterLock(m_Interlock);
	m_ResourceMngr->BsmResource.SetUnready(m_Unready);
}

/******************************************************************************
* Function:InitialMain(VOID)
*
* Describe:
*
* Input   : None
*
* Output  : BOOL
*******************************************************************************/
VOID CBsm::InitialMain(VOID)
{
	;
}

/******************************************************************************
* Function:StandbyMain(VOID)
*
* Describe:
*
* Input   : None
*
* Output  : BOOL
*******************************************************************************/
VOID CBsm::StandbyMain(VOID)
{	
	//send plan data
	if (SendPlanMain())		//sending succeed
	{
		
		//m_Interlock &=~BSM_ETHERCAT_COMM_FAILURE;
		//m_ResourceMngr->BsmResource.SetInterLock(m_Interlock);
	}
	else                   //sending failed after retransmitting 3 times
	{
		//RTC & BSM EtherCAT communication failure interlock
		//m_Interlock |= BSM_ETHERCAT_COMM_FAILURE;
		//m_ResourceMngr->BsmResource.SetInterLock(m_Interlock);

		//waiting reset cmd
		if (m_ResourceMngr->FsmResource.GuiCmd.Reset)
		{
			m_TransState.SendFailed = FALSE;
			m_TransState.RetransCounter = 0;
		}
	}

	//check Manual mode signal
	//if ()
	//{
	//	JumpToNextState(BSM_MANUAL);
	//}	
}

/******************************************************************************
* Function:StandbyMain(VOID)
*
* Describe:
*
* Input   : None
*
* Output  : BOOL
*******************************************************************************/
BOOL CBsm::SendPlanMain(VOID)
{
	//send plan data to BSM
	switch (m_PlanState)
	{
	case BSM_TRANS_STATE_IDLE:
		m_PlanState = RunIdleState();
		break;
	case BSM_TRANS_STATE_SEND:
		m_PlanState = RunSendState();
		break;
	case BSM_TRANS_STATE_ERROR:
		m_PlanState = RunErrorState();
		break;
	case BSM_TRANS_STATE_FINISH:
		m_PlanState = RunFinishState();
		break;
	default:
		break;
	}

	//return plan data sending result
	if (m_TransState.PlanSendFinish)
	{
		return m_TransState.PlanSendFinish;
	}
	else if (m_TransState.SendFailed)
	{
		return m_TransState.SendFailed;
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
BsmFsmState_u CBsm::JumpToNextState(BsmFsmState_u RequiredState)
{
	m_RequiredState = RequiredState;	
	return m_RequiredState;				
}

/******************************************************************************
* Function:PrepareMain(VOID)
*
* Describe:
*
* Input   : None
*
* Output  : BOOL
*******************************************************************************/
VOID CBsm::PrepareMain(VOID)
{	
	m_ResComponent->OutputsBsm->RadiationIndex = m_BsmData.PlanData[0][127];
}

/******************************************************************************
* Function:ServoMain(VOID)
*
* Describe:
*
* Input   : None
*
* Output  : BOOL
*******************************************************************************/
VOID CBsm::ServoMain(VOID)
{
	//send radiationIndex
	SendRadiationIndexToBSM();
	//get BSm current RadiationIndex
	GetRadiationIndexFrBSM();	
}

/******************************************************************************
* Function:SendRadiationIndex(VOID)
*
* Describe:
*
* Input   : None
*
* Output  : BOOL
*******************************************************************************/
VOID CBsm::SendRadiationIndexToBSM(VOID)
{	
	if (m_RtcCurrentState == RTC_READY)
	{
		//RTC_Ready: send RaditionIndex of the first control point to BSM
		m_ResComponent->OutputsBsm->RadiationIndex = m_BsmData.PlanData[0][127];		
	}
	else
	{
		m_ResComponent->OutputsBsm->RadiationIndex = m_ResComponent->OutputsBeam->RadiationIndex;
	}
	
	//m_ResComponent->Trace->Log(tlInfo, "Line:%d::CBsm::<SendRadiationIndex>::RadiationIndex::%d", __LINE__, m_ResComponent->OutputsBsm->RadiationIndex);

}

/******************************************************************************
* Function::ManualMain(VOID)
*
* Describe:
*
* Input   : None
*
* Output  : BOOL
*******************************************************************************/
VOID CBsm::ManualMain(VOID)
{
	;
}

/******************************************************************************
* Function::GetRadiationIndexFrBSMVOID)
*
* Describe:
*
* Input   : None
*
* Output  : BOOL
*******************************************************************************/
VOID CBsm::GetRadiationIndexFrBSM(VOID)
{
	m_RadiationIndexFrBsm = m_ResComponent->InputsBsm->RadiationIndex;
}

/******************************************************************************
* Function::OutputRequiredState(VOID)
*
* Describe:
*
* Input   : None
*
* Output  : BOOL
*******************************************************************************/
VOID CBsm::OutputRequiredState(VOID)
{
	//before connected with GUI,set BSM =no state
	if (!m_ResComponent->InputsFsm->com_status_gui)
	{
		m_RequiredState = BSM_NO_STATE;
		m_ResComponent->OutputsBsm->RequiredState = m_RequiredState;
	}
	else
	{
		if (EmulatorMode())				//PLC emulator enable
		{
			m_ResComponent->OutputsBsm->RequiredState = m_RequiredState | 0x80;		//bit7:Emulator
		}
		else
		{
			m_ResComponent->OutputsBsm->RequiredState = m_RequiredState;
		}
	}	
}

/******************************************************************************
* Function::FaultMain(VOID)
*
* Describe:
*
* Input   : None
*
* Output  : BOOL
*******************************************************************************/
VOID CBsm::FaultMain(VOID)
{
	if (m_ResourceMngr->FsmResource.GuiCmd.Reset)
	{
		m_CommandToBsm |= BSM_RESET;		
	}
	else
	{
		m_CommandToBsm &= ~BSM_RESET;		
	}

	SendCmdToBSM(m_CommandToBsm);
}

/******************************************************************************
* Function::RunState(VOID)
*
* Describe:
*
* Input   : None
*
* Output  : BOOL
*******************************************************************************/
VOID CBsm::RunState(VOID)
{
	switch (m_CurrentState)
	{
	case BSM_INITIAL:
	{
		InitialMain();
		break;
	}
	case BSM_STANDBY:
	{
		StandbyMain();
		break;
	}
	case BSM_MANUAL:
	{
		ManualMain();
	}
	case BSM_PREPARE:
	{
		PrepareMain();
		break;
	}
	case BSM_SERVO:
	{
		ServoMain();
		break;
	}
	case BSM_FAULT:
	{
		FaultMain();
		break;
	}
	case BSM_OFF:
	{
		OffMain();
		break;
	}

	default:
		
		break;
	}
}

/******************************************************************************
* Function::SendCmdToBSM(VOID)
*
* Describe:
*
* Input   : None
*
* Output  : BOOL
*******************************************************************************/
VOID CBsm::SendCmdToBSM(UINT8 Command)
{
	m_ResComponent->OutputsBsm->Commands = Command;
}

/******************************************************************************
* Function::OffMain(VOID)
*
* Describe:
*
* Input   : None
*
* Output  : BOOL
*******************************************************************************/
VOID CBsm::OffMain(VOID)
{
	;
}

/******************************************************************************
* Function::EmulatorMode(VOID)
*
* Describe:
*
* Input   : None
*
* Output  : BOOL
*******************************************************************************/
BOOL CBsm::EmulatorMode(VOID)
{	
	if (m_ResComponent->InputsBsmFmGUI->EmulatorEnable)
	{
		m_ResComponent->OutputsBsmToGUI->EmulatorMode = TRUE;
		return TRUE;
	}		
	else
	{
		m_ResComponent->OutputsBsmToGUI->EmulatorMode = FALSE;
		return FALSE;
	}
}

/******************************************************************************
* Function::ClearResetCmd(VOID)
*
* Describe:
*
* Input   : None
*
* Output  : BOOL
*******************************************************************************/
VOID CBsm::ClearResetCmd(VOID)
{
	m_CommandToBsm &= ~BSM_RESET;
	SendCmdToBSM(m_CommandToBsm);
}
/******************************************************************************
* Function::UpdateInputsFmGUI(VOID)
*
* Describe:
*
* Input   : None
*
* Output  : None
*******************************************************************************/
VOID CBsm::UpdateInputsFmGUI(VOID)
{
	m_ResComponent->OutputsBsm->BsmOk = m_ResComponent->InputsBsmFmGUI->BSMOk;
	m_ResComponent->OutputsBsm->HeadSpeed = m_ResComponent->InputsBsmFmGUI->HeadSpeed;
	m_ResComponent->OutputsBsm->UpJaw1Speed = m_ResComponent->InputsBsmFmGUI->UpJaw1Speed;
	m_ResComponent->OutputsBsm->UpJaw2Speed = m_ResComponent->InputsBsmFmGUI->UpJaw2Speed;
	m_ResComponent->OutputsBsm->HeadAngle = m_ResComponent->InputsBsmFmGUI->HeadAnagle;
	m_ResComponent->OutputsBsm->HeadAngleOffset = m_ResComponent->InputsBsmFmGUI->HeadAngleOffset;
	//m_ResComponent->OutputsBsm->HwEnable = m_ResComponent->InputsBsmFmGUI->BSMEnalbe;

	if ((m_ResComponent->InputsBsmFmGUI->BSMEnalbe & 0x0001))
	{
		m_ResComponent->OutputsBsm->HwEnable |= 0x01;
	}
	else
	{
		m_ResComponent->OutputsBsm->HwEnable &= ~0x01;
	}
	if ((m_ResComponent->InputsBsm->FieldLightFrmBSC) || ( m_ResComponent->InputsBsmFmGUI->BSMEnalbe & 0x0002)) 
	{
		m_ResComponent->OutputsBsm->HwEnable |= (0x0001 << 1);
	}
	else
	{
		m_ResComponent->OutputsBsm->HwEnable &= ~(0x0001<<1);
	}
	
	if ((m_ResComponent->InputsBsm->OdiLight) || (m_ResComponent->InputsBsmFmGUI->BSMEnalbe & 0x0004))
	{
		m_ResComponent->OutputsBsm->HwEnable |= (0x0001 << 2);
	}
	else
	{
		m_ResComponent->OutputsBsm->HwEnable &= ~(0x0001 << 2);
	}
	if ((m_ResComponent->InputsBsm->OdiLight) || (m_ResComponent->InputsBsmFmGUI->BSMEnalbe & 0x0008))
	{
		m_ResComponent->OutputsBsm->HwEnable |= (0x0001 <<3);
	}
	else
	{
		m_ResComponent->OutputsBsm->HwEnable &= ~(0x0001 << 3);
	}

	if ( (m_ResComponent->InputsBsm->BsmMotionEnable & 0x01))
	{
		m_ResComponent->OutputsBsm->HwEnable |= (0x0001 << 4);
	}
	else
	{
		m_ResComponent->OutputsBsm->HwEnable &= ~(0x0001 <<4);
	}
	if ((m_ResComponent->InputsBsm->BsmMotionEnable & 0x02))
	{
		m_ResComponent->OutputsBsm->HwEnable |= (0x0001 << 5);
	}
	else
	{
		m_ResComponent->OutputsBsm->HwEnable &= ~(0x01 << 5);

	}
	if ((m_ResComponent->InputsBsm->BsmMotionEnable & 0x04))
	{
		m_ResComponent->OutputsBsm->HwEnable |= (0x0001 << 6);
	}
	else
	{
		m_ResComponent->OutputsBsm->HwEnable &= ~(0x01 << 6);
	}
	if ((m_ResComponent->InputsBsm->BsmMotionEnable & 0x08))
	{
		m_ResComponent->OutputsBsm->HwEnable |= (0x0001 << 7);
	}
	else
	{
		m_ResComponent->OutputsBsm->HwEnable &= ~(0x01 << 7);
	}
	//MLC Init mode setting
	m_ResComponent->OutputsBsm->BSMInitMode = m_ResComponent->InputsBsmFmGUI->BSMInitMode;

	//Output Bsm Calibration Jaw
	for (size_t i = 0; i < 16; i++)
	{
		m_ResComponent->OutputsBsm->CalibrationJaw[i] = m_ResComponent->InputsBsmFmGUI->CalibrationJaw[i];		
	}

	m_ResComponent->OutputsBsm->HeadAngleK = m_ResComponent->InputsBsmFmGUI->HeadAngleK;
	m_ResComponent->OutputsBsm->HeadAngleSecondaryK = m_ResComponent->InputsBsmFmGUI->HeadAngleSecondaryK;
	m_ResComponent->OutputsBsm->HeadLimitEncoder = m_ResComponent->InputsBsmFmGUI->HeadLimitEncoder;
	m_ResComponent->OutputsBsm->CollimatorPosition = m_ResComponent->InputsBsmFmGUI->CollimatorPosition;
	
}
/******************************************************************************
* Function::UpdateOutputsToGUI(VOID)
*
* Describe:
*
* Input   : None
*
* Output  : None
*******************************************************************************/
VOID CBsm::UpdateOutputsToGUI(VOID)
{
	m_ResComponent->OutputsBsmToGUI->HeadP1 = m_ResComponent->InputsBsm->HeadP1;
	m_ResComponent->OutputsBsmToGUI->HeadP2 = m_ResComponent->InputsBsm->HeadP2;
	m_ResComponent->OutputsBsmToGUI->HeadSpeed = m_ResComponent->InputsBsm->HeadSpeed;
	m_ResComponent->OutputsBsmToGUI->Jaw1_P1 = m_ResComponent->InputsBsm->UpJaw1P1;
	m_ResComponent->OutputsBsmToGUI->Jaw1_P2 = m_ResComponent->InputsBsm->UpJaw1P2;
	m_ResComponent->OutputsBsmToGUI->Jaw2_P1 = m_ResComponent->InputsBsm->UpJaw2P1;
	m_ResComponent->OutputsBsmToGUI->Jaw2_P2 = m_ResComponent->InputsBsm->UpJaw2P2;
	m_ResComponent->OutputsBsmToGUI->BSMStatus = m_ResComponent->InputsBsm->Status;
	m_ResComponent->OutputsBsmToGUI->CollimatorPosition = m_ResComponent->InputsBsm->CollimatorPosition;
	
	m_ResComponent->OutputsBsmToGUI->InitResults0 = (UINT32)(m_ResComponent->InputsBsm->InitResult1 & 0xFFFFFFFF);
	m_ResComponent->OutputsBsmToGUI->InitResults1 = (UINT32)((m_ResComponent->InputsBsm->InitResult1 >> 30) & 0xFFFFFFFF);
	m_ResComponent->OutputsBsmToGUI->InitResults2 = (UINT32)(m_ResComponent->InputsBsm->InitResult2 & 0xFFFFFFFF);
	m_ResComponent->OutputsBsmToGUI->InitResults3 = (UINT32)((m_ResComponent->InputsBsm->InitResult2 >> 30) & 0xFFFFFFFF);
	m_ResComponent->OutputsBsmToGUI->LeafMaxSpeed = m_ResComponent->InputsBsm->LeafMaxSpeed;
	m_ResComponent->OutputsBsmToGUI->JawMaxSpeed = m_ResComponent->InputsBsm->JawMaxSpeed;
}

