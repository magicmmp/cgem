/*****sd_trg_runtasks.cpp*****/
#include "5100_test.h"
#include "vxWorks.h"
#include "rng_dma.h"
#include "intLib.h"
#include "sysLib.h"
#include "sockLib.h"
#include "taskLib.h"
#include "msgQLib.h"

#include "fw_dcgvas.h"
#include "fragheaders.h"
#include "fw_filefunc.h"
#include "fw_staterrspt.h"
#include "fw_configinfo.h"
#include "fw_runtasks.h"
#include "fw_define.h"

#include "sd_trg.h"
#include "sd_trg_hw.h"
#include "sd_trg_runtasks.h"
#include "sd_trg_actions.h"
#include "sd_trg_datapack.h"

#define TRG_MAX_RETRY 1000

void TRGDataPack()
{
	char *p_cCbltRng = NULL ;
	char *p_cNetRng = NULL ;
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
			//perror("1");
			if( !(nRetryTimes %TRG_MAX_RETRY))
			{
				DEBUG_OUTPUT(("msgQReceive(g_msgQcblt..., 1) failed %d times\n", nRetryTimes)) ;
				nRetryTimes = 0 ;
				Push2ErrStack(PPCWARNING+TRG_FLG+ATOM_ERR+106,0,0,0,0);		/*trg error 106*/

			}
			if(DPAC_STOP == gv_nRunningCtrl)
			{
				goto TRG_DPAC_EXITING ;
			}
		}
		//perror("2");
		//printf("nBytesToBePacked:%d\n",nBytesToBePacked);
	
		nRetryTimes=0;
		while(NULL == (p_cCbltRng = rng_dmaRead(g_DmaRng,nBytesToBePacked,CPU_METHOD)))
		{
			nRetryTimes ++ ;
			if(nRetryTimes > TRG_MAX_RETRY)
			{
				DEBUG_OUTPUT(("rng_dmaRead from cblt rng failed\n") ) ;
				Push2ErrStack(PPCWARNING+TRG_FLG+ATOM_ERR+107,0,0,0,0);		/*trg error 107*/	
				nRetryTimes=0;
				taskDelay(1) ;
			}
		}
		
		nMaxNetBytes=g_nTrgPerInt *20480*2;  /*must be changed ,when running*/
		while(NULL == (p_cNetRng = rng_dmaApply(g_NetRng,nMaxNetBytes,CPU_METHOD)))
		{
			nRetryTimes ++ ;
			if(nRetryTimes > TRG_MAX_RETRY)
			{
				DEBUG_OUTPUT( ("rng_dmaApply from Net rng failed\n") );
				Push2ErrStack(PPCWARNING+TRG_FLG+ATOM_ERR+108,0,0,0,0);		/*trg error 108*/	
				nRetryTimes = 0 ;
			}
			taskDelay(1) ;
		}

		if(g_nCrateID==160)
		{
			nBytesToBeSent=FWAbnormPack((UINT*)p_cNetRng,(UINT*)p_cCbltRng);
		}
		else
		{
			if(TRGDataPackCore((UINT*)p_cNetRng, (UINT*)p_cCbltRng,&nBytesToBeSent)==-1)
			{
				nBytesToBeSent=FWAbnormPack((UINT*)p_cNetRng,(UINT*)p_cCbltRng);
			}
		}
		

		if( nBytesToBeSent >nMaxNetBytes )  //check Writen size,must<nMaxNetBytes
		{
			DEBUG_OUTPUT(("ERR:nBytesToBeSent =%d > nMaxNetBytes\n", nBytesToBeSent )) ;
			Push2ErrStack(PPCERROR+TRG_FLG+ATOM_ERR+109,0,0,0,0);		/*trg error 109*/	
			break;
		}
		
		if(ERROR == rng_dmaWritten(g_NetRng,nBytesToBeSent))
		{
			DEBUG_OUTPUT( ("Written(g_NetRng...) error\n"));
			Push2ErrStack(PPCERROR+TRG_FLG+ATOM_ERR+110,0,0,0,0);		/*trg error 110*/	
			break ;
		}
			
		if(ERROR == rng_dmaFree(g_DmaRng,nBytesToBePacked))
		{
			DEBUG_OUTPUT( ("free rng (g_DmaRng...) failed\n") );
			Push2ErrStack(PPCERROR+TRG_FLG+ATOM_ERR+111,0,0,0,0);		/*trg error 111*/	
			break ;
		}

		
		nRetryTimes = 0 ;		
		while(msgQSend(g_msgQnet,(char*)(&nBytesToBeSent),sizeof(int),10,MSG_PRI_NORMAL)==ERROR)
		{	
			nRetryTimes++;
			if(nRetryTimes>10)
			{
				DEBUG_OUTPUT(("msgQSend(g_msgQnet...) send fail!\n"));
				Push2ErrStack(PPCWARNING+TRG_FLG+ATOM_ERR+112,0,0,0,0);		/*tof error 112*/	
				nRetryTimes=0;
			}
		}

		
	
	}
TRG_DPAC_EXITING:
	gv_nRunningCtrl= NET_STOP ;
	nPackTaskId=0;//if exit normal then not delete at last
	DEBUG_OUTPUT(("end of the data pack proccess\n"));
}



int TRGEnteringCbltTrans(int nArg)
{
	g_nMaxCbltBytes = g_nTrgPerInt * 20480 *2;   /*CBLT NEED 512 */
	if(g_TRGDBInfo.trg_tdc_sum>0)
	{
		g_nCbltAdrs = uTrgTDCCbltAdrs;
	}
	else
		g_nCbltAdrs =0x01000000;
	DEBUG_OUTPUT(("g_nCbltAdrs = 0x%x.\n", g_nCbltAdrs)) ;
	return 0 ;
}

int TRGBeforeInterrupt(int nArg)
{
	sysIntEnable(TRG_INT_LEVEL) ;

	if(g_TRGDBInfo.trg_tdc_sum==0)
	{
		if (MCSTwrite(0x02000000,D32,0x00000000) != 0)
		{
			DEBUG_OUTPUT(("ERROR:MCST D32 error!\n"));
			Push2ErrStack(PPCFATAL+TRG_FLG+ATOM_ERR+113,0,0,0,0);		/*trg error 113*/	
			return -1;
		}
	}
	return 0  ;
}


int TRGBeforeDMARun(int nArg)
{
	//comment by lifei for JinDP
	//WriteToVme16(0x1c+(TrgTFCFBSEL<<19), VA24, 0x1);//enable interrupt
	return 0 ;
}

int TRGAfterDMARun(int nArg)
{

//	only for crate 160
	if ((g_TRGDBInfo.trg_ets_sum>0)&&(TrgETSCbltEnable==1))
	{

		unsigned int data1,data2;	
		ReadFrmVme32( 0x10+(TrgETSBSEL<<19), VA24,&data1);
		ReadFrmVme32( 0x10+(TrgETSBSEL<<19), VA24,&data2);

		if(g_nL1Count==1)		
			printf("ETS TAKE DATA!!! data1:0x%x; data2:0x%x\n",data1,data2);

		*((int*)g_nETSWrite) = data1;
		*((UINT32*)g_nETSWrite + 1) = data2 ;

		
		g_nBytesCblted = g_nBytesCblted +8;
	}
	
	return 0 ;
}

