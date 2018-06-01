/*sd_empty.cpp, test an empty system*/

#include "vxWorks.h"
#include "stdio.h"

#include "fw_roppc.h"
#include "fw_dcgvas.h"

#include "sd_etf.h"
#include "sd_etf_actions.h"
#include "sd_etf_runtasks.h"

struct etf_db_info  g_ETFDBInfo={0,NULL,0,{NULL},0,NULL,0,NULL};

unsigned int	g_uEtfCbltGeo[MAXFEENUM];
unsigned int	g_uEtfCbltModuleSum;
unsigned int	g_uEtfMonitorGeo;
	
unsigned int 	g_uEtfCalibSignAmp;

unsigned int 	g_uEtfLDPulseInterval;
unsigned int 	g_uEtfLDPulseNum;
unsigned int 	g_uEtfLDSolenoid;

int 			nETFGenL1TaskId=0;

unsigned int g_nConverTable[MAXFEENUM][9][8];	

void ETFLinkRtns()
{
	g_pfnInitializedLOAD 		= ETFInitializedLOAD ;
  	g_pfnLoadedPRCF 		= ETFLoadedPRCF ;
  	g_pfnPreconfigedCONF 	= ETFPreconfigedCONF;
  	g_pfnConfigedSYN1 		= ETFConfigedSYN1 ;
  	g_pfnSynchronized1SYN2 = ETFSynchronized1SYN2 ;
  	g_pfnSynchronized2SYN3 = ETFSynchronized2SYN3 ;
  	g_pfnSynchronized3SYN4 = ETFSynchronized3SYN4 ;
  	g_pfnSynchronized4PREP = ETFSynchronized4PREP ;
  	g_pfnReadySATR  		= ETFReadySATR;
  	g_pfnRunningSPTR  		= ETFRunningSPTR;
  	g_pfnReadySTOP  		= ETFReadySTOP;	
  	g_pfnConfigedUNCF 		= ETFConfigedUNCF;
  	g_pfnPreconfigedUNPC 	= ETFPreconfigedUNPC ;
  	g_pfnLoadedUNLD 		= ETFLoadedUNLD;
  	g_pfnAnyEXIT 			= ETFAnyEXIT;	
	

	g_pfnFWEnteringCbltTrans 	= ETFEnteringCbltTrans ;
	g_pfnFWBeforeInterrupt 		= ETFBeforeInterrupt ;
	g_pfnFWBeforeDMARun 		= ETFBeforeDMARun ;
	g_pfnFWAfterDMARun 		= ETFAfterDMARun ;

}


