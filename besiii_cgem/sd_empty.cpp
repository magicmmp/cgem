/*sd_empty.cpp, test an empty system*/

#include "vxWorks.h"
#include "stdio.h"

#include "fw_roppc.h"
#include "fw_dcgvas.h"

#include "sd_empty.h"
#include "sd_empty_actions.h"
#include "sd_empty_runtasks.h"

void EMPTYLinkRtns()
{
	g_pfnInitializedLOAD 		= EMPTYInitializedLOAD ;
  	g_pfnLoadedPRCF 		= EMPTYLoadedPRCF ;
  	g_pfnPreconfigedCONF 	= EMPTYPreconfigedCONF;
  	g_pfnConfigedSYN1 		= EMPTYConfigedSYN1 ;
  	g_pfnSynchronized1SYN2 = EMPTYSynchronized1SYN2 ;
  	g_pfnSynchronized2SYN3 = EMPTYSynchronized2SYN3 ;
  	g_pfnSynchronized3SYN4 = EMPTYSynchronized3SYN4 ;
  	g_pfnSynchronized4PREP = EMPTYSynchronized4PREP ;
  	g_pfnReadySATR  		= EMPTYReadySATR;
  	g_pfnRunningSPTR  		= EMPTYRunningSPTR;
  	g_pfnReadySTOP  		= EMPTYReadySTOP;	
  	g_pfnConfigedUNCF 		= EMPTYConfigedUNCF;
  	g_pfnPreconfigedUNPC 	= EMPTYPreconfigedUNPC ;
  	g_pfnLoadedUNLD 		= EMPTYLoadedUNLD;
  	g_pfnAnyEXIT 			= EMPTYAnyEXIT;	
	

	g_pfnFWEnteringCbltTrans 	= EMPTYEnteringCbltTrans ;
	g_pfnFWBeforeInterrupt 		= EMPTYBeforeInterrupt ;
	g_pfnFWBeforeDMARun 		= EMPTYBeforeDMARun ;
	g_pfnFWAfterDMARun 		= EMPTYAfterDMARun ;

}


