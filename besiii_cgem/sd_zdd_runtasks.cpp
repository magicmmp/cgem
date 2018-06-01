#include "vxWorks.h"
#include "rng_dma.h"
#include "intLib.h"
#include "sysLib.h"
#include "sockLib.h"
#include "taskLib.h"
#include "msgQLib.h"
#include "5100_test.h"

#include "fragheaders.h"
#include "fw_filefunc.h"
#include "fw_runtasks.h"
#include "fw_dcgvas.h"
#include "fw_define.h"
#include "fw_staterrspt.h"

#include "sd_zdd.h"
#include "sd_zdd_runtasks.h"
#include "sd_zdd_hw.h"
#include "sd_zdd_datapack.h"

#define MAX_RETRY 1000

void ZDDDataPack()
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
			if( !(nRetryTimes %MAX_RETRY))
			{
				printf("msgQReceive(g_msgQcblt..., 1) failed %d times\n", nRetryTimes) ;
				Push2ErrStack(PPCWARNING+ZDD_FLG+ATOM_ERR+80,0,0,0,0);		/*zdd error 80*/	
				nRetryTimes = 0 ;

			}
			if(DPAC_STOP == gv_nRunningCtrl)
			{
				goto ZDD_DPAC_EXITING ;
			}
		}
		nRetryTimes=0;
		while(NULL == (p_cCbltRng = rng_dmaRead(g_DmaRng,nBytesToBePacked,CPU_METHOD)))
		{
			nRetryTimes ++ ;
			if(nRetryTimes > MAX_RETRY)
				break ;
		}
		if(nRetryTimes > MAX_RETRY)
		{
			printf("rng_dmaRead from cblt rng failed\n") ;
			Push2ErrStack(PPCWARNING+ZDD_FLG+ATOM_ERR+81,0,0,0,0);		/*zdd error 81*/	
			break ;
		}
		else
		{
			nRetryTimes = 0 ;
		}
		nMaxNetBytes=g_nTrgPerInt*ZDD_MAX_BYTES_PERMDL*(g_ZDDDBInfo.v1721_sum)*10;//for blank data pack
		while(NULL == (p_cNetRng = rng_dmaApply(g_NetRng,nMaxNetBytes,CPU_METHOD)))
		{
			nRetryTimes ++ ;
			taskDelay(1) ;
			if(nRetryTimes > MAX_RETRY)
				break ;
		}
		if(nRetryTimes > MAX_RETRY)
		{
			//get the netrng buff  failed\0");
			Push2ErrStack(PPCWARNING+ZDD_FLG+ATOM_ERR+82,0,0,0,0);		/*zdd error 82*/	
			break ;
		}
		else
		{
			nRetryTimes = 0 ;
		}
		
		if(ZDDDataPackCore((UINT*)p_cNetRng, (UINT*)p_cCbltRng,&nBytesToBeSent)==-1)
		{
			//UINT dataLen = *p_cCbltRng;
			//printf("err dataLen = %d\n",dataLen);
			//d( (UINT32)(p_cCbltRng + 2 * 4 ), dataLen/4, 4) ;
			nBytesToBeSent=FWAbnormPack((UINT*)p_cNetRng,(UINT*)p_cCbltRng);//blank data
			//data pack error\0");
			Push2ErrStack(PPCWARNING+ZDD_FLG+ATOM_ERR+83,0,0,0,0);		/*zdd error 83*/	
		}

		if( nBytesToBeSent >nMaxNetBytes )  //check Writen size,must<nMaxNetBytes
		{
			DEBUG_OUTPUT(("ERR:nBytesToBeSent =%d > nMaxNetBytes\n", nBytesToBeSent )) ;
			Push2ErrStack(PPCWARNING+ZDD_FLG+ATOM_ERR+84,0,0,0,0);		/*zdd error 84*/	
			break;
		}
		
		if(ERROR == rng_dmaWritten(g_NetRng,nBytesToBeSent))
		{
			//Written(g_NetRng...) error\0");
			Push2ErrStack(PPCWARNING+ZDD_FLG+ATOM_ERR+85,0,0,0,0);		/*zdd error 85*/	
			break ;
		}
		
		if(ERROR == rng_dmaFree(g_DmaRng,nBytesToBePacked))
		{
			//free rng (g_DmaRng...) failed\0");
			Push2ErrStack(PPCWARNING+ZDD_FLG+ATOM_ERR+86,0,0,0,0);		/*zdd error 86*/	
			break ;
		}
		nRetryTimes = 0 ;
		while(msgQSend(g_msgQnet,(char*)(&nBytesToBeSent),sizeof(int),10,MSG_PRI_NORMAL)==ERROR)
		{	
			nRetryTimes++;
			if(nRetryTimes>10)
			{
				DEBUG_OUTPUT(("msgQSend(g_msgQnet...) send fail!\n"));
				Push2ErrStack(PPCWARNING+ZDD_FLG+ATOM_ERR+87,0,0,0,0);		/*zdd error 87*/	
				nRetryTimes=0;
			}
		}		
	
	}
ZDD_DPAC_EXITING:
	gv_nRunningCtrl= NET_STOP ;
	printf("end of the data pack proccess\n");

	nPackTaskId=0;//if exit normal then not delete at last
}


int ZDDEnteringCbltTrans(int nArg)
{
	g_nMaxCbltBytes=g_nTrgPerInt*ZDD_MAX_BYTES_PERMDL*(g_ZDDDBInfo.v1721_sum);
	g_nCbltAdrs = 0xaa000000 ;

	if(g_sidZddSpecial!=NULL)
	{
		if(-1 == semDelete(g_sidZddSpecial) )
		{
			DEBUG_OUTPUT(("semDelete(g_sidZddSpecial)  failed.\n")) ;
		}
		g_sidZddSpecial=NULL;
	}
	
	g_sidZddSpecial=semBCreate(SEM_Q_FIFO, SEM_FULL) ;
	if(NULL == g_sidZddSpecial )
	{
		printf("g_sidZddSpecial semBCreate failed\n") ;
	}
	sysIntEnable(ZDD_INT_LEVEL);
	return 0 ;
}

int ZDDBeforeInterrupt(int nArg)
{
	return 0  ;
}


int ZDDBeforeDMARun(int nArg)
{
	semTake(g_sidZddSpecial,WAIT_FOREVER);
	return 0 ;
}

int ZDDAfterDMARun(int nArg)
{
	//for ZDD more than once CBLT
	UINT nBytesCblted = 0;

	nBytesCblted = nZDDEventSize;

	//if(nBytesCblted>=4096)
	//	printf("nBytesCblted = %d\n",nBytesCblted);
	
	while(4096 == nBytesCblted )
	{

		if(-1 == DMArun((UINT32)g_nCbltAdrs, (UINT32)(g_nZDDCBLTed), g_nMaxCbltBytes))
		{
			DEBUG_OUTPUT(("DMArun failed.\n")) ;
			Push2ErrStack(PPCFATAL+ZDD_FLG+ATOM_ERR+88,0,0,0,0);		/*error 88*/
		}
		nBytesCblted = cbltwait(g_nMaxCbltBytes) ;	
		//if (nBytesCblted==0)
		//	printf("nBytesCblted==0 in ZDDAfterDMARun\n");
		g_nBytesCblted += nBytesCblted ;	
		g_nZDDCBLTed = (UINT32)(g_nZDDCBLTed+nBytesCblted);
		//if(g_nTrgCounter==0)
		//	printf("g_nBytesCblted = %d \n",g_nBytesCblted);

	}

	V1721_configVMEControl(0);/*re-enable interrupt in ROAK mode*/
	//printf("enable interrupt again\n");
	
	semGive(g_sidZddSpecial);
	return 0 ;
}

