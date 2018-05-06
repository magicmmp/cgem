#include "vxWorks.h"
#include "5100_test.h"
#include "stdlib.h"
#include "sockLib.h"
#include "taskLib.h"
#include "msgQLib.h"


#include "fw_dcgvas.h"
#include "fw_configinfo.h"
#include "fw_roppc.h"
#include "fw_filefunc.h"
#include "fw_tcpfunc.h"
#include "fw_udpfunc.h"
#include "fw_staterrspt.h"



int tmptest()
{
	MSG_Q_ID		g_msgtest=NULL;	
	UINT nRetryTimes = 0;
	
	if((g_msgtest=msgQCreate(MAXNUM_MSGS,sizeof(unsigned),MSG_Q_FIFO))==NULL)
	{
		printf("Message Queue: cannot Create g_msgQcblt!\n");

		return -1 ;
	}

	UINT ttt=0;	
	
	for(ttt=0;ttt<20;ttt++)
	{
		while(msgQSend(g_msgtest,(char*)(&ttt),sizeof(unsigned),10,MSG_PRI_NORMAL)==ERROR)
		{
	
			perror("1");

		}
		printf("%d\n",ttt);
		perror("2");
	}

	UINT ppp=0;	
	for(ttt=0;ttt<20;ttt++)
	{	
		while(msgQReceive(g_msgtest,(char *)(&ppp),sizeof(int),TIMEOUT_TICKS)==ERROR)
		{
			perror("3");

		}
		printf("%d;%d\n",ttt,ppp);
		perror("4");
	}

}

//for some parameter to init ,et: MSG,RINGBUF, SEMinterrupt
int fw_GlobalParaCreate()
{
	if((g_msgQcblt=msgQCreate(MAXNUM_MSGS,sizeof(unsigned),MSG_Q_FIFO))==NULL)
	{
		DEBUG_OUTPUT(("Message Queue: cannot Create g_msgQcblt!\n"));
		Push2ErrStack(PPCERROR+FRM_FLG+ATOM_ERR+11,0,0,0,0);		/*error 11*/
		return -1 ;
	}
	/*printf("MAXNUM_MSGS= %d \n",MAXNUM_MSGS);*/
	if((g_msgQnet=msgQCreate(MAXNUM_MSGS,sizeof(unsigned),MSG_Q_FIFO))==NULL)
	{
        	DEBUG_OUTPUT(("Message Queue: cannot Create g_msgQnet!\n"));
		Push2ErrStack(PPCERROR+FRM_FLG+ATOM_ERR+12,0,0,0,0);		/*error 12*/
		return -1 ;
	}


	if( (g_DmaRng = rng_dmaCreate(DMARNG_SIZE)) ==NULL)
	{
		DEBUG_OUTPUT(("rng_damCreate g_DmaRng failed\n")) ;
		Push2ErrStack(PPCERROR+FRM_FLG+ATOM_ERR+13,0,0,0,0);		/*error 13*/
		return -1 ;
	}

	if( (g_NetRng = rng_dmaCreate(NETRNG_SIZE))==NULL)
	{
		DEBUG_OUTPUT(("rng_damCreate g_NetRng failed\n")) ;
		Push2ErrStack(PPCERROR+FRM_FLG+ATOM_ERR+14,0,0,0,0);		/*error 14*/
		return -1 ;
	}

	//if( (g_sidInterrupt= semBCreate(SEM_Q_PRIORITY, SEM_EMPTY))==NULL)
	if( (g_sidInterrupt= semCCreate(SEM_Q_PRIORITY, 0))==NULL)	
	{
		DEBUG_OUTPUT(("semBCreate g_sidInterrupt failed\n")) ;
		Push2ErrStack(PPCERROR+FRM_FLG+ATOM_ERR+15,0,0,0,0);		/*error 15*/
		return -1 ;
	}
	
	if( (g_sidSATR = semBCreate(SEM_Q_PRIORITY, SEM_EMPTY))==NULL)
	{
		DEBUG_OUTPUT(("semBCreate g_sidSATR failed\n")) ;
		Push2ErrStack(PPCERROR+FRM_FLG+ATOM_ERR+16,0,0,0,0);		/*error 16*/
		return -1 ;
	}

	if( (g_sidBufEmpty = semBCreate(SEM_Q_PRIORITY , SEM_EMPTY ))==NULL)
	{
		DEBUG_OUTPUT(("semBCreate g_sidBufEmpty failed\n")) ;
		Push2ErrStack(PPCERROR+FRM_FLG+ATOM_ERR+17,0,0,0,0);		/*error 17*/
		return -1 ;
	}

	DEBUG_OUTPUT(("fw_GlobalParaCreate SUCC\n"));
	return 0;
}

int fw_GlobalParaFree()
{
	if(g_msgQcblt!=NULL)
	{
		if(-1 == msgQDelete(g_msgQcblt))
		{
			DEBUG_OUTPUT(("msgQDelete(g_msgQcblt) failed.\n")) ;
			Push2ErrStack(PPCERROR+FRM_FLG+ATOM_ERR+28,0,0,0,0);		/*error 28*/
			return -1;
		}
		g_msgQcblt=NULL;
	}
	if(g_msgQnet!=NULL)
	{
		if(-1 == msgQDelete(g_msgQnet))
		{
			DEBUG_OUTPUT(("msgQDelete(g_msgQnet) failed.\n")) ;
			Push2ErrStack(PPCERROR+FRM_FLG+ATOM_ERR+29,0,0,0,0);		/*error 29*/
			return -1;
		}
		g_msgQnet=NULL;
	}
	if(g_DmaRng!=NULL)
	{
		if(-1 == rng_dmaDelete(g_DmaRng))
		{
			DEBUG_OUTPUT(("rng_dmaDelete(g_DmaRng) failed.\n")) ;
			Push2ErrStack(PPCERROR+FRM_FLG+ATOM_ERR+30,0,0,0,0);		/*error 30*/
			return -1;
		}
		g_DmaRng=NULL;
	}
	if(g_NetRng!=NULL)
	{
		if(-1 == rng_dmaDelete(g_NetRng))
		{
			DEBUG_OUTPUT(("rng_dmaDelete(g_NetRng) failed.\n")) ;
			Push2ErrStack(PPCERROR+FRM_FLG+ATOM_ERR+31,0,0,0,0);		/*error 31*/
			return -1;
		}
		g_NetRng=NULL;
	}
	if(g_sidSATR!=NULL)
	{
		if (-1 == semDelete(g_sidSATR))
		{
			DEBUG_OUTPUT(("semDelete(g_sidSATR) failed.\n")) ;
			Push2ErrStack(PPCERROR+FRM_FLG+ATOM_ERR+32,0,0,0,0);		/*error 32*/
			return -1;
		}
		g_sidSATR=NULL;
	}
	if(g_sidInterrupt!=NULL)
	{
		if(-1 == semDelete(g_sidInterrupt) )
		{
			DEBUG_OUTPUT(("semDelete(g_sidInterrupt)  failed.\n")) ;
			Push2ErrStack(PPCERROR+FRM_FLG+ATOM_ERR+33,0,0,0,0);		/*error 33*/
			return -1;
		}
		g_sidInterrupt=NULL;
	}
	if(g_sidBufEmpty!=NULL)
	{
		if(-1 == semDelete(g_sidBufEmpty))
		{
			DEBUG_OUTPUT(("semDelete(g_sidBufEmpty) failed.\n")) ;
			Push2ErrStack(PPCERROR+FRM_FLG+ATOM_ERR+34,0,0,0,0);		/*error 34*/
			return -1;
		}
		g_sidBufEmpty=NULL;
	}

	DEBUG_OUTPUT(("fw_GlobalParaFree SUCC\n"));
	return 0;
}

int fw_DeleteAllTask()
{
	if(nCbltTaskId)
	{
		if(ERROR==taskDelete(nCbltTaskId) )
		{
			printf("taskDelete nCbltTaskId fail\n");
			return -1;
		}
		nCbltTaskId=0;
		printf("taskDelete nCbltTaskId succ\n");
	}

	if(nPackTaskId)
	{
		if(ERROR==taskDelete(nPackTaskId) )
		{
			printf("taskDelete nPackTaskId fail\n");
			return -1;
		}
		nPackTaskId=0;
		printf("taskDelete nPackTaskId succ\n");
	}
	
	if(nNetTaskId)
	{
		if(ERROR==taskDelete(nNetTaskId) )
		{
			printf("taskDelete nNetTaskId fail\n");
			return -1;
		}
		nNetTaskId=0;
		printf("taskDelete nNetTaskId succ\n");
	}


	if(nStatErrTaskId)
	{
		if(ERROR==taskDelete(nStatErrTaskId) )
		{
			printf("taskDelete nStatErrTaskId fail\n");
			return -1;
		}
		nStatErrTaskId=0;
		printf("taskDelete nStatErrTaskId succ\n");
	}
	
	DEBUG_OUTPUT(("fw_DeleteAllTask SUCC\n"));
	return 0;
}

//____________________________________________________send ack___*/
void SendAck(char * p_cAck)
{
	if(ACK_BYTES != send(g_sktCmdAck, p_cAck, ACK_BYTES,0))
	{
		DEBUG_OUTPUT(("sending %s failed at g_sktCmdAck\n", p_cAck)) ;
		Push2ErrStack(PPCERROR+FRM_FLG+ATOM_ERR+5,0,0,0,0);		/*error 5*/
	}
	else
	{
		if(!strcmp("FAIL", p_cAck))
		{
			DEBUG_OUTPUT(("send the str %s at g_sktCmdAck\n", p_cAck)) ;
		}
	}
}
/*actions bellow */
int 	fw_InitializedLOAD (int nArg1) 
{
	int 	nRecv;
	char pcFirst4BytesConfigInfo[8]={0};
	int 	nBytesOfConfigInfo;

	bzero((char*)(0x10000000),1024*1024);//clear config infor buffer
	g_pCrateConfigHeader=NULL;
	
	if (4== Recv(g_sktCmdAck, pcFirst4BytesConfigInfo, 4))
	{
		g_pCrateConfigHeader = (struct Header*) pcFirst4BytesConfigInfo ;
		
		nBytesOfConfigInfo = g_pCrateConfigHeader->All_len ;
		DEBUG_OUTPUT( ("All crate config info length: %d bytes\n", nBytesOfConfigInfo) ) ;
		
		g_pCrateConfigHeader = (struct Header *)(0x10000000); //no need free
														    //put config infor to the >256M position
 		memcpy(g_pCrateConfigHeader, pcFirst4BytesConfigInfo, 4) ;
		if (Recv(g_sktCmdAck, ((char*)g_pCrateConfigHeader) + 4, nBytesOfConfigInfo - 4)== (nBytesOfConfigInfo - 4) )
		{
			//if for general implement for explant, here to do ntohl
			//in vxworks ntohl do nothing ,so coment it
			//UINT * upConvert=g_pCrateConfigHeader;
			//for(int idx=0;idx<nBytesOfConfigInfo/4;idx++)
			//{
			//	*upConvert=ntohl(*upConvert);
			//	upConvert++;
			//}
			
		}
		else
		{
			DEBUG_OUTPUT( ("Recv(g_sktCmdAck, ((char*)g_pCrateConfigHeader) + 4...) error, errno = %d\n", errno) ) ;
			Push2ErrStack(PPCERROR+FRM_FLG+ATOM_ERR+6,0,0,0,0);		/*error 6*/
			SendAck(FAIL);
			return -1 ;
		}
	}
	else
	{
		printf("Recv(g_sktCmdAck, pcFirst4BytesConfigInfo...) error, errno = %d\n", errno) ;
		Push2ErrStack(PPCERROR+FRM_FLG+ATOM_ERR+7,0,0,0,0);			/*error 7*/
		SendAck(FAIL);
		return -1 ;
	}
	/*
	unsigned short * tttt;
	tttt=(unsigned short *)g_pCrateConfigHeader;
	for(int tdd=nBytesOfConfigInfo/2-100;tdd<nBytesOfConfigInfo/2;tdd++)
	{
		printf("%4x  ",tttt[tdd]);
		if((tdd%10)==0)printf("\n");
	}
	printf("\n");
	*/
	
	UINT nCrateID =  g_pCrateConfigHeader->crate_id;/*compare crate id*/
	if(g_nCrateID != nCrateID)
	{
		// tmp jixl 20140411
		//DEBUG_OUTPUT(("ERROR: g_nCrateID = %d, Header.crate_id = %d\n", g_nCrateID, nCrateID)) ;
		//Push2ErrStack(PPCERROR+FRM_FLG+ATOM_ERR+8,0,0,0,0);			/*error 8*/
		//SendAck(FAIL) ;
		//return -1 ;
	}
	Show_Header(g_pCrateConfigHeader);
	
	g_nRunMode = g_pCrateConfigHeader->trigger_type ;
//	g_nRunMode = NOMALMODE;
	g_nTrgPerInt = g_pCrateConfigHeader->events_per_read;
	g_nMdlSum = g_pCrateConfigHeader->module_num;
	g_nL1Count = g_pCrateConfigHeader->run_times;

	if (NULL == g_pfnInitializedLOAD)
	{
		DEBUG_OUTPUT(("g_pfnInitializedLOAD is not linked to an instance.\n")) ;
		Push2ErrStack(PPCERROR+FRM_FLG+ATOM_ERR+9,0,0,0,0);			/*error 9*/
		SendAck(FAIL) ;
		return -1;
	}
	if(-1 == g_pfnInitializedLOAD(nArg1))
	{
		DEBUG_OUTPUT(("g_pfnInitializedLOAD failed\n")) ;   /*in function send error detail*/
		SendAck(FAIL) ;
		return -1 ;
	}		
	
	DEBUG_OUTPUT (("InitializedLOAD OK.\n")) ;
	g_nCurnState = STID_Loaded;
	SendAck(SUCC) ;
	DEBUG_OUTPUT (("SUCC is sent.\n")) ;
	return 0 ;
}
int 	fw_LoadedPRCF (int nArg1)
{
	if (NULL == g_pfnLoadedPRCF)
	{
		DEBUG_OUTPUT(("g_pfnLoadedPRCF is not linked to an instance.\n")) ;
		Push2ErrStack(PPCERROR+FRM_FLG+ATOM_ERR+10,0,0,0,0);		/*error 10*/
		SendAck(FAIL) ;
		return -1;
	}
	if(-1 == g_pfnLoadedPRCF(nArg1))
	{
		DEBUG_OUTPUT(("g_pfnLoadedPRCF failed")) ;
		SendAck(FAIL) ;
		return -1 ;
	}	

	DEBUG_OUTPUT (("LoadedPRCF OK.\n")) ;
	g_nCurnState = STID_PreConfiged;
	SendAck(SUCC) ;
	DEBUG_OUTPUT (("SUCC is sent.\n")) ;
	return 0 ;
}
int 	fw_PreconfigedCONF (int nArg1)
{
	if(-1==fw_GlobalParaCreate())
	{
		SendAck(FAIL) ;
		return -1;
	}

	if (NULL == g_pfnPreconfigedCONF)
	{
		DEBUG_OUTPUT(("g_pfnPreconfigedCONF is not linked to an instance.\n")) ;
		Push2ErrStack(PPCERROR+FRM_FLG+ATOM_ERR+18,0,0,0,0);		/*error 18*/
		SendAck(FAIL) ;
		return -1;
	}
	if(-1 == g_pfnPreconfigedCONF(nArg1))
	{
		DEBUG_OUTPUT(("g_pfnPreconfigedCONF failed\n")) ;
		SendAck(FAIL) ;
		return -1 ;
	}	
	
	DEBUG_OUTPUT (("PreconfigedCONF OK.\n")) ;
	g_nCurnState = STID_Configed;
	SendAck(SUCC) ;
	DEBUG_OUTPUT (("SUCC is sent.\n")) ;
	return 0 ;
}
int 	fw_ConfigedSYN1 (int nArg1) 
{ 
	if (NULL == g_pfnConfigedSYN1)
	{
		DEBUG_OUTPUT(("g_pfnConfigedSYN1 is not linked to an instance.\n")) ;
		Push2ErrStack(PPCERROR+FRM_FLG+ATOM_ERR+19,0,0,0,0);		/*error 19*/
		SendAck(FAIL) ;
		return -1;
	}
	if(-1 == g_pfnConfigedSYN1(nArg1))
	{
		DEBUG_OUTPUT(("g_pfnConfigedSYN1 failed")) ;
		SendAck(FAIL) ;
		return -1 ;
	}	
	DEBUG_OUTPUT (("ConfigedSYN1 OK.\n")) ;
	g_nCurnState = STID_Synchronized1;
	SendAck(SUCC) ;
	DEBUG_OUTPUT (("SUCC is sent.\n")) ;
	return 0 ;
}
int 	fw_Synchronized1SYN2 (int nArg1)
{
	if (NULL == g_pfnSynchronized1SYN2)
	{
		DEBUG_OUTPUT(("g_pfnSynchronized1SYN2 is not linked to an instance.\n")) ;
		Push2ErrStack(PPCERROR+FRM_FLG+ATOM_ERR+20,0,0,0,0);		/*error 20*/
		SendAck(FAIL) ;
		return -1;
	}
	if(-1 == g_pfnSynchronized1SYN2(nArg1))
	{
		DEBUG_OUTPUT(("g_pfnSynchronized1SYN2 failed")) ;
		SendAck(FAIL) ;
		return -1 ;
	}	
	DEBUG_OUTPUT (("Synchronized1SYN2 OK.\n")) ;
	g_nCurnState = STID_Synchronized2;
	SendAck(SUCC) ;
	DEBUG_OUTPUT (("SUCC is sent.\n")) ;
	return 0 ;
}
int 	fw_Synchronized2SYN3 (int nArg1)
{
	if (NULL == g_pfnSynchronized2SYN3)
	{
		DEBUG_OUTPUT(("g_pfnSynchronized2SYN3 is not linked to an instance.\n")) ;
		Push2ErrStack(PPCERROR+FRM_FLG+ATOM_ERR+21,0,0,0,0);		/*error 21*/
		SendAck(FAIL) ;
		return -1;
	}
	if(-1 == g_pfnSynchronized2SYN3(nArg1))
	{
		DEBUG_OUTPUT(("g_pfnSynchronized2SYN3 failed")) ;
		SendAck(FAIL) ;
		return -1 ;
	}	
	DEBUG_OUTPUT (("Synchronized2SYN3 OK.\n")) ;
	g_nCurnState = STID_Synchronized3;
	SendAck(SUCC) ;
	DEBUG_OUTPUT (("SUCC is sent.\n")) ;
	return 0 ;
}
int 	fw_Synchronized3SYN4 (int nArg1) 
{
	if (NULL == g_pfnSynchronized3SYN4)
	{
		DEBUG_OUTPUT(("g_pfnSynchronized3SYN4 is not linked to an instance.\n")) ;
		Push2ErrStack(PPCERROR+FRM_FLG+ATOM_ERR+22,0,0,0,0);		/*error 22*/
		SendAck(FAIL) ;
		return -1;
	}
	if(-1 == g_pfnSynchronized3SYN4(nArg1))
	{
		DEBUG_OUTPUT(("g_pfnSynchronized3SYN4 failed")) ;
		SendAck(FAIL) ;
		return -1 ;
	}	
	DEBUG_OUTPUT (("Synchronized3SYN4 OK.\n")) ;
	g_nCurnState = STID_Synchronized4;
	SendAck(SUCC) ;
	DEBUG_OUTPUT (("SUCC is sent.\n")) ;
	return 0 ;
}
int 	fw_Synchronized4PREP (int nArg1)
{
	g_nTrgCounter = 0 ;
	g_nInterruptCounter = 0 ;
	g_nEventsSent = 0 ;

	//initial the RODHeader & RODTail  leigkleigk
	g_fwRODHead.startOfHeaderMarker =s_rodMarker;
	g_fwRODHead.headerSize          = (sizeof(RODHeader) / sizeof(unsigned int));
	g_fwRODHead.formatVersionNumber = (s_formatVersionNumber);
	g_fwRODHead.sourceIdentifier    = 0xa00000+(g_nSysID<<16)+ (g_nPPCIDOfSRH&0x1f);;
	g_fwRODHead.runNumber		 = 0;
	g_fwRODHead.level1Id            = 0;
	g_fwRODHead.bunchCrossingId     = (0x0);
	g_fwRODHead.level1TriggerType   = g_nRunMode;
	g_fwRODHead.detectorEventType   = (0x0);	//jixl080111 for EMC calib
	//RODtail	
	g_fwRODTail.numberOfStatusElements = (1 );		
	g_fwRODTail.numberOfDataElements = (0);
	g_fwRODTail.statusBlockPosition = (1 );
	//after initial Globle ROD

	gv_nRunningCtrl = RUNNING_NOW ;		//RunningCtrl control the stop sequence,first stop cblt then pack,last net
	g_bStatusSend=TRUE;				//after PREP,start to send status infor up,add by leigk 070404 
	
	if (NULL == g_pfnSynchronized4PREP)
	{
		DEBUG_OUTPUT(("g_pfnSynchronized4PREP is not linked to an instance.\n")) ;
		Push2ErrStack(PPCERROR+FRM_FLG+ATOM_ERR+23,0,0,0,0);		/*error 23*/
		SendAck(FAIL) ;
		return -1;
	}

	if(-1 == g_pfnSynchronized4PREP(nArg1))
	{
		DEBUG_OUTPUT(("g_pfnSynchronized4PREP failed")) ;
		SendAck(FAIL) ;
		return -1 ;
	}		

	DEBUG_OUTPUT (("Synchronized4PREP OK.\n")) ;
	g_nCurnState = STID_Ready;
	SendAck(SUCC) ;
	DEBUG_OUTPUT (("SUCC is sent.\n")) ;
	return 0 ;
}
int 	fw_ReadySATR  (int nArg1)
{
	if (NULL == g_pfnReadySATR)
	{
		DEBUG_OUTPUT(("g_pfnReadySATR is not linked to an instance.\n")) ;
		Push2ErrStack(PPCERROR+FRM_FLG+ATOM_ERR+24,0,0,0,0);		/*error 24*/
		SendAck(FAIL) ;
		return -1;
	}
	if(-1 == g_pfnReadySATR(nArg1))
	{
		DEBUG_OUTPUT(("g_pfnReadySATR run fail.\n")) ;
		SendAck(FAIL) ;
		return -1;
	}	
	g_nCurnState = STID_Running;
	DEBUG_OUTPUT (("ReadySATR OK.\n")) ;
	SendAck(SUCC) ;
	DEBUG_OUTPUT (("SUCC is sent.\n")) ;
	return 0 ;
}
int 	fw_RunningSPTR  (int nArg1)
{
	if (NULL == g_pfnRunningSPTR)
	{
		DEBUG_OUTPUT(("g_pfnRunningSPTR is not linked to an instance.\n")) ;
		Push2ErrStack(PPCERROR+FRM_FLG+ATOM_ERR+25,0,0,0,0);		/*error 25*/
		SendAck(FAIL) ;
		return -1;
	}
	if(-1 == g_pfnRunningSPTR(nArg1))
	{
		DEBUG_OUTPUT(("g_pfnRunningSPTR run fail\n")) ;
		SendAck(FAIL) ;
		return -1 ;
	}
	DEBUG_OUTPUT (("RunningSPTR OK.\n")) ;
	g_nCurnState = STID_Ready;
	SendAck(SUCC) ;
	DEBUG_OUTPUT (("SUCC is sent.\n")) ;
	return 0 ;
}
int 	fw_ReadySTOP  (int nArg1)
{
	if (NULL == g_pfnReadySTOP)
	{
		DEBUG_OUTPUT(("g_pfnPostRunningSTOP is not linked to an instance.\n")) ;
		Push2ErrStack(PPCERROR+FRM_FLG+ATOM_ERR+26,0,0,0,0);		/*error 26*/
		SendAck(FAIL) ;
		return -1;
	}
	
	if(-1 == g_pfnReadySTOP(nArg1))
	{
		DEBUG_OUTPUT(("g_pfnPostRunningSTOP run fail.\n")) ;
		SendAck(FAIL) ;
		return -1 ;
	}
	gv_nRunningCtrl = CBLT_STOP;

	g_bEMCMonictrl = true;	//for EMC Monitor		
				
	semTake(g_sidBufEmpty, WAIT_FOREVER) ;
	if(nCbltTaskId)
	{
		if(ERROR==taskDelete(nCbltTaskId) )
		{
			printf("taskDelete nCbltTaskId fail\n");
			return -1;
		}
		nCbltTaskId=0;
		printf("taskDelete nCbltTaskId succ\n");
	}

	if(nPackTaskId)
	{
		if(ERROR==taskDelete(nPackTaskId) )
		{
			printf("taskDelete nPackTaskId fail\n");
			return -1;
		}
		nPackTaskId=0;
		printf("taskDelete nPackTaskId succ\n");
	}
	

	g_bStatusSend=FALSE;				//after STOP ,stop to send status infor up,add by leigk 070404 
	
	DEBUG_OUTPUT (("ReadySTOP OK.\n")) ;
	g_nCurnState = STID_Configed;
	SendAck(SUCC) ;
	DEBUG_OUTPUT (("SUCC is sent.\n")) ;
	return 0 ;
}
int 	fw_ConfigedUNCF (int nArg1)
{
	if (NULL == g_pfnConfigedUNCF)
	{
		DEBUG_OUTPUT(("g_pfnConfigedUNCF is not linked to an instance.\n")) ;
		Push2ErrStack(PPCERROR+FRM_FLG+ATOM_ERR+27,0,0,0,0);		/*error 27*/
		SendAck(FAIL) ;
		return -1;
	}
	if(-1 == g_pfnConfigedUNCF(nArg1))
	{
		DEBUG_OUTPUT(("g_pfnConfigedUNCF call fail\n")) ;
		SendAck(FAIL) ;
		return -1 ;
	}

	if(-1==fw_GlobalParaFree())
	{
		SendAck(FAIL) ;
		return -1;
	}
	
	DEBUG_OUTPUT (("ConfigedUNCF OK.\n")) ;
	g_nCurnState = STID_PreConfiged;
	SendAck(SUCC) ;
	DEBUG_OUTPUT (("SUCC is sent.\n")) ;
	return 0 ;
}
int 	fw_PreconfigedUNPC (int nArg1) 
{
	if (NULL == g_pfnPreconfigedUNPC)
	{
		DEBUG_OUTPUT(("g_pfnPreconfigedUNPC is not linked to an instance.\n")) ;
		Push2ErrStack(PPCERROR+FRM_FLG+ATOM_ERR+35,0,0,0,0);		/*error 35*/
		SendAck(FAIL) ;
		return -1;
	}
	if(-1 == g_pfnPreconfigedUNPC(nArg1))
	{
		DEBUG_OUTPUT(("g_pfnPreconfigedUNPC run fail\n")) ;
		SendAck(FAIL) ;
		return -1 ;
	}
	if(-1==CloseDataSocket() )
	{
		Push2ErrStack(PPCWARNING+FRM_FLG+ATOM_ERR+38,0,0,0,0);			/*error 38*/
		SendAck(FAIL) ;
		return -1 ;
	}     
	DEBUG_OUTPUT (("PreconfigedUNPC OK.\n")) ;
	g_nCurnState = STID_Loaded;
	SendAck(SUCC) ;
	DEBUG_OUTPUT (("SUCC is sent.\n")) ;
	return 0 ;
}
int 	fw_LoadedUNLD (int nArg1)
{
	if (NULL == g_pfnLoadedUNLD)
	{
		DEBUG_OUTPUT(("g_pfnInitializedUNLD is not linked to an instance.\n")) ;
		Push2ErrStack(PPCERROR+FRM_FLG+ATOM_ERR+36,0,0,0,0);		/*error 36*/
		SendAck(FAIL) ;
		return -1;
	}
	if(-1 == g_pfnLoadedUNLD(nArg1))
	{
		DEBUG_OUTPUT(("g_pfnInitializedLOAD failed")) ;
		SendAck(FAIL) ;
		return -1 ;
	}
	
	dmadisable() ; //leigk 060720 CBLTinit at InitializedLOAD
	   
	DEBUG_OUTPUT (("LoadedUNLD OK.\n")) ;
	g_nCurnState = STID_Initialized;
	SendAck(SUCC) ;
	DEBUG_OUTPUT (("SUCC is sent.\n")) ;
	return 0 ;
}
int	fw_AnyEXIT(int nArg1) 
{
	g_bShutdownOpt = 0;
	if(STID_Initialized != g_nCurnState)
	{
		DEBUG_OUTPUT(("EXIT from a state which is NOT Initialized!\n")) ;
	}

	g_nReportOn=0;					//for exit the report task
	g_bStatusSend=FALSE;			//for exit the report task
	wdCancel(g_widStatusTimer) ;
	semGive(g_sidStaErrTrig) ;		/*for send the last status*/
	taskDelay(10);

	if(-1 == g_pfnAnyEXIT(nArg1))
	{
		SendAck(SUCC) ;//haveto give success to upside,for Shutdown Normally
		taskDelay(rand()%300);		
		reboot(2);
	}

	if(-1== fw_DeleteAllTask())
	{
		DEBUG_OUTPUT (("fw_DeleteAllTask Failed.\n")) ;

		SendAck(SUCC) ;
		taskDelay(rand()%300);			
		reboot(2);
	}

	if(-1==fw_GlobalParaFree())		//ensure free all the global parameter 
	{
		DEBUG_OUTPUT (("Free Global Parameter Failed.\n")) ;

		SendAck(FAIL) ;
		taskDelay(rand()%300);			
		reboot(2);
	}

	if(-1==StateErrParaFree())		//ensure free all the parameter for state errror send task
	{
		DEBUG_OUTPUT (("StateErrParaFree Failed.\n")) ;

		SendAck(SUCC) ;
		taskDelay(rand()%300);			
		reboot(2);
	}

	SendAck(SUCC) ;
	DEBUG_OUTPUT (("SUCC is sent.\n")) ;

	if(-1==CloseAllSockets() )		//sure to close all socket,prepare for next start
	{
		DEBUG_OUTPUT(("CloseAllSockets Error\n"));
		taskDelay(rand()%300);			
		reboot(2);
	}

	dmadisable() ;					//dmadisable once more,ensure free the using semphore
	
	//set parameter zero
	g_nTrgCounter = 0 ;
	g_nInterruptCounter = 0 ;
	g_nEventsSent = 0 ;
	
	if(-1 == taskSpawn("tWaiting", PRI_WAITING, 0, TASK_STACK_SIZE, (FUNCPTR)Waiting, 
		0, 0, 0, 0, 0, 0, 0, 0, 0,0))
	{
		DEBUG_OUTPUT(("taskSpawn tWaiting Failed\n")) ;
		return -1;
	}
	DEBUG_OUTPUT (("AnyEXIT OK.\n")) ;
	return 0 ;
}

int	fw_SelfTrans(int nArg1)
{
	DEBUG_OUTPUT (("Just SelfTrans.OK.\n")) ;
	Push2ErrStack(PPCWARNING+FRM_FLG+ATOM_ERR+37,0,0,0,0);		/*error 37*/
	SendAck(SUCC) ;
	DEBUG_OUTPUT (("SUCC is sent.\n")) ;
	return 0 ;
}

int	fw_AnyImpo(int nArg1)
{
	DEBUG_OUTPUT (("None Defined Operation.OK.\n")) ;
	Push2ErrStack(PPCWARNING+FRM_FLG+ATOM_ERR+38,0,0,0,0);		/*error 38*/
	SendAck(IMPO) ;
	DEBUG_OUTPUT (("IMPO is sent.\n")) ;
	return 0 ;
}

