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

#include "sd_etf.h"
#include "sd_etf_hw.h"
//#include "sd_etf_monitor.h"
#include "sd_etf_runtasks.h"
#include "sd_etf_actions.h"
#include "sd_etf_datapack.h"


int	ETFInitializedLOAD 		(int nArg) 
{
	char * pcMdlHeader ;
	int nMdlType;
	int nMdlIdx = 0 ;
	int nClibMoniInfoLen=0;

	sysIntDisable(ETF_INT_LEVEL) ;

	if((g_nCrateID<0xE0)||(g_nCrateID>=0xF0))
	{
		DEBUG_OUTPUT(("ETF CrateID out of 0xe0-0xf0\n")) ;
		Push2ErrStack(PPCFATAL+ETF_FLG+ATOM_ERR+1,0,0,0,0);		/*etf error 1*/
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
//			g_nCalibMoni_Points=nClibMoniInfoLen/16;//LDPulseInteval + LDPulseNum+LDSolidnoid+runtimes
			break ;
	}
	DEBUG_OUTPUT( ("g_nCalibMoni_Points=%d\n",g_nCalibMoni_Points) );
	pcMdlHeader = g_cCalibMoni_Info+nClibMoniInfoLen ;
	//----segment for Calib|Monitor infor


//
	/**get temp config paras**/
	char comment[1000];
	int opt[100]={0};
	int iopt=0;
	FILE* fOpts;
	int icnt = 0;
	int nRetryTimes = 0;
	while(1)
	{
		if ((fOpts = fopen("TDIGNum.txt","r")) == NULL)
		{
			icnt++;
			taskDelay(60);
			if(icnt%5==0)
			{
				printf("open file TDIGNum.txt failed %d times\n",icnt);
				/*break;*/
			}
		}
		else
		{
			printf("open file TDIGNum.txt succ\n");
			break;
		}
	}

	
	while ( 1 ) {
	if ( ! fgets(comment,1000,fOpts) ) break;
	if ( comment[0]=='#' ) continue;
	sscanf(comment,"0x%x",&opt[iopt]);
	iopt++;
	}
	fclose(fOpts);

	if(g_nCrateID==224)
		g_ETFDBInfo.etf_tdig_sum = opt[0];
	else
		g_ETFDBInfo.etf_tdig_sum = opt[1];
	printf("TDIG NUMBER = %d \n",g_ETFDBInfo.etf_tdig_sum);

//

	g_ETFDBInfo.etf_ppc_sum=0;
	//g_ETFDBInfo.etf_tdig_sum=1;
	g_ETFDBInfo.etf_roc_sum=1;
	g_ETFDBInfo.etf_clk_sum=1;
	int TDIGGeo[12]={4,5,6,7,8,9,13,14,16,17,18,19};
//	g_ETFDBInfo.etf_monitor_sum=0;
//	g_uEtfMonitorGeo=0;

	int nTempLen=0;
	for(nMdlIdx=0;nMdlIdx<g_nMdlSum;nMdlIdx++)
	{
		nMdlType =  (UINT)(*(UINT*)(pcMdlHeader + 4)) ;
		DEBUG_OUTPUT(("Module Type: 0x%x\n", nMdlType) ) ;
		switch(nMdlType)
		{
			case ETF_PPC:
			{
				g_ETFDBInfo.etf_ppc_header=(struct GE_ppc *)pcMdlHeader;	
				//g_ETFDBInfo.etf_ppc_header->NormModeSelfL1 = 0; 
				Show_ppc(g_ETFDBInfo.etf_ppc_header);
				
				g_ETFDBInfo.etf_ppc_sum++;
				if(g_ETFDBInfo.etf_ppc_sum>1)
				{
					DEBUG_OUTPUT( ("PPC config infor error,sum >1\n") );
					Push2ErrStack(PPCFATAL+ETF_FLG+ATOM_ERR+2,0,0,0,0);		/*etf error 2*/
					return -1;
				}
				break;
			}
			case ETF_TDIG :
			{
				g_ETFDBInfo.etf_TDIG_header[g_ETFDBInfo.etf_tdig_sum]=(struct etf_tdig *)pcMdlHeader;
				if((g_ETFDBInfo.etf_TDIG_header[g_ETFDBInfo.etf_tdig_sum]->geo)>256)
				{
					g_ETFDBInfo.etf_TDIG_header[g_ETFDBInfo.etf_tdig_sum]->geo&=0xff;
					Show_etf_tdig(g_ETFDBInfo.etf_TDIG_header[g_ETFDBInfo.etf_tdig_sum]);
					g_ETFDBInfo.etf_tdig_sum++;
				}
				else
				{
					DEBUG_OUTPUT( ("recv one disable TDIG config\n") );
				}
				break;
			}
			case ETF_ROC:
			{
				g_ETFDBInfo.etf_roc_header=(struct etf_roc *)pcMdlHeader;
				if((g_ETFDBInfo.etf_roc_header->geo)>256)
				{
					g_ETFDBInfo.etf_roc_header->geo&=0xff;
					Show_etf_roc(g_ETFDBInfo.etf_roc_header);
					g_ETFDBInfo.etf_roc_sum++;
				}
				else
				{
					DEBUG_OUTPUT( ("recv one disable ROC config\n") );
				}
				if(g_ETFDBInfo.etf_roc_sum>1)
				{
					DEBUG_OUTPUT( ("roc config infor error,sum >1\n") );
					Push2ErrStack(PPCFATAL+ETF_FLG+ATOM_ERR+3,0,0,0,0);		/*etf error 3*/
					return -1;
				}
				break;
			}
			case ETF_CLK:
			{
				g_ETFDBInfo.etf_clk_header=(struct etf_clk *)pcMdlHeader;
				
				if((g_ETFDBInfo.etf_clk_header->geo)>256)
				{
				
					g_ETFDBInfo.etf_clk_header->geo&=0xff;
					Show_etf_clk(g_ETFDBInfo.etf_clk_header);
					g_ETFDBInfo.etf_clk_sum++;
				}
				else
				{
					DEBUG_OUTPUT( ("recv one disable CLK config\n") );
				}
				if(g_ETFDBInfo.etf_clk_sum>1)
				{
					DEBUG_OUTPUT( ("clk config infor error,sum >1\n") );
					Push2ErrStack(PPCFATAL+ETF_FLG+ATOM_ERR+4,0,0,0,0);		/*etf error 4*/
					return -1;
				}
				break;
			}
/*			
		
			case ETF_MONITOR:
			{
				g_ETFDBInfo.etf_monitor_header=(struct etf_monitor*)pcMdlHeader;
				if((g_ETFDBInfo.etf_monitor_header->geo)>256)
				{
					g_ETFDBInfo.etf_monitor_header->geo&=0xff;
					
					Show_etf_monitor(g_ETFDBInfo.etf_monitor_header);
					g_ETFDBInfo.etf_monitor_sum++;
				}
				else
				{
					DEBUG_OUTPUT( ("recv one disable Monitor config\n") );
				}
				if(g_ETFDBInfo.etf_monitor_sum>1)
				{
					DEBUG_OUTPUT( ("monitor config infor error,sum >1\n") );
					Push2ErrStack(PPCFATAL+ETF_FLG+ATOM_ERR+5,0,0,0,0);		
					return -1;
				}
				break;
			}
*/
			default :
			{
				DEBUG_OUTPUT( ("Unknown module type\n") );
				Push2ErrStack(PPCFATAL+ETF_FLG+ATOM_ERR+6,0,0,0,0);		/*etf error 6*/
				return (-1);
			}
		}
		pcMdlHeader = pcMdlHeader +(UINT)(*(UINT*)pcMdlHeader) ;
		
	}


	//tmp jixl 20140304
	/*
	g_ETFDBInfo.etf_TDIG_header[0] = (struct etf_tdig*)malloc(sizeof(struct etf_tdig));
	if(g_ETFDBInfo.etf_TDIG_header[0] == NULL)
	{
		printf("malloc failed 1\n");
		return -1;
	}
	g_ETFDBInfo.etf_TDIG_header[0]->geo = 13;
	g_ETFDBInfo.etf_TDIG_header[0]->trigger_latency = 0x32F ;
	g_ETFDBInfo.etf_TDIG_header[0]->windowSet = 0x2D0;
	g_ETFDBInfo.etf_TDIG_header[0]->trigger_source = 0;
	g_ETFDBInfo.etf_TDIG_header[0]->errDataEnb = 0;
	DEBUG_OUTPUT(("TDIG 1 GEO :%d\n",g_ETFDBInfo.etf_TDIG_header[0]->geo));


	g_ETFDBInfo.etf_TDIG_header[1] = (struct etf_tdig*)malloc(sizeof(struct etf_tdig));
	if(g_ETFDBInfo.etf_TDIG_header[1] == NULL)
	{
		printf("malloc failed 1\n");
		return -1;
	}
	g_ETFDBInfo.etf_TDIG_header[1]->geo = 15;
	g_ETFDBInfo.etf_TDIG_header[1]->trigger_latency = 0x32F ;
	g_ETFDBInfo.etf_TDIG_header[1]->windowSet = 0x2D0;
	g_ETFDBInfo.etf_TDIG_header[1]->trigger_source = 0;
	g_ETFDBInfo.etf_TDIG_header[1]->errDataEnb = 0;
	DEBUG_OUTPUT(("TDIG 2 GEO :%d\n",g_ETFDBInfo.etf_TDIG_header[1]->geo));
*/
	for(int moduleIndx=0;moduleIndx<g_ETFDBInfo.etf_tdig_sum;moduleIndx++)
	{
		g_ETFDBInfo.etf_TDIG_header[moduleIndx] = (struct etf_tdig*)malloc(sizeof(struct etf_tdig));
		if(g_ETFDBInfo.etf_TDIG_header[moduleIndx] == NULL)
		{
			printf("malloc failed 1\n");
			return -1;
		}
		g_ETFDBInfo.etf_TDIG_header[moduleIndx]->geo = TDIGGeo[moduleIndx];
		//g_ETFDBInfo.etf_TDIG_header[moduleIndx]->trigger_latency = 0x32F ;
		//g_ETFDBInfo.etf_TDIG_header[moduleIndx]->windowSet = 0x2D0;
		g_ETFDBInfo.etf_TDIG_header[moduleIndx]->trigger_source = 0;
		g_ETFDBInfo.etf_TDIG_header[moduleIndx]->errDataEnb = 0;
		DEBUG_OUTPUT(("TDIG %d GEO :%d\n",moduleIndx,g_ETFDBInfo.etf_TDIG_header[moduleIndx]->geo));

	}



	g_ETFDBInfo.etf_roc_header = (struct etf_roc*)malloc(sizeof(etf_roc));
	if(g_ETFDBInfo.etf_roc_header == NULL)
	{
		printf("malloc failed 2\n");
		return -1;
	}	
	
	g_ETFDBInfo.etf_roc_header->geo = 11;
	g_ETFDBInfo.etf_roc_header->clksource = 1;
	g_ETFDBInfo.etf_roc_header->L1Latency = 0x100;
	g_ETFDBInfo.etf_roc_header->semiAutoL1Num = 4000;
	g_ETFDBInfo.etf_roc_header->autoL1Interval = 10000;//4KHZ	
	//g_ETFDBInfo.etf_roc_header->cttp_enable = 1;
	DEBUG_OUTPUT(("ROC GEO :%d\n",g_ETFDBInfo.etf_roc_header->geo));
	printf("ROC semiAutoL1Num is set to %d\n",g_ETFDBInfo.etf_roc_header->semiAutoL1Num);
	unsigned int ROC_freq;
	ROC_freq = (1000000000/(g_ETFDBInfo.etf_roc_header->autoL1Interval * 25)) ;
	printf("ROC L1 frequency is set to %d\n",ROC_freq);

	g_ETFDBInfo.etf_clk_header = (struct etf_clk*)malloc(sizeof(etf_clk));	
	if(g_ETFDBInfo.etf_clk_header == NULL)
	{
		printf("malloc failed 3\n");
		return -1;
	}	
	g_ETFDBInfo.etf_clk_header->geo = 21;
	//g_ETFDBInfo.etf_clk_header->clksource = 0;
	DEBUG_OUTPUT(("CLK GEO :%d\n",g_ETFDBInfo.etf_clk_header->geo));



	
	/*queue the FEE config info for create the map table*/
/*	
	for(int nQueIdx=0;nQueIdx<g_ETFDBInfo.etf_tdig_sum-1;nQueIdx++)
	{
		struct etf_tdig * pETF_FEETmp;
		for(int nQueIdx2=nQueIdx+1;nQueIdx2<g_ETFDBInfo.etf_tdig_sum;nQueIdx2++)
		{
			if((g_ETFDBInfo.etf_TDIG_header[nQueIdx]->geo)>(g_ETFDBInfo.etf_TDIG_header[nQueIdx2]->geo))
			{
				pETF_FEETmp=g_ETFDBInfo.etf_TDIG_header[nQueIdx];
				g_ETFDBInfo.etf_TDIG_header[nQueIdx]=g_ETFDBInfo.etf_TDIG_header[nQueIdx2];
				g_ETFDBInfo.etf_TDIG_header[nQueIdx2]=pETF_FEETmp;
			}
		}
	}
*/
/*
	for(int nQueResult=0;nQueResult<g_ETFDBInfo.etf_tdig_sum;nQueResult++)
	{
		DEBUG_OUTPUT(("queue of GEO :%d\n",g_ETFDBInfo.etf_TDIG_header[nQueResult]->geo));
	}
*/	
/*	
	//set g_uEtfMonitorGeo value
	if(g_ETFDBInfo.etf_monitor_sum==1)
	{
		g_uEtfMonitorGeo=g_ETFDBInfo.etf_monitor_header->geo;
	}
	else
	{
		g_uEtfMonitorGeo=0;
	}
*/



// here create the cblt module list geo,and with geo sequence

	g_uEtfCbltModuleSum=0;	//reset CBLT Moudle NUM=0;
	for(nMdlIdx=0;nMdlIdx<g_ETFDBInfo.etf_tdig_sum;nMdlIdx++)
	{
		g_uEtfCbltGeo[nMdlIdx]=g_ETFDBInfo.etf_TDIG_header[nMdlIdx]->geo;
	}
	g_uEtfCbltModuleSum=g_ETFDBInfo.etf_tdig_sum;



	/****/
	memset(g_nConverTable,0xfff,sizeof(g_nConverTable));
	if(g_nCrateID==224)
		initial_convert_table_224();
	else
		initial_convert_table_225();
	

/*
	//if monior mode and have monitor module,then plus monitor moudle to cblt
	//insert monitor module to list
	if( (1==g_ETFDBInfo.etf_monitor_sum)&&((g_nRunMode==CALIBRATION)||(g_nRunMode==MONITOR)))
	{
		nMdlIdx=g_ETFDBInfo.etf_tdig_sum-1;
		while( (nMdlIdx>=0)&&(g_uEtfCbltGeo[nMdlIdx]> g_uEtfMonitorGeo))//get the monitor posistion should be where
		{
			g_uEtfCbltGeo[nMdlIdx+1]=g_uEtfCbltGeo[nMdlIdx];
			nMdlIdx--;
		}
		nMdlIdx++;
		g_uEtfCbltGeo[nMdlIdx]=g_uEtfMonitorGeo;
		g_uEtfCbltModuleSum++;					//plus monitor module
	}
	printf("g_uEtfCbltModuleSum=%d\n",g_uEtfCbltModuleSum);
*/	
	/*compare the PPC id in db and jumper*/
	if ( g_nPPCIDOfSRH!= g_ETFDBInfo.etf_ppc_header->PPC_id)
	{
		Push2ErrStack(PPCFATAL+ETF_FLG+ATOM_ERR+7,0,0,0,0);		/*etf error 7*/
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
			if(ETFConfigIntCBLT()==-1)
			{
				Push2ErrStack(PPCFATAL+ETF_FLG+ATOM_ERR+8,0,0,0,0);		/*etf error 8*/
				DEBUG_OUTPUT(("ETFConfigIntCBLT Error\n"));
				return -1;
			}
			break ;
		default:
			Push2ErrStack(PPCFATAL+ETF_FLG+ATOM_ERR+9,0,0,0,0);		/*etf error 9*/
			DEBUG_OUTPUT(("unknown running mode\n"));
			return -1;
	}
	printf("ETFInitializedLOAD OK\n") ;
	taskDelay(120);
	return 0;
}
int  	ETFLoadedPRCF 			(int nArg) 
{
	/*connect data socket*/
	unsigned short nDataPort = g_ETFDBInfo.etf_ppc_header->ROS_port ;
	unsigned int	 nRemoteIP = g_ETFDBInfo.etf_ppc_header->ROS_ip;
	DEBUG_OUTPUT(("ROS IP = %x, PORT = %d.\n",nRemoteIP, nDataPort)) ;
	if(DataSockConnect(nRemoteIP,nDataPort)==-1)
	{
		DEBUG_OUTPUT(("connect data socket error\n"));
		Push2ErrStack(PPCFATAL+ETF_FLG+ATOM_ERR+10,0,0,0,0);		/*etf error 10*/
		CloseDataSocket();
		return -1;
	}
	
	DEBUG_OUTPUT(("ETFLoadedPRCF OK\n")) ;
	return 0;
}
int  	ETFPreconfigedCONF 	(int nArg) 
{
	int nModuIdx;
	int nChnlIdx;

	switch(g_nRunMode)
	{
		case NOMALMODE:
		case PEDMODE:
		case BASELINE:
		case WAVEMODE:
			g_uEtfCalibSignAmp=0xffff;	//this parameter only have meaning at calibmode
			break;
		case MONITOR:
/*
			g_uEtfLDPulseInterval=*(unsigned int *)g_cCalibMoni_Info;
			g_cCalibMoni_Info=g_cCalibMoni_Info+4;
			g_uEtfLDPulseNum=*(unsigned int *)g_cCalibMoni_Info;
			g_cCalibMoni_Info=g_cCalibMoni_Info+4;
			g_uEtfLDSolenoid=*(unsigned int *)g_cCalibMoni_Info;
			g_cCalibMoni_Info=g_cCalibMoni_Info+4;
			g_nL1Count=*(unsigned int *)g_cCalibMoni_Info;
			g_cCalibMoni_Info=g_cCalibMoni_Info+4;
			DEBUG_OUTPUT(("LDPulseInterval=%d,LDPulseNum=%d,LDSolenoid=%d\n",g_uEtfLDPulseInterval,g_uEtfLDPulseNum,g_uEtfLDSolenoid));
*/
			break ;
		case CALIBRATION:
			{
				g_uEtfCalibSignAmp=*(unsigned int *)g_cCalibMoni_Info;
				g_cCalibMoni_Info=g_cCalibMoni_Info+4;
				g_nL1Count=*(unsigned int *)g_cCalibMoni_Info;
				g_cCalibMoni_Info=g_cCalibMoni_Info+4;
				DEBUG_OUTPUT(("ETF CalibDAC=%d,g_nL1Count=%d \n",g_uEtfCalibSignAmp,g_nL1Count));
			}
			break ;
		default:
			Push2ErrStack(PPCFATAL+ETF_FLG+ATOM_ERR+9,0,0,0,0);		/*etf error 9*/
			DEBUG_OUTPUT(("unknown running mode\n")) ;
			return -1;
	}

/*	
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
*/
	switch(g_nRunMode)
	{
		case NOMALMODE:
			if((g_nTrgPerInt<1)||(g_nTrgPerInt>16))
			{
				DEBUG_OUTPUT(("normal mode,TrgPerInt must between 1-16.\n"));
				Push2ErrStack(PPCFATAL+ETF_FLG+ATOM_ERR+12,0,0,0,0);		/*etf error 12*/
				return -1;
			}
			
			break;
		case PEDMODE:
		case CALIBRATION:
		case MONITOR:
		case BASELINE:
		case WAVEMODE:
			if(g_nTrgPerInt!=1)
			{
				DEBUG_OUTPUT(("Non normal mode,TrgPerInt must == 1\n"));
				Push2ErrStack(PPCFATAL+ETF_FLG+ATOM_ERR+13,0,0,0,0);		/*etf error 13*/
				return -1;
			}
/*			
			for(nModuIdx = 0; nModuIdx <g_ETFDBInfo.etf_tdig_sum; nModuIdx ++) 
			{
				UINT nPosition = g_ETFDBInfo.etf_TDIG_header[nModuIdx]->geo;
				for(nChnlIdx = 0; nChnlIdx < 16; nChnlIdx ++)
				{
					g_nConvertTable[nPosition][nChnlIdx] =(nPosition<<25)+(nChnlIdx<<21);
				}
			}
*/
			/*
			//init monitor ConvertTable special
			if( 1==g_ETFDBInfo.etf_monitor_sum )
			{
				for(nChnlIdx = 0; nChnlIdx < 16; nChnlIdx ++)
				{
					g_nConvertTable[g_uEtfMonitorGeo][nChnlIdx] =(g_uEtfMonitorGeo<<25)+(nChnlIdx<<21);//for up system to distinguish Monitor data
				}
			}*/
			break;
		default:
			//unknown running mode\0");
			Push2ErrStack(PPCFATAL+ETF_FLG+ATOM_ERR+9,0,0,0,0);		/*etf error 9*/
			return -1;
	}

	if(-1==ETFConfigAll())
	{
		DEBUG_OUTPUT(("ETFConfigAll Failed !\n"));
		Push2ErrStack(PPCFATAL+ETF_FLG+ATOM_ERR+15,0,0,0,0);		/*etf error 15*/
		return -1;
	}

	DEBUG_OUTPUT(("ETFPreconfiged OK\n")) ;
	return 0;
}
int  	ETFConfigedSYN1 		(int nArg) 
{
	DEBUG_OUTPUT(("ETFConfigedSYN1 OK\n")) ;
	return 0;
}
int  	ETFSynchronized1SYN2 	(int nArg) 
{
	DEBUG_OUTPUT(("ETFSynchronized1SYN2 OK\n")) ;
	return 0;
}
int  	ETFSynchronized2SYN3 	(int nArg) 
{
	//open L1 gate
	switch(g_nRunMode)
	{
		case NOMALMODE:
			if(0==g_ETFDBInfo.etf_ppc_header->NormModeSelfL1) //true run mode,not NormalMode selfL1
			{
				ETFRocStartL1();
			}
			break;
		case PEDMODE:
			ETFRocStartL1();
			break;
		case MONITOR:
			ETFRocStartL1();
			break;
		case CALIBRATION:
		case BASELINE:
		case WAVEMODE:		
			break ;
		default:
			//unknown running mode\0");
			Push2ErrStack(PPCFATAL+ETF_FLG+ATOM_ERR+9,0,0,0,0);		/*etf error 9*/
			return -1 ;
	}
	DEBUG_OUTPUT(("ETFSynchronized2SYN3 OK\n")) ;
	return 0;
}
int  	ETFSynchronized3SYN4 	(int nArg) 
{
	DEBUG_OUTPUT(("ETFSynchronized3SYN4 OK\n")) ;
	return 0;
}
int  	ETFSynchronized4PREP 	(int nArg) 
{
	//clear roc L1 num & FEE events
	ETFClearRocFEE();
	
	//spawn task
	
	if(g_ETFDBInfo.etf_tdig_sum)
	{
		if(-1 == (nCbltTaskId= taskSpawn("tFWCbltTrans", PRI_CBLTTRANS, 0, TASK_STACK_SIZE, (FUNCPTR)FWCbltTrans, 
								0, 0, 0, 0, 0, 0, 0, 0, 0, 0) )  )
		{
			Push2ErrStack(PPCFATAL+ETF_FLG+ATOM_ERR+17,0,0,0,0);		/*etf error 17*/
			DEBUG_OUTPUT(("CbltTrans taskSpawn failed\n")) ;
			return -1;
		}
		if(-1 == (nPackTaskId= taskSpawn("tETFDataPack", PRI_DATAPACK, 0, TASK_STACK_SIZE, (FUNCPTR)ETFDataPack, 
								0, 0, 0, 0, 0, 0, 0, 0, 0, 0) )  )
		{
			Push2ErrStack(PPCFATAL+ETF_FLG+ATOM_ERR+18,0,0,0,0);		/*etf error 18*/
			DEBUG_OUTPUT(("Pack taskSpawn failed\n")) ;
			return -1;
		}
		if(-1 == (nNetTaskId= taskSpawn("tFWNetTrans", PRI_NETTRANS, 0, TASK_STACK_SIZE, (FUNCPTR)FWNetTrans, 
								0, 0, 0, 0, 0, 0, 0, 0, 0, 0) )  )
		{
			Push2ErrStack(PPCFATAL+ETF_FLG+ATOM_ERR+19,0,0,0,0);		/*etf error 19*/
			DEBUG_OUTPUT(("NetTrans taskSpawn failed\n")) ;
			return -1;
		}
	}
		
	switch(g_nRunMode)
	{
		case NOMALMODE:
		case CALIBRATION:
			if(g_ETFDBInfo.etf_tdig_sum)
			{
				if(ERROR == (nETFGenL1TaskId=taskSpawn("tETFGenL1", 180, 0, TASK_STACK_SIZE, (FUNCPTR)ETFGenL1, 
										0, 0, 0, 0, 0, 0, 0, 0, 0, 0) )  )
				{
					DEBUG_OUTPUT(("task ETFTrgGen spawn error\n"));
					Push2ErrStack(PPCFATAL+ETF_FLG+ATOM_ERR+20,0,0,0,0);		/*etf error 20*/
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
			Push2ErrStack(PPCFATAL+ETF_FLG+ATOM_ERR+9,0,0,0,0);		/*etf error 9*/
			break ;
	}
	DEBUG_OUTPUT(("ETFSynchronized4PREP OK\n")) ;
	return 0;
}
int  	ETFReadySATR  			(int nArg)
{
	sysIntEnable(ETF_INT_LEVEL);
	
	switch(g_nRunMode)
	{
		case NOMALMODE:
		case CALIBRATION:
			semGive(g_sidSATR) ;
			break;
		case MONITOR:
//			ETFLaserPulseGen();
			break;
		case PEDMODE:
		case BASELINE:
		case WAVEMODE:		
			break ;
		default:
			//unknown running mode\0");
			Push2ErrStack(PPCFATAL+ETF_FLG+ATOM_ERR+9,0,0,0,0);		/*etf error 9*/
			break ;
	}
	
	DEBUG_OUTPUT(("ETFReadySATR OK\n")) ;
	return 0;
}
int  	ETFRunningSPTR  		(int nArg)
{
	int nClibMoniInfoLen;
	switch(g_nRunMode)
	{
		case NOMALMODE:
			if(g_ETFDBInfo.etf_ppc_header->NormModeSelfL1)
			{
				g_bTrgStop=TRUE;
			}
			break;
		case CALIBRATION:
			if(g_nCalibMoni_Points==1)
			{
				ETFRocStop();
			}
			if(g_nCalibMoni_Points!=1)
			{
				printf("g_nTrgCounter=%d\n",g_nTrgCounter);
				g_uEtfCalibSignAmp=*(unsigned int *)g_cCalibMoni_Info;
				g_cCalibMoni_Info=g_cCalibMoni_Info+4;
				g_nL1Count=*(unsigned int *)g_cCalibMoni_Info;
				g_cCalibMoni_Info=g_cCalibMoni_Info+4;
				g_nCalibMoni_Points=g_nCalibMoni_Points-1;
				printf("new CalibSignAmp=%d,g_nL1Count=%d \n",g_uEtfCalibSignAmp,g_nL1Count);
				//set new dac and clear events
				UINT SelfTestAmp=g_uEtfCalibSignAmp;
				ETFSelfTestAmp_Setup(SelfTestAmp);
			}
			else
			{
				g_cCalibMoni_Info  =g_cCalibMoni_Info_ori ;	

				nClibMoniInfoLen=*(unsigned int*)(g_cCalibMoni_Info)-4; 	//the exact calib|Monitor infor length			//other mode use default to config
				g_cCalibMoni_Info=g_cCalibMoni_Info+4;                 			//jump the length infor
				g_nCalibMoni_Points=nClibMoniInfoLen/8; //calib amp + runtimes


				
				g_uEtfCalibSignAmp=*(unsigned int *)g_cCalibMoni_Info;
				g_cCalibMoni_Info=g_cCalibMoni_Info+4;
				g_nL1Count=*(unsigned int *)g_cCalibMoni_Info;
				g_cCalibMoni_Info=g_cCalibMoni_Info+4;

				//set new dac and clear events
				UINT SelfTestAmp=g_uEtfCalibSignAmp;
				ETFSelfTestAmp_Setup(SelfTestAmp);				
			}
			break;
		case MONITOR:
			g_bTrgStop=TRUE;			
			break;
		case PEDMODE:
		case BASELINE:
		case WAVEMODE:		
			break ;
		default:
			//unknown running mode\0");
			Push2ErrStack(PPCFATAL+ETF_FLG+ATOM_ERR+9,0,0,0,0);		/*etf error 9*/
			break ;
	}
	
	DEBUG_OUTPUT(("ETFRunningSPTR OK\n")) ;
	return 0;
}
int  	ETFReadySTOP  			(int nArg)
{
	switch(g_nRunMode)
	{
		case NOMALMODE:
			ETFRocStop();
			break;
		case CALIBRATION:
			g_bTrgStop=true;
			semGive(g_sidSATR) ;
			break;
		case MONITOR:
//			ETFRocStop();
//			ETFLaserDeviceOff();
			break;
		case PEDMODE:
		case BASELINE:
		case WAVEMODE:		
			break ;
		default:
			//unknown running mode\0");
			Push2ErrStack(PPCFATAL+ETF_FLG+ATOM_ERR+9,0,0,0,0);		/*etf error 9*/
			break ;
	}
	DEBUG_OUTPUT(("ETFReadySTOP OK\n")) ;
	return 0;
}
int  	ETFConfigedUNCF 		(int nArg)
{
	DEBUG_OUTPUT(("ETFConfigedUNCF OK\n")) ;
	return 0;
}
int  	ETFPreconfigedUNPC 	(int nArg) 
{
	DEBUG_OUTPUT(("ETFPreconfigedUNPC OK\n")) ;
	return 0;
}
int  	ETFLoadedUNLD 			(int nArg)
{
	free(g_ETFDBInfo.etf_TDIG_header[0]);
	free(g_ETFDBInfo.etf_roc_header);
	free(g_ETFDBInfo.etf_clk_header);
	DEBUG_OUTPUT(("ETFLoadedUNLD OK\n")) ;
	return 0;
}
int  	ETFAnyEXIT 			(int nArg)
{
	if(nETFGenL1TaskId)
	{
		if(ERROR==taskDelete(nETFGenL1TaskId) )
		{
			printf("taskDelete(nETFGenL1TaskId fail)\n");
			Push2ErrStack(PPCFATAL+ETF_FLG+ATOM_ERR+25,0,0,0,0);		/*etf error 25*/
			return -1;
		}
		nETFGenL1TaskId=0;
	}
	DEBUG_OUTPUT(("ETFAnyEXIT OK\n")) ;
	return 0;
}
