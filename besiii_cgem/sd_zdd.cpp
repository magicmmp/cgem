/*sd_empty.cpp, test an empty system*/

#include "vxWorks.h"
#include "stdio.h"

#include "fw_roppc.h"
#include "fw_dcgvas.h"

#include "fw_roppc.h"
#include "fw_dcgvas.h"
#include "sd_zdd.h"
#include "sd_zdd_actions.h"
#include "sd_zdd_hw.h"
#include "sd_zdd_runtasks.h"

struct zdd_db_info  g_ZDDDBInfo={0,NULL,0,{NULL},0,{NULL},0,NULL,0,NULL};
struct zdd_hw_info g_ZDDHWInfo={0,{0},0,{0}};

SEM_ID g_sidZddSpecial=NULL; 

unsigned int delta_triggerNum;

unsigned short gZddV1721Geo[ZDD_V1721_AMOUNT];
unsigned short gZddV1190Geo[ZDD_V1190_AMOUNT];


UINT32 CustomOut;
UINT32 CompressionMode;
UINT32 NoSupDataLen;


bool b_singleBoard;	

DMA_RING_ID	g_ZDDRng[2];		
MSG_Q_ID	g_msgQZDD[2];

UINT trigger_Diff = 0;
UINT trigger_Diff_last = 0;


void ZDDLinkRtns()
{
	g_pfnInitializedLOAD 		= ZDDInitializedLOAD ;
  	g_pfnLoadedPRCF 		= ZDDLoadedPRCF ;
  	g_pfnPreconfigedCONF 	= ZDDPreconfigedCONF;
  	g_pfnConfigedSYN1 		= ZDDConfigedSYN1 ;
  	g_pfnSynchronized1SYN2 = ZDDSynchronized1SYN2 ;
  	g_pfnSynchronized2SYN3 = ZDDSynchronized2SYN3 ;
  	g_pfnSynchronized3SYN4 = ZDDSynchronized3SYN4 ;
  	g_pfnSynchronized4PREP = ZDDSynchronized4PREP ;
  	g_pfnReadySATR  		= ZDDReadySATR;
  	g_pfnRunningSPTR  		= ZDDRunningSPTR;
  	g_pfnReadySTOP  		= ZDDReadySTOP;	
  	g_pfnConfigedUNCF 		= ZDDConfigedUNCF;
  	g_pfnPreconfigedUNPC 	= ZDDPreconfigedUNPC ;
  	g_pfnLoadedUNLD 		= ZDDLoadedUNLD;
  	g_pfnAnyEXIT 			= ZDDAnyEXIT;	
	

	g_pfnFWEnteringCbltTrans 	= ZDDEnteringCbltTrans ;
	g_pfnFWBeforeInterrupt 		= ZDDBeforeInterrupt ;
	g_pfnFWBeforeDMARun 		= ZDDBeforeDMARun ;
	g_pfnFWAfterDMARun 		= ZDDAfterDMARun ;

}


