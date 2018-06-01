/*****sd_trg.cpp*****/

#include "vxWorks.h"
#include "stdio.h"

#include "fw_roppc.h"
#include "fw_dcgvas.h"

#include "sd_trg.h"
#include "sd_trg_actions.h"
#include "sd_trg_runtasks.h"

struct trg_db_info  g_TRGDBInfo={0,NULL,0,NULL,0,{NULL},0,NULL,0,NULL,0,NULL,0,NULL,0,NULL,0,NULL,
								0,NULL,0,NULL,0,{NULL},0,NULL,0,NULL,0,NULL,0,NULL,0,NULL,0,NULL,0,NULL};

UINT	uTrgCbltModGeo[16] = {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0};//jixl 071130
int 		nTrgCbltModNum= 0;
UINT 	uTrgTDCCbltAdrs = 0 ;

unsigned short TrgFCTLGeo;
unsigned short TrgFCTLBSEL;

unsigned short TrgTFCFGeo;
unsigned short TrgTFCFBSEL;

unsigned short TrgCLKFGeo;
unsigned short TrgCLKFBSEL;

unsigned short TrgTROCGeo;
unsigned short TrgTROCBSEL;

unsigned short TrgEaccGeo;
unsigned short TrgEaccBSEL;
unsigned short TrgEaccSYNCheck = 0;

unsigned short TrgSAFGeo[3];
unsigned short TrgSAFBSEL[3];

unsigned short TrgGTLGeo;
unsigned short TrgGTLBSEL;

unsigned short TrgTDCGeo;
unsigned short TrgTDCBSEL;

unsigned short TrgLTKCGeo;
unsigned short TrgLTKCBSEL;

unsigned short TrgSTKCGeo;
unsigned short TrgSTKCBSEL;	

unsigned short TrgTKFGeo[9];
unsigned short TrgTKFBSEL[9];
unsigned short TrgTKFSYNCheck[9];

unsigned short TrgMUTLGeo;
unsigned short TrgMUTLBSEL;

unsigned short TrgBASGGeo;
unsigned short TrgBASGBSEL;

unsigned short TrgTOFGeo;
unsigned short TrgTOFBSEL;
unsigned short TrgTOFCBLT;
unsigned short TrgTOFSYNCheck = 1;

unsigned short TrgETFGeo;
unsigned short TrgETFBSEL;


unsigned short TrgBTMGeo;
unsigned short TrgBTMBSEL;

unsigned short TrgETMGeo;
unsigned short TrgETMBSEL;

unsigned short TrgETSGeo;
unsigned short TrgETSBSEL;
unsigned short TrgETSCbltEnable = 0;

unsigned int etft_readout_window ;

void TRGLinkRtns()
{
	g_pfnInitializedLOAD 		= TRGInitializedLOAD ;
  	g_pfnLoadedPRCF 		= TRGLoadedPRCF ;
  	g_pfnPreconfigedCONF 	= TRGPreconfigedCONF;
  	g_pfnConfigedSYN1 		= TRGConfigedSYN1 ;
  	g_pfnSynchronized1SYN2 = TRGSynchronized1SYN2 ;
  	g_pfnSynchronized2SYN3 = TRGSynchronized2SYN3 ;
  	g_pfnSynchronized3SYN4 = TRGSynchronized3SYN4 ;
  	g_pfnSynchronized4PREP = TRGSynchronized4PREP ;
  	g_pfnReadySATR  		= TRGReadySATR;
  	g_pfnRunningSPTR  		= TRGRunningSPTR;
  	g_pfnReadySTOP  		= TRGReadySTOP;	
  	g_pfnConfigedUNCF 		= TRGConfigedUNCF;
  	g_pfnPreconfigedUNPC 	= TRGPreconfigedUNPC ;
  	g_pfnLoadedUNLD 		= TRGLoadedUNLD;
  	g_pfnAnyEXIT 			= TRGAnyEXIT;	
	

	g_pfnFWEnteringCbltTrans 	= TRGEnteringCbltTrans ;
	g_pfnFWBeforeInterrupt 		= TRGBeforeInterrupt ;
	g_pfnFWBeforeDMARun 		= TRGBeforeDMARun ;
	g_pfnFWAfterDMARun 		= TRGAfterDMARun ;

}


