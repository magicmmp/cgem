/*sd_empty.cpp, test an empty system*/

#include "vxWorks.h"
#include "stdio.h"
#include "taskLib.h"
#include "sysLib.h"
#include "usrLib.h"

#include "fw_configinfo.h"
#include "fw_roppc.h"
#include "fw_dcgvas.h"
#include "fw_define.h"
#include "fw_actions.h"
#include "fw_runtasks.h"
#include "fw_staterrspt.h"
#include "fw_filefunc.h"
#include "fw_tcpfunc.h"

#include "sd_emc.h"
#include "sd_emc_actions.h"
#include "sd_emc_hw.h"
#include "sd_emc_runtasks.h"

int	EMCInitializedLOAD 			(int nArg) 
{
	char * pcMdlHeader ;
	UINT  nMdlType;
	int nMdlIdx = 0 ;
	int nClibInfoLen;


	sysIntDisable(EMC_INT_LEVEL) ;

	
	if((g_nCrateID<0x60)||(g_nCrateID>=0x80))
	{
		DEBUG_OUTPUT(("EMC CrateID out of 0x60-0x80\n")) ;
		Push2ErrStack(PPCFATAL+EMC_FLG+ATOM_ERR+1,0,0,0,0);		/*emc error 1*/
		return -1;
	}
	//----segment for Calib&Monitor infor
	g_cCalibMoni_Info=((char*)g_pCrateConfigHeader) + g_pCrateConfigHeader->crate_len ;	//calib mode ,all use these infor to config
	g_cCalibMoni_Info_ori = g_cCalibMoni_Info;
	
	nClibInfoLen=*(unsigned int*)(g_cCalibMoni_Info)-4; 	//the exact calib infor length			//other mode use default to config
	g_cCalibMoni_Info=g_cCalibMoni_Info+4;                 			//jump the length infor
	g_nCalibMoni_Points=nClibInfoLen/8;		
	DEBUG_OUTPUT( ("ClibInfoLen=%d\n",nClibInfoLen) );
	DEBUG_OUTPUT( ("g_nCalibMoni_Points=%d\n",g_nCalibMoni_Points) );
	//----segment for Calib&Monitor infor

	
	//reset module sum in DBinfo
	g_EMCDBInfo.emc_ppc_sum=0;
	g_EMCDBInfo.emc_Q_sum=0;
	g_EMCDBInfo.emc_roc_sum=0;
	g_EMCDBInfo.emc_fanout_sum=0;
	g_EMCDBInfo.emc_trgtcba_sum=0;
	g_EMCDBInfo.emc_monitor_sum = 0;

	nMonitorStep = 8;
	nLEDCount = 0;
	g_bEMCMonitor = false;
	g_bEMCMonictrl = false;

	g_nTCheckAmpMode = 0;
	if((g_pCrateConfigHeader->unused)&0x2)	
	{
		g_nTCheckAmpMode = 1;//by jak
		printf("******AMP GAIN CALIB MODE*******\n");
	}
	//----begin config informaton
	pcMdlHeader = g_cCalibMoni_Info+nClibInfoLen ;
	
	for(nMdlIdx=0;nMdlIdx<g_nMdlSum;nMdlIdx++)
	{
		nMdlType =  (UINT)(*(UINT*)(pcMdlHeader + 4)) ;
		DEBUG_OUTPUT(("Module Type: 0x%x\n", nMdlType) ) ;
		
		switch(nMdlType)
		{
			case EMC_PPC:
			{
				g_EMCDBInfo.emc_ppc_header=(struct GE_ppc *)pcMdlHeader;
				
				Show_ppc(g_EMCDBInfo.emc_ppc_header);
				
				g_EMCDBInfo.emc_ppc_sum++;
				if(g_EMCDBInfo.emc_ppc_sum>1)
				{
					DEBUG_OUTPUT(("PPC sum>1,emc_ppc_sum=%d\n",g_EMCDBInfo.emc_ppc_sum ) );
					Push2ErrStack(PPCFATAL+EMC_FLG+ATOM_ERR+2,0,0,0,0);		/*emc error 2*/
					return -1;
				}
				break;
			}
			case EMC_Q :
			{
				g_EMCDBInfo.emc_Q_header[g_EMCDBInfo.emc_Q_sum]=(struct emc_Q *)pcMdlHeader;

				if((g_EMCDBInfo.emc_Q_header[g_EMCDBInfo.emc_Q_sum]->geo)<256)
				{
					QDisableGeo[nDisableIdx] = g_EMCDBInfo.emc_Q_header[g_EMCDBInfo.emc_Q_sum]->geo;
					nDisableIdx++;
					DEBUG_OUTPUT(("A EMC_Q module config disable\n") );
					Push2ErrStack(PPCWARNING+EMC_FLG+ATOM_ERR+3,0,0,0,0);		/*emc error 3*/
				}
				if((g_EMCDBInfo.emc_Q_header[g_EMCDBInfo.emc_Q_sum]->geo)>=256)
				{
					g_EMCDBInfo.emc_Q_header[g_EMCDBInfo.emc_Q_sum]->geo&=0xff;

					Show_emc_Q(g_EMCDBInfo.emc_Q_header[g_EMCDBInfo.emc_Q_sum]);
					
					g_EMCDBInfo.emc_Q_sum++;
				}
				break;
			}
			case EMC_ROC:
			{
				g_EMCDBInfo.emc_roc_header=(struct emc_roc *)pcMdlHeader;

				if((g_EMCDBInfo.emc_roc_header->geo)<256)
				{
					DEBUG_OUTPUT(("one emc_roc module disable!!\n") );
				}
				if((g_EMCDBInfo.emc_roc_header->geo)>=256)
				{
					g_EMCDBInfo.emc_roc_header->geo&=0xff;
					
					Show_emc_roc(g_EMCDBInfo.emc_roc_header);

					g_EMCDBInfo.emc_roc_sum++;
				}
				
				if(g_EMCDBInfo.emc_roc_sum>1)
				{
					DEBUG_OUTPUT(("emc_roc_sum=%d,config infor error\n",g_EMCDBInfo.emc_roc_sum ));
					Push2ErrStack(PPCFATAL+EMC_FLG+ATOM_ERR+4,0,0,0,0);		/*emc error 4*/
					return -1;
				}
				break;
			}
			case EMC_FANOUT:
			{
				g_EMCDBInfo.emc_fanout_header=(struct emc_fanout *)pcMdlHeader;

				if((g_EMCDBInfo.emc_fanout_header->geo)<256)
				{
					DEBUG_OUTPUT(("one emc_fanout module disable!!\n") );
				}
				if((g_EMCDBInfo.emc_fanout_header->geo)>=256)
				{
					g_EMCDBInfo.emc_fanout_header->geo&=0xff;
					
					Show_emc_fanout(g_EMCDBInfo.emc_fanout_header);
					
					g_EMCDBInfo.emc_fanout_sum++;
				}
				
				if(g_EMCDBInfo.emc_fanout_sum>1)
				{
					DEBUG_OUTPUT(("emc_fanout_sum=%d,config infor error\n",g_EMCDBInfo.emc_fanout_sum ));
					Push2ErrStack(PPCFATAL+EMC_FLG+ATOM_ERR+5,0,0,0,0);		/*emc error 5*/
					return -1;
				}
				break;
			}
			case EMC_MONITOR:
			{
				g_EMCDBInfo.emc_monitor_header=(struct emc_monitor *)pcMdlHeader;
//				d( (UINT32)(pcMdlHeader ), 1692>>2, 4) ;

				if((g_EMCDBInfo.emc_monitor_header->geo)<256)
				{
					DEBUG_OUTPUT(("one emc_monitor module disable!!\n") );
				}
				if((g_EMCDBInfo.emc_monitor_header->geo)>=256)
				{
					g_EMCDBInfo.emc_monitor_header->geo&=0xff;
					
					Show_emc_monitor(g_EMCDBInfo.emc_monitor_header);
					
					g_EMCDBInfo.emc_monitor_sum++;
				}
				
				if(g_EMCDBInfo.emc_monitor_sum>1)
				{
					DEBUG_OUTPUT(("emc_monitor_sum=%d,config infor error\n",g_EMCDBInfo.emc_monitor_sum ));
					Push2ErrStack(PPCFATAL+EMC_FLG+ATOM_ERR+67,0,0,0,0);		/*emc error 67*/
					return -1;
				}
				break;
			}			
			case EMC_TRGTCBA:
			{
				g_EMCDBInfo.emc_trgtcba_header=(struct emc_trgtcba *)pcMdlHeader;
				Show_emc_trgtcba(g_EMCDBInfo.emc_trgtcba_header);
				if((g_EMCDBInfo.emc_trgtcba_header->geo)<256)
				{
					DEBUG_OUTPUT(("one emc trgtcba module may disable!!\n") );
				}
				
				if((g_EMCDBInfo.emc_trgtcba_header->geo)>=256)
				{
					g_EMCDBInfo.emc_trgtcba_header->geo&=0xff;
					g_EMCDBInfo.emc_trgtcba_sum++;
				}
				if(g_EMCDBInfo.emc_trgtcba_sum>1)
				{
					DEBUG_OUTPUT(("emc_trgtcba_sum=%d,config infor error\n",g_EMCDBInfo.emc_trgtcba_sum ));
					Push2ErrStack(PPCFATAL+EMC_FLG+ATOM_ERR+68,0,0,0,0);		/*emc error 68*/
					return -1;
				}
				break;
			}
			default :
			{
				DEBUG_OUTPUT(("Unknown module type\n"));
				Push2ErrStack(PPCERROR+EMC_FLG+ATOM_ERR+6,0,0,0,0);		/*emc error 6*/
				return (-1);
			}
		}
		//jump to next module infor,phead+length
		pcMdlHeader =pcMdlHeader + (UINT)(*(UINT*)pcMdlHeader)  ;
		
	}
	
/******EMC-Q*******
	unsigned int	info_len;//renzy:in bytes
	unsigned int	module_type;//renzy:e.g EMC-Q 0x62
	unsigned int	geo;
	unsigned int	sample_win;
	unsigned int	trigger_latency;
	unsigned int  	outL1;
	unsigned int	unused;	
	unsigned int	map[32];
	unsigned int	QThreshold[32];
	unsigned int 	MainAmpGeo[32];	//but config throuth ROC module
	unsigned int 	GainValue[32];	//but config throuth ROC module
	float			low_adjust_a2[32];
	float			low_adjust_a1[32];
	float			low_adjust_a0[32];	
	float			mid_adjust_a2[32];
	float			mid_adjust_a1[32];
	float			mid_adjust_a0[32];	
	float			high_adjust_a2[32];
	float			high_adjust_a1[32];
	float			high_adjust_a0[32];	
****/
	int itemp;
	if(g_EMCDBInfo.emc_monitor_sum)
	{
		switch(g_nRunMode)
		{
			case NOMALMODE:
				break;
			case MONITOR:
			case CALIBRATION:		
			case PEDMODE:
			case BASELINE:
			case WAVEMODE:
				g_EMCDBInfo.emc_Q_header[g_EMCDBInfo.emc_Q_sum]= (emc_Q *)g_EMCDBInfo.emc_monitor_header;
//				printf("g_EMCDBInfo.emc_Q_sum = %d\n",g_EMCDBInfo.emc_Q_sum);
				g_EMCDBInfo.emc_Q_sum++;			
				break;
			default:
				Push2ErrStack(PPCERROR+EMC_FLG+ATOM_ERR+8,0,0,0,0);			/*emc error 8*/
				return -1;
		}	
	}
	
	//queue EMC Q config infor with GEO
	for(int nQueIdx=0;nQueIdx<g_EMCDBInfo.emc_Q_sum-1;nQueIdx++)
	{
		struct emc_Q * pEMC_QTmp;
		for(int nQueIdx2=nQueIdx+1;nQueIdx2<g_EMCDBInfo.emc_Q_sum;nQueIdx2++)
		{
			if((g_EMCDBInfo.emc_Q_header[nQueIdx]->geo)>(g_EMCDBInfo.emc_Q_header[nQueIdx2]->geo))
			{
				pEMC_QTmp=g_EMCDBInfo.emc_Q_header[nQueIdx];
				g_EMCDBInfo.emc_Q_header[nQueIdx]=g_EMCDBInfo.emc_Q_header[nQueIdx2];
				g_EMCDBInfo.emc_Q_header[nQueIdx2]=pEMC_QTmp;
			}
		}
	}

	//compare the PPC ID in db with jumper on board
	if ( g_nPPCIDOfSRH!= g_EMCDBInfo.emc_ppc_header->PPC_id)
	{
		DEBUG_OUTPUT(("PPC id not equal SRH\n"));
		Push2ErrStack(PPCERROR+EMC_FLG+ATOM_ERR+7,0,0,0,0);		/*emc error 7*/
		return -1 ;
	}

	//config interrupt CBLT func
	if(EMCConfigIntCBLT()==-1)
	{
		DEBUG_OUTPUT(("config cblt error\n"));
		Push2ErrStack(PPCERROR+EMC_FLG+ATOM_ERR+39,0,0,0,0);		/*emc error 39*/
		return -1;
	}
	printf("EMCInitializedLOAD OK\n") ;
	return 0;
}
int  	EMCLoadedPRCF 			(int nArg) 
{
	//connect NonBlock data socket,with IP,PORT
	unsigned int	 nRemoteIP = g_EMCDBInfo.emc_ppc_header->ROS_ip;
	unsigned short nDataPort = g_EMCDBInfo.emc_ppc_header->ROS_port ;
	DEBUG_OUTPUT(("remote IP is: 0x%x,Port: %d\n",nRemoteIP,nDataPort));
	if(DataSockConnect(nRemoteIP,nDataPort)==-1)
	{
		DEBUG_OUTPUT(("connect data sock error\n"));
		Push2ErrStack(PPCERROR+EMC_FLG+ATOM_ERR+40,0,0,0,0);		/*emc error 40*/
		CloseDataSocket();
		return -1;
	}
	
	//hi,jak
	printf("g_nTCheckAmpMode=%d\n",g_nTCheckAmpMode);
	if(1==g_nTCheckAmpMode)
	{
		if( (g_sidTCBAInterrupt= semBCreate(SEM_Q_PRIORITY, SEM_EMPTY))==NULL)
		{
			DEBUG_OUTPUT(("semBCreate g_sidTCBAInterrupt failed\n")) ;
			Push2ErrStack(PPCFATAL+EMC_FLG+ATOM_ERR+69,0,0,0,0);				//emc error 69
			return -1 ;
		}
	}
	//bye,jak

	switch(g_nRunMode)
	{
		case NOMALMODE:
		case PEDMODE:
			g_nEmcDAC=0xffff;	//these two parameter only have meaning at calibmode
			break ;
		case CALIBRATION:
			{
				g_nEmcDAC=*(unsigned int *)g_cCalibMoni_Info;
				g_cCalibMoni_Info=g_cCalibMoni_Info+4;
				g_nL1Count=*(unsigned int *)g_cCalibMoni_Info;
				g_cCalibMoni_Info=g_cCalibMoni_Info+4;
				DEBUG_OUTPUT(("EMCDAC=%d,g_nL1Count=%d \n",g_nEmcDAC,g_nL1Count));
			}
			break ;			
		case MONITOR:
			{
				g_nEmcDAC=*(unsigned int *)g_cCalibMoni_Info;
				g_cCalibMoni_Info=g_cCalibMoni_Info+4;
				g_nL1Count=*(unsigned int *)g_cCalibMoni_Info;
				g_cCalibMoni_Info=g_cCalibMoni_Info+4;
				DEBUG_OUTPUT(("nMonitorStep = %d,EMCDAC=%d,g_nL1Count=%d \n",nMonitorStep,g_nEmcDAC,g_nL1Count));
			}
			break ;
		case BASELINE:
		case WAVEMODE:
			break;
		default:
			Push2ErrStack(PPCERROR+EMC_FLG+ATOM_ERR+8,0,0,0,0);			/*emc error 8*/
			return -1;
	}	
	
	DEBUG_OUTPUT(("EMCLoadedPRCF OK\n")) ;
	return 0;
}
int  	EMCPreconfigedCONF 	(int nArg) 
{
	int nModuIdx;
	int nChnlIdx;

	for(nModuIdx = 0; nModuIdx < MAX_SLOT_AMOUNT_PLUS_ONE; nModuIdx ++)
	{
		for(nChnlIdx = 0; nChnlIdx < MAX_CHNL_AMOUNT; nChnlIdx ++)
		{
			g_nConvertTable[nModuIdx][nChnlIdx] = 0 ;
			
			g_low_fAjust_a2[nModuIdx][nChnlIdx] = 0.0 ;		//add for adjust 071004
			g_low_fAjust_a1[nModuIdx][nChnlIdx] = 1.0 ;		//add for adjust 071004
			g_low_fAjust_a0[nModuIdx][nChnlIdx] = 0.0 ;		//add for adjust 071004	

			g_mid_fAjust_a2[nModuIdx][nChnlIdx] = 0.0 ;		//add for adjust 071004
			g_mid_fAjust_a1[nModuIdx][nChnlIdx] = 1.0 ;		//add for adjust 071004
			g_mid_fAjust_a0[nModuIdx][nChnlIdx] = 0.0 ;		//add for adjust 071004
			
			g_high_fAjust_a2[nModuIdx][nChnlIdx] = 0.0 ;		//add for adjust 071004
			g_high_fAjust_a1[nModuIdx][nChnlIdx] = 1.0 ;		//add for adjust 071004
			g_high_fAjust_a0[nModuIdx][nChnlIdx] = 0.0 ;		//add for adjust 071004
		
		}
	}
	//Chk nTrg/Int,create crystal-round table
	switch(g_nRunMode)
	{
		case NOMALMODE:
		case MONITOR:
			if((g_nTrgPerInt<1)||(g_nTrgPerInt>16))
			{
				DEBUG_OUTPUT(("normal mode,TrgPerInt must equal 1-16.\n"));
				Push2ErrStack(PPCERROR+EMC_FLG+ATOM_ERR+41,0,0,0,0);			/*emc error 41*/
				return -1;
			}
			for(nModuIdx = 0; nModuIdx <g_EMCDBInfo.emc_Q_sum; nModuIdx ++) 
			{
				int nPosition = g_EMCDBInfo.emc_Q_header[nModuIdx]->geo;
				for(nChnlIdx = 0; nChnlIdx < MAX_CHNL_AMOUNT; nChnlIdx ++)
				{
					unsigned int uWestEast= (g_EMCDBInfo.emc_Q_header[nModuIdx]->map[nChnlIdx])/100000;
					unsigned int nCircle 	= ((g_EMCDBInfo.emc_Q_header[nModuIdx]->map[nChnlIdx])%100000)/1000;
					unsigned int nCrystal  	= (g_EMCDBInfo.emc_Q_header[nModuIdx]->map[nChnlIdx])%1000;
					g_nConvertTable[nPosition][nChnlIdx] =(uWestEast<<31)+(nCircle<<26)+(nCrystal<<19);

					if((g_pCrateConfigHeader->unused)&0x1)	//need calib
					{
						g_low_fAjust_a2[nPosition][nChnlIdx]=g_EMCDBInfo.emc_Q_header[nModuIdx]->low_adjust_a2[nChnlIdx]/2.0;//add for adjust 071104
						g_low_fAjust_a1[nPosition][nChnlIdx]=g_EMCDBInfo.emc_Q_header[nModuIdx]->low_adjust_a1[nChnlIdx]/2.0;//add for adjust 071104
						g_low_fAjust_a0[nPosition][nChnlIdx]=g_EMCDBInfo.emc_Q_header[nModuIdx]->low_adjust_a0[nChnlIdx]/2.0;//add for adjust 071104
					
						g_mid_fAjust_a2[nPosition][nChnlIdx]=g_EMCDBInfo.emc_Q_header[nModuIdx]->mid_adjust_a2[nChnlIdx]/16.0;//add for adjust 071104
						g_mid_fAjust_a1[nPosition][nChnlIdx]=g_EMCDBInfo.emc_Q_header[nModuIdx]->mid_adjust_a1[nChnlIdx]/16.0;//add for adjust 071104
						g_mid_fAjust_a0[nPosition][nChnlIdx]=g_EMCDBInfo.emc_Q_header[nModuIdx]->mid_adjust_a0[nChnlIdx]/16.0;//add for adjust 071104
					
						g_high_fAjust_a2[nPosition][nChnlIdx]=g_EMCDBInfo.emc_Q_header[nModuIdx]->high_adjust_a2[nChnlIdx]/64.0;//add for adjust 071104
						g_high_fAjust_a1[nPosition][nChnlIdx]=g_EMCDBInfo.emc_Q_header[nModuIdx]->high_adjust_a1[nChnlIdx]/64.0;//add for adjust 071104
						g_high_fAjust_a0[nPosition][nChnlIdx]=g_EMCDBInfo.emc_Q_header[nModuIdx]->high_adjust_a0[nChnlIdx]/64.0;//add for adjust 071104
					}
					
					if((nChnlIdx%4)==0)
					{
//						DEBUG_OUTPUT(("g_high_fAjust_a2[%d][%d]=%f\n",nPosition,nChnlIdx,g_high_fAjust_a2[nPosition][nChnlIdx]));
//						DEBUG_OUTPUT(("g_high_fAjust_a1[%d][%d]=%f\n",nPosition,nChnlIdx,g_high_fAjust_a1[nPosition][nChnlIdx]));
//						DEBUG_OUTPUT(("g_high_fAjust_a0[%d][%d]=%f\n",nPosition,nChnlIdx,g_high_fAjust_a0[nPosition][nChnlIdx]));



  
//						DEBUG_OUTPUT(("g_mid_fAjust_a2[%d][%d]=%f\n",nPosition,nChnlIdx,g_mid_fAjust_a2[nPosition][nChnlIdx]));
 //						DEBUG_OUTPUT(("g_mid_fAjust_a1[%d][%d]=%f\n",nPosition,nChnlIdx,g_mid_fAjust_a1[nPosition][nChnlIdx]));
 //						DEBUG_OUTPUT(("g_mid_fAjust_a0[%d][%d]=%f\n",nPosition,nChnlIdx,g_mid_fAjust_a0[nPosition][nChnlIdx]));

//						DEBUG_OUTPUT(("g_low_fAjust_a2[%d][%d]=%f\n",nPosition,nChnlIdx,g_low_fAjust_a2[nPosition][nChnlIdx]));
//						DEBUG_OUTPUT(("g_low_fAjust_a1[%d][%d]=%f\n",nPosition,nChnlIdx,g_low_fAjust_a1[nPosition][nChnlIdx]));
//						DEBUG_OUTPUT(("g_low_fAjust_a0[%d][%d]=%f\n",nPosition,nChnlIdx,g_low_fAjust_a0[nPosition][nChnlIdx]));						
					}
				}
			}
			break;
		case PEDMODE:
		case CALIBRATION:
			if(g_nTrgPerInt!=1)
			{
				DEBUG_OUTPUT(("Calib&Ped mode,TrgPerInt must equal 1.\n"));
				Push2ErrStack(PPCERROR+EMC_FLG+ATOM_ERR+42,0,0,0,0);			/*emc error 42*/
				return -1;
			}
			for(nModuIdx = 0; nModuIdx <g_EMCDBInfo.emc_Q_sum; nModuIdx ++) 
			{
				int nPosition = g_EMCDBInfo.emc_Q_header[nModuIdx]->geo;
				for(nChnlIdx = 0; nChnlIdx < MAX_CHNL_AMOUNT; nChnlIdx ++)
				{
					g_nConvertTable[nPosition][nChnlIdx] =(nPosition<<26)+(nChnlIdx<<19);
					//printf("g_nConvertTable[%d][%d]=0x%x\n",nPosition,nChnlIdx,g_nConvertTable[nPosition][nChnlIdx]);				
				}
			}
			break;
		case BASELINE:
		case WAVEMODE:
			break;
	}

	if(-1==EMCConfigHW())
	{	
		//EMCConfigHW Fail
		Push2ErrStack(PPCERROR+EMC_FLG+ATOM_ERR+43,0,0,0,0);			/*emc error 43*/
		return -1;
	}			

	DEBUG_OUTPUT(("EMCPreconfig-Config OK\n")) ;
	return 0;
}
int  	EMCConfigedSYN1 		(int nArg) 
{
	//set fanout monitor mode, for shielding trigger FCTL

	//here config ROC,Monitor module
	if(-1==EMCClearEvents())
	{
		//EMCClearEvents Fail
		Push2ErrStack(PPCERROR+EMC_FLG+ATOM_ERR+44,0,0,0,0);			/*emc error 44*/
		return -1;
	}
	DEBUG_OUTPUT(("EMCConfigedSYN1 OK\n")) ;
	return 0;
}
int  	EMCSynchronized1SYN2 	(int nArg) 
{
	DEBUG_OUTPUT(("EMCSynchronized1SYN2 OK\n")) ;
	return 0;
}
int  	EMCSynchronized2SYN3 	(int nArg) 
{
	switch(g_nRunMode)
	{
		case NOMALMODE:
			if(g_EMCDBInfo.emc_ppc_header->NormModeSelfL1)
			{
				EMCSetFanoutMoniMode();
			}
			else
			{
				//emc fanout
				EMCSetFanoutHitMode();
		
				//trigger
				EMCTcbaDCMReset();//TCBA DCM reset
			}
			break;
		case PEDMODE:
			EMCSetFanoutMoniMode();
			//EMCSetFanoutHitMode();
			break ;
		case CALIBRATION:
		case MONITOR:
			break ;
		case BASELINE:
		case WAVEMODE:
			EMCSetFanoutMoniMode();
			break;
		default:
			Push2ErrStack(PPCERROR+EMC_FLG+ATOM_ERR+8,0,0,0,0);			/*emc error 8*/
			return -1;
	}
	
	DEBUG_OUTPUT(("EMCSynchronized2SYN3 OK\n")) ;
	return 0;
}
int  	EMCSynchronized3SYN4 	(int nArg) 
{
	DEBUG_OUTPUT(("EMCSynchronized3SYN4 OK\n")) ;
	return 0;
}
int  	EMCSynchronized4PREP 	(int nArg) 
{	
	switch(g_nRunMode)
	{
		case NOMALMODE:
		case PEDMODE:
		case CALIBRATION:
		case WAVEMODE:
			/*four mode above use same pack task, */
			if(g_EMCDBInfo.emc_Q_sum)
			{
				//byjak
				if(1==g_nTCheckAmpMode)
				{
					if(-1 == (nCbltTaskId= taskSpawn("tEMCTCBADMARead", PRI_CBLTTRANS, 0, TASK_STACK_SIZE, (FUNCPTR)EMCTCBADMARead, 
										0, 0, 0, 0, 0, 0, 0, 0, 0, 0) )  )
					{
						Push2ErrStack(PPCFATAL+EMC_FLG+ATOM_ERR+70,0,0,0,0);		//emc error 70
						return -1;
					}
				}
				else
				{
					if(-1 == (nCbltTaskId= taskSpawn("tFWCbltTrans", PRI_CBLTTRANS, 0, TASK_STACK_SIZE, (FUNCPTR)FWCbltTrans, 
											0, 0, 0, 0, 0, 0, 0, 0, 0, 0) )  )
					{
						Push2ErrStack(PPCFATAL+EMC_FLG+ATOM_ERR+9,0,0,0,0);		/*emc error 9*/
						return -1;
					}				
				}
			

				if(-1 == (nPackTaskId= taskSpawn("tEMCDataPack", PRI_DATAPACK, 0, TASK_STACK_SIZE, (FUNCPTR)EMCDataPack, 
										0, 0, 0, 0, 0, 0, 0, 0, 0, 0) ) )
				{
					Push2ErrStack(PPCFATAL+EMC_FLG+ATOM_ERR+10,0,0,0,0);		/*emc error 10*/
					return -1;
				}
				if(-1 == (nNetTaskId= taskSpawn("tFWNetTrans", PRI_NETTRANS, 0, TASK_STACK_SIZE, (FUNCPTR)FWNetTrans, 
										0, 0, 0, 0, 0, 0, 0, 0, 0, 0) ) )
				{
					Push2ErrStack(PPCFATAL+EMC_FLG+ATOM_ERR+11,0,0,0,0);		/*emc error 11*/
					return -1;
				}
			} 
			break;
		case MONITOR:
			if(g_EMCDBInfo.emc_Q_sum)
			{
				printf("*******nMonitorStep = %d*********\n",nMonitorStep);
				if( (nMonitorStep<=7)&&(0==g_EMCDBInfo.emc_monitor_sum)  )
				{
						if(-1 == (nPackTaskId= taskSpawn("tEMCDataPack", PRI_DATAPACK, 0, TASK_STACK_SIZE, (FUNCPTR)EMCMoniOthDataPackP1, 
												0, 0, 0, 0, 0, 0, 0, 0, 0, 0) ) )
						{
							Push2ErrStack(PPCFATAL+EMC_FLG+ATOM_ERR+10,0,0,0,0);		/*emc error 10*/
							return -1;
						}
						if(-1 == (nNetTaskId= taskSpawn("tFWNetTrans", PRI_NETTRANS, 0, TASK_STACK_SIZE, (FUNCPTR)EMCMoniOtheNetTransP1, 
												0, 0, 0, 0, 0, 0, 0, 0, 0, 0) ) )
						{
							Push2ErrStack(PPCFATAL+EMC_FLG+ATOM_ERR+11,0,0,0,0);		/*emc error 11*/
							return -1;
						}							

				}
				else if( (nMonitorStep<=7)&&(1==g_EMCDBInfo.emc_monitor_sum)  )
				{
						if(-1 == (nCbltTaskId= taskSpawn("tFWCbltTrans", PRI_CBLTTRANS, 0, TASK_STACK_SIZE, (FUNCPTR)FWCbltTrans, 
												0, 0, 0, 0, 0, 0, 0, 0, 0, 0) )  )
						{
							Push2ErrStack(PPCFATAL+EMC_FLG+ATOM_ERR+9,0,0,0,0);		/*emc error 9*/
							return -1;
						}				

						if(-1 == (nPackTaskId= taskSpawn("tEMCDataPack", PRI_DATAPACK, 0, TASK_STACK_SIZE, (FUNCPTR)EMCMoniCrateDataPackP1, 
												0, 0, 0, 0, 0, 0, 0, 0, 0, 0) ) )
						{
							Push2ErrStack(PPCFATAL+EMC_FLG+ATOM_ERR+10,0,0,0,0);		/*emc error 10*/
							return -1;
						}
						if(-1 == (nNetTaskId= taskSpawn("tFWNetTrans", PRI_NETTRANS, 0, TASK_STACK_SIZE, (FUNCPTR)EMCMoniCrateNetTransP1, 
												0, 0, 0, 0, 0, 0, 0, 0, 0, 0) ) )
						{
							Push2ErrStack(PPCFATAL+EMC_FLG+ATOM_ERR+11,0,0,0,0);		/*emc error 11*/
							return -1;
						}	
				
				}
				else  if( nMonitorStep>=8 )
				{
						if(-1 == (nCbltTaskId= taskSpawn("tFWCbltTrans", PRI_CBLTTRANS, 0, TASK_STACK_SIZE, (FUNCPTR)FWCbltTrans, 
												0, 0, 0, 0, 0, 0, 0, 0, 0, 0) )  )
						{
							Push2ErrStack(PPCFATAL+EMC_FLG+ATOM_ERR+9,0,0,0,0);		/*emc error 9*/
							return -1;
						}				

						if(-1 == (nPackTaskId= taskSpawn("tEMCDataPack", PRI_DATAPACK, 0, TASK_STACK_SIZE, (FUNCPTR)EMCDataPack, 
												0, 0, 0, 0, 0, 0, 0, 0, 0, 0) ) )
						{
							Push2ErrStack(PPCFATAL+EMC_FLG+ATOM_ERR+10,0,0,0,0);		/*emc error 10*/
							return -1;
						}
						if(-1 == (nNetTaskId= taskSpawn("tFWNetTrans", PRI_NETTRANS, 0, TASK_STACK_SIZE, (FUNCPTR)FWNetTrans, 
												0, 0, 0, 0, 0, 0, 0, 0, 0, 0) ) )
						{
							Push2ErrStack(PPCFATAL+EMC_FLG+ATOM_ERR+11,0,0,0,0);		/*emc error 11*/
							return -1;
						}							

				}			
			}	
			break;
		case BASELINE:
			if(-1 == (taskSpawn("tEMCBaseLine", PRI_DATAPACK, 0, TASK_STACK_SIZE, (FUNCPTR)EMCBaselinePack, 
										0, 0, 0, 0, 0, 0, 0, 0, 0, 0) ) )
			{
				Push2ErrStack(PPCFATAL+EMC_FLG+ATOM_ERR+11,0,0,0,0);		/*emc error 11*/
				return -1;
			}
			break;
		default:
			DEBUG_OUTPUT(("unknown running mode"));
			Push2ErrStack(PPCERROR+EMC_FLG+ATOM_ERR+8,0,0,0,0);		/*emc error 8*/
			SendAck(FAIL) ;
			break ;
	}
	
	//byjak
	if(1==g_nTCheckAmpMode)
	{
		//do nothing
	}
	else
	{
		if(ERROR ==(nEMCGenL1TaskId= taskSpawn("tEMCGenL1", 180, 0, TASK_STACK_SIZE, (FUNCPTR)EMCGenL1, 
												0, 0, 0, 0, 0, 0, 0, 0, 0, 0) ) )
		{
			printf("task EMCGiveL1 spawn error\n");
			Push2ErrStack(PPCERROR+EMC_FLG+ATOM_ERR+45,0,0,0,0);			/*emc error 45*/
			return -1;
		}	
	}

	
	printf("EMCSynchronized4PREP OK\n") ;
	return 0;
}
int  	EMCReadySATR  			(int nArg)
{
	switch(g_nRunMode)
	{
		case NOMALMODE:
		case PEDMODE:
			g_bTrgStop=false;
			semGive(g_sidSATR) ;
			break;
		case CALIBRATION:
		case MONITOR:
			g_bEMCMonitor = true;
			g_bTrgStop=false;
			semGive(g_sidSATR) ;
			break;
		case BASELINE:
		case WAVEMODE:
			g_bTrgStop=false;
			semGive(g_sidSATR) ;
			break;
		default:
			DEBUG_OUTPUT(("unknown running mode"));
			Push2ErrStack(PPCERROR+EMC_FLG+ATOM_ERR+8,0,0,0,0);		/*emc error 8*/
			SendAck(FAIL) ;
			break ;
	}
	
	DEBUG_OUTPUT(("EMCReadySATR OK\n")) ;
	return 0;
}
int  	EMCRunningSPTR  		(int nArg)
{
	int nClibInfoLen;
	switch(g_nRunMode)
	{
		case NOMALMODE:
		case PEDMODE:
			StopGenL1();
			break;
		case CALIBRATION:
		case MONITOR:
			if(g_nCalibMoni_Points!=1)
			{
				g_nEmcDAC=*(unsigned int *)g_cCalibMoni_Info;
				g_cCalibMoni_Info=g_cCalibMoni_Info+4;
				g_nL1Count=*(unsigned int*)g_cCalibMoni_Info;
				g_cCalibMoni_Info=g_cCalibMoni_Info+4;
				printf("new CalibDAC=%d,g_nL1Count=%d \n",g_nEmcDAC,g_nL1Count);
				g_nCalibMoni_Points=g_nCalibMoni_Points-1;
			}
			else
			{
				g_cCalibMoni_Info = g_cCalibMoni_Info_ori;
				nClibInfoLen=*(unsigned int*)(g_cCalibMoni_Info)-4; 	//the exact calib infor length			//other mode use default to config
				g_cCalibMoni_Info=g_cCalibMoni_Info+4;                 			//jump the length infor
				g_nCalibMoni_Points=nClibInfoLen/8;		

				g_nEmcDAC=*(unsigned int *)g_cCalibMoni_Info;
				g_cCalibMoni_Info=g_cCalibMoni_Info+4;
				g_nL1Count=*(unsigned int *)g_cCalibMoni_Info;
				g_cCalibMoni_Info=g_cCalibMoni_Info+4;
			}
			break;
		case BASELINE:
		case WAVEMODE:
			break;
		default:
			DEBUG_OUTPUT(("unknown running mode"));
			Push2ErrStack(PPCERROR+EMC_FLG+ATOM_ERR+8,0,0,0,0);		/*emc error 8*/
			SendAck(FAIL) ;
			break ;
	}
	DEBUG_OUTPUT(("EMCRunningSPTR OK\n")) ;
	return 0;
}
int  	EMCReadySTOP  			(int nArg)
{
	switch(g_nRunMode)
	{
		case NOMALMODE:
		case PEDMODE:
			EMCSetFanoutMoniMode(); //leigk 080128 for block trg: should not be done in SPTR :jixl080208
			break;
		case CALIBRATION:
		case MONITOR:
			g_bTrgStop=true;
	
			semGive(g_sidSATR) ;
			break;
		case BASELINE:
		case WAVEMODE:
			break;
		default:
			DEBUG_OUTPUT(("unknown running mode"));
			Push2ErrStack(PPCERROR+EMC_FLG+ATOM_ERR+8,0,0,0,0);		/*emc error 8*/
			SendAck(FAIL) ;
			break ;
	}
	DEBUG_OUTPUT(("EMCReadySTOP OK\n")) ;
	return 0;
}
int  	EMCConfigedUNCF 		(int nArg)
{
	DEBUG_OUTPUT(("EMCConfigedUNCF OK\n")) ;
	return 0;
}
int  	EMCPreconfigedUNPC 	(int nArg) 
{
	//hi,jak
	if(1==g_nTCheckAmpMode)
	{
		if(g_sidTCBAInterrupt!=NULL)
		{
			if(-1 == semDelete(g_sidTCBAInterrupt) )
			{
				DEBUG_OUTPUT(("semDelete(g_sidTCBAInterrupt)  failed.\n")) ;
				Push2ErrStack(PPCFATAL+EMC_FLG+ATOM_ERR+71,0,0,0,0);		//emc error 71
				return -1;
			}
			g_sidTCBAInterrupt=NULL;
		}
	}	
	//bye,jak

	DEBUG_OUTPUT(("EMCPreconfigedUNPC OK\n")) ;
	return 0;
}
int  	EMCLoadedUNLD 			(int nArg)
{
	DEBUG_OUTPUT(("EMCLoadedUNLD OK\n")) ;
	return 0;
}
int  	EMCAnyEXIT 			(int nArg)
{
	if(nEMCGenL1TaskId)
	{
		if(ERROR==taskDelete(nEMCGenL1TaskId) )
		{
			printf("taskDelete(nEMCGenL1TaskId fail\n");
			Push2ErrStack(PPCERROR+EMC_FLG+ATOM_ERR+46,0,0,0,0);		/*emc error 46*/
			return -1;
		}
		nEMCGenL1TaskId=0;
	}
	

	//hi,jak
	if(1==g_nTCheckAmpMode)
	{
		if(g_sidTCBAInterrupt!=NULL)
		{
			if(-1 == semDelete(g_sidTCBAInterrupt) )
			{
				DEBUG_OUTPUT(("semDelete(g_sidTCBAInterrupt)  failed.\n")) ;
				Push2ErrStack(PPCFATAL+EMC_FLG+ATOM_ERR+71,0,0,0,0);		//emc error 71
				return -1;
			}
			g_sidTCBAInterrupt=NULL;
		}
	}	
	//bye,jak


	DEBUG_OUTPUT(("EMCAnyEXIT OK\n")) ;
	return 0;
}
