#include "5100_test.h"
#include "vxWorks.h"
#include "rng_dma.h"
#include "intLib.h"
#include "sysLib.h"
#include "sockLib.h"
#include "taskLib.h"
#include "msgQLib.h"

#include "fw_dcgvas.h"
#include "fw_define.h"
#include "fragheaders.h"
#include "fw_runtasks.h"
#include "fw_filefunc.h"
#include "fw_staterrspt.h"

#include "sd_etf.h"
#include "sd_etf_hw.h"
#include "sd_etf_runtasks.h"
#include "sd_etf_actions.h"
#include "sd_etf_datapack.h"


#define ETF_MAX_RETRY 1000


void ETFDataPack()
{
	char *p_cCbltRng = NULL ;
	char *p_cNetRng = NULL ;
	int nBytesToBePacked = 0;
	int nBytesToBeSent = 0 ;
	int nMaxNetBytes=0;
	int nRetryTimes = 0 ;
	
	while(1)
	{
		//DEBUG_OUTPUT(("DataPack:DataPack is waiting at g_msgQcblt\n")) ;
		nRetryTimes=0;
		while(msgQReceive(g_msgQcblt,(char *)(&nBytesToBePacked),sizeof(unsigned),1)==ERROR)
		{
			nRetryTimes ++ ;
			if( !(nRetryTimes %ETF_MAX_RETRY))
			{
				printf("msgQReceive(g_msgQcblt..., 1) failed %d times\n", nRetryTimes) ;
				nRetryTimes = 0 ;
				Push2ErrStack(PPCWARNING+ETF_FLG+ATOM_ERR+86,0,0,0,0);		/*etf error 86*/		

			}
			if(DPAC_STOP == gv_nRunningCtrl)
			{
				goto ETF_DPAC_EXITING ;
			}
		}
		//if(DPAC_STOP == gv_nRunningCtrl)
		//{
		//	break ;
		//}
		nRetryTimes=0;
		while(NULL == (p_cCbltRng = rng_dmaRead(g_DmaRng,nBytesToBePacked,CPU_METHOD)))
		{
			nRetryTimes ++ ;
			if(nRetryTimes > ETF_MAX_RETRY)
				break ;
		}
		if(nRetryTimes > ETF_MAX_RETRY)
		{
			printf("rng_dmaRead from cblt rng failed\n") ;
			//rng_dmaRead from cblt rng failed\0");
			Push2ErrStack(PPCERROR+ETF_FLG+ATOM_ERR+87,0,0,0,0);		/*etf error 87*/		
			break ;
		}
		else
		{
			nRetryTimes = 0 ;
		}
		nMaxNetBytes=g_nTrgPerInt*ETF_MAX_BYTES_PERMDL*(g_ETFDBInfo.etf_tdig_sum);
		while(NULL == (p_cNetRng = rng_dmaApply(g_NetRng,nMaxNetBytes,CPU_METHOD)))
		{
			nRetryTimes ++ ;
			taskDelay(1) ;
			if(nRetryTimes > ETF_MAX_RETRY)
				break ;
		}
		if(nRetryTimes > ETF_MAX_RETRY)
		{
			//get the netrng buff  failed\0");
			Push2ErrStack(PPCERROR+ETF_FLG+ATOM_ERR+88,0,0,0,0);		/*etf error 88*/		
			break ;
		}
		else
		{
			nRetryTimes = 0 ;
		}
		
		if(ETFDataPackCore((UINT*)p_cNetRng, (UINT*)p_cCbltRng,&nBytesToBeSent)==-1)
		{
			nBytesToBeSent=FWAbnormPack((UINT*)p_cNetRng,(UINT*)p_cCbltRng);//blank data
			//data pack error\0");
			Push2ErrStack(PPCERROR+ETF_FLG+ATOM_ERR+89,0,0,0,0);		/*etf error 89*/		
		}

		if( nBytesToBeSent >nMaxNetBytes )  //check Writen size,must<nMaxNetBytes
		{
			DEBUG_OUTPUT(("ERR:nBytesToBeSent =%d > nMaxNetBytes\n", nBytesToBeSent )) ;
			Push2ErrStack(PPCERROR+ETF_FLG+ATOM_ERR+90,0,0,0,0);		/*etf error 90*/			
			break;
		}
	
		if(ERROR == rng_dmaWritten(g_NetRng,nBytesToBeSent))
		{
			//Written(g_NetRng...) error\0");
			Push2ErrStack(PPCERROR+ETF_FLG+ATOM_ERR+91,0,0,0,0);		/*etf error 91*/	
			break ;
		}
		
		if(ERROR == rng_dmaFree(g_DmaRng,nBytesToBePacked))
		{
			//free rng (g_DmaRng...) failed\0");
			Push2ErrStack(PPCERROR+ETF_FLG+ATOM_ERR+92,0,0,0,0);		/*etf error 92*/	
			break ;
		}
		nRetryTimes = 0 ;		
		while(msgQSend(g_msgQnet,(char*)(&nBytesToBeSent),sizeof(int),10,MSG_PRI_NORMAL)==ERROR)
		{	
			nRetryTimes++;
			if(nRetryTimes>10)
			{
				DEBUG_OUTPUT(("msgQSend(g_msgQnet...) send fail!\n"));
				Push2ErrStack(PPCWARNING+ETF_FLG+ATOM_ERR+93,0,0,0,0);		/*etf error 93*/	
				nRetryTimes=0;
			}
		}
		
		
	}
ETF_DPAC_EXITING:
	gv_nRunningCtrl= NET_STOP ;

	printf("end of the data pack proccess\n");
	nPackTaskId=0;
}


int ETFEnteringCbltTrans(int nArg)
{
	g_nMaxCbltBytes=g_nTrgPerInt*ETF_MAX_BYTES_PERMDL*(g_uEtfCbltModuleSum);
	g_nCbltAdrs = 0x02000000 ;
	return 0 ;
}

int ETFBeforeInterrupt(int nArg)
{
	return 0  ;
}


int ETFBeforeDMARun(int nArg)
{
	return 0 ;
}

int ETFAfterDMARun(int nArg)
{
	if ( -1 == MCSTwrite(MCST_ADRS, 32, 0x1) ) 
	{ /*clear purged */
		printf("\n mcst Write fail");
		Push2ErrStack(PPCFATAL+ETF_FLG+ATOM_ERR+94,0,0,0,0);		/*etf error 94*/	
		return -1;
	} 
	return 0 ;
}

