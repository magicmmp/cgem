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

#include "sd_cgem.h"
#include "sd_cgem_runtasks.h"
#include "sd_cgem_datapack.h"
#include "sd_cgem_cfg.h"

#define MAX_RETRY 1000

void cgemDataPack()
{
	char *p_cCbltRng = NULL ;//cblt读出的原始数据
	char *p_cNetRng = NULL ; //准备发向网络的数据
	int nBytesToBePacked = 0;
	int nBytesToBeSent = 0 ;
	int nMaxNetBytes=0;
	int nRetryTimes = 0 ;

	while(1)
	{
		nRetryTimes=0;
		while(msgQReceive(g_msgQcblt,(char *)(&nBytesToBePacked),sizeof(unsigned),1)==ERROR)
		{
			nRetryTimes ++ ;
			if( !(nRetryTimes %MAX_RETRY))
			{
				printf("msgQReceive(g_msgQcblt..., 1) failed %d times\n", nRetryTimes) ;
				Push2ErrStack(PPCWARNING+MUC_FLG+ATOM_ERR+40,0,0,0,0);		/*muc error 40*/
				nRetryTimes = 0 ;

			}
			if(DPAC_STOP == gv_nRunningCtrl)
			{
				goto MUON_DPAC_EXITING ;
			}
		}
		nRetryTimes=0;
                printf("From g_msgQcblt, nBytesToBePacked=%d bytes.\n",nBytesToBePacked);
		while(NULL == (p_cCbltRng = rng_dmaRead(g_DmaRng,nBytesToBePacked,CPU_METHOD)))
		{
			nRetryTimes ++ ;
			if(nRetryTimes > MAX_RETRY)
				break ;
		}
		if(nRetryTimes > MAX_RETRY)
		{
			printf("rng_dmaRead from cblt rng failed\n") ;
			Push2ErrStack(PPCWARNING+MUC_FLG+ATOM_ERR+41,0,0,0,0);		/*muc error 41*/
			break ;
		}
		else
		{
			nRetryTimes = 0 ;
		}
		nMaxNetBytes=2048;
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
			Push2ErrStack(PPCWARNING+MUC_FLG+ATOM_ERR+42,0,0,0,0);		/*muc error 42*/
			break ;
		}
		else
		{
			nRetryTimes = 0 ;
		}

		if(cgemDataPackCore((UINT*)p_cNetRng, (UINT*)p_cCbltRng,&nBytesToBeSent)==-1)
		{
			nBytesToBeSent=FWAbnormPack((UINT*)p_cNetRng,(UINT*)p_cCbltRng);//blank data
			//data pack error\0");
			Push2ErrStack(PPCWARNING+MUC_FLG+ATOM_ERR+43,0,0,0,0);		/*muc error 43*/
		}
                 printf("After cgemDataPackCore , nBytesToBeSent=%d bytes.\n",nBytesToBeSent);
		if( nBytesToBeSent >nMaxNetBytes )  //check Writen size,must<nMaxNetBytes
		{
			DEBUG_OUTPUT(("ERR:nBytesToBeSent =%d > nMaxNetBytes\n", nBytesToBeSent )) ;
			Push2ErrStack(PPCWARNING+MUC_FLG+ATOM_ERR+44,0,0,0,0);		/*muc error 44*/
			break;
		}

		if(ERROR == rng_dmaWritten(g_NetRng,nBytesToBeSent))
		{
			//Written(g_NetRng...) error\0");
			Push2ErrStack(PPCWARNING+MUC_FLG+ATOM_ERR+45,0,0,0,0);		/*muc error 45*/
			break ;
		}

		if(ERROR == rng_dmaFree(g_DmaRng,nBytesToBePacked))//读之后的rng设置
		{
			//free rng (g_DmaRng...) failed\0");
			Push2ErrStack(PPCWARNING+MUC_FLG+ATOM_ERR+46,0,0,0,0);		/*muc error 46*/
			break ;
		}
		nRetryTimes = 0 ;
		while(msgQSend(g_msgQnet,(char*)(&nBytesToBeSent),sizeof(int),10,MSG_PRI_NORMAL)==ERROR)
		{
			nRetryTimes++;
			if(nRetryTimes>10)
			{
				DEBUG_OUTPUT(("msgQSend(g_msgQnet...) send fail!\n"));
				Push2ErrStack(PPCWARNING+MUC_FLG+ATOM_ERR+47,0,0,0,0);		/*muc error 47*/
				nRetryTimes=0;
			}
		}

	}
MUON_DPAC_EXITING:
	gv_nRunningCtrl= NET_STOP ;
	printf("end of the data pack proccess\n");

	nPackTaskId=0;//if exit normal then not delete at last
}


int cgemEnteringCbltTrans(int nArg)
{
	g_nMaxCbltBytes=2048;
	g_nCbltAdrs = 0x00000000 ;
	return 0 ;
}

int cgemBeforeInterrupt(int nArg)
{
	return 0  ;
}


int cgemBeforeDMARun(int nArg)
{

	return 0 ;
}

int cgemAfterDMARun(int nArg)
{
	return 0 ;
}

