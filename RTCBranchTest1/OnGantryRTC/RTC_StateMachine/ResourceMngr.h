/*******************************************************************************
** File	      : ResourceMgnr.h
** Author     : ChenLong
** Version    :	1.1
** Environment: VS2013
** Description:
** History    :	17/12/13    Chenlong    Created
*******************************************************************************/
/*==== DECLARATION CONTROL ===================================================*/
#pragma once

/*==== INCLUDES ==============================================================*/
#include "TcDef.h"
#include "RTC_StateMachineServices.h"
#include "RTC_StateMachineInterfaces.h"
#include "TcFileAccessInterfaces.h"
#include "TcTimeConversion.h"

#include "Crc32.h"
#include "ResourceBgm.h"
#include "ResourceIgs.h"
#include "ResourceFsm.h"
#include "ResourcePlan.h"
#include "ResourceBsm.h"

/*==== MACROS ================================================================*/
//Debug Macros
#define		RTC_DEBUG_EN			1
#if RTC_DEBUG_EN
#define		RTC_DEBUG_TRACE_EN		1
#define		RTC_DEBUG_EXCEPTION		1
#endif

#if RTC_DEBUG_TRACE_EN
#define		RTC_DEBUG_TRACE			0
#define		IGS_DEBUG_TRACE			0
#define		BSM_DEBUG_TRACE			0
#endif

#define		OUTPUT_PLAN_ENABLE		0

#define		INTL_DOSE_OVER_HIGHLIMIT	0x0000000800000000				//DoseRateVoerHighLimit
#define		INTL_DOSE_OVER_LOWLIMIT		0x0000001000000000   			//DoseRateOverLowLimit

//BGM interlock define
#define		INTL_DOSE_INTER1			0x0000000000400000				//Dose1 Symmetry
#define		INTL_DOSE_INTER6			0x0000000008000000				//Dose2 Symmetry
#define		INTL_DOSE_INTER10			0x0000000080000000				//Dose Dual Channel Exceed
/*==== CONST =================================================================*/
const UINT32 MAX_CP_TOTAL = 360;	//max control point number
const UINT32 MLC_AXIS_NUM = 128;	//mlc axis number
const FLOAT  EPSINON      = 0.00001f; // float  compare
const UINT32 GUI_COM_CHECK_INTERVAL = 1000;	//unit ms
/*==== ENUMS =================================================================*/
//GUI command define
enum{
	GUI_CMD_RESET		=		0x01,
	GUI_CMD_CLINICAL	=		0x02,
	GUI_CMD_PHYSICS		=		0x03,
	GUI_CMD_SERVICE		=		0x04,
	GUI_CMD_DAILY_QA	=		0x05,
	GUI_CMD_LOGIN		=		0x06,
	GUI_CMD_LOGOUT		=		0x07,
	GUI_CMD_NEXT_BEAM	=		0x08,

	GUI_CMD_CONTINUE		=	0x09,
	GUI_CMD_STOP_RADIATION	=	0x0a,
	GUI_CMD_CLOSE_PATIENT	=	0x0b,
	GUI_CMD_PREPARE			=   0x0c,
	GUI_CMD_RADIATION		=   0x0d,
	GUI_CMD_EXCEPTION_BEAMOFF=	0x0e,
	GUI_CMD_KV_IMAGE        =   0x0f,
	GUI_CMD_TCSNOTREADY     =   0x10,
	GUI_CMD_TCSREADY        =   0x11,
	GUI_CMD_OFFSTATE        =   0x12,
	GUI_CMD_SHUTDOWN        =   0x13,
	GUI_CMD_ClEANPLAN       =   0x14,
	GUI_CMD_IGSCT_NOTREADY  =   0x15,
	GUI_CMD_IGSCT_READY     =   0x16,

};

//OnGantry Intelocks Bit define
enum{
	INTL_GUI_COMM_ERROR                =0x0001,
	INTL_CANOPEN_MASTER_COMM_ERROR	   =0x0002,
	INTL_ETHERCAT_BSM_COMM_ERROR	   =0x0004,
	INTL_ETHERCAT_BGM_COMM_ERROR	   =0x0008,
	INTL_ETHERCAT_IGS_COMM_ERROR	   =0x0010,
	INTL_BSM_SUBSYSTEM_NOT_SYNC		   =0x0020,
	INTL_BGM_SUBSYSTEM_NOT_SYNC		   =0x0040,
	INTL_IGS_SUBSYSTEM_NOT_SYNC		   =0x0080,
	INTL_OGR_NOT_SYNC				   =0x0100,
};

//OnGantryRTC Unreadys Bit define
enum{
	UNREADY_CASE		= 0x01,		//case not received(GUI Service Mode->beam setting  GUI Treatment Mode->case recevie)
	TCS_NOTREADY		= 0X02,       //treatment  control   system    not ready
	PARAMETER_NOT_READY = 0x04,		//NGR parameter not ready
};

//EtherCAT state define
enum {
	ETHERCAT_OP_STATE = 8,					//normal state OP is 8
	CANOPEN_STATE = 0
};

//Dose interlock enable define
enum{
	DOSE_RATE_HIGH_INTL_EN = 0x01,
	DOSE_RATE_LOW_INTL_EN = 0x02
};

//Watch dog state define in Gui&Rtc protocal
enum{
	WD_NGR_OK = 0x01,
	WD_BSM_OK = 0x02,
	WD_BGM_OK = 0x04,
	WD_IGS_OK = 0x08,
	WD_RGS_OK = 0x10,
};

/*==== STRUCT ================================================================*/
//struct define for interlocks
struct Interlocks_t{
	UINT64 BgmInterlock;				//OnGantryRTC interlock
	UINT32 IgsInterlock;				//BGM interlock from EtherCAT communication
	UINT64 BsmInterlock;                //IGS interlock from EtherCAT communication
	UINT16 NGRInterlock;				//BSM interlock from EtherCAT communication
};

//struct define for Unready ----------------------------------------------------
struct Unreadys_t{
	UINT32 NGRUnready;
	UINT32 BgmUnready;					//BGM unready from EtherCAT communication 
	UINT32 BsmUnready;					//Bsm get to required state timeout
	UINT32 IgsUnready;					//Bsm get to required state timeout
};

//struct define for Terminate ----------------------------------------------------
struct Terminates_t{
	UINT32 NGRTerminate;
};
struct ResComponent_t{
	//Module Resources
	CTcTrace *Trace;
	RTC_StateMachineInputsFsm*			InputsFsm;
	RTC_StateMachineOutputsFsm*			OutputsFsm;
	RTC_StateMachineInputsSubFsm*		InputsSubFsm;
	RTC_StateMachineOutputsSubFsm*		OutputsSubFsm;
	RTC_StateMachineInputsOGR*			InputsOGR;
	RTC_StateMachineOutputsOGR*			OutputsOGR;
	RTC_StateMachineInputsBeam*			InputsBeam;
	RTC_StateMachineOutputsBeam*		OutputsBeam;
	RTC_StateMachineInputsDose*			InputsDose;
	RTC_StateMachineOutputsDose*		OutputsDose;
	RTC_StateMachinePlanInputs*			PlanInputs;
	RTC_StateMachinePlanOutputs*		PlanOutputs;
	RTC_StateMachineInputsBgmPlan*		InputsBgmPlan;
	RTC_StateMachineOutputsBgmPlan*		OutputsBgmPlan;
	RTC_StateMachineInputsIgsSlv*		InputsIgsSlv;
	RTC_StateMachineOutputsIgsSlv*		OutputsIgsSlv;
	RTC_StateMachineInputsBsm*			InputsBsm;
	RTC_StateMachineOutputsBsm*			OutputsBsm;
	RTC_StateMachineInputsVMAT*			InputsVMAT;
	RTC_StateMachineOutputsVMAT*		OutputsVMAT;
	RTC_StateMachineInputsCtrlState*	InputsCtrlState;
	RTC_StateMachineOutputsCtrlState*	OutputsCtrlState;
	RTC_StateMachineInputsOnGantryHD*   InputsOnGantryHD;
	RTC_StateMachineOutputsOnGantryHD*  OutputsOnGantryHD;
	RTC_StateMachineInputsBsmFmGUI*		InputsBsmFmGUI;
	RTC_StateMachineOutptsBsmToGUI*		OutputsBsmToGUI;
	RTC_StateMachineInputsIgs*			InputsIgs;
	RTC_StateMachineOutputsIgs*			OutputsIgs;
	RTC_StateMachineFmDevices*			FmDevices;
	RTC_StateMachineIUTMonitor*			IUTMonitor;
};

/*==== CLASS ================================================================*/
class CResourceMngr
{
public:
	CResourceMngr();
	~CResourceMngr();

	//FileIO resource
	ITcFileAccessPtr	m_PlanTxtPtr; //used for TXT
	TcFileHandle		hFile; //used for TXT
	//Interlock & Unreadys & Terminates
	
	Interlocks_t		Interlocks;
	Unreadys_t			Unreadys;
	Terminates_t		Terminates;
	//OnGantryRTC interlock&unready&terminate flag
	BYTE OGR_IUT;
	//Resource of control object
	CFsmResource		FsmResource;
	CPlanResource		PlanResource;
	CResourceBgm		BgmResource;
	CResourceIgs		IgsResource;
	CResourceBsm		BsmResource;
	//Tools
	CCrc32				ToolCrc32;

	BOOL				CycleUpdate(VOID);
	VOID				SetCurrentSysTime(LONGLONG Time);
	LONGLONG			GetCurrentSysTime(VOID);
	BOOL				SetResourceCom(ResComponent_t Resource);	//set component resource
	ResComponent_t*		GetResourceCom(VOID);						//get component resource pointer
	
private:
	//Method members
	VOID				Initialize(VOID);
	BOOL				UpdateInputResource(VOID);
	BOOL				UpdateOutputResource(VOID);
	VOID				UpdateInputTerminates(VOID);
	UINT8				GetCmd(UINT8 cmd);
	BOOL				SetCmdStruct(VOID);
	BOOL				ResourceIsReady(VOID);
	BOOL				SyncItuToOGR(VOID);
	BOOL				CheckHardwareState(VOID);
	VOID				CheckGuiCase(VOID);
	BOOL                CheckTcsReady(VOID);
	VOID				CheckParameterFile(VOID);
	BOOL				OutputToMonitor(VOID);
	VOID				OutputRadiationIndexToGUI();

	//Data members
	BOOL				m_InitEn;
	LONGLONG			m_CurrentSysTime;
	ResComponent_t		m_ResComponent;
	CSoftTimer			m_GuiComCheckTimer;
	BOOL				m_GuiComOk;
};

/*==== END OF FILE ===========================================================*/