/*sd_empty.cpp, test an empty system*/

#include "vxWorks.h"
#include "stdio.h"

#include "fw_roppc.h"
#include "fw_dcgvas.h"

#include "fw_roppc.h"
#include "fw_dcgvas.h"
#include "sd_muon.h"
#include "sd_muon_actions.h"
#include "sd_muon_hw.h"
#include "sd_muon_runtasks.h"

struct muon_db_info  g_MUONDBInfo={0,NULL,0,{NULL},0,{NULL},0,NULL,0,{NULL} };
struct muon_hw_info g_MUONHWInfo={0,{0}};

SEM_ID g_sidMuonSpecial=NULL; /*leigkleigk 060820*/

unsigned short gMucEnablRomoGeo[10];
unsigned short gMucEnablModuleID[10];
unsigned short gMucDisableChainID[10];
unsigned short gMucExchangeChainID[10];

unsigned short MucMUDTBSEL[5];
unsigned short MucMUDTGeo[5];

bool b_TrgErrFst;
bool b_ChkErrFst;


void MUONLinkRtns()
{
	g_pfnInitializedLOAD 		= MUONInitializedLOAD ;
  	g_pfnLoadedPRCF 		= MUONLoadedPRCF ;
  	g_pfnPreconfigedCONF 	= MUONPreconfigedCONF;
  	g_pfnConfigedSYN1 		= MUONConfigedSYN1 ;
  	g_pfnSynchronized1SYN2 = MUONSynchronized1SYN2 ;
  	g_pfnSynchronized2SYN3 = MUONSynchronized2SYN3 ;
  	g_pfnSynchronized3SYN4 = MUONSynchronized3SYN4 ;
  	g_pfnSynchronized4PREP = MUONSynchronized4PREP ;
  	g_pfnReadySATR  		= MUONReadySATR;
  	g_pfnRunningSPTR  		= MUONRunningSPTR;
  	g_pfnReadySTOP  		= MUONReadySTOP;	
  	g_pfnConfigedUNCF 		= MUONConfigedUNCF;
  	g_pfnPreconfigedUNPC 	= MUONPreconfigedUNPC ;
  	g_pfnLoadedUNLD 		= MUONLoadedUNLD;
  	g_pfnAnyEXIT 			= MUONAnyEXIT;	
	

	g_pfnFWEnteringCbltTrans 	= MUONEnteringCbltTrans ;
	g_pfnFWBeforeInterrupt 		= MUONBeforeInterrupt ;
	g_pfnFWBeforeDMARun 		= MUONBeforeDMARun ;
	g_pfnFWAfterDMARun 		= MUONAfterDMARun ;

}


