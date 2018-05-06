#include "vxWorks.h"
#include "intLib.h"
#include "sockLib.h"
#include "taskLib.h"
#include "msgQLib.h"
#include "rebootLib.h"

#include "5100_test.h"

#include "fw_roppc.h"
#include "fw_dcgvas.h"
#include "fw_staterrspt.h"
#include "fw_filefunc.h"
#include "fw_tcpfunc.h"
#include "fw_runtasks.h"
#include "sd_trg_hw.h"


#include "fw_define.h"
#include "fragheaders.h"

int leicount=0;

void FWCbltTrans()
{
	char *	p_cLocalWrite ;
	UINT32 	nLocalWrite ;
	int 		nBytesCblted ;/*bytes actually transfered in a CBLT*/
	int 		nBytesToBePacked ;
	
	int 		nRetryTimes = 0 ;   /*for apply source*/

	int count2print=0;

	if(NULL == g_pfnFWEnteringCbltTrans)
	{
		DEBUG_OUTPUT(("ERROR: g_pfnFWEnteringCbltTrans is not linked to an instance.\n")) ;
		Push2ErrStack(PPCERROR+FRM_FLG+ATOM_ERR+39,0,0,0,0);		/*error 39*/
		nCbltTaskId=0;
		return;
	}

	if(NULL == g_pfnFWBeforeInterrupt)
	{
		DEBUG_OUTPUT(("ERROR: g_pfnFWBeforeInterrupt is not linked to an instance.\n")) ;
		Push2ErrStack(PPCERROR+FRM_FLG+ATOM_ERR+40,0,0,0,0);		/*error 40*/
		nCbltTaskId=0;
		return;
	}

			
	if(NULL == g_pfnFWBeforeDMARun)
	{
		DEBUG_OUTPUT(("ERROR: g_pfnFWBeforeDMARun is not linked to an instance.\n")) ;
		Push2ErrStack(PPCERROR+FRM_FLG+ATOM_ERR+41,0,0,0,0);		/*error 41*/
		nCbltTaskId=0;
		return;
	}
	
	if(NULL == g_pfnFWAfterDMARun)
	{
		DEBUG_OUTPUT(("ERROR: g_pfnFWAfterDMARun is not linked to an instance.\n")) ;
		Push2ErrStack(PPCERROR+FRM_FLG+ATOM_ERR+42,0,0,0,0);		/*error 42*/
		nCbltTaskId=0;
		return;
	}

	g_pfnFWEnteringCbltTrans(0) ;
	
	DEBUG_OUTPUT(("#######################\n"));
	gv_nRunningCtrl = RUNNING_NOW;
	 leicount=0;

	while(1)
	{

		nRetryTimes = 0 ;
		while(NULL == (p_cLocalWrite= rng_dmaApply(g_DmaRng,g_nMaxCbltBytes,DMA_METHOD)))
		{
			nRetryTimes ++ ;
			if( nRetryTimes >FW_MAX_RETRY) //5 sec wait
			{
				DEBUG_OUTPUT(("rng_dmaApply(g_DmaRng,g_nMaxCbltBytes) failed %d times.\n", nRetryTimes)) ;
				Push2ErrStack(PPCWARNING+FRM_FLG+ATOM_ERR+43,0,0,0,0);		/*error 43*/
				nRetryTimes=0;
			}
			taskDelay(1) ;
		}
		nRetryTimes = 0 ;
		nLocalWrite = (UINT32)p_cLocalWrite ;
		
		g_pfnFWBeforeInterrupt(0) ;
		
//		if(g_nTrgCounter==10000)	//for full test
//			taskDelay(1800);

		nRetryTimes = 0 ;
		while(ERROR == semTake(g_sidInterrupt, TIMEOUT_TICKS) ) 
		{
			nRetryTimes ++ ;
			if( nRetryTimes > FW_MAX_RETRY)
			{
				DEBUG_OUTPUT(("semTake(g_sidInterrupt, %d) times out %d times\n", TIMEOUT_TICKS, nRetryTimes)) ;// waiting for interrupt...
				if(g_nCrateID==160)
					trg_status();//jixl 20150615
				Push2ErrStack(PPCWARNING+FRM_FLG+ATOM_ERR+44,0,0,0,0);		/*error 44*/
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
		
		if(-1 == DMArun((UINT32)g_nCbltAdrs, (UINT32)(nLocalWrite + 2 * 4 ), g_nMaxCbltBytes))
		{
			DEBUG_OUTPUT(("DMArun failed.\n")) ;
			Push2ErrStack(PPCFATAL+FRM_FLG+ATOM_ERR+45,0,0,0,0);		/*error 45*/
		}
		nBytesCblted= cbltwait(g_nMaxCbltBytes) ;
		
		g_nBytesCblted  =nBytesCblted;//for ets\zdd

		//for crate 160, ETS module. see function TRGAfterDMARun in file trg_runtasks.cpp
		g_nETSWrite = (UINT32)(nLocalWrite+2*4+nBytesCblted);

		//for crate 192, ZDD 
		if(192==g_nCrateID)
		{
			g_nZDDCBLTed = (UINT32)(nLocalWrite+2*4+nBytesCblted);
			nZDDEventSize = nBytesCblted;
		}

		//printf("nBytesCblted=%d\n",nBytesCblted);
		
		if(nBytesCblted <= 0)
		{
			DEBUG_OUTPUT(("FWCbltTrans: nBytesCblted = %d\n", nBytesCblted)) ;
			Push2ErrStack(PPCERROR+FRM_FLG+ATOM_ERR+46,0,0,0,0);		/*error 46*/
			nBytesCblted=0;
		}
		else
		{
			
		}	
		g_pfnFWAfterDMARun (0) ;

		//taskDelay(100);

	
//		leicount++;
//		if(( (leicount%20)==0)&&(nBytesCblted<120))
/*		{
			printf("event:%d\n",leicount);
			printf("FWCbltTrans: nBytesCblted = %d\n", nBytesCblted);
			d( (UINT32)(nLocalWrite + 2 * 4 ), nBytesCblted/4, 4) ;
			taskDelay(180) ;
		}
*/

/***for full test
		leicount++;
		if((leicount%5000)==0)
			{
			printf("wait 1 second for full ... \n");
			taskDelay(60);
			}
		*/

		
		nBytesCblted = g_nBytesCblted;
		nBytesToBePacked = nBytesCblted + 2 * 4  ;
/*		
		if((nBytesToBePacked==4104)&&(192==g_nCrateID))
		{
			printf("ZDD length 4104! \n");
			d( (UINT32)(nLocalWrite + 2 * 4 ), nBytesCblted/4, 4) ;
			taskDelay(100) ;			
		}*/
			
		*((int*)nLocalWrite) = nBytesToBePacked;
		*((UINT32*)nLocalWrite + 1) = 0xffffffff ;

		if( nBytesToBePacked>g_nMaxCbltBytes )  //check Writen size,must<g_nMaxCbltBytes
		{
			DEBUG_OUTPUT(("ERR:nBytesToBePacked=%d > g_nMaxCbltBytes\n", nBytesToBePacked)) ;
			Push2ErrStack(PPCERROR+FRM_FLG+ATOM_ERR+47,0,0,0,0);		/*error 47*/
			break;
		}

		if(ERROR== rng_dmaWritten(g_DmaRng,nBytesToBePacked))
		{
			DEBUG_OUTPUT(("ERROR:  rng_dmaWritten(g_DmaRng... ) failed\n")) ;
			Push2ErrStack(PPCERROR+FRM_FLG+ATOM_ERR+47,0,0,0,0);		/*error 47*/
			break;
		}
		nRetryTimes = 0 ;
//		printf("before msgQSend.\n");
		while(msgQSend(g_msgQcblt,(char*)(&nBytesToBePacked),sizeof(unsigned),10,MSG_PRI_NORMAL)==ERROR)
		{
			nRetryTimes ++ ;
			if( nRetryTimes > 30 )
			{
				DEBUG_OUTPUT(("cblt Message Queue send fail\n")) ;
				Push2ErrStack(PPCWARNING+ATOM_ERR+48,0,0,0,0);		/*error 48*/
				nRetryTimes=0;
			}
//			perror("1");
		}
//		perror("2");
//		printf("after msgQSend.\n");
	}

FW_CBLT_EXITING:
	gv_nRunningCtrl = DPAC_STOP ;
	DEBUG_OUTPUT(("FWCbltTrans exited\n")) ;
	
	nCbltTaskId=0;//if exit normally,then not delete at exit 
}

void FWNetTrans()
{
	int nBytesToBeSent;
	int nRetryTimes ;
	char     * p_cNetRng;

	while(1)
	{
		nRetryTimes=0;
		while(msgQReceive(g_msgQnet,(char *)(&nBytesToBeSent),sizeof(int),TIMEOUT_TICKS)==ERROR)
		{
			nRetryTimes++;
			if( nRetryTimes >FW_MAX_RETRY)
			{
				DEBUG_OUTPUT(("netmsg receive failed %d times.\n", nRetryTimes)) ;
				Push2ErrStack(PPCWARNING+FRM_FLG+ATOM_ERR+49,0,0,0,0);		/*error 49*/
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
				Push2ErrStack(PPCWARNING+FRM_FLG+ATOM_ERR+50,0,0,0,0);		/*error 50*/
				nRetryTimes = 0 ;
			}
		}
		
	 	if (nBytesToBeSent !=SelectSend(g_sktData, p_cNetRng, nBytesToBeSent,g_DataTcpcb))
	 	{
			DEBUG_OUTPUT(("sending failed at g_sktData,exit NetTask\n")) ;
			Push2ErrStack(PPCERROR+FRM_FLG+ATOM_ERR+50,0,0,0,0);		//error 50
			break;
	 	}

/*	 	if (nBytesToBeSent !=Snd(g_sktData, p_cNetRng, nBytesToBeSent))
	 	{
			DEBUG_OUTPUT(("sending failed at g_sktData,exit NetTask\n")) ;
			Push2ErrStack(PPCERROR+FRM_FLG+ATOM_ERR+50,0,0,0,0);		//error 50
			break;
	 	}
		
*/
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
	g_fwRODHead.level1Id=g_nTrgCounter;
	
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
		Push2ErrStack(PPCERROR+FRM_FLG+ATOM_ERR+52,0,0,0,0);		/*error 52*/
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






int FWBlankPack(UINT *nDestination)
{
	int IndexIndex;
	register UINT *p_uDesti=nDestination;
	register UINT *p_PointEveHead;

	for(IndexIndex=0;IndexIndex<g_nTrgPerInt;IndexIndex++)
	{
		p_PointEveHead=p_uDesti;
		p_uDesti++;

		g_fwRODHead.level1Id=g_nTrgCounter;
		
		memcpy((char *)p_uDesti,&g_fwRODHead,sizeof(RODHeader));
		p_uDesti=p_uDesti+(sizeof(RODHeader)>>2);
		
		*p_uDesti=0;  //write status before tail
		p_uDesti++; 
		g_fwRODTail.numberOfStatusElements=1;//leigk071106
		g_fwRODTail.numberOfDataElements=0;
		memcpy((char *)p_uDesti,&g_fwRODTail,sizeof(RODTrailer));
		p_uDesti=p_uDesti+(sizeof(RODTrailer)>>2);
		*p_PointEveHead=13*4;
		g_nTrgCounter++;
	}
	return(g_nTrgPerInt*14*4);
}

int FWAbnormPack(UINT *nDestination ,UINT *nSource)
{
	int IndexIndex;
	register UINT *p_uDesti=nDestination;
	register UINT *p_uSource=nSource;
	register UINT *p_PointEveHead;
	UINT uDataLength = *nSource ;

	for(IndexIndex=0;IndexIndex<g_nTrgPerInt;IndexIndex++)
	{
		p_PointEveHead=p_uDesti;
		p_uDesti++;

		g_fwRODHead.level1Id=g_nTrgCounter;
		g_fwRODHead.detectorEventType =( g_fwRODHead.detectorEventType|0x2) ; //means this is an abnormal event d'10
		
		memcpy((char *)p_uDesti,&g_fwRODHead,sizeof(RODHeader));
		p_uDesti=p_uDesti+(sizeof(RODHeader)>>2);

		if (IndexIndex==0)
		{
			memcpy((char *)p_uDesti,(char *)p_uSource,uDataLength);
			p_uDesti=p_uDesti+(uDataLength>>2);
			*p_uDesti=0;  //write status before tail
			p_uDesti++; 
			g_fwRODTail.numberOfStatusElements=1;
			g_fwRODTail.numberOfDataElements=(uDataLength>>2);
			memcpy((char *)p_uDesti,&g_fwRODTail,sizeof(RODTrailer));
			p_uDesti=p_uDesti+(sizeof(RODTrailer)>>2);
			
			*p_PointEveHead=(13*4+uDataLength);			
		}
		else
		{
			*p_uDesti=0;  //write status before tail
			p_uDesti++; 
			g_fwRODTail.numberOfStatusElements=1;//leigk071106
			g_fwRODTail.numberOfDataElements=0;
			memcpy((char *)p_uDesti,&g_fwRODTail,sizeof(RODTrailer));
			p_uDesti=p_uDesti+(sizeof(RODTrailer)>>2);
			
			*p_PointEveHead=(13*4 );
		}
		g_nTrgCounter++;
	}
	
	g_fwRODHead.detectorEventType = (g_fwRODHead.detectorEventType&0xfffffffd); //means this is an abnormal event d'10
	
	return( (14*4+uDataLength)+(g_nTrgPerInt-1)*14*4 ); //  4+13*4+uDataLength+ (g_nTrgPerInt-1)*14*4
}
