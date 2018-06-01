/*fw_linkrtns.cpp, functions used to link the abstract function pointers to routines of specific systems*/
#include "vxWorks.h"

#include "fw_dcgvas.h"
#include "fw_actions.h"
#include "fw_linkrtns.h"

#include "sd_empty.h"
#include "sd_mdc.h"
#include "sd_emc.h"
#include "sd_muon.h"
#include "sd_tof.h"
#include "sd_trg.h"
#include "sd_zdd.h"
#include "sd_etf.h"


/*




#include "sd_tof.h"
#include "sd_trg.h"
*/

void SetDftActions()
{
	int i, j;
	for(i = STID_Waiting; i< MAX_STATES_AMOUNT; i ++)
	{
		for(j= CMID_LOAD; j< MAX_CMD_AMOUNT; j++)
		{
			Actions[i][j] = fw_AnyImpo;
		}
	}

	/*ensure that EXIT is the last command before doing so*/
	for(j = 1; j < MAX_STATES_AMOUNT; j ++)
	{
		Actions[j][CMID_EXIT] = fw_AnyEXIT ;
	}
	
	Actions[STID_Initialized][CMID_LOAD] 		= fw_InitializedLOAD ;
	Actions[STID_Loaded][CMID_LOAD] 			= fw_SelfTrans ;

	Actions[STID_Loaded][CMID_PRCF] 			= fw_LoadedPRCF;
	Actions[STID_PreConfiged][CMID_PRCF]		= fw_SelfTrans ;

	Actions[STID_PreConfiged][CMID_CONF]		= fw_PreconfigedCONF;
	Actions[STID_Configed][CMID_CONF]		= fw_SelfTrans ;

	Actions[STID_Configed][CMID_SYN1]			= fw_ConfigedSYN1;
	Actions[STID_Synchronized1][CMID_SYN1]	= fw_SelfTrans ;

	Actions[STID_Synchronized1][CMID_SYN2]	= fw_Synchronized1SYN2;
	Actions[STID_Synchronized2][CMID_SYN2]	= fw_SelfTrans ;
	
	Actions[STID_Synchronized2][CMID_SYN3]	= fw_Synchronized2SYN3;
	Actions[STID_Synchronized3][CMID_SYN3]	= fw_SelfTrans ;

	Actions[STID_Synchronized3][CMID_SYN4]	= fw_Synchronized3SYN4;
	Actions[STID_Synchronized4][CMID_SYN4]	= fw_SelfTrans ;

	Actions[STID_Synchronized4][CMID_PREP]	= fw_Synchronized4PREP;
	Actions[STID_Ready][CMID_PREP]			= fw_SelfTrans ;

	Actions[STID_Ready][CMID_SATR]			= fw_ReadySATR;
	Actions[STID_Running][CMID_SATR]			= fw_SelfTrans ;


	Actions[STID_Running][CMID_SPTR]			= fw_RunningSPTR;
	Actions[STID_Ready][CMID_SPTR]			= fw_SelfTrans ;

	Actions[STID_Ready][CMID_STOP]			= fw_ReadySTOP;
	Actions[STID_Configed][CMID_STOP]			= fw_SelfTrans ;

	Actions[STID_Configed][CMID_UNCF]			= fw_ConfigedUNCF;
	Actions[STID_PreConfiged][CMID_UNCF]		= fw_SelfTrans ;


	Actions[STID_PreConfiged][CMID_UNPC]		= fw_PreconfigedUNPC;
	Actions[STID_Loaded][CMID_UNPC] 			= fw_SelfTrans ;

	Actions[STID_Loaded][CMID_UNLD] 			= fw_LoadedUNLD;
	Actions[STID_Initialized][CMID_UNLD] 		= fw_SelfTrans ;
	
}

void LinkRoutines()
{
	switch(g_nSysID)
	{
		case SYSID_EMPTY:
			EMPTYLinkRtns() ;
			break ;
		case SYSID_EMC:	
			EMCLinkRtns() ;
			break ;
		case SYSID_MDC:
			MDCLinkRtns() ;
			break ;
		case SYSID_MUC:
			MUONLinkRtns() ;
			break ;
		case SYSID_TOF:
			TOFLinkRtns() ;
			break ;
		case SYSID_TRG:
			TRGLinkRtns() ;
			break ;
		case SYSID_ZDD:
			ZDDLinkRtns() ;
			break ;
		case SYSID_ETF:
			ETFLinkRtns() ;
			break ;			
		default:
			break ;
	}
}
