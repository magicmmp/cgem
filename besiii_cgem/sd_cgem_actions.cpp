

#include "vxWorks.h"
#include "5100_test.h"
#include "stdio.h"
#include "taskLib.h"
#include "sysLib.h"
#include "usrLib.h"

#include "fw_roppc.h"
#include "fw_dcgvas.h"
#include "fw_define.h"
#include "fw_filefunc.h"
#include "fw_tcpfunc.h"
#include "fw_runtasks.h"
#include "fw_staterrspt.h"
#include "fw_configinfo.h"

#include "sd_cgem.h"
#include "sd_cgem_cfg.h"
#include "sd_cgem_actions.h"
#include "sd_cgem_runtasks.h"


int	cgemInitializedLOAD 		(int nArg)
{

	printf("cgemInitializedLOAD OK\n") ;
	return 0;
}
int  	cgemLoadedPRCF 			(int nArg)
{
    struct GE_ppc * pcMdlHeader ;
    pcMdlHeader= (struct GE_ppc *)(((char*)g_pCrateConfigHeader) + g_pCrateConfigHeader->crate_len +4);
/**    unsigned short   nDataPort = pcMdlHeader->ROS_port ;
    unsigned int 	 nRemoteIP = pcMdlHeader->ROS_ip;
  **/  
    unsigned short   nDataPort = 9240 ;
    unsigned int     nRemoteIP = inet_addr("192.168.1.220");
    
    if(DataSockConnect(nRemoteIP,nDataPort)==-1)
    {
      DEBUG_OUTPUT(("connect data socket error\n"));
      Push2ErrStack(PPCFATAL+MUC_FLG+ATOM_ERR+6,0,0,0,0);		/*muc error 6*/
      CloseDataSocket();
      return -1;
    }
    printf("successfully connected on data socket.\n") ;
    
	DEBUG_OUTPUT(("cgemLoadedPRCF OK\n")) ;
	return 0;
}

int  	cgemSynchronized2SYN3 	(int nArg)
{
    cgem();
	printf("cgemPreconfiged OK\n") ;
	return 0;
}
int  	cgemConfigedSYN1 		(int nArg)
{

	DEBUG_OUTPUT(("cgemConfigedSYN1 OK\n")) ;
	return 0;
}
int  	cgemSynchronized1SYN2 	(int nArg)
{
	DEBUG_OUTPUT(("cgemSynchronized1SYN2 OK\n") );
	return 0;
}

int  	cgemPreconfigedCONF 	(int nArg)
{
	DEBUG_OUTPUT(("cgemSynchronized2SYN3 OK\n")) ;
	return 0;
}

int  	cgemSynchronized3SYN4 	(int nArg)
{
	DEBUG_OUTPUT(("cgemSynchronized3SYN4 OK\n")) ;
	return 0;
}
int  	cgemSynchronized4PREP 	(int nArg)
{

    int taskid_1;
    BufSem = semBCreate(SEM_Q_FIFO , SEM_FULL );
    semTake(BufSem, WAIT_FOREVER) ;
    taskid_1=taskSpawn("UDP_recv", PRI_CBLTTRANS, 0, TASK_STACK_SIZE, (FUNCPTR)udp_recv,0, 0, 0, 0, 0, 0, 0, 0, 0, 0);


				if(-1 ==  (nCbltTaskId=taskSpawn("tFWCbltTrans", PRI_CBLTTRANS, 0, TASK_STACK_SIZE, (FUNCPTR)FWCbltTrans,
										0, 0, 0, 0, 0, 0, 0, 0, 0, 0)))
				{
					DEBUG_OUTPUT(("taskSpawn tFWCbltTrans failed\n")) ;
					Push2ErrStack(PPCFATAL+MUC_FLG+ATOM_ERR+37,0,0,0,0);
					return -1;
				}

				if(-1 ==  (nPackTaskId=taskSpawn("tFWNetTrans", PRI_DATAPACK, 0, TASK_STACK_SIZE, (FUNCPTR)FWNetTrans,
										0, 0, 0, 0, 0, 0, 0, 0, 0, 0)))
				{
					DEBUG_OUTPUT(("taskSpawn tFWNetTrans failed\n")) ;
					Push2ErrStack(PPCFATAL+MUC_FLG+ATOM_ERR+38,0,0,0,0);
					return -1;
				}

				if(-1 ==  (nNetTaskId=taskSpawn("tMUONDataPack", PRI_NETTRANS, 0, TASK_STACK_SIZE, (FUNCPTR)cgemDataPack,
										0, 0, 0, 0, 0, 0, 0, 0, 0, 0)))
				{
					DEBUG_OUTPUT(("taskSpawn tMUONDataPack failed\n")) ;
					Push2ErrStack(PPCFATAL+MUC_FLG+ATOM_ERR+39,0,0,0,0);
					return -1;
				}


	DEBUG_OUTPUT(("cgemSynchronized4PREP OK\n")) ;
	return 0;
}
int  	cgemReadySATR  			(int nArg)
{

	DEBUG_OUTPUT(("cgemReadySATR OK\n") );
	return 0;
}
int  	cgemRunningSPTR  		(int nArg)
{

	DEBUG_OUTPUT(("cgemRunningSPTR OK\n")) ;
	return 0;
}
int  	cgemReadySTOP  			(int nArg)
{


	DEBUG_OUTPUT(("cgemReadySTOP OK\n") );
	return 0;
}
int  	cgemConfigedUNCF 		(int nArg)
{
	DEBUG_OUTPUT(("cgemConfigedUNCF OK\n")) ;
	return 0;
}
int  	cgemPreconfigedUNPC 	(int nArg)
{
	DEBUG_OUTPUT(("cgemPreconfigedUNPC OK\n")) ;
	return 0;
}
int  	cgemLoadedUNLD 			(int nArg)
{
	DEBUG_OUTPUT(("cgemLoadedUNLD OK\n")) ;
	return 0;
}
int  	cgemAnyEXIT 			(int nArg)
{

	DEBUG_OUTPUT(("cgemInitializedEXIT OK\n")) ;
	return 0;
}
