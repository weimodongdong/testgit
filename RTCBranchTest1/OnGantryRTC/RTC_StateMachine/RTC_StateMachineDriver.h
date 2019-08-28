///////////////////////////////////////////////////////////////////////////////
// RTC_StateMachineDriver.h

#ifndef __RTC_STATEMACHINEDRIVER_H__
#define __RTC_STATEMACHINEDRIVER_H__

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "TcBase.h"

#define RTC_STATEMACHINEDRV_NAME        "RTC_STATEMACHINE"
#define RTC_STATEMACHINEDRV_Major       1
#define RTC_STATEMACHINEDRV_Minor       0

#define DEVICE_CLASS CRTC_StateMachineDriver

#include "ObjDriver.h"

class CRTC_StateMachineDriver : public CObjDriver
{
public:
	virtual IOSTATUS	OnLoad();
	virtual VOID		OnUnLoad();

	//////////////////////////////////////////////////////
	// VxD-Services exported by this driver
	static unsigned long	_cdecl RTC_STATEMACHINEDRV_GetVersion();
	//////////////////////////////////////////////////////
	
};

Begin_VxD_Service_Table(RTC_STATEMACHINEDRV)
	VxD_Service( RTC_STATEMACHINEDRV_GetVersion )
End_VxD_Service_Table


#endif // ifndef __RTC_STATEMACHINEDRIVER_H__