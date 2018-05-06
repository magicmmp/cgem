#include "5100_test.h"
#include "vxWorks.h"
#include "stdio.h"
#include "sysLib.h"
#include "semLib.h"
#include "taskLib.h"
#include "intLib.h"
#include "arch/ppc/ivPpc.h"

#include "fw_dcgvas.h"
#include "fw_define.h"
#include "fw_staterrspt.h"
#include "fw_configinfo.h"
#include "sd_emc.h"
#include "sd_emc_hw.h"

bool g_bEMCRocIntConed=FALSE;

bool uStopL1Flag=FALSE;

void EMCIntHandler(int nArg)
{
	sysIntDisable(EMC_INT_LEVEL);
	semGive(g_sidInterrupt);
	g_nInterruptCounter+=g_nTrgPerInt;
}

//hi,jak,
void EMCTCBAIntHandler(int nArg)
{
	sysIntDisable(EMC_TCBA_INT_LEVEL);
	semGive(g_sidTCBAInterrupt);
}
//bye,jak

/******only for EMCMonitor module ********/
void EMCSetQMonitorOutL1()
{
	if(1==g_EMCDBInfo.emc_monitor_sum)
	{
		WriteToVme32((0x180000+((g_EMCDBInfo.emc_monitor_header->geo )-3)*0x80000)+ 0x14, VA24, 1); /*set ext trig mode*/

		printf("*******EMCSetQMonitor Ext L1*************\n");
	}
}

void EMCSetQMonitorSelfL1()
{
	if(1==g_EMCDBInfo.emc_monitor_sum)
	{
		WriteToVme32((0x180000+((g_EMCDBInfo.emc_monitor_header->geo )-3)*0x80000)+ 0x10, VA24, 1); /* set self trig mode*/

		printf("*******EMCSetQMonitor Self L1*************\n");
	}
}

void EMCMaskQMonitor()
{
	if(1==g_EMCDBInfo.emc_monitor_sum)
	{
		WriteToVme32((0x180000+((g_EMCDBInfo.emc_monitor_header->geo )-3)*0x80000), VA24, 0);
		printf("*******EMCMaskQMonitor*************\n");
	}
}
/****************************/
	
void EMCSetQThreshold(int nModuleAddr, int nMdlIdx)
{
	unsigned err_info = 0;
	int kk;
	int yy=0;
	unsigned int uReadTmp;
	
	for(kk = 0x80; kk <= 0xfc; kk = kk + 4)
	{
		err_info += WriteToVme32(nModuleAddr + kk, VA24, g_EMCDBInfo.emc_Q_header[nMdlIdx]->QThreshold[yy]);
//		err_info += WriteToVme32(nModuleAddr + kk, VA24, 0);
		
		ReadFrmVme32(nModuleAddr + kk, VA24, &uReadTmp );
		if(uReadTmp!=g_EMCDBInfo.emc_Q_header[nMdlIdx]->QThreshold[yy])
		{
			//write QThreshold error
			printf("g_EMCDBInfo.emc_Q_header[%d]->QThreshold[%d],threashold error!\n",nMdlIdx,yy);
			printf("ReadRegister=0x%x,DBQThreshold=%d\n",uReadTmp,g_EMCDBInfo.emc_Q_header[nMdlIdx]->QThreshold[yy]);
			Push2ErrStack(PPCERROR+EMC_FLG+ATOM_ERR+47,0,0,0,0);		/*emc error 47*/
			return ;
		}
		yy++;
	}

/*	
	for(kk = 0x80; kk <= 0xfc; kk = kk + 16)
	{	
		err_info += WriteToVme32(nModuleAddr + kk, VA24, 2000);
		err_info += WriteToVme32(nModuleAddr + kk+4, VA24, 0);
		err_info += WriteToVme32(nModuleAddr + kk+8, VA24, 2000);
		err_info += WriteToVme32(nModuleAddr + kk+12, VA24, 0);
	}
*/	
	//err_info += WriteToVme32(nModuleAddr + 0x90, VA24, 2000);
	//err_info += WriteToVme32(nModuleAddr + 0x94, VA24, 2000);
}

void EMCConfigEachQ(int nModuleAddr, int nModuleType,int nMdlIdx,int nCBLTtype)
{
  	int err_info = 0;
			  /* ------------------------------------------
			  |  if n=4 first and also last module|
			  | if n=1 first module                    |
			  | if n=2 mid module                    |
			  | is n=3 last module                    |
			  ---------------------------------------------*/

  	err_info += WriteToVme32(nModuleAddr+ 0x00, VA24, 0x80); /*reset module, set work mode*/

	taskDelay(1);

	if(1==nCBLTtype)//CBLT enable
	{
	 	if(nModuleType == 1) err_info+= WriteToVme32(nModuleAddr+ 0x00, VA24, 0x53);
	  	if(nModuleType == 2) err_info += WriteToVme32(nModuleAddr+ 0x00, VA24, 0x51);
	  	if(nModuleType == 3) err_info += WriteToVme32(nModuleAddr+ 0x00, VA24, 0x55);
	  	if(nModuleType == 4) err_info+= WriteToVme32(nModuleAddr+ 0x00, VA24, 0x57);
	}
	else if(0==nCBLTtype)//CBLT disable
	{
	 	if(nModuleType == 1) err_info+= WriteToVme32(nModuleAddr+ 0x00, VA24, 0x52);
	  	if(nModuleType == 2) err_info += WriteToVme32(nModuleAddr+ 0x00, VA24, 0x50);
	  	if(nModuleType == 3) err_info += WriteToVme32(nModuleAddr+ 0x00, VA24, 0x54);
	  	if(nModuleType == 4) err_info+= WriteToVme32(nModuleAddr+ 0x00, VA24, 0x56);
	}
	else
	{
		printf("CBLT Type error\n");
	}


 	err_info += WriteToVme32(nModuleAddr + 0x04, VA24, g_nTrgPerInt); /*triger per int*/

 	EMCSetQThreshold(nModuleAddr,nMdlIdx);

	if (EMC_Q_SELF_TRIG_MODE == 1)
	{
		err_info += WriteToVme32(nModuleAddr+ 0x10, VA24, 1); /* set self trig mode*/
	}
	else
	{
		err_info += WriteToVme32(nModuleAddr + 0x14, VA24, 1); /*set ext trig mode*/
	}
	  
	if(err_info!=0)
  	{
		printf("config EMC Q module %d error\n",nMdlIdx);
		Push2ErrStack(PPCERROR+EMC_FLG+ATOM_ERR+48,0,0,0,0);		/*emc error 48*/
		return ;
  	}

}


int 	EMCConfigIntCBLT()
{
	int error_info;
	
	if(FALSE == g_bEMCRocIntConed) //only connect interrupt handler once,so here need some mem permanently
	{
		printf("int connect!\n");
		{
			if(intConnect (INUM_TO_IVEC(EMC_INT_VEC), (VOIDFUNCPTR)EMCIntHandler, EMC_INT_VEC) == ERROR)
		  	{
		     		DEBUG_OUTPUT(("interrupt handler connect error\n"));
		     		Push2ErrStack(PPCFATAL+EMC_FLG+ATOM_ERR+34,0,0,0,0);		/*emc error 34*/
				return  -1;
		  	}

			//hi,jak
			if(1==g_nTCheckAmpMode)
			{
				printf("*************EMCTCBAIntHandler************************\n");
				if(intConnect (INUM_TO_IVEC(EMC_TCBA_INT_VEC), (VOIDFUNCPTR)EMCTCBAIntHandler, EMC_TCBA_INT_VEC) == ERROR)
			  	{
			     		DEBUG_OUTPUT(("TCBA interrupt handler connect error\n"));
			     		Push2ErrStack(PPCFATAL+EMC_FLG+ATOM_ERR+72,0,0,0,0);		//emc error 72
					return  -1;
			  	}
			  }	
			//bye,jak	
			
		}
		 g_bEMCRocIntConed= TRUE ;
	}
	sysIntDisable(EMC_INT_LEVEL);	
	if(CBLTinit(32)==-1)
	{
		DEBUG_OUTPUT(("CBLT init error!\n"));
		Push2ErrStack(PPCERROR+EMC_FLG+ATOM_ERR+35,0,0,0,0);		/*emc error 35*/
		return -1;
	}
	return 0;
}

int 	EMCConfigQ()
{
	int i;
	int nMdlIdx;
	UINT uModuleAddr;
	int  nQMdlAmount;
	int kk;
	int idis = 0;
	/*------------------------------------------config EMC Q module--------------*/

	switch(g_nRunMode)
	{
		case NOMALMODE:
			nQMdlAmount=g_EMCDBInfo.emc_Q_sum;
			if(nQMdlAmount==1)
			{
				uModuleAddr=0x180000+((g_EMCDBInfo.emc_Q_header[0]->geo )-3)*0x80000;
				EMCConfigEachQ(uModuleAddr, 4,0,1);
			}
			else
			{
				for(i=0;i<nQMdlAmount;i++)
				{
					uModuleAddr=0x180000+((g_EMCDBInfo.emc_Q_header[i]->geo )-3)*0x80000;
					
					if(i==0)
					{
						EMCConfigEachQ(uModuleAddr, 1,i,1);
					}
					else if(i==(nQMdlAmount-1))
					{
						EMCConfigEachQ(uModuleAddr, 3,i,1);
					}
					else
					{
						EMCConfigEachQ(uModuleAddr, 2,i,1);
					}
				}
			}			
			EMCMaskQMonitor();	//normal mode . QMonitor module disabled
			/**for test**/
			/*
			if(0x64==g_nCrateID)
			{
				WriteToVme32(0x580000, VA24, 0);
				printf("*******EMC 0x64 GEO 0x580000 mask*************\n");
			}
			*/
			/*for disable Q module when necessary**/
			
			for(;idis<nDisableIdx;idis++ )
				WriteToVme32(QDisableGeo[idis], VA24, 0);
			break;
		case PEDMODE:
		case CALIBRATION:
		case BASELINE:
		case WAVEMODE:
			{
				nQMdlAmount=g_EMCDBInfo.emc_Q_sum;
				if(nQMdlAmount==1)
				{
					uModuleAddr=0x180000+((g_EMCDBInfo.emc_Q_header[0]->geo )-3)*0x80000;
					EMCConfigEachQ(uModuleAddr, 4,0,1);
				}
				else
				{
					for(i=0;i<nQMdlAmount;i++)
					{
						uModuleAddr=0x180000+((g_EMCDBInfo.emc_Q_header[i]->geo )-3)*0x80000;
						
						if(i==0)
						{
							EMCConfigEachQ(uModuleAddr, 1,i,1);
						}
						else if(i==(nQMdlAmount-1))
						{
							EMCConfigEachQ(uModuleAddr, 3,i,1);
						}
						else
						{
							EMCConfigEachQ(uModuleAddr, 2,i,1);
						}
					}
				}
			}		
			break;		
		case MONITOR:
			if((nMonitorStep>=0)&&(nMonitorStep<=7))
			{
//				if(0==g_EMCDBInfo.emc_monitor_sum)
				{
					nQMdlAmount=g_EMCDBInfo.emc_Q_sum;

					//all module disable CBLT in first 8 step
					for(i=0;i<nQMdlAmount;i++)
					{
						uModuleAddr=0x180000+((g_EMCDBInfo.emc_Q_header[i]->geo )-3)*0x80000;
						
						if(i==0)
						{
							EMCConfigEachQ(uModuleAddr, 1,i,0);
						}
						else if(i==(nQMdlAmount-1))
						{
							EMCConfigEachQ(uModuleAddr, 3,i,0);
						}
						else
						{
							EMCConfigEachQ(uModuleAddr, 2,i,0);
						}
					}
				}
//				else if(g_EMCDBInfo.emc_monitor_sum)
				if(g_EMCDBInfo.emc_monitor_sum)
				{
			
					//set CBLT chain
					uModuleAddr=0x180000+((g_EMCDBInfo.emc_monitor_header->geo )-3)*0x80000;
					
					//enable QMonitor CBLT,the only board in chain
					EMCConfigEachQ(uModuleAddr, 4,0,1);

					//set threshold.all high
					for(kk = 0x80; kk <= 0xfc; kk = kk + 4)
					{
						//WriteToVme32(uModuleAddr + kk, VA24, 2000);
						WriteToVme32(uModuleAddr + kk, VA24, 0);
					}
					printf("set all channel threshold to 0\n");
					//set two channal low threshold
					//WriteToVme32(uModuleAddr + (0x80+4*(nMonitorStep*2) ) , VA24, g_EMCDBInfo.emc_monitor_header->QThreshold[(nMonitorStep*2)]+10);
					//WriteToVme32(uModuleAddr + (0x80+4*(nMonitorStep*2+1) ) , VA24, g_EMCDBInfo.emc_monitor_header->QThreshold[(nMonitorStep*2+1)]+10);
					//test for no source jixl 091123
					//WriteToVme32(uModuleAddr + (0x80+4*(nMonitorStep*2) ) , VA24, 0);
					//WriteToVme32(uModuleAddr + (0x80+4*(nMonitorStep*2+1) ) , VA24, 0);		
					/*
					printf("over threshold channal are 0x%x and 0x%x ;QThreshold are 0x%x and 0x%x\n",
						uModuleAddr + (0x80+4*(nMonitorStep*2) ), uModuleAddr + (0x80+4*(nMonitorStep*2+1) ), 
						g_EMCDBInfo.emc_monitor_header->QThreshold[(nMonitorStep*2)]+10, g_EMCDBInfo.emc_monitor_header->QThreshold[(nMonitorStep*2+1)]+10);
						*/
					//printf("over threshold channal are 0x%x and 0x%x ;QThreshold are 0x%x and 0x%x\n",
					//	uModuleAddr + (0x80+4*(nMonitorStep*2) ), uModuleAddr + (0x80+4*(nMonitorStep*2+1) ), 
					//	0, 0);						
				}
			}
			else
			{
				nQMdlAmount=g_EMCDBInfo.emc_Q_sum;
				if(nQMdlAmount==1)
				{
					uModuleAddr=0x180000+((g_EMCDBInfo.emc_Q_header[0]->geo )-3)*0x80000;
					EMCConfigEachQ(uModuleAddr, 4,0,1);
				}
				else
				{
					for(i=0;i<nQMdlAmount;i++)
					{
						uModuleAddr=0x180000+((g_EMCDBInfo.emc_Q_header[i]->geo )-3)*0x80000;
						
						if(i==0)
						{
							EMCConfigEachQ(uModuleAddr, 1,i,1);
						}
						else if(i==(nQMdlAmount-1))
						{
							EMCConfigEachQ(uModuleAddr, 3,i,1);
						}
						else
						{
							EMCConfigEachQ(uModuleAddr, 2,i,1);
						}
					}
				}
			}		
			break;
		default:
			Push2ErrStack(PPCERROR+EMC_FLG+ATOM_ERR+8,0,0,0,0);			/*emc error 8*/
			return -1;
	}
		
	/*-----------------------------------------end-initial EMC Q module--------------*/
	return 0;
}


void EMCQClear()
{
	int i;
	int 		nQMdlAmount;
	UINT 	uModuleAddr;

	switch(g_nRunMode)
	{
		case NOMALMODE:
			nQMdlAmount=g_EMCDBInfo.emc_Q_sum;
			if(nQMdlAmount==1)
			{
				uModuleAddr=0x180000+((g_EMCDBInfo.emc_Q_header[0]->geo )-3)*0x80000;
				WriteToVme32(uModuleAddr, VA24, 0x80);	//reset
				taskDelay(2);
				WriteToVme32(uModuleAddr, VA24, 0x57);	//only one
				taskDelay(2);
			}
			else
			{
				for(i=0;i<nQMdlAmount;i++)
				{
					uModuleAddr=0x180000+((g_EMCDBInfo.emc_Q_header[i]->geo )-3)*0x80000;
					WriteToVme32(uModuleAddr, VA24, 0x80);	//reset
					taskDelay(2);
					if(i==0)
					{
						WriteToVme32(uModuleAddr, VA24, 0x53);
						taskDelay(2);
					}
					else if(i==(nQMdlAmount-1))
					{
						WriteToVme32(uModuleAddr, VA24, 0x55);
						taskDelay(2);
					}
					else
					{
						WriteToVme32(uModuleAddr, VA24, 0x51);
						taskDelay(2);
					}
				}
			}	
			break;
		case PEDMODE:
		case CALIBRATION:
		case BASELINE:
		case WAVEMODE:
			if(0==g_EMCDBInfo.emc_monitor_sum)
			{
				nQMdlAmount=g_EMCDBInfo.emc_Q_sum;
				if(nQMdlAmount==1)
				{
					uModuleAddr=0x180000+((g_EMCDBInfo.emc_Q_header[0]->geo )-3)*0x80000;
					WriteToVme32(uModuleAddr, VA24, 0x80);	//reset
					taskDelay(2);
					WriteToVme32(uModuleAddr, VA24, 0x57);	//only one
					taskDelay(2);
				}
				else
				{
					for(i=0;i<nQMdlAmount;i++)
					{
						uModuleAddr=0x180000+((g_EMCDBInfo.emc_Q_header[i]->geo )-3)*0x80000;
						WriteToVme32(uModuleAddr, VA24, 0x80);	//reset
						taskDelay(2);
						if(i==0)
						{
							WriteToVme32(uModuleAddr, VA24, 0x53);
							taskDelay(2);
						}
						else if(i==(nQMdlAmount-1))
						{
							WriteToVme32(uModuleAddr, VA24, 0x55);
							taskDelay(2);
						}
						else
						{
							WriteToVme32(uModuleAddr, VA24, 0x51);
							taskDelay(2);
						}
					}
				}		
			}
			else
			{
				nQMdlAmount=g_EMCDBInfo.emc_Q_sum;
				if(g_EMCDBInfo.emc_Q_sum==0)
				{
					uModuleAddr=0x180000+((g_EMCDBInfo.emc_monitor_header->geo )-3)*0x80000;
					WriteToVme32(uModuleAddr, VA24, 0x80);	//reset
					taskDelay(2);
					WriteToVme32(uModuleAddr, VA24, 0x57);	//only one
					taskDelay(2);
				}
				else
				{
					for(i=0;i<nQMdlAmount;i++)
					{
						uModuleAddr=0x180000+((g_EMCDBInfo.emc_Q_header[i]->geo )-3)*0x80000;
						WriteToVme32(uModuleAddr, VA24, 0x80);	//reset
						taskDelay(2);						
						if(i==0)
						{
							WriteToVme32(uModuleAddr, VA24, 0x53);
							taskDelay(2);
						}
						else
						{
							WriteToVme32(uModuleAddr, VA24, 0x51);
							taskDelay(2);
						}
					}
					uModuleAddr=0x180000+((g_EMCDBInfo.emc_monitor_header->geo )-3)*0x80000;
					WriteToVme32(uModuleAddr, VA24, 0x80);	//reset
					taskDelay(2);					
					WriteToVme32(uModuleAddr, VA24, 0x55);
					taskDelay(2);				
				}

			}
			break;
		case MONITOR:
			if(nMonitorStep<=7)
			{
				if(g_EMCDBInfo.emc_monitor_sum)
				{
					uModuleAddr=0x180000+((g_EMCDBInfo.emc_monitor_header->geo )-3)*0x80000;
					WriteToVme32(uModuleAddr, VA24, 0x80);	//reset
					taskDelay(2);
					WriteToVme32(uModuleAddr, VA24, 0x57);
					taskDelay(2);
				}	
			}
			else if(nMonitorStep>=8)
			{
				nQMdlAmount=g_EMCDBInfo.emc_Q_sum;
				if(nQMdlAmount==1)
				{
					uModuleAddr=0x180000+((g_EMCDBInfo.emc_Q_header[0]->geo )-3)*0x80000;
					WriteToVme32(uModuleAddr, VA24, 0x80);	//reset
					taskDelay(2);
					WriteToVme32(uModuleAddr, VA24, 0x57);	//only one
					taskDelay(2);
				}
				else
				{
					for(i=0;i<nQMdlAmount;i++)
					{
						uModuleAddr=0x180000+((g_EMCDBInfo.emc_Q_header[i]->geo )-3)*0x80000;
						WriteToVme32(uModuleAddr, VA24, 0x80);	//reset
						taskDelay(2);
						if(i==0)
						{
							WriteToVme32(uModuleAddr, VA24, 0x53);
							taskDelay(2);
						}
						else if(i==(nQMdlAmount-1))
						{
							WriteToVme32(uModuleAddr, VA24, 0x55);
							taskDelay(2);
						}
						else
						{
							WriteToVme32(uModuleAddr, VA24, 0x51);
							taskDelay(2);
						}
					}
				}		
			}
			break;
		default:
			Push2ErrStack(PPCERROR+EMC_FLG+ATOM_ERR+8,0,0,0,0);			/*emc error 8*/
			return -1;
	}
	
}


int	EMCConfigAmpGAIN()
{
	unsigned short nChInx;
	unsigned short nModuInx=0;
	
	unsigned short nCsrVal=0;
	unsigned short nGainValue=0;
	unsigned short nChannelID = 0;
	unsigned short nAMPID = 0;	
	
	for(nModuInx=0;nModuInx<g_EMCDBInfo.emc_Q_sum;nModuInx++)
	{
		for(nChInx=0;nChInx<32;nChInx++)	//every channel
		{
			nChannelID=(g_EMCDBInfo.emc_Q_header[nModuInx]->MainAmpGeo[nChInx])&0xffff;
			nAMPID= (g_EMCDBInfo.emc_Q_header[nModuInx]->MainAmpGeo[nChInx])>>16;

			switch(g_nRunMode)
			{
				case NOMALMODE:
					nGainValue = g_EMCDBInfo.emc_Q_header[nModuInx]->GainValue[nChInx];		
 					break;
				case MONITOR:
				case CALIBRATION:		
				case PEDMODE:
				case BASELINE:
				case WAVEMODE:
					nGainValue = 0;		
					break;
				default:
					Push2ErrStack(PPCERROR+EMC_FLG+ATOM_ERR+98,0,0,0,0);			/*emc error 98*/
					return -1;
			}		
			
			ReadFrmVme16(0x0F30000, VA24, &nCsrVal);	/*read register to control L1 send,protect to delay L1 send*/
			while((nCsrVal&0x4) !=0)
			{
				taskDelay(1);
				ReadFrmVme16(0x0F30000, VA24, &nCsrVal);	
			}

			if( (nAMPID& 0x10)==0)//gain0
			{
				WriteToVme16(0x0F30006, VA24, ((nAMPID&0xf)<<12)+( (nChannelID&0xf)<<8)+(nGainValue&0xff) );
//				printf("gain0:Q nModuInx = %d,Q nChInx = %d, set Amp(nAMPID=%d,nChannelID=%d)=%d\n",nModuInx,nChInx,nAMPID,nChannelID,nGainValue);
			}
			else//gain1
			{
				WriteToVme16(0x0F30008, VA24, ((nAMPID&0xf)<<12)+( (nChannelID&0xf)<<8)+(nGainValue&0xff) );
//				printf("gain1:Q nModuInx = %d,Q nChInx = %d, set Amp(nAMPID=%d,nChannelID=%d)=%d\n",nModuInx,nChInx,nAMPID,nChannelID,nGainValue);
			}

			taskDelay(1);

			ReadFrmVme16(0x0F30000, VA24, &nCsrVal);	
			while((nCsrVal&0x4) !=0)
			{
				taskDelay(1);
				ReadFrmVme16(0x0F30000, VA24, &nCsrVal);	
			}

		}
	}
	
	return 0;
}

int EMCSetRocSelfL1()
{
	WriteToVme16(0x0F30000, VA24, 0x8000); /*reset roc controler */
	taskDelay(5);
	WriteToVme16(0x0F30000, VA24, 0x10);		/*roc interrupt enable,selfL1*/
	taskDelay(5);
	return 0;
}
int EMCSetRocOutL1()
{
	WriteToVme16(0x0F30000, VA24, 0x8000); /*reset roc controler */
	taskDelay(5);
	WriteToVme16(0x0F30000, VA24, 0x11);		/*roc interrupt enable,outL1*/
	taskDelay(5);
}
int	EMCConfigRoc()
{
	if(g_EMCDBInfo.emc_roc_sum==1)
	{
		switch(g_nRunMode)
		{
			case NOMALMODE:
				{
					if(g_EMCDBInfo.emc_ppc_header->NormModeSelfL1)
					{
						//EMCSetRocSelfL1();//jixl071116
						EMCSetRocOutL1();
					}
					else
					{
						EMCSetRocOutL1();
					}
					taskDelay(5);
				}
				break ;
			case PEDMODE:
				{
						EMCSetRocOutL1();
				}
				break ;
			case CALIBRATION:
				{
					EMCSetRocSelfL1();
				}
				break;
			case MONITOR:
				{
					EMCSetRocOutL1();
				}
				break;
			case BASELINE:
			case WAVEMODE:
				break;
			default:
				DEBUG_OUTPUT(("unknown running mode\n"));
				Push2ErrStack(PPCERROR+EMC_FLG+ATOM_ERR+8,0,0,0,0);		/*emc error 8*/
				return -1;
		}
	}
	/*
	if(-1==EMCConfigAmpGAIN())
	{
		return -1;
	}
	*/
	return 0;
}

int EMCSetFanoutMoniMode()
{
	WriteToVme32(0xe80000, VA24, 0x8000);	/*reset fanout module*/
	taskDelay(5);
	WriteToVme32(0xe80000, VA24, 0x0);		/*set monitor mode */
	taskDelay(5);
	return 0;
}
int EMCSetFanoutHitMode()
{
	WriteToVme32(0xe80000, VA24, 0x8000);	/*reset fanout module*/
	taskDelay(5);
	WriteToVme32(0xe80000, VA24, 0x1);		/*set hit mode */
	taskDelay(5);
	return 0;
}

int 	EMCConfigFanout()
{
	//logMsg("before enable %s::%d\n", __FILE__, __LINE__,0,0,0,0);
	if(g_EMCDBInfo.emc_fanout_sum==1)
	{
		switch(g_nRunMode)
		{
			case NOMALMODE:
				{
					if(g_EMCDBInfo.emc_ppc_header->NormModeSelfL1)
					{
						EMCSetFanoutMoniMode();
					}
					else
					{
						EMCSetFanoutMoniMode();
					}
				}
				break;
			case PEDMODE:
				{
					//EMCSetFanoutHitMode();
					EMCSetFanoutMoniMode();
				}
				break ;
			case CALIBRATION:
			case MONITOR:
				{
					EMCSetFanoutMoniMode();
				}
				break;
			case BASELINE:
			case WAVEMODE:
				break;
			default:
				DEBUG_OUTPUT(("unknown running mode\n"));
				Push2ErrStack(PPCERROR+EMC_FLG+ATOM_ERR+8,0,0,0,0);		/*emc error 8*/
				return -1;
		}

	}
	return 0;

}

int 	EMCTcbaDCMReset()
{
	WriteToVme32(0xd00094, VA24, 0x1);//DCM reset
	taskDelay(1);
	return 0;
}

int	EMCConfigTcba()
{
	unsigned int uReadTmp=0;
	if(g_EMCDBInfo.emc_trgtcba_sum==1)
	{
		EMCTcbaDCMReset();//DCM reset

		WriteToVme32(0xd000fc, VA24, g_EMCDBInfo.emc_trgtcba_header->WinLen);
		taskDelay(1);
		ReadFrmVme32(0xd000fc, VA24, &uReadTmp);
		if(uReadTmp!=g_EMCDBInfo.emc_trgtcba_header->WinLen)
		{
			printf("tcba WinLen error:addr:0xd000fc,0x%08x\n",uReadTmp);
			Push2ErrStack(PPCERROR+EMC_FLG+ATOM_ERR+49,0,0,0,0);		/*emc error 49*/
			return -1;
		}
		
		WriteToVme32(0xd000f8, VA24, g_nTrgPerInt);//L1 count
		taskDelay(1);
		ReadFrmVme32(0xd000f8, VA24, &uReadTmp);
		if(uReadTmp!=g_nTrgPerInt)
		{
			printf("config TRIG error:addr:0xd000f8,0x%08x\n",uReadTmp);
			Push2ErrStack(PPCERROR+EMC_FLG+ATOM_ERR+50,0,0,0,0);		/*emc error 50*/
			return -1;			
		}
		WriteToVme32(0xd000e0, VA24, g_EMCDBInfo.emc_trgtcba_header->HL_Vth);//voltage thresh
		taskDelay(1);
		ReadFrmVme32(0xd000e0, VA24, &uReadTmp);
		if(uReadTmp!=g_EMCDBInfo.emc_trgtcba_header->HL_Vth)
		{
			printf("tcba HL_Vth error:addr:0xd000e0,0x%08x\n",uReadTmp);
			Push2ErrStack(PPCERROR+EMC_FLG+ATOM_ERR+51,0,0,0,0);		/*emc error 51*/
			return -1;
		}
		WriteToVme32(0xd00090, VA24, g_EMCDBInfo.emc_trgtcba_header->L1Sel);//out L1 0
		taskDelay(1);
		ReadFrmVme32(0xd00090, VA24, &uReadTmp);
		if(uReadTmp!=g_EMCDBInfo.emc_trgtcba_header->L1Sel)
		{
			printf("tcba L1Sel error:addr:0xd00090,0x%08x\n",uReadTmp);
			Push2ErrStack(PPCERROR+EMC_FLG+ATOM_ERR+52,0,0,0,0);		/*emc error 52*/
			return -1;
		}
		
		WriteToVme32(0xd0008c, VA24, g_EMCDBInfo.emc_trgtcba_header->IntEn);//interrupt disable
		taskDelay(1);
		ReadFrmVme32(0xd0008c, VA24, &uReadTmp);
		if(uReadTmp!=g_EMCDBInfo.emc_trgtcba_header->IntEn)
		{
			printf("tcba IntEn error:addr:0xd0008c,0x%08x\n",uReadTmp);
			Push2ErrStack(PPCERROR+EMC_FLG+ATOM_ERR+53,0,0,0,0);		/*emc error 53*/
			return -1;
		}
		for(int Idx=0;Idx<17;Idx++)
		{
			if((g_EMCDBInfo.emc_trgtcba_header->ChTimeCompens[Idx]&0xff) != 0xff)
			{
				WriteToVme32(0xd00048+4*Idx, VA24, g_EMCDBInfo.emc_trgtcba_header->ChTimeCompens[Idx]);
				taskDelay(1);
				ReadFrmVme32(0xd00048+4*Idx, VA24, &uReadTmp);
				if(uReadTmp!=g_EMCDBInfo.emc_trgtcba_header->ChTimeCompens[Idx])
				{
					printf("tcba ChTimeCompens error:addr:0x%08x,0x%08x\n",0xd00048+4*Idx,uReadTmp);
					Push2ErrStack(PPCERROR+EMC_FLG+ATOM_ERR+54,0,0,0,0);		/*emc error 54*/
					return -1;
				}
			}
		}
		for(Idx=17;Idx<30;Idx++)
		{
			if((g_EMCDBInfo.emc_trgtcba_header->ChTimeCompens[Idx]&0xff) != 0xff)
			{		
				WriteToVme32(0xd000a0+4*(Idx-17), VA24, g_EMCDBInfo.emc_trgtcba_header->ChTimeCompens[Idx]);
				taskDelay(1);
				ReadFrmVme32(0xd000a0+4*(Idx-17), VA24, &uReadTmp);
				if(uReadTmp!=g_EMCDBInfo.emc_trgtcba_header->ChTimeCompens[Idx])
				{
					printf("tcba ChTimeCompens error:addr:0x%08x,0x%08x\n",0xd000a0+4*(Idx-17),uReadTmp);
					Push2ErrStack(PPCERROR+EMC_FLG+ATOM_ERR+55,0,0,0,0);		/*emc error 55*/
					return -1;
				}
			}
		}
		EMCTcbaDCMReset();
	}
	return 0;
}

int	EMCConfigHW()
{

	if(-1==EMCSetRocSelfL1())
	{
		DEBUG_OUTPUT(("config Fanout module error\n")) ;
		Push2ErrStack(PPCERROR+EMC_FLG+ATOM_ERR+56,0,0,0,0);		//emc error 56
		return -1;
	}
/*	
	if(-1==EMCPreAmpPlus())
	{
		DEBUG_OUTPUT(("config AmpPlus error\n")) ;
		Push2ErrStack(PPCERROR+EMC_FLG+ATOM_ERR+57,0,0,0,0);		//emc error 57
		return -1;
	}
*/	

	if(g_nRunMode!=MONITOR)
	{
		if(-1==EMCConfigAmpGAIN())
		{
			DEBUG_OUTPUT(("config AmpGain error\n")) ;
			Push2ErrStack(PPCERROR+EMC_FLG+ATOM_ERR+57,0,0,0,0);		//emc error 57
			return -1;
		}
		printf("EMCConfigAmpGAIN\n");
	}
	
	
 	if(-1==EMCConfigFanout())
	{
		DEBUG_OUTPUT(("config Fanout module error\n")) ;
		Push2ErrStack(PPCERROR+EMC_FLG+ATOM_ERR+58,0,0,0,0);		/*emc error 58*/
		return -1;
	}

	if(-1==EMCConfigQ())
	{
		DEBUG_OUTPUT(("config Q module error\n")) ;
		Push2ErrStack(PPCERROR+EMC_FLG+ATOM_ERR+59,0,0,0,0);		/*emc error 59*/
		return -1;
	}

	if(-1==EMCConfigRoc())
	{
		DEBUG_OUTPUT(("config ROC module error\n")) ;
		Push2ErrStack(PPCERROR+EMC_FLG+ATOM_ERR+60,0,0,0,0);		/*emc error 60*/
		return -1;
	}

	if(-1==EMCConfigTcba())
	{
		DEBUG_OUTPUT(("config emc trgtcba module error\n")) ;
		Push2ErrStack(PPCERROR+EMC_FLG+ATOM_ERR+61,0,0,0,0);		/*emc error 61*/
		return -1;
	}
	
	return 0;
}

int	EMCClearEvents()
{
	if(-1==EMCConfigRoc())
	{
		DEBUG_OUTPUT(("config ROC module error\n")) ;
		Push2ErrStack(PPCERROR+EMC_FLG+ATOM_ERR+62,0,0,0,0);		/*emc error 62*/
		return -1;
	}
	if(-1==EMCConfigFanout())
	{
		DEBUG_OUTPUT(("config Fanout module error\n")) ;
		Push2ErrStack(PPCERROR+EMC_FLG+ATOM_ERR+63,0,0,0,0);		/*emc error 63*/
		return -1;
	}
	//call clear just clear event in the module
	EMCQClear();	
	
	return 0;
}


void EMC_ROC_CnstL1(unsigned int OnceGen)//for constant L1,or continuous L1
{
	unsigned short nCaTmp=0;
//	uStopL1Flag=false;
	while(!uStopL1Flag)
	{
		ReadFrmVme16(0x0F30000, VA24, &nCaTmp);	
		while((nCaTmp&0xa) !=0)
		{
			taskDelay(1);
			ReadFrmVme16(0x0F30000, VA24, &nCaTmp);
		}
		WriteToVme16(0x0F30002, VA24, 30000);   //DAC value
		WriteToVme16(0x0F30004, VA24, 0x300+OnceGen);	
	}
}


void EMC_Fanout_CnstL1(unsigned int nOnceGen)//for constant L1,or continuous L1
{
	unsigned nMoTmp=0;
//	uStopL1Flag=false;
	while(!uStopL1Flag)
	{
		ReadFrmVme32(0xe80000, VA24,&nMoTmp);	
		while((nMoTmp&0xc) !=0)
		{
			taskDelay(1);
			ReadFrmVme32(0xe80000, VA24,&nMoTmp);
		}
		WriteToVme32(0xe80004, VA24, 0); //dac
		WriteToVme32(0xe80008, VA24,nOnceGen);	
		//taskDelay(120);
	}
}

void EMCGenL1()
{
	unsigned short nCaTmp;
	unsigned int 	 nMoTmp;
	int 			emc_cal;
	int			nTimeIdx=0;
	int			nPointIdx=0;
	
	semTake(g_sidSATR, WAIT_FOREVER) ;
	uStopL1Flag=FALSE;
	switch(g_nRunMode)
	{
		case NOMALMODE:
			if(g_EMCDBInfo.emc_ppc_header->NormModeSelfL1)
			{
				if(g_EMCDBInfo.emc_fanout_sum==1)
				{
					EMC_Fanout_CnstL1(30);  //jixl071116
				}
			}
			break;
		case PEDMODE:
			if(g_EMCDBInfo.emc_fanout_sum==1)
			{
				EMC_Fanout_CnstL1(1);  //leigk071127
			}
			break ;
		case CALIBRATION:
			emc_cal=0x300+1;
			while(1)
			{
				for(nTimeIdx=1;nTimeIdx<=g_nL1Count;nTimeIdx++)
				{
					ReadFrmVme16(0x0F30000, VA24, &nCaTmp);	/*read register to control L1 send,protect to delay L1 send*/
					while((nCaTmp&0xa) !=0)
					{
						ReadFrmVme16(0x0F30000, VA24, &nCaTmp);
					}
				
					WriteToVme16(0x0F30002, VA24, g_nEmcDAC);
					WriteToVme16(0x0F30004, VA24, emc_cal);	
					if(!(nTimeIdx%10))taskDelay(1);
				}
				nPointIdx ++ ;
				printf("Calib %d Point done.\n", nPointIdx) ;
				
				semTake(g_sidSATR, WAIT_FOREVER) ;				
				if(g_bTrgStop)
				{
					break;
				}
				
			}
			break;
		case MONITOR:
			if(nMonitorStep<=7)
			{
				EMCSetQMonitorSelfL1();
				
				while(1)
				{
					if(g_bTrgStop)
					{
						EMCSetQMonitorOutL1();
						
						break;
					}
				}
				nMonitorStep++;
			}

			else if(nMonitorStep>=8)
			{			
				while(1)
				{
					if(g_EMCDBInfo.emc_fanout_sum==1)
					{
						if( (nPointIdx%10)==0 )
//						if( (nPointIdx%((int)((g_nCalibMoni_Points-8)/10)))==0 )
						{
							emc_cal=5;
					
							WriteToVme32(0xe80000, VA24, 0x8000);	/*reset fanout module*/
							taskDelay(5);
							WriteToVme32(0xe80000, VA24, (0x6000+(0x100*nLEDCount) ));
							
							printf("nLEDCount = %d, value to CSR = 0x%x\n",nLEDCount,(0x6000+(0x100*nLEDCount) ));
							printf("nMonitorStep = %d,g_nEmcDAC = %d, g_nL1Count = %d\n",nMonitorStep,g_nEmcDAC,g_nL1Count);
							taskDelay(1);	
						}
						for(nTimeIdx=1;nTimeIdx<=(g_nL1Count/emc_cal);nTimeIdx++)
						{
							ReadFrmVme32(0xe80000, VA24,&nMoTmp);	/*read register to control L1 send,protect to delay L1 send*/
							while((nMoTmp&0xc) !=0)
							{
								ReadFrmVme32(0xe80000, VA24,&nMoTmp);
							}
							WriteToVme32(0xe80004, VA24, g_nEmcDAC);
							WriteToVme32(0xe80008, VA24, emc_cal);	
							// if(!(nTimeIdx%10))taskDelay(1);
							taskDelay(1);
						}
					}
					nPointIdx ++ ;
					printf("EMC Monitor part2 %d Point done.\n", nPointIdx) ;

					if( (nPointIdx%10)==0 )
//					if( (nPointIdx%((int)((g_nCalibMoni_Points-8)/10)))==0 )
					{
						nLEDCount++;
						printf("**********NOW LEDCount = %d*********\n",nLEDCount);
					}
					
					semTake(g_sidSATR, WAIT_FOREVER) ;				
					if(g_bTrgStop)
					{
						break;
					}
				}
			}
			
			break;
		case BASELINE:
			if(g_EMCDBInfo.emc_fanout_sum==1)
			{
				EMC_Fanout_CnstL1(16);  //leigk
			}
			break;
		case WAVEMODE:
			break;
		default:
			DEBUG_OUTPUT(("unknown running mode\n"));
			Push2ErrStack(PPCERROR+EMC_FLG+ATOM_ERR+8,0,0,0,0);		/*emc error 8*/
			break ;
	}
	
	nEMCGenL1TaskId=0; //ensure not delete task which have exit normally
}

void StopGenL1()
{
	uStopL1Flag=true;
}

/***********simple function, no map*************/
int EMCPreAmpPlus() 
{
	int 		    chIndx,bdIndx;
	unsigned short nCsrVal;
	unsigned addr;
	
	unsigned short uModAddr = 0;
	unsigned short uValue=150; //value 1-255,commonly set >10 080125

	for(bdIndx=0;bdIndx<30;bdIndx++)
	{
		if(bdIndx<=15 )
		{
			uModAddr = bdIndx;
			addr = 0x0F30006;	//gain0
		}
		else
		{
			uModAddr = bdIndx -16;
			addr = 0x0F30008;	//gain1
		}
		chIndx=0;		
		
		while(chIndx<16)
		{
			ReadFrmVme16(0x0F30000, VA24, &nCsrVal);	/*read register to control L1 send,protect to delay L1 send*/
			if((nCsrVal&0x4) ==0)
			{
//				printf("addr=0x%x,data=0x%x\n",addr,(uModAddr<<12)+( (chIndx&0xf)<<8)+  (uValue&0xff));
				WriteToVme16(addr, VA24, (uModAddr<<12)+( (chIndx&0xf)<<8)+  (uValue&0xff) );	
				taskDelay(1);
				chIndx++;
			}
		}
	}
}


int testReadVME(unsigned int ndelay)
{
	unsigned short nCaTmp;
	while(1)
	{
		if(ndelay > 0)
			taskDelay(ndelay);
		ReadFrmVme16(0x0F30000, VA24, &nCaTmp);
	}
}




