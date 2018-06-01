
#include "vxWorks.h"
#include "taskLib.h"
#include "stdio.h"
#include "sysLib.h"
#include "usrLib.h"
#include "intLib.h"
#include "arch/ppc/ivPpc.h"

#include "5100_test.h"

#include "fw_define.h"
#include "fw_roppc.h"
#include "fw_actions.h"
#include "fw_dcgvas.h"
#include "fw_staterrspt.h"
#include "fw_runtasks.h"
#include "fw_filefunc.h"
#include "fw_tcpfunc.h"

#include "sd_mdc.h"
#include "sd_mdc_actions.h"
#include "sd_mdc_hw.h"
#include "sd_mdc_runtasks.h"

#ifdef DEBUG_MODE
//#undef DEBUG_MODE
#endif

int	MDCInitializedLOAD 	(int nArg) 
{ 
	char * 	pcMdlHeader ;
	UINT 	nMdlType ;

	int 		nClibInfoLen;
	
	sysIntDisable(MDC_ROC_INT_LEVEL) ;

	if((g_nCrateID<0x1f)||(g_nCrateID>=0x40))
	{
		DEBUG_OUTPUT(("MDC CrateID out of 0x1F-0x40\n")) ;
		Push2ErrStack(PPCERROR+MDC_FLG+ATOM_ERR+101,0,0,0,0);		/*mdc error 101*/
		return -1;
	}
	
	//------ calibaration configuration info seg
	g_cCalibMoni_Info=((char*)g_pCrateConfigHeader) + g_pCrateConfigHeader->crate_len ;//calib mode ,all use these infor to config
																					//other mode use default to config
	g_cCalibMoni_Info_ori = g_cCalibMoni_Info;																				

	nClibInfoLen=*(unsigned int*)(g_cCalibMoni_Info)-4; 	//the exact calib infor length			
	g_cCalibMoni_Info=g_cCalibMoni_Info+4;                 			//jump the length infor
	g_nCalibMoni_Points=nClibInfoLen/12;						//get the num of calib points
	//------calib
	DEBUG_OUTPUT( ("ClibInfoLen=%d\n",nClibInfoLen) );
	DEBUG_OUTPUT(("g_nMDCCalibPoints=%d\n",g_nCalibMoni_Points) );
	
	//reset count
	g_MDCDBInfo.ppc_sum =	0 ;
	g_MDCDBInfo.mcc_sum = 	0 ;
	g_MDCDBInfo.roc_sum = 	0 ;
	g_MDCDBInfo.mqt_sum = 	0 ;
	g_MDCDBInfo.mti_sum = 	0 ;
	//locate configuration info 
	pcMdlHeader = g_cCalibMoni_Info+nClibInfoLen ;
	
	for(int nMdlIdx = 0; nMdlIdx < g_nMdlSum; nMdlIdx ++)
	{
		nMdlType =  (UINT)(*(UINT*)(pcMdlHeader + 4)) ;
		DEBUG_OUTPUT(("Module Type: 0x%x\n", nMdlType) ) ;
		
		switch(nMdlType)
		{
			case MDC_PPC:
				g_MDCDBInfo.ppc_header = (struct GE_ppc *)pcMdlHeader ;
				
				Show_ppc(g_MDCDBInfo.ppc_header);
				
				g_MDCDBInfo.ppc_sum++;
				if (g_MDCDBInfo.ppc_sum>1)
				{		
					DEBUG_OUTPUT(("error, PPC sum > 1.\n")) ;
					Push2ErrStack(PPCERROR+MDC_FLG+ATOM_ERR+1,0,0,0,0);		/*mdc error 1*/
					return -1 ;
				}			
				break;

			case MDC_MQT:
				g_MDCDBInfo.mqt_header[g_MDCDBInfo.mqt_sum] = (struct mdc_mqt*)pcMdlHeader ;

				Show_mdc_mqt(g_MDCDBInfo.mqt_header[g_MDCDBInfo.mqt_sum]);
				
				if((g_MDCDBInfo.mqt_header[g_MDCDBInfo.mqt_sum]->geo)>256)
				{
					g_MDCDBInfo.mqt_header[g_MDCDBInfo.mqt_sum]->geo &=0xff;
					g_MDCDBInfo.mqt_enable[g_MDCDBInfo.mqt_sum]=1;
				}
				else
				{
					g_MDCDBInfo.mqt_enable[g_MDCDBInfo.mqt_sum]=0;
				}
				g_MDCDBInfo.mqt_sum ++;
				break ;
				
			case MDC_ROC:
				g_MDCDBInfo.roc_header = (struct mdc_roc*)pcMdlHeader ;

				if((g_MDCDBInfo.roc_header->geo)>256)
				{
					g_MDCDBInfo.roc_header->geo&=0xff;
					Show_mdc_roc(g_MDCDBInfo.roc_header);
					g_MDCDBInfo.roc_sum++;
				}

				if (g_MDCDBInfo.roc_sum> 1)
				{
					DEBUG_OUTPUT(("ERROR: g_MDCDBInfo.roc_sum > 1.\n")) ;
					Push2ErrStack(PPCERROR+MDC_FLG+ATOM_ERR+2,0,0,0,0);		/*mdc error 2*/
					return -1 ;
				}
				break ;
				
			case MDC_MCC:
				g_MDCDBInfo.mcc_header = (struct mdc_mcc*)pcMdlHeader ;

				if((g_MDCDBInfo.mcc_header->geo)>256)
				{
					g_MDCDBInfo.mcc_header->geo&=0xff;
					Show_mdc_mcc(g_MDCDBInfo.mcc_header);
					g_MDCDBInfo.mcc_sum++;
				}
				
				if ( g_MDCDBInfo.mcc_sum > 1)
				{
					DEBUG_OUTPUT(("ERROR: g_MDCDBInfo.mcc_sum > 1. \n")) ;
					Push2ErrStack(PPCERROR+MDC_FLG+ATOM_ERR+3,0,0,0,0);		/*mdc error 3*/
					return -1 ;
				}
				break ;
				
			case MDC_MTI:
				g_MDCDBInfo.mti_header = (struct mdc_mti*)pcMdlHeader ;

				if( (g_MDCDBInfo.mti_header->geo)>256)
				{
					g_MDCDBInfo.mti_header->geo&=0xff;
					Show_mdc_mti(g_MDCDBInfo.mti_header);
					g_MDCDBInfo.mti_sum++;
				}
				if ( g_MDCDBInfo.mti_sum> 1)
				{
					DEBUG_OUTPUT(("ERROR: g_MDCDBInfo.mti_sum > 1. \n")) ;
					Push2ErrStack(PPCERROR+MDC_FLG+ATOM_ERR+4,0,0,0,0);		/*mdc error 4*/
					return -1 ;
				}
				break ;
			default:
				DEBUG_OUTPUT(("Unkown modules type.\n")) ;
				Push2ErrStack(PPCERROR+MDC_FLG+ATOM_ERR+5,0,0,0,0);			/*mdc error 5*/
				return -1 ;
		}
		
		pcMdlHeader = pcMdlHeader + (UINT)(*(UINT*)pcMdlHeader) ;
		
	}
	
	//compare the PPC id in db and jumper
	if ( g_nPPCIDOfSRH!= g_MDCDBInfo.ppc_header->PPC_id)
	{
		DEBUG_OUTPUT(("The PPC id in db (%d) is not equal to the one in SRH (%d).\n", g_MDCDBInfo.ppc_header->PPC_id, g_nPPCIDOfSRH)) ;
		Push2ErrStack(PPCERROR+MDC_FLG+ATOM_ERR+6,0,0,0,0);			/*mdc error 6*/
		return -1 ;
	}

	if(g_MDCDBInfo.mqt_sum)
	{
		//queue mqt db infor with geo 
		for(int nIndex=0; nIndex < g_MDCDBInfo.mqt_sum - 1; nIndex ++)//sort  the mqt with GEO & enable set
		{
			struct mdc_mqt * mqt_header_temp;
			int 			       enable_Temp;
			for(int nIndex2 =nIndex+1; nIndex2 < g_MDCDBInfo.mqt_sum; nIndex2 ++)
			{
				if ( (g_MDCDBInfo.mqt_header[nIndex]->geo) > (g_MDCDBInfo.mqt_header[nIndex2]->geo))
				{						
					mqt_header_temp = g_MDCDBInfo.mqt_header[nIndex] ;	//swap [nMdlIdx] & [nMdlIdx +1]
					g_MDCDBInfo.mqt_header[nIndex] = g_MDCDBInfo.mqt_header[nIndex2] ;
					g_MDCDBInfo.mqt_header[nIndex2]  = mqt_header_temp ;
					enable_Temp=g_MDCDBInfo.mqt_enable[nIndex] ;
					g_MDCDBInfo.mqt_enable[nIndex]=g_MDCDBInfo.mqt_enable[nIndex];
					g_MDCDBInfo.mqt_enable[nIndex]=enable_Temp;
				}
			}
		}
		
		//count the enable num and record geo
		g_nMqtEnableSum=0;
		for(nIndex = 0; nIndex < g_MDCDBInfo.mqt_sum; nIndex ++)
		{
			if(g_MDCDBInfo.mqt_enable[nIndex])
			{
				g_nMqtEnableGeo[g_nMqtEnableSum]=g_MDCDBInfo.mqt_header[nIndex]->geo;
				g_nMqtEnableSum++;
			}
		}
	
		if(MDCConfigIntCBLT()==-1)
		{
			DEBUG_OUTPUT(("MDC config interruput |CBLT error\n")) ;
			Push2ErrStack(PPCERROR+MDC_FLG+ATOM_ERR+102,0,0,0,0);			/*mdc error 102*/
			return -1;
		}
	}

	//config MCC here, configuration of MQT & ROC is put to Conf
	if(g_MDCDBInfo.mcc_sum)
	{
		if(-1==MDCConfigMCCLoad(g_MDCDBInfo.mcc_header->geo,g_MDCDBInfo.mcc_header->T0_latency))
		{
			DEBUG_OUTPUT(("MDC config MCC in LOAD error\n")) ;
			Push2ErrStack(PPCERROR+MDC_FLG+ATOM_ERR+103,0,0,0,0);			/*mdc error 103*/
			return -1;
		}
		MDCConfigMTILoad(g_MDCDBInfo.mti_header->geo);
	}
	DEBUG_OUTPUT(("MDCInitializedLOAD OK\n")) ;
	return 0;
}
int  	MDCLoadedPRCF 		(int nArg) 
{ 
	if(g_MDCDBInfo.mqt_sum)
	{	
		//get the ROS ip and port,and connect data socket
		unsigned int	 nRemoteIP = g_MDCDBInfo.ppc_header->ROS_ip;
		unsigned short nDataPort= g_MDCDBInfo.ppc_header->ROS_port ;
		DEBUG_OUTPUT(("ROS IP = %x, PORT = %d.\n",nRemoteIP, nDataPort)) ;
		if(DataSockConnect(nRemoteIP,nDataPort)==-1)
		{
			printf("connect data socket error\n");
			Push2ErrStack(PPCERROR+MDC_FLG+ATOM_ERR+104,0,0,0,0);			/*mdc error 104*/
			CloseDataSocket();
			return -1;
		}
		DEBUG_OUTPUT( ("successfully connected on data socket.\n") ) ;
	}
	DEBUG_OUTPUT(("MDCLoadedPRCF OK\n")) ;
	return 0;
}	
int  	MDCPreconfigedCONF 	(int nArg) 
{
	int nSlotIdx,nIdx;
	int nMQTIdx,nChnlIdx;
	
	switch(g_nRunMode)
	{
		case NOMALMODE:
		case PEDMODE:
		case BASELINE:
			g_nCalibL1OffSet=0;
			g_nMQTDAC=0xffff;//these two parameter only have meaning at calibmode
			g_nMCCT0Latency=g_MDCDBInfo.mcc_header->T0_latency;	//use value in db,for set T0 below
			break ;
		case WAVEMODE:
			break;
		case CALIBRATION:
			{
				g_nCalibL1OffSet=0;
				g_nMQTDAC=*(unsigned int *)g_cCalibMoni_Info;
//				printf("g_nMQTDAC in CALIBMODE is %d\n",g_nMQTDAC);
				g_cCalibMoni_Info=g_cCalibMoni_Info+4;
				g_nMCCT0Latency=*(unsigned int *)g_cCalibMoni_Info;
//				printf("g_nMCCT0Latency in CALIBMODE is %d\n",g_nMCCT0Latency);
				g_cCalibMoni_Info=g_cCalibMoni_Info+4;
				g_nL1Count=*(unsigned int *)g_cCalibMoni_Info;
				g_cCalibMoni_Info=g_cCalibMoni_Info+4;
				DEBUG_OUTPUT(("MQTDAC=%d,T0Latency=%d,g_nL1Count=%d \n",g_nMQTDAC,g_nMCCT0Latency,g_nL1Count));
			}
			break ;
		case MONITOR:
			break;
		default:
			Push2ErrStack(PPCERROR+MDC_FLG+ATOM_ERR+9,0,0,0,0);			/*mdc error 9*/
			return -1;
	}
	
	if(g_MDCDBInfo.mqt_sum)
	{
		
		//*(UINT32*)0xfd000404 = 0x00000602 ;  disable vme bus error tiemout
		*(UINT32*)0xfd000114 = 0x0000c280 ;   //write the window register, post write disable

		nIdx=0;
		g_MDCHWInfo.mqt_sum = 0 ;
		for (nSlotIdx = 2; nSlotIdx < MAX_SLOT_AMOUNT_PLUS_ONE; nSlotIdx ++)//scan for MQT
		{
			if (0xffffffff !=  VALUE_OF_A24D32(MDC_ADRS_WITH_GEO(nSlotIdx, MDC_MQT_FUNC_ADRS(MDC_MQT_CRCSR_SPACE) | MDC_MQT_FUNC_SUB_ADRS(MDC_MQT_ID_REG))))
			{
				if(0x1 == (0x3 & VALUE_OF_A24D32(MDC_ADRS_WITH_GEO(nSlotIdx, MDC_MQT_FUNC_ADRS(MDC_MQT_CRCSR_SPACE) | MDC_MQT_FUNC_SUB_ADRS(MDC_MQT_ID_REG)))))
				{
					g_MDCHWInfo.mqt_sum = g_MDCHWInfo.mqt_sum +1 ;
					g_MDCHWInfo.mqt_geo[nIdx] = nSlotIdx ;
					DEBUG_OUTPUT(("MQT is scanned, at slot %d.\n", nSlotIdx)) ;
					nIdx ++ ;
				}
			}
		}
		if(g_MDCHWInfo.mqt_sum!=g_MDCDBInfo.mqt_sum)
		{
			DEBUG_OUTPUT(("MQT num Scaned %d!= MQT sum %d in DB\n",g_MDCHWInfo.mqt_sum,g_MDCDBInfo.mqt_sum) );
			Push2ErrStack(PPCERROR+MDC_FLG+ATOM_ERR+10,0,0,0,0);			/*mdc error 10*/
			return -1;
		}
		else
		{
			for(nIdx=0;nIdx< g_MDCHWInfo.mqt_sum; nIdx++)
			{
				if(g_MDCDBInfo.mqt_header[nIdx]->geo!=g_MDCHWInfo.mqt_geo[nIdx])
				{
					DEBUG_OUTPUT(("MQT GEO Scan not correspond GEO in DB\n"));
					Push2ErrStack(PPCERROR+MDC_FLG+ATOM_ERR+11,0,0,0,0);			/*mdc error 11*/
					return -1;
				}
			}
		}
		
		//reset convert table
		for(nMQTIdx = 0; nMQTIdx < MAX_SLOT_AMOUNT_PLUS_ONE; nMQTIdx ++)
		{
			for(nChnlIdx = 0; nChnlIdx < MAX_CHNL_AMOUNT; nChnlIdx ++)
			{
				g_nConvertTable[nMQTIdx][nChnlIdx] = 0 ;
				g_fAjust_a2[nMQTIdx][nChnlIdx] = 0.0 ;		
				g_fAjust_a1[nMQTIdx][nChnlIdx] = 1.0 ;		
				g_fAjust_a0[nMQTIdx][nChnlIdx] = 0.0 ;						
			}
		}

		//Init convert table,CHK nTrg/Int
		switch(g_nRunMode)
		{
			case NOMALMODE:	//make layer-cell table
				if((g_nTrgPerInt<1)||(g_nTrgPerInt>16))
				{
					DEBUG_OUTPUT(("normal mode,TrgPerInt must equal 1-16.\n"));
					Push2ErrStack(PPCERROR+MDC_FLG+ATOM_ERR+105,0,0,0,0);			/*mdc error 105*/
					return -1;
				}
				for(nMQTIdx = 0; nMQTIdx <g_MDCDBInfo.mqt_sum; nMQTIdx ++) 
				{
					int nMQTPosition = g_MDCDBInfo.mqt_header[nMQTIdx]->geo;
					for(nChnlIdx = 0; nChnlIdx < MAX_CHNL_AMOUNT; nChnlIdx ++)
					{
						int nLayerNum = g_MDCDBInfo.mqt_header[nMQTIdx]->layercell[nChnlIdx] / 1000 ;
						int nCellNum =  g_MDCDBInfo.mqt_header[nMQTIdx]->layercell[nChnlIdx] % 1000 ;
						if(nLayerNum < 32)
						{
							g_nConvertTable[nMQTPosition][nChnlIdx] = (nLayerNum << 26) | (nCellNum << 18);
						}
						else
						{
							g_nConvertTable[nMQTPosition][nChnlIdx] = ((nLayerNum - 16) << 27) | (nCellNum << 18);
						}
						if((g_pCrateConfigHeader->unused)&0x1)	//need calib
						{
							g_fAjust_a2[nMQTPosition][nChnlIdx]=g_MDCDBInfo.mqt_header[nMQTIdx]->adjust_a2[nChnlIdx];
							g_fAjust_a1[nMQTPosition][nChnlIdx]=g_MDCDBInfo.mqt_header[nMQTIdx]->adjust_a1[nChnlIdx];
							g_fAjust_a0[nMQTPosition][nChnlIdx]=g_MDCDBInfo.mqt_header[nMQTIdx]->adjust_a0[nChnlIdx];
						}
					}
				}
				break ;
			case PEDMODE:
			case BASELINE:
			case CALIBRATION:			//make module-channel table
				if(g_nTrgPerInt!=1)
				{
					DEBUG_OUTPUT(("Non Norm mode,TrgPerInt must equal 1.\n"));
					Push2ErrStack(PPCERROR+MDC_FLG+ATOM_ERR+106,0,0,0,0);			/*mdc error 106*/
					return -1;
				}
				for(nMQTIdx = 0; nMQTIdx <g_MDCDBInfo.mqt_sum; nMQTIdx ++) 
				{
					int nMQTPosition = g_MDCDBInfo.mqt_header[nMQTIdx]->geo;
					for(nChnlIdx = 0; nChnlIdx < MAX_CHNL_AMOUNT; nChnlIdx ++)
					{
						g_nConvertTable[nMQTPosition][nChnlIdx] = (nMQTPosition<<26)|(nChnlIdx<<18);
						//testtest//
						/*
						if((g_pCrateConfigHeader->unused)&0x1)	//need calib
						{
//							printf("**********need to calib********\n");
							g_fAjust_a2[nMQTPosition][nChnlIdx]=g_MDCDBInfo.mqt_header[nMQTIdx]->adjust_a2[nChnlIdx];
							g_fAjust_a1[nMQTPosition][nChnlIdx]=g_MDCDBInfo.mqt_header[nMQTIdx]->adjust_a1[nChnlIdx];
							g_fAjust_a0[nMQTPosition][nChnlIdx]=g_MDCDBInfo.mqt_header[nMQTIdx]->adjust_a0[nChnlIdx];
							
						}*/
						
					}
				}
				break ;
			case WAVEMODE:
				break;
			case MONITOR:
				break;
			default:
				Push2ErrStack(PPCERROR+MDC_FLG+ATOM_ERR+9,0,0,0,0);			/*mdc error 9*/
				return -1;
		}

		if(-1==MDCConfigROC())
		{
			DEBUG_OUTPUT(("MDCConfigROC failed\n")) ;
			Push2ErrStack(PPCERROR+MDC_FLG+ATOM_ERR+107,0,0,0,0);			/*mdc error 107*/
			return -1;
		}

		if(-1 == MDCMQTConfigAll())
		{
			DEBUG_OUTPUT(("MDCMQTConfigAll failed\n")) ;
			Push2ErrStack(PPCERROR+MDC_FLG+ATOM_ERR+108,0,0,0,0);			/*mdc error 108*/
			return -1 ;
		}
	}
	printf("MDCPreconfigedCONF OK\n") ;
	return 0;
}
int  	MDCConfigedSYN1 		(int nArg) 
{ 
	//set mcc calib mode for block L1 signal
	if(g_MDCDBInfo.mcc_sum)
	{
		if(-1==MDCConfigMCCSyn1(g_MDCDBInfo.mcc_header->geo,g_nMCCT0Latency))
		{
			DEBUG_OUTPUT(("MDCConfigedSYN1 error\n")) ;
			Push2ErrStack(PPCERROR+MDC_FLG+ATOM_ERR+109,0,0,0,0);			/*mdc error 109*/
			return -1;
		}
	}
	DEBUG_OUTPUT(("MDCConfigedSYN1 OK\n")) ;
	return 0;
}
int  	MDCSynchronized1SYN2 (int nArg)
{ 
	DEBUG_OUTPUT(("MDCSynchronized1SYN2 OK\n")) ;
	return 0;
}
int  	MDCSynchronized2SYN3 (int nArg)
{ 
	if(g_MDCDBInfo.mcc_sum)
	{
		if(-1==MDCConfigMCCSyn3(g_MDCDBInfo.mcc_header->geo,g_nMCCT0Latency))
		{
			DEBUG_OUTPUT(("MDCSynchronized2SYN3 error\n")) ;
			Push2ErrStack(PPCERROR+MDC_FLG+ATOM_ERR+110,0,0,0,0);			/*mdc error 110*/
			return -1;
		}
	}
	DEBUG_OUTPUT(("MDCSynchronized2SYN3 OK\n")) ;
	return 0;
}
int  	MDCSynchronized3SYN4 (int nArg) 
{ 
	DEBUG_OUTPUT(("MDCSynchronized3SYN4 OK\n")) ;
	return 0;
}
int  	MDCSynchronized4PREP (int nArg)
{ 
	if(g_MDCDBInfo.mcc_sum)
	{
		MDCMCCResetCheck((g_MDCDBInfo.mcc_header->geo & 0xff)); //for reset check signal
		for(int i = 0; i < 500; i ++) ;
	}
	if(g_MDCDBInfo.mqt_sum)
	{
		taskDelay(20);
		for(int nGEOIdx =g_nMqtEnableSum-1;  nGEOIdx >=0;  nGEOIdx --)	
		{
			MDCClrMQTL1(g_nMqtEnableGeo[nGEOIdx]);
		}
	}
	if(g_MDCDBInfo.mqt_sum)
	{
		switch(g_nRunMode)
		{
			case NOMALMODE:
			case PEDMODE:
			case BASELINE:
			case CALIBRATION:
			case MONITOR:
				
				if(-1 ==  (nCbltTaskId=taskSpawn("tFWCbltTrans", PRI_CBLTTRANS, 0, TASK_STACK_SIZE, (FUNCPTR)FWCbltTrans, 
										0, 0, 0, 0, 0, 0, 0, 0, 0, 0)))
				{
					DEBUG_OUTPUT(("taskSpawn CbltTrans failed\n") );
					Push2ErrStack(PPCERROR+MDC_FLG+ATOM_ERR+14,0,0,0,0);			/*mdc error 14*/
					return -1;
				}

				if(-1 ==  (nPackTaskId=taskSpawn("tFWNetTrans", PRI_DATAPACK, 0, TASK_STACK_SIZE, (FUNCPTR)FWNetTrans, 
										0, 0, 0, 0, 0, 0, 0, 0, 0, 0)))
				{
					DEBUG_OUTPUT(("taskSpawn NetTrans failed\n")) ;
					Push2ErrStack(PPCERROR+MDC_FLG+ATOM_ERR+15,0,0,0,0);			/*mdc error 15*/
					return -1;
				}

				if(-1 ==  (nNetTaskId=taskSpawn("tMDCDataPack", PRI_NETTRANS, 0, TASK_STACK_SIZE, (FUNCPTR)MDCDataPack, 
										0, 0, 0, 0, 0, 0, 0, 0, 0, 0)))
				{
					DEBUG_OUTPUT(("taskSpawn NormalPack failed\n")) ;
					Push2ErrStack(PPCERROR+MDC_FLG+ATOM_ERR+16,0,0,0,0);			/*mdc error 16*/
					return -1;
				}
				
				break ;
			
			case WAVEMODE:
				if(-1 == (nPackTaskId=taskSpawn("tMDCWaveForm", PRI_DATAPACK, 0, TASK_STACK_SIZE, (FUNCPTR)MDCWaveFormPack, 
										0, 0, 0, 0, 0, 0, 0, 0, 0, 0)))
				{
					DEBUG_OUTPUT(("taskSpawn failed\n") );
					Push2ErrStack(PPCERROR+MDC_FLG+ATOM_ERR+23,0,0,0,0);			/*mdc error 23*/
					return -1;
				}
				break ;
			
			default:
				Push2ErrStack(PPCERROR+MDC_FLG+ATOM_ERR+9,0,0,0,0);			/*mdc error 9*/
				return -1;
		}
	}
	if(g_MDCDBInfo.mcc_sum)
	{
		if(-1 == (nMDCGenL1TaskId= taskSpawn("tMDCAuxTrg", PRI_TAKEDATA, 0, TASK_STACK_SIZE, (FUNCPTR)MDCAuxTrg, 
								0, 0, 0, 0, 0, 0, 0, 0, 0, 0)))
		{
			DEBUG_OUTPUT(("taskSpawn AuxTrg failed\n") );
			Push2ErrStack(PPCERROR+MDC_FLG+ATOM_ERR+28,0,0,0,0);			/*mdc error 28*/
			return -1;
		}
	}
	DEBUG_OUTPUT(("MDCSynchronized4PREP OK\n")) ;
	return 0;
}
int  	MDCReadySATR  			(int nArg)
{ 
	switch(g_nRunMode)
	{
		case NOMALMODE:
		case PEDMODE:
		case BASELINE:
		case WAVEMODE:
			g_bTrgStop=false;
			semGive(g_sidSATR) ;
			break;
		case CALIBRATION:	
			g_nTrgCounter=0; 	//because set mqt's dac,so the triger number start from 0
			g_bTrgStop=false;	//set false,ensure continue calib ,leigk 060803
			semGive(g_sidSATR) ;
			break;
		case MONITOR:
			g_bTrgStop=false;
			semGive(g_sidSATR) ;
		default:
			Push2ErrStack(PPCERROR+MDC_FLG+ATOM_ERR+9,0,0,0,0);			/*mdc error 9*/
			return -1;
	}
	DEBUG_OUTPUT(("MDCReadySATR OK\n") );
	return 0;
}
int  	MDCRunningSPTR  		(int nArg)
{ 
	int nClibInfoLen;
	switch(g_nRunMode)
	{
		case NOMALMODE:
			break;
		case BASELINE:
			break;
		case PEDMODE:
			break;
		case WAVEMODE:
			break;
		case CALIBRATION:
			{
				if(g_nCalibMoni_Points!=1)
				{
					g_nCalibL1OffSet+=g_nL1Count;		//set the L1 offset value
					g_nMQTDAC=*(unsigned int *)g_cCalibMoni_Info;
					g_cCalibMoni_Info=g_cCalibMoni_Info+4;
					g_nMCCT0Latency=*(unsigned int *)g_cCalibMoni_Info;
					g_cCalibMoni_Info=g_cCalibMoni_Info+4;
					g_nL1Count=*(unsigned int *)g_cCalibMoni_Info;
					g_cCalibMoni_Info=g_cCalibMoni_Info+4;
					printf("new MQTCalibDAC=%d,T0Latency=%d,g_nL1Count=%d \n",g_nMQTDAC,g_nMCCT0Latency,g_nL1Count);
					if(g_MDCDBInfo.mcc_sum)
					{
						MDCMCCResetCheck((g_MDCDBInfo.mcc_header->geo & 0xff)); //for reset check signal
						for(int i = 0; i < 500; i ++) ;
						MDCMCCSetData((g_MDCDBInfo.mcc_header->geo & 0xff), g_nMCCT0Latency) ;
						for(i = 0; i < 500; i ++) ;
					}
					if(g_MDCDBInfo.mqt_sum)
					{
						for(int nGEOIdx =g_nMqtEnableSum-1;  nGEOIdx >=0;  nGEOIdx --)	
						{
							MDCSetMQTNewDAC(g_nMqtEnableGeo[nGEOIdx]);
						}
					}
					g_nCalibMoni_Points=g_nCalibMoni_Points-1;
				}	
				else
				{
					g_nCalibL1OffSet = 0;
					g_cCalibMoni_Info  = g_cCalibMoni_Info_ori;																				
					nClibInfoLen=*(unsigned int*)(g_cCalibMoni_Info)-4; 	//the exact calib infor length			
					g_cCalibMoni_Info=g_cCalibMoni_Info+4;                 			//jump the length infor
					g_nCalibMoni_Points=nClibInfoLen/12;						//get the num of calib points
					g_nMQTDAC=*(unsigned int *)g_cCalibMoni_Info;
					g_cCalibMoni_Info=g_cCalibMoni_Info+4;
					g_nMCCT0Latency=*(unsigned int *)g_cCalibMoni_Info;
					g_cCalibMoni_Info=g_cCalibMoni_Info+4;
					g_nL1Count=*(unsigned int *)g_cCalibMoni_Info;
					g_cCalibMoni_Info=g_cCalibMoni_Info+4;				

					if(g_MDCDBInfo.mcc_sum)
					{
						MDCMCCResetCheck((g_MDCDBInfo.mcc_header->geo & 0xff)); //for reset check signal
						for(int i = 0; i < 500; i ++) ;
						MDCMCCSetData((g_MDCDBInfo.mcc_header->geo & 0xff), g_nMCCT0Latency) ;
						for(i = 0; i < 500; i ++) ;
					}
					if(g_MDCDBInfo.mqt_sum)
					{
						for(int nGEOIdx =g_nMqtEnableSum-1;  nGEOIdx >=0;  nGEOIdx --)	
						{
							MDCSetMQTNewDAC(g_nMqtEnableGeo[nGEOIdx]);
						}
					}
					
				}
			}
			break;
		case MONITOR:
			break;
		default:
			Push2ErrStack(PPCERROR+MDC_FLG+ATOM_ERR+9,0,0,0,0);			/*mdc error 9*/
			return -1;
	}
	DEBUG_OUTPUT(("MDCRunningSPTR OK\n") );
	return 0;
}
int  	MDCReadySTOP  			(int nArg)
{ 
	switch(g_nRunMode)
	{
		case NOMALMODE:
		case BASELINE:
		case PEDMODE:
		case WAVEMODE:
			g_bTrgStop=true;
			break;
		case CALIBRATION:	
			g_bTrgStop=true;
			semGive(g_sidSATR) ;   //leigkleigk 060803
			break;
		case MONITOR:
			g_bTrgStop=true;
			break;
		default:
			Push2ErrStack(PPCERROR+MDC_FLG+ATOM_ERR+9,0,0,0,0);			/*mdc error 9*/
			return -1;
	}
	DEBUG_OUTPUT(("MDCReadySTOP OK\n") );
	return 0;
}
int  	MDCConfigedUNCF 		(int nArg)
{ 
	DEBUG_OUTPUT(("MDCConfigedUNCF OK\n") );
	return 0;
}
int  	MDCPreconfigedUNPC 	(int nArg)
{ 
	DEBUG_OUTPUT(("MDCPreconfigedUNPC OK\n") );
	return 0;
}
int  	MDCLoadedUNLD 		(int nArg)
{ 
	DEBUG_OUTPUT(("MDCLoadedUNLD OK\n") );
	return 0;
}
int  	MDCAnyEXIT 		(int nArg)
{ 

	if(nMDCGenL1TaskId)
	{
		if(ERROR==taskDelete(nMDCGenL1TaskId) )
		{
			printf("taskDelete(nMDCGenL1TaskId fail\n");
			Push2ErrStack(PPCERROR+MDC_FLG+ATOM_ERR+111,0,0,0,0);			/*mdc error 111*/
			return -1;
		}
		nMDCGenL1TaskId=0;
	}

	DEBUG_OUTPUT(("MDCAnyEXIT OK\n") );
	return 0;
}

/*
void MDCPrintInfo()
{
	if(g_MDCDBInfo.mcc_sum)
	{
		
		DEBUG_OUTPUT(("MCC INFO:\n" ));
		DEBUG_OUTPUT(("MCC geo=%d,t0_latenccy=%d\n",g_MDCDBInfo.mcc_header->geo,g_MDCDBInfo.mcc_header->T0_latency) );
		
		DEBUG_OUTPUT(("MTI  INFO:\n") );
		DEBUG_OUTPUT(("MTI  geo=%d\n",g_MDCDBInfo.mti_header->geo) );
	}

	if(g_MDCDBInfo.mqt_sum)
	{
		DEBUG_OUTPUT(("ROC INFO:\n"));
		DEBUG_OUTPUT(("ROC geo=%d\n",g_MDCDBInfo.roc_header->geo));
		
		DEBUG_OUTPUT(("MQT INFO:\n"));
		for(int i=0;i<g_MDCDBInfo.mqt_sum;i++)
		{
			printf("MQT  geo=%d\n",g_MDCDBInfo.mqt_header[i]->geo);
			printf("MQT  DAC=%d\n",g_MDCDBInfo.mqt_header[i]->DAC);
										
			printf("MQT TimeThreshold_l=%d,TimeThreshold_h=%d\n",g_MDCDBInfo.mqt_header[i]->TimeThreshold_l,
															g_MDCDBInfo.mqt_header[i]->TimeThreshold_h);
			printf("MQT controlregiste=%d\n",g_MDCDBInfo.mqt_header[i]->controlregister);

			printf("MQT  T_trigger_latency=%d\n",g_MDCDBInfo.mqt_header[i]->T_trigger_latency);
			printf("MQT  Q_trigger_latency=%d\n",g_MDCDBInfo.mqt_header[i]->Q_trigger_latency);
			printf("MQT  T_sample_win=%d\n",g_MDCDBInfo.mqt_header[i]->T_sample_win);

			printf("MQT T_modules_Enable=%d\n",g_MDCDBInfo.mqt_header[i]->T_modules_Enable);
			for(int j=0;j<32;j++)
			{
				printf("MQT  layercell[%d]=%d\n",j,g_MDCDBInfo.mqt_header[i]->layercell[j]);
			}
			for(j=0;j<32;j++)
			{
				printf("MQT  QThreshold[%d]=%d\n",j,g_MDCDBInfo.mqt_header[i]->QThreshold[j]);
			}
		}
	}
}
*/

