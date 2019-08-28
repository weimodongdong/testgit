/*******************************************************************************
** File	      : Queue.cpp
** Author:    : ChenLong
** Version    :	1.0
** Environment: VS2013
** Description:
** History    :	16/11/10    Chenlong    Created
				18/03/07    ChenLong	Modify naming format	
*******************************************************************************/
/* INCLUDE ===================================================================*/
#include "Queue.h"

/* GLOBAL VARS================================================================*/
/* FUNCTIONS =================================================================*/
/******************************************************************************
* Function: CQueue()
*
* Describe: queue construct function
*
* Input   : None
*
* Output  : None
*
* Return  : 
*
* Others  :
*******************************************************************************/
CQueue::CQueue()
{
	memset(QueueBuf, 0, sizeof(QueueBuf));
	QueueHead = 0;
	QueueTail = 0;
	QueueUsage = 0;
}

CQueue::~CQueue()
{
	
}

/******************************************************************************
* Function: IsEmpty()
*
* Describe: judge whether queueu empty
*
* Input   : None
*
* Output  : None
*
* Return  : QUEUE_EMPTY or QUEUE_NONEMPTY
*
* Others  :
*******************************************************************************/
BOOL CQueue::IsEmpty()
{
	if (QueueHead == QueueTail)	//head == tail
	{
		#if QUEUE_DEBUG_TRACE
			//client_obj->m_Trace.Log(tlError, "Queue: Line%d: Queue is empty", __LINE__);
		#endif

		return QUEUE_EMPTY;
	}

	return QUEUE_NONEMPTY;
}

/******************************************************************************
* Function: IsFull(VOID)
*
* Describe: judge whether queueu full
*
* Input   : None
*
* Output  : None
*
* Return  : QUEUE_FULL or QUEUE_NONFULL
*
* Others  :
*******************************************************************************/
BOOL CQueue::IsFull()
{
	/*tail + 1 == head*/
	if ((QueueTail + 1) % QUEUE_BUF_LEN == QueueHead)
	{
		#if QUEUE_DEBUG_TRACE
			//client_obj->m_Trace.Log(tlError, "Queue: Line%d: Queue is full", __LINE__);
		#endif

		return QUEUE_FULL;
	}

	return QUEUE_NONFULL;
}

/******************************************************************************
* Function: GetFreeSpace(VOID)
*
* Describe: query free space of queue
*
* Input   : None
*
* Output  : None
*
* Return  : 0 or QUEUE_BUF_LEN-1 or count
*
* Others  :
*******************************************************************************/
UINT32 CQueue::GetFreeSpace(VOID)
{
	UINT32 Count = 0;
	UINT32 TempTail = QueueTail;	//local variable

	if (QUEUE_FULL == IsFull())	//queue full
	{
		return Count;
	}

	if (QUEUE_EMPTY == IsEmpty())//queue empty,return QUEUE_BUF_LEN-1
	{
		return QUEUE_BUF_LEN - 1;
	}

	#if 0
		/*get amount of free space*/
		while (((temp_tail + 1) % QUEUE_BUF_LEN) != modulator_queue_head)
		{
			temp_tail = (temp_tail + 1) % QUEUE_BUF_LEN;
			count++;
		}
	#endif

		Count = QUEUE_MAX_SIZE - (QueueTail - QueueHead + QUEUE_BUF_LEN) % QUEUE_BUF_LEN;

	return Count;
}

/******************************************************************************
* Function: ReleaseSpace(UINT16 *FreeSpace, UINT16 Len)
*
* Describe: release free space
*
* Input   : len
*
* Output  : free_space
*
* Return  : QUEUE_OK
*
* Others  :
*******************************************************************************/
BOOL CQueue::ReleaseSpace(UINT16 *FreeSpace, UINT16 Len)
{
	UINT16 size = 0;

	/*cycle release space that head point to*/
	while (Len + 2  > * FreeSpace)
	{
		GetFramebytes(&size, QueueHead);
		QueueHead = ((QueueHead + size + 2)) % QUEUE_BUF_LEN;	//change footnote to release space(head dont need add 1)
		*FreeSpace += size + 2;													//update value of free_space
	}

	return QUEUE_OK;
}

/******************************************************************************
* Function: GetFramebytes(UINT16 *Size, UINT32 TempHead)
*
* Describe: get size of current frame data head point to
*
* Input   : temp_head
*
* Output  : size
*
* Return  : size
*
* Others  :
*******************************************************************************/
UINT16 CQueue::GetFramebytes(UINT16 *Size, UINT32 TempHead)
{
	/*2 char transform to 1 short*/
	*Size |= QueueBuf[TempHead];
	*Size <<= 8;
	TempHead = (TempHead + 1) % QUEUE_BUF_LEN;
	*Size |= QueueBuf[TempHead];

	return *Size;
}

/******************************************************************************
* Function: StoreData(UCHAR *databuf, UINT16 len)
*
* Describe: store data to queue
*
* Input   : databuf, len
*
* Output  : None
*
* Return  : QUEUE_OK
*
* Others  :
*******************************************************************************/
BOOL CQueue::StoreData(UCHAR *DataBuf, UINT16 Len)
{
	UINT32 i;

	/*store len into queue*/
	QueueBuf[QueueTail] = (UCHAR)(Len >> 8);
	QueueTail = (QueueTail + 1) % QUEUE_BUF_LEN;

	QueueBuf[QueueTail] = (UCHAR)(Len);
	QueueTail = (QueueTail + 1) % QUEUE_BUF_LEN;
	/*store data into queue*/
	for (i = 0; i < Len; i++)
	{
		QueueBuf[QueueTail] = DataBuf[i];
		QueueTail = (QueueTail + 1) % QUEUE_BUF_LEN;
	}

	return QUEUE_OK;
}

/******************************************************************************
* Function: Enqueue(UCHAR *DataBuf, UINT16 Len)
*
* Describe: push data into queue
*
* Input   : databuf, len
*
* Output  : None
*
* Return  : QUEUE_OK
*
* Others  :
*******************************************************************************/
BOOL CQueue::Enqueue(UCHAR *DataBuf, UINT16 Len)
{
	UINT16 FreeSpace = 0;

	/*check availablity of input parameters*/
	if (Len > QUEUE_BUF_LEN - 3)				//max size of available space is QUEUE_BUF_LEN - 3
	{
		#if QUEUE_DEBUG
			cout << "enqueue:Parameter is invailed\n";
		#elif QUEUE_DEBUG_TRACE
			//client_obj->m_Trace.Log(tlError, "Queue: Line%d: Parameter is invailed", __LINE__);
		#endif

		return QUEUE_FAIL;
	}
	/*get free space*/
	FreeSpace = GetFreeSpace();

	/*judge free space enough*/
	if (Len + 2 > FreeSpace)					//len value takes 2 bytes
	{
		#if QUEUE_DEBUG_TRACE
			//client_obj->m_Trace.Log(tlError, "Queue: Line%d: Release queue space", __LINE__);
		#endif
		//queue_release_space(&free_space, len);	//free space

		return QUEUE_FAIL;
	}

	/*store data into queue*/
	StoreData(DataBuf, Len);

	/*update queue usage value*/
	QueueUsage = (QUEUE_MAX_SIZE - GetFreeSpace()) / QUEUE_MAX_SIZE;

	return QUEUE_OK;
}

/******************************************************************************
* Function: Dequeue(UCHAR *DataBuf)
*
* Describe: popping data from queue
*
* Input   : databuf
*
* Output  : None
*
* Return  : size
*
* Others  :
*******************************************************************************/
UINT32 CQueue::Dequeue(UCHAR *DataBuf)
{
	UINT16 Size = 0;
	UINT32 i;

	if (QUEUE_EMPTY == IsEmpty())
	{
		return Size;
	}
	/*get length of current data frame*/
	GetFramebytes(&Size, QueueHead);
	/*drop length bytes of data frame*/
	QueueHead = (QueueHead + 1) % QUEUE_BUF_LEN;
	QueueHead = (QueueHead + 1) % QUEUE_BUF_LEN;
	/*copy data to sdatabuf*/
	for (i = 0; i < Size; i++)
	{
		DataBuf[i] = QueueBuf[QueueHead];
		QueueHead = (QueueHead + 1) % QUEUE_BUF_LEN;
	}

	/*update queue usage value*/
	QueueUsage = (QUEUE_MAX_SIZE - GetFreeSpace())/QUEUE_MAX_SIZE;

	return Size;
}