/*sd_empty.cpp, test an empty system*/

#include "vxWorks.h"
#include "stdio.h"

#include "fw_roppc.h"
#include "fw_dcgvas.h"

#include "sd_emc.h"
#include "sd_emc_actions.h"
#include "sd_emc_runtasks.h"


//hi,jak

SEM_ID	g_sidTCBAInterrupt=NULL ;		/*waiting for TCBA interrupt*///byjak
UINT32	g_nMaxTCBADMABytes 	= 0 ;
UINT32 	g_nTCBADMAAdrs 			= 0 ;
//bye,jak
UINT32 	g_nTCheckAmpMode 			= 0 ;

struct emc_db_info  g_EMCDBInfo={0,NULL,0,{NULL},0,NULL,0,NULL,0,NULL,0,NULL};

unsigned int		g_nEmcDAC;

int 	nEMCGenL1TaskId=0;		//task for delete

int nMonitorStep = 0;	//for EMCmonitor
int nLEDCount = 0;

unsigned int QDisableGeo[16];
unsigned int nDisableIdx = 0;
	

void EMCLinkRtns()
{
	g_pfnInitializedLOAD 		= EMCInitializedLOAD ;
  	g_pfnLoadedPRCF 		= EMCLoadedPRCF ;
  	g_pfnPreconfigedCONF 	= EMCPreconfigedCONF;
  	g_pfnConfigedSYN1 		= EMCConfigedSYN1 ;
  	g_pfnSynchronized1SYN2 = EMCSynchronized1SYN2 ;
  	g_pfnSynchronized2SYN3 = EMCSynchronized2SYN3 ;
  	g_pfnSynchronized3SYN4 = EMCSynchronized3SYN4 ;
  	g_pfnSynchronized4PREP = EMCSynchronized4PREP ;
  	g_pfnReadySATR  		= EMCReadySATR;
  	g_pfnRunningSPTR  		= EMCRunningSPTR;
  	g_pfnReadySTOP  		= EMCReadySTOP;	
  	g_pfnConfigedUNCF 		= EMCConfigedUNCF;
  	g_pfnPreconfigedUNPC 	= EMCPreconfigedUNPC ;
  	g_pfnLoadedUNLD 		= EMCLoadedUNLD;
  	g_pfnAnyEXIT 			= EMCAnyEXIT;	
	

	g_pfnFWEnteringCbltTrans 	= EMCEnteringCbltTrans ;
	g_pfnFWBeforeInterrupt 		= EMCBeforeInterrupt ;
	g_pfnFWBeforeDMARun 		= EMCBeforeDMARun ;
	g_pfnFWAfterDMARun 		= EMCAfterDMARun ;

}

