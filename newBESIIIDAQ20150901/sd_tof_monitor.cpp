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
#include "sd_tof_monitor.h"
#include "sd_tof_fee.h"
#include "sd_tof_hptdc_config.h"


int TOFConfigMoniBaseAddr()
{
	//config monitor
	if(g_TOFDBInfo.tof_monitor_sum==1)
	{
		WriteToVme8((g_uTofMonitorGeo<<19)+0x7ffd8+0x3, VCSR, (g_uTofMonitorGeo&0x0f)<<4);  //nGeo low 4 bit write to one register
		WriteToVme8((g_uTofMonitorGeo<<19)+0x7ffdc+0x3, VCSR, (g_uTofMonitorGeo&0xf0)>>4);	//nGeo high 4 bit write to another register
		//printf("write:0x%08x to address: 0x%08x\n",g_uTofMonitorGeo<<4,(g_uTofMonitorGeo<<19)+0x7ffdc+0x3);
	}
	return 0;
}

int TOFCheckMoniSerialNum()
{
	unsigned int SerialRead=0;
	//check monitor ?
	if(g_TOFDBInfo.tof_monitor_sum==1)
	{
		unsigned int nadr=(g_uTofMonitorGeo<<20);
			
		//read SerialRead  Board ID
		ReadFrmVme32(nadr+0x28,VA32,&SerialRead);
		SerialRead=SerialRead&0x3f;
		
		if(SerialRead!=g_TOFDBInfo.tof_monitor_header->serialNum)
		{
			//printf("BoardID:%d, ID in DB: %d\n",SerialRead,g_TOFDBInfo.tof_monitor_header->serialNum);
		}
	}
	return 0;
}

int TOFConfigMoniHPTDC()
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
	
	if(g_TOFDBInfo.tof_monitor_sum==1)
	{
		UINT 	nadr=(g_uTofMonitorGeo<<20);
		
		chanlEnable=g_TOFDBInfo.tof_monitor_header->chanlEnable;
		trgEnable=g_TOFDBInfo.tof_monitor_header->trgEnable;	

		switch(g_nRunMode)
		{
			case NOMALMODE:
			{
				rejectLaten=4096-142; 
				trgLatency=4096-140;
				trgSearchWinT=20;
				trgMatchWinT=20;
				trgSearchWinQT=40;
				trgMatchWinQT=40;
				hitCntPerEvtT=9;
				hitCntPerEvtQT=9;
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
				rejectLaten=4096-g_TOFDBInfo.tof_monitor_header->rejectLaten; //act here we trans reject counter to function
				trgLatency=4096-g_TOFDBInfo.tof_monitor_header->trgLatency;
				trgSearchWinT=g_TOFDBInfo.tof_monitor_header->trgSearchWinT;
				trgMatchWinT=g_TOFDBInfo.tof_monitor_header->trgMatchWinT;
				trgSearchWinQT=g_TOFDBInfo.tof_monitor_header->trgSearchWinQT;
				trgMatchWinQT=g_TOFDBInfo.tof_monitor_header->trgMatchWinQT;
				hitCntPerEvtT=g_TOFDBInfo.tof_monitor_header->hitCntPerEvtT;
				hitCntPerEvtQT=g_TOFDBInfo.tof_monitor_header->hitCntPerEvtQT;
				
				break;
			}	
		}
		
		if(FeeHptdcConfig(nadr,chanlEnable,trgEnable,rejectLaten,trgLatency,trgSearchWinT
								,trgMatchWinT,trgSearchWinQT,trgMatchWinQT,hitCntPerEvtT,hitCntPerEvtQT)== -1)
		{
			DEBUG_OUTPUT(("FeeHptdcConfig Failed !\n"));
			Push2ErrStack(PPCFATAL+TOF_FLG+ATOM_ERR+77,0,0,0,0);		/*tof error 77*/	
			return -1;
		}
		taskDelay(60);//please taskDelay enough time to ensure HPDC working
		unsigned abc=0xffffffff;
		ReadFrmVme32(nadr+0x10550,VA32,&abc);
		printf("before 0x%08x=0x%x\n",nadr+0x10550,abc);
		//after config Monitor as FEE,then config some register monitor special
		if(-1==MoniHptdcConfigSpec(nadr))
		{
			DEBUG_OUTPUT(("MoniHptdcConfigSpec Failed !\n"));
			Push2ErrStack(PPCFATAL+TOF_FLG+ATOM_ERR+78,0,0,0,0);		/*tof error 78*/			
			return -1;
		}
		taskDelay(60);
		abc=0xffffffff;
		ReadFrmVme32(nadr+0x10550,VA32,&abc);
		//printf("after 0x%08x=0x%x\n",nadr+0x10550,abc);
		//printf("after config %d slot monitor hptdc\n",g_uTofMonitorGeo);
	}
	return 0;
}


int TOFConfigMonitorDAC()  
{
	if(g_TOFDBInfo.tof_monitor_sum==1)
	{
		UINT nadr=(g_uTofMonitorGeo<<20);
		if(-1== WriteToVme32((nadr+0xc),VA32,g_nTrgPerInt))
		{
			//write g_nTrgPerInt error(TOFConfigMonitorDAC)
			printf(">>VME:: Slot %02d : Write 0x%08x to 0x%08x ->ERROR!\n",g_uTofMonitorGeo,g_nTrgPerInt,(nadr+0xc));
			Push2ErrStack(PPCFATAL+TOF_FLG+ATOM_ERR+79,0,0,0,0);		/*tof error 79*/	
			return -1;
		}
		//config other register
		UINT	lowThresh=g_TOFDBInfo.tof_monitor_header->lowThresh;
				lowThresh=lowThresh*2*4096/2500;
		UINT	highThresh=g_TOFDBInfo.tof_monitor_header->highThresh;
				highThresh=highThresh*2*4096/2500;
		UINT	lowThreshQT=g_TOFDBInfo.tof_monitor_header->lowThreshQT;
				lowThreshQT=lowThreshQT*4096*2/2500;
		UINT	highThreshQT=g_TOFDBInfo.tof_monitor_header->highThreshQT;
				highThreshQT=highThreshQT*4096/(2500*2);
		UINT	constVoltage=g_TOFDBInfo.tof_monitor_header->constVoltage;
				constVoltage=constVoltage*4096/(2500*2);
		UINT	selfTestSignAmp;
		switch(g_nRunMode)
		{
			case NOMALMODE:
			case PEDMODE:
			case MONITOR:
				selfTestSignAmp=g_TOFDBInfo.tof_monitor_header->selfTestSignAmp;
				selfTestSignAmp=selfTestSignAmp*4096/(2500*2);
				break;
			case CALIBRATION:
				selfTestSignAmp=g_uTofCalibSignAmp;
				selfTestSignAmp=selfTestSignAmp*4096/(2500*2);
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
			Push2ErrStack(PPCFATAL+TOF_FLG+ATOM_ERR+80,0,0,0,0);		/*tof error 80*/	
			return -1;
		}
		//InitAdrs,HighThresh,LowThresh,ConstVoltage,SelfTestSignAmp,QTLowThresh,QTHighThresh)
		if(TOFConfigEachDAC(nadr,highThresh,lowThresh,constVoltage,selfTestSignAmp,lowThreshQT,highThreshQT)== -1)
		{
			DEBUG_OUTPUT(("TOFConfigEachDAC Failed 1!\n"));
			Push2ErrStack(PPCFATAL+TOF_FLG+ATOM_ERR+81,0,0,0,0);		/*tof error 81*/					
			return -1;
		}
		//printf("after config %d slot monitor DAC\n",g_uTofMonitorGeo);
	}
	return 0;
}


int TOFMonitorLaserSetup()
{
	UINT nBaseAddr;
	UINT nL1DelayClk;
	if(g_TOFDBInfo.tof_monitor_sum==1)
	{
		nBaseAddr=(g_uTofMonitorGeo<<20);
		
		WriteToVme32(nBaseAddr+0x30, VA32, g_uTofLDPulseInterval);
		taskDelay(1);
		WriteToVme32(nBaseAddr+0x34, VA32, g_uTofLDPulseNum);
		taskDelay(1);
		WriteToVme32(nBaseAddr+0x38, VA32, g_TOFDBInfo.tof_monitor_header->MonitorL1LatencyReg);
		taskDelay(1);
		WriteToVme32(nBaseAddr+0x3c, VA32, g_TOFDBInfo.tof_monitor_header->LDPulseWidthReg);
		taskDelay(1);
		WriteToVme32(nBaseAddr+0x40, VA32, g_TOFDBInfo.tof_monitor_header->MonitorL1PulseWidthReg);
		taskDelay(1);
		
		nL1DelayClk=g_TOFDBInfo.tof_monitor_header->L1DelayClk;
		if( (nL1DelayClk<0)||(nL1DelayClk>20) )
		{
			printf("err:Monitor L1 Delay must 0-20\n");
			Push2ErrStack(PPCFATAL+TOF_FLG+ATOM_ERR+82,0,0,0,0);		/*tof error 82*/		
			return -1;
		}
		/*
		if(g_TOFDBInfo.tof_monitor_header->L1DelayClk ==0)
		{
			nL1DelayBitSelect=0;
		}
		else
		{
			nL1DelayBitSelect=(0x1<<(nL1DelayClk-1));
		}
		*/
		WriteToVme32(nBaseAddr+0x44, VA32, nL1DelayClk);
		taskDelay(1);
	}
	return 0;

}


int TOFConfigMoniInlTable()
{
	if(g_TOFDBInfo.tof_monitor_sum==1)
	{
		UINT nadr=(g_uTofMonitorGeo<<20);
		char * cpInl=g_TOFDBInfo.tof_monitor_header->feeINL;
		if(VME_INLConfig(nadr,cpInl,4)==-1)				// 4 channel
		{
			DEBUG_OUTPUT(("VME_INLConfig Failed !\n"));
			Push2ErrStack(PPCFATAL+TOF_FLG+ATOM_ERR+83,0,0,0,0);		/*tof error 83*/				
			return -1;
		}
		//printf("after config %d slot monitor INL table\n",g_uTofMonitorGeo);
	}
	return 0;
}


int  TOFConfigMoniWorkMode()
{
	if(g_TOFDBInfo.tof_monitor_sum==1)
	{
		UINT32 MonitorConfigData=0;
		UINT nBaseAddr=(g_uTofMonitorGeo<<20);
		switch(g_nRunMode)
		{
			case NOMALMODE:
				if(g_TOFDBInfo.tof_ppc_header->NormModeSelfL1)
				{
					MonitorConfigData=0x4548;
				}
				else
				{
					MonitorConfigData=0x4548; //no INL:0x4508;INL:0x4548
				}
				break;
			case MONITOR:
				MonitorConfigData=0x4440;
				break;
			case CALIBRATION:
				MonitorConfigData=0x84440;
				break;
			case PEDMODE:
			case BASELINE:
			case WAVEMODE:		
				break ;
			default:
				Push2ErrStack(PPCFATAL+TOF_FLG+ATOM_ERR+9,0,0,0,0);		/*tof error 9*/		
				break ;
		}
		if (-1 == WriteToVme32(nBaseAddr, VA32, MonitorConfigData)) 
		{
			printf(">>Monitor err: VA32 Write 0x%08x to 0x%08x with Errors!\n",MonitorConfigData,nBaseAddr); 
		} 
		else
		{
			printf(">>Monitor work: VA32 Write 0x%08x to 0x%08x\n",MonitorConfigData,nBaseAddr); 
		}
		switch(g_nRunMode)
		{
			case NOMALMODE:
			case CALIBRATION:
			case PEDMODE:
			case BASELINE:
			case WAVEMODE:		
				break ;
			case MONITOR:
				TOFLaserDeviceOn();
				break;
			default:
				Push2ErrStack(PPCFATAL+TOF_FLG+ATOM_ERR+9,0,0,0,0);		/*tof error 9*/		
				break ;
		}
	}
	return 0;
}

//-----------------------------------
//for control Laser Device
int TOFLaserDeviceOn()
{
	unsigned nBaseAddr=(g_uTofMonitorGeo<<20);

	if(g_TOFDBInfo.tof_monitor_sum==1)
	{
		WriteToVme32(nBaseAddr+0x8, VA32, 0xa);  //LDMaster ON 
		taskDelay(1);
		WriteToVme32(nBaseAddr+0x8, VA32, 0xc);	 //LDInterLock ON
		taskDelay(1);
		printf("LaserDeviceOn\n");
	}
	return 0;
}

int TOFLaserPulseGen()
{
	unsigned uSoleStat[3];
	unsigned nBaseAddr=(g_uTofMonitorGeo<<20);
	if(g_TOFDBInfo.tof_monitor_sum==1)
	{
		//set  solenoid status
		//g_uTofLDSolenoid,bit 0,1,2, if bit set 1,open solinoid
		uSoleStat[0]= (g_uTofLDSolenoid&0x1)?0x10:0x11;
		uSoleStat[1]= ((g_uTofLDSolenoid>>1)&0x1)?0x12:0x13;
		uSoleStat[2]= ((g_uTofLDSolenoid>>2)&0x1)?0x14:0x15;
		for(int idx=0;idx<3;idx++)
		{
			WriteToVme32(nBaseAddr+0x8, VA32, uSoleStat[idx]); //write solenoid 
			taskDelay(1);
		}
		taskDelay(20);
		WriteToVme32(nBaseAddr+0x8, VA32, 0xe);  //Send pulse train 
		printf("write 0x%x with val 0x%x\n",nBaseAddr+0x8,0xe);
	}
	return 0;
}
int TOFLaserPulseStop()
{
	unsigned nBaseAddr=(g_uTofMonitorGeo<<20);
	if(g_TOFDBInfo.tof_monitor_sum==1)
	{
		WriteToVme32(nBaseAddr+0x8, VA32, 0xf);  //Stop pulse train     
		taskDelay(1);
		
		for(int idx=0;idx<3;idx++)
		{
			WriteToVme32(nBaseAddr+0x8, VA32, 0x11+idx*2); //write solenoid 
			taskDelay(1);
		}
		taskDelay(20);
	}
	
	return 0;
}

int TOFLaserDeviceOff()
{
	if(g_TOFDBInfo.tof_monitor_sum==1)
	{
		unsigned nBaseAddr=(g_uTofMonitorGeo<<20);
		WriteToVme32(nBaseAddr+0x8, VA32, 0xd);	 //LDInterLock OFF
		taskDelay(1);
		WriteToVme32(nBaseAddr+0x8, VA32, 0xb);  //LDMaster OFF 
		taskDelay(1);
	}
	return 0;
}

