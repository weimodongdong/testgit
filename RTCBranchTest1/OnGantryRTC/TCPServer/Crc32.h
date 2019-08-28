/*******************************************************************************
** File	      : Crc32.h
** Author     : ChenLong
** Version    :	1.0
** Environment: VS2013
** Description:
** History    :	17/10/11    ChenLong    Created
				18/03/07    ChenLong	Modify naming format			
*******************************************************************************/
/*==== DECLARATION CONTROL ===================================================*/
#ifndef __CRC32_H__
#define __CRC32_H__

/*==== INCLUDES ==============================================================*/
#include <TcDef.h>
/*==== MACROS ================================================================*/
#define		CRC_TABLE_SIZE		256		//

/*==== FUNCTIONS =============================================================*/
/*==== CLASS =================================================================*/
class CCrc32 
{
public:
	CCrc32();
	~CCrc32();

	//variables
	UINT32 CrcTable[CRC_TABLE_SIZE];
	
	//functions
	BOOL InitCrc32Table(VOID);
	UINT32 Crc32Buffer(UCHAR *Buf, UINT32 Size);

//private:

};

#endif
/*==== END OF FILE ===========================================================*/