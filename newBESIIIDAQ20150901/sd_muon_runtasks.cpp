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

#include "sd_muon.h"
#include "sd_muon_runtasks.h"
#include "sd_muon_hw.h"
#include "sd_muon_datapack.h"

#define MAX_RETRY 1000

void MUONDataPack()
{
	char *p_cCbltRng = NULL ;
	char *p_cNetRng = NULL ;
	int nBytesToBePacked = 0;
	int nBytesToBeSent = 0 ;
	int nMaxNetBytes=0;
	int nRetryTimes = 0 ;
	
	b_TrgErrFst = false;
	b_ChkErrFst = false;
	
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
				Push2ErrStack(PPCWARNING+MUC_FLG+ATOM_ERR+40,0,0,0,0);		/*muc error 40*/	
				nRetryTimes = 0 ;

			}
			if(DPAC_STOP == gv_nRunningCtrl)
			{
				goto MUON_DPAC_EXITING ;
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
			Push2ErrStack(PPCWARNING+MUC_FLG+ATOM_ERR+41,0,0,0,0);		/*muc error 41*/	
			break ;
		}
		else
		{
			nRetryTimes = 0 ;
		}
		nMaxNetBytes=g_nTrgPerInt*512*(g_MUONDBInfo.enabl_romo_sum);
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
		
		if(MUONDataPackCore((UINT*)p_cNetRng, (UINT*)p_cCbltRng,&nBytesToBeSent)==-1)
		{
			nBytesToBeSent=FWAbnormPack((UINT*)p_cNetRng,(UINT*)p_cCbltRng);//blank data
			//data pack error\0");
			Push2ErrStack(PPCWARNING+MUC_FLG+ATOM_ERR+43,0,0,0,0);		/*muc error 43*/	
		}

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
		
		if(ERROR == rng_dmaFree(g_DmaRng,nBytesToBePacked))
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


int MUONEnteringCbltTrans(int nArg)
{
	g_nMaxCbltBytes=g_nTrgPerInt*MUON_MAX_BYTES_PERMDL*(g_MUONDBInfo.enabl_romo_sum);
	g_nCbltAdrs = 0x04000000 ;

	if(g_sidMuonSpecial!=NULL)
	{
		if(-1 == semDelete(g_sidMuonSpecial) )
		{
			DEBUG_OUTPUT(("semDelete(g_sidMuonSpecial)  failed.\n")) ;
		}
		g_sidMuonSpecial=NULL;
	}
	
	g_sidMuonSpecial=semBCreate(SEM_Q_FIFO, SEM_FULL) ;/*leigkleigk 060820*/
	if(NULL == g_sidMuonSpecial )
	{
		printf("g_sidMuonSpecial semBCreate failed\n") ;
	}
	sysIntEnable(MUON_INT_LEVEL);
	return 0 ;
}

int MUONBeforeInterrupt(int nArg)
{
	return 0  ;
}


int MUONBeforeDMARun(int nArg)
{
	semTake(g_sidMuonSpecial,WAIT_FOREVER);
	return 0 ;
}

int MUONAfterDMARun(int nArg)
{
	if(MCSTwrite((UINT32)0x03000000,32,(UINT32)0xa89a000a)==-1)
	{
		printf("MUON VMEWrite MCST Error!\n");
		Push2ErrStack(PPCFATAL+MUC_FLG+ATOM_ERR+48,0,0,0,0);		/*muc error 48*/	
		return -1;
	}
	semGive(g_sidMuonSpecial);
	return 0 ;
}

