#include "vxWorks.h"
#include "sysLib.h"
#include "semLib.h"
#include "stdio.h"
#include "taskLib.h"
#include "intLib.h"
#include "arch/ppc/ivPpc.h"

#include "5100_test.h"
#include "fw_roppc.h"
#include "fw_dcgvas.h"
#include "fw_define.h"
#include "fw_staterrspt.h"
#include "fw_configinfo.h"

#include "sd_muon.h"
#include "sd_muon_hw.h"
#include "sd_trg_hw.h"

bool g_bMUONRocIntConed=false;

void MUONIntHandler(int nArg)
{
	//logMsg("interrupt receive OK,%d\n", nArg, 0, 0, 0, 0, 0);

	semGive(g_sidInterrupt);
	g_nInterruptCounter+=g_nTrgPerInt;
}
int   MUONConfIntCBLT()
{
	//error_info = InitISR((VOIDFUNCPTR) EmcIntHandler,EMC_INT_VEC,EMC_INT_LEVEL);  /* warning*/
	if(FALSE == g_bMUONRocIntConed) 
	{
		if(intConnect (INUM_TO_IVEC(MUON_INT_VEC), (VOIDFUNCPTR)MUONIntHandler, 0) == ERROR)
	  	{
	     		printf("intConnect  failed.\n");
			Push2ErrStack(PPCFATAL+MUC_FLG+ATOM_ERR+64,0,0,0,0);		/*muc error 64*/	
			return  -1;
	  	}
		g_bMUONRocIntConed = TRUE ;
	}

	printf("Before CBLTinit.\n") ;
	if(-1 == CBLTinit(32))
	{
		DEBUG_OUTPUT(("MUC CBLT32 init failed\n") );
		Push2ErrStack(PPCFATAL+MUC_FLG+ATOM_ERR+62,0,0,0,0);		/*muc error 62*/
		return  -1;
	}		
	return 0;
}

/**D[31:24]=8'ha4； D[23:16]=8'hcb； 
D[15:8]=8'hXX, XX为要屏蔽的链所在的板在VME机箱中的位置（即此板的Slot号，取值范围为1到21）；
D[7:0]=8'hxx， xx为要屏蔽的链在其所在板中的位置（即此链在板中的编号，取值范围为0，1，2，3
***/
int MUONDisableChain()
{
		int nIndex;
		unsigned int Chain[4];
		unsigned Val;
		for(nIndex=0;nIndex<g_MUONDBInfo.enabl_romo_sum;nIndex++)
		{

			Chain[0] = gMucDisableChainID[nIndex]&0x1;
			if(Chain[0]!=0)
			{
				
				Val = 0xa4cb0000 + (gMucEnablRomoGeo[nIndex]<<8);
				printf("DisableChain : chain 0 , Geo 0x%x , Val 0x%x \n",gMucEnablRomoGeo[nIndex],Val);
				if(MCSTwrite((UINT32)0x03000000,32,Val)==-1)
				{
					DEBUG_OUTPUT(("MUONDisableChain Error!\n"));
					return -1;				
				}
				taskDelay(10);
			}
			
			Chain[1] = gMucDisableChainID[nIndex]&0x2;
			if(Chain[1]!=0)
			{
				
				Val = 0xa4cb0001 + (gMucEnablRomoGeo[nIndex]<<8);
				printf("DisableChain : chain 1 , Geo 0x%x , Val 0x%x  \n",gMucEnablRomoGeo[nIndex],Val);
				if(MCSTwrite((UINT32)0x03000000,32,Val)==-1)
				{
					DEBUG_OUTPUT(("MUONDisableChain Error!\n"));
					return -1;				
				}
				taskDelay(10);
			}
			
			Chain[2] = gMucDisableChainID[nIndex]&0x4;
			if(Chain[2]!=0)
			{
				
				Val = 0xa4cb0002 + (gMucEnablRomoGeo[nIndex]<<8);
				printf("DisableChain : chain 2 , Geo 0x%x , Val 0x%x   \n",gMucEnablRomoGeo[nIndex],Val);
				if(MCSTwrite((UINT32)0x03000000,32,Val)==-1)
				{
					DEBUG_OUTPUT(("MUONDisableChain Error!\n"));
					return -1;				
				}
				taskDelay(10);
			}
			
			Chain[3] = gMucDisableChainID[nIndex]&0x8;
			if(Chain[3]!=0)
			{
				
				Val = 0xa4cb0003 + (gMucEnablRomoGeo[nIndex]<<8);
				printf("DisableChain : chain 3 , Geo 0x%x , Val 0x%x    \n",gMucEnablRomoGeo[nIndex],Val);
				if(MCSTwrite((UINT32)0x03000000,32,Val)==-1)
				{
					DEBUG_OUTPUT(("MUONDisableChain Error!\n"));
					return -1;				
				}
				taskDelay(10);
			}
			

		}

		return 0;
}

int MUONRomoConfigAll()
{
	//pay attention only config the module set enabled
	//set CBLT Chain, the first board and last board
	int nFirstBoardGeo=22;
	int nLastBoardGeo=0;
	int nIndex;
	int nGeoTmp;

	
	//for set cblt chain
	for(nIndex=0;nIndex<g_MUONDBInfo.enabl_romo_sum;nIndex++)
	{
		if(gMucEnablRomoGeo[nIndex]<nFirstBoardGeo)
		{
			nFirstBoardGeo=gMucEnablRomoGeo[nIndex];
		}
		if(gMucEnablRomoGeo[nIndex]>nLastBoardGeo)
		{
			nLastBoardGeo=gMucEnablRomoGeo[nIndex];
		}
	}
	//Assignned First&Last board;  
	if(nFirstBoardGeo==nLastBoardGeo)
	{
		if(WriteToVme32(((nFirstBoardGeo<<19)+0x060040),VCSR,(UINT32)0x28)==-1)
		{
			DEBUG_OUTPUT(("Write only board %d wrong\n",nFirstBoardGeo));
			Push2ErrStack(PPCFATAL+MUC_FLG+ATOM_ERR+12,0,0,0,0);		/*muc error 12*/
			return -1;
		}
		else
		{
			printf("assign only board %d \n",nFirstBoardGeo);
		}
		taskDelay(1);
	}
	else
	{
		//Assignned First board;
		if(WriteToVme32(((nFirstBoardGeo<<19)+0x060040),VCSR,(UINT32)0x20)==-1)
		{
			DEBUG_OUTPUT(("Write first board CSR wrong:%d\n",nFirstBoardGeo));
			Push2ErrStack(PPCFATAL+MUC_FLG+ATOM_ERR+13,0,0,0,0);		/*muc error 13*/
			return -1;			
		}
		else
		{
			printf("assign first board geo:%d\n",nFirstBoardGeo);
		}
		taskDelay(1);
		//Assignned last board;
		if(WriteToVme32(((nLastBoardGeo<<19)+0x060040),VCSR,(UINT32)0x08)==-1)
		{
			DEBUG_OUTPUT(("Write last CSR wrong:%d\n",nLastBoardGeo));
			Push2ErrStack(PPCFATAL+MUC_FLG+ATOM_ERR+14,0,0,0,0);		/*muc error 14*/
			return -1;			
		}
		else
		{
			printf("assign last board geo:%d\n",nLastBoardGeo);
		}
		taskDelay(1);
	} 
printf("*******************!\n");
	//config VME&FEC
	if(MCSTwrite((UINT32)0x03000000,32,0xa8910001)==-1)
	{
		printf("VMEWriteError1!\n");
		Push2ErrStack(PPCFATAL+MUC_FLG+ATOM_ERR+65,0,0,0,0);		/*muc error 65*/
	}
	taskDelay(1);

	if(MCSTwrite((UINT32)0x03000000,32,0xa8920002)==-1)
	{
		printf("VMEWriteError2!\n");
		Push2ErrStack(PPCFATAL+MUC_FLG+ATOM_ERR+66,0,0,0,0);		/*muc error 66*/
	}
	taskDelay(60);


	if(MCSTwrite((UINT32)0x03000000,32,0xa4930003)==-1)
	{
		printf("VMEWriteError3!\n");
		Push2ErrStack(PPCFATAL+MUC_FLG+ATOM_ERR+67,0,0,0,0);		/*muc error 67*/
	}
	taskDelay(1);	

	if(MCSTwrite((UINT32)0x03000000,32,0xa4940004)==-1)
	{
		printf("VMEWriteError4!\n");
		Push2ErrStack(PPCFATAL+MUC_FLG+ATOM_ERR+68,0,0,0,0);		/*muc error 68*/
	}
	taskDelay(60);



 
	//clear ROMO triger number
	if(MCSTwrite((UINT32)0x03000000,32,0xaea4aaa4)==-1)
	{
		DEBUG_OUTPUT(("Clear ROMO triger number Error!\n"));
		Push2ErrStack(PPCWARNING+MUC_FLG+ATOM_ERR+15,0,0,0,0);		/*muc error 15*/
	}
	taskDelay(30);

	//for disable module
	DEBUG_OUTPUT(("g_MUONDBInfo.disabl_romo_sum = %d\n",g_MUONDBInfo.disabl_romo_sum));
	DEBUG_OUTPUT(("g_MUONDBInfo.enabl_romo_sum = %d\n",g_MUONDBInfo.enabl_romo_sum));
	
	for(nIndex=0;nIndex<g_MUONDBInfo.disabl_romo_sum;nIndex++)
	{	
		nGeoTmp=g_MUONDBInfo.disabl_romo_header[nIndex]->Position;
		if(MCSTwrite((UINT32)0x03000000,32,0xa0a3bb00+nGeoTmp)==-1)//slot n
		{
			DEBUG_OUTPUT(("VME Disable Module %d Error!\n",nGeoTmp));
			Push2ErrStack(PPCWARNING+MUC_FLG+ATOM_ERR+16,0,0,0,0);		/*muc error 16*/
		}

	}	

	//set romo module work mode 
	switch(g_nRunMode)
	{
		case NOMALMODE:
		case PEDMODE:
		case CALIBRATION:
		case MONITOR:
		case BASELINE:
		case WAVEMODE:
			//set trigger source---a8950005-readout||a8960006-ROC||a8970007-lemo
			if(MCSTwrite((UINT32)0x03000000,32,0xa8960006)==-1)
			{
				DEBUG_OUTPUT(("set trigger source Error!\n"));
				Push2ErrStack(PPCFATAL+MUC_FLG+ATOM_ERR+17,0,0,0,0);		/*muc error 17*/
				return -1;
			}
			DEBUG_OUTPUT(("set trigger source ROC!\n"));
			taskDelay(10);

			//set data source 
			DEBUG_OUTPUT(("g_MUONDBInfo.ppc_header->NormModeSelfL1=%d\n",g_MUONDBInfo.ppc_header->NormModeSelfL1));
			
			if(g_MUONDBInfo.ppc_header->NormModeSelfL1)
			{
				//set data source ---fec
				//if(MCSTwrite((UINT32)0x03000000,32,0xa4810081)==-1)
				//change 0xa4810081 to 0xa4820082 by lifei for FEC generate data
				if(MCSTwrite((UINT32)0x03000000,32,0xa4820082)==-1)
				{
					DEBUG_OUTPUT(("set data source Error!\n"));
					Push2ErrStack(PPCFATAL+MUC_FLG+ATOM_ERR+19,0,0,0,0);		/*muc error 19*/
					return -1;
				}
				DEBUG_OUTPUT(("set data source fec!\n"));
			}
			else
			{
				//set data source---detector
				if(MCSTwrite((UINT32)0x03000000,32,0xa4810081)==-1)
				{
					DEBUG_OUTPUT(("set data source Error!\n"));
					Push2ErrStack(PPCFATAL+MUC_FLG+ATOM_ERR+19,0,0,0,0);		/*muc error 19*/
					return -1;
				}
				DEBUG_OUTPUT(("set data source detector!\n"));
			}
			taskDelay(10);

			//set data mode ---84-zip || 85-nozip
			if(MCSTwrite((UINT32)0x03000000,32,0xa4840084)==-1)
			{
				DEBUG_OUTPUT(("set data mode Error!\n"));
				Push2ErrStack(PPCFATAL+MUC_FLG+ATOM_ERR+20,0,0,0,0);		/*muc error 20*/
				return -1;				
			}
			taskDelay(10);

			//clear register
			if(MCSTwrite((UINT32)0x03000000,32,0xaea4a4a4)==-1)
			{
				DEBUG_OUTPUT(("MUC Clear Register Error!\n"));
				Push2ErrStack(PPCWARNING+MUC_FLG+ATOM_ERR+34,0,0,0,0);		/*muc error 34*/	
			}
			taskDelay(10);			

			//set patten mode
			if(MCSTwrite((UINT32)0x03000000,32,0xa6865555)==-1)
			{
				DEBUG_OUTPUT(("set patten mode Error!\n"));
				Push2ErrStack(PPCFATAL+MUC_FLG+ATOM_ERR+21,0,0,0,0);		/*muc error 21*/
				return -1;
			}
			taskDelay(10);

			//set threshold, a2a900ef,see the document, read from vme module no effect
			if(g_MUONDBInfo.ppc_header->NormModeSelfL1)
			{
				g_MUONDBInfo.enabl_romo_header[0]->threshold=249; //jixl temp for self test
			}
			DEBUG_OUTPUT(("threshold= %d\n",g_MUONDBInfo.enabl_romo_header[0]->threshold));
			if(MCSTwrite((UINT32)0x03000000,32,(0xa2a90000+(g_MUONDBInfo.enabl_romo_header[0]->threshold)))==-1)
			{
				DEBUG_OUTPUT(("set threshold Error!\n"));
				Push2ErrStack(PPCFATAL+MUC_FLG+ATOM_ERR+22,0,0,0,0);		/*muc error 22*/
				return -1;
			}
			taskDelay(10);

			//set test value, a2aa0001,read from vme module mode no effect
			if(MCSTwrite((UINT32)0x03000000,32,0xa2aa0000)==-1)
			{
				DEBUG_OUTPUT(("set test value Error!\n"));
				Push2ErrStack(PPCFATAL+MUC_FLG+ATOM_ERR+23,0,0,0,0);		/*muc error 23*/
				return -1;				
			}
			taskDelay(10);

			//set FEC board num per chain
			if(MCSTwrite((UINT32)0x03000000,32,0xa48a000c)==-1)
			{
				DEBUG_OUTPUT(("set FEC board num Error!\n"));
				Push2ErrStack(PPCFATAL+MUC_FLG+ATOM_ERR+24,0,0,0,0);		/*muc error 24*/
				return -1;				
			}
			taskDelay(10);

			//set n triggers per intterupt		
			if(MCSTwrite((UINT32)0x03000000,32,(0xa89f0000+g_nTrgPerInt))==-1)
			{
				DEBUG_OUTPUT(("set trig per int Error!\n"));
				Push2ErrStack(PPCFATAL+MUC_FLG+ATOM_ERR+25,0,0,0,0);		/*muc error 25*/
				return -1;				
			}
			taskDelay(10);

			if(g_MUONDBInfo.ppc_header->NormModeSelfL1)
			{
				//set triger rate ---only for self trigger
				g_MUONDBInfo.roc_header->triger_rate =1;
				if(WriteToVme32(0xf00000,VA32,0x00a00000+((g_MUONDBInfo.roc_header->triger_rate)<<3))==-1)
				{
					DEBUG_OUTPUT(("set tirg speed Error!\n"));
					Push2ErrStack(PPCFATAL+MUC_FLG+ATOM_ERR+26,0,0,0,0);		/*muc error 26*/
					return -1;	
				}
				else 
				{
					DEBUG_OUTPUT(("trigger_rate =%d\n",g_MUONDBInfo.roc_header->triger_rate));
	 			}
	 			taskDelay(10);	
	 		}
			
			//set trigger delay
			if(g_MUONDBInfo.ppc_header->NormModeSelfL1)
			{				
				if(MCSTwrite((UINT32)0x03000000,32,0xa48c00d2)==-1) //a48c00xx delay time= xx*50ns
				{
					DEBUG_OUTPUT(("set trigger delay error!\n"));
					Push2ErrStack(PPCFATAL+MUC_FLG+ATOM_ERR+27,0,0,0,0);		/*muc error 27*/
					return -1;						
				}
				DEBUG_OUTPUT(("set trigger delay 0xa48c00a0!\n"));
				taskDelay(10);
			}
			else
			{
				if(MCSTwrite((UINT32)0x03000000,32,0xa48c001b)==-1) //a48c00xx delay time= xx*50ns, 080126 chg to 2f
				{
					DEBUG_OUTPUT(("set trigger delay error!\n"));
					Push2ErrStack(PPCFATAL+MUC_FLG+ATOM_ERR+27,0,0,0,0);		/*muc error 27*/
					return -1;						
				}
				DEBUG_OUTPUT(("set trigger delay 0xa48c002f!\n"));
				taskDelay(10);			
			}

			MUONDisableChain();
				
			//set readout module start
			if(MCSTwrite((UINT32)0x03000000,32,0xaea3aaa3)==-1)
			{
				DEBUG_OUTPUT(("module start Error!\n"));
				Push2ErrStack(PPCFATAL+MUC_FLG+ATOM_ERR+31,0,0,0,0);		/*muc error 31*/
				return -1;					
			}
			taskDelay(10);

			break ;	
		default:
			break ;
	}
}




void MUONStartTrig()
{	
	//start trig		
	if(WriteToVme32(0xf00000,VA32,0xaaa007)==-1)	
	{
		DEBUG_OUTPUT(("start tirg Error!\n"));
		Push2ErrStack(PPCFATAL+MUC_FLG+ATOM_ERR+32,0,0,0,0);		/*muc error 32*/
		return ;	
	}	
	
}

void MUONStopTrig()
{	
	semTake(g_sidMuonSpecial,WAIT_FOREVER);
	
	//stop trig
	if(WriteToVme32(0xf00000,VA32,0xaaa077)==-1)	
	{
		DEBUG_OUTPUT(("stop trig Error!\n"));
		Push2ErrStack(PPCFATAL+MUC_FLG+ATOM_ERR+33,0,0,0,0);		/*muc error 33*/
		return ;			
	}
	semGive(g_sidMuonSpecial);
	taskDelay(300);

	//clear register
	if(MCSTwrite((UINT32)0x03000000,32,0xaea4a4a4)==-1)
	{
		DEBUG_OUTPUT(("MUC Clear Register Error!\n"));
		Push2ErrStack(PPCWARNING+MUC_FLG+ATOM_ERR+34,0,0,0,0);		/*muc error 34*/	
	}
	taskDelay(30);
	
	//set patten mode
	if(MCSTwrite((UINT32)0x03000000,32,0xa6865555)==-1)
	{
		DEBUG_OUTPUT(("Reset patten mode Error!\n"));
		Push2ErrStack(PPCWARNING+MUC_FLG+ATOM_ERR+35,0,0,0,0);		/*muc error 35*/	
	}
	taskDelay(1);
}

void MUONStop() //jixl 071204
{
	//stop trig
	if(WriteToVme32(0xf00000,VA32,0xaaa077)==-1)	
	{
		DEBUG_OUTPUT(("stop trig Error!\n"));
		Push2ErrStack(PPCFATAL+MUC_FLG+ATOM_ERR+69,0,0,0,0);		/*muc error 69*/
	}
	
	//stop readout module
	taskDelay(300);			
	if(MCSTwrite((UINT32)0x03000000,32,0xaea4aaa4)==-1) 
	{
		DEBUG_OUTPUT(("stop readout module error!\n"));
		Push2ErrStack(PPCFATAL+MUC_FLG+ATOM_ERR+70,0,0,0,0);		/*muc error 70*/
	}
	else DEBUG_OUTPUT(("*****readout module stopped!*****\n"));

}


int TRG_MUDT_syn3()
{
	DEBUG_OUTPUT(("g_MUONDBInfo.mudt_sum = %d\n",g_MUONDBInfo.mudt_sum));
	for(int imudt = 0;imudt<g_MUONDBInfo.mudt_sum;imudt++)
	{
		if(-1==WrtRdA24D16(0x70000+(MucMUDTBSEL[imudt]<<19),g_MUONDBInfo.mudt_header[imudt]->ControlState,"MUDT_ControlState",0xf) ) 
		{
			DEBUG_OUTPUT(("Write MUDT_ControlState Fail!\n"));
			Push2ErrStack(PPCFATAL+MUC_FLG+ATOM_ERR+36,0,0,0,0);		/*muc error 36*/	
			return -1;
		}
	}
}


/*here below for update fpga logic*/

int MUONsetHW()
{
	int nFirstBoardGeo=22;
	int nLastBoardGeo=1;

	g_MUONHWInfo.romo_sum=0;
	for(int nMdlIdx=2;nMdlIdx<22;nMdlIdx++)
	{
		unsigned short nValueTmp;
		if(ReadFrmVme16((UINT32)((nMdlIdx<<19)+0x060040),VCSR,&nValueTmp)==-1)
		{
			printf("There is no board at %d slot\n",nMdlIdx);
		}
		else
		{	
			if(nValueTmp==0x41)
			{
				g_MUONHWInfo.romo_geo[g_MUONHWInfo.romo_sum]=nMdlIdx;
				printf("scan one module at %d slot\n",nMdlIdx);	
				g_MUONHWInfo.romo_sum++;
			}	
			else
			{
				printf("scan slot %d error\n",nMdlIdx);
			}
		}
	}
	printf("g_MUONHWInfo.romo_sum=%d\n",g_MUONHWInfo.romo_sum);
	
	for(nMdlIdx=0;nMdlIdx<g_MUONHWInfo.romo_sum;nMdlIdx++)
	{
		printf("g_MUONHWInfo.romo_geo[nMdlIdx]=%d\n",g_MUONHWInfo.romo_geo[nMdlIdx]);
		if(g_MUONHWInfo.romo_geo[nMdlIdx]<nFirstBoardGeo)
		{
			nFirstBoardGeo=g_MUONHWInfo.romo_geo[nMdlIdx];
		}
		if(g_MUONHWInfo.romo_geo[nMdlIdx]>nLastBoardGeo)
		{
			nLastBoardGeo=g_MUONHWInfo.romo_geo[nMdlIdx];
		}
	}
	//Assignned First&Last board;
	if(nFirstBoardGeo==nLastBoardGeo)
	{
		if(WriteToVme32(((nFirstBoardGeo<<19)+0x060040),VCSR,(UINT32)0x28)==-1)
		{
			printf("Write only board %d wrong\n",nFirstBoardGeo);
			return -1;
		}
		else
		{
			printf("assign only board %d \n",nFirstBoardGeo);
		}
	}
	else
	{
		//Assignned First board;
		if(WriteToVme32(((nFirstBoardGeo<<19)+0x060040),VCSR,(UINT32)0x20)==-1)
		{
			printf("Write first board CSR wrong:%d\n",nFirstBoardGeo);
			return -1;
		}
		else
		{
			printf("assign first board geo:%d\n",nFirstBoardGeo);
		}
		//Assignned last board;
		if(WriteToVme32(((nLastBoardGeo<<19)+0x060040),VCSR,(UINT32)0x08)==-1)
		{
			printf("Write last CSR wrong:%d\n",nLastBoardGeo);
			return -1;
		}
		else
		{
			printf("assign last board geo:%d\n",nLastBoardGeo);
		}
	}
	printf("nFirstBoardGeo=%d,nLastBoardGeo=%d\n",nFirstBoardGeo,nLastBoardGeo);
	return 0;
}
int MUONupdateFPGA()
{
	FILE *pReadFile;
	char cBuf[512];
	int nGetBytes;

	printf("-----------------------------------------------\n\n");
	printf("####################leigk###\n");
	printf("Please Pay Attention to The Life Of Graduate\n");
	printf("-----------------------------------------------\n");
	printf("GOOD JOB,Please Check Your Program\n");
	
	
	printf("-----------------------------------------------\n");
	printf(":) :P :, :>\n\n\n\n");
	
	printf("Update FPGA Start\n");
	if (NULL == (pReadFile = fopen("ROS070913.rbf", "rb")) ) 
	{
		printf("\n fail to open fpga1 data file \n" );
		taskDelay(100);
		return -1;
	} 
	else 
	{
		printf("\n success to open fpga1 file \n\n" );	
	}
	printf("Erase the Flash1 First,Plese wait 10 s.....\n");
	if(MCSTwrite((UINT32)0x01000000,32,0xf1fcee01)==-1)
	{
		printf("VMEWrite Error,for earse Flash1!\n");
		return -1;
	}
	taskDelay(1);
	if(MCSTwrite((UINT32)0x01000000,32,0xf1000000)==-1)
	{
		printf("VMEWrite Error,for earse Flash1!\n");
		return -1;
	}
	taskDelay(600);
	printf("Erase the Flash1 OK\n");
	//write data
	printf("write fpga1 begin...\n");

	int delaytime = 0;
	
		while(!feof(pReadFile))
                {
			//printf("Now initial page\n");
			if(MCSTwrite((UINT32)0x01000000,32,0xf1fcdd01)==-1)
			{
				printf("VMEWrite Error,for write Flash1!\n");
				return -1;
			}
			for(delaytime = 0; delaytime <1000 ;delaytime++);
			nGetBytes=fread(cBuf,sizeof(char),256,pReadFile);
			if(nGetBytes==-1)
			{
			        printf("read file error\n");
			}
			for(int i=0;i<nGetBytes;i++)
			{
				if(MCSTwrite((UINT32)0x01000000,32,(0xf1000000+cBuf[i]))==-1)
				{
					printf("Write Flash1 Error\n");
					return -1;
				}
				for(delaytime = 0; delaytime <1000 ;delaytime++);
			}
			if(MCSTwrite((UINT32)0x01000000,32,0xf1fcddb1)==-1)
			{
				printf("VMEWrite Error,for write Flash1!\n");
				return -1;
			}
			for(delaytime = 0; delaytime <1000 ;delaytime++);
			if(MCSTwrite((UINT32)0x01000000,32,0xf1000000)==-1)
			{
				printf("VMEWrite Error,for write Flash1!\n");
				return -1;
			}
			taskDelay(1);
                }

	printf("write fpga1 end flag...\n");
	if(MCSTwrite((UINT32)0x01000000,32,0xf1fcdde1)==-1)
	{
		printf("VMEWrite Error,for erase Flash1!\n");
		return -1;
	}
	taskDelay(1);
	if(MCSTwrite((UINT32)0x01000000,32,0xf1000000)==-1)
	{
		printf("VMEWrite Error,for earse Flash1!\n");
		return -1;
	}
	taskDelay(1);
	printf("write fpga1 success\n");
	fclose(pReadFile);

	//////////////////////////////////////////////////////

/*	
	printf("-----------------------------------------------\n");
	printf("####################leigk###\n");
	printf("-----------------------------------------------\n");
	if (NULL == (pReadFile = fopen("", "rb")) ) 
	{
		printf("\n fail to open fpga2 data file \n" );
		taskDelay(100);
		return -1;
	} 
	else 
	{
		printf("\n success to open fpga2 file\n \n" );	
	}
	printf("Erase the Flash2 ,Plese wait 10 seconds......\n");
	if(MCSTwrite((UINT32)0x01000000,32,0xf2fcee01)==-1)
	{
		printf("VMEWrite Error,for earse Flash2!\n");
		return -1;
	}
	taskDelay(1);
	if(MCSTwrite((UINT32)0x01000000,32,0xf2000000)==-1)
	{
		printf("VMEWrite Error,for earse Flash2!\n");
		return -1;
	}
	taskDelay(600);
	printf("Erase the Flash2 OK\n");
	//write data
	printf("write fpga2 begin...\n");
		while(!feof(pReadFile))
                {
			//printf("Now initial page\n");
			if(MCSTwrite((UINT32)0x01000000,32,0xf2fcdd01)==-1)
			{
				printf("VMEWrite Error,for write Flash2!\n");
				return -1;
			}
			for(delaytime = 0; delaytime <1000 ;delaytime++);
			nGetBytes=fread(cBuf,sizeof(char),256,pReadFile);
			if(nGetBytes==-1)
			{
			        printf("read file error\n");
			}
			for(int i=0;i<nGetBytes;i++)
			{
				if(MCSTwrite((UINT32)0x01000000,32,(0xf2000000+cBuf[i]))==-1)
				{
					printf("Write Flash2 Error\n");
					return -1;
				}
				for(delaytime = 0; delaytime <1000 ;delaytime++);
			}
			if(MCSTwrite((UINT32)0x01000000,32,0xf2fcddb1)==-1)
			{
				printf("VMEWrite Error,for write Flash2!\n");
				return -1;
			}
			for(delaytime = 0; delaytime <1000 ;delaytime++);
			if(MCSTwrite((UINT32)0x01000000,32,0xf2000000)==-1)
			{
				printf("VMEWrite Error,for earse Flash2!\n");
				return -1;
			}
			taskDelay(1);
                }
	printf("write fpga2 end flag...\n");
	if(MCSTwrite((UINT32)0x01000000,32,0xf2fcdde1)==-1)
	{
		printf("VMEWrite Error,for write Flash2!\n");
		return -1;
	}
	taskDelay(1);
	if(MCSTwrite((UINT32)0x01000000,32,0xf2000000)==-1)
	{
		printf("VMEWrite Error,for earse Flash2!\n");
		return -1;
	}
	taskDelay(1);
	fclose(pReadFile);
	printf("write fpga2 success\n");
*/
}


