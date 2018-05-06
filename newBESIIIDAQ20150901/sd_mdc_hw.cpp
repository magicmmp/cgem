#include "vxWorks.h"
#include "sysLib.h"
#include "semLib.h"
#include "stdio.h"
#include "taskLib.h"
#include "intLib.h"
#include "arch/ppc/ivPpc.h"


#include "fw_roppc.h"
#include "fw_dcgvas.h"
#include "fw_define.h"
#include "fw_staterrspt.h"
#include "5100_test.h"

#include "sd_mdc.h"
#include "sd_mdc_hw.h"
#include "sd_mdc_mqtv24.h"

#ifdef DEBUG_OUTPUT
#undef DEBUG_OUTPUT
#define DEBUG_OUTPUT(x)  /* printf x */
#endif

bool g_bMDCRocIntConed;

void MDCRocIntHandler()
{
	semGive(g_sidInterrupt) ;
	g_nInterruptCounter+=g_nTrgPerInt ;
}
int 	MDCConfigIntCBLT()
{
		if(FALSE == g_bMDCRocIntConed) 
		{
			if(intConnect (INUM_TO_IVEC(MDC_ROC_INT_VECTOR), (VOIDFUNCPTR)MDCRocIntHandler, 0) == ERROR)
		  	{
		     		DEBUG_OUTPUT(("intConnect  failed.\n"));
				Push2ErrStack(PPCERROR+MDC_FLG+ATOM_ERR+83,0,0,0,0);			/*mdc error 83*/
				return  -1;
		  	}
			g_bMDCRocIntConed = TRUE ;
		}

		if(-1 == CBLTinit(64))
		{
			DEBUG_OUTPUT(("CBLTinit failed\n")) ;
			Push2ErrStack(PPCERROR+MDC_FLG+ATOM_ERR+84,0,0,0,0);			/*mdc error 84*/
			return  -1;
		}		
		DEBUG_OUTPUT(("after CBLTinit.\n")) ;
}
/*routines for MCC*/
void MDCMCCReset(int nSlot)
{
	if(0 == nSlot)
	{
		printf("WARNNING: MCC geo = 0\n") ;
		Push2ErrStack(PPCWARNING+MDC_FLG+ATOM_ERR+112,0,0,0,0);			/*mdc error 112*/
	}
	VALUE_OF_A24D32(MDC_ADRS_WITH_GEO(nSlot, MDC_MCC_RESET_ADRS)) = 0 ;
}

void MDCMCCSetData(int nSlot, UINT32 nValue)
{
	if(0 == nSlot)
	{
		printf("WARNNING: MCC geo = 0\n") ;
		Push2ErrStack(PPCWARNING+MDC_FLG+ATOM_ERR+112,0,0,0,0);			/*mdc error 112*/
	}
	VALUE_OF_A24D32(MDC_ADRS_WITH_GEO(nSlot, MDC_MCC_SET_DATA_ADRS)) = nValue ;
}

void MDCMCCCalib(int nSlot)
{
	if(0 == nSlot)
	{
		printf("WARNNING: MCC geo = 0\n") ;
		Push2ErrStack(PPCWARNING+MDC_FLG+ATOM_ERR+112,0,0,0,0);			/*mdc error 112*/
	}
	VALUE_OF_A24D32(MDC_ADRS_WITH_GEO(nSlot, MDC_MCC_CALIB_ADRS)) = 0 ;
}

void MDCMCCCrystOn(int nSlot)
{
	if(0 == nSlot)
	{
		printf("WARNNING: MCC geo = 0\n") ;
		Push2ErrStack(PPCWARNING+MDC_FLG+ATOM_ERR+112,0,0,0,0);			/*mdc error 112*/
	}
	VALUE_OF_A24D32(MDC_ADRS_WITH_GEO(nSlot, MDC_MCC_CRYST_ON_ADRS)) = 0 ;
}

void MDCMCCResetTDC(int nSlot)
{
	if(0 == nSlot)
	{
		printf("WARNNING: MCC geo = 0\n") ;
		Push2ErrStack(PPCWARNING+MDC_FLG+ATOM_ERR+112,0,0,0,0);			/*mdc error 112*/
	}
	VALUE_OF_A24D32(MDC_ADRS_WITH_GEO(nSlot, MDC_MCC_RESET_TDC_ADRS)) = 0 ;
}

void MDCMCCOnline(int nSlot)
{
	if(0 == nSlot)
	{
		printf("WARNNING: MCC geo = 0\n") ;
		Push2ErrStack(PPCWARNING+MDC_FLG+ATOM_ERR+112,0,0,0,0);			/*mdc error 112*/
	}
	printf("MDCMCCOnline: MCC geo = %d\n",nSlot) ;
	VALUE_OF_A24D32(MDC_ADRS_WITH_GEO(nSlot, MDC_MCC_ONLINE_ADRS)) = 0 ;
}

void MDCMCCReload(int nSlot)
{
	if(0 == nSlot)
	{
		printf("WARNNING: MCC geo = 0\n") ;
		Push2ErrStack(PPCWARNING+MDC_FLG+ATOM_ERR+112,0,0,0,0);			/*mdc error 112*/
	}
	
	VALUE_OF_A24D32(MDC_ADRS_WITH_GEO(nSlot, MDC_MCC_RELOAD_ADRS)) = 0 ;
}

void MDCMCCStart(int nSlot)
{
	if(0 == nSlot)
	{
		printf("WARNNING: MCC geo = 0\n") ;
		Push2ErrStack(PPCWARNING+MDC_FLG+ATOM_ERR+112,0,0,0,0);			/*mdc error 112*/
	}
	VALUE_OF_A24D32(MDC_ADRS_WITH_GEO(nSlot, MDC_MCC_START_ADRS)) = 0 ;
}

void MDCMCCResetCheck(int nSlot)   //leigkleigk060720
{
	if(0 == nSlot)
	{
		printf("WARNNING: MCC geo = 0\n") ;
		Push2ErrStack(PPCWARNING+MDC_FLG+ATOM_ERR+112,0,0,0,0);			/*mdc error 112*/
	}
	VALUE_OF_A24D32(MDC_ADRS_WITH_GEO(nSlot, MDC_MCC_SET_CHECK_ADRS)) = 0 ;
}

/*routines for ROC*/
void MDCROCReset(int nSlot)
{
	if(0 == nSlot)
	{
		printf("WARNNING: ROC geo = 0\n") ;
		Push2ErrStack(PPCWARNING+MDC_FLG+ATOM_ERR+113,0,0,0,0);			/*mdc error 113*/
	}
	VALUE_OF_A24D32(MDC_ADRS_WITH_GEO(nSlot, MDC_ROC_RESET_ADRS)) = 0 ;
}

void MDCROCSetN(int nSlot, UINT32 nValue)
{
	if(0 == nSlot)
	{
		printf("WARNNING: ROC geo = 0\n") ;
		Push2ErrStack(PPCWARNING+MDC_FLG+ATOM_ERR+113,0,0,0,0);			/*mdc error 113*/
	}
	VALUE_OF_A24D32(MDC_ADRS_WITH_GEO(nSlot, MDC_ROC_SET_N_ADRS)) = nValue ;
}

UINT32 MDCROCReadN(int nSlot) 
{
	UINT32 nReturn ;
	if(0 == nSlot)
	{
		printf("WARNNING: ROC geo = 0\n") ;
		Push2ErrStack(PPCWARNING+MDC_FLG+ATOM_ERR+113,0,0,0,0);			/*mdc error 113*/
	}
	nReturn = VALUE_OF_A24D32(MDC_ADRS_WITH_GEO(nSlot, MDC_ROC_READ_N_ADRS)) ;
	return nReturn ;
}

void MDCROCEnableInt(int nSlot)
{
	UINT32 nReturn ;
	if(0 == nSlot)
	{
		printf("WARNNING: ROC geo = 0\n") ;
		Push2ErrStack(PPCWARNING+MDC_FLG+ATOM_ERR+113,0,0,0,0);			/*mdc error 113*/
	}
	nReturn = VALUE_OF_A24D32(MDC_ADRS_WITH_GEO(nSlot, MDC_ROC_ENABLE_INT)) ;
}


int MDCMQTResetMainBd(int nMQTIdx) /*step 1 reset main board*/
{
	//for(int nMQTIdx =(g_MDCDBInfo.mqt_sum -1);  nMQTIdx >=0;  nMQTIdx --)	
	{
		int nMQTPosition = g_MDCDBInfo.mqt_header[nMQTIdx]->geo ;
		int nDataWrite = 0 ;
		UINT nMappedAddress = MDC_ADRS_WITH_GEO(nMQTPosition, MDC_MQT_FUNC_ADRS(MDC_MQT_RESET_ADDR)) ;
		VALUE_OF_A24D32(nMappedAddress) = nDataWrite;

		DEBUG_OUTPUT(("+++reset mainboard: nMQTPosition = %d, nMappedAddress = 0x%x, nDataWrite = %d .\n", nMQTPosition, nMappedAddress, nDataWrite)) ;
		DEBUG_OUTPUT(("\n")) ;
	}
	taskDelay(1) ;
	return 0;
}
int MDCMQTEnsureClockInput(int nMQTIdx)/*step 2 ensure clock input*/
{
	//for(int nMQTIdx =(g_MDCDBInfo.mqt_sum -1);  nMQTIdx >=0;  nMQTIdx --)	
	{
		int 	nMQTPosition = g_MDCDBInfo.mqt_header[nMQTIdx]->geo ;
		UINT nMappedAddress = MDC_ADRS_WITH_GEO(nMQTPosition, MDC_MQT_FUNC_ADRS(MDC_MQT_STATUS_ADDR)) ;
		UINT nDataRead = VALUE_OF_A24D32(nMappedAddress) ;
		DEBUG_OUTPUT(("+++MQT status reg: nMQTPosition = %d, nMappedAddress = 0x%x, nDataRead = 0x%x .\n", nMQTPosition, nMappedAddress, nDataRead)) ;
		if(1 != ( nDataRead & 1))
		{
			printf("No clock input.\n") ;
			Push2ErrStack(PPCFATAL+MDC_FLG+ATOM_ERR+114,0,0,0,0);			/*mdc error 114*/
			return -1;
		}
	}
	taskDelay(1) ;
	return 0;
}
int MDCMQTHptdcInit(int nMQTIdx) /*step 3 hptdc init*/
{
	UINT nTimeSampleWindow;
	UINT nTimeTriggerLatency;
	//for(int nMQTIdx =(g_MDCDBInfo.mqt_sum -1);  nMQTIdx >=0;  nMQTIdx --)	
	{
		int 	nMQTPosition = g_MDCDBInfo.mqt_header[nMQTIdx]->geo ;
		UINT nTimeChannelEnable  = g_MDCDBInfo.mqt_header[nMQTIdx]->T_modules_Enable;
		switch(g_nRunMode)
		{
			case NOMALMODE:
			case BASELINE:
			case PEDMODE:	
			case WAVEMODE:
			case MONITOR:	
				nTimeSampleWindow = g_MDCDBInfo.mqt_header[nMQTIdx]->T_sample_win ;
//				printf("***nTimeSampleWindow in PEDMODE = %d , while in CALIBMODE is 40 \n",nTimeSampleWindow);
				nTimeTriggerLatency = g_MDCDBInfo.mqt_header[nMQTIdx]->T_trigger_latency ;
//				printf("***nTimeTriggerLatency in PEDMODE = %d , while in CALIBMODE is 248 \n",nTimeTriggerLatency);
				break;
			case CALIBRATION:
//				nTimeSampleWindow = 40 ;
				nTimeSampleWindow = g_MDCDBInfo.mqt_header[nMQTIdx]->T_sample_win ;
				nTimeTriggerLatency = 248 ;
				break;
		}
		
		DEBUG_OUTPUT(("+++HPTDC init MQT slot %d, TimeChannelEnable:0x%x,nTimeSampleWindow: 0x%x, nTimeTriggerLatency:0x%x.\n", nMQTPosition, nTimeChannelEnable, nTimeSampleWindow, nTimeTriggerLatency)) ;
		printf("+++HPTDC init MQT slot %d, TimeChannelEnable:0x%x,nTimeSampleWindow: 0x%x, nTimeTriggerLatency:0x%x.\n", nMQTPosition, nTimeChannelEnable, nTimeSampleWindow, nTimeTriggerLatency);
		if(0 != hptdcupload(nMQTPosition, nTimeChannelEnable, nTimeSampleWindow,  nTimeTriggerLatency))
		{
			DEBUG_OUTPUT(("hptdcupload failed, MQT at slot %d.\n", nMQTPosition)) ;
			Push2ErrStack(PPCFATAL+MDC_FLG+ATOM_ERR+85,0,0,0,0);			/*mdc error 85*/
			return -1 ;
		}
	}
	taskDelay(1) ;
	return 0;
}
int MDCMQTSetupQL1latency(int nMQTIdx)/*step 5 setup Q L1latency*/
{
	int 		nMQTPosition;
	int 		nSubBoardQL1Latency;
	UINT 	nMappedAddress;
	//for(int nMQTIdx =(g_MDCDBInfo.mqt_sum -1);  nMQTIdx >=0;  nMQTIdx --)	
	{
		nMQTPosition = g_MDCDBInfo.mqt_header[nMQTIdx]->geo ;
		DEBUG_OUTPUT(("+++setup Q L1 Latency register\n")) ;
		for(int nSubIdx = 0; nSubIdx < MDC_MQT_SUB_AMOUNT; nSubIdx ++)
		{
			printf("g_MDCDBInfo.mqt_header[%d]->Q_trigger_latency = %d\n", nMQTIdx,g_MDCDBInfo.mqt_header[nMQTIdx]->Q_trigger_latency);
			if((0 == g_MDCDBInfo.mqt_header[nMQTIdx]->Q_trigger_latency) || (511 <  g_MDCDBInfo.mqt_header[nMQTIdx]->Q_trigger_latency))
			{
				printf("MQT Geo = %d, Sub board ID = %d, Q_trigger_latency == 0 or > 255!\n",nMQTPosition, nSubIdx) ;
			}
			
			else
			{
				switch(g_nRunMode)
				{
					case NOMALMODE:
					case BASELINE:	
					case PEDMODE:	
					case WAVEMODE:
					case MONITOR:	
						nSubBoardQL1Latency= g_MDCDBInfo.mqt_header[nMQTIdx]->Q_trigger_latency ;
//						printf("***nSubBoardQL1Latency in PEDMODE = %d , while in CALIBMODE is 248 \n",nSubBoardQL1Latency);
						break;
					case CALIBRATION:
						nSubBoardQL1Latency= 248 ;
						break ;
				}
			}
			nMappedAddress = MDC_ADRS_WITH_GEO(nMQTPosition, MDC_MQT_FUNC_ADRS(MDC_MQT_SUB_ACCESS_ADDR) | MDC_MQT_QSB_FUNC_ADRS(MDC_MQT_SUB_SET_QL1) | MDC_MQT_QSB_FUNC_SUB_ADRS(nSubIdx << 3)) ;
			DEBUG_OUTPUT(("+++MQT at slot %d, sub board %d, the mapped address: 0x%x, Q L1 Latency : %d\n", nMQTPosition, nSubIdx, nMappedAddress, nSubBoardQL1Latency)) ;
			VALUE_OF_A24D32(nMappedAddress) = nSubBoardQL1Latency;
			taskDelay(1) ;
			if(nSubBoardQL1Latency != VALUE_OF_A24D32(nMappedAddress))
			{
				printf("setting sub board trigger delay register failed at MQT slot#: %d, sub board#: %d\n", nMQTPosition, nSubIdx) ;
				Push2ErrStack(PPCFATAL+MDC_FLG+ATOM_ERR+86,0,0,0,0);			/*mdc error 86*/
				return -1 ;
			}

		}

		for(nSubIdx = 0; nSubIdx < 4; nSubIdx = nSubIdx + 1)
		{
			/*check the sub board error registers*/
			nMappedAddress = MDC_ADRS_WITH_GEO(nMQTPosition, MDC_MQT_FUNC_ADRS(MDC_MQT_SUB_ACCESS_ADDR) | MDC_MQT_QSB_FUNC_ADRS(MDC_MQT_SUB_ERR_REG) | MDC_MQT_QSB_FUNC_SUB_ADRS(nSubIdx << 3)) ;
			int nDataRead = VALUE_OF_A24D32(nMappedAddress)  ;

			DEBUG_OUTPUT(("+++sub board error register, nMappedAddress = 0x%08x, nDataRead = 0x%08x\n", nMappedAddress,nDataRead) ) ;

			if(1 != ((nDataRead& 0x4) >> 2))
			{
				printf("Phase lock at sub board #%d is not 1!\n", nSubIdx) ;
				Push2ErrStack(PPCFATAL+MDC_FLG+ATOM_ERR+87,0,0,0,0);			/*mdc error 87*/
			}
		}
		DEBUG_OUTPUT(("\n")) ;
		nMappedAddress = MDC_ADRS_WITH_GEO(nMQTPosition, MDC_MQT_FUNC_ADRS(MDC_MQT_STATUS_ADDR)) ;
		DEBUG_OUTPUT(("+++Status Register: values is 0x%08x, at slot#%d, function address is 0x%x.\n", VALUE_OF_A24D32(nMappedAddress) , nMQTPosition, MDC_MQT_STATUS_ADDR)) ;
	}
	taskDelay(1) ;
	return 0;
}
int MDCMQTSubBdQSampWin(int nMQTIdx)/*step 6 sub board Q sample window*/
{
	DEBUG_OUTPUT(("+++setup sub board Q sample window\n")) ;
	int 	nMQTPosition;
	int	nSubBoardQSmpWin=0;
	//for(int nMQTIdx =(g_MDCDBInfo.mqt_sum -1);  nMQTIdx >=0;  nMQTIdx --)	
	{
		nMQTPosition = g_MDCDBInfo.mqt_header[nMQTIdx]->geo ;
		for(int nSubIdx = 0; nSubIdx < MDC_MQT_SUB_AMOUNT; nSubIdx ++)
		{
			if((0 == g_MDCDBInfo.mqt_header[nMQTIdx]->Q_sample_win) || (255 <  g_MDCDBInfo.mqt_header[nMQTIdx]->Q_sample_win))
			{
				printf("MQT Geo = %d, Sub board ID = %d, Q_sample_win == 0 or > 255!\n", nMQTPosition, nSubIdx) ;
			}
			else
			{
				switch(g_nRunMode)
				{
					case NOMALMODE:
					case BASELINE:
					case PEDMODE:
					case WAVEMODE:
					case MONITOR:
						nSubBoardQSmpWin = g_MDCDBInfo.mqt_header[nMQTIdx]->Q_sample_win;
//						printf("***nSubBoardQSmpWin in PEDMODE = %d , while in CALIBMODE is 40 \n",nSubBoardQSmpWin);
						break;
					case CALIBRATION:
//						nSubBoardQSmpWin = 40;
						nSubBoardQSmpWin = g_MDCDBInfo.mqt_header[nMQTIdx]->Q_sample_win;
						break ;
						
				}
			}

			UINT nMappedAddress = MDC_ADRS_WITH_GEO(nMQTPosition, MDC_MQT_FUNC_ADRS(MDC_MQT_SUB_ACCESS_ADDR) | MDC_MQT_QSB_FUNC_ADRS(MDC_MQT_SUB_SAMPLE_WINDOW) | MDC_MQT_QSB_FUNC_SUB_ADRS(nSubIdx << 3)) ;
			DEBUG_OUTPUT(("+++MQT at slot %d, sub board %d, the mapped address: 0x%x, sub board sample window: %d\n", nMQTPosition, nSubIdx, nMappedAddress, nSubBoardQSmpWin) );

			VALUE_OF_A24D32(nMappedAddress) = nSubBoardQSmpWin;
			if(nSubBoardQSmpWin != VALUE_OF_A24D32(nMappedAddress))
			{
				printf("setting sub board sample window register failed at MQT slot#: %d, sub board#: %d\n", nMQTPosition, nSubIdx) ;
				Push2ErrStack(PPCFATAL+MDC_FLG+ATOM_ERR+88,0,0,0,0);			/*mdc error 88*/
				
				return -1 ;
			}
		}
	}
	return 0;
}
int MDCMQTSubBdBaseLine(int nMQTIdx)/*7. sub board base line*/
{
	DEBUG_OUTPUT(("+++write the baseline to MQT.\n")) ;
	int nSubBoardQBaseline;
	//for(int nMQTIdx =(g_MDCDBInfo.mqt_sum -1);  nMQTIdx >=0;  nMQTIdx --)	
	{
		int nMQTPosition = g_MDCDBInfo.mqt_header[nMQTIdx]->geo ;
		for(int nChnlIdx = 0; nChnlIdx < MAX_CHNL_AMOUNT; nChnlIdx ++)
		{
			switch(g_nRunMode)		//leigk 070403
			{
				case NOMALMODE:	
				case BASELINE:	
				case PEDMODE:	
				case WAVEMODE:
				case MONITOR:
				case CALIBRATION:
					nSubBoardQBaseline =(g_MDCDBInfo.mqt_header[nMQTIdx]->controlregister & 0x8000);	//0x8000-no deduct;0x0-deduct			
					break ;
			}
			UINT nMappedAddress = MDC_ADRS_WITH_GEO(nMQTPosition, MDC_MQT_FUNC_ADRS(MDC_MQT_SUB_ACCESS_ADDR) | MDC_MQT_QSB_FUNC_ADRS(MDC_MQT_SUB_BASELINE) | MDC_MQT_QSB_FUNC_SUB_ADRS(nChnlIdx)) ;
			DEBUG_OUTPUT(("+++MQT at slot %d, channel %d, the mapped address: 0x%x, Q sub board baseline: %d.\n", nMQTPosition, nChnlIdx, nMappedAddress,nSubBoardQBaseline )) ;
			VALUE_OF_A24D32(nMappedAddress) = nSubBoardQBaseline;
			if(nSubBoardQBaseline!= VALUE_OF_A24D32(nMappedAddress))
			{
				printf("setting sub board base line mode failed at MQT slot#: %d, channel#: %d\n", nMQTPosition, nChnlIdx) ;
				Push2ErrStack(PPCFATAL+MDC_FLG+ATOM_ERR+89,0,0,0,0);			/*mdc error 89*/
				
				return -1 ;
			}
		}	
	}
	return 0;
}
int MDCMQTSubBdQthresh(int nMQTIdx)/*8. sub board Q threshold*/
{
	DEBUG_OUTPUT(("+++write the Q threshold to MQT.\n")) ;
	int nSubBoardQThreshold;
	//for(int nMQTIdx =(g_MDCDBInfo.mqt_sum -1);  nMQTIdx >=0;  nMQTIdx --)	
	{
		int nMQTPosition = g_MDCDBInfo.mqt_header[nMQTIdx]->geo ;
		for(int nChnlIdx = 0; nChnlIdx < MAX_CHNL_AMOUNT; nChnlIdx ++)
		{
			switch(g_nRunMode)
			{
				case NOMALMODE:
					nSubBoardQThreshold = g_MDCDBInfo.mqt_header[nMQTIdx]->QThreshold[nChnlIdx]  ;
//					nSubBoardQThreshold = 0 ;
					break ;
				case BASELINE:
				case PEDMODE:
				case WAVEMODE:
				case MONITOR:
				case CALIBRATION:
					nSubBoardQThreshold = 0 ;
					break ;
			}
			UINT nMappedAddress = MDC_ADRS_WITH_GEO(nMQTPosition, MDC_MQT_FUNC_ADRS(MDC_MQT_SUB_ACCESS_ADDR) | MDC_MQT_QSB_FUNC_ADRS(MDC_MQT_SUB_THRESHOLD) | MDC_MQT_QSB_FUNC_SUB_ADRS(nChnlIdx))  ;
			DEBUG_OUTPUT(("+++MQT at slot %d, channel %d, the mapped address: 0x%x, Q sub board threshold: %d.\n", nMQTPosition, nChnlIdx, nMappedAddress,nSubBoardQThreshold )) ;
			VALUE_OF_A24D32(nMappedAddress)= nSubBoardQThreshold;
			if(nSubBoardQThreshold != VALUE_OF_A24D32(nMappedAddress))
			{
				printf("setting sub board threshold failed at MQT slot#: %d, channel#: %d\n", nMQTPosition, nChnlIdx) ;
				Push2ErrStack(PPCFATAL+MDC_FLG+ATOM_ERR+90,0,0,0,0);			/*mdc error 90*/
				return -1 ;
			}
		}
	}
	return 0;
}
int MDCMQTSetCBLTReg(int nMQTIdx)/*9.CBLT register 1 2 3*/
{
	int 	nDataWrite;
	UINT nMappedAddress;
	int 	nCBLTCtrlReg = 0xaa ;
	//for(int nMQTIdx =(g_MDCDBInfo.mqt_sum -1);  nMQTIdx >=0;  nMQTIdx --)	
	{
		int nMQTPosition = g_MDCDBInfo.mqt_header[nMQTIdx]->geo ;
		DEBUG_OUTPUT(("+++setup CBLT control register 1.\n")) ;/*register 1*/
		//nDataWrite = 5 ; //leigkleigk 060720 00 01 10 11
 		nDataWrite =(g_MDCDBInfo.mqt_header[nMQTIdx]->QT_hit_option & 0xf);	//leigk070403
		
		nMappedAddress = MDC_ADRS_WITH_GEO(nMQTPosition, MDC_MQT_FUNC_ADRS(MDC_MQT_CRCSR_SPACE) | MDC_MQT_FUNC_SUB_ADRS(MDC_MQT_CBLT_REG1)) ;
		VALUE_OF_A24D32(nMappedAddress) = nDataWrite; 

		DEBUG_OUTPUT(("+++MQT at slot %d, nMappedAddress: 0x%x, nDataWrite = 0x%x.\n",nMQTPosition, nMappedAddress, nDataWrite )) ;
		if(nDataWrite != VALUE_OF_A24D32(nMappedAddress) )
		{
			printf("Failed to set CBLT register 1, MQT slot# = %d.\n", nMQTPosition) ;
			Push2ErrStack(PPCFATAL+MDC_FLG+ATOM_ERR+91,0,0,0,0);			/*mdc error 91*/
			return -1 ;
		}	
		DEBUG_OUTPUT(("\n")) ;
		DEBUG_OUTPUT(("setup CBLT control register 2.\n")) ;/*register 2*/
		nMappedAddress = MDC_ADRS_WITH_GEO(nMQTPosition, MDC_MQT_FUNC_ADRS(MDC_MQT_CRCSR_SPACE) | MDC_MQT_FUNC_SUB_ADRS(MDC_MQT_CBLT_REG2)) ;
		VALUE_OF_A24D32(nMappedAddress) = g_nTrgPerInt ;
		DEBUG_OUTPUT(("MQT at slot %d, the mapped address: 0x%x, the value = 0x%x.\n",nMQTPosition, nMappedAddress, g_nTrgPerInt )) ;

		if(g_nTrgPerInt!= VALUE_OF_A24D32(nMappedAddress) )
		{
			printf("Failed to set CBLT register 2, MQT slot# = %d.\n", nMQTPosition) ;
			Push2ErrStack(PPCFATAL+MDC_FLG+ATOM_ERR+92,0,0,0,0);			/*mdc error 92*/
			return -1 ;
		}	
		DEBUG_OUTPUT(("\n")) ;
		DEBUG_OUTPUT(("setup CBLT control register 3.\n")) ;/* CBLT register 3*/
		switch(g_nRunMode)
		{
			case NOMALMODE:
			case BASELINE:
			case PEDMODE:
			case MONITOR:
			case CALIBRATION:
				if(1 == g_nMqtEnableSum)
				{
					DEBUG_OUTPUT(("There is only one MQT.\n")) ;
					nCBLTCtrlReg = 0x55 ;
				}
				else
				{
					if(g_nMqtEnableGeo[0] == nMQTPosition)
					{/*the first MQT*/
						nCBLTCtrlReg = 0x59 ;
					}
					else if(g_nMqtEnableGeo[g_nMqtEnableSum-1]== nMQTPosition)
					{/*the last MQT*/
						nCBLTCtrlReg = 0x65 ;
					}
					else
					{
						nCBLTCtrlReg = 0x69 ;
					}
				}
				break ;
			case WAVEMODE:
				nCBLTCtrlReg = 0xaa ;
				break ;
		}
		if(!g_MDCDBInfo.mqt_enable[nMQTIdx]) //for set disable mqt
		{
			nCBLTCtrlReg = 0xaa; // kejun
		}
		nMappedAddress = MDC_ADRS_WITH_GEO(nMQTPosition, MDC_MQT_FUNC_ADRS(MDC_MQT_CRCSR_SPACE) | MDC_MQT_FUNC_SUB_ADRS(0X7C)) ;
		VALUE_OF_A24D32(nMappedAddress) = nCBLTCtrlReg;
		DEBUG_OUTPUT(("MQT at slot %d, the mapped address: 0x%x, CBLT control register 3: 0x%x.\n",nMQTPosition, nMappedAddress, nCBLTCtrlReg )) ;
		if((nCBLTCtrlReg & 0x55) != (0x55 & VALUE_OF_A24D32(nMappedAddress)))
		{
			printf("setting CBLT control register failed at MQT slot#: %d\n", nMQTPosition) ;
			Push2ErrStack(PPCFATAL+MDC_FLG+ATOM_ERR+93,0,0,0,0);			/*mdc error 93*/
			
			return -1 ;
		}
		DEBUG_OUTPUT(("\n")) ;
	}
	return 0;
}
int MDCMQTSetTimeThresh(int nMQTIdx)/*10.set Time threshold*/
{
	int nTimeThreshold;
	UINT nMappedAddress;
	//for(int nMQTIdx =(g_MDCDBInfo.mqt_sum -1);  nMQTIdx >=0;  nMQTIdx --)	
	{
		int nMQTPosition = g_MDCDBInfo.mqt_header[nMQTIdx]->geo ;
		DEBUG_OUTPUT(("setup time low threshold register\n")) ;/*12. low timethreshold*/
		switch(g_nRunMode)
		{
			case NOMALMODE:
				nTimeThreshold = g_MDCDBInfo.mqt_header[nMQTIdx]->TimeThreshold_l ;
				break ;
			case BASELINE:
				nTimeThreshold = 0; 
				break;
			case PEDMODE:
			case MONITOR:
			case CALIBRATION:
				g_MDCDBInfo.mqt_header[nMQTIdx]->TimeThreshold_l  = 500;
				nTimeThreshold = g_MDCDBInfo.mqt_header[nMQTIdx]->TimeThreshold_l ;
				break ; 
			case WAVEMODE:
				nTimeThreshold = g_MDCDBInfo.mqt_header[nMQTIdx]->TimeThreshold_l ;
				break ;
				
		}
		nMappedAddress = MDC_ADRS_WITH_GEO(nMQTPosition, (MDC_MQT_FUNC_ADRS(MDC_MQT_DAC_ACCESS) | MDC_MQT_FUNC_SUB_ADRS(MDC_MQT_DAC_LOW))) ;
		DEBUG_OUTPUT(("MQT at slot %d, the mapped address: 0x%x, nTimeThreshold: %d.\n", nMQTPosition, nMappedAddress, nTimeThreshold)) ;
		VALUE_OF_A24D32(nMappedAddress) = nTimeThreshold;		
		DEBUG_OUTPUT(("\n")) ;
		DEBUG_OUTPUT(("setup time high threshold register\n")) ;/*13.high timethreshold*/
		switch(g_nRunMode)
		{
			case NOMALMODE:
				nTimeThreshold = g_MDCDBInfo.mqt_header[nMQTIdx]->TimeThreshold_h ;
				break ;
			case BASELINE:
				nTimeThreshold = 0;
				break;
			case PEDMODE:
			case MONITOR:
			case CALIBRATION:
				g_MDCDBInfo.mqt_header[nMQTIdx]->TimeThreshold_h = 500;
				nTimeThreshold = g_MDCDBInfo.mqt_header[nMQTIdx]->TimeThreshold_h ;
				break ;				
			case WAVEMODE:
				nTimeThreshold = g_MDCDBInfo.mqt_header[nMQTIdx]->TimeThreshold_h ;
				break ;

		}
		nMappedAddress = MDC_ADRS_WITH_GEO(nMQTPosition, (MDC_MQT_FUNC_ADRS(MDC_MQT_DAC_ACCESS) | MDC_MQT_FUNC_SUB_ADRS(MDC_MQT_DAC_HIGH))) ;
		DEBUG_OUTPUT(("MQT at slot %d, the mapped address: 0x%x, nTimeThreshold: %d.\n", nMQTPosition, nMappedAddress, nTimeThreshold)) ;
		VALUE_OF_A24D32(nMappedAddress) = nTimeThreshold;		
		DEBUG_OUTPUT(("\n")) ;
	}
	return 0;
}
int MDCMQTPreAmpVolDAC(int nMQTIdx)/*11.pre-amp voltage ,DAC*/
{
	int nPreAmpValue;
	//for(int nMQTIdx =(g_MDCDBInfo.mqt_sum -1);  nMQTIdx >=0;  nMQTIdx --)	
	{
		int nMQTPosition = g_MDCDBInfo.mqt_header[nMQTIdx]->geo ;
		DEBUG_OUTPUT(("setup pre-amp register\n")) ;
		switch(g_nRunMode)
		{
			case NOMALMODE:
				nPreAmpValue = g_MDCDBInfo.mqt_header[nMQTIdx]->DAC;	//should =0
				break ;
			case BASELINE:
				nPreAmpValue = g_MDCDBInfo.mqt_header[nMQTIdx]->DAC;	//should =0
				break;
			case PEDMODE:
			case MONITOR:
				nPreAmpValue = g_MDCDBInfo.mqt_header[nMQTIdx]->DAC;	
				break;
			case WAVEMODE:
				nPreAmpValue = g_MDCDBInfo.mqt_header[nMQTIdx]->DAC;	//should =0
				break;
			case CALIBRATION:
				nPreAmpValue =g_nMQTDAC;
				break ;
		}
		if((0 == nPreAmpValue )&&(CALIBRATION==g_nRunMode))
		{
			printf("WARNING! MQT(geo = %d): nPreAmpValue == 0 !\n",nMQTPosition) ;
			Push2ErrStack(PPCFATAL+MDC_FLG+ATOM_ERR+94,0,0,0,0);			/*mdc error 94*/
			
		}
		UINT nMappedAddress = MDC_ADRS_WITH_GEO(nMQTPosition, (MDC_MQT_FUNC_ADRS(MDC_MQT_DAC_ACCESS) | MDC_MQT_FUNC_SUB_ADRS(MDC_MQT_DAC_PREAMP)))  ;
		DEBUG_OUTPUT(("MQT at slot %d, the mapped address: 0x%x, pre-amp: %d.\n", nMQTPosition, nMappedAddress, nPreAmpValue)) ;
		VALUE_OF_A24D32(nMappedAddress)= nPreAmpValue;
		DEBUG_OUTPUT(("\n")) ;
	}
}
int MDCMQTSetWorkEnable(int nMQTIdx)/*12.set work enable*/
{
	int 		nDataWrite;
	UINT	nMappedAddress;
	//for(int nMQTIdx =(g_MDCDBInfo.mqt_sum -1);  nMQTIdx >=0;  nMQTIdx --)	
	{
		int nMQTPosition = g_MDCDBInfo.mqt_header[nMQTIdx]->geo ;
		if(g_nRunMode == WAVEMODE)
		{
			DEBUG_OUTPUT(("WorkDisable.\n")) ;
			nDataWrite= 0 ;
		}
		else if(g_MDCDBInfo.mqt_enable[nMQTIdx])
		{
			DEBUG_OUTPUT(("WorkEnable.\n")) ;
			nDataWrite= 1 ;
		}
		else
		{
			DEBUG_OUTPUT(("WorkDisable.\n")) ;
			nDataWrite= 0 ;
		}
		nMappedAddress = MDC_ADRS_WITH_GEO(nMQTPosition, (MDC_MQT_FUNC_ADRS(MDC_MQT_WORKENABLE) | MDC_MQT_FUNC_SUB_ADRS(0X0)))  ;
		VALUE_OF_A24D32(nMappedAddress)= nDataWrite;
		DEBUG_OUTPUT(("MQT at slot %d, the mapped address: 0x%x, nDataWrite = 0x%x.\n", nMQTPosition, nMappedAddress, nDataWrite)) ;
		DEBUG_OUTPUT(("\n")) ;
	}
	taskDelay(1) ;
	return 0;
}
int MDCMQTChkWorkEnReg(int nMQTIdx)/*13.check work enable register*/
{
	int nDataRead;
	int nCheckVal;
	UINT nMappedAddress;
	//for(int nMQTIdx =(g_MDCDBInfo.mqt_sum -1);  nMQTIdx >=0;  nMQTIdx --)	
	{
		int nMQTPosition = g_MDCDBInfo.mqt_header[nMQTIdx]->geo ;
		nDataRead = 0 ;
		nMappedAddress = MDC_ADRS_WITH_GEO(nMQTPosition, (MDC_MQT_FUNC_ADRS(MDC_MQT_STATUS) | MDC_MQT_FUNC_SUB_ADRS(0X0))) ;
		nDataRead = VALUE_OF_A24D32(nMappedAddress) ;
		DEBUG_OUTPUT(("MQT at slot %d, nMappedAddress: 0x%x, nDataRead = 0x%x.\n", nMQTPosition, nMappedAddress, nDataRead)) ;
		if(g_nRunMode == WAVEMODE)
		{
			DEBUG_OUTPUT(("WorkDisable.\n")) ;
			nCheckVal= 0 ;
		}
		else if(g_MDCDBInfo.mqt_enable[nMQTIdx])
		{
			DEBUG_OUTPUT(("WorkEnsable.\n")) ;
			nCheckVal= 1 ;
		}
		else
		{
			DEBUG_OUTPUT(("WorkDisable.\n")) ;
			nCheckVal= 0 ;
		}
		if(nCheckVal != ((0x4 & nDataRead)>>2))
		{
			printf("Not in work status, MQT Slot# = %d.\n", nMQTPosition) ;
			Push2ErrStack(PPCFATAL+MDC_FLG+ATOM_ERR+95,0,0,0,0);			/*mdc error 95*/
			
			return -1 ;
		}
	}
	return 0;
}


int MDCConfigROC()
{
	MDCROCReset((g_MDCDBInfo.roc_header->geo & 0xff)) ;
	for(int i = 0; i < 500; i ++) ;
	MDCROCSetN((g_MDCDBInfo.roc_header->geo & 0xff), g_nTrgPerInt) ;
	for(i = 0; i < 500; i ++) ;
	printf("g_nTrgPerInt=%d\n",g_nTrgPerInt);
	printf("g_MDCDBInfo.roc_header->geo=0x%x\n",g_MDCDBInfo.roc_header->geo);
	if(g_nTrgPerInt != (0xff & MDCROCReadN(g_MDCDBInfo.roc_header->geo & 0xff)))
	{
		DEBUG_OUTPUT(("set nTriger Per int,MDCROCSetN failed\n")) ;
		Push2ErrStack(PPCFATAL+MDC_FLG+ATOM_ERR+13,0,0,0,0);				/*mdc error 13*/
		return -1;
	}
	taskDelay(1) ;
	return 0;
}

int MDCMQTConfigAll()
{
	for(int nMQTIdx =(g_MDCDBInfo.mqt_sum -1);  nMQTIdx >=0;  nMQTIdx --)
	{
		MDCMQTResetMainBd(nMQTIdx);
		MDCMQTEnsureClockInput(nMQTIdx);
		MDCMQTHptdcInit(nMQTIdx);
		MDCMQTResetMainBd(nMQTIdx); /*reset once more*/
		MDCMQTSetupQL1latency(nMQTIdx);
		MDCMQTSubBdQSampWin(nMQTIdx);
		MDCMQTSubBdBaseLine(nMQTIdx);
		MDCMQTSubBdQthresh(nMQTIdx);
		MDCMQTSetCBLTReg(nMQTIdx);
		MDCMQTSetTimeThresh(nMQTIdx);
		MDCMQTPreAmpVolDAC(nMQTIdx);
		MDCMQTSetWorkEnable(nMQTIdx);
		MDCMQTChkWorkEnReg(nMQTIdx);
	}
	return 0;
}

int MDCConfigMCCLoad(int geo,UINT uT0Latency)
{
	printf("MCC slot:%d.\n", geo) ;
	printf("MDCMCCReset.\n") ;
	MDCMCCReset(geo) ;
	taskDelay(1);
	
	printf("Config MDCMCCCalib Frst.\n") ;
	MDCMCCCalib(geo) ;
	taskDelay(2);
			
	printf("MDCMCCCrystOn.\n") ;
	MDCMCCCrystOn(geo) ;
	taskDelay(4);
	printf("MDCMCCSetData, MCC T0 latency: %d.\n", uT0Latency) ;
	MDCMCCSetData(geo, uT0Latency) ;
	taskDelay(2);
	return 0;
}

int MDCConfigMCCSyn1(int geo,UINT uT0Latency)
{
	printf("MCC slot:%d.\n", geo) ;
	printf("MDCMCCReset.\n") ;
	MDCMCCReset(geo) ;
	taskDelay(1);
	MDCMCCCalib(geo) ;
	printf("MDCMCCCrystOn.\n") ;
	MDCMCCCrystOn(geo) ;
	taskDelay(4);
	printf("MDCMCCSetData, MCC T0 latency: %d.\n", uT0Latency) ;
	MDCMCCSetData(geo, uT0Latency) ;
	taskDelay(2);
	return 0;
}

int MDCConfigMCCSyn3(int geo,UINT uT0Latency)
{
	printf("enter MDCConfigMCCSyn3\n");
	printf("MCC slot:%d.\n", geo) ;
	printf("MDCMCCReset.\n") ;
	MDCMCCReset(geo) ;
	taskDelay(1);

	switch(g_nRunMode)
	{
		case NOMALMODE:
			if(g_MDCDBInfo.ppc_header->NormModeSelfL1)
			{
				printf(" MDCMCCCalib\n");
				MDCMCCCalib(geo) ;
			}
			else
			{
				printf(" MDCMCConline\n");
				MDCMCCOnline(geo) ;
			}

			taskDelay(1);
			break ;
		case PEDMODE:	
			printf("MDCMCCPED.\n") ;
			MDCMCCCalib(geo) ;
			taskDelay(1);
			break ;
		default:
			printf("MDCMCCCalib.\n") ;
			MDCMCCCalib(geo) ;
			taskDelay(2);
	}
	
	printf("MDCMCCCrystOn.\n") ;
	MDCMCCCrystOn(geo) ;
	taskDelay(4);
	printf("MDCMCCSetData, MCC T0 latency: %d.\n", uT0Latency) ;
	MDCMCCSetData(geo, uT0Latency) ;
	taskDelay(2);
	return 0;
}

void MDCConfigMTILoad(int geo)
{
	VALUE_OF_A24D32(MDC_ADRS_WITH_GEO(geo, MDC_MTI_RESET_ADRS)) = 0 ;
	VALUE_OF_A24D32(MDC_ADRS_WITH_GEO(geo, MDC_MTI_ENABLE_ADRS)) = 0 ;
}

int MDCSetMQTNewDAC(int nMQTPosition)
{
	UINT32 nMappedAddress = 0;
	UINT32 nDataWrite = 0 ;
	UINT32 nDataRead = 0 ;

	/*first send signal WorkEnable ,false*/
	nDataWrite= 0 ;
	nMappedAddress = MDC_ADRS_WITH_GEO(nMQTPosition, (MDC_MQT_FUNC_ADRS(MDC_MQT_WORKENABLE) | MDC_MQT_FUNC_SUB_ADRS(0X0)))  ;
	VALUE_OF_A24D32(nMappedAddress)= nDataWrite;
	taskDelay(1) ;


	/*set  pre-amp voltage, DAC*/
	if(0 == g_nMQTDAC)
	{
		printf("WARNING! MQT(geo = %d): nPreAmpValue == 0 !\n", nMQTPosition) ;
		Push2ErrStack(PPCFATAL+MDC_FLG+ATOM_ERR+96,0,0,0,0);			/*mdc error 96*/
	}
	nMappedAddress = MDC_ADRS_WITH_GEO(nMQTPosition, (MDC_MQT_FUNC_ADRS(MDC_MQT_DAC_ACCESS) | MDC_MQT_FUNC_SUB_ADRS(MDC_MQT_DAC_PREAMP)))  ;
	VALUE_OF_A24D32(nMappedAddress)= g_nMQTDAC;
	
	/*set signal WorkEnable*/
	nDataWrite= 1 ;
	nMappedAddress = MDC_ADRS_WITH_GEO(nMQTPosition, (MDC_MQT_FUNC_ADRS(MDC_MQT_WORKENABLE) | MDC_MQT_FUNC_SUB_ADRS(0X0)))  ;
	VALUE_OF_A24D32(nMappedAddress)= nDataWrite;
	taskDelay(1) ;

	/*check the status register*/
	nDataRead = 0 ;
	nMappedAddress = MDC_ADRS_WITH_GEO(nMQTPosition, (MDC_MQT_FUNC_ADRS(MDC_MQT_STATUS) | MDC_MQT_FUNC_SUB_ADRS(0X0))) ;
	nDataRead = VALUE_OF_A24D32(nMappedAddress) ;
	if(1 != ((0x4 & nDataRead)>>2))
	{
		printf("Not in work status, MQT Slot# = %d.\n", nMQTPosition) ;
		Push2ErrStack(PPCFATAL+MDC_FLG+ATOM_ERR+97,0,0,0,0);			/*mdc error 97*/
		return -1 ;
	}
	return 0;
}


int MDCClrMQTL1(int nMQTPosition)
{
	UINT32 nMappedAddress = 0;
	UINT32 nDataWrite = 0 ;
	UINT32 nDataRead = 0 ;
	/*first send signal WorkEnable ,false*/
	nDataWrite= 0 ;
	nMappedAddress = MDC_ADRS_WITH_GEO(nMQTPosition, (MDC_MQT_FUNC_ADRS(MDC_MQT_WORKENABLE) | MDC_MQT_FUNC_SUB_ADRS(0X0)))  ;
	VALUE_OF_A24D32(nMappedAddress)= nDataWrite;
	taskDelay(5) ;
	
	/*15. send signal WorkEnable*/
	nDataWrite= 1 ;
	nMappedAddress = MDC_ADRS_WITH_GEO(nMQTPosition, (MDC_MQT_FUNC_ADRS(MDC_MQT_WORKENABLE) | MDC_MQT_FUNC_SUB_ADRS(0X0)))  ;
	VALUE_OF_A24D32(nMappedAddress)= nDataWrite;
	taskDelay(5) ;

	/*After 15 . check the status register*/
	nDataRead = 0 ;
	nMappedAddress = MDC_ADRS_WITH_GEO(nMQTPosition, (MDC_MQT_FUNC_ADRS(MDC_MQT_STATUS) | MDC_MQT_FUNC_SUB_ADRS(0X0))) ;
	nDataRead = VALUE_OF_A24D32(nMappedAddress) ;
	if(1 != ((0x4 & nDataRead)>>2))
	{
		printf("Not in work status, MQT Slot# = %d.\n", nMQTPosition) ;
		Push2ErrStack(PPCFATAL+MDC_FLG+ATOM_ERR+98,0,0,0,0);			/*mdc error 98*/
		return -1 ;
	}
	return 0;
}


