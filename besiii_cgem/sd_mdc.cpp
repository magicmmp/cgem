
#include "vxWorks.h"

#include "fw_roppc.h"
#include "fw_dcgvas.h"
#include "sd_mdc.h"
#include "sd_mdc_actions.h"
#include "sd_mdc_runtasks.h"
#include "sd_mdc_hw.h"

struct mdc_db_info  	g_MDCDBInfo = {0, NULL, 
								0, {NULL},{0},
								0, NULL, 
								0, NULL,
								0,NULL
								};

struct mdc_hw_info 	g_MDCHWInfo = {0, {0}} ;
int 					g_nMqtEnableSum;
int					g_nMqtEnableGeo[MDC_MQT_AMOUNT+1];

/*for calibration  leigk061027*/
unsigned int 		g_nMQTDAC;
unsigned int		g_nMCCT0Latency;

int 	nMDCGenL1TaskId=0;		//task for delete
	
void MDCLinkRtns()
{
  	g_pfnInitializedLOAD 		= MDCInitializedLOAD ;
  	g_pfnLoadedPRCF 		= MDCLoadedPRCF ;
  	g_pfnPreconfigedCONF 	= MDCPreconfigedCONF;
  	g_pfnConfigedSYN1 		= MDCConfigedSYN1 ;
  	g_pfnSynchronized1SYN2 = MDCSynchronized1SYN2 ;
  	g_pfnSynchronized2SYN3 = MDCSynchronized2SYN3 ;
  	g_pfnSynchronized3SYN4 = MDCSynchronized3SYN4 ;
  	g_pfnSynchronized4PREP = MDCSynchronized4PREP ;
  	g_pfnReadySATR  		= MDCReadySATR;
  	g_pfnRunningSPTR  		= MDCRunningSPTR;
  	g_pfnReadySTOP  		= MDCReadySTOP;	
  	g_pfnConfigedUNCF 		= MDCConfigedUNCF;
  	g_pfnPreconfigedUNPC 	= MDCPreconfigedUNPC ;
  	g_pfnLoadedUNLD 		= MDCLoadedUNLD;
  	g_pfnAnyEXIT 			= MDCAnyEXIT;	
	

	g_pfnFWEnteringCbltTrans 	= MDCEnteringCbltTrans ;
	g_pfnFWBeforeInterrupt 		= MDCBeforeInterrupt ;
	g_pfnFWBeforeDMARun 		= MDCBeforeDMARun ;
	g_pfnFWAfterDMARun 		= MDCAfterDMARun ;
	
}
