///////////////////////////////////////////////////////////////////////////////
// RTC_StateMachineCtrl.h

#ifndef __RTC_STATEMACHINECTRL_H__
#define __RTC_STATEMACHINECTRL_H__

#include <atlbase.h>
#include <atlcom.h>

#define RTC_STATEMACHINEDRV_NAME "RTC_STATEMACHINE"

#include "resource.h"       // main symbols
#include "RTC_StateMachineW32.h"
#include "TcBase.h"
#include "RTC_StateMachineClassFactory.h"
#include "TcOCFCtrlImpl.h"

class CRTC_StateMachineCtrl 
	: public CComObjectRootEx<CComMultiThreadModel>
	, public CComCoClass<CRTC_StateMachineCtrl, &CLSID_RTC_StateMachineCtrl>
	, public IRTC_StateMachineCtrl
	, public ITcOCFCtrlImpl<CRTC_StateMachineCtrl, CRTC_StateMachineClassFactory>
{
public:
	CRTC_StateMachineCtrl();
	virtual ~CRTC_StateMachineCtrl();

DECLARE_REGISTRY_RESOURCEID(IDR_RTC_STATEMACHINECTRL)
DECLARE_NOT_AGGREGATABLE(CRTC_StateMachineCtrl)

DECLARE_PROTECT_FINAL_CONSTRUCT()

BEGIN_COM_MAP(CRTC_StateMachineCtrl)
	COM_INTERFACE_ENTRY(IRTC_StateMachineCtrl)
	COM_INTERFACE_ENTRY(ITcCtrl)
	COM_INTERFACE_ENTRY(ITcCtrl2)
END_COM_MAP()

};

#endif // #ifndef __RTC_STATEMACHINECTRL_H__
