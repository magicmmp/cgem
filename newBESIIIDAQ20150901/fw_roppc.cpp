/*fw_rdppc.cpp*/
#include "vxWorks.h"
#include "stdlib.h"
#include "taskLib.h"
#include "sysLib.h"
#include "rebootLib.h"

#include "fw_roppc.h"
#include "fw_dcgvas.h"
#include "fw_linkrtns.h"
#include "fw_actions.h"
#include "fw_filefunc.h"
#include "fw_staterrspt.h"
#include "fw_tcpfunc.h"
#include "fw_udpfunc.h"

#define 	SHOW_STATE  printf("	[%s]\n", g_strState[g_nCurnState])

void PPCStart()
{
	
	if (sysUnivIntDisable(0x00004000) == OK)//UNIVERSE_SYSFAIL_INT	 0x00004000
	{
		PPCbegin();
	}
	else
	{
		printf("can not disable sysfail interrupt\n");
	}
	return;
}

void PPCbegin()
{
	g_nCrateID = g_nPPCIDOfSRH ;

	
	if(g_nCrateID==0x1f)				//MDC 6u CRATE SPECIAL
	{
		g_nSysID=SYSID_MDC;
	}
	else
	{
		g_nSysID = (g_nPPCIDOfSRH & 0xe0) >> 5 ;
	}
	if((g_nSysID < SYSID_EMPTY) || (g_nSysID > MAX_SYS_AMOUNT - 1))
	{
		printf("Invalid SysID: %d\n", g_nSysID) ;
		return ;
	}

	char cLocalPPCIP[24];
	sprintf(cLocalPPCIP, "172.16.8.%d", g_nPPCIDOfSRH) ;

	//sprintf(cLocalPPCIP, "10.0.9.%d", g_nPPCIDOfSRH) ;

	if(-1 == ifAddrSet("er0", cLocalPPCIP))
	{
		printf("Setting IP to %s failed.\n", cLocalPPCIP) ;
		return ;
	}
	DEBUG_OUTPUT(("Crate ID: %d\n", g_nCrateID)) ;
	DEBUG_OUTPUT(("System ID: %d\n", g_nSysID)) ;
	DEBUG_OUTPUT(("PPC IP is: %s\n", cLocalPPCIP)) ;
	
	SetDftActions() ;	//establish actions table
	LinkRoutines() ;

	if(-1==StartUdpMonitor())		//see UDP FUNC,put here just spawn it once
	{
		DEBUG_OUTPUT(("StartUdpServ failed\n") );
	}
	
	if(-1 ==taskSpawn("tWaiting", PRI_WAITING, 0, TASK_STACK_SIZE, (FUNCPTR)Waiting, 
		0, 0, 0, 0, 0, 0, 0, 0, 0,0)   )
	{
		DEBUG_OUTPUT(("taskSpawn tWaiting failed\n") );
		return ;
	}
	
}

void Waiting()
{
	g_nCurnState = STID_Waiting;
	
	SHOW_STATE;

	
	if(-1 == CmdStatSockConnect())		//see TCP FUNC,muset before StateErrReportInit,so that can send info immediately
	{
		DEBUG_OUTPUT(("Setup Local Service&accept link failed\n") );
		CloseCmdStatSocket() ;				//see TCP FUNC
		return ;
	}

	if(-1==StateErrReportInit())			//see staterrspt
	{
		DEBUG_OUTPUT( ("call Error State Report init  failed\n") );
		return ;
	}
	
	if(-1 == taskSpawn("tDispatcher", PRI_DISPATCHER, 0, TASK_STACK_SIZE, (FUNCPTR)Dispatcher, 
		0, 0, 0, 0, 0, 0, 0, 0, 0,0) )
	{
		DEBUG_OUTPUT( ("taskSpawn tDispatcher failed\n") );
		Push2ErrStack(PPCERROR+FRM_FLG+ATOM_ERR+1,0,0,0,0);		/*error 1*/
		return ;
	}
}
void Dispatcher()
{
	int 	nRecv;
	int 	nCmd ;
	char	pcCmd[CMD_BYTES + 1] = {(CMD_BYTES + 1) * '\0'} ;
	int 	nRebootDelaySec = 1;
	int	nTmpState;
	DEBUG_OUTPUT(("Dispatcher started\n")) ;
	g_bShutdownOpt = 1;
	
	nTmpState = g_nCurnState ;
	g_nCurnState = STID_Initialized ;
	
	SHOW_STATE;
	while(1)
	{
		if((nRecv = Recv(g_sktCmdAck, pcCmd, CMD_BYTES)) > 0) 
		{
			DEBUG_OUTPUT(("socket recv %s\n", pcCmd)) ;
			nCmd=ParseCmd(pcCmd);
			if((nCmd>=0)&&(nCmd<=14))
			{
				DEBUG_OUTPUT(("recv %s\n", g_strCmd[nCmd])) ;
				nTmpState = g_nCurnState ;
				DEBUG_OUTPUT (("executing Actions[%d][%d](0) \n", g_nCurnState,nCmd)) ;
				if ( -1 == Actions[g_nCurnState][nCmd](0))	// transition failed
				{
					g_nCurnState = nTmpState ;
					DEBUG_OUTPUT (("Actions[%d][%d](0) failed \n", g_nCurnState, nCmd)) ;
					Push2ErrStack(PPCFATAL+FRM_FLG+STAT_ERR+2,g_nCurnState,nCmd,0,0);	/*error 2*/
				}
				
				SHOW_STATE;
			}
			else
			{
				SendAck(IMPO) ;
				Push2ErrStack(PPCWARNING+FRM_FLG+ATOM_ERR+3,0,0,0,0);		/*error 3*/
				DEBUG_OUTPUT(("unknown command!\n")) ;
				continue ;
			}	
		}
		else
		{
			DEBUG_OUTPUT (("Recv(g_sktCmdAck, pcCmd...)fail\n") );
			Push2ErrStack(PPCFATAL+FRM_FLG+ATOM_ERR+4,0,0,0,0);				/*error 4*/
			break ;
		}
	}
	if (g_bShutdownOpt)
	{
		g_bShutdownOpt = 0;
		fw_AnyEXIT(0);
	}
	
	printf("closed all socket ,and restart without reboot\n");
	return;
}
int ParseCmd(char * str)
{
	for(int index=0;index<15;index++)
	{
		if(strcmp(str,g_strCmd[index])==0)
		{
			return index;
		}
	}
	return index;
}

