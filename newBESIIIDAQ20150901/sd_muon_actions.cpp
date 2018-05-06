/*sd_empty.cpp, test an empty system*/

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

#include "sd_muon.h"
#include "sd_muon_hw.h"
#include "sd_muon_actions.h"
#include "sd_muon_runtasks.h"


int	MUONInitializedLOAD 		(int nArg) 
{
	char * pcMdlHeader ;
	int nMdlType;
	int nMdlIdx = 0 ;
	sysIntDisable(MUON_INT_LEVEL) ;

	if((g_nCrateID<0x80)||(g_nCrateID>=0xA0))
	{
		DEBUG_OUTPUT(("Muon CrateID out of 0x80-0xA0\n")) ;
		Push2ErrStack(PPCFATAL+MUC_FLG+ATOM_ERR+1,0,0,0,0);		/*muc error 1*/

		return -1;
	}
	
	//point to the data buff
	
	
	printf("\nModule Type: 0x%x\n", nMdlType) ;

	g_MUONDBInfo.ppc_sum=0;
	g_MUONDBInfo.enabl_romo_sum=0;
	g_MUONDBInfo.disabl_romo_sum=0;
	g_MUONDBInfo.roc_sum=0;
	g_MUONDBInfo.mudt_sum=0;
	int iRomo = 0;

	struct muon_romo* p_romo_tmp=NULL;
	int nRomoGeoTmp = 0;
	int nModuIDTmp=0;
	int imudt = 0;

	
	pcMdlHeader= ((char*)g_pCrateConfigHeader) + g_pCrateConfigHeader->crate_len ;
	for(nMdlIdx = 0; nMdlIdx < g_nMdlSum; nMdlIdx ++)
	{
		nMdlType =  (UINT)(*(UINT*)(pcMdlHeader + 4)) ;
		DEBUG_OUTPUT(("Module Type: 0x%x\n", nMdlType) ) ;
		switch(nMdlType)
		{
			case MUON_PPC:
				g_MUONDBInfo.ppc_header = (struct GE_ppc *)pcMdlHeader ;
				
				Show_ppc(g_MUONDBInfo.ppc_header);
				
				g_MUONDBInfo.ppc_sum++ ;
				if(g_MUONDBInfo.ppc_sum>1)
				{
					DEBUG_OUTPUT(("PPC sum>1,muc_ppc_sum=%d\n",g_MUONDBInfo.ppc_sum ) );
					Push2ErrStack(PPCFATAL+MUC_FLG+ATOM_ERR+2,0,0,0,0);		/*muc error 2*/
					return -1;
				}

				break;

			case MUON_ROMO:

				p_romo_tmp= (struct muon_romo*)pcMdlHeader ;
				nRomoGeoTmp=(p_romo_tmp->Position)&0x1ff;	//get geo position
				if(nRomoGeoTmp>256)
				{
					g_MUONDBInfo.enabl_romo_header[g_MUONDBInfo.enabl_romo_sum]=p_romo_tmp;
					
					gMucEnablModuleID[g_MUONDBInfo.enabl_romo_sum] = (p_romo_tmp->Position)>>12;
					gMucEnablRomoGeo[g_MUONDBInfo.enabl_romo_sum] = nRomoGeoTmp&0xff;
					gMucDisableChainID[g_MUONDBInfo.enabl_romo_sum] = (p_romo_tmp->ChainDisable) & 0xf;
					gMucExchangeChainID[g_MUONDBInfo.enabl_romo_sum] = ( (p_romo_tmp->ChainDisable)>>4 )& 0xf;
					g_MUONDBInfo.enabl_romo_header[g_MUONDBInfo.enabl_romo_sum]->Position = nRomoGeoTmp&0xff;
					//save geo infor only now
					
					Show_muon_romo(g_MUONDBInfo.enabl_romo_header[g_MUONDBInfo.enabl_romo_sum]);
					g_MUONDBInfo.enabl_romo_sum++;
				}
				else
				{
					g_MUONDBInfo.disabl_romo_header[g_MUONDBInfo.disabl_romo_sum] =p_romo_tmp;
					g_MUONDBInfo.disabl_romo_header[g_MUONDBInfo.disabl_romo_sum]->Position = nRomoGeoTmp;
					
					g_MUONDBInfo.disabl_romo_sum++;
				}

				break ;
				
			case MUON_ROC:
				g_MUONDBInfo.roc_header = (struct muon_roc*)pcMdlHeader ;
				if((g_MUONDBInfo.roc_header->geo)>256)
				{
					g_MUONDBInfo.roc_header->geo&=0xff;
					Show_muon_roc(g_MUONDBInfo.roc_header);
					
					g_MUONDBInfo.roc_sum++;
				}
				if (g_MUONDBInfo.roc_sum> 1)
				{
					DEBUG_OUTPUT(("ERROR: g_MUONDBInfo.roc_sum > 1.\n") );
					Push2ErrStack(PPCFATAL+MUC_FLG+ATOM_ERR+3,0,0,0,0);		/*muc error 3*/
					return -1 ;
				}
				break ;
			case MUON_MUDT	:
				g_MUONDBInfo.mudt_header[imudt] = (struct muon_mudt*)pcMdlHeader ;
				
				MucMUDTBSEL[imudt] = (g_MUONDBInfo.mudt_header[imudt]->Position>>19)&0x1f;
				MucMUDTGeo[imudt] = (g_MUONDBInfo.mudt_header[imudt]->Position&0x1ff);
				
				if(MucMUDTGeo[imudt] >=256)
				{
					MucMUDTGeo[imudt] &=0xff;
					Show_muon_mudt(g_MUONDBInfo.mudt_header[imudt]);
					
					g_MUONDBInfo.mudt_sum++;
					imudt++;
				}
				else
					printf("A MUDT disable !\n",imudt);
				break ;	
			default:
				printf("Unkown modules type.\n") ;
				Push2ErrStack(PPCFATAL+MUC_FLG+ATOM_ERR+18,0,0,0,0);		/*muc error 18*/
				return -1 ;
		}

		pcMdlHeader = pcMdlHeader + (UINT)(*(UINT*)pcMdlHeader) ;
		
	}
	/*check the ppcid between db and jump*/
	if (g_nPPCIDOfSRH!= g_MUONDBInfo.ppc_header->PPC_id)
	{
		DEBUG_OUTPUT(("The PPC id in db (%d) is not equal to the one in SRH (%d).\n", g_MUONDBInfo.ppc_header->PPC_id, g_nPPCIDOfSRH) );
		Push2ErrStack(PPCFATAL+MUC_FLG+ATOM_ERR+4,0,0,0,0);		/*muc error 4*/
		return -1 ;
	}

	/*just queue enable module,by leigk 071230*/
	for(int nQueIdx=0;nQueIdx<g_MUONDBInfo.enabl_romo_sum-1;nQueIdx++)
	{
		for(int nQueIdx2=nQueIdx+1;nQueIdx2<g_MUONDBInfo.enabl_romo_sum;nQueIdx2++)
		{
			if(gMucEnablRomoGeo[nQueIdx]>gMucEnablRomoGeo[nQueIdx2] )
			{
				p_romo_tmp=g_MUONDBInfo.enabl_romo_header[nQueIdx];
				g_MUONDBInfo.enabl_romo_header[nQueIdx]=g_MUONDBInfo.enabl_romo_header[nQueIdx2];
				g_MUONDBInfo.enabl_romo_header[nQueIdx2]=p_romo_tmp;
				
				nRomoGeoTmp=gMucEnablRomoGeo[nQueIdx];
				gMucEnablRomoGeo[nQueIdx]=gMucEnablRomoGeo[nQueIdx2];
				gMucEnablRomoGeo[nQueIdx2]=nRomoGeoTmp;

				nModuIDTmp=gMucEnablModuleID[nQueIdx];
				gMucEnablModuleID[nQueIdx]=gMucEnablModuleID[nQueIdx2];
				gMucEnablModuleID[nQueIdx2]=nModuIDTmp;
			}
		}
	}

	
	for(nQueIdx=0;nQueIdx<g_MUONDBInfo.enabl_romo_sum;nQueIdx++)
		DEBUG_OUTPUT(("gMucEnablModuleID[%d] = %d\n", nQueIdx, gMucEnablModuleID[nQueIdx]));

	if(g_MUONDBInfo.enabl_romo_sum)
	{	
		if(-1==MUONConfIntCBLT())
		{
			DEBUG_OUTPUT(("Config interrupt or config CBLT error.\n"));
			Push2ErrStack(PPCFATAL+MUC_FLG+ATOM_ERR+5,0,0,0,0);		/*muc error 5*/
			return -1;
		}
		printf("After CBLTinit.\n") ;
		
	}
	printf("MUONInitializedLOAD OK\n") ;
	return 0;
}
int  	MUONLoadedPRCF 			(int nArg) 
{
	if(g_MUONDBInfo.enabl_romo_sum)
	{
		unsigned short nDataPort = g_MUONDBInfo.ppc_header->ROS_port ;
		unsigned int 	 nRemoteIP = g_MUONDBInfo.ppc_header->ROS_ip;
		DEBUG_OUTPUT(("ROS IP = %x, PORT = %d.\n",nRemoteIP, nDataPort)) ;
		if(DataSockConnect(nRemoteIP,nDataPort)==-1)
		{
			DEBUG_OUTPUT(("connect data socket error\n"));
			Push2ErrStack(PPCFATAL+MUC_FLG+ATOM_ERR+6,0,0,0,0);		/*muc error 6*/
			CloseDataSocket();
			return -1;
		}
		printf("successfully connected on data socket.\n") ;
	}
	printf("MUONLoadedPRCF OK\n") ;
	return 0;
}

int  	MUONSynchronized2SYN3 	(int nArg) 
//int  	MUONPreconfigedCONF 	(int nArg) 
{
	int nDBIdx ;
	int nMdlIdx ;
	bool bMatchFlag;

	switch(g_nRunMode)
	{
		case NOMALMODE:
			if((g_nTrgPerInt<1)||(g_nTrgPerInt>16))
			{
				DEBUG_OUTPUT(("normal mode,TrgPerInt must equal 1-16.\n"));
				Push2ErrStack(PPCFATAL+MUC_FLG+ATOM_ERR+7,0,0,0,0);		/*muc error 7*/
				return -1;
			}
			break;
		case PEDMODE:
		case CALIBRATION:
		case MONITOR:
		case BASELINE:
		case WAVEMODE:
			DEBUG_OUTPUT(("MUC system has not  NonPhysics mode.\n"));
			Push2ErrStack(PPCFATAL+MUC_FLG+ATOM_ERR+8,0,0,0,0);		/*muc error 8*/
			return -1;
		default:
			break;	
	}


	if(WriteToVme32(0xf00000,VA32,0xaaa010)==-1) 
	{
		DEBUG_OUTPUT(("set tirg mode Error!\n"));
		Push2ErrStack(PPCFATAL+MUC_FLG+ATOM_ERR+29,0,0,0,0);		//muc error 29
		return -1;						
	}
	DEBUG_OUTPUT(("set tirg mode selfL1!\n"));
	taskDelay(10);	
	
		
	//initial cmd--system reset	//jixl
	if(WriteToVme32(0xf00000,VA32,0xaaa111)==-1)
	{
		DEBUG_OUTPUT(("MUC Initial Error!\n"));
		Push2ErrStack(PPCFATAL+MUC_FLG+ATOM_ERR+9,0,0,0,0);		/*muc error 9*/
		return -1;
	}
	printf("********Initialized OK...system reset!***********!\n");
	taskDelay(30);	

	//romo module scan
	for(nMdlIdx=2;nMdlIdx<22;nMdlIdx++)
	{
		if(WriteToVme16((UINT32)((nMdlIdx<<19)+0x060040),VCSR,0x14)==-1)
		{
			printf("no board at %d to reset scan register\n",nMdlIdx);
		}
	}

	g_MUONHWInfo.romo_sum=0;
	for(nMdlIdx=2;nMdlIdx<22;nMdlIdx++)
	{
		unsigned short nValueTmp=0;
		ReadFrmVme16((UINT32)((nMdlIdx<<19)+0x060040),VCSR,&nValueTmp);
		if(nValueTmp==0xffff)
		{
			DEBUG_OUTPUT(("no board at %d slot\n",nMdlIdx) );
		}
		else
		{	
			//printf("the value:0x%08x\n",nValueTmp);
			if(nValueTmp==0x41)
			{
				g_MUONHWInfo.romo_geo[g_MUONHWInfo.romo_sum]=nMdlIdx;
				DEBUG_OUTPUT(("scan one module at %d slot\n",nMdlIdx) );	
				g_MUONHWInfo.romo_sum++;
			}	
			else
			{
//				DEBUG_OUTPUT(("slot %d have module,but scan failed\n",nMdlIdx) );
			}
		}
	}
	
	
	//check module with db config
	if(g_MUONHWInfo.romo_sum != (g_MUONDBInfo.enabl_romo_sum+g_MUONDBInfo.disabl_romo_sum))
	{
		printf("g_MUONHWInfo.romo_sum (%d) !=   g_MUONDBInfo.romo_sum (%d).\n", g_MUONHWInfo.romo_sum,
													(g_MUONDBInfo.enabl_romo_sum+g_MUONDBInfo.disabl_romo_sum)) ;
		Push2ErrStack(PPCFATAL+MUC_FLG+ATOM_ERR+10,0,0,0,0);		/*muc error 10*/
		return -1 ;
	}
	
	for(nMdlIdx=0;nMdlIdx<g_MUONHWInfo.romo_sum;nMdlIdx++)
	{
		bMatchFlag=false;
		for(nDBIdx=0;nDBIdx<g_MUONDBInfo.enabl_romo_sum;nDBIdx++)
		{
			if(g_MUONHWInfo.romo_geo[nMdlIdx]==gMucEnablRomoGeo[nDBIdx])
			{
				bMatchFlag=true;
			}
		}
		
		for(nDBIdx=0;nDBIdx<g_MUONDBInfo.disabl_romo_sum;nDBIdx++)
		{
			if(g_MUONHWInfo.romo_geo[nMdlIdx]==g_MUONDBInfo.disabl_romo_header[nDBIdx]->Position)
			{
				bMatchFlag=true;
			}
		}
		
		if(!bMatchFlag)
		{
				DEBUG_OUTPUT(("Module in Slot %d have no infor in db\n",g_MUONHWInfo.romo_geo[nMdlIdx])) ;
				Push2ErrStack(PPCFATAL+MUC_FLG+ATOM_ERR+11,0,0,0,0);		/*muc error 11*/
				return -1;
		}
	}

	if(-1 == MUONRomoConfigAll())
	{
		printf("MUONRomoConfigAll failed\n") ;
		return -1 ;
	}
	
/****/
	if(!g_MUONDBInfo.ppc_header->NormModeSelfL1)
	{
		//for unshield
		//set trigger mode  10-self/20-ext/30-normal
		for (int k=0;k<3;k++)
		{
			if(WriteToVme32(0xf00000,VA32,0xaaa030)==-1) 
			{
				printf("set tirg mode Error!\n");
				Push2ErrStack(PPCFATAL+MUC_FLG+ATOM_ERR+29,0,0,0,0);		//muc error 29
				return -1;
			}
			printf("********unshield********\n");
			taskDelay(10);	
		}
	}

	if(g_MUONDBInfo.mudt_sum>0)
	{
		if(-1==TRG_MUDT_syn3() )
		{
			DEBUG_OUTPUT(("syn3 MUDT error.\n"));
			Push2ErrStack(PPCFATAL+MUC_FLG+ATOM_ERR+63,0,0,0,0);		//muc error 63
			return -1;
		}	
	}	

	printf("MUONPreconfiged OK\n") ;
	return 0;
}
int  	MUONConfigedSYN1 		(int nArg) 
{
/*
	if(!g_MUONDBInfo.ppc_header->NormModeSelfL1)
	{
		//for shield
		//set trigger mode  10-self/20-ext/30-normal
		if(WriteToVme32(0xf00000,VA32,0xaaa010)==-1) 
		{
			printf("set tirg mode Error!\n");
		}
		printf("********shield********\n");
		
		taskDelay(10);
	}
*/	
	DEBUG_OUTPUT(("MUONConfigedSYN1 OK\n")) ;
	return 0;
}
int  	MUONSynchronized1SYN2 	(int nArg) 
{
	DEBUG_OUTPUT(("MUONSynchronized1SYN2 OK\n") );
	return 0;
}

int  	MUONPreconfigedCONF 	(int nArg) 
//int  	MUONSynchronized2SYN3 	(int nArg) 
{
/*
	if(!g_MUONDBInfo.ppc_header->NormModeSelfL1)
	{
		//for unshield
		//set trigger mode  10-self/20-ext/30-normal
		if(WriteToVme32(0xf00000,VA32,0xaaa030)==-1) 
		{
			printf("set tirg mode Error!\n");
			Push2ErrStack(PPCFATAL+MUC_FLG+ATOM_ERR+29,0,0,0,0);		//muc error 29
			return -1;
		}
		printf("********unshield********\n");
		taskDelay(10);	
	}


	if(g_MUONDBInfo.mudt_sum>0)
	{
		if(-1==TRG_MUDT_syn3() )
		{
			DEBUG_OUTPUT(("syn3 MUDT error.\n"));
			Push2ErrStack(PPCFATAL+MUC_FLG+ATOM_ERR+63,0,0,0,0);		//muc error 63
			return -1;
		}	
	}	
*/
/*******/
	if(WriteToVme32(0xf00000,VA32,0xaaa010)==-1) 
	{
		DEBUG_OUTPUT(("set tirg mode Error!\n"));
		Push2ErrStack(PPCFATAL+MUC_FLG+ATOM_ERR+29,0,0,0,0);		//muc error 29
		return -1;						
	}
	DEBUG_OUTPUT(("set tirg mode selfL1!\n"));
	taskDelay(10);	
	
	DEBUG_OUTPUT(("MUONSynchronized2SYN3 OK\n")) ;
	return 0;
}

int  	MUONSynchronized3SYN4 	(int nArg) 
{	
	DEBUG_OUTPUT(("MUONSynchronized3SYN4 OK\n")) ;
	return 0;
}
int  	MUONSynchronized4PREP 	(int nArg) 
{
	switch(g_nRunMode)
	{
		case NOMALMODE:
		case PEDMODE:
		case CALIBRATION:
		case MONITOR:
		case BASELINE:
		case WAVEMODE:
			if(g_MUONDBInfo.enabl_romo_sum)
			{

				if(-1 ==  (nCbltTaskId=taskSpawn("tFWCbltTrans", PRI_CBLTTRANS, 0, TASK_STACK_SIZE, (FUNCPTR)FWCbltTrans, 
										0, 0, 0, 0, 0, 0, 0, 0, 0, 0)))
				{
					DEBUG_OUTPUT(("taskSpawn tFWCbltTrans failed\n")) ;
					Push2ErrStack(PPCFATAL+MUC_FLG+ATOM_ERR+37,0,0,0,0);		/*muc error 37*/	
					return -1;
				}

				if(-1 ==  (nPackTaskId=taskSpawn("tFWNetTrans", PRI_DATAPACK, 0, TASK_STACK_SIZE, (FUNCPTR)FWNetTrans, 
										0, 0, 0, 0, 0, 0, 0, 0, 0, 0)))
				{
					DEBUG_OUTPUT(("taskSpawn tFWNetTrans failed\n")) ;
					Push2ErrStack(PPCFATAL+MUC_FLG+ATOM_ERR+38,0,0,0,0);		/*muc error 38*/	
					return -1;
				}

				if(-1 ==  (nNetTaskId=taskSpawn("tMUONDataPack", PRI_NETTRANS, 0, TASK_STACK_SIZE, (FUNCPTR)MUONDataPack, 
										0, 0, 0, 0, 0, 0, 0, 0, 0, 0)))
				{
					DEBUG_OUTPUT(("taskSpawn tMUONDataPack failed\n")) ;
					Push2ErrStack(PPCFATAL+MUC_FLG+ATOM_ERR+39,0,0,0,0);		/*muc error 39*/	
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
			MUONStartTrig();//write once may fail . so write 3 times. 20100308 by jixl
			taskDelay(30);
			MUONStartTrig();
			taskDelay(30);
			MUONStartTrig();			
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

	DEBUG_OUTPUT(("MUONSynchronized4PREP OK\n")) ;
	return 0;
}
int  	MUONReadySATR  			(int nArg)
{
	
	DEBUG_OUTPUT(("MUONReadySATR OK\n") );
	return 0;
}
int  	MUONRunningSPTR  		(int nArg)
{

	DEBUG_OUTPUT(("MUONRunningSPTR OK\n")) ;
	return 0;
}
int  	MUONReadySTOP  			(int nArg)
{
	//should be done after trigger system stopped trigger
	switch(g_nRunMode)
	{
		case NOMALMODE:
			MUONStopTrig();
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
	
	DEBUG_OUTPUT(("MUONReadySTOP OK\n") );
	return 0;
}
int  	MUONConfigedUNCF 		(int nArg)
{
	DEBUG_OUTPUT(("MUONConfigedUNCF OK\n")) ;
	return 0;
}
int  	MUONPreconfigedUNPC 	(int nArg) 
{
	DEBUG_OUTPUT(("MUONPreconfigedUNPC OK\n")) ;
	return 0;
}
int  	MUONLoadedUNLD 			(int nArg)
{
	DEBUG_OUTPUT(("MUONLoadedUNLD OK\n")) ;
	return 0;
}
int  	MUONAnyEXIT 			(int nArg)
{
//	MUONStop();	//jixl 071204

	DEBUG_OUTPUT(("MUONInitializedEXIT OK\n")) ;
	return 0;
}
