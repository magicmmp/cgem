/*sd_empty.cpp, test an empty system*/

#include "vxWorks.h"
#include "stdio.h"

#include "fw_roppc.h"
#include "fw_dcgvas.h"

#include "sd_tof.h"
#include "sd_tof_actions.h"
#include "sd_tof_runtasks.h"

struct tof_db_info  g_TOFDBInfo={0,NULL,0,{NULL},0,NULL,0,NULL,0,NULL};

unsigned int	g_uTofCbltGeo[16];
unsigned int	g_uTofCbltModuleSum;
unsigned int	g_uTofMonitorGeo;
	
unsigned int 	g_uTofCalibSignAmp;

unsigned int 	g_uTofLDPulseInterval;
unsigned int 	g_uTofLDPulseNum;
unsigned int 	g_uTofLDSolenoid;

int 			nTOFGenL1TaskId=0;

/** for TDIG parameters**/

//int reject_cnt_offset ;/*48-59, data 2 bit 16-27*/
//int search_window ;/*60-71, data 2 bit 28-31, data 3 bit 0-7*/
//int match_window ;/*72-83, data 3 bit 8-19*/
//int trigger_latency ;/*138-149,data 5 bit 10-21*/
//int inlCorrection ;
//int configFPGA_enable;
//int interrupt_enable;
//unsigned int	TDIG_geo;
//int TDIG_boardEn;
//int TDIG_CBLTEn;
	

void TOFLinkRtns()
{
	g_pfnInitializedLOAD 		= TOFInitializedLOAD ;
  	g_pfnLoadedPRCF 		= TOFLoadedPRCF ;
  	g_pfnPreconfigedCONF 	= TOFPreconfigedCONF;
  	g_pfnConfigedSYN1 		= TOFConfigedSYN1 ;
  	g_pfnSynchronized1SYN2 = TOFSynchronized1SYN2 ;
  	g_pfnSynchronized2SYN3 = TOFSynchronized2SYN3 ;
  	g_pfnSynchronized3SYN4 = TOFSynchronized3SYN4 ;
  	g_pfnSynchronized4PREP = TOFSynchronized4PREP ;
  	g_pfnReadySATR  		= TOFReadySATR;
  	g_pfnRunningSPTR  		= TOFRunningSPTR;
  	g_pfnReadySTOP  		= TOFReadySTOP;	
  	g_pfnConfigedUNCF 		= TOFConfigedUNCF;
  	g_pfnPreconfigedUNPC 	= TOFPreconfigedUNPC ;
  	g_pfnLoadedUNLD 		= TOFLoadedUNLD;
  	g_pfnAnyEXIT 			= TOFAnyEXIT;	
	

	g_pfnFWEnteringCbltTrans 	= TOFEnteringCbltTrans ;
	g_pfnFWBeforeInterrupt 		= TOFBeforeInterrupt ;
	g_pfnFWBeforeDMARun 		= TOFBeforeDMARun ;
	g_pfnFWAfterDMARun 		= TOFAfterDMARun ;

}


