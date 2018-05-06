#include "5100_test.h"
#include "vxWorks.h"
#include "rng_dma.h"
#include "intLib.h"
#include "sysLib.h"
#include "sockLib.h"
#include "taskLib.h"
#include "msgQLib.h"

#include "fw_configinfo.h"
#include "fw_dcgvas.h"
#include "fw_define.h"
#include "fragheaders.h"
#include "fw_filefunc.h"
#include "fw_tcpfunc.h"
#include "fw_staterrspt.h"
#include "fw_runtasks.h"
#include "fw_actions.h"

#include "sd_tof.h"
#include "sd_tof_hw.h"
#include "sd_tof_monitor.h"
#include "sd_tof_runtasks.h"
#include "sd_tof_actions.h"
#include "sd_tof_datapack.h"


void table2file()
{
	FILE* fp;
	char file[64];
	sprintf(file,"table%d",g_nCrateID);
	if(NULL==(fp=fopen(file,"wb")) )
	{
		printf("open file error\n");
	}
	for(int ix=0;ix<g_TOFDBInfo.tof_FEE_sum; ix++)
	{
		UINT nPos= g_TOFDBInfo.tof_FEE_header[ix]->geo;;
		for(int iy=0;iy<16; iy++)
		{
			fwrite(&g_nConvertTable[nPos][iy],4,1,fp);
		}
	}
	fclose(fp);
}


int	TOFInitializedLOAD 		(int nArg) 
{
	char * pcMdlHeader ;
	int nMdlType;
	int nMdlIdx = 0 ;
	int nClibMoniInfoLen=0;

	sysIntDisable(TOF_INT_LEVEL) ;

	if((g_nCrateID<0x40)||(g_nCrateID>=0x60))
	{
		DEBUG_OUTPUT(("TOF CrateID out of 0x40-0x60\n")) ;
		Push2ErrStack(PPCFATAL+TOF_FLG+ATOM_ERR+1,0,0,0,0);		/*tof error 1*/
		return -1;
	}

	//----segment for Calib|Monitor infor
	g_cCalibMoni_Info=((char*)g_pCrateConfigHeader) + g_pCrateConfigHeader->crate_len ;	//calib&Monitor mode ,all use these infor to config
	g_cCalibMoni_Info_ori = g_cCalibMoni_Info;	
	nClibMoniInfoLen=*(unsigned int*)(g_cCalibMoni_Info)-4; 	//the exact calib|Monitor infor length			//other mode use default to config
	DEBUG_OUTPUT( ("ClibMoniInfoLen=%d\n",nClibMoniInfoLen) );
	g_cCalibMoni_Info=g_cCalibMoni_Info+4;                 			//jump the length infor
	switch(g_nRunMode)
	{
		case NOMALMODE:
		case PEDMODE:
		case BASELINE:
		case WAVEMODE:	
			break;
		case CALIBRATION:
			g_nCalibMoni_Points=nClibMoniInfoLen/8; //calib amp + runtimes
			break;
		case MONITOR:
			g_nCalibMoni_Points=nClibMoniInfoLen/16;//LDPulseInteval + LDPulseNum+LDSolidnoid+runtimes
			break ;
	}
	DEBUG_OUTPUT( ("g_nCalibMoni_Points=%d\n",g_nCalibMoni_Points) );
	pcMdlHeader = g_cCalibMoni_Info+nClibMoniInfoLen ;
	//----segment for Calib|Monitor infor

	g_TOFDBInfo.tof_ppc_sum=0;
	g_TOFDBInfo.tof_FEE_sum=0;
	g_TOFDBInfo.tof_roc_sum=0;
	g_TOFDBInfo.tof_clk_sum=0;
	g_TOFDBInfo.tof_monitor_sum=0;
	g_uTofMonitorGeo=0;

	int nTempLen=0;
	for(nMdlIdx=0;nMdlIdx<g_nMdlSum;nMdlIdx++)
	{
		nMdlType =  (UINT)(*(UINT*)(pcMdlHeader + 4)) ;
		DEBUG_OUTPUT(("Module Type: 0x%x\n", nMdlType) ) ;
		switch(nMdlType)
		{
			case TOF_PPC:
			{
				g_TOFDBInfo.tof_ppc_header=(struct GE_ppc *)pcMdlHeader;	
//				g_TOFDBInfo.tof_ppc_header->NormModeSelfL1 = 1; //temp leigk 071220
				Show_ppc(g_TOFDBInfo.tof_ppc_header);
				
				g_TOFDBInfo.tof_ppc_sum++;
				if(g_TOFDBInfo.tof_ppc_sum>1)
				{
					DEBUG_OUTPUT( ("PPC config infor error,sum >1\n") );
					Push2ErrStack(PPCFATAL+TOF_FLG+ATOM_ERR+2,0,0,0,0);		/*tof error 2*/
					return -1;
				}
				break;
			}
			case TOF_FEE :
			{
				g_TOFDBInfo.tof_FEE_header[g_TOFDBInfo.tof_FEE_sum]=(struct tof_FEE *)pcMdlHeader;
				if((g_TOFDBInfo.tof_FEE_header[g_TOFDBInfo.tof_FEE_sum]->geo)>256)
				{
					g_TOFDBInfo.tof_FEE_header[g_TOFDBInfo.tof_FEE_sum]->geo&=0xff;
					Show_tof_FEE(g_TOFDBInfo.tof_FEE_header[g_TOFDBInfo.tof_FEE_sum]);
					g_TOFDBInfo.tof_FEE_sum++;
				}
				else
				{
					DEBUG_OUTPUT( ("recv one disable FEE config\n") );
				}
				break;
			}
			case TOF_ROC:
			{
				g_TOFDBInfo.tof_roc_header=(struct tof_roc *)pcMdlHeader;
				if((g_TOFDBInfo.tof_roc_header->geo)>256)
				{
					g_TOFDBInfo.tof_roc_header->geo&=0xff;
					Show_tof_roc(g_TOFDBInfo.tof_roc_header);
					g_TOFDBInfo.tof_roc_sum++;
				}
				else
				{
					DEBUG_OUTPUT( ("recv one disable ROC config\n") );
				}
				if(g_TOFDBInfo.tof_roc_sum>1)
				{
					DEBUG_OUTPUT( ("roc config infor error,sum >1\n") );
					Push2ErrStack(PPCFATAL+TOF_FLG+ATOM_ERR+3,0,0,0,0);		/*tof error 3*/
					return -1;
				}
				break;
			}
			case TOF_CLK:
			{
				g_TOFDBInfo.tof_clk_header=(struct tof_clk *)pcMdlHeader;
				
				if((g_TOFDBInfo.tof_clk_header->geo)>256)
				{
				
					g_TOFDBInfo.tof_clk_header->geo&=0xff;
					Show_tof_clk(g_TOFDBInfo.tof_clk_header);
					g_TOFDBInfo.tof_clk_sum++;
				}
				else
				{
					DEBUG_OUTPUT( ("recv one disable CLK config\n") );
				}
				if(g_TOFDBInfo.tof_clk_sum>1)
				{
					DEBUG_OUTPUT( ("clk config infor error,sum >1\n") );
					Push2ErrStack(PPCFATAL+TOF_FLG+ATOM_ERR+4,0,0,0,0);		/*tof error 4*/
					return -1;
				}
				break;
			}
			/*
			case TOF_TDIG:
			{
				g_TOFDBInfo.tof_tdig_header=(struct tof_tdig *)pcMdlHeader;
				
				if((g_TOFDBInfo.tof_tdig_header->geo)>256)
				{
				
					g_TOFDBInfo.tof_tdig_header->geo&=0xff;
					Show_tof_tdig(g_TOFDBInfo.tof_tdig_header);
					g_TOFDBInfo.tof_tdig_sum++;
				}
				else
				{
					DEBUG_OUTPUT( ("recv one disable tdig config\n") );
				}
				if(g_TOFDBInfo.tof_tdig_sum>1)
				{
					DEBUG_OUTPUT( ("tdig config infor error,sum >1\n") );
					Push2ErrStack(PPCFATAL+MRPC_FLG+ATOM_ERR+51,0,0,0,0);		
					return -1;
				}
				break;
			}	
			*/
			case TOF_MONITOR:
			{
				g_TOFDBInfo.tof_monitor_header=(struct tof_monitor*)pcMdlHeader;
				if((g_TOFDBInfo.tof_monitor_header->geo)>256)
				{
					g_TOFDBInfo.tof_monitor_header->geo&=0xff;
					
					Show_tof_monitor(g_TOFDBInfo.tof_monitor_header);
					g_TOFDBInfo.tof_monitor_sum++;
				}
				else
				{
					DEBUG_OUTPUT( ("recv one disable Monitor config\n") );
				}
				if(g_TOFDBInfo.tof_monitor_sum>1)
				{
					DEBUG_OUTPUT( ("monitor config infor error,sum >1\n") );
					Push2ErrStack(PPCFATAL+TOF_FLG+ATOM_ERR+5,0,0,0,0);		/*tof error 5*/
					return -1;
				}
				break;
			}
			default :
			{
				DEBUG_OUTPUT( ("Unknown module type\0") );
				Push2ErrStack(PPCFATAL+TOF_FLG+ATOM_ERR+6,0,0,0,0);		/*tof error 6*/
				return (-1);
			}
		}
		pcMdlHeader = pcMdlHeader +(UINT)(*(UINT*)pcMdlHeader) ;
		
	}
	
	/*queue the FEE config info for create the map table*/
	for(int nQueIdx=0;nQueIdx<g_TOFDBInfo.tof_FEE_sum-1;nQueIdx++)
	{
		struct tof_FEE * pTOF_FEETmp;
		for(int nQueIdx2=nQueIdx+1;nQueIdx2<g_TOFDBInfo.tof_FEE_sum;nQueIdx2++)
		{
			if((g_TOFDBInfo.tof_FEE_header[nQueIdx]->geo)>(g_TOFDBInfo.tof_FEE_header[nQueIdx2]->geo))
			{
				pTOF_FEETmp=g_TOFDBInfo.tof_FEE_header[nQueIdx];
				g_TOFDBInfo.tof_FEE_header[nQueIdx]=g_TOFDBInfo.tof_FEE_header[nQueIdx2];
				g_TOFDBInfo.tof_FEE_header[nQueIdx2]=pTOF_FEETmp;
			}
		}
	}

	for(int nQueResult=0;nQueResult<g_TOFDBInfo.tof_FEE_sum;nQueResult++)
	{
		DEBUG_OUTPUT(("queue of GEO :%d\n",g_TOFDBInfo.tof_FEE_header[nQueResult]->geo));
	}
	
	//set g_uTofMonitorGeo value
	if(g_TOFDBInfo.tof_monitor_sum==1)
	{
		g_uTofMonitorGeo=g_TOFDBInfo.tof_monitor_header->geo;
	}
	else
	{
		g_uTofMonitorGeo=0;
	}

	
// here create the cblt module list geo,and with geo sequence
	g_uTofCbltModuleSum=0;	//reset CBLT Moudle NUM=0;
	for(nMdlIdx=0;nMdlIdx<g_TOFDBInfo.tof_FEE_sum;nMdlIdx++)
	{
		g_uTofCbltGeo[nMdlIdx]=g_TOFDBInfo.tof_FEE_header[nMdlIdx]->geo;
	}
	g_uTofCbltModuleSum=g_TOFDBInfo.tof_FEE_sum;

	//if monior mode and have monitor module,then plus monitor moudle to cblt
	//insert monitor module to list
	if( (1==g_TOFDBInfo.tof_monitor_sum)&&((g_nRunMode==CALIBRATION)||(g_nRunMode==MONITOR)))
	{
		nMdlIdx=g_TOFDBInfo.tof_FEE_sum-1;
		while( (nMdlIdx>=0)&&(g_uTofCbltGeo[nMdlIdx]> g_uTofMonitorGeo))//get the monitor posistion should be where
		{
			g_uTofCbltGeo[nMdlIdx+1]=g_uTofCbltGeo[nMdlIdx];
			nMdlIdx--;
		}
		nMdlIdx++;
		g_uTofCbltGeo[nMdlIdx]=g_uTofMonitorGeo;
		g_uTofCbltModuleSum++;					//plus monitor module
	}
	printf("g_uTofCbltModuleSum=%d\n",g_uTofCbltModuleSum);
	
	/*compare the PPC id in db and jumper*/
	if ( g_nPPCIDOfSRH!= g_TOFDBInfo.tof_ppc_header->PPC_id)
	{
		Push2ErrStack(PPCFATAL+TOF_FLG+ATOM_ERR+7,0,0,0,0);		/*tof error 7*/
		DEBUG_OUTPUT(("PPC id not equal SRH\n"));
		return -1 ;
	}
	
	switch(g_nRunMode)
	{
		case NOMALMODE:
		case PEDMODE:
		case CALIBRATION:
		case MONITOR:
		case BASELINE:
		case WAVEMODE:		
			if(TOFConfigIntCBLT()==-1)
			{
				Push2ErrStack(PPCFATAL+TOF_FLG+ATOM_ERR+8,0,0,0,0);		/*tof error 8*/
				DEBUG_OUTPUT(("config cblt error\n"));
				return -1;
			}
			break ;
		default:
			Push2ErrStack(PPCFATAL+TOF_FLG+ATOM_ERR+9,0,0,0,0);		/*tof error 9*/
			DEBUG_OUTPUT(("unknown running mode\n"));
			return -1;
	}
	printf("TOFInitializedLOAD OK\n") ;
	return 0;
}
int  	TOFLoadedPRCF 			(int nArg) 
{
	/*connect data socket*/
	unsigned short nDataPort = g_TOFDBInfo.tof_ppc_header->ROS_port ;
	unsigned int	 nRemoteIP = g_TOFDBInfo.tof_ppc_header->ROS_ip;
	DEBUG_OUTPUT(("ROS IP = %x, PORT = %d.\n",nRemoteIP, nDataPort)) ;
	if(DataSockConnect(nRemoteIP,nDataPort)==-1)
	{
		DEBUG_OUTPUT(("connect data socket error\n"));
		Push2ErrStack(PPCFATAL+TOF_FLG+ATOM_ERR+10,0,0,0,0);		/*tof error 10*/
		CloseDataSocket();
		return -1;
	}
	
	DEBUG_OUTPUT(("TOFLoadedPRCF OK\n")) ;
	return 0;
}
int  	TOFPreconfigedCONF 	(int nArg) 
{
	int nModuIdx;
	int nChnlIdx;

	switch(g_nRunMode)
	{
		case NOMALMODE:
		case PEDMODE:
		case BASELINE:
		case WAVEMODE:
			g_uTofCalibSignAmp=0xffff;	//this parameter only have meaning at calibmode
			break;
		case MONITOR:
			g_uTofLDPulseInterval=*(unsigned int *)g_cCalibMoni_Info;
			g_cCalibMoni_Info=g_cCalibMoni_Info+4;
			g_uTofLDPulseNum=*(unsigned int *)g_cCalibMoni_Info;
			g_cCalibMoni_Info=g_cCalibMoni_Info+4;
			g_uTofLDSolenoid=*(unsigned int *)g_cCalibMoni_Info;
			g_cCalibMoni_Info=g_cCalibMoni_Info+4;
			g_nL1Count=*(unsigned int *)g_cCalibMoni_Info;
			g_cCalibMoni_Info=g_cCalibMoni_Info+4;
			DEBUG_OUTPUT(("LDPulseInterval=%d,LDPulseNum=%d,LDSolenoid=%d\n",g_uTofLDPulseInterval,g_uTofLDPulseNum,g_uTofLDSolenoid));
			break ;
		case CALIBRATION:
			{
				g_uTofCalibSignAmp=*(unsigned int *)g_cCalibMoni_Info;
				g_cCalibMoni_Info=g_cCalibMoni_Info+4;
				g_nL1Count=*(unsigned int *)g_cCalibMoni_Info;
				g_cCalibMoni_Info=g_cCalibMoni_Info+4;
				DEBUG_OUTPUT(("TOF CalibDAC=%d,g_nL1Count=%d \n",g_uTofCalibSignAmp,g_nL1Count));
			}
			break ;
		default:
			Push2ErrStack(PPCFATAL+TOF_FLG+ATOM_ERR+9,0,0,0,0);		/*tof error 9*/
			DEBUG_OUTPUT(("unknown running mode\n")) ;
			return -1;
	}

	
	for(nModuIdx = 0; nModuIdx < MAX_SLOT_AMOUNT_PLUS_ONE; nModuIdx ++)
	{
		for(nChnlIdx = 0; nChnlIdx < MAX_CHNL_AMOUNT; nChnlIdx ++)
		{
			g_nConvertTable[nModuIdx][nChnlIdx] = 0 ;
			g_fAjust_a2[nModuIdx][nChnlIdx] = 0.0 ;		
			g_fAjust_a1[nModuIdx][nChnlIdx] = 1.0 ;		
			g_fAjust_a0[nModuIdx][nChnlIdx] = 0.0 ;					
		}
	}

	switch(g_nRunMode)
	{
		case NOMALMODE:
			if((g_nTrgPerInt<1)||(g_nTrgPerInt>16))
			{
				DEBUG_OUTPUT(("normal mode,TrgPerInt must equal 1-16.\n"));
				Push2ErrStack(PPCFATAL+TOF_FLG+ATOM_ERR+12,0,0,0,0);		/*tof error 12*/
				return -1;
			}
			for(nModuIdx = 0; nModuIdx <g_TOFDBInfo.tof_FEE_sum; nModuIdx ++) 
			{
				UINT nPosition = g_TOFDBInfo.tof_FEE_header[nModuIdx]->geo;
				for(nChnlIdx = 0; nChnlIdx < 16; nChnlIdx ++)
				{
					unsigned int nWestEast= (g_TOFDBInfo.tof_FEE_header[nModuIdx]->map[nChnlIdx])/1000;
					unsigned int nLayer	= ((g_TOFDBInfo.tof_FEE_header[nModuIdx]->map[nChnlIdx])%1000)/100;
					unsigned int nCell	= (g_TOFDBInfo.tof_FEE_header[nModuIdx]->map[nChnlIdx])%100;
					//printf("%d  %d   %d\n",nWestEast,nLayer,nCell);
					g_nConvertTable[nPosition][nChnlIdx] =((nWestEast&0x1)<<30)|((nLayer&0x3)<<28)|((nCell&0x7f)<<21);
					//printf("[%d][%d]=0x%08x\n",nPosition,nChnlIdx,g_nConvertTable[nPosition][nChnlIdx] );

					if((g_pCrateConfigHeader->unused)&0x1)	//need calib
					{
						g_fAjust_a2[nPosition][nChnlIdx]=(2.5*(g_TOFDBInfo.tof_FEE_header[nModuIdx]->adjust_a2[nChnlIdx]));
						g_fAjust_a1[nPosition][nChnlIdx]=(2.5*(g_TOFDBInfo.tof_FEE_header[nModuIdx]->adjust_a1[nChnlIdx]));
						g_fAjust_a0[nPosition][nChnlIdx]=(2.5*(g_TOFDBInfo.tof_FEE_header[nModuIdx]->adjust_a0[nChnlIdx]));
					}
				}
			}

			//table2file();
			
			break;
		case PEDMODE:
		case CALIBRATION:
			//testtest//
/*			
			for(nModuIdx = 0; nModuIdx <g_TOFDBInfo.tof_FEE_sum; nModuIdx ++) 
			{
				UINT nPosition = g_TOFDBInfo.tof_FEE_header[nModuIdx]->geo;
				for(nChnlIdx = 0; nChnlIdx < 16; nChnlIdx ++)
				{
					if((g_pCrateConfigHeader->unused)&0x1)	//need calib
					{
//						printf("**********need to calib********\n");
						g_fAjust_a2[nPosition][nChnlIdx]=g_TOFDBInfo.tof_FEE_header[nModuIdx]->adjust_a2[nChnlIdx];
						g_fAjust_a1[nPosition][nChnlIdx]=g_TOFDBInfo.tof_FEE_header[nModuIdx]->adjust_a1[nChnlIdx];
						g_fAjust_a0[nPosition][nChnlIdx]=g_TOFDBInfo.tof_FEE_header[nModuIdx]->adjust_a0[nChnlIdx];
					}
				}
			}	
			*/
		case MONITOR:
		case BASELINE:
		case WAVEMODE:
			if(g_nTrgPerInt!=1)
			{
				DEBUG_OUTPUT(("Non normal mode,TrgPerInt must equal 1\n"));
				Push2ErrStack(PPCFATAL+TOF_FLG+ATOM_ERR+13,0,0,0,0);		/*tof error 13*/
				return -1;
			}
			for(nModuIdx = 0; nModuIdx <g_TOFDBInfo.tof_FEE_sum; nModuIdx ++) 
			{
				UINT nPosition = g_TOFDBInfo.tof_FEE_header[nModuIdx]->geo;
				for(nChnlIdx = 0; nChnlIdx < 16; nChnlIdx ++)
				{
					g_nConvertTable[nPosition][nChnlIdx] =(nPosition<<25)+(nChnlIdx<<21);
				}
			}
			//init monitor ConvertTable special
			if( 1==g_TOFDBInfo.tof_monitor_sum )
			{
				for(nChnlIdx = 0; nChnlIdx < 16; nChnlIdx ++)
				{
					g_nConvertTable[g_uTofMonitorGeo][nChnlIdx] =(g_uTofMonitorGeo<<25)+(nChnlIdx<<21);//for up system to distinguish Monitor data
				}
			}
			break;
		default:
			//unknown running mode\0");
			Push2ErrStack(PPCFATAL+TOF_FLG+ATOM_ERR+9,0,0,0,0);		/*tof error 9*/
			return -1;
	}

	if(-1==TOFConfigAll())
	{
		DEBUG_OUTPUT(("TOFConfigAll Failed !\n"));
		Push2ErrStack(PPCFATAL+TOF_FLG+ATOM_ERR+15,0,0,0,0);		/*tof error 15*/
		return -1;
	}

	DEBUG_OUTPUT(("TOFPreconfiged OK\n")) ;
	return 0;
}
int  	TOFConfigedSYN1 		(int nArg) 
{
	if(g_nRunMode==NOMALMODE)
		if(g_nCrateID == 0x40)
		{
			if(-1==TOFFiberChk())
			{
				DEBUG_OUTPUT(("TOFFiberChk Failed !\n"));
				Push2ErrStack(PPCWARNING+TOF_FLG+ATOM_ERR+16,0,0,0,0);		/*tof error 16*/
				//return -1;
			}
		}
		
	DEBUG_OUTPUT(("TOFConfigedSYN1 OK\n")) ;
	return 0;
}
int  	TOFSynchronized1SYN2 	(int nArg) 
{
	DEBUG_OUTPUT(("TOFSynchronized1SYN2 OK\n")) ;
	return 0;
}
int  	TOFSynchronized2SYN3 	(int nArg) 
{
	//open L1 gate
	switch(g_nRunMode)
	{
		case NOMALMODE:
			if(0==g_TOFDBInfo.tof_ppc_header->NormModeSelfL1) //true run mode,not NormalMode selfL1
			{
				TOFRocStartL1();
			}
			break;
		case PEDMODE:
			TOFRocStartL1();
			break;
		case MONITOR:
			TOFRocStartL1();
			break;
		case CALIBRATION:
		case BASELINE:
		case WAVEMODE:		
			break ;
		default:
			//unknown running mode\0");
			Push2ErrStack(PPCFATAL+TOF_FLG+ATOM_ERR+9,0,0,0,0);		/*tof error 9*/
			return -1 ;
	}
	DEBUG_OUTPUT(("TOFSynchronized2SYN3 OK\n")) ;
	return 0;
}
int  	TOFSynchronized3SYN4 	(int nArg) 
{
	DEBUG_OUTPUT(("TOFSynchronized3SYN4 OK\n")) ;
	return 0;
}
int  	TOFSynchronized4PREP 	(int nArg) 
{
	//clear roc L1 num & FEE events
	TOFClearRocFEE();
/*
	if(TDIG_boardEn)
	{
		if (-1== TDIG_Clear(TDIG_geo<<20))
			return -1;
	}
*/	
	//spawn task
	
	if(g_TOFDBInfo.tof_FEE_sum)
	{
		if(-1 == (nCbltTaskId= taskSpawn("tFWCbltTrans", PRI_CBLTTRANS, 0, TASK_STACK_SIZE, (FUNCPTR)FWCbltTrans, 
								0, 0, 0, 0, 0, 0, 0, 0, 0, 0) )  )
		{
			Push2ErrStack(PPCFATAL+TOF_FLG+ATOM_ERR+17,0,0,0,0);		/*tof error 17*/
			DEBUG_OUTPUT(("CbltTrans taskSpawn failed\n")) ;
			return -1;
		}
		if(-1 == (nPackTaskId= taskSpawn("tTOFDataPack", PRI_DATAPACK, 0, TASK_STACK_SIZE, (FUNCPTR)TOFDataPack, 
								0, 0, 0, 0, 0, 0, 0, 0, 0, 0) )  )
		{
			Push2ErrStack(PPCFATAL+TOF_FLG+ATOM_ERR+18,0,0,0,0);		/*tof error 18*/
			DEBUG_OUTPUT(("Pack taskSpawn failed\n")) ;
			return -1;
		}
		if(-1 == (nNetTaskId= taskSpawn("tFWNetTrans", PRI_NETTRANS, 0, TASK_STACK_SIZE, (FUNCPTR)FWNetTrans, 
								0, 0, 0, 0, 0, 0, 0, 0, 0, 0) )  )
		{
			Push2ErrStack(PPCFATAL+TOF_FLG+ATOM_ERR+19,0,0,0,0);		/*tof error 19*/
			DEBUG_OUTPUT(("NetTrans taskSpawn failed\n")) ;
			return -1;
		}
	}
		
	switch(g_nRunMode)
	{
		case NOMALMODE:
		case CALIBRATION:
			if(g_TOFDBInfo.tof_FEE_sum)
			{
				if(ERROR == (nTOFGenL1TaskId=taskSpawn("tTOFGenL1", 180, 0, TASK_STACK_SIZE, (FUNCPTR)TOFGenL1, 
										0, 0, 0, 0, 0, 0, 0, 0, 0, 0) )  )
				{
					DEBUG_OUTPUT(("task TOFTrgGen spawn error\n"));
					Push2ErrStack(PPCFATAL+TOF_FLG+ATOM_ERR+20,0,0,0,0);		/*tof error 20*/
					return -1;
				}
			}
			break;
		case MONITOR:
		case PEDMODE:
		case BASELINE:
		case WAVEMODE:	
			break;
		default:
			//unknown running mode\0");
			Push2ErrStack(PPCFATAL+TOF_FLG+ATOM_ERR+9,0,0,0,0);		/*tof error 9*/
			break ;
	}
	DEBUG_OUTPUT(("TOFSynchronized4PREP OK\n")) ;
	return 0;
}
int  	TOFReadySATR  			(int nArg)
{
	sysIntEnable(TOF_INT_LEVEL);
	
	switch(g_nRunMode)
	{
		case NOMALMODE:
		case CALIBRATION:
			semGive(g_sidSATR) ;
			break;
		case MONITOR:
			TOFLaserPulseGen();
			break;
		case PEDMODE:
		case BASELINE:
		case WAVEMODE:		
			break ;
		default:
			//unknown running mode\0");
			Push2ErrStack(PPCFATAL+TOF_FLG+ATOM_ERR+9,0,0,0,0);		/*tof error 9*/
			break ;
	}
	
	DEBUG_OUTPUT(("TOFReadySATR OK\n")) ;
	return 0;
}
int  	TOFRunningSPTR  		(int nArg)
{
	int nClibMoniInfoLen;
	switch(g_nRunMode)
	{
		case NOMALMODE:
			if(g_TOFDBInfo.tof_ppc_header->NormModeSelfL1)
			{
				g_bTrgStop=TRUE;
			}
			break;
		case CALIBRATION:
			if(g_nCalibMoni_Points==1)
			{
				TOFRocStop();
			}
			if(g_nCalibMoni_Points!=1)
			{
				printf("g_nTrgCounter=%d\n",g_nTrgCounter);
				g_uTofCalibSignAmp=*(unsigned int *)g_cCalibMoni_Info;
				g_cCalibMoni_Info=g_cCalibMoni_Info+4;
				g_nL1Count=*(unsigned int *)g_cCalibMoni_Info;
				g_cCalibMoni_Info=g_cCalibMoni_Info+4;
				g_nCalibMoni_Points=g_nCalibMoni_Points-1;
				printf("new CalibSignAmp=%d,g_nL1Count=%d \n",g_uTofCalibSignAmp,g_nL1Count);
				//set new dac and clear events
				UINT SelfTestAmp=g_uTofCalibSignAmp;
				SelfTestAmp_Setup(SelfTestAmp);
			}
			else
			{
				g_cCalibMoni_Info  =g_cCalibMoni_Info_ori ;	

				nClibMoniInfoLen=*(unsigned int*)(g_cCalibMoni_Info)-4; 	//the exact calib|Monitor infor length			//other mode use default to config
				g_cCalibMoni_Info=g_cCalibMoni_Info+4;                 			//jump the length infor
				g_nCalibMoni_Points=nClibMoniInfoLen/8; //calib amp + runtimes


				
				g_uTofCalibSignAmp=*(unsigned int *)g_cCalibMoni_Info;
				g_cCalibMoni_Info=g_cCalibMoni_Info+4;
				g_nL1Count=*(unsigned int *)g_cCalibMoni_Info;
				g_cCalibMoni_Info=g_cCalibMoni_Info+4;

				//set new dac and clear events
				UINT SelfTestAmp=g_uTofCalibSignAmp;
				SelfTestAmp_Setup(SelfTestAmp);				
			}
			break;
		case MONITOR:
			TOFLaserPulseStop();
			taskDelay(1);
			
			if(g_nCalibMoni_Points!=1)
			{
				g_uTofLDPulseInterval=*(unsigned int *)g_cCalibMoni_Info;
				g_cCalibMoni_Info=g_cCalibMoni_Info+4;
				g_uTofLDPulseNum=*(unsigned int *)g_cCalibMoni_Info;
				g_cCalibMoni_Info=g_cCalibMoni_Info+4;
				g_uTofLDSolenoid=*(unsigned int *)g_cCalibMoni_Info;
				g_cCalibMoni_Info=g_cCalibMoni_Info+4;
				g_nL1Count=*(unsigned int *)g_cCalibMoni_Info;
				g_cCalibMoni_Info=g_cCalibMoni_Info+4;
				g_nCalibMoni_Points=g_nCalibMoni_Points-1;
				DEBUG_OUTPUT(("LDPulseInterval=%d,LDPulseNum=%d,LDSolenoid=%d\n",g_uTofLDPulseInterval,g_uTofLDPulseNum,g_uTofLDSolenoid));
			}
			break;
		case PEDMODE:
		case BASELINE:
		case WAVEMODE:		
			break ;
		default:
			//unknown running mode\0");
			Push2ErrStack(PPCFATAL+TOF_FLG+ATOM_ERR+9,0,0,0,0);		/*tof error 9*/
			break ;
	}
	
	DEBUG_OUTPUT(("TOFRunningSPTR OK\n")) ;
	return 0;
}
int  	TOFReadySTOP  			(int nArg)
{
	switch(g_nRunMode)
	{
		case NOMALMODE:
			TOFRocStop();
			break;
		case CALIBRATION:
			g_bTrgStop=true;
			semGive(g_sidSATR) ;
			break;
		case MONITOR:
			TOFRocStop();
			TOFLaserDeviceOff();
			break;
		case PEDMODE:
		case BASELINE:
		case WAVEMODE:		
			break ;
		default:
			//unknown running mode\0");
			Push2ErrStack(PPCFATAL+TOF_FLG+ATOM_ERR+9,0,0,0,0);		/*tof error 9*/
			break ;
	}
	DEBUG_OUTPUT(("TOFReadySTOP OK\n")) ;
	return 0;
}
int  	TOFConfigedUNCF 		(int nArg)
{
	DEBUG_OUTPUT(("TOFConfigedUNCF OK\n")) ;
	return 0;
}
int  	TOFPreconfigedUNPC 	(int nArg) 
{
	DEBUG_OUTPUT(("TOFPreconfigedUNPC OK\n")) ;
	return 0;
}
int  	TOFLoadedUNLD 			(int nArg)
{
	DEBUG_OUTPUT(("TOFLoadedUNLD OK\n")) ;
	return 0;
}
int  	TOFAnyEXIT 			(int nArg)
{
	if(nTOFGenL1TaskId)
	{
		if(ERROR==taskDelete(nTOFGenL1TaskId) )
		{
			printf("taskDelete(nTOFGenL1TaskId fail)\n");
			Push2ErrStack(PPCFATAL+TOF_FLG+ATOM_ERR+25,0,0,0,0);		/*tof error 25*/
			return -1;
		}
		nTOFGenL1TaskId=0;
	}
	DEBUG_OUTPUT(("TOFAnyEXIT OK\n")) ;
	return 0;
}
