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

#include "sd_zdd.h"
#include "sd_zdd_hw.h"
#include "sd_zdd_actions.h"
#include "sd_zdd_runtasks.h"

#include "sd_mdc.h"
#include "sd_mdc_hw.h"

//for some parameter to init ,et: MSG,RINGBUF
int ZDDParaCreate()
{
	for(int i=0;i<2;i++)
	{
		if((g_msgQZDD[i]=msgQCreate(MAXNUM_ZDDMSGS,sizeof(unsigned),MSG_Q_FIFO))==NULL)
		{
			DEBUG_OUTPUT(("Message Queue: cannot Create g_msgQZDD!\n"));
			Push2ErrStack(PPCERROR+ZDD_FLG+ATOM_ERR+11,0,0,0,0);		/*error 11*/
			return -1 ;
		}
		printf("MAXNUM_MSGS= %d \n",MAXNUM_MSGS);
	}
	
	for(i=0;i<2;i++)
	{
		if( (g_ZDDRng[i] = rng_dmaCreate(ZDDRNG_SIZE)) ==NULL)
		{
			DEBUG_OUTPUT(("Create g_ZDDRng failed\n")) ;
			Push2ErrStack(PPCERROR+ZDD_FLG+ATOM_ERR+13,0,0,0,0);		/*error 13*/
			return -1 ;
		}
	}
}

int ZDDParaFree()
{
	for(int i=0;i<2;i++)
	{
		if(g_msgQZDD[i]!=NULL)
		{
			if(-1 == msgQDelete(g_msgQZDD[i]))
			{
				DEBUG_OUTPUT(("msgQDelete(g_msgQZDD) failed.\n")) ;
				Push2ErrStack(PPCERROR+ZDD_FLG+ATOM_ERR+28,0,0,0,0);		/*error 28*/
				return -1;
			}
			g_msgQZDD[i]=NULL;
		}
	}

	for(i=0;i<2;i++)
	{
		if(g_ZDDRng[i]!=NULL)
		{
			if(-1 == rng_dmaDelete(g_ZDDRng[i]))
			{
				DEBUG_OUTPUT(("rng_dmaDelete(g_ZDDRng) failed.\n")) ;
				Push2ErrStack(PPCERROR+ZDD_FLG+ATOM_ERR+30,0,0,0,0);		/*error 30*/
				return -1;
			}
			g_ZDDRng[i]=NULL;
		}
	}
}

int	ZDDInitializedLOAD 		(int nArg) 
{
	char * pcMdlHeader ;
	int nMdlType;
	int nMdlIdx = 0 ;
	sysIntDisable(ZDD_INT_LEVEL) ;

	if((g_nCrateID<0xc0)||(g_nCrateID>=0xd0))
	{
		DEBUG_OUTPUT(("Zdd CrateID error : out of 0xc0-0xd0\n")) ;
		Push2ErrStack(PPCFATAL+ZDD_FLG+ATOM_ERR+1,0,0,0,0);		/*zdd error 1*/
		return -1;
	}
	
	//point to the data buff
	
	
	printf("\nModule Type: 0x%x\n", nMdlType) ;

	g_ZDDDBInfo.ppc_sum=0;
	g_ZDDDBInfo.v1721_sum=0;
	g_ZDDDBInfo.v1190_sum=0;
	g_ZDDDBInfo.mcc_sum = 1;
	g_ZDDDBInfo.mti_sum = 0;

	struct zdd_v1721* p_v1721_tmp=NULL;
	int nV1721GeoTmp = 0;
	struct zdd_v1190* p_v1190_tmp=NULL;
	int nV1190GeoTmp = 0;
	int nModuIDTmp=0;

	
	pcMdlHeader= ((char*)g_pCrateConfigHeader) + g_pCrateConfigHeader->crate_len ;
	for(nMdlIdx = 0; nMdlIdx < g_nMdlSum; nMdlIdx ++)
	{
		nMdlType =  (UINT)(*(UINT*)(pcMdlHeader + 4)) ;
		DEBUG_OUTPUT(("Module Type: 0x%x\n", nMdlType) ) ;
		switch(nMdlType)
		{
			case ZDD_PPC:
				g_ZDDDBInfo.ppc_header = (struct GE_ppc *)pcMdlHeader ;
				
				Show_ppc(g_ZDDDBInfo.ppc_header);
				
				g_ZDDDBInfo.ppc_sum++ ;
				if(g_ZDDDBInfo.ppc_sum>1)
				{
					DEBUG_OUTPUT(("PPC sum>1,zdd_ppc_sum=%d\n",g_ZDDDBInfo.ppc_sum ) );
					Push2ErrStack(PPCFATAL+ZDD_FLG+ATOM_ERR+2,0,0,0,0);		/*zdd error 2*/
					return -1;
				}

				break;

			case ZDD_V1721:


				p_v1721_tmp= (struct zdd_v1721*)pcMdlHeader ;
				nV1721GeoTmp=(p_v1721_tmp->Position)&0x1ff;	//get geo position

					g_ZDDDBInfo.v1721_header[g_ZDDDBInfo.v1721_sum]=p_v1721_tmp;

					g_ZDDDBInfo.v1721_header[g_ZDDDBInfo.v1721_sum]->Position = nV1721GeoTmp&0xff;

					gZddV1721Geo[g_ZDDDBInfo.v1721_sum] = nV1721GeoTmp&0xff;
					
					Show_zdd_v1721(g_ZDDDBInfo.v1721_header[g_ZDDDBInfo.v1721_sum]);
					g_ZDDDBInfo.v1721_sum++;
					
				break ;
			case ZDD_V1190:


				p_v1190_tmp= (struct zdd_v1190*)pcMdlHeader ;
				nV1190GeoTmp=(p_v1190_tmp->Position)&0x1ff;	//get geo position

					g_ZDDDBInfo.v1190_header[g_ZDDDBInfo.v1190_sum]=p_v1190_tmp;

					g_ZDDDBInfo.v1190_header[g_ZDDDBInfo.v1190_sum]->Position = nV1190GeoTmp&0xff;

					gZddV1190Geo[g_ZDDDBInfo.v1190_sum] = nV1190GeoTmp&0xff;
					
					Show_zdd_v1190(g_ZDDDBInfo.v1190_header[g_ZDDDBInfo.v1190_sum]);
					g_ZDDDBInfo.v1190_sum++;
					
				break ;				
			case ZDD_MCC:
				g_ZDDDBInfo.mcc_header = (struct mdc_mcc*)pcMdlHeader ;

				if((g_ZDDDBInfo.mcc_header->geo)>256)
				{
					g_ZDDDBInfo.mcc_header->geo&=0xff;
					Show_mdc_mcc(g_ZDDDBInfo.mcc_header);
					g_ZDDDBInfo.mcc_sum++;
				}
				
				if ( g_ZDDDBInfo.mcc_sum > 1)
				{
					DEBUG_OUTPUT(("ERROR: g_ZDDDBInfo.mcc_sum > 1. \n")) ;
					Push2ErrStack(PPCERROR+ZDD_FLG+ATOM_ERR+3,0,0,0,0);		/*zdd error 3*/
					return -1 ;
				}
				break ;
				
			case ZDD_MTI:
				g_ZDDDBInfo.mti_header = (struct mdc_mti*)pcMdlHeader ;

				if( (g_ZDDDBInfo.mti_header->geo)>256)
				{
					g_ZDDDBInfo.mti_header->geo&=0xff;
					Show_mdc_mti(g_ZDDDBInfo.mti_header);
					g_ZDDDBInfo.mti_sum++;
				}
				if ( g_ZDDDBInfo.mti_sum> 1)
				{
					DEBUG_OUTPUT(("ERROR: g_ZDDDBInfo.mti_sum > 1. \n")) ;
					Push2ErrStack(PPCERROR+ZDD_FLG+ATOM_ERR+4,0,0,0,0);		/*zdd error 4*/
					return -1 ;
				}
				break ;				
			default:
				printf("Unkown modules type.\n") ;
				Push2ErrStack(PPCFATAL+ZDD_FLG+ATOM_ERR+5,0,0,0,0);		/*zdd error 5*/
				return -1 ;
		}

		pcMdlHeader = pcMdlHeader + (UINT)(*(UINT*)pcMdlHeader) ;
		
	}
	/*check the ppcid between db and jump*/
	if (g_nPPCIDOfSRH!= g_ZDDDBInfo.ppc_header->PPC_id)
	{
		//tmp jixl 20140411
		//DEBUG_OUTPUT(("The PPC id in db (%d) is not equal to the one in SRH (%d).\n", g_ZDDDBInfo.ppc_header->PPC_id, g_nPPCIDOfSRH) );
		//Push2ErrStack(PPCFATAL+ZDD_FLG+ATOM_ERR+6,0,0,0,0);		/*zdd error 6*/
		//return -1 ;
	}

	g_ZDDDBInfo.v1721_sum = 2; //tmp jixl 1017
	g_ZDDDBInfo.v1190_sum = 1; //tmp jixl 20140227
	if(g_ZDDDBInfo.v1721_sum + g_ZDDDBInfo.v1190_sum)
	{	
		if(-1==ZDDConfIntCBLT())
		{
			DEBUG_OUTPUT(("Config interrupt or config CBLT error.\n"));
			Push2ErrStack(PPCFATAL+ZDD_FLG+ATOM_ERR+7,0,0,0,0);		/*zdd error 7*/
			return -1;
		}
		printf("After CBLTinit.\n") ;
		
	}
	

	//config MCC here
	g_ZDDDBInfo.mcc_header->geo = 7;
	g_ZDDDBInfo.mcc_header->T0_latency = 0;
	printf("g_ZDDDBInfo.mcc_sum = %d\n",g_ZDDDBInfo.mcc_sum);
	printf("g_ZDDDBInfo.mcc_header->geo = %d\n",g_ZDDDBInfo.mcc_header->geo);
	printf("g_ZDDDBInfo.mcc_header->T0_latency = %d\n",g_ZDDDBInfo.mcc_header->T0_latency);
	if(g_ZDDDBInfo.mcc_sum)
	{
		if(-1==MDCConfigMCCLoad(g_ZDDDBInfo.mcc_header->geo,g_ZDDDBInfo.mcc_header->T0_latency))
		{
			printf("ZDD config MCC in LOAD error\n") ;
			Push2ErrStack(PPCERROR+ZDD_FLG+ATOM_ERR+8,0,0,0,0);			/*zdd error 8*/
			return -1;
		}
		//MDCConfigMTILoad(g_ZDDDBInfo.mti_header->geo);
	}
	
	printf("ZDDInitializedLOAD OK\n") ;
	return 0;
}
int  	ZDDLoadedPRCF 			(int nArg) 
{
	
	if(g_ZDDDBInfo.v1721_sum)
	{
		unsigned short nDataPort = g_ZDDDBInfo.ppc_header->ROS_port ;
		unsigned int 	 nRemoteIP = g_ZDDDBInfo.ppc_header->ROS_ip;
		DEBUG_OUTPUT(("ROS IP = %x, PORT = %d.\n",nRemoteIP, nDataPort)) ;
		if(DataSockConnect(nRemoteIP,nDataPort)==-1)
		{
			DEBUG_OUTPUT(("connect data socket error\n"));
			Push2ErrStack(PPCFATAL+ZDD_FLG+ATOM_ERR+9,0,0,0,0);		/*zdd error 9*/
			CloseDataSocket();
			return -1;
		}
		printf("successfully connected on data socket.\n") ;
	}
	printf("ZDDLoadedPRCF OK\n") ;
	return 0;
}

int  	ZDDPreconfigedCONF 	(int nArg) 
{
	int nDBIdx ;
	int nMdlIdx ;
	bool bMatchFlag;

	if(-1==ZDDParaCreate())
	{
		return -1;
	}
	

	switch(g_nRunMode)
	{
		case NOMALMODE:
			if((g_nTrgPerInt<1)||(g_nTrgPerInt>16))
			{
				DEBUG_OUTPUT(("normal mode,TrgPerInt must equal 1-16.\n"));
				Push2ErrStack(PPCFATAL+ZDD_FLG+ATOM_ERR+10,0,0,0,0);		/*zdd error 10*/
				return -1;
			}
			break;
		case PEDMODE:
		case CALIBRATION:
		case MONITOR:
		case BASELINE:
		case WAVEMODE:
			DEBUG_OUTPUT(("ZDD system has not  NonPhysics mode.\n"));
			Push2ErrStack(PPCFATAL+ZDD_FLG+ATOM_ERR+11,0,0,0,0);		/*zdd error 11*/
			return -1;
		default:
			break;	
	}

	if(g_ZDDDBInfo.v1721_sum)
	{
		if(-1 == ZDDV1721Config())
		{
			printf("ZDDV1721ConfigAll failed\n") ;//need to reboot PPC
			Push2ErrStack(PPCFATAL+ZDD_FLG+ATOM_ERR+17,0,0,0,0);		/*zdd error 17*/
			return -1 ;
		}
		else
			printf("ZDDV1721ConfigAll succ\n") ;
	}
	if(g_ZDDDBInfo.v1190_sum)
	{
		if(-1 == ZDDV1190Config())
		{
			printf("ZDDV1190ConfigAll failed\n") ;
			Push2ErrStack(PPCFATAL+ZDD_FLG+ATOM_ERR+17,0,0,0,0);		/*zdd error 17*/
			return -1 ;
		}
		else
			printf("ZDDV1190ConfigAll succ\n") ;
	}	
/****/
	if(!g_ZDDDBInfo.ppc_header->NormModeSelfL1)
	{

	}	

	printf("ZDDPreconfiged OK\n") ;
	return 0;
}

int  	ZDDConfigedSYN1 		(int nArg) 
{	
	//set mcc calib mode for block L1 signal
	
	if(g_ZDDDBInfo.mcc_sum)
	{
		if(-1==MDCConfigMCCSyn1(g_ZDDDBInfo.mcc_header->geo,g_ZDDDBInfo.mcc_header->T0_latency))
		{
			DEBUG_OUTPUT(("ZDDConfigedSYN1 error\n")) ;
			Push2ErrStack(PPCERROR+ZDD_FLG+ATOM_ERR+12,0,0,0,0);		/*zdd error 12*/	
			return -1;
		}
	}
	
	DEBUG_OUTPUT(("ZDDConfigedSYN1 OK\n")) ;
	return 0;
}
int  	ZDDSynchronized1SYN2 	(int nArg) 
{
	DEBUG_OUTPUT(("ZDDSynchronized1SYN2 OK\n") );
	return 0;
}

int  	ZDDSynchronized2SYN3 	(int nArg) 
{
	if(g_ZDDDBInfo.mcc_sum)
	{
		printf("before MDCConfigMCCSyn3\n");
		if(-1==ZDDConfigMCCSyn3(g_ZDDDBInfo.mcc_header->geo,g_ZDDDBInfo.mcc_header->T0_latency))
		{
			DEBUG_OUTPUT(("ZDDSynchronized2SYN3 error\n")) ;
			Push2ErrStack(PPCERROR+ZDD_FLG+ATOM_ERR+13,0,0,0,0);		//zdd error 13	
			return -1;
		}
	}
	
	DEBUG_OUTPUT(("ZDDSynchronized2SYN3 OK\n")) ;
	return 0;
}

int  	ZDDSynchronized3SYN4 	(int nArg) 
{	
	DEBUG_OUTPUT(("ZDDSynchronized3SYN4 OK\n")) ;
	return 0;
}
int  	ZDDSynchronized4PREP 	(int nArg) 
{
	if(g_ZDDDBInfo.mcc_sum)
	{
		MDCMCCResetCheck((g_ZDDDBInfo.mcc_header->geo & 0xff)); //for reset check signal
		for(int i = 0; i < 500; i ++) ;
	}
	
	switch(g_nRunMode)
	{
		case NOMALMODE:
		case PEDMODE:
		case CALIBRATION:
		case MONITOR:
		case BASELINE:
		case WAVEMODE:
			if(g_ZDDDBInfo.v1721_sum)
			{

				if(-1 ==  (nCbltTaskId=taskSpawn("tFWCbltTrans", PRI_CBLTTRANS, 0, TASK_STACK_SIZE, (FUNCPTR)FWCbltTrans, 
										0, 0, 0, 0, 0, 0, 0, 0, 0, 0)))
				{
					DEBUG_OUTPUT(("taskSpawn tFWCbltTrans failed\n")) ;
					Push2ErrStack(PPCFATAL+ZDD_FLG+ATOM_ERR+14,0,0,0,0);		/*zdd error 14*/	
					return -1;
				}

				if(-1 ==  (nPackTaskId=taskSpawn("tFWNetTrans", PRI_DATAPACK, 0, TASK_STACK_SIZE, (FUNCPTR)FWNetTrans, 
										0, 0, 0, 0, 0, 0, 0, 0, 0, 0)))
				{
					DEBUG_OUTPUT(("taskSpawn tFWNetTrans failed\n")) ;
					Push2ErrStack(PPCFATAL+ZDD_FLG+ATOM_ERR+15,0,0,0,0);		/*zdd error 15*/	
					return -1;
				}

				if(-1 ==  (nNetTaskId=taskSpawn("tZDDDataPack", PRI_NETTRANS, 0, TASK_STACK_SIZE, (FUNCPTR)ZDDDataPack, 
										0, 0, 0, 0, 0, 0, 0, 0, 0, 0)))
				{
					DEBUG_OUTPUT(("taskSpawn tZDDDataPack failed\n")) ;
					Push2ErrStack(PPCFATAL+ZDD_FLG+ATOM_ERR+16,0,0,0,0);		/*zdd error 16*/	
					return -1;
				}
				
			}
			break ;	
		default:
			break ;
	}
	
 	//should be done before trigger system start trigger
	switch(g_nRunMode)
	{
		case NOMALMODE:
			if(g_ZDDDBInfo.v1721_sum)
			{
				ZDDStartTrig();
				taskDelay(1);	
			}
			break;			
		case PEDMODE:
		case CALIBRATION:
		case MONITOR:
		case BASELINE:
		case WAVEMODE:
			break;
		default:
			break ;
	}

	DEBUG_OUTPUT(("ZDDSynchronized4PREP OK\n")) ;
	return 0;
}
int  	ZDDReadySATR  			(int nArg)
{
 trigger_Diff = 0;
 trigger_Diff_last = 0;	
 
	DEBUG_OUTPUT(("ZDDReadySATR OK\n") );
	return 0;
}
int  	ZDDRunningSPTR  		(int nArg)
{
// for readout the data remain in buffer
/*
	taskDelay(1);
	for (int i=0;i<INTERRUPTEVENTNUM;i++)
	{
		semGive(g_sidInterrupt);
		printf("send a semaphore\n");
	}
	*/
	DEBUG_OUTPUT(("ZDDRunningSPTR OK\n")) ;
	return 0;
}

int  	ZDDReadySTOP  			(int nArg)
{
	//should be done after trigger system stopped trigger
	switch(g_nRunMode)
	{
		case NOMALMODE:
			if(g_ZDDDBInfo.v1721_sum)
			{
				ZDDStopTrig();
			}
			break;			
		case PEDMODE:
		case CALIBRATION:
		case MONITOR:
		case BASELINE:
		case WAVEMODE:
			break;
		default:
			break ;
	}
	
	DEBUG_OUTPUT(("ZDDReadySTOP OK\n") );
	return 0;
}

int  	ZDDConfigedUNCF 		(int nArg)
{
	if(-1==ZDDParaFree())
	{
		return -1;
	}
	

	DEBUG_OUTPUT(("ZDDConfigedUNCF OK\n")) ;
	return 0;
}
int  	ZDDPreconfigedUNPC 	(int nArg) 
{
	DEBUG_OUTPUT(("ZDDPreconfigedUNPC OK\n")) ;
	return 0;
}
int  	ZDDLoadedUNLD 			(int nArg)
{
	DEBUG_OUTPUT(("ZDDLoadedUNLD OK\n")) ;
	return 0;
}
int  	ZDDAnyEXIT 			(int nArg)
{
	if(-1==ZDDParaFree())
	{
		return -1;
	}

	DEBUG_OUTPUT(("ZDDInitializedEXIT OK\n")) ;
	return 0;
}
