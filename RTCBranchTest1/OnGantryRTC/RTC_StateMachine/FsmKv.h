/*******************************************************************************
** File	      : FsnKv.h
** Author     : ChenLong
** Version    :	1.0
** Environment: VS2013
** Description:
** History    :	19/01/21    Chenlong    Created
*******************************************************************************/
/*==== DECLARATION CONTROL ===================================================*/
#pragma once

/*==== INCLUDES ==============================================================*/
#include "Fsm.h"
#include "ResourceMngr.h"

/*==== MACROS ================================================================*/
#define		KV_CHAIN_DELAY_TIME		1000					//unit:ms

/*==== FUNCTIONS =============================================================*/
/*==== CLASS =================================================================*/
class CFsmKv : public CFsm
{
public:
	CFsmKv();
	CFsmKv(CResourceMngr* Resource);
	virtual ~CFsmKv();
	virtual FsmState_u PrepareMain(VOID) override; //redefine prepare method
	virtual FsmState_u RadiationMain(VOID) override; //redefine radiation method
	virtual VOID CheckBgmFsmState(VOID) override; //redefine check bgm fsm method
	virtual VOID CheckBSMFsmState(VOID) override; //redefine check bsm fsm method
	virtual VOID CheckIgsFsmState(VOID) override; //redefine check igs fsm method
	virtual BOOL UpdateOutputResource(VOID) override; //redefine update output resource
private:
	VOID UpdateKvEnableChain(VOID);

	CSoftTimer m_KvChainDelayTimer;
};

