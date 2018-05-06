
#include "vxWorks.h"
#include "time.h"
#include "stdlib.h"
#include "taskLib.h"
#include "sysLib.h"
#include "usrLib.h"

#include "5100_test.h"
#include "rng_dma.h"

#include "fw_roppc.h"
#include "fw_runtasks.h"
#include "fw_dcgvas.h"
#include "fw_define.h"
#include "fw_staterrspt.h"
#include "fragheaders.h"
#include "fw_filefunc.h"
#include "fw_tcpfunc.h"

#include "sd_mdc.h"
#include "sd_mdc_runtasks.h"
#include "sd_mdc_datapack.h"
#include "sd_mdc_hw.h"

#define MDC_MAX_RETRY 1000

#define MDC_AUX_TRG_TICKS 1
#define MDC_AUX_TRG_LOOPS (60*150)  /* 60 loops ~= 1 us on MVME5100*/

#define MDC_PRINT

int g_nPointIdx = 0 ;

void MDCDataPack()
{
	char *	p_cCbltRng = NULL ;
	char *	p_cNetRng = NULL ;
	int 		nBytesToBePacked = 0;
	int 		nBytesToBeSent = 0 ;
	int 		nExpectedMaxBytesAfterPacked = 0 ;

	int 		nRetryTimes = 0 ;
	while(1)
	{
		nRetryTimes = 0 ;
		while(msgQReceive(g_msgQcblt,(char *)(&nBytesToBePacked),sizeof(unsigned),TIMEOUT_TICKS)==ERROR)
		{
			nRetryTimes ++ ;
			if( nRetryTimes >MDC_MAX_RETRY)
			{
				DEBUG_OUTPUT( ("msgQReceive(g_msgQcblt..., 1) failed %d times\n", nRetryTimes) );
				Push2ErrStack(PPCWARNING+MDC_FLG+ATOM_ERR+32,0,0,0,0);			/*mdc error 32*/
				nRetryTimes = 0 ;
			}
			if(DPAC_STOP == gv_nRunningCtrl)
			{
				goto MDC_DPAC_EXITING ;
			}
		}
		
		nRetryTimes = 0 ;
		while(NULL == (p_cCbltRng = rng_dmaRead(g_DmaRng,nBytesToBePacked,CPU_METHOD)))
		{
			nRetryTimes ++ ;
			if( nRetryTimes >MDC_MAX_RETRY)
			{
				DEBUG_OUTPUT( ("rng_dmaRead(g_DmaRng...) failed %d times.\n", nRetryTimes) );
				Push2ErrStack(PPCWARNING+MDC_FLG+ATOM_ERR+33,0,0,0,0);			/*mdc error 33*/
				nRetryTimes = 0 ;
			}
			taskDelay(1) ;
		}
		nRetryTimes = 0 ;
		nExpectedMaxBytesAfterPacked=nBytesToBePacked+14*4*g_nTrgPerInt;
		while(NULL == (p_cNetRng = rng_dmaApply(g_NetRng,nExpectedMaxBytesAfterPacked,CPU_METHOD)))
		{
			nRetryTimes ++ ;
			if(nRetryTimes>MDC_MAX_RETRY)
			{
				DEBUG_OUTPUT( ("rng_dmaApply(g_NetRng...) failed %d times.\n", nRetryTimes) ) ;
				Push2ErrStack(PPCWARNING+MDC_FLG+ATOM_ERR+34,0,0,0,0);			/*mdc error 34*/
				nRetryTimes = 0 ;
			}
			taskDelay(1) ;
		}
		if(-1==MDCDataPackCore((UINT*)p_cNetRng, (UINT*)p_cCbltRng, g_nTrgPerInt, 
									g_nMqtEnableSum, g_nMqtEnableGeo, &nBytesToBeSent) )
		{
			nBytesToBeSent=FWAbnormPack((UINT*)p_cNetRng,(UINT*)p_cCbltRng);
			DEBUG_OUTPUT(("Data pack error\n"));
			Push2ErrStack(PPCERROR+MDC_FLG+ATOM_ERR+115,0,0,0,0);			/*mdc error 115*/
		}

		if( nBytesToBeSent >nExpectedMaxBytesAfterPacked )  //check Writen size,must<nMaxNetBytes
		{
			DEBUG_OUTPUT(("ERR:nBytesToBeSent =%d > MaxBytesPackApply\n", nBytesToBeSent )) ;
			Push2ErrStack(PPCERROR+MDC_FLG+ATOM_ERR+34,0,0,0,0);		/*error 34*/
			break;
		}
		
		if(ERROR == rng_dmaWritten(g_NetRng,nBytesToBeSent))
		{
			DEBUG_OUTPUT( ("rng_dmaWritten(g_NetRng...) error\n") );
			Push2ErrStack(PPCWARNING+MDC_FLG+ATOM_ERR+35,0,0,0,0);				/*mdc error 35*/
			break;
		}
		if(ERROR == rng_dmaFree(g_DmaRng,nBytesToBePacked))
		{
			DEBUG_OUTPUT( ("rng_dmaFree(g_DmaRng...) failed\n") );
			Push2ErrStack(PPCWARNING+MDC_FLG+ATOM_ERR+36,0,0,0,0);				/*mdc error 36*/
			break ;
		}
		nRetryTimes = 0 ;
		while(msgQSend(g_msgQnet,(char*)(&nBytesToBeSent),sizeof(int),TIMEOUT_TICKS,MSG_PRI_NORMAL)==ERROR)
		{
			nRetryTimes++;
			if(nRetryTimes>MDC_MAX_RETRY)
			{
				DEBUG_OUTPUT(("msgQSend(g_msgQnet...) fail!\n") );
				Push2ErrStack(PPCWARNING+MDC_FLG+ATOM_ERR+37,0,0,0,0);			/*mdc error 37*/
				nRetryTimes=0;
			}
		}
		
	}

MDC_DPAC_EXITING:
	DEBUG_OUTPUT(("DataPack: DataPackk exited\n")) ;
	gv_nRunningCtrl = NET_STOP ;

	nPackTaskId=0;//if exit normal then not delete at last
	
}

void  MDCWaveFormPack()
{
	int 		nSamplePoints ;
	int 		nPlusOne;
	int 		nBytesPerEvent ;
	char * 	p_cOneEvent ;
	char *	p_cTmp ;
	char *	p_cFakeData ;
	int		nRetryTimes;

	int		nMQTIdx,
			nChnlIdx,
			nTmIdx;
	int		nWaveDataAdrsPort,
			nWaveData;
	int		nMQTPosition;
	
	if(g_MDCDBInfo.mqt_sum) 
	{
		nSamplePoints = g_MDCDBInfo.mqt_header[0]->Q_sample_win ;
		printf("nSamplePoints = %d.\n", nSamplePoints) ;
		if((nSamplePoints%2)==0)
		{
			nPlusOne=0;
		}
		else
		{
			nPlusOne=1;
		}
		nBytesPerEvent = sizeof(UINT32)+ sizeof(RODHeader)+ sizeof(UINT32) + sizeof(RODTrailer) + 
										(32 * (nSamplePoints+ nPlusOne)* sizeof(unsigned short)+sizeof(UINT)) *g_nMqtEnableSum;
		while(NULL == (p_cOneEvent = rng_dmaApply(g_NetRng,nBytesPerEvent,CPU_METHOD)))
		{
			nRetryTimes ++ ;
			taskDelay(1) ;
			if(nRetryTimes>MDC_MAX_RETRY)
			{
				DEBUG_OUTPUT( ("rng_dmaApply(g_NetRng...) failed %d times.\n", nRetryTimes) ) ;
				Push2ErrStack(PPCWARNING+MDC_FLG+ATOM_ERR+79,0,0,0,0);			/*mdc error 79*/
				nRetryTimes = 0 ;
			}

		}
		if(NULL == p_cOneEvent)
		{
			printf("malloc failed\n") ;
			Push2ErrStack(PPCERROR+MDC_FLG+ATOM_ERR+116,0,0,0,0);			/*mdc error 116*/
			return ;
		}
		bzero(p_cOneEvent, nBytesPerEvent) ;

		sysIntEnable(MDC_ROC_INT_LEVEL) ;

		while(1)
		{
			p_cTmp =p_cOneEvent ;
			*(UINT*)p_cTmp= nBytesPerEvent - 4 ;
			p_cTmp += 4;

			g_fwRODHead.level1Id=g_nTrgCounter;

			memcpy(p_cTmp, (char*)&g_fwRODHead, sizeof(g_fwRODHead)) ;
			p_cTmp += sizeof(g_fwRODHead) ;
			
			/*enable interrupt of ROC*/
			MDCROCEnableInt((g_MDCDBInfo.roc_header->geo & 0xff)) ;

			nRetryTimes = 0 ;
			while(ERROR == semTake(g_sidInterrupt, 1))
			{
				nRetryTimes ++ ;
				if( nRetryTimes>MDC_MAX_RETRY)
				{
					DEBUG_OUTPUT(("semTake g_sidRocInt times out %d times\n" ,nRetryTimes)) ;
					Push2ErrStack(PPCWARNING+MDC_FLG+ATOM_ERR+80,0,0,0,0);		/*error 80*/
					nRetryTimes=0;
				}
				if(CBLT_STOP == gv_nRunningCtrl)
				{
					goto MDC_WAVEFORM_EXIT;
				}
			}


			for(nMQTIdx = 0; nMQTIdx < g_nMqtEnableSum; nMQTIdx = nMQTIdx + 1)
			{
				nMQTPosition = g_nMqtEnableGeo[nMQTIdx] ;
				
				*(UINT16*)p_cTmp = nMQTPosition ;
				p_cTmp = p_cTmp + sizeof(UINT16) ;
				*(UINT16*)p_cTmp = nSamplePoints;
				p_cTmp = p_cTmp + sizeof(UINT16) ;
				
				for(nChnlIdx = 0; nChnlIdx < MAX_CHNL_AMOUNT; nChnlIdx = nChnlIdx +1)
				{
					nWaveDataAdrsPort = VA24 + MDC_ADRS_WITH_GEO(nMQTPosition, MDC_MQT_FUNC_ADRS(MDC_MQT_SUB_ACCESS_ADDR) | MDC_MQT_QSB_FUNC_ADRS(MDC_MQT_WAVE_READ) | MDC_MQT_QSB_FUNC_SUB_ADRS(nChnlIdx)) ;
					/*the data of the first time you read in each channel are invalide, so dump it*/
					nWaveData = *(UINT32*)nWaveDataAdrsPort;
					// kejun skip one more point
					nWaveData = *(UINT32*)nWaveDataAdrsPort;
					
					for(nTmIdx = 0; nTmIdx < nSamplePoints; nTmIdx++)
					{
						nWaveData = *(UINT32*)nWaveDataAdrsPort;
						*(UINT16*)p_cTmp  = nWaveData ;
						//if((*(UINT16*)p_cTmp) > 1024) printf("0x%x is read, g_nInterruptCounter: %d, slot#: %d, channel#: %d, nSamplePoint: %d .\n", *(UINT16*)p_cTmp, g_nInterruptCounter, nMQTPosition, nChnlIdx, 2*nTmIdx) ;
						p_cTmp = p_cTmp + sizeof(UINT16) ;
					}
					if(nPlusOne==1)
					{
						*(UINT16*)p_cTmp  = 0xffff ;
						p_cTmp = p_cTmp + sizeof(UINT16) ;
					}
			}
		}
			 
		/*Status*/
		*(int*)p_cTmp = (0) ;
		p_cTmp += sizeof(int) ;

		g_fwRODTail.numberOfDataElements = (32 * (nSamplePoints+ nPlusOne)/2+1)*g_nMqtEnableSum;
		memcpy(p_cTmp, (char*)&g_fwRODTail, sizeof(g_fwRODTail)) ;

		//printf("before send, nBytesPerEvent = %d\n", nBytesPerEvent) ;
		if (nBytesPerEvent !=SelectSend(g_sktData, p_cOneEvent, nBytesPerEvent,g_DataTcpcb))
		{
			DEBUG_OUTPUT( ("(WAVE)g_sktData send failed\n") );
			Push2ErrStack(PPCFATAL+MDC_FLG+ATOM_ERR+81,0,0,0,0);		/*error 81*/
			return ;
		}

		g_nTrgCounter = g_nTrgCounter + 1 ;
		g_nEventsSent=g_nEventsSent+1;
		
		if(CBLT_STOP == gv_nRunningCtrl)
		{
			goto MDC_WAVEFORM_EXIT;
		}
		}
		
	}

MDC_WAVEFORM_EXIT:
	if(g_MDCDBInfo.mqt_sum)
	{
		nBytesPerEvent = g_nTrgPerInt*14*4 ;
		p_cFakeData = (char*) malloc(nBytesPerEvent) ;
		nBytesPerEvent = FWBlankPack((UINT *) p_cFakeData) ;
		*(UINT*)(p_cFakeData + 4) = LAST_EVENT_RODMARKER ;
		if (nBytesPerEvent !=SelectSend(g_sktData, p_cFakeData, nBytesPerEvent,g_DataTcpcb))
		{
			DEBUG_OUTPUT( ("Sending the last data failed.\n") );
			Push2ErrStack(PPCFATAL+MDC_FLG+ATOM_ERR+82,0,0,0,0);		/*error 82*/
			return ;
		}
		DEBUG_OUTPUT(("send last event\n")) ;
		free(p_cFakeData) ;
	}
	semGive(g_sidBufEmpty) ;
	DEBUG_OUTPUT(("WaveForm exited\n")) ;
	nPackTaskId=0;//if exit normal then not delete at last
}


void MDCAuxTrg()
{
	UINT32 nTimeIdx=0 ;
	int i ;
	DEBUG_OUTPUT(("auxtrg before take g_sidSATR \n"));
	semTake(g_sidSATR, WAIT_FOREVER) ;
	DEBUG_OUTPUT(("auxtrg after take g_sidSATR\n"));

	int nMtiGeoTemp=g_MDCDBInfo.mti_header->geo;
	
	switch (g_nRunMode)
	{
		case NOMALMODE:
			if(g_MDCDBInfo.ppc_header->NormModeSelfL1)
			{
				while(1)
				{
					while(1 == (0x1 & (VALUE_OF_A24D32(MDC_ADRS_WITH_GEO(nMtiGeoTemp, MDC_MTI_FULL_ADRS)) ))) ;
					MDCMCCStart((g_MDCDBInfo.mcc_header->geo & 0xff)) ;
					for(i = 0; i < MDC_AUX_TRG_LOOPS; i = i + 1) ;
					if(g_bTrgStop)break ;
				}
			}
			else
			{
				while(1)
				{
					taskDelay(1) ;
					if(g_bTrgStop)   
					{
						break;
					}
				}
			}
					
			break;
		case PEDMODE:
			while(1)
			{
				while(1 == (0x1 & (VALUE_OF_A24D32(MDC_ADRS_WITH_GEO(nMtiGeoTemp, MDC_MTI_FULL_ADRS)) ))) ;
				MDCMCCStart((g_MDCDBInfo.mcc_header->geo & 0xff)) ;
				taskDelay(1) ;
				if(g_bTrgStop)   
				{
					break;
				}
			}			
			break;
		case CALIBRATION:
			while(1)
			{
				for(nTimeIdx= 0; nTimeIdx < g_nL1Count; nTimeIdx ++)
				{
					while(1 == (0x1 & (VALUE_OF_A24D32(MDC_ADRS_WITH_GEO(nMtiGeoTemp, MDC_MTI_FULL_ADRS)) ))) ;
					MDCMCCStart((g_MDCDBInfo.mcc_header->geo & 0xff)) ;
					taskDelay(MDC_AUX_TRG_TICKS) ;
					//for(i = 0; i < MDC_AUX_TRG_LOOPS; i = i + 1) ;
				}
				semTake(g_sidSATR, WAIT_FOREVER) ;				
				if(g_bTrgStop)break;
				DEBUG_OUTPUT(("Point: %d done.\n", g_nPointIdx)) ;
				g_nPointIdx ++ ;
			}		
			break ;
		case WAVEMODE: 
			while(1)
			{
				while(1 == (0x1 & (VALUE_OF_A24D32(MDC_ADRS_WITH_GEO(nMtiGeoTemp, MDC_MTI_FULL_ADRS)) ))) ;
				MDCMCCStart((g_MDCDBInfo.mcc_header->geo & 0xff)) ;
				//for(i = 0; i < MDC_AUX_TRG_LOOPS; i = i + 1) ;
				taskDelay(30);	
				if(g_bTrgStop)break ;
			}
			break;	
		case BASELINE:
		case MONITOR:
			while(1)
			{
				taskDelay(1) ;
				if(g_bTrgStop)   
				{
					break;
				}
			}
			break;
		default:
				Push2ErrStack(PPCFATAL+MDC_FLG+ATOM_ERR+9,0,0,0,0);			/*mdc error 9*/
				break;
	}

	DEBUG_OUTPUT(("nTimeIdx = %d. \n", nTimeIdx)) ;
	semGive(g_sidBufEmpty) ;				//this give for 6u crate to stop,be sure
	DEBUG_OUTPUT(("MDCAuxTrg exited.\n")) ;
	nMDCGenL1TaskId=0;
}


int MDCEnteringCbltTrans(int nArg)
{
	g_nMaxCbltBytes = g_nTrgPerInt * MDC_MAX_BYTES_PERMDL * g_nMqtEnableSum; 

	g_nCbltAdrs = MDC_MQT_CBLT_ADRS ;
	DEBUG_OUTPUT(("g_nCbltAdrs = 0x%x.\n", g_nCbltAdrs)) ;

	sysIntEnable(MDC_ROC_INT_LEVEL) ;

	return 0 ;
}

int MDCBeforeInterrupt(int nArg)
{
	MDCROCEnableInt((g_MDCDBInfo.roc_header->geo & 0xff)) ;
	return 0  ;
}


int MDCBeforeDMARun(int nArg)
{
	int i ;
	//for(i = 0; i < 20000; i ++) ;
	return 0 ;
}

int MDCAfterDMARun(int nArg)
{
	/*check status*/
	int nMQTPosition ;
	int nMappedAddress ;
	int nMQTIdx ;

	for(nMQTIdx = 0; nMQTIdx < g_nMqtEnableSum; nMQTIdx ++)
	{	
			nMQTPosition = g_nMqtEnableGeo[nMQTIdx] ;
			nMappedAddress = MDC_ADRS_WITH_GEO(nMQTPosition, (MDC_MQT_FUNC_ADRS(MDC_MQT_STATUS) | MDC_MQT_FUNC_SUB_ADRS(0X0))) ;
	}

	return 0 ;
}

