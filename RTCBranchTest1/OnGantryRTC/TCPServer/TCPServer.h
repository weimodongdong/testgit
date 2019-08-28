///////////////////////////////////////////////////////////////////////////////
// TCPServer.h
#pragma once

#include "TCPServerInterfaces.h"
#include "Queue.h"
#include "TcSpinlock.h"
#include "Crc32.h"
#include "TcTimeConversion.h"
#include "SoftTimer.h"

/* == Customer Add ============================================================ */
/* -- Debug Defines ----------------------------------------------------------- */
#define		MC_COM_DEBUG_ENABLE			1
#if MC_COM_DEBUG_ENABLE
#define		MC_COM_DEBUG_FUN_RUN		0		//trace function run state
#define		MC_COM_DEBUG_EXCEPTION		1		//trace function run state
#define		MC_COM_DEBUG_COM_TRACE		0		//trace communiction state
#define		MC_COM_DEBUG_RECV_EVENT		0		//trace receive event
#define		MC_COM_DEBUG_RECV_DATA		0		//receive data function
#define		MC_COM_DEBUG_CRC			1		//enable debug crc
#endif

#if MC_COM_DEBUG_EXCEPTION
#define	MC_COM_DEBUG_EXP_WR				1
#define	MC_COM_DEBUG_EXP_RD				1
#endif

#if MC_COM_DEBUG_COM_TRACE
#define MC_COM_TRACE_WR					1
#define MC_COM_TRACE_RD					1
#endif

#define		AUTO_RESET_INTERVAL			1000		//uint ms
#define		AUTO_RESET_NUMBER			1		//

#define     DELAY_LISTEN_TIME			(1000*120)	//120s		unit:ms

/* -- Buffer Defines ---------------------------------------------------------- */
//buffer sized define
#define		MC_COM_SEND_BUF_SIZE		4096	//send buffer size
#define		MC_COM_RECV_BUF_SIZE		2048	//receive buffer size
#define		MC_COM_RECV_MAX_SIZE		2048	//used for receive data function
#define		MC_COM_POP_QUEUE_BUF_SIZE	2048	//temp buf size queue poped
//time define
#define		MC_AUTO_RESET_TIME			50		//cycle time
//read request flag bit define
#define		MC_UNIT_ID_MODULATOR_FlAG	0x00000001	//used for set or judge data unit 
#define		MC_UNIT_ID_GUN_FlAG			0x00000002	//used for set or judge data unit 
#define		MC_UNIT_ID_WATER_FlAG		0x00000004	//used for set or judge data unit 
#define		MC_UNIT_ID_AFC_FlAG			0x00000008	//used for set or judge data unit 
#define		MC_UNIT_ID_BEAM_FlAG		0x00000010	//used for set or judge data unit 
#define		MC_UNIT_ID_VACUUM_FlAG		0x00000020	//used for set or judge data unit 
#define		MC_UNIT_ID_GAS_FlAG			0x00000040	//used for set or judge data unit 
#define		MC_UNIT_ID_GEOMETRY_FlAG	0x00000080	//used for set or judge data unit
#define		MC_UNIT_ID_INTL_FlAG		0x00000100	//used for set or judge data unit
#define		MC_UNIT_ID_UNREADY_FlAG		0x00000200	//used for set or judge data unit
#define		MC_UNIT_ID_SYS_STATUS_FlAG	0x00000400	//used for set or judge data unit
#define		MC_UNIT_ID_EVENT_FlAG		0x00000800	//used for set or judge data unit
#define		MC_UNIT_ID_CTRL_STATE_FlAG	0x00001000	//used for set or judge data unit
#define		MC_UNIT_ID_BSM_FlAG			0x00002000	//used for set or judge data unit
#define		MC_UNIT_ID_DOSE_FlAG		0x00004000	//used for set or judge data unit
#define		MC_UNIT_ID_IGS_FlAG		    0x00008000	//used for set or judge data unit
#define		MC_UNIT_ID_IGSICU_FlAG		0x00010000	//used for set or judge data unit
#define		MC_UNIT_ID_IGSEXPOSE_GANTRY_LIST_FlAG	0x00020000	//used for set or judge data unit

/* -- Network Defines -------------------------------------------------------- */
/* -- Communication Protocal Defines ----------------------------------------- */
// header of packet
#define		MC_COM_PACKAGE_START_POS	0		//package header start postion
#define		MC_COM_HEADER_SIZE			5		//size of package header 
//sender/receiver identifier
#define		MC_MACHINE_ID_UM			0x01	//identifier:up  machine
#define		MC_MACHINE_ID_LM			0x02	//identifier:low machine 
//request type
#define		MC_READ_REQUEST				0x01	//up machine read request
#define		MC_WRITE_REQUEST			0x02	//up machine write request
#define		MC_READ_FEEDBACK			0x11	//up machine read feedback
#define		MC_WRITE_FEEDBACK			0x12	//up machine write feedback
//tail of packet
#define		MC_COM_STOP_BYTE			0xff	//stop byte

// TCLV struct define - 'T' data unit identifier define
/*------------------------------------  RTC  --------------------------------------- */
//read request identifier
#define		MC_UNIT_ID_GEOMETRY			0x00	//geometry-mlc postions
#define		MC_UNIT_ID_INTERLOCK		0x01	//safe interlock indicate
#define		MC_UNIT_ID_UNREADY			0x02	//unready
#define		MC_UNIT_ID_SYS_STATUS		0x03	//system status
#define		MC_UNIT_ID_EVENT			0x04	//event
#define		MC_UNIT_ID_CTRL_STATE		0x05	//controller state

//write request identifier
#define		MC_UNIT_ID_CMD				0x81	//up machine command
#define		MC_UNIT_ID_EMULAOTR_EN		0x82	//emulator enable
#define		MC_UNIT_ID_BEAM_WR			0x83	//beam parameters
#define		MC_UNIT_ID_PLANBEAM_WR		0x84	//transmit plan beam
#define		MC_UNIT_ID_PLANCP_WR		0x85	//transmit plan cp
#define		MC_UNIT_ID_CTRL_STATE_WR	0x86	//controller state setting
#define		MC_UNIT_ID_VMAT_PARA_WR		0x87	//VMAT parameters setting

/*------------------------------------  BGM  --------------------------------------- */
//read request identifier
#define		MC_UNIT_ID_MODULATOR		0x10	//modulator
#define		MC_UNIT_ID_COOLING			0x11	//water coolling
#define		MC_UNIT_ID_GUN				0x12	//gun
#define		MC_UNIT_ID_AFC				0x13	//afc
#define		MC_UNIT_ID_BEAM				0x14	//beam
#define		MC_UNIT_ID_VACUUM			0x15	//vacuum -> not used now
#define		MC_UNIT_ID_GAS				0x16	//gas -> not used now
#define     MC_UNIT_ID_DOSE				0x17	//dose
//write request identifier
#define		MC_UNIT_ID_MODULATOR_WR		0x90	//modulator
#define		MC_UNIT_ID_COOLING_WR		0x91	//water coolling(not used now)
#define		MC_UNIT_ID_GUN_WR			0x92	//gun
#define		MC_UNIT_ID_AFC_WR			0x93	//afc non-realtime data
#define		MC_UNIT_ID_AFC_CTL_WR		0x94	//afc realtime data
#define		MC_UNIT_ID_DOSE_WR			0x95	//dose
#define		MC_UNIT_ID_PRF_WR			0x96	//set prf
#define		MC_UNIT_ID_BSM_WR			0xfd	//bsm

/*-------------------------------------  MLC  --------------------------------------- */
#define		CP_CRC_ERROR				0xfe
#define		EXCEPT_CRC					0		//
#define		EXCEPT_MISS_MIDDLE			2
#define		EXCEPT_MISS_LAST			3

/*-------------------------------------  BSM  --------------------------------------- */
#define		BSM_CP_TOTAL				180
#define		BSM_CP_SHAFT_POS			128
#define		BSM_CRC_ERROR				0xff
#define		BSM_BEAM_ID_ERROR			0xfe
#define		BSM_SEGMENT_NUM				89		//bsm case data segment 0-89
#define		MC_UNIT_ID_BSM				0x7f	//bsm


/*-------------------------------------  IGS  --------------------------------------- */
// read request identifier
#define		MC_UNIT_ID_IGS			    0x50
#define		MC_UNIT_ID_IGSICU			0x51
#define		MC_UNIT_ID_IGSEXPOSE_GANTRY_LIST   0x53

//write request identifier
#define		MC_UNIT_ID_IGS_WR				0xd0	
#define		MC_UNIT_ID_IGSICU_WR		    0xd1	



// TCLV struct define - 'C' control byte define
#define		MC_FUNC_RESERVE				0x00	//function reserve define

// TCLV struct define - 'L' length define
//unit length define (uint byte)
#define		MC_UNIT_MODULATOR_LEN		78		//modulator read response TLV--L
#define		MC_UINT_MODULAOTR_WR_LEN	16		//modulator write request TLV--L
#define		MC_UNIT_GUN_LEN				18		//gun read response TLV--L
#define		MC_UINT_GUN_WR_LEN			148		//gun write request TLV--L
#define		MC_UNIT_WATER_LEN			66		//water read response TLV--L
#define		MC_UNIT_AFC_LEN				12		//afc read response TLV--L
#define		MC_UINT_AFC_WR_LEN			28		//afc write request TLV--L
#define		MC_UINT_AFC_CTL_WR_LEN		10		//afc control write request TLV--L
#define		MC_UNIT_BEAM_LEN			26		//beam read response TLV--L
#define		MC_UINT_BEAM_WR_LEN			16		//beam write request TLV--L
#define		MC_UINT_CMD_LEN				8		//up machine command TLV--L
#define		MC_UINT_PRF_LEN				58		//up machine command TLV--L
#define     MC_UNIT_DOSE_LEN			6		//dose read response TLV--L
#define		MC_UINT_DOSE_WR_LEN			54		//dose write request TLV--L
#define		MC_UNIT_INTERLOCK_LEN		22		//interlock TLV--L -> auto upload
#define		MC_UNIT_SYS_STATUS_LEN		3		//system status TLV--L -> auto upload
#define		MC_UNIT_UNREADY_LEN			4		//unready TLV--L -> auto upload
#define		MC_UNIT_VACUUM_LEN			8		//vacuum TLV--L
#define		MC_UNIT_GAS_LEN				4		//vacuum TLV--L
#define		MC_UNIT_BSM_LEN				567		//bsm read response TLV--L
#define		MC_UINT_BSM_WR_LEN			105		//bsm write request TLV--L
#define		MC_UINT_CP_WR_LEN			565		//cp write request TLV--L
#define		MC_UINT_PLANBEAM_WR_LEN		46		//plan beam write request TLV--L
#define		MC_UNIT_MLC_LEN				1008	//bsm read response TLV--L
#define		MC_UNIT_EVENT_LEN			1		//event read response TLV--L
#define		MC_UINT_EMULATOR_LEN		8		//emulator write request TLV--L
#define     MC_UNIT_CTRL_STATE_LEN		3		//controller state read request TLV--L
#define     MC_UNIT_CTRL_STATE_WR_LEN	2		//controller state write request TLV--L
#define     MC_UNIT_VMAT_PARA_WR_LEN	80		//VMAT parameters write request TLV--L
#define     MC_UNIT_IGS_WR_LEN	        48		//Igs  write request TLV--L
#define     MC_UNIT_IGSICU_WR_LEN	    19		//Igs  icu write request TLV--L
#define     MC_UNIT_IGS_LEN			    48		//Igs read response TLV--L
#define     MC_UNIT_IGSICU_LEN			11		//Igs icu read response TLV--L
#define     MC_UNIT_IGSEXPOSE_GANTRY_LIST_LEN	466//Igs  expose list read response TLV--L

/* -- Union Defines ----------------------------------------------------------------- */
//union define
//Note: program platform is x86(little endian)
union float_union_t
{
	struct
	{
		UCHAR lw_low_byte;		//low word--low byte
		UCHAR lw_high_byte;		//low word--high byte
		UCHAR hw_low_byte;		//high word--low byte
		UCHAR hw_high_byte;		//high word--high byte
	}float_byte;
	float value;
};

union uint16_union_t
{
	struct
	{
		UCHAR low_byte;			//low byte
		UCHAR high_byte;		//high byte
	}uint16_byte;
	UINT16 value;
};

union int16_union_t
{
	struct
	{
		UCHAR low_byte;			//low byte
		UCHAR high_byte;		//high byte
	}int16_byte;
	INT16 value;
};

union uint32_union_t
{
	struct
	{
		UCHAR lw_low_byte;		//low word--low byte
		UCHAR lw_high_byte;		//low word--high byte
		UCHAR hw_low_byte;		//high word--low byte
		UCHAR hw_high_byte;		//high word--high byte
	}uint32_byte;
	UINT32 value;
};

union int32_union_t
{
	struct
	{
		UCHAR lw_low_byte;		//low word--low byte
		UCHAR lw_high_byte;		//low word--high byte
		UCHAR hw_low_byte;		//high word--low byte
		UCHAR hw_high_byte;		//high word--high byte
	}int32_byte;
	INT32 value;
};

/* -- Data Sutruct Define ----------------------------------------------------------- */
//Struct define
//packet header of read/write response
struct PackageHead_s{
	union uint16_union_t SequenceNum;
	UINT8	SenderID;
	UINT8	ReceiverID;
	UINT8	RequestType;
};

//packet tail of read/write response
struct PackageTail_s{
	UINT8	StopFlag;
};

//interlock indicator
struct IntlIndicator_s{
	BYTE	Indicator1;
	BYTE	Indicator2;
	BYTE	Indicator3;
	BYTE	Indicator4;
};

//plan cp
struct PlanOneCP_t
{
	UINT32 CPIndex;
	FLOAT  BeamEnergy;
	FLOAT  DoseWeight;
	FLOAT  MlcPos[128];
	FLOAT  MlcAngle;
	FLOAT  GantryAngle;
	FLOAT  PssAngle;
	FLOAT  TableVerPos;
	FLOAT  TableLonPos;
	FLOAT  TableLatPos;
	FLOAT  TablePitchAngle;
	FLOAT  TableRollAngle;
	BYTE   MlcDir;
	BYTE   GantryDir;
	BYTE   PssDir;
	BYTE   TablePitchDir;
	BYTE   TableRollDir;
};

struct PlanExceptionCtrl{
	BOOL CrcErrorEn;
	BOOL MissMiddleEn;
	BOOL MissLastEn;
};

/* -- Enum Define ----------------------------------------------------------- */
//const variables
enum
{
	PREPARE_STATE_AUTH = 0x0c,
	RADIATION_STATE_AUTH = 0x0d
};
/* ==End===================================================================== */

class CTCPServer 
	: public ITComObject
	, public ITcADI
	, public ITcWatchSource
///<AutoGeneratedContent id="InheritanceList">
	, public ITcCyclic
	, public ITcIoTcpProtocolRecv
///</AutoGeneratedContent>
{
public:
	DECLARE_IUNKNOWN()
	DECLARE_IPERSIST(CID_TCPServerCTCPServer)
	DECLARE_ITCOMOBJECT_LOCKOP()
	DECLARE_ITCADI()
	DECLARE_ITCWATCHSOURCE()
	DECLARE_OBJPARAWATCH_MAP()
	DECLARE_OBJDATAAREA_MAP()

	CTCPServer();
	virtual	~CTCPServer();

	/* ==Customer Add============================================================ */
	/* --Variables----------------------------------------------------------------*/
	ULONG		ClientSocket;								//linsten socket for client to communication
	UINT16		SequenceNum;						
	UCHAR		SendBuffer[MC_COM_SEND_BUF_SIZE];	
	UCHAR		RecvBuffer[MC_COM_RECV_BUF_SIZE];
	UCHAR		DequeueBuf[MC_COM_POP_QUEUE_BUF_SIZE];
	UINT16		SendBufferCursor;							//decide postion to fill data
	UINT32      CycleCounter;								//used for reset module output
	LONGLONG	CurrentSysTime;								//ms uint since 1. January 1601 (UTC)
	SYSTEMTIME  FormatSysTime;								//formatted system time


	BYTE       m_IgsExpListGroup;                           // GUI reads gantry position and dose data for which RTc group is requested  
	/* --Structs----------------------------------------------------------------*/
	IntlIndicator_s   IntlIndicator;							//interlock indicator struct
	PlanExceptionCtrl m_PlanExceptionCtrl;					//plan exception control 

	/* --Class-----------------------------------------------------------------*/
	 CSpinlock	Spinlock;									//spinlock
	 CQueue		CycleQueue;									//buffer to store read response data
	 CQueue		WriteQueue;									//buffer to store write response data
	 CCrc32		Crc32;
	
	/* --Functions------------------------------------------------------------*/
	// logic handle layer functions
	BOOL	GetCurSysTime(ITcTask* ipTask);
	BOOL    ResetModuleOutput(VOID);
	BOOL	ResponseRequest(VOID);

	// data handle layer functions
	BOOL	ResponseGuiRequest(VOID);
	BOOL	ResponseReadReq(UCHAR *DequeueBuf, UINT32 Flags);
	BOOL	ResponseWriteReq(UCHAR *DequeueBuf);
	BOOL	ResponseWriteReqModulator(UCHAR *DequeueBuf, UINT16 *Postion);
	BOOL	ResponseWriteReqGun(UCHAR *DequeueBuf, UINT16 *Postion);
	BOOL	ResponseWriteReqAfc(UCHAR *DequeueBuf, UINT16 *Postion);
	BOOL	ResponseWriteReqAfcCtl(UCHAR *DequeueBuf, UINT16 *Postion);
	BOOL	ResponseWriteReqDose(UCHAR *DequeueBuf, UINT16 *Postion);
	BOOL	ResponseWriteReqBeam(UCHAR *DequeueBuf, UINT16 *Postion);
	BOOL	ResponseWriteReqCmd(UCHAR *DequeueBuf, UINT16 *Postion);
	BOOL	ResponseWriteReqPrf(UCHAR *DequeueBuf, UINT16 *Postion);
	UINT8   ResponseWriteReqCP(UCHAR *DequeueBuf, UINT16 *Postion);
	UINT8	ResponseWriteReqPlanBeam(UCHAR *DequeueBuf, UINT16 *Postion);
	BOOL	ResponseWriteReqEmulator(UCHAR *DequeueBuf, UINT16 *Postion);
	BOOL    ResponseWriteReqCtrlState(UCHAR *DequeueBuf, UINT16 *Postion);
	BOOL    ResponseWriteReqBsm(UCHAR *dequeue_buf, UINT16 *postion);
	BOOL    ResponseWriteReqVMAT(UCHAR *dequeue_buf, UINT16 *postion);
	BOOL    ResponseWriteReqIgs(UCHAR *dequeue_buf, UINT16 *postion);
	BOOL    ResponseWriteReqIgsIcu(UCHAR *dequeue_buf, UINT16 *postion);

	BOOL	GetReadReqIdentifier(UCHAR *DequeueBuf, UINT16 Num, UINT32 *Flags);

	BOOL	PackageHeader(PackageHead_s PackageHead, UCHAR *SendBuf, UINT16 *Postion);
	BOOL	PackageBody(UCHAR DataUnit, UCHAR *SendBuf, UINT16 *Postion);
	BOOL	PackageBodyModulator(UCHAR *SendBuf, UINT16 *Postion);
	BOOL	PackageBodyGun(UCHAR *SendBuf, UINT16 *Postion);
	BOOL	PackageBodyWater(UCHAR *SendBuf, UINT16 *Postion);
	BOOL	PackageBodyAfc(UCHAR *SendBuf, UINT16 *Postion);
	BOOL	PackageBodyBeam(UCHAR *SendBuf, UINT16 *Postion);
	BOOL	PackageBodyInterlock(UCHAR *SendBuf, UINT16 *Postion);
	BOOL	PackageBodySysStatus(UCHAR *SendBuf, UINT16 *Postion);
	BOOL	PackageBodyUnready(UCHAR *SendBuf, UINT16 *Postion);
	BOOL    PackageBodyCtrlState(UCHAR *SendBuf, UINT16 *Postion);
	BOOL	PackageBodyGeometry(UCHAR *SendBuf, UINT16 *Postion);
	BOOL	PackageBodyEvent(UCHAR *SendBuf, UINT16 *Postion);
	BOOL    PackageBodyBsm(UCHAR *sendbuf, UINT16 *postion);
	BOOL    PackageBodyDose(UCHAR *sendbuf, UINT16 *postion);
	BOOL    PackageBodyIgs(UCHAR *sendbuf, UINT16 *postion);
	BOOL    PackageBodyIgsIcu(UCHAR *sendbuf, UINT16 *postion);
	BOOL    PackageBodyIgsExposeGantryList(UCHAR *sendbuf, UINT16 *postion);
	BOOL	PackageTail(PackageTail_s PackageTailer, UCHAR *SendBuf, UINT16 *Postion);
	// overloaded functions
	BOOL	ConvertModubsFormat(UINT16 *Src, UCHAR *Dst, UINT16 *DstPostion, UINT32 Number);
	BOOL	ConvertModubsFormat(INT16 *Src, UCHAR *Dst, UINT16 *Postion, UINT32 Number);
	BOOL	ConvertModubsFormat(UINT32 *Src, UCHAR *Dst, UINT16 *Postion, UINT32 Number);
	BOOL	ConvertModubsFormat(INT32 *Src, UCHAR *Dst, UINT16 *Postion, UINT32 Number);
	BOOL	ConvertModubsFormat(FLOAT *Src, UCHAR *Dst, UINT16 *Postion, UINT32 Number);

	// basic service functions
	BOOL	ListenSocket(VOID);
	/* ==End===================================================================== */

///<AutoGeneratedContent id="InterfaceMembers">
	// ITcCyclic
	virtual HRESULT TCOMAPI CycleUpdate(ITcTask* ipTask, ITcUnknown* ipCaller, ULONG_PTR context);

	// ITcIoTcpProtocolRecv
	virtual HRESULT TCOMAPI ReceiveData(ULONG socketId, ULONG nData, PVOID pData);
	virtual HRESULT TCOMAPI ReceiveEvent(ULONG socketId, TCPIP_EVENT tcpEvent);

///</AutoGeneratedContent>

protected:
	DECLARE_ITCOMOBJECT_SETSTATE();

	HRESULT AddModuleToCaller();
	VOID RemoveModuleFromCaller();

	// Tracing
	CTcTrace m_Trace;

///<AutoGeneratedContent id="Members">
	TcTraceLevel m_TraceLevelMax;
	TCPServerParameter m_Parameter;
	USHORT m_TcpServerPort;
	TCPServerInputsModulator m_InputsModulator;
	TCPServerOutputsModulator m_OutputsModulator;
	TCPServerInputsGun m_InputsGun;
	TCPServerOutputsGun m_OutputsGun;
	TCPServerInputsWater m_InputsWater;
	TCPServerOutputsWater m_OutputsWater;
	TCPServerInputsDose m_InputsDose;
	TCPServerOutputsDose m_OutputsDose;
	TCPServerInputsAfc m_InputsAfc;
	TCPServerOutputsAfc m_OutputsAfc;
	TCPServerInputsBeam m_InputsBeam;
	TCPServerOutputsBeam m_OutputsBeam;
	TCPServerInputsVac m_InputsVac;
	TCPServerOutputsVac m_OutputsVac;
	TCPServerInputsSF6 m_InputsSF6;
	TCPServerOutputsSF6 m_OutputsSF6;
	TCPServerInputsPrf m_InputsPrf;
	TCPServerOutputsPrf m_OutputsPrf;
	TCPServerInputsFsm m_InputsFsm;
	TCPServerOutputsFsm m_OutputsFsm;
	TCPServerPlanInputs m_PlanInputs;
	TCPServerPlanOutputs m_PlanOutputs;
	TCPServerInputsHalState m_InputsHalState;
	TCPServerOutputsHalState m_OutputsHalState;
	TCPServerInputsBsm m_InputsBsm;
	TCPServerOutputsBsm m_OutputsBsm;
	TCPServerInputsVMAT m_InputsVMAT;
	TCPServerOutputsVMAT m_OutputsVMAT;
	TCPServerInputsIgs m_InputsIgs;
	TCPServerOutputsIgs m_OutputsIgs;
	ITcCyclicCallerInfoPtr m_spCyclicCaller;
	ITcIoTcpProtocolPtr m_spTcpProt;
///</AutoGeneratedContent>

	// TODO: Custom variable
	ULONG nSentData; //used for report of sent packets
	CHAR m_response[100]; //for printing the answer
	ULONG m_SockId;
	CSoftTimer m_AutoResetTimer;
	UINT32 m_AutoResetCounter;
	CSoftTimer m_DelayListenTimer;
};
