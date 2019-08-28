///////////////////////////////////////////////////////////////////////////////
// RTC_StateMachineDriver.cpp
#include "TcPch.h"
#pragma hdrstop

#include "RTC_StateMachineDriver.h"
#include "RTC_StateMachineClassFactory.h"

DECLARE_GENERIC_DEVICE(RTC_STATEMACHINEDRV)

IOSTATUS CRTC_StateMachineDriver::OnLoad( )
{
	TRACE(_T("CObjClassFactory::OnLoad()\n") );
	m_pObjClassFactory = new CRTC_StateMachineClassFactory();

	return IOSTATUS_SUCCESS;
}

VOID CRTC_StateMachineDriver::OnUnLoad( )
{
	delete m_pObjClassFactory;
}

unsigned long _cdecl CRTC_StateMachineDriver::RTC_STATEMACHINEDRV_GetVersion( )
{
	return( (RTC_STATEMACHINEDRV_Major << 8) | RTC_STATEMACHINEDRV_Minor );
}

