/*******************************************************************************
** File	      : PlanMngr.cpp
** Author:    : ChenLong
** Version    :	1.0
** Environment: VS2013
** Description:
** History    :	18/01/05    Chenlong    Created
*******************************************************************************/
/* INCLUDE ===================================================================*/
#include "PlanMngr.h"

/* GLOBAL VARS================================================================*/
/* FUNCTIONS =================================================================*/
/******************************************************************************
* Function: CPlanMngr()
*
* Describe: construct function
*
* Input   : None
*
* Output  : None
*******************************************************************************/
CPlanMngr::CPlanMngr()
{
	
}

CPlanMngr::CPlanMngr(CResourceMngr *Resource)
{
	memset(&m_BeamPlan, 0, sizeof(m_BeamPlan));
	memset(&m_OriginalCP, 0, sizeof(m_OriginalCP));
	memset(&m_DeltaCP, 0, sizeof(m_DeltaCP));
	memset(&m_BackCalSpeedCP, 0, sizeof(m_BackCalSpeedCP));

	m_ResourceMngr = Resource;
	m_ResComponentPtr = Resource->GetResourceCom();
	m_PlanDataRecvEn = TRUE;
	
	m_HandlerInputs.BeamPlanPtr = &m_BeamPlan;
	m_HandlerInputs.OriginalPlanPtr = m_OriginalCP;
	m_HandlerInputs.DeltaPtr = &m_DeltaCP;
	m_HandlerInputs.BackCalSpeedPtr = &m_BackCalSpeedCP;
	m_HandlerInputs.ResComponentPtr = m_ResComponentPtr;
	#if OUTPUT_PLAN_ENABLE
		FileBite = FALSE;
	#endif
	m_InitEn = TRUE;
}


CPlanMngr::~CPlanMngr()
{
}

/******************************************************************************
* Function: CycleUpdate(VOID)
*
* Describe: CycleUpdate
*
* Input   : None
*
* Output  : None
*******************************************************************************/
BOOL CPlanMngr::CycleUpdate(VOID)
{
	/*if (!m_PlanDataRecvEn)
	{
	
	    if (!FileBite)
	    {
	    	PrintFileInWrite("%TC_TARGETPATH%PlanGantry.txt");
	    	FileBite = TRUE;
	    }
   }*/

	//Initialization
	Initialize();
	
	//Mnage plan data
	if (m_ResComponentPtr->PlanInputs->PlanReset) //TcpServer thread reset command
	{
		ClearALLPlan();
		m_ResourceMngr->PlanResource.SetPlanReady(FALSE); //set plan is ready
		m_ResComponentPtr->PlanOutputs->PlanReady = FALSE;
		m_PlanDataRecvEn = TRUE; //enable receive new plan
		m_ResourceMngr->PlanResource.SetPlanEvent(CLOSE_PATIENT); //notify to switch fsm

		return TRUE;
	}
	else if (m_PlanDataRecvEn && PlanIsReceived())
	{
		//Store plan data
		StorePlanData(); //store plan data in loacl from share memory
		//Handle data
		if (RadiationMv == m_BeamPlan.RadiationType)
		{
			Linac.HandlePlanData(); //handle plan data
			m_ResourceMngr->PlanResource.SetServoAxisType(Linac.GetServoAxisType());
			m_ResourceMngr->PlanResource.SetPlanEvent(MV_PLAN_RECV); //notify to switch fsm
		}
		else if (RadiationKv == m_BeamPlan.RadiationType)
		{
			Igs.HandlePlanData();
			m_ResourceMngr->PlanResource.SetServoAxisType(None);
			m_ResourceMngr->PlanResource.SetPlanEvent(KV_PLAN_RECV); //notify to switch fsm
		}
		else
		{
			m_ResComponentPtr->Trace->Log(tlInfo, "Line%d::CPlanMngr::<CycleUpdate>::Unexcepted radiation type");
		}
		//Output else resource
		m_ResourceMngr->PlanResource.SetRadiationProtTime(Linac.GetProtectTime());
		SetPlanReady();	//notify other control objects
		m_PlanDataRecvEn = FALSE;		//disable receive plan data

		return TRUE;
	}
	else
	{
		return FALSE;  //wait for TcpServer thread receive plan data complete
	}
}

/******************************************************************************
* Function: StorePlanData(VOID)
*
* Describe: store Plan Data
*
* Input   : None
*
* Output  : None
*******************************************************************************/
BOOL CPlanMngr::StorePlanData(VOID)
{
	//Store data from module inputs
	//memcpy(&m_BeamPlan, &m_ResComponentPtr->PlanInputs->BeamPlan, sizeof(m_ResComponentPtr->PlanInputs->BeamPlan));
	m_BeamPlan.BeamType						= m_ResComponentPtr->PlanInputs->BeamPlan.BeamType;
	m_BeamPlan.RadiationType				= m_ResComponentPtr->PlanInputs->BeamPlan.RadiationType;
	m_BeamPlan.BeamNumber					= m_ResComponentPtr->PlanInputs->BeamPlan.BeamNumber;
	m_BeamPlan.TotalCP						= m_ResComponentPtr->PlanInputs->BeamPlan.TotalCP;
	m_BeamPlan.DoseRateSet					= m_ResComponentPtr->PlanInputs->BeamPlan.DoseRateSet;
	m_BeamPlan.Dose							= m_ResComponentPtr->PlanInputs->BeamPlan.Dose;
	m_BeamPlan.GantryAngleTolerance			= m_ResComponentPtr->PlanInputs->BeamPlan.GantryAngleTolerance;
	m_BeamPlan.BeamLimitingAngleTolerance	= m_ResComponentPtr->PlanInputs->BeamPlan.BeamLimitingAngleTolerance;
	m_BeamPlan.AsymxPositionTolerance		= m_ResComponentPtr->PlanInputs->BeamPlan.AsymxPositionTolerance;
	m_BeamPlan.AsymyPositionTolerance		= m_ResComponentPtr->PlanInputs->BeamPlan.AsymyPositionTolerance;
	m_BeamPlan.MlcPositionTolerance			= m_ResComponentPtr->PlanInputs->BeamPlan.MlcPositionTolerance;
	m_BeamPlan.TableTopVerPositionTolerance	= m_ResComponentPtr->PlanInputs->BeamPlan.TableTopVerPositionTolerance;
	m_BeamPlan.TableTopLonPositionTolerance	= m_ResComponentPtr->PlanInputs->BeamPlan.TableTopLonPositionTolerance;
	m_BeamPlan.TableTopLatPositionTolerance = m_ResComponentPtr->PlanInputs->BeamPlan.TableTopLatPositionTolerance;

	for (UINT32 i = 0; i < m_BeamPlan.TotalCP; i++)
	{
		m_OriginalCP[i].BeamEnergy		= m_ResComponentPtr->PlanInputs->BeamEnergyPlan[i];
		m_OriginalCP[i].DoseWeight		= m_ResComponentPtr->PlanInputs->DoseWeight[i];
		memcpy(m_OriginalCP[i].MlcPos, m_ResComponentPtr->PlanInputs->MlcPos[i], sizeof(m_OriginalCP[i].MlcPos));
		m_OriginalCP[i].MlcAngle		= m_ResComponentPtr->PlanInputs->MlcAngle[i];
		m_OriginalCP[i].GantryAngle		= m_ResComponentPtr->PlanInputs->GantryAngle[i];
		m_OriginalCP[i].PssAngle		= m_ResComponentPtr->PlanInputs->PssAngle[i];
		m_OriginalCP[i].TableVerPos		= m_ResComponentPtr->PlanInputs->TableVerPos[i];
		m_OriginalCP[i].TableLonPos		= m_ResComponentPtr->PlanInputs->TableLonPos[i];
		m_OriginalCP[i].TableLatPos		= m_ResComponentPtr->PlanInputs->TableLatPos[i];
		m_OriginalCP[i].TablePitchAngle = m_ResComponentPtr->PlanInputs->TablePitchAngle[i];
		m_OriginalCP[i].TableRollAngle	= m_ResComponentPtr->PlanInputs->TableRollAngle[i];
		m_OriginalCP[i].MlcDir			= m_ResComponentPtr->PlanInputs->MlcDir[i];
		m_OriginalCP[i].GantryDir		= m_ResComponentPtr->PlanInputs->GantryDir[i];
		m_OriginalCP[i].PssDir			= m_ResComponentPtr->PlanInputs->PssDir[i];
		m_OriginalCP[i].TablePitchDir	= m_ResComponentPtr->PlanInputs->TablePitchDir[i];
		m_OriginalCP[i].TableRollDir	= m_ResComponentPtr->PlanInputs->TableRollDir[i];
	}

	

	return TRUE;
}

/******************************************************************************
* Function: PlanIsReceived(VOID)
*
* Describe: Check Plan is received ok
*
* Input   : None
*
* Output  : None
*******************************************************************************/
BOOL CPlanMngr::PlanIsReceived(VOID)
{
	UINT16 RecvCounter = 0;

	if (0 != m_ResourceMngr->GetResourceCom()->PlanInputs->BeamPlan.TotalCP)
	{
		for (size_t i = 0; i < m_ResourceMngr->GetResourceCom()->PlanInputs->BeamPlan.TotalCP; i++)
		{
			if (m_ResourceMngr->GetResourceCom()->PlanInputs->RecvedCP[i])
				RecvCounter++;
		}
		
		if (m_ResourceMngr->GetResourceCom()->PlanInputs->BeamPlan.TotalCP == RecvCounter)
			return TRUE;
		else
			return FALSE;
	}
	else
	{
		return FALSE;
	}
}

/******************************************************************************
* Function: ClearALLPlan(VOID)
*
* Describe: Clear all plan data
*
* Input   : None
*
* Output  : None
*******************************************************************************/
BOOL CPlanMngr::ClearALLPlan(VOID)
{
	memset(&m_BeamPlan, 0, sizeof(m_BeamPlan));
	memset(&m_OriginalCP, 0, sizeof(m_OriginalCP));
	memset(&m_DeltaCP, 0, sizeof(m_DeltaCP));
	memset(&m_BackCalSpeedCP, 0, sizeof(m_BackCalSpeedCP));
	m_ResourceMngr->PlanResource.SetPlanReady(FALSE);
	m_ResourceMngr->GetResourceCom()->PlanOutputs->PlanReady = FALSE;
	m_ResourceMngr->PlanResource.SetServoAxisType(None);
	Linac.ClearPlanData();
	Igs.ClearPlanData();

	return TRUE;
}

/******************************************************************************
* Function: SetPlanReady(VOID)
*
* Describe: set plan is ready state
*
* Input   : None
*
* Output  : None
*******************************************************************************/
BOOL CPlanMngr::SetPlanReady(VOID)
{
	m_ResourceMngr->PlanResource.SetPlanCPTotal(m_BeamPlan.TotalCP);		//set total cp number
	m_ResourceMngr->PlanResource.SetDoseTotal(m_BeamPlan.Dose);

	m_ResourceMngr->PlanResource.SetPlanReady(TRUE);						//set plan is ready
	m_ResComponentPtr->PlanOutputs->PlanReady = TRUE;

	return TRUE;
}

#if OUTPUT_PLAN_ENABLE
VOID CPlanMngr::PrintFileInWrite(PCCH szFileName)
{
	TcFileHandle hFile = NULL;
	m_ResourceMngr->m_PlanTxtPtr->FileOpen(szFileName, static_cast<TcFileAccessMode>(amText | amAppend), &hFile);    //open file
	char Buffer[1024] = { 0 };
	char Buffer1[1024] = { 0 };
	char Buffer2[1024] = { 0 };
	
	// origal   data
	
	for (size_t i = 0; i < m_BeamPlan.TotalCP; i++) //first and last cp not include
	{   
		memset(Buffer, 0, sizeof(Buffer));
		sprintf(Buffer, "%04f, ", m_OriginalCP[i].GantryAngle);
		memset( Buffer1, 0, sizeof(Buffer1) );
		sprintf(Buffer1, "%04f, ", m_OriginalCP[i].DoseWeight);
		memset(Buffer2, 0, sizeof(Buffer2));
		sprintf(Buffer2, "%02d, ", m_OriginalCP[i].RadiationIndex);
		strcat(Buffer, Buffer1);
		strcat(Buffer, Buffer2);
		UINT32 nRead = strlen(Buffer);                                                              //Record the length to be written
		UINT32 nWritten = 0;
		m_ResourceMngr->m_PlanTxtPtr->FileWrite(hFile, Buffer, nRead, &nWritten);
		for (size_t j = 0; j < MLC_LEAF_NUM; j++)
		{
			memset(Buffer, 0, sizeof(Buffer));
			sprintf(Buffer, "%04f, ", m_OriginalCP[i].MlcPos[j]);
			nRead = strlen(Buffer);
			nWritten = 0;
			m_ResourceMngr->m_PlanTxtPtr->FileWrite(hFile, Buffer, nRead, &nWritten);
		}
		
		char chr[10] = { "\r\n" };
		nRead = strlen(chr);
		nWritten = 0;
		m_ResourceMngr->m_PlanTxtPtr->FileWrite(hFile, chr, nRead, &nWritten);
	}
	m_ResourceMngr->m_PlanTxtPtr->FileClose(hFile);                                                                  //close file "free hFile"

	/*//delta data
	for (size_t i = 0; i < m_BeamPlan.TotalCP-1; i++) //first and last cp not include
	{
		memset(Buffer, 0, sizeof(Buffer));
		sprintf(Buffer, "%04f, ", m_DeltaCP.GantryAngle[i]);
		//m_ResourceMngr->m_PlanTxtPtr->FileWrite(hFile, Buffer, nRead, &nWritten);                                //write data into file

		memset(&Buffer1, 0, sizeof(Buffer1));
		sprintf(Buffer1, "%04f, ", m_DeltaCP.DoseWeight[i]);
		strcat(Buffer, Buffer1);
		UINT32 nRead = strlen(Buffer);                                                              //Record the length to be written
		UINT32 nWritten = 0;
		m_ResourceMngr->m_PlanTxtPtr->FileWrite(hFile, Buffer, nRead, &nWritten);
		for (size_t j = 0; j < MLC_LEAF_NUM; j++)
		{
			memset(Buffer, 0, sizeof(Buffer));
			sprintf(Buffer, "%04f, ", m_DeltaCP.MlcPos[i][j]);
			nRead = strlen(Buffer);
			nWritten = 0;
			m_ResourceMngr->m_PlanTxtPtr->FileWrite(hFile, Buffer, nRead, &nWritten);
		}


		char chr[10] = { "\r\n" };
		nRead = strlen(chr);
		nWritten = 0;
		m_ResourceMngr->m_PlanTxtPtr->FileWrite(hFile, chr, nRead, &nWritten);
	}
	*/

	/*
	//Servo  speed
	for (size_t i = 0; i < m_BeamPlan.TotalCP-1; i++) //first and last cp not include
	{
	memset(Buffer, 0, sizeof(Buffer));
	sprintf(Buffer, "%04f, ", m_BackCalSpeedCP.Gantry[i]);
	//m_ResourceMngr->m_PlanTxtPtr->FileWrite(hFile, Buffer, nRead, &nWritten);                                //write data into file

	memset(&Buffer1, 0, sizeof(Buffer1));
	sprintf(Buffer1, "%04f, ", m_BackCalSpeedCP.Dose[i]);
	strcat(Buffer, Buffer1);
	UINT32 nRead = strlen(Buffer);                                                              //Record the length to be written
	UINT32 nWritten = 0;
	m_ResourceMngr->m_PlanTxtPtr->FileWrite(hFile, Buffer, nRead, &nWritten);
	for (size_t j = 0; j < MLC_LEAF_NUM; j++)
	{
	memset(Buffer, 0, sizeof(Buffer));
	sprintf(Buffer, "%04f, ", m_BackCalSpeedCP.Mlc[i][j]);
	nRead = strlen(Buffer);
	nWritten = 0;
	m_ResourceMngr->m_PlanTxtPtr->FileWrite(hFile, Buffer, nRead, &nWritten);
	}


	char chr[10] = { "\r\n" };
	nRead = strlen(chr);
	nWritten = 0;
	m_ResourceMngr->m_PlanTxtPtr->FileWrite(hFile, chr, nRead, &nWritten);
	}
	m_ResourceMngr->m_PlanTxtPtr->FileClose(hFile);                                                                  //close file "free hFile"
	*/
}
#endif

/******************************************************************************
* Function: Initialize(VOID)
*
* Describe: set plan is ready state
*
* Input   : None
*
* Output  : None
*******************************************************************************/
VOID CPlanMngr::Initialize(VOID)
{
	if (m_InitEn)
	{
		BindingResourceToMngr();
		
		m_InitEn = FALSE;
	}
	else
	{
		//do nothing
	}

	return;
}

/******************************************************************************
* Function: Initialize(VOID)
*
* Describe: binding resource to resource manager
*
* Input   : None
*
* Output  : None
*******************************************************************************/
VOID CPlanMngr::BindingResourceToMngr(VOID)
{
	m_ResourceMngr->PlanResource.BindingPtrBeamPlanData(&m_BeamPlan);
	m_ResourceMngr->PlanResource.BindingPtrOriginalPlan(m_OriginalCP);
	m_ResourceMngr->PlanResource.BindingPtrSpeedData(&m_BackCalSpeedCP);
	m_ResourceMngr->PlanResource.BindingPtrDeltaTimeCp(Linac.GetDeltaTimeCp());
	
	return;
}