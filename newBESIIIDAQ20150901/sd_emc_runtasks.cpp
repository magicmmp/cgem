#include "vxWorks.h"
#include "rng_dma.h"
#include "time.h"
#include "stdlib.h"
#include "taskLib.h"
#include "sysLib.h"
#include "usrLib.h"

#include "5100_test.h"

#include "fw_dcgvas.h"
#include "fw_define.h"
#include "fragheaders.h"
#include "fw_filefunc.h"
#include "fw_staterrspt.h"
#include "fw_runtasks.h"
#include "fw_tcpfunc.h"
#include "fw_configinfo.h"

#include "sd_emc_runtasks.h"

#include "sd_emc_hw.h"
#include "sd_emc.h"
#include "sd_emc_datapack.h"

#define EMC_MAX_RETRY 1000

//for monitor mode.
void EMCMoniOtheNetTransP1()
{
	int nBytesToBeSent;
	int nRetryTimes ;
	char     * p_cNetRng;

	while(!g_bEMCMonitor)
	{
		taskDelay(60);
	}
	
	g_bEMCMonitor = false;
	
	while(1)
	{
		nRetryTimes=0;
		while(msgQReceive(g_msgQnet,(char *)(&nBytesToBeSent),sizeof(int),TIMEOUT_TICKS)==ERROR)
		{
			nRetryTimes++;
			if( nRetryTimes >FW_MAX_RETRY)
			{
				DEBUG_OUTPUT(("netmsg receive failed %d times.\n", nRetryTimes)) ;
				Push2ErrStack(PPCWARNING+EMC_FLG+ATOM_ERR+73,0,0,0,0);		/*emc error 73*/
				nRetryTimes=0;
			}

			if(NET_STOP == gv_nRunningCtrl)
			{
				DEBUG_OUTPUT(("net task stop act\n"));

				goto FW_NET_EXITING;
			}
		}

		nRetryTimes = 0 ;
		while(NULL == (p_cNetRng = rng_dmaRead(g_NetRng,nBytesToBeSent,CPU_METHOD)))
		{
			nRetryTimes ++ ;
			if( nRetryTimes >FW_MAX_RETRY )
			{
				DEBUG_OUTPUT(("rng_dmaRead(g_NetRng...) failed %d times.\n", nRetryTimes)) ;
				Push2ErrStack(PPCWARNING+EMC_FLG+ATOM_ERR+74,0,0,0,0);		/*emc error 74*/
				nRetryTimes = 0 ;
			}
		}

//		d( (void *)(p_cNetRng+24), 1, 4) ;
	 	if (nBytesToBeSent !=SelectSend(g_sktData, p_cNetRng, nBytesToBeSent,g_DataTcpcb))
	 	{
			DEBUG_OUTPUT(("sending failed at g_sktData,exit NetTask\n")) ;
			Push2ErrStack(PPCERROR+EMC_FLG+ATOM_ERR+75,0,0,0,0);		/*emc error 75*/
			break;
	 	}
		
		g_nEventsSent = g_nEventsSent + g_nTrgPerInt; //leigkleigk060726

		if(ERROR == rng_dmaFree(g_NetRng,nBytesToBeSent))
		{
			DEBUG_OUTPUT(("rng_dmaFree(g_NetRng...) failed.\n")) ;
			break;
		}

 	}

FW_NET_EXITING:
	//leigkleigk060726 be sure of send only one special event
	nBytesToBeSent =14*4 ;
	char *p_cFakeData = (char*) malloc(nBytesToBeSent) ;
	char *p_cFakeDataLast=p_cFakeData;

	g_fwRODHead.startOfHeaderMarker=LAST_EVENT_RODMARKER;//special
	g_fwRODHead.level1Id=g_nL1Count;	//EMCMonitor
	
	*(UINT*)p_cFakeDataLast=13*4;
	p_cFakeDataLast=p_cFakeDataLast+4;
	memcpy((char *)p_cFakeDataLast,&g_fwRODHead,sizeof(RODHeader));
	p_cFakeDataLast=p_cFakeDataLast+sizeof(RODHeader);
	*(UINT *)p_cFakeDataLast=0;  	//write status before tail
	p_cFakeDataLast=p_cFakeDataLast+4;
	
	g_fwRODTail.numberOfStatusElements=1;//leigk071106
	g_fwRODTail.numberOfDataElements=0;
	memcpy((char *)p_cFakeDataLast,&g_fwRODTail,sizeof(RODTrailer));
	p_cFakeDataLast=p_cFakeDataLast+sizeof(RODTrailer);	
	if(nBytesToBeSent != SelectSend(g_sktData, p_cFakeData, nBytesToBeSent,g_DataTcpcb))
	{
		DEBUG_OUTPUT(("Sending the last data failed.\n")) ;
		Push2ErrStack(PPCERROR+EMC_FLG+ATOM_ERR+76,0,0,0,0);		/*emc error 76*/
	}
	free(p_cFakeData) ;
	
	Push2ErrStack(PPCINFO+ISTATUS,666,888,g_nInterruptCounter,g_nEventsSent) ;	//:) here after send last special event, just send indeed actual g_nEventsSent info
	
	DEBUG_OUTPUT(("g_nInterruptCounter = %d.\n", g_nInterruptCounter)) ; //from 1
	DEBUG_OUTPUT(("g_nTrgCounter = %d.\n", g_nTrgCounter)) ;//from 0;include the last invalid event
	DEBUG_OUTPUT(("g_nEventsSent = %d. \n", g_nEventsSent)) ;	//from 0;include the last invalid event
	semGive(g_sidBufEmpty) ;	
	DEBUG_OUTPUT(("FWNetTrans exited\n")) ;

	nNetTaskId=0;   //if have exit normal,not delete task at last
}

//for monitor mode. in data trans, calc data number.
void EMCMoniCrateNetTransP1()
{
	int nBytesToBeSent;
	int nRetryTimes ;
	char     * p_cNetRng;
	
	int L1Count = g_nL1Count;
	
	int iMoniNetCount = 0;		

	while(1)
	{
		nRetryTimes=0;
		while(msgQReceive(g_msgQnet,(char *)(&nBytesToBeSent),sizeof(int),TIMEOUT_TICKS)==ERROR)
		{
			nRetryTimes++;
			if( nRetryTimes >FW_MAX_RETRY)
			{
				DEBUG_OUTPUT(("netmsg receive failed %d times.\n", nRetryTimes)) ;
				Push2ErrStack(PPCWARNING+EMC_FLG+ATOM_ERR+73,0,0,0,0);		/*emc error 73*/
				nRetryTimes=0;
			}
			
			if(NET_STOP == gv_nRunningCtrl)
			{
				DEBUG_OUTPUT(("net task stop act\n"));
				goto FW_NET_EXITING;
			}
		}

		nRetryTimes = 0 ;
		while(NULL == (p_cNetRng = rng_dmaRead(g_NetRng,nBytesToBeSent,CPU_METHOD)))
		{
			nRetryTimes ++ ;
			if( nRetryTimes >FW_MAX_RETRY )
			{
				DEBUG_OUTPUT(("rng_dmaRead(g_NetRng...) failed %d times.\n", nRetryTimes)) ;
				Push2ErrStack(PPCWARNING+EMC_FLG+ATOM_ERR+74,0,0,0,0);		/*emc error 74*/
				nRetryTimes = 0 ;
			}
		}
		
		iMoniNetCount++;
		
		if(iMoniNetCount<=L1Count)
		{
//			d( (void *)(p_cNetRng+24), 1, 4) ;
		 	if (nBytesToBeSent !=SelectSend(g_sktData, p_cNetRng, nBytesToBeSent,g_DataTcpcb))
		 	{
				DEBUG_OUTPUT(("sending failed at g_sktData,exit NetTask\n")) ;
				Push2ErrStack(PPCERROR+EMC_FLG+ATOM_ERR+75,0,0,0,0);		/*emc error 75*/
				break;
		 	}
			
			g_nEventsSent = g_nEventsSent + g_nTrgPerInt; //leigkleigk060726
		}

		if(ERROR == rng_dmaFree(g_NetRng,nBytesToBeSent))
		{
			DEBUG_OUTPUT(("rng_dmaFree(g_NetRng...) failed.\n")) ;
			break;
		}

 	}

FW_NET_EXITING:
	//leigkleigk060726 be sure of send only one special event
	nBytesToBeSent =14*4 ;
	char *p_cFakeData = (char*) malloc(nBytesToBeSent) ;
	char *p_cFakeDataLast=p_cFakeData;

	g_fwRODHead.startOfHeaderMarker=LAST_EVENT_RODMARKER;//special
	g_fwRODHead.level1Id = L1Count;
	
	*(UINT*)p_cFakeDataLast=13*4;
	p_cFakeDataLast=p_cFakeDataLast+4;
	memcpy((char *)p_cFakeDataLast,&g_fwRODHead,sizeof(RODHeader));
	p_cFakeDataLast=p_cFakeDataLast+sizeof(RODHeader);
	*(UINT *)p_cFakeDataLast=0;  	//write status before tail
	p_cFakeDataLast=p_cFakeDataLast+4;
	
	g_fwRODTail.numberOfStatusElements=1;//leigk071106
	g_fwRODTail.numberOfDataElements=0;
	memcpy((char *)p_cFakeDataLast,&g_fwRODTail,sizeof(RODTrailer));
	p_cFakeDataLast=p_cFakeDataLast+sizeof(RODTrailer);	
	if(nBytesToBeSent != SelectSend(g_sktData, p_cFakeData, nBytesToBeSent,g_DataTcpcb))
	{
		DEBUG_OUTPUT(("Sending the last data failed.\n")) ;
		Push2ErrStack(PPCERROR+EMC_FLG+ATOM_ERR+76,0,0,0,0);		/*emc error 76*/
	}
	free(p_cFakeData) ;
	
	Push2ErrStack(PPCINFO+ISTATUS,666,888,g_nInterruptCounter,g_nEventsSent) ;	//:) here after send last special event, just send indeed actual g_nEventsSent info
	
	DEBUG_OUTPUT(("g_nInterruptCounter = %d.\n", g_nInterruptCounter)) ; //from 1
	DEBUG_OUTPUT(("g_nTrgCounter = %d.\n", g_nTrgCounter)) ;//from 0;include the last invalid event
	DEBUG_OUTPUT(("g_nEventsSent = %d. \n", g_nEventsSent)) ;	//from 0;include the last invalid event
	semGive(g_sidBufEmpty) ;	
	DEBUG_OUTPUT(("FWNetTrans exited\n")) ;

	nNetTaskId=0;   //if have exit normal,not delete task at last
}



void  EMCDataPack()
{
	char *p_cCbltRng = NULL ;
	char *p_cNetRng = NULL ;
	int nBytesToBePacked = 0;
	int nBytesToBeSent = 0 ;
	int nMaxNetBytes=0;
	int nRetryTimes = 0 ;

	int pritcnt=0;
	
	while(1)
	{
		nRetryTimes=0;
		while(msgQReceive(g_msgQcblt,(char *)(&nBytesToBePacked),sizeof(unsigned),TIMEOUT_TICKS)==ERROR)
		{
			nRetryTimes ++ ;
			if( nRetryTimes > EMC_MAX_RETRY)
			{
				DEBUG_OUTPUT(("msgQReceive(g_msgQcblt..., 1) failed %d times\n", nRetryTimes)) ;
				Push2ErrStack(PPCWARNING+EMC_FLG+ATOM_ERR+16,0,0,0,0);		/*emc error 16*/
				nRetryTimes = 0 ;
			}
			if(DPAC_STOP == gv_nRunningCtrl)
			{
				DEBUG_OUTPUT(("datapack task stop act\n"));
				goto EMC_DPAC_EXIT ;
			}
		}
		nRetryTimes=0;
		while(NULL == (p_cCbltRng = rng_dmaRead(g_DmaRng,nBytesToBePacked,CPU_METHOD)))
		{
			nRetryTimes ++ ;
			if(nRetryTimes > EMC_MAX_RETRY)
			{
				DEBUG_OUTPUT(("rng_dmaRead from cblt rng failed\n") ) ;
				Push2ErrStack(PPCWARNING+EMC_FLG+ATOM_ERR+17,0,0,0,0);		/*emc error 17*/
				nRetryTimes=0;
			}
			taskDelay(1) ;
		}
		nRetryTimes = 0 ;
		nMaxNetBytes=g_nTrgPerInt*512*(g_EMCDBInfo.emc_Q_sum);
		while(NULL == (p_cNetRng = rng_dmaApply(g_NetRng,nMaxNetBytes,CPU_METHOD)))
		{
			nRetryTimes ++ ;
			if(nRetryTimes > EMC_MAX_RETRY)
			{
				DEBUG_OUTPUT( ("rng_dmaApply from Net rng failed\n") );
				Push2ErrStack(PPCWARNING+EMC_FLG+ATOM_ERR+18,0,0,0,0);		/*emc error 18*/
				nRetryTimes = 0 ;
			}
			taskDelay(1) ;
		}

		//byjak
		if(1==g_nTCheckAmpMode)
		{
			nBytesToBeSent=FWAbnormPack((UINT*)p_cNetRng,(UINT*)p_cCbltRng);
		}
		else
		{
			if(EMCDataPackCore((UINT*)p_cNetRng, (UINT*)p_cCbltRng,&nBytesToBeSent)==-1)
			{
				//"data pack error\0");
				nBytesToBeSent=FWAbnormPack((UINT*)p_cNetRng,(UINT*)p_cCbltRng);
			}
		
		}

		
		if( nBytesToBeSent >nMaxNetBytes )  //check Writen size,must<nMaxNetBytes
		{
			DEBUG_OUTPUT(("ERR:nBytesToBeSent =%d > nMaxNetBytes\n", nBytesToBeSent )) ;
			Push2ErrStack(PPCERROR+EMC_FLG+ATOM_ERR+64,0,0,0,0);		/*emc error 64*/
			break;
		}

		if(ERROR == rng_dmaWritten(g_NetRng,nBytesToBeSent ))
		{
			DEBUG_OUTPUT(("ERROR:  rng_dmaWritten(g_NetRng... ) failed.\n")) ;
			Push2ErrStack(PPCERROR+EMC_FLG+ATOM_ERR+19,0,0,0,0);		/*emc error 19*/
			break;
		}

		if(ERROR == rng_dmaFree(g_DmaRng,nBytesToBePacked))
		{
			DEBUG_OUTPUT(("rng_dmaFree(g_DmaRng...) failed.\n")) ;
			Push2ErrStack(PPCERROR+EMC_FLG+ATOM_ERR+20,0,0,0,0);		/*emc error 20*/
			break;
		}
		
		nRetryTimes = 0 ;
		while(msgQSend(g_msgQnet,(char*)(&nBytesToBeSent),sizeof(int),10,MSG_PRI_NORMAL)==ERROR)
		{		
			nRetryTimes++;
			if(nRetryTimes>10)
			{
				DEBUG_OUTPUT(("msgQSend(g_msgQnet...) send fail!\n"));
				Push2ErrStack(PPCWARNING+EMC_FLG+ATOM_ERR+21,0,0,0,0);		/*emc error 21*/
				nRetryTimes=0;
			}
		}
	
	}
EMC_DPAC_EXIT:
	gv_nRunningCtrl= NET_STOP ;
	printf("end of the data pack proccess\n");

	nPackTaskId=0;//if exit normal then not delete at last
}


void  EMCMoniCrateDataPackP1()
{
	char *p_cCbltRng = NULL ;
	char *p_cNetRng = NULL ;
	int nBytesToBePacked = 0;
	int nBytesToBeSent = 0 ;
	int nMaxNetBytes=0;
	int nRetryTimes = 0 ;

	int pritcnt=0;
	
	while(1)
	{
		nRetryTimes=0;
		while(msgQReceive(g_msgQcblt,(char *)(&nBytesToBePacked),sizeof(unsigned),TIMEOUT_TICKS)==ERROR)
		{
			nRetryTimes ++ ;
			if( nRetryTimes > EMC_MAX_RETRY)
			{
				DEBUG_OUTPUT(("msgQReceive(g_msgQcblt..., 1) failed %d times\n", nRetryTimes)) ;
				Push2ErrStack(PPCWARNING+EMC_FLG+ATOM_ERR+16,0,0,0,0);		/*emc error 16*/
				nRetryTimes = 0 ;
			}
			if(DPAC_STOP == gv_nRunningCtrl)
			{
				DEBUG_OUTPUT(("datapack task stop act\n"));
				goto EMC_DPAC_EXIT ;
			}
		}
		nRetryTimes=0;
		while(NULL == (p_cCbltRng = rng_dmaRead(g_DmaRng,nBytesToBePacked,CPU_METHOD)))
		{
			nRetryTimes ++ ;
			if(nRetryTimes > EMC_MAX_RETRY)
			{
				DEBUG_OUTPUT(("rng_dmaRead from cblt rng failed\n") ) ;
				Push2ErrStack(PPCWARNING+EMC_FLG+ATOM_ERR+17,0,0,0,0);		/*emc error 17*/
				nRetryTimes=0;
			}
			taskDelay(1) ;
		}


		nRetryTimes = 0 ;
		nMaxNetBytes=g_nTrgPerInt*512*(g_EMCDBInfo.emc_Q_sum);
		while(NULL == (p_cNetRng = rng_dmaApply(g_NetRng,nMaxNetBytes,CPU_METHOD)))
		{
			nRetryTimes ++ ;
			if(nRetryTimes > EMC_MAX_RETRY)
			{
				DEBUG_OUTPUT( ("rng_dmaApply from Net rng failed\n") );
				Push2ErrStack(PPCWARNING+EMC_FLG+ATOM_ERR+18,0,0,0,0);		/*emc error 18*/
				nRetryTimes = 0 ;
			}
			taskDelay(1) ;
		}

		if(EMCMoniCrateDataPackCore((UINT*)p_cNetRng, (UINT*)p_cCbltRng,&nBytesToBeSent)==-1)
		{
			//"data pack error\0");
			nBytesToBeSent=FWAbnormPack((UINT*)p_cNetRng,(UINT*)p_cCbltRng);
		}
		

		if( nBytesToBeSent >nMaxNetBytes )  //check Writen size,must<nMaxNetBytes
		{
			DEBUG_OUTPUT(("ERR:nBytesToBeSent =%d > nMaxNetBytes\n", nBytesToBeSent )) ;
			Push2ErrStack(PPCERROR+EMC_FLG+ATOM_ERR+64,0,0,0,0);		/*emc error 64*/
			break;
		}

		if(ERROR == rng_dmaWritten(g_NetRng,nBytesToBeSent ))
		{
			DEBUG_OUTPUT(("ERROR:  rng_dmaWritten(g_NetRng... ) failed.\n")) ;
			Push2ErrStack(PPCERROR+EMC_FLG+ATOM_ERR+19,0,0,0,0);		/*emc error 19*/
			break;
		}


		if(ERROR == rng_dmaFree(g_DmaRng,nBytesToBePacked))
		{
			DEBUG_OUTPUT(("rng_dmaFree(g_DmaRng...) failed.\n")) ;
			Push2ErrStack(PPCERROR+EMC_FLG+ATOM_ERR+20,0,0,0,0);		/*emc error 20*/
			break;
		}

	
		nRetryTimes = 0 ;
		while(msgQSend(g_msgQnet,(char*)(&nBytesToBeSent),sizeof(int),10,MSG_PRI_NORMAL)==ERROR)
		{		
			nRetryTimes++;
			if(nRetryTimes>10)
			{
				DEBUG_OUTPUT(("msgQSend(g_msgQnet...) send fail!\n"));
				Push2ErrStack(PPCWARNING+EMC_FLG+ATOM_ERR+21,0,0,0,0);		/*emc error 21*/
				nRetryTimes=0;
			}
		}

	
	}
EMC_DPAC_EXIT:
	gv_nRunningCtrl= NET_STOP ;
	printf("end of the data pack proccess\n");

	nPackTaskId=0;//if exit normal then not delete at last
}


//for EMCmonitor. no CBLT trans,send blank pack only
void  EMCMoniOthDataPackP1()	
{
	char *p_cCbltRng = NULL ;
	char *p_cNetRng = NULL ;
	int nBytesToBePacked = 0;
	int nBytesToBeSent = 0 ;
	int nMaxNetBytes=0;
	int nRetryTimes = 0 ;

	int pritcnt=0;

	int L1Count = g_nL1Count;
	UINT iBlankCount;
		
	for(iBlankCount = 0;iBlankCount<L1Count;iBlankCount++)
	{
		nRetryTimes = 0 ;
		nMaxNetBytes=g_nTrgPerInt*512*(g_EMCDBInfo.emc_Q_sum);
		while(NULL == (p_cNetRng = rng_dmaApply(g_NetRng,nMaxNetBytes,CPU_METHOD)))
		{
			nRetryTimes ++ ;
			if(nRetryTimes > EMC_MAX_RETRY)
			{
				DEBUG_OUTPUT( ("rng_dmaApply from Net rng failed\n") );
				Push2ErrStack(PPCWARNING+EMC_FLG+ATOM_ERR+18,0,0,0,0);		/*emc error 18*/
				nRetryTimes = 0 ;
			}
			taskDelay(1) ;
		}

		nBytesToBeSent= EMCMoniBlankPackCore((UINT*)p_cNetRng,iBlankCount);

		
		if( nBytesToBeSent >nMaxNetBytes )  //check Writen size,must<nMaxNetBytes
		{
			DEBUG_OUTPUT(("ERR:nBytesToBeSent =%d > nMaxNetBytes\n", nBytesToBeSent )) ;
			Push2ErrStack(PPCERROR+EMC_FLG+ATOM_ERR+64,0,0,0,0);		/*emc error 64*/
			break;
		}

		if(ERROR == rng_dmaWritten(g_NetRng,nBytesToBeSent ))
		{
			DEBUG_OUTPUT(("ERROR:  rng_dmaWritten(g_NetRng... ) failed.\n")) ;
			Push2ErrStack(PPCERROR+EMC_FLG+ATOM_ERR+19,0,0,0,0);		/*emc error 19*/
			break;
		}

		if(ERROR == rng_dmaFree(g_DmaRng,nBytesToBePacked))
		{
			DEBUG_OUTPUT(("rng_dmaFree(g_DmaRng...) failed.\n")) ;
			Push2ErrStack(PPCERROR+EMC_FLG+ATOM_ERR+20,0,0,0,0);		/*emc error 20*/
			break;
		}
		
		nRetryTimes = 0 ;
		while(msgQSend(g_msgQnet,(char*)(&nBytesToBeSent),sizeof(int),10,MSG_PRI_NORMAL)==ERROR)
		{		
			nRetryTimes++;
			if(nRetryTimes>10)
			{
				DEBUG_OUTPUT(("msgQSend(g_msgQnet...) send fail!\n"));
				Push2ErrStack(PPCWARNING+EMC_FLG+ATOM_ERR+21,0,0,0,0);		/*emc error 21*/
				nRetryTimes=0;
			}
		}
	
	}
EMC_DPAC_EXIT:

	while(!g_bEMCMonictrl)
	{
		taskDelay(60);
	}
	g_bEMCMonictrl = false;	//for EMC Monitor	
	gv_nRunningCtrl= NET_STOP ;
	printf("gv_nRunningCtrl = %d\n",gv_nRunningCtrl);
	printf("end of the data pack proccess\n");

	nPackTaskId=0;//if exit normal then not delete at last
}

void EMCBaselinePack()
{
	char *	p_cDmaWrite ;
	UINT 	nDmaWriteAddr ;

	char *	p_cNetWrite ;
	UINT* 	p_uNetWriteAddr ;
	
	int 		nBytesCblted ; /*bytes actually transfered in a CBLT*/
	int 		nMaxBytesSend ;
	int 		nBytesToBeSend ;

	UINT	nQBaseAddr;
	UINT	nReadValue;
	UINT	nGeoTmp;
	
	int 		nRetryTimes;   /*for apply source*/

	EMCEnteringCbltTrans(0);//CBLT memsize

	nRetryTimes = 0 ;
	while(NULL == (p_cDmaWrite= rng_dmaApply(g_DmaRng,g_nMaxCbltBytes,DMA_METHOD)))
	{
		nRetryTimes ++ ;
		if( nRetryTimes> 300)
		{
			DEBUG_OUTPUT(("g_DmaRng apply failed %d times.\n", nRetryTimes)) ;
			Push2ErrStack(PPCWARNING+EMC_FLG+ATOM_ERR+43,0,0,0,0);		/*emc error 43*/
			nRetryTimes=0;
		}
		taskDelay(1) ;
	}
	nMaxBytesSend=sizeof(UINT32)+ sizeof(RODHeader)+ sizeof(UINT32) + sizeof(RODTrailer) + 
										(32 * sizeof(UINT)+sizeof(UINT)) *g_EMCDBInfo.emc_Q_sum;
	nRetryTimes = 0 ;
	while(NULL == (p_cNetWrite = rng_dmaApply(g_NetRng,nMaxBytesSend,CPU_METHOD)))
	{
		nRetryTimes ++ ;
		if(nRetryTimes > 300)
		{
			DEBUG_OUTPUT( ("rng_dmaApply from Net rng failed\n") );
			Push2ErrStack(PPCWARNING+EMC_FLG+ATOM_ERR+18,0,0,0,0);		/*emc error 18*/
			nRetryTimes=0;
		}
		taskDelay(1) ;
	}
	
	gv_nRunningCtrl = RUNNING_NOW;
	while(1)
	{
		nDmaWriteAddr = (UINT32)p_cDmaWrite ;
		
		EMCBeforeInterrupt(0) ;
				
		nRetryTimes = 0 ;
		
		while(ERROR == semTake(g_sidInterrupt, TIMEOUT_TICKS)) 
		{
			nRetryTimes ++ ;
			if( nRetryTimes >500)
			{
				DEBUG_OUTPUT(("semTake(g_sidInterrupt, %D) times out %d times\n", TIMEOUT_TICKS, nRetryTimes)) ;
				Push2ErrStack(PPCWARNING+EMC_FLG+ATOM_ERR+44,0,0,0,0);		/*emc error 44*/
				nRetryTimes=0;
			}
			if(CBLT_STOP ==  gv_nRunningCtrl)
			{
				DEBUG_OUTPUT(("CBLT task stop act\n"));
				goto EMCBase_EXITING;
			}
		}
		if(-1 == DMArun((UINT32)g_nCbltAdrs, (UINT32)(nDmaWriteAddr + 2 * 4 ), g_nMaxCbltBytes))
		{
			DEBUG_OUTPUT(("DMArun failed.\n")) ;
			Push2ErrStack(PPCFATAL+EMC_FLG+ATOM_ERR+45,0,0,0,0);		/*emc error 45*/
		}
		nBytesCblted= cbltwait(g_nMaxCbltBytes) ;

		if(nBytesCblted <= 0)
		{
			DEBUG_OUTPUT(("EMC Baseline: nBytesCblted = %d\n", nBytesCblted)) ;
			Push2ErrStack(PPCERROR+EMC_FLG+ATOM_ERR+46,0,0,0,0);		/*emc error 46*/
			nBytesCblted=0;
		}
		
		//begin read FADC data from Q module 
		p_uNetWriteAddr=(UINT *)p_cNetWrite;
		nBytesToBeSend=sizeof(UINT)+sizeof(RODHeader)+ sizeof(UINT) + sizeof(RODTrailer) + 
										32 * sizeof(UINT)*g_EMCDBInfo.emc_Q_sum;
		*p_uNetWriteAddr=nBytesToBeSend-4;//not include the size record--first uint
		p_uNetWriteAddr++;
		
		g_fwRODHead.level1Id=g_nTrgCounter;
		memcpy((char *)p_uNetWriteAddr,&g_fwRODHead,sizeof(RODHeader));
		p_uNetWriteAddr=p_uNetWriteAddr+(sizeof(RODHeader)>>2);
		
		for(int nModuIdx=0;nModuIdx<g_EMCDBInfo.emc_Q_sum;nModuIdx++)
		{
			nGeoTmp=g_EMCDBInfo.emc_Q_header[nModuIdx]->geo;
			nQBaseAddr=nGeoTmp*0x80000+0x180;
			for(int nChnIdx=0;nChnIdx<32;nChnIdx++)
			{
				ReadFrmVme32(nQBaseAddr, VA24, &nReadValue);
				*p_uNetWriteAddr=(nReadValue&0x7ffff)+(nGeoTmp<<26 )+(nChnIdx<<19);
				p_uNetWriteAddr++;
			}
		}

		*p_uNetWriteAddr=0;   /*status*/
		p_uNetWriteAddr++;

		g_fwRODTail.numberOfStatusElements=1;
		g_fwRODTail.numberOfDataElements=32 * g_EMCDBInfo.emc_Q_sum;
		memcpy((char *)p_uNetWriteAddr,&g_fwRODTail,sizeof(RODTrailer));
		p_uNetWriteAddr=p_uNetWriteAddr + (sizeof(RODTrailer)>>2);

		if (nBytesToBeSend !=SelectSend(g_sktData, p_cNetWrite, nBytesToBeSend,g_DataTcpcb))
	 	{
			DEBUG_OUTPUT(("sending failed at g_sktData,exit NetTask\n")) ;
			Push2ErrStack(PPCERROR+EMC_FLG+ATOM_ERR+65,0,0,0,0);		/*emc error 65*/
			return;
	 	}
		g_nTrgCounter++;
		g_nEventsSent++;
		
	}

EMCBase_EXITING:

	gv_nRunningCtrl=NET_STOP;

	nBytesToBeSend=14*4;
	g_fwRODHead.startOfHeaderMarker=LAST_EVENT_RODMARKER;//special
	g_fwRODHead.level1Id=g_nTrgCounter;
	p_uNetWriteAddr=(UINT *)p_cNetWrite;
	*p_uNetWriteAddr=13*4;
	p_uNetWriteAddr++;
	memcpy((char *)p_uNetWriteAddr ,&g_fwRODHead,sizeof(RODHeader));
	p_uNetWriteAddr=p_uNetWriteAddr+(sizeof(RODHeader)>>2);
	*p_uNetWriteAddr=0;  	//write status before tail
	p_uNetWriteAddr++;
	
	g_fwRODTail.numberOfStatusElements=1;//leigk071106
	g_fwRODTail.numberOfDataElements=0;
	memcpy((char *)p_uNetWriteAddr,&g_fwRODTail,sizeof(RODTrailer));
	p_uNetWriteAddr=p_uNetWriteAddr+(sizeof(RODTrailer)>>2);	
	if(nBytesToBeSend!= SelectSend(g_sktData, p_cNetWrite, nBytesToBeSend,g_DataTcpcb))
	{
		DEBUG_OUTPUT(("Sending the last data failed.\n")) ;
		Push2ErrStack(PPCERROR+EMC_FLG+ATOM_ERR+52,0,0,0,0);		/*emc error 52*/
	}
	
	DEBUG_OUTPUT(("g_nInterruptCounter = %d.\n", g_nInterruptCounter)) ;
	DEBUG_OUTPUT(("g_nTrgCounter = %d.\n", g_nTrgCounter)) ;
	DEBUG_OUTPUT(("g_nEventsSent = %d. \n", g_nEventsSent)) ;	
	semGive(g_sidBufEmpty) ;	
	DEBUG_OUTPUT(("EMC baseline exited\n")) ;
	
}

int EMCEnteringCbltTrans(int nArg)
{
	g_nMaxCbltBytes=g_nTrgPerInt*EMC_MAX_BYTES_PERMDL*(g_EMCDBInfo.emc_Q_sum);
	g_nCbltAdrs = 0x01000000 ;
	sysIntDisable(EMC_INT_LEVEL);

	//hi,jak

	if(1==g_nTCheckAmpMode)
	{
		g_nMaxTCBADMABytes=1*48*4;
		g_nTCBADMAAdrs = 0xd0d800 ;
		sysIntDisable(EMC_TCBA_INT_LEVEL);

		WriteToVme32( 0xD000f8,0xfa000000,1);//enable TCBA L1/Interrupt
		WriteToVme32( 0xD0008c,0xfa000000,1);//enable TCBA Interrupt
		
//		g_nEmcDAC=100;//temp
//		g_nEmcDAC=5000;
		g_nEmcDAC=15000;
		printf("set DAC=%d\n",g_nEmcDAC);
		WriteToVme16(0x0F30002, VA24, g_nEmcDAC);
		
		semTake(g_sidSATR, WAIT_FOREVER) ;
	}
	//bye,jak

	
	return 0 ;
}

int EMCBeforeInterrupt(int nArg)
{

	if(1==g_nTCheckAmpMode)
	{
		unsigned short nCaTmp;
		ReadFrmVme16(0x0F30000, VA24, &nCaTmp);
		while((nCaTmp&0xa) !=0)
		{
			taskDelay(1);
			ReadFrmVme16(0x0F30000, VA24, &nCaTmp);
		}
		WriteToVme16(0x0F30004, VA24, 0x300+1);	//byjak: let roc send L1
	}

	sysIntEnable(EMC_INT_LEVEL);
	return 0  ;
}


int EMCBeforeDMARun(int nArg)
{
	// taskDelay(60);
	return 0 ;
}

int EMCAfterDMARun(int nArg)
{
	return 0 ;
}


void EMCTCBADMARead()
{
	char *	p_cLocalWrite ;
	UINT32 	nLocalWrite ;
	int 		nBytesCblted ;/*bytes actually transfered in a CBLT*/
	int 		nBytesToBePacked ;
	
	int 		nRetryTimes = 0 ;   /*for apply source*/

	int count2print=0;

	if(NULL == g_pfnFWEnteringCbltTrans)
	{
		DEBUG_OUTPUT(("ERROR: g_pfnFWEnteringCbltTrans error.\n")) ;
		Push2ErrStack(PPCERROR+EMC_FLG+ATOM_ERR+77,0,0,0,0);		/*emc error 77*/
		nCbltTaskId=0;
		return;
	}

	if(NULL == g_pfnFWBeforeInterrupt)
	{
		DEBUG_OUTPUT(("ERROR: g_pfnFWBeforeInterrupt error.\n")) ;
		Push2ErrStack(PPCERROR+EMC_FLG+ATOM_ERR+78,0,0,0,0);		/*emc error 78*/
		nCbltTaskId=0;
		return;
	}

			
	if(NULL == g_pfnFWBeforeDMARun)
	{
		DEBUG_OUTPUT(("ERROR: g_pfnFWBeforeDMARun error.\n")) ;
		Push2ErrStack(PPCERROR+EMC_FLG+ATOM_ERR+79,0,0,0,0);		/*emc error 79*/
		nCbltTaskId=0;
		return;
	}
	
	if(NULL == g_pfnFWAfterDMARun)
	{
		DEBUG_OUTPUT(("ERROR: g_pfnFWAfterDMARun error.\n")) ;
		Push2ErrStack(PPCERROR+EMC_FLG+ATOM_ERR+80,0,0,0,0);		/*emc error 80*/
		nCbltTaskId=0;
		return;
	}

	g_pfnFWEnteringCbltTrans(0) ;

	
	DEBUG_OUTPUT(("#######################\n"));
	gv_nRunningCtrl = RUNNING_NOW;

	if(1)
	{
		//hi,jak
		unsigned short times_per_point=10;
		unsigned short nQmodule=g_EMCDBInfo.emc_Q_sum;

		unsigned short nCsrVal=0;
		unsigned short nChannelID = 0;
		unsigned short nAMPID = 0;
		unsigned short nGainValue = 0;
		unsigned short nTempGain[14]={0,31,63,95,127,145,149,153,155,159,163,191,223,255};

		unsigned short nModuInx=0;
		unsigned short nChInx=0;
		unsigned short nAmpInx=0;
		unsigned short nInx=0;
		unsigned short n=0;
		
		//init all amp to 0
		for(nModuInx=0;nModuInx<nQmodule;nModuInx++)//every module
		{
			for(nChInx=0;nChInx<32;nChInx++)	//every channel
			{
				
				if(g_EMCDBInfo.emc_Q_header[nModuInx]->MainAmpGeo[nChInx]!=0xffffffff)
				{
					nGainValue=0;
					ReadFrmVme16(0x0F30000, VA24, &nCsrVal);	//read register to control L1 send,protect to delay L1 send
					while((nCsrVal&0x4) !=0)
					{
						taskDelay(1);
						ReadFrmVme16(0x0F30000, VA24, &nCsrVal);	//read register to control L1 send,protect to delay L1 send
					}
					
					nAMPID= (g_EMCDBInfo.emc_Q_header[nModuInx]->MainAmpGeo[nChInx])>>16;
					nChannelID=(g_EMCDBInfo.emc_Q_header[nModuInx]->MainAmpGeo[nChInx])&0xffff;
					if( (nAMPID& 0x10)==0)//gain0
					{
						WriteToVme16(0x0F30006, VA24, ((nAMPID&0xf)<<12)+( (nChannelID&0xf)<<8)+(nGainValue&0xff) );	//gain0
						//printf("gain0, set Amp(nMainAmpGeo=%d,nChInx=%d)=%d\n",nMainAmpGeo,nChInx,nGainVal);
					}
					else//gain1
					{
						WriteToVme16(0x0F30008, VA24, ((nAMPID&0xf)<<12)+( (nChannelID&0xf)<<8)+(nGainValue&0xff) );	//gain1
						//printf("gain1, set Amp(nMainAmpGeo=%d,nChInx=%d)=%d\n",nMainAmpGeo,nChInx,nGainVal);
					}
					taskDelay(1);
				}
			}
		}

		for(nModuInx=0;nModuInx<nQmodule;nModuInx++)//every module
		{
			for(nChInx=0;nChInx<32;nChInx++)	//every channel
			{
				if(g_EMCDBInfo.emc_Q_header[nModuInx]->MainAmpGeo[nChInx]!=0xffffffff)
				{			
					nChannelID=(g_EMCDBInfo.emc_Q_header[nModuInx]->MainAmpGeo[nChInx])&0xffff;
				 	nAMPID= (g_EMCDBInfo.emc_Q_header[nModuInx]->MainAmpGeo[nChInx])>>16;
				 	nGainValue = g_EMCDBInfo.emc_Q_header[nModuInx]->GainValue[nChInx];
					printf("Q mod:%d,Q channel:%d,AMP ID:%d,AMP channel=%d,GAIN:%d\n",nModuInx,nChInx,nAMPID,nChannelID,nGainValue);

					WriteToVme32 (0xD00044,0xfa000000,((nModuInx+4)<<8)+nChInx);//set TCBA module/Channel info
					for(nAmpInx=0;nAmpInx<2;nAmpInx++)	//every amp
					//for(nAmpInx=0;nAmpInx<14;nAmpInx++)	//every amp
					{
						
						if(nAmpInx==0)
							nGainValue = 0;
						else
						 	nGainValue = g_EMCDBInfo.emc_Q_header[nModuInx]->GainValue[nChInx];
						
						//nGainValue = nTempGain[nAmpInx];
						ReadFrmVme16(0x0F30000, VA24, &nCsrVal);
						while((nCsrVal&0x4) !=0)
						{
							taskDelay(1);
							ReadFrmVme16(0x0F30000, VA24, &nCsrVal);	
						}


						if( (nAMPID& 0x10)==0)//gain0
						{
							WriteToVme16(0x0F30006, VA24, ((nAMPID&0xf)<<12)+( (nChannelID&0xf)<<8)+(nGainValue&0xff) );
//							printf("gain0:Q nModuInx = %d,Q nChInx = %d, set Amp(nAMPID=%d,nChannelID=%d)=%d\n",nModuInx,nChInx,nAMPID,nChannelID,nGainValue);
						}
						else//gain1
						{
							WriteToVme16(0x0F30008, VA24, ((nAMPID&0xf)<<12)+( (nChannelID&0xf)<<8)+(nGainValue&0xff) );
//							printf("gain1:Q nModuInx = %d,Q nChInx = %d, set Amp(nAMPID=%d,nChannelID=%d)=%d\n",nModuInx,nChInx,nAMPID,nChannelID,nGainValue);
						}

						ReadFrmVme16(0x0F30000, VA24, &nCsrVal);	
						while((nCsrVal&0x4) !=0)
						{
							taskDelay(1);
							ReadFrmVme16(0x0F30000, VA24, &nCsrVal);	
						}
						
						//set TCBA amp
						WriteToVme32 (0xD0003c,0xfa000000,nGainValue);
						taskDelay(1);
					
						for(nInx=0;nInx<times_per_point;nInx++)//every time
						{
							nRetryTimes = 0 ;
							while(NULL == (p_cLocalWrite= rng_dmaApply(g_DmaRng,g_nMaxCbltBytes,DMA_METHOD)))
							{
								nRetryTimes ++ ;
								if( nRetryTimes >FW_MAX_RETRY) //5 sec wait
								{
									DEBUG_OUTPUT(("rng_dmaApply(g_DmaRng,g_nMaxCbltBytes) failed %d times.\n", nRetryTimes)) ;
									Push2ErrStack(PPCWARNING+EMC_FLG+ATOM_ERR+81,0,0,0,0);		//emc error 81
									nRetryTimes=0;
								}
								taskDelay(1) ;
							}

							nRetryTimes = 0 ;
							nLocalWrite = (UINT32)p_cLocalWrite ;
							
							g_pfnFWBeforeInterrupt(0) ;
							
							nRetryTimes = 0 ;
							while(ERROR == semTake(g_sidInterrupt, TIMEOUT_TICKS) ) 
							{
								nRetryTimes ++ ;
								if( nRetryTimes > FW_MAX_RETRY)
								{
									DEBUG_OUTPUT(("semTake(g_sidInterrupt, %d) times out %d times\n", TIMEOUT_TICKS, nRetryTimes)) ;
									Push2ErrStack(PPCWARNING+EMC_FLG+ATOM_ERR+82,0,0,0,0);		//emc error 82
									nRetryTimes=0;
								}
								if(CBLT_STOP ==  gv_nRunningCtrl)
								{
									DEBUG_OUTPUT(("CBLT task stop act\n"));
									goto FW_CBLT_EXITING;
								}
							}

							nRetryTimes = 0 ;

							g_pfnFWBeforeDMARun (0) ;

							DMAinit(32,1,0x0f,0);
							if(-1 == DMArun((UINT32)g_nCbltAdrs, (UINT32)(nLocalWrite + 2 * 4 ), g_nMaxCbltBytes))
							{
								DEBUG_OUTPUT(("DMArun failed.\n")) ;
								Push2ErrStack(PPCFATAL+EMC_FLG+ATOM_ERR+83,0,0,0,0);		//emc error 83
							}
							nBytesCblted= cbltwait(g_nMaxCbltBytes) ;
							
							if(nBytesCblted <= 0)
							{
								DEBUG_OUTPUT(("FWCbltTrans: nBytesCblted = %d\n", nBytesCblted)) ;
								Push2ErrStack(PPCERROR+EMC_FLG+ATOM_ERR+84,0,0,0,0);		//emc error 84
								nBytesCblted=0;
							}
							
							g_pfnFWAfterDMARun (0) ;
							dmadisable();
								
							nBytesToBePacked = nBytesCblted + 2 * 4  ;
							*((int*)nLocalWrite) = nBytesToBePacked;
							*((UINT32*)nLocalWrite + 1) = 0xffffffff ;

							if( nBytesToBePacked>g_nMaxCbltBytes )  //check Writen size,must<g_nMaxCbltBytes
							{
								DEBUG_OUTPUT(("ERR:nBytesToBePacked=%d > g_nMaxCbltBytes\n", nBytesToBePacked)) ;
								Push2ErrStack(PPCERROR+EMC_FLG+ATOM_ERR+85,0,0,0,0);		//emc error 85
								break;
							}

							//hi,jak,DMA
							nRetryTimes = 0 ;
							while(NULL == (p_cLocalWrite= rng_dmaApply(g_DmaRng,g_nMaxTCBADMABytes,DMA_METHOD)))
							{
								nRetryTimes ++ ;
								if( nRetryTimes >FW_MAX_RETRY) //5 sec wait
								{
									DEBUG_OUTPUT(("rng_dmaApply(g_DmaRng,g_nMaxTCBADMABytes) failed %d times.\n", nRetryTimes)) ;
									Push2ErrStack(PPCWARNING+EMC_FLG+ATOM_ERR+86,0,0,0,0);		//emc error 86
									nRetryTimes=0;
								}
								taskDelay(1) ;
							}
							nRetryTimes = 0 ;
							nLocalWrite = (UINT32)p_cLocalWrite ;
			
							sysIntEnable(EMC_TCBA_INT_LEVEL);

							nRetryTimes = 0 ;
							while(ERROR == semTake(g_sidTCBAInterrupt, TIMEOUT_TICKS) ) 
							{
								nRetryTimes ++ ;
								if( nRetryTimes > FW_MAX_RETRY)
								{
									DEBUG_OUTPUT(("semTake(g_sidTCBAInterrupt, %d) times out %d times\n", TIMEOUT_TICKS, nRetryTimes)) ;
									Push2ErrStack(PPCWARNING+EMC_FLG+ATOM_ERR+87,0,0,0,0);		//emc error 87
									nRetryTimes=0;
								}
								if(CBLT_STOP ==  gv_nRunningCtrl)
								{
									DEBUG_OUTPUT(("CBLT task stop act\n"));
									goto FW_CBLT_EXITING;
								}
							}
							nRetryTimes = 0 ;

							g_pfnFWBeforeDMARun (0) ;
							DMAinit(32,1,0x3f,0);
							if(-1 == DMArun((UINT32)g_nTCBADMAAdrs, (UINT32)(nLocalWrite + 2 * 4 ), g_nMaxTCBADMABytes))
							{
								DEBUG_OUTPUT(("TCBA DMArun failed.\n")) ;
								Push2ErrStack(PPCFATAL+EMC_FLG+ATOM_ERR+88,0,0,0,0);		//emc error 88
							}
							
							nBytesCblted= dmawait(g_nMaxTCBADMABytes) ;
							
							if(nBytesCblted <= 0)
							{
								DEBUG_OUTPUT(("TCBA DMA FWCbltTrans: nBytesCblted = %d\n", nBytesCblted)) ;
								Push2ErrStack(PPCERROR+EMC_FLG+ATOM_ERR+89,0,0,0,0);		//emc error 89
								nBytesCblted=0;
							}

							g_pfnFWAfterDMARun (0) ;
							dmadisable();
								
							nBytesToBePacked = nBytesCblted + 2 * 4  ;
							*((int*)nLocalWrite) = nBytesToBePacked;

							*((UINT32*)nLocalWrite + 1) = 0x88880000+g_nCrateID;//byjak

							if( nBytesToBePacked>g_nMaxTCBADMABytes+2*4 )  //check Writen size,must<g_nMaxTCBADMABytes //byjak
							{
								DEBUG_OUTPUT(("ERR:nBytesToBePacked=%d > g_nMaxTCBADMABytes=%d\n", nBytesToBePacked,g_nMaxTCBADMABytes)) ;
								Push2ErrStack(PPCERROR+EMC_FLG+ATOM_ERR+90,0,0,0,0);		//emc error 90
								break;
							}

							if(ERROR== rng_dmaWritten(g_DmaRng,nBytesToBePacked))
							{
								DEBUG_OUTPUT(("ERROR:  rng_dmaWritten(g_DmaRng... ) failed\n")) ;
								Push2ErrStack(PPCERROR+EMC_FLG+ATOM_ERR+91,0,0,0,0);		//emc error 91
								break;
							}
							nRetryTimes = 0 ;
							while(msgQSend(g_msgQcblt,(char*)(&nBytesToBePacked),sizeof(unsigned),10,MSG_PRI_NORMAL)==ERROR)
							{
								nRetryTimes ++ ;
								if( nRetryTimes > 30 )
								{
									DEBUG_OUTPUT(("cblt Message Queue send fail\n")) ;
									Push2ErrStack(PPCWARNING+EMC_FLG+ATOM_ERR+92,0,0,0,0);		//emc error 92
									nRetryTimes=0;
								}
							}
						}

					}
				
					//set amp back
					nGainValue=0;
					
					ReadFrmVme16(0x0F30000, VA24, &nCsrVal);	//read register to control L1 send,protect to delay L1 send
					while((nCsrVal&0x4) !=0)
					{
						taskDelay(1);
						ReadFrmVme16(0x0F30000, VA24, &nCsrVal);	//read register to control L1 send,protect to delay L1 send
					}

					if( (nAMPID& 0x10)==0)//gain0
					{
						WriteToVme16(0x0F30006, VA24, ((nAMPID&0xf)<<12)+( (nChannelID&0xf)<<8)+(nGainValue&0xff) );	//gain0
						//printf("gain0, set Amp(nMainAmpGeo=%d,nChInx=%d)=%d\n",nMainAmpGeo,nChInx,nGainVal);
					}
					else//gain1
					{
						WriteToVme16(0x0F30008, VA24, ((nAMPID&0xf)<<12)+( (nChannelID&0xf)<<8)+(nGainValue&0xff) );	//gain1
						//printf("gain1, set Amp(nMainAmpGeo=%d,nChInx=%d)=%d\n",nMainAmpGeo,nChInx,nGainVal);
					}
					taskDelay(1);				

				}
			}
		}
	//bye,jak
	}

FW_CBLT_EXITING:

	gv_nRunningCtrl = DPAC_STOP ;
	DEBUG_OUTPUT(("FWCbltTrans exited\n")) ;
	
	nCbltTaskId=0;//if exit normally,then not delete at exit 
}


