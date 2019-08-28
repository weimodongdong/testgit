/*******************************************************************************
** File	      : Bgm.cpp
** Author:    : ChenLong
** Version    :	1.0
** Environment: VS2013
** Description:
** History    :	18/01/30    Chenlong    Created
              : 19/01/25    Chenlong    Reconstruct Program
*******************************************************************************/
/* INCLUDE ===================================================================*/
#include "Bgm.h"

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
CBgm::CBgm(CResourceMngr* ResourceMngr)
{
	m_ResourceMngr = ResourceMngr;
	m_ResComponent = ResourceMngr->GetResourceCom();
	m_PlanState = TRANS_STATE_IDLE;
	memset(&m_TransControl, 0, sizeof(m_TransControl));
	memset(&m_TransState, 0, sizeof(m_TransState));
	memset(&m_DoseData, 0, sizeof(m_DoseData));
	memset(&m_DebugControl, 0, sizeof(m_DebugControl));
	m_DebugControl.DummyEnable = FALSE;
	m_BankNumber = 0;
	m_InitEn = TRUE;
}

CBgm::~CBgm()
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
VOID CBgm::CycleUpdate(VOID)
{
	Initialize();

	UpdateInputResource();

	ExecuteBusiness();

	UpdateOutputResource();
}

/******************************************************************************
* Function: SendPlanToBGM(VOID)
*
* Describe:
*
* Input   : None
*
* Output  : None
*******************************************************************************/
VOID CBgm::SendDosePlanToBGM(VOID)
{
	if (SendBankDataToBGM() && (m_BankNumber == m_TransControl.BankCounter))		//send bank success and counter reach specified value 
		m_TransState.PlanSendFinish = TRUE;		//plan data send completed
	else
		m_TransState.PlanSendFinish = FALSE;	//continue send

	return;
}

/******************************************************************************
* Function: SendBankDataToBGM(VOID)
*
* Describe:
*
* Input   : None
*
* Output  : None
*******************************************************************************/
BOOL CBgm::SendBankDataToBGM(VOID)
{
	if (InputBusIsIdle() && OutputBusIsIdle())
	{
		LoadDataToBus();
		LoadControlToBus();
		LoadServoAxisTygeToBus(m_ResourceMngr->PlanResource.GetServoAxisType());
		m_TransControl.BankCounter++;	//next bank
		/*if (m_AckTimer.IsReady())		//start ack time out timer
			m_AckTimer.StartTimer(m_ResourceMngr->GetCurrentSysTime(), ACK_INTERVAL);
		else{ ; }*/

		return FALSE;			
	}
	else if (RecvAckOk())
	{
		SetBusIdle();					//set data bus to idle state
//		m_AckTimer.ResetTimer();		//reset timer
		
		return TRUE;					//send one bank data success
	}
/*	else if (m_AckTimer.IsTimeOut(m_ResourceMngr->GetCurrentSysTime()))
	{
		m_TransState.AckTimeOut = TRUE;		//resend plan data

		return FALSE;
	}*/
	else
	{
		return FALSE;			//wait for response
	}
}

/******************************************************************************
* Function: RecvAckOk(VOID)
*
* Describe:
*
* Input   : None
*
* Output  : None
*******************************************************************************/
BOOL CBgm::RecvAckOk(VOID)
{
	if (m_TransControl.BankCounter + DATA_BUS_FB_OFFSET == m_ResComponent->InputsBgmPlan->BankIndex)
		return TRUE;
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
VOID CBgm::LoadDataToBus(VOID)
{
	//Load Data
	UINT32 StartPos = DATA_BUS_DATA_LEN * m_TransControl.BankCounter;
	if (StartPos + DATA_BUS_DATA_LEN >= MAX_CP_TOTAL)
	{
		m_ResComponent->Trace->Log(tlInfo, "Line%d::CBgm::<LoadDataToBus>::Unexcepted length");
		return;
	}
	else
	{
		for (UINT32 i = 0; i < DATA_BUS_DATA_LEN; i++)
		{
			m_ResComponent->OutputsBgmPlan->Data[i].RadiationIndex = m_DoseData.Data[StartPos + i].RadiationIndex;
			m_ResComponent->OutputsBgmPlan->Data[i].Dose = m_DoseData.Data[StartPos + i].OriginalData;
			m_ResComponent->OutputsBgmPlan->Data[i].DoseRate = m_DoseData.Data[StartPos + i].Speed;
		}
		m_ResComponent->OutputsBgmPlan->DoseTotal = m_DoseData.DoseTotal;
		m_ResComponent->OutputsBgmPlan->CPTotal = m_DoseData.CPTotal;
	}

	//Construct exception
	if (m_DebugControl.TransIDEn)
	{
		m_TransControl.BankCounter == 2 ? m_TransControl.BeamTransID++ : NULL;
	}
	else if (m_DebugControl.Crc32En)
	{
		m_TransControl.BankCounter == 2 ? m_ResComponent->OutputsBgmPlan->Data[0].Dose++ : NULL;
	}
	else{ ; }

	return;
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
VOID CBgm::LoadControlToBus(VOID)
{
	m_ResComponent->OutputsBgmPlan->BeamID		= m_TransControl.BeamTransID;
	m_ResComponent->OutputsBgmPlan->CPIndex		= m_TransControl.BankCounter * DATA_BUS_DATA_LEN;
	m_ResComponent->OutputsBgmPlan->BankIndex	= m_TransControl.BankCounter+1;			//counter start with 0
	m_ResComponent->OutputsBgmPlan->Crc32		= m_TransControl.Crc32;

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
BOOL CBgm::TransHasError(VOID)
{
	if (m_ResComponent->InputsBgmPlan->BankIndex == DOSE_ERROR_CRC ||
		m_ResComponent->InputsBgmPlan->BankIndex == DOSE_ERROR_BEAM_ID ||
		m_TransState.AckTimeOut)
	{
		#if RTC_DEBUG_EXCEPTION
		m_ResComponent->Trace->Log(tlInfo, "CBgm::<RunSendState>::Has Error:BankIndex:%u,Ack Time out:%d", m_ResComponent->InputsBgmPlan->BankIndex, m_TransState.AckTimeOut);
		#endif

		return TRUE;
	}
	else
		return FALSE;
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
VOID CBgm::ResetCommunication(VOID)
{
	m_ResComponent->OutputsBgmPlan->BeamID = 0;			//reset singal
	m_ResComponent->OutputsBgmPlan->BankIndex = 0;
	m_ResComponent->OutputsBgmPlan->CPIndex = 0;
	m_ResComponent->OutputsBgmPlan->CPTotal = 0;
	m_ResComponent->OutputsBgmPlan->Crc32 = 0;
	memset(&m_ResComponent->OutputsBgmPlan->Data, 0, sizeof(m_ResComponent->OutputsBgmPlan->Data));
	m_ResComponent->OutputsBgmPlan->DoseTotal = 0;

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
BOOL CBgm::InputBusIsIdle(VOID)
{
	if (m_ResComponent->InputsBgmPlan->BankIndex == DATA_BUS_IDLE)
		return TRUE;
	else
		return FALSE;
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
BOOL CBgm::OutputBusIsIdle(VOID)
{
	if (m_ResComponent->OutputsBgmPlan->BankIndex == DATA_BUS_IDLE)
		return TRUE;
	else
		return FALSE;
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
BOOL CBgm::BusIsReset(VOID)
{
	if (m_ResComponent->InputsBgmPlan->BeamID == DATA_BUS_FB_RESET)
		return TRUE;
	else
		return FALSE;
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
VOID CBgm::SetBusIdle(VOID)
{
	m_ResComponent->OutputsBgmPlan->BankIndex = 0;
	m_ResComponent->OutputsBgmPlan->CPIndex = 0;
	m_ResComponent->OutputsBgmPlan->CPTotal = 0;
	m_ResComponent->OutputsBgmPlan->Crc32 = 0;

	return;
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
BgmPlanState_eu CBgm::RunIdleState(VOID)
{
	m_ResourceMngr->BgmResource.SetPlanBgmIsReady(FALSE);

	SetBusIdle();		//set data bus to idle state
	CleanServoAxisBusData(); // clean Servo AxisBus Data
	if (m_TransState.RetransCounter == MAX_RESEND_TIME)
	{
		m_TransState.SendFailed = TRUE;
		return TRANS_STATE_IDLE;		//hold on idle state
	}
	if (InputBusIsIdle())
	{
		if (m_ResourceMngr->PlanResource.PlanIsReady() || m_TransState.RetransCounter != 0)
		{
			GetPlanData();

			return TRANS_STATE_SEND;
		}
		else
		{
			return TRANS_STATE_IDLE;		//hold on idle state
		}
	}
	else
	{
		return TRANS_STATE_IDLE;		//hold on idle state
	}
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
BgmPlanState_eu CBgm::RunSendState(VOID)
{
	m_ResourceMngr->BgmResource.SetPlanBgmIsReady(FALSE);

	SendDosePlanToBGM();					

	if (TransHasError())
	{
		m_TransState.RetransCounter++;		//record retransition number of times
//		m_AckTimer.ResetTimer();			//reset check timer
		
		return TRANS_STATE_ERROR;			//go to error state
	}
	else if (m_ResourceMngr->FsmResource.GuiCmd.ClosePatient || !m_ResourceMngr->PlanResource.PlanIsReady() || m_ResourceMngr->FsmResource.GuiCmd.CleanPlan)	//close patient or new plan
	{
		return TRANS_STATE_ERROR;			//go to error state
	}
	else if (m_TransState.PlanSendFinish)
	{
		m_TransState.RetransCounter = 0;
		return TRANS_STATE_FINISH;			//go to finish state
	}
	else
	{
		return TRANS_STATE_SEND;			//hold on send state
	}
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
BgmPlanState_eu CBgm::RunFinishState(VOID)
{
	m_ResourceMngr->BgmResource.SetPlanBgmIsReady(TRUE);

	SetBusIdle();

	m_TransState.RetransCounter = 0;	//reset resend counter
	
	if (m_ResourceMngr->FsmResource.GuiCmd.ClosePatient || !m_ResourceMngr->PlanResource.PlanIsReady() || m_ResourceMngr->FsmResource.GuiCmd.CleanPlan)
	{
		CleanServoAxisBusData();
		return TRANS_STATE_ERROR;
	}
	else
	{
		return TRANS_STATE_FINISH;			//hold on finish state
	}
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
BgmPlanState_eu CBgm::RunErrorState(VOID)
{
	m_ResourceMngr->BgmResource.SetPlanBgmIsReady(FALSE);	//set transition not complete
	ResetCommunication();
	
	if (BusIsReset())
	{
		ClearPlan();
		return TRANS_STATE_IDLE;								//go to idle state
	}
	else
		return TRANS_STATE_ERROR;								//hold on error state
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
VOID CBgm::GetPlanData(VOID)
{
	//Debug control
	if (m_DebugControl.DummyEnable)
	{
		ConstructDummyData();
		return; //end run
	}
	else
	{
		//do nothing
	}

	//Get plan handler
	BeamPlan_t* beamPlanPtr = m_ResourceMngr->PlanResource.GetBeamPlanData();
	OriginalCP_t* originalPtr = m_ResourceMngr->PlanResource.GetOriginalPlan();
	BackCalSpeedCP_t* speedPtr = m_ResourceMngr->PlanResource.GetSpeedData();
	if (NULL == beamPlanPtr || NULL == originalPtr || NULL == speedPtr)
	{
		#if RTC_DEBUG_EXCEPTION
			m_ResComponent->Trace->Log(tlInfo, "Line:%d::CBgm::<GetPlanData>::beam plan handler is null", __LINE__);
		#endif
		return; //end run
	}
	else
	{
		//do nothing
	}
			
	//Store CP total
	m_DoseData.CPTotal = beamPlanPtr->TotalCP;

	//Store total dose
	m_DoseData.DoseTotal = beamPlanPtr->Dose;

	//Store original dose data
	for (UINT32 i = 0; i < m_DoseData.CPTotal; i++)
	{
		m_DoseData.Data[i].RadiationIndex = originalPtr->RadiationIndex;
		m_DoseData.Data[i].OriginalData = originalPtr->DoseWeight;
		originalPtr++;
	}
	
	//Store dose speed
	for (UINT16 i = 0; i < m_DoseData.CPTotal - 1; i++)
	{
		m_DoseData.Data[i].Speed = speedPtr->Dose[i];
	}
	
	//Get bank number
	FLOAT Value = (FLOAT)m_ResourceMngr->PlanResource.GetPlanCPTotal() / DATA_BUS_DATA_LEN;
	m_BankNumber = (UINT16)ceil_(Value);				//example: 3.1 to 4

	//Add transition beam number
	m_TransControl.BeamTransID++;	//add transition number
	m_TransControl.BeamTransID = (m_TransControl.BeamTransID == 65535 ? 1 : m_TransControl.BeamTransID);

	//Get crc32 value
	m_TransControl.Crc32 = m_ResourceMngr->ToolCrc32.Crc32Buffer((UCHAR*)(&m_DoseData), sizeof(m_DoseData));
		
	return;
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
VOID CBgm::ConstructDummyData(VOID)
{
	m_DoseData.CPTotal = 178;

	for (UINT32 i = 0; i < m_DoseData.CPTotal; i++)
	{
		m_DoseData.Data[i].OriginalData = static_cast<FLOAT>(i);
		m_DoseData.Data[i].Speed		= static_cast<FLOAT>(i + 0.3);
	}

	FLOAT Value = (FLOAT)m_DoseData.CPTotal / 30;
	m_BankNumber = (UINT16)ceil_(Value);				//example: 3.1 to 4

	//Add transition beam number
	m_TransControl.BeamTransID++;	//add transition number
	m_TransControl.BeamTransID = (m_TransControl.BeamTransID == 65535 ? 1 : m_TransControl.BeamTransID);

	return;
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
VOID CBgm::ClearPlan(VOID)
{
	//clear data buffer
	memset(&m_DoseData, 0, sizeof(m_DoseData));
	//clear control sigal
	m_TransControl.BankCounter = 0;
	m_TransControl.Crc32 = 0;
	m_TransState.AckTimeOut = FALSE;
	m_TransState.PlanSendFinish = FALSE;
	//reset ack timer
//	m_AckTimer.ResetTimer();
	m_BankNumber = 0;
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
VOID CBgm::LoadServoAxisTygeToBus(BYTE ServoAxisTyge)
{
	m_ResComponent->OutputsBgmPlan->ServoAxisType = ServoAxisTyge;
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
VOID CBgm::CleanServoAxisBusData()
{
	m_ResComponent->OutputsBgmPlan->ServoAxisType = 0;
}

/******************************************************************************
* Function: Initialize(VOID)
*
* Describe:
*
* Input   : None
*
* Output  : BOOL
*******************************************************************************/
VOID CBgm::Initialize(VOID)
{
	if (m_InitEn)
	{
		m_InitEn = FALSE;
	}
	else
	{

	}
}

/******************************************************************************
* Function: ExecuteBusiness(VOID)
*
* Describe:
*
* Input   : None
*
* Output  : BOOL
*******************************************************************************/
VOID CBgm::ExecuteBusiness(VOID)
{
	switch (m_PlanState)
	{
	case TRANS_STATE_IDLE:
		m_PlanState = RunIdleState();
		break;
	case TRANS_STATE_SEND:
		m_PlanState = RunSendState();
		break;
	case TRANS_STATE_ERROR:
		m_PlanState = RunErrorState();
		break;
	case TRANS_STATE_FINISH:
		m_PlanState = RunFinishState();
		break;
	default:
		break;
	}
}

/******************************************************************************
* Function: UpdateInputResource(VOID)
*
* Describe:
*
* Input   : None
*
* Output  : BOOL
*******************************************************************************/
BOOL CBgm::UpdateInputResource(VOID)
{
	return TRUE;
}

/******************************************************************************
* Function: UpdateOutputResource(VOID)
*
* Describe:
*
* Input   : None
*
* Output  : BOOL
*******************************************************************************/
BOOL CBgm::UpdateOutputResource(VOID)
{
	return TRUE;
}


