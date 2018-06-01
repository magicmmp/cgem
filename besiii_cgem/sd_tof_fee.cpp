/*sd_empty.cpp, test an empty system*/

#include "vxWorks.h"
#include "stdio.h"
#include "stdio.h"
#include "intLib.h"

#include "5100_test.h"
#include "fw_roppc.h"
#include "fw_dcgvas.h"
#include "fw_define.h"
#include "fw_configinfo.h"
#include "fw_staterrspt.h"

#include "sd_tof.h"
#include "sd_tof_fee.h"
#include "sd_tof_hptdc_config.h"


int TOFConfigFeeBaseAddr()
{
	UINT nGeo;
	UINT nadr;
	UINT uRead;
	
	//tof fee use csr space to config A32D32 space on each board,for cblt or other control
	for(int nModuIndx=0;nModuIndx<g_TOFDBInfo.tof_FEE_sum;nModuIndx++)			//set base addr ,use a24d32
	{
		nGeo=g_TOFDBInfo.tof_FEE_header[nModuIndx]->geo;
		WriteToVme8((nGeo<<19)+0x7ffd8+0x3, VCSR, (nGeo&0x0f)<<4);  //nGeo low 4 bit write to one register
		WriteToVme8((nGeo<<19)+0x7ffdc+0x3, VCSR, (nGeo&0xf0)>>4);	//nGeo high 4 bit write to another register
		//printf("write:0x%08x to address: 0x%08x\n",nGeo<<4,(nGeo<<19)+0x7ffdc+0x3);
	}

	for(nModuIndx=0;nModuIndx<g_TOFDBInfo.tof_FEE_sum;nModuIndx ++)//add for check FEE in slot wether match  config infor
	{
			nGeo=g_TOFDBInfo.tof_FEE_header[nModuIndx]->geo;
			nadr=(nGeo<<20);									//after set fee base addr,use a32d32 access		
			uRead=0;
			ReadFrmVme32(nadr,VA32,&uRead);
			if(uRead==0xffffffff)
			{
				printf("Config FEE BaseAddr error, Slot %d may have no module,pls chk config info\n",nGeo);
				Push2ErrStack(PPCFATAL+TOF_FLG+ATOM_ERR+26,0,0,0,0);		/*tof error 26*/
				//return -1;
			}
	}
	return 0;
}

int TOFCheckFeeSerialNum()
{
	unsigned int SerialRead=0;
	for(int nIndx=0;nIndx<g_TOFDBInfo.tof_FEE_sum;nIndx ++)
	{
			unsigned int nGeo=g_TOFDBInfo.tof_FEE_header[nIndx]->geo;
			unsigned int nadr=(nGeo<<20);
			SerialRead=0;
			//read SerialRead  Board ID
			ReadFrmVme32(nadr+0x28,VA32,&SerialRead);
			SerialRead=SerialRead&0x3f;
			if(SerialRead!=g_TOFDBInfo.tof_FEE_header[nIndx]->serialNum)
			{
				//printf("BoardID:%d, ID in DB: %d\n",SerialRead,g_TOFDBInfo.tof_FEE_header[nIndx]->serialNum);
			}
	}
	return 0;
}

int TOFConfigFeeHPTDC()
{
	UINT	chanlEnable;
	UINT	trgEnable;	
	UINT	rejectLaten; 
	UINT	trgLatency;
	UINT	trgSearchWinT;
	UINT	trgMatchWinT;
	UINT	trgSearchWinQT;
	UINT	trgMatchWinQT;
	UINT	hitCntPerEvtT;
	UINT	hitCntPerEvtQT;
	
	for(int nIndx=0;nIndx<g_TOFDBInfo.tof_FEE_sum;nIndx ++)
	{
		UINT 	nGeo=g_TOFDBInfo.tof_FEE_header[nIndx]->geo;
		UINT 	nadr=(nGeo<<20);

		chanlEnable=g_TOFDBInfo.tof_FEE_header[nIndx]->chanlEnable;
		trgEnable=g_TOFDBInfo.tof_FEE_header[nIndx]->trgEnable;	
		switch(g_nRunMode)
		{
			case NOMALMODE:
			{
				rejectLaten=4096-g_TOFDBInfo.tof_FEE_header[nIndx]->rejectLaten; //act here we trans reject counter to function
				trgLatency=4096-g_TOFDBInfo.tof_FEE_header[nIndx]->trgLatency;
				trgSearchWinT=g_TOFDBInfo.tof_FEE_header[nIndx]->trgSearchWinT;
				trgMatchWinT=g_TOFDBInfo.tof_FEE_header[nIndx]->trgMatchWinT;
				trgSearchWinQT=g_TOFDBInfo.tof_FEE_header[nIndx]->trgSearchWinQT;
				trgMatchWinQT=g_TOFDBInfo.tof_FEE_header[nIndx]->trgMatchWinQT;
				hitCntPerEvtT=g_TOFDBInfo.tof_FEE_header[nIndx]->hitCntPerEvtT;
				hitCntPerEvtQT=g_TOFDBInfo.tof_FEE_header[nIndx]->hitCntPerEvtQT;
				break;
			}
			case CALIBRATION:
			case PEDMODE:
			case BASELINE:
			case WAVEMODE:
			{
				rejectLaten=4096-182; 
				trgLatency =4096-180;
				trgSearchWinT=100;
				trgMatchWinT=100;
				trgSearchWinQT=100;
				trgMatchWinQT=100;
				hitCntPerEvtT=9;
				hitCntPerEvtQT=9;	
				break;
			}
			case MONITOR:
			{
				rejectLaten=4096-142; 
				trgLatency=4096-140;
				trgSearchWinT=40;
				trgMatchWinT=40;
				trgSearchWinQT=40;
				trgMatchWinQT=40;
				hitCntPerEvtT=9;
				hitCntPerEvtQT=9;	
				break;
			}
			
		}
		if(FeeHptdcConfig(nadr,chanlEnable,trgEnable,rejectLaten,trgLatency,trgSearchWinT
								,trgMatchWinT,trgSearchWinQT,trgMatchWinQT,hitCntPerEvtT,hitCntPerEvtQT)== -1)
		{
			DEBUG_OUTPUT(("FeeHptdcConfig Failed !\n"));
			Push2ErrStack(PPCFATAL+TOF_FLG+ATOM_ERR+27,0,0,0,0);		/*tof error 27*/
			return -1;
		}
		//printf("after config %d slot fee hptdc\n",nGeo);
		taskDelay(60);//we'd better to delay enough time to make the hptdc working
	}
	return 0;
}


/* Configure each fee & monitor information */
int TOFConfigEachDAC(UINT32 InitAdrs,UINT32 HighThresh,UINT32 LowThresh,UINT32 ConstVoltage
								,UINT32 SelfTestSignAmp,UINT32 QTLowThresh,UINT32 QTHighThresh)  
{
	UINT32 InitData0 = 0x20;
	
	//printf("\n InitAdrs=0x%08x\n",InitAdrs);
	//printf("VME:: Begin Init  Board DAC\n");

	/*step 0 */
	if ( -1 == WriteToVme32(InitAdrs, VA32, InitData0) ) 
	{
		//write InitData to InitAdrs Fail
		printf("write 0x%08x to 0x%08x fail \n",InitData0, InitAdrs );
		Push2ErrStack(PPCFATAL+TOF_FLG+ATOM_ERR+28,0,0,0,0);		/*tof error 28*/
		return -1;
	}
	else
	{
		//printf(">>write 0x%08x to 0x%08x success \n", InitData0,InitAdrs);
	}
	taskDelay(1);
	
	/*set threshold 1*/
	if ( -1 == WriteToVme32(InitAdrs+0x40000, VA32, LowThresh) )  
	{
		//write LowThresh Fail
		printf("write LowThresh       0x%08x to 0x%08x fail \n",LowThresh,InitAdrs+0x40000);
		Push2ErrStack(PPCFATAL+TOF_FLG+ATOM_ERR+29,0,0,0,0);		/*tof error 29*/
		return -1;
	}
	else
	{
		//printf(">>write LowThresh       0x%08x to 0x%08x success \n", LowThresh,InitAdrs+0x40000);
	}
	taskDelay(1);
	/*set threshold 2*/
	if ( -1 == WriteToVme32(InitAdrs+0x40004, VA32, HighThresh) ) 
	{
		//write HighThresh Fail
		printf("write HighThresh      0x%08x to 0x%08x fail \n",HighThresh,InitAdrs+0x40004);
		Push2ErrStack(PPCFATAL+TOF_FLG+ATOM_ERR+30,0,0,0,0);		/*tof error 30*/
		return -1;
	} 
	else 
	{
		//printf(">>write HighThresh      0x%08x to 0x%08x success \n", HighThresh,InitAdrs+0x40004);
	}
	taskDelay(1);
	/*set */
	if ( -1 == WriteToVme32(InitAdrs+0x40008, VA32,ConstVoltage) )  
	{
		//write Constvoltage Fail
		printf("write Constvoltage:   0x%08x to 0x%08x fail \n",ConstVoltage,InitAdrs+0x40008);
		Push2ErrStack(PPCFATAL+TOF_FLG+ATOM_ERR+31,0,0,0,0);		/*tof error 31*/
		return -1;
	} 
	else 
	{
		//printf(">>write Constvoltage    0x%08x to 0x%08x success \n",ConstVoltage,InitAdrs+0x40008);
	}

	taskDelay(1);
	
	/*set threshold 4*/
	if ( -1 == WriteToVme32(InitAdrs+0x4000c, VA32, SelfTestSignAmp) )  
	{
		//write SelfTestSignAmp Fail
		printf("write SelfTestSignAmp 0x%08x to 0x%08x fail \n",SelfTestSignAmp,InitAdrs+0x4000c);
		Push2ErrStack(PPCFATAL+TOF_FLG+ATOM_ERR+32,0,0,0,0);		/*tof error 32*/
		return -1;
	} 
	else 
	{
		//printf(">>write SelfTestSignAmp 0x%08x to 0x%08x success \n",SelfTestSignAmp,InitAdrs+0x4000c);
	}
	taskDelay(1);
	/*set*/
	if ( -1 == WriteToVme32(InitAdrs+0x40010, VA32, QTLowThresh) )  
	{
		//write QTLowThresh Fail
		printf("write QTLowThresh     0x%08x to 0x%08x fail \n", QTLowThresh,InitAdrs+0x40010);
		Push2ErrStack(PPCFATAL+TOF_FLG+ATOM_ERR+33,0,0,0,0);		/*tof error 33*/
		return -1;
	} 
	else 
	{
		//printf(">>write QTLowThresh     0x%08x to 0x%08x success \n", QTLowThresh,InitAdrs+0x40010);
	}
	taskDelay(1);
	/*set*/

	if ( -1 == WriteToVme32(InitAdrs+0x40014, VA32, QTHighThresh) )  
	{
		//write QTHighThresh Fail
		printf("write QTHighThresh    0x%08x to 0x%08x fail \n", QTHighThresh,InitAdrs+0x40014);
		Push2ErrStack(PPCFATAL+TOF_FLG+ATOM_ERR+34,0,0,0,0);		/*tof error 34*/
		return -1;
	} 
	else 
	{
		//printf(">>write QTHighThresh    0x%08x to 0x%08x success \n", QTHighThresh,InitAdrs+0x40014);
	}
	taskDelay(1);
	return 0;
}

//call Config each DAC
int TOFConfigFeeDAC()
{
	for(int nModuIndx=0;nModuIndx<g_TOFDBInfo.tof_FEE_sum;nModuIndx++)  /* configure L1 num for each FEE Board */
	{   
			UINT nGeo=g_TOFDBInfo.tof_FEE_header[nModuIndx]->geo;
			UINT nadr=(nGeo<<20);
			//set n trigger/interrupt of FEE
			if(-1== WriteToVme32((nadr+0xc),VA32,g_nTrgPerInt))
			{
				//write g_nTrgPerInt Fail
				printf(">>VME:: Slot %02d : Write 0x%08x to 0x%08x ->ERROR!\n",nGeo,g_nTrgPerInt,(nadr+0xc));
				Push2ErrStack(PPCFATAL+TOF_FLG+ATOM_ERR+35,0,0,0,0);		/*tof error 35*/
				return -1;
			}
			//config other register
			UINT	lowThresh=g_TOFDBInfo.tof_FEE_header[nModuIndx]->lowThresh;
					lowThresh=lowThresh*2*4096/2500;
			UINT	highThresh=g_TOFDBInfo.tof_FEE_header[nModuIndx]->highThresh;
					highThresh=highThresh*2*4096/2500;
					
			UINT	lowThreshQT=g_TOFDBInfo.tof_FEE_header[nModuIndx]->lowThreshQT;
					lowThreshQT=lowThreshQT*4096/2500;
			UINT	highThreshQT=g_TOFDBInfo.tof_FEE_header[nModuIndx]->highThreshQT;
					highThreshQT=highThreshQT*4096/(2500*2);
					
			UINT	constVoltage=g_TOFDBInfo.tof_FEE_header[nModuIndx]->constVoltage;
					constVoltage=constVoltage*(4096*2)/(2500*3);
					
			UINT	selfTestSignAmp;
			switch(g_nRunMode)
			{
				case NOMALMODE:
				case PEDMODE:					
					selfTestSignAmp=g_TOFDBInfo.tof_FEE_header[nModuIndx]->selfTestSignAmp;
					selfTestSignAmp=selfTestSignAmp*4096/(2500*2);				
					break;
				case MONITOR:
					selfTestSignAmp=g_TOFDBInfo.tof_FEE_header[nModuIndx]->selfTestSignAmp;
					selfTestSignAmp=selfTestSignAmp*4096/(2500*2);
					/**** temp test:in monitor mode, threshold **************/
					if((1==g_TOFDBInfo.tof_FEE_header[nModuIndx]->barrel_end)&&(MONITOR==g_nRunMode))
					{
						highThresh = 50;
						highThresh=highThresh*2*4096/2500;
						lowThresh = 25;
						lowThresh=lowThresh*2*4096/2500;
					}
					break;
				case CALIBRATION:
					selfTestSignAmp=g_uTofCalibSignAmp;
					selfTestSignAmp=selfTestSignAmp*4096/(2500*2);
					/****in calib mode, threshold **************/
					lowThresh = 50;
					lowThresh=lowThresh*2*4096/2500;
					highThresh = 200;
					highThresh=highThresh*2*4096/2500;
					break;
				case BASELINE:
					break;
				case WAVEMODE:		
					break ;
				default:
					Push2ErrStack(PPCFATAL+TOF_FLG+ATOM_ERR+9,0,0,0,0);		/*tof error 9*/
					break ;
			}
			if(selfTestSignAmp>3200)
			{
				printf("sign Amp valtage val>3200\n");
				Push2ErrStack(PPCFATAL+TOF_FLG+ATOM_ERR+37,0,0,0,0);		/*tof error 37*/
				return -1;
			}
			//InitAdrs,HighThresh,LowThresh,ConstVoltage,SelfTestSignAmp,QTLowThresh,QTHighThresh)
			if(TOFConfigEachDAC(nadr,highThresh,lowThresh,constVoltage,selfTestSignAmp,lowThreshQT,highThreshQT)== -1)
			{
				DEBUG_OUTPUT(("TOFConfigEachDAC Failed 2!\n"));
				Push2ErrStack(PPCFATAL+TOF_FLG+ATOM_ERR+38,0,0,0,0);		/*tof error 38*/
				return -1;
			}
			//printf("after config %d slot fee DAC\n",nGeo);
	}
	return 0;
}

//config INl table
int TOFConfigFeeInlTable()
{
	for(int nModuIndx=0;nModuIndx<g_TOFDBInfo.tof_FEE_sum;nModuIndx++)  /* configure L1 num for each FEE Board */
	{   
			UINT nGeo=g_TOFDBInfo.tof_FEE_header[nModuIndx]->geo;
			UINT nadr=(nGeo<<20);
			char * cpInl=g_TOFDBInfo.tof_FEE_header[nModuIndx]->feeINL;
			if(VME_INLConfig(nadr,cpInl,16)==-1)			// 16 channel
			{
				DEBUG_OUTPUT(("VME_INLConfig Failed !\n"));
				Push2ErrStack(PPCFATAL+TOF_FLG+ATOM_ERR+39,0,0,0,0);		/*tof error 39*/
				return -1;
			}
			//printf("after config %d slot fee INL table\n",nGeo);
	}
	return 0;
}

int  TOFConfigFeeWorkMode()
{
	//workmode==0 : Run mode ;workmode==1 Self Test Mode
	UINT32 nadr;
	UINT32 FeeConfigureDataTmp;

	for(int nModuIndx=0;nModuIndx<g_TOFDBInfo.tof_FEE_sum;nModuIndx++)  
	{
		UINT nGeo=g_TOFDBInfo.tof_FEE_header[nModuIndx]->geo;
		nadr=(nGeo<<20);

		switch(g_nRunMode)
		{
			case NOMALMODE:
				if(g_TOFDBInfo.tof_ppc_header->NormModeSelfL1)
				{
					FeeConfigureDataTmp=0x84440;
				}
				else
				{
					if(g_TOFDBInfo.tof_FEE_header[nModuIndx]->barrel_end==0)
					{
						FeeConfigureDataTmp=0xc440;//barrel	//no INL:0xc400;INL:0xc440
					}
					else if(g_TOFDBInfo.tof_FEE_header[nModuIndx]->barrel_end==1)
					{
						FeeConfigureDataTmp=0x4440;//endcap	//no INL:0x4400;INL:0x4440
					}
					else
					{
						printf("distinguish barrel_end error\n");
						Push2ErrStack(PPCFATAL+TOF_FLG+ATOM_ERR+40,0,0,0,0);		/*tof error 40*/
						return -1;
					}
				}
				break;
			case PEDMODE:
			case MONITOR:
				if(g_TOFDBInfo.tof_FEE_header[nModuIndx]->barrel_end==0)
				{
					FeeConfigureDataTmp=0xc440;//barrel
				}
				else if(g_TOFDBInfo.tof_FEE_header[nModuIndx]->barrel_end==1)
				{
					FeeConfigureDataTmp=0x4440;//end_cap
				}
				else
				{
					printf("distinguish barrel_end error\n");
					Push2ErrStack(PPCFATAL+TOF_FLG+ATOM_ERR+41,0,0,0,0);		/*tof error 41*/
					return -1;
				}
				break;
			case CALIBRATION:
				FeeConfigureDataTmp=0x84440;
				break;
			case BASELINE:
			case WAVEMODE:		
				break ;
			default:
				Push2ErrStack(PPCFATAL+TOF_FLG+ATOM_ERR+9,0,0,0,0);		/*tof error 9*/
				break ;
		}
	
		if (-1 == WriteToVme32(nadr, VA32, FeeConfigureDataTmp)) 
		{
			printf(">>VME:: ERROR!! VA32 Write 0x%08x to 0x%08x with Errors!\n",FeeConfigureDataTmp,nadr); 
		} 
		else 
		{
			//printf(">>VME:: Data Pack Mode : VA32 Write 0x%08x to 0x%08x Successfully!\n",FeeConfigureDataTmp,nadr); 
		}
		taskDelay(1);
	}
	return 0;
}



