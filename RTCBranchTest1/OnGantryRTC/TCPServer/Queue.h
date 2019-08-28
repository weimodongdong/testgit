/*******************************************************************************
** File	      : Queue.h
** Author     : ChenLong
** Version    :	1.0
** Environment: VS2013
** Description: 
** History    :	16/11/10    Chenlong    Created
				18/03/07    ChenLong	Modify naming format	
*******************************************************************************/
/*==== DECLARATION CONTROL ===================================================*/
#ifndef __QUEUE_H__
#define __QUEUE_H__

/*==== INCLUDES ==============================================================*/
#include <basetsd.h>
#include <ntdef.h>
#include <TcDef.h>

/*==== MACROS ================================================================*/
/*debug switch*/
#define QUEUE_DEBUG_EXCEPTION			1		//macro switch for exception message
#define QUEUE_DEBUG						0
#define QUEUE_DEBUG_TRACE				0

/*function return value*/
#define QUEUE_OK						0
#define QUEUE_FAIL						1

#define QUEUE_EMPTY						0
#define QUEUE_NONEMPTY					1

#define QUEUE_FULL						0
#define QUEUE_NONFULL					1

/*queue buf*/
#define QUEUE_BUF_LEN					2048
#define QUEUE_MAX_SIZE					QUEUE_BUF_LEN - 1
//#define QUEUE_BUF_LEN		2048

/*==== FUNCTIONS =============================================================*/
/*==== CLASS =================================================================*/
class CQueue{
public:
	
	CQueue();
	~CQueue();

	/*variables*/
	UCHAR  QueueBuf[QUEUE_BUF_LEN];
	UINT16 QueueHead;						//head out
	UINT16 QueueTail;						//tail in
	UINT8  QueueUsage;						//queue usage

	/*functions*/
	BOOL   Enqueue(UCHAR *DataBuf, UINT16 Len);
	UINT32 Dequeue(UCHAR *DataBuf);
	BOOL   IsEmpty(VOID);

protected:
	BOOL   IsFull(VOID);
	UINT32 GetFreeSpace(VOID);
	BOOL   ReleaseSpace(unsigned short *Free_space, unsigned short Len);
	UINT16 GetFramebytes(UINT16 *Size, UINT32 TempHead);
	BOOL   StoreData(UCHAR *DataBuf, UINT16 Len);
};

#endif
/*==== END OF FILE ===========================================================*/