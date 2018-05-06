#include "stdio.h"
#include "fw_define.h"
#include "fw_configinfo.h"

void Show_Header(Header_t * pConf)
{
	DEBUG_OUTPUT( ("crate_id: %d\n", pConf->crate_id) );
	DEBUG_OUTPUT( ("All_len: %d\n", pConf->All_len) );
	DEBUG_OUTPUT( ("trigger_type: %d\n", pConf->trigger_type) );
	DEBUG_OUTPUT( ("crate_len: %d\n", pConf->crate_len) );
	DEBUG_OUTPUT( ("events_per_read: %d\n",pConf->events_per_read) );
	DEBUG_OUTPUT( ("module_num: %d\n", pConf->module_num) );
	DEBUG_OUTPUT( ("run_times: %d\n", pConf->run_times) );
	DEBUG_OUTPUT( ("unused: 0x%x\n", pConf->unused) );	
}

void Show_ppc(struct GE_ppc* pConf)
{
	DEBUG_OUTPUT( ("ppc:GEO=%d,PPC_id=%d\n",pConf->geo,pConf->PPC_id  )  );
	DEBUG_OUTPUT( ("ppc:ROS_ip=0x%x,ROS_port=%d\n",pConf->ROS_ip, pConf->ROS_port)  );
	DEBUG_OUTPUT( ("ppc:NormModeSelfL1=%d\n",pConf->NormModeSelfL1)  );
}

void Show_mdc_mqt(mdc_mqt_t * pConf)
{
	DEBUG_OUTPUT( ("mqt:GEO=%d,DAC=%d\n",pConf->geo,pConf->DAC )  );
	DEBUG_OUTPUT( ("mqt:TimeThreshold_l=%d,TimeThreshold_h=%d\n",pConf->TimeThreshold_l,pConf->TimeThreshold_h)  );
	DEBUG_OUTPUT( ("mqt:T_trigger_latency=%d,Q_trigger_latency=%d\n",pConf->T_trigger_latency,pConf->Q_trigger_latency)  );

	int nChnlIdx;
/*	
	for(nChnlIdx = 0; nChnlIdx < 32; nChnlIdx ++)
	{
		printf("adjust_a2[%d]=%f\n",nChnlIdx,pConf->adjust_a2[nChnlIdx]);
	}*/
/*
	for(nChnlIdx = 0; nChnlIdx < 32; nChnlIdx ++)
	{
		printf("adjust_a1[%d]=%f\n",nChnlIdx,pConf->adjust_a1[nChnlIdx]);
	}
	for(nChnlIdx = 0; nChnlIdx < 32; nChnlIdx ++)
	{
		printf("adjust_a0[%d]=%f\n",nChnlIdx,pConf->adjust_a0[nChnlIdx]);
	}
	*/
}
void Show_mdc_roc(mdc_roc_t * proc)
{
	DEBUG_OUTPUT( ("mdcROC:GEO=%d\n",proc->geo)  );
}
void Show_mdc_mcc(mdc_mcc_t* pmcc)
{
	DEBUG_OUTPUT( ("mdcMCC:GEO=%d,T0_latency=%d\n",pmcc->geo,pmcc->T0_latency)  );
}
void Show_mdc_mti(mdc_mti_t* pmti)
{
	DEBUG_OUTPUT( ("mdcMTI:GEO=%d\n",pmti->geo)  );
}
/******MDC-EMC*******
	unsigned int	info_len;//renzy:in bytes
	unsigned int	module_type;//renzy:e.g EMC-Q 0x62
	unsigned int	geo;
	unsigned int	sample_win;
	unsigned int	trigger_latency;
	unsigned int  	outL1;
	unsigned int	unused;	
	unsigned int	map[32];
	unsigned int	QThreshold[32];
	unsigned int 	MainAmpGeo[32];	//but config throuth ROC module
	unsigned int 	GainValue[32];	//but config throuth ROC module
	float			low_adjust_a2[32];
	float			low_adjust_a1[32];
	float			low_adjust_a0[32];	
	float			mid_adjust_a2[32];
	float			mid_adjust_a1[32];
	float			mid_adjust_a0[32];	
	float			high_adjust_a2[32];
	float			high_adjust_a1[32];
	float			high_adjust_a0[32];	
****/

void Show_emc_Q(emc_Q_t* pConf)
{
	DEBUG_OUTPUT( ("emcQ:info_len=%d\n",pConf->info_len)  );
	DEBUG_OUTPUT( ("emcQ:module_type=0x%x\n",pConf->module_type)  );
	DEBUG_OUTPUT( ("emcQ:GEO=0x%x\n",pConf->geo)  );
	DEBUG_OUTPUT( ("emcQ:sample_win=0x%x\n",pConf->sample_win)  );
	DEBUG_OUTPUT( ("emcQ:trigger_latency=0x%x\n",pConf->trigger_latency)  );
	DEBUG_OUTPUT( ("emcQ:outL1=0x%x\n",pConf->outL1)  );

	int i;
/*	
	for(i=0;i<32;i++)
	{
		DEBUG_OUTPUT( ("emcQ:map[%d]=%d\n",i,pConf->map[i])  );
	}

	for(i=0;i<32;i++)
	{
		DEBUG_OUTPUT( ("emcQ:QThreshold[%d]=%d\n",i,pConf->QThreshold[i])  );
	}

	for(i=0;i<32;i++)
	{
		DEBUG_OUTPUT( ("emcQ:MainAmpGeo[%d]=%d\n",i,pConf->MainAmpGeo[i])  );
	}	

	for(i=0;i<32;i++)
	{
		DEBUG_OUTPUT( ("emcQ:GainValue[%d]=%d\n",i,pConf->GainValue[i])  );
	}

	for(i=0;i<32;i++)
	{
		DEBUG_OUTPUT( ("emcQ:low_adjust_a2[%d]=%d\n",i,pConf->low_adjust_a2[i])  );
	}

	for(i=0;i<32;i++)
	{
		DEBUG_OUTPUT( ("emcQ:low_adjust_a1[%d]=%d\n",i,pConf->low_adjust_a1[i])  );
	}

	for(i=0;i<32;i++)
	{
		DEBUG_OUTPUT( ("emcQ:low_adjust_a0[%d]=%d\n",i,pConf->low_adjust_a0[i])  );
	}	

	for(i=0;i<32;i++)
	{
		DEBUG_OUTPUT( ("emcQ:mid_adjust_a2[%d]=%d\n",i,pConf->mid_adjust_a2[i])  );
	}

	for(i=0;i<32;i++)
	{
		DEBUG_OUTPUT( ("emcQ:mid_adjust_a1[%d]=%d\n",i,pConf->mid_adjust_a1[i])  );
	}

	for(i=0;i<32;i++)
	{
		DEBUG_OUTPUT( ("emcQ:mid_adjust_a0[%d]=%d\n",i,pConf->mid_adjust_a0[i])  );
	}

	for(i=0;i<32;i++)
	{
		DEBUG_OUTPUT( ("emcQ:high_adjust_a2[%d]=%d\n",i,pConf->high_adjust_a2[i])  );
	}

	for(i=0;i<32;i++)
	{
		DEBUG_OUTPUT( ("emcQ:high_adjust_a1[%d]=%d\n",i,pConf->high_adjust_a1[i])  );
	}

	for(i=0;i<32;i++)
	{
		DEBUG_OUTPUT( ("emcQ:high_adjust_a0[%d]=%d\n",i,pConf->high_adjust_a0[i])  );
	}	
*/	
}

void Show_emc_roc(emc_roc_t* pConf)
{
	DEBUG_OUTPUT( ("emcROC:GEO=%d,dac=%d,cal=%d\n",pConf->geo,pConf->dac,pConf->cal)  );
}

void Show_emc_fanout(emc_fanout_t* pConf)
{
	DEBUG_OUTPUT( ("emcFanout:GEO=%d,dac=%d,cal=%d\n",pConf->geo,pConf->dac,pConf->cal)  );
	DEBUG_OUTPUT( ("emcFanout: L1 speed=%d\n",pConf->unused) );
}

void Show_emc_monitor(emc_monitor_t* pConf)
{
	DEBUG_OUTPUT( ("emcMonitor:info_len=%d\n",pConf->info_len)  );
	DEBUG_OUTPUT( ("emcMonitor:module_type=0x%x\n",pConf->module_type)  );
	DEBUG_OUTPUT( ("emcMonitor:GEO=0x%x\n",pConf->geo)  );
	DEBUG_OUTPUT( ("emcMonitor:sample_win=0x%x\n",pConf->sample_win)  );
	DEBUG_OUTPUT( ("emcMonitor:trigger_latency=0x%x\n",pConf->trigger_latency)  );
	DEBUG_OUTPUT( ("emcMonitor:outL1=0x%x\n",pConf->outL1)  );

	int i;
/*	
	for(i=0;i<32;i++)
	{
		DEBUG_OUTPUT( ("emcMonitor:map[%d]=%d\n",i,pConf->map[i])  );
	}

	for(i=0;i<32;i++)
	{
		DEBUG_OUTPUT( ("emcMonitor:QThreshold[%d]=%d\n",i,pConf->QThreshold[i])  );
	}

	for(i=0;i<32;i++)
	{
		DEBUG_OUTPUT( ("emcMonitor:MainAmpGeo[%d]=%d\n",i,pConf->MainAmpGeo[i])  );
	}	

	for(i=0;i<32;i++)
	{
		DEBUG_OUTPUT( ("emcMonitor:GainValue[%d]=%d\n",i,pConf->GainValue[i])  );
	}

	for(i=0;i<32;i++)
	{
		DEBUG_OUTPUT( ("emcMonitor:low_adjust_a2[%d]=%f\n",i,pConf->low_adjust_a2[i])  );
	}
*/
	for(i=0;i<32;i++)
	{
		DEBUG_OUTPUT( ("emcMonitor:low_adjust_a1[%d]=%f\n",i,pConf->low_adjust_a1[i])  );
	}
/*
	for(i=0;i<32;i++)
	{
		DEBUG_OUTPUT( ("emcMonitor:low_adjust_a0[%d]=%f\n",i,pConf->low_adjust_a0[i])  );
	}	

	for(i=0;i<32;i++)
	{
		DEBUG_OUTPUT( ("emcMonitor:mid_adjust_a2[%d]=%f\n",i,pConf->mid_adjust_a2[i])  );
	}
*/
	for(i=0;i<32;i++)
	{
		DEBUG_OUTPUT( ("emcMonitor:mid_adjust_a1[%d]=%f\n",i,pConf->mid_adjust_a1[i])  );
	}
/*
	for(i=0;i<32;i++)
	{
		DEBUG_OUTPUT( ("emcMonitor:mid_adjust_a0[%d]=%f\n",i,pConf->mid_adjust_a0[i])  );
	}

	for(i=0;i<32;i++)
	{
		DEBUG_OUTPUT( ("emcMonitor:high_adjust_a2[%d]=%f\n",i,pConf->high_adjust_a2[i])  );
	}
*/
	for(i=0;i<32;i++)
	{
		DEBUG_OUTPUT( ("emcMonitor:high_adjust_a1[%d]=%f\n",i,pConf->high_adjust_a1[i])  );
	}
/*
	for(i=0;i<32;i++)
	{
		DEBUG_OUTPUT( ("emcMonitor:high_adjust_a0[%d]=%f\n",i,pConf->high_adjust_a0[i])  );
	}	
*/	
}


void Show_emc_trgtcba(emc_trgtcba_t * pConf)
{
	DEBUG_OUTPUT( (" emc_trgtcba:info_len=%u\n",pConf->info_len)  );	
	DEBUG_OUTPUT( (" emc_trgtcba:module_type=%x\n",pConf->module_type)  );
	DEBUG_OUTPUT( (" emc_trgtcba:geo=%x\n",pConf->geo)  );
	DEBUG_OUTPUT( (" emc_trgtcba:DatamoduleFlag=%x\n",pConf->DatamoduleFlag)  );
	DEBUG_OUTPUT( (" emc_trgtcba:WinLen=%x\n",pConf->WinLen)  );
	DEBUG_OUTPUT( (" emc_trgtcba:HL_Vth=%x\n",pConf->HL_Vth)  );	
	DEBUG_OUTPUT( (" emc_trgtcba:L1Sel=%x\n",pConf->L1Sel)  );
	DEBUG_OUTPUT( (" emc_trgtcba:IntEn=%x\n",pConf->IntEn)  );

	int iChTimeCompens = 0;
	for(;iChTimeCompens<30;iChTimeCompens++)			
		DEBUG_OUTPUT( (" emc_trgtcba:ChTimeCompens[%d]=0x%x\n",iChTimeCompens,pConf->ChTimeCompens[iChTimeCompens])  );
	
	DEBUG_OUTPUT( (" emc_trgtcba:unused=%x\n",pConf->unused)  );	
}

/******EMC-MUON***********/

void Show_muon_romo(muon_romo_t* pConf)
{
	DEBUG_OUTPUT( ("muonRomo:Position=%d,data_source=%d,threshold=%d,ChainDisable=0x%x\n",pConf->Position,pConf->data_source,pConf->threshold,pConf->ChainDisable)  );
}

void Show_muon_roc(muon_roc_t* pConf)
{
	DEBUG_OUTPUT( ("muonRoc:GEO=%d,triger_rate=%d\n",pConf->geo,pConf->triger_rate)  );
}

void Show_muon_mudt(muon_mudt_t * pConf)
{
	DEBUG_OUTPUT( ("muonMudt:Position=0x%x,ControlState=0x%x\n",pConf->Position,pConf->ControlState)  );
}
/******MUON-TOF***********/

void Show_tof_FEE(tof_FEE_t* pConf)
{
//DEBUG_OUTPUT( ("tofFEE:length=%d\n",pConf->info_len) );
DEBUG_OUTPUT( ("tofFEE:GEO=%d\n",pConf->geo)  );
DEBUG_OUTPUT( ("tofFEE:barrel_end=%d\n",pConf->barrel_end)  );
//DEBUG_OUTPUT( ("tofFEE:chanlEnable=%d\n",pConf->chanlEnable)  );
//DEBUG_OUTPUT( ("tofFEE:trgEnable=%d\n",pConf->trgEnable)  );
DEBUG_OUTPUT( ("tofFEE:rejectLaten=%d\n",pConf->rejectLaten)  );
DEBUG_OUTPUT( ("tofFEE:trgLatency=%d\n",pConf->trgLatency)  );
DEBUG_OUTPUT( ("tofFEE:trgSearchWinT=%d\n",pConf->trgSearchWinT)  );
DEBUG_OUTPUT( ("tofFEE:trgMatchWinT=%d\n",pConf->trgMatchWinT)  );
DEBUG_OUTPUT( ("tofFEE:trgSearchWinQT=%d\n",pConf->trgSearchWinQT)  );
DEBUG_OUTPUT( ("tofFEE:trgMatchWinQT=%d\n",pConf->trgMatchWinQT)  );
//DEBUG_OUTPUT( ("tofFEE:hitCntPerEvtT=%d\n",pConf->hitCntPerEvtT)  );
//DEBUG_OUTPUT( ("tofFEE:hitCntPerEvtQT=%d\n",pConf->hitCntPerEvtQT)  );
DEBUG_OUTPUT( ("tofFEE:highThresh=%d\n",pConf->highThresh)  );
DEBUG_OUTPUT( ("tofFEE:lowThresh=%d\n",pConf->lowThresh)  );
DEBUG_OUTPUT( ("tofFEE:highThreshQT=%d\n",pConf->highThreshQT)  );
DEBUG_OUTPUT( ("tofFEE:lowThreshQT=%d\n",pConf->lowThreshQT)  );
DEBUG_OUTPUT( ("tofFEE:constVoltage=%d\n",pConf->constVoltage)  );
DEBUG_OUTPUT( ("tofFEE:selfTestSignAmp=%d\n",pConf->selfTestSignAmp)  );
int nChnlIdx;
/*
for(nChnlIdx = 0; nChnlIdx < 16; nChnlIdx ++)
{
	printf("map[%d]=0x%08x\n",nChnlIdx,pConf->map[nChnlIdx]);
}
*/
/*
for(nChnlIdx = 0; nChnlIdx < 16; nChnlIdx ++)
{
	printf("adjust_a2[%d]=%f\n",nChnlIdx,pConf->adjust_a2[nChnlIdx]);
}
*/
/*
for(nChnlIdx = 0; nChnlIdx < 16; nChnlIdx ++)
{
	printf("adjust_a1[%d]=%f\n",nChnlIdx,pConf->adjust_a1[nChnlIdx]);
}
for(nChnlIdx = 0; nChnlIdx < 16; nChnlIdx ++)
{
	printf("adjust_a0[%d]=%f\n",nChnlIdx,pConf->adjust_a0[nChnlIdx]);
}
*/
/*
DEBUG_OUTPUT( ("tofFEE: %d  ,%d \n",pConf->feeINL[0],pConf->feeINL[1]) );
DEBUG_OUTPUT( ("tofFEE: %d  ,%d \n",pConf->feeINL[2],pConf->feeINL[3]) );
DEBUG_OUTPUT( ("tofFEE: %d  ,%d \n",pConf->feeINL[4],pConf->feeINL[5]) );
DEBUG_OUTPUT( ("tofFEE: %d  ,%d \n",pConf->feeINL[6],pConf->feeINL[7]) );
*/
/*
if(testtest)
{
	for(int tdd=0;tdd<1024;tdd++)
	{
		if((tdd%8)==0)printf("\n");
		int sdd=(signed char)(pConf->feeINL[tdd]);
		printf("%d  ",sdd);
	}
	testtest=0;
}
*/
}

void Show_tof_monitor(tof_monitor_t* pConf)
{
	DEBUG_OUTPUT( ("tofmonitor:length=%d\n",pConf->info_len) );
	DEBUG_OUTPUT( ("tofmonitor:GEO=%d\n",pConf->geo)  );

	DEBUG_OUTPUT( ("tofmonitor:LDPulseInterval=%d\n",pConf->LDPulseIntervalReg)  );
	DEBUG_OUTPUT( ("tofmonitor:LDPulseNum=%d\n",pConf->LDPulseNumReg)  );
	DEBUG_OUTPUT( ("tofmonitor:MonitorL1Latency=%d\n",pConf->MonitorL1LatencyReg)  );
	DEBUG_OUTPUT( ("tofmonitor:LDPulseWidth=%d\n",pConf->LDPulseWidthReg)  );
	DEBUG_OUTPUT( ("tofmonitor:L1DelayClk=%d\n",pConf->L1DelayClk)  );
	DEBUG_OUTPUT( ("tofmonitor:LDSolenoid=0x%x\n",pConf->LDSolenoid)  );

	DEBUG_OUTPUT( ("tofmonitor: %d  ,%d \n",pConf->feeINL[0],pConf->feeINL[1]) );
	DEBUG_OUTPUT( ("tofmonitor: %d  ,%d \n",pConf->feeINL[2],pConf->feeINL[3]) );
	DEBUG_OUTPUT( ("tofmonitor: %d  ,%d \n",pConf->feeINL[4],pConf->feeINL[5]) );
	DEBUG_OUTPUT( ("tofmonitor: %d  ,%d \n",pConf->feeINL[6],pConf->feeINL[7]) );

}

void Show_tof_roc(tof_roc_t* pConf)
{
	DEBUG_OUTPUT( ("tofRoc :GEO=%d\n",pConf->geo)  );
	DEBUG_OUTPUT( ("tofRoc :clksource=%d\n",pConf->clksource)  );
	DEBUG_OUTPUT( ("tofRoc :autoL1Interval=%d\n",pConf->autoL1Interval)  );
	DEBUG_OUTPUT( ("tofRoc :semiAutoL1Num=%d\n",pConf->semiAutoL1Num)  );
	DEBUG_OUTPUT( ("tofRoc :L1Latency=%d\n",pConf->L1Latency)  );
}

void Show_tof_clk(tof_clk_t* pConf)
{
	DEBUG_OUTPUT( ("tofClk :GEO=%d ,clksource=%d \n",pConf->geo,pConf->clksource)  );
}


/***** ETF **********/

void Show_etf_tdig(etf_tdig_t* pConf)
{
	DEBUG_OUTPUT( ("etftdig :GEO=%d\n",pConf->geo)  );
	DEBUG_OUTPUT( ("etftdig :windowSet=0x%x\n",pConf->windowSet)  );
	DEBUG_OUTPUT( ("etftdig :trigger_latency=0x%x\n",pConf->trigger_latency)  );
	DEBUG_OUTPUT( ("etftdig :trigger_source=0x%x\n",pConf->trigger_source)  );
	DEBUG_OUTPUT( ("etftdig :errDataEnb=0x%x\n",pConf->errDataEnb)  );
	int nChnlIdx;

	for(nChnlIdx = 0; nChnlIdx < 72; nChnlIdx ++)
	{
		printf("map[%d]=0x%08x\n",nChnlIdx,pConf->map[nChnlIdx]);
	}
	
}
void Show_etf_roc(etf_roc_t* pConf)
{
	DEBUG_OUTPUT( ("etfRoc :GEO=%d\n", pConf->geo)  );
	DEBUG_OUTPUT( ("etfRoc :clksource=%d\n", pConf->clksource)  );
	DEBUG_OUTPUT( ("etfRoc :autoL1Interval=%d\n", pConf->autoL1Interval)  );
	DEBUG_OUTPUT( ("etfRoc :semiAutoL1Num=%d\n", pConf->semiAutoL1Num)  );
	DEBUG_OUTPUT( ("etfRoc :L1Latency=%d\n", pConf->L1Latency)  );
	DEBUG_OUTPUT( ("etfRoc :clock_phase=%d\n", pConf->clock_phase)  );
}

void Show_etf_clk(etf_clk_t* pConf)
{
	DEBUG_OUTPUT( ("etfClk :GEO=%d ,clksource=%d ,latency = 0x%x\n",pConf->geo,pConf->clksource,pConf->clkLatency)  );
}

void Show_etf_cttp(etf_cttp_t* pConf)
{
	DEBUG_OUTPUT( ("etfCTTP :cttp_enable=%d\n",pConf->cttp_enable)  );
	DEBUG_OUTPUT( ("etfCTTP :mod_ID=%d\n",pConf->mod_ID)  );
	DEBUG_OUTPUT( ("etfCTTP :nino_low_threshold=%d\n",pConf->nino_low_threshold)  );
	DEBUG_OUTPUT( ("etfCTTP :nino_high_threshold=%d\n",pConf->nino_high_threshold)  );
	DEBUG_OUTPUT( ("etfCTTP :power_low_threshold=%d\n",pConf->power_low_threshold)  );
	DEBUG_OUTPUT( ("etfCTTP :power_high_threshold=%d\n",pConf->power_high_threshold)  );
	DEBUG_OUTPUT( ("etfCTTP :cal_amp=%d\n",pConf->cal_amp)  );
}

/******TOF-TRG***********/
/*
void Show_trg_fctl(trg_fctl_t* pConf)
{
	DEBUG_OUTPUT( ("trgFctl :GEO=%d\n",pConf->geo)  );
}
*/

void Show_trg_fctl(trg_fctl_t* pConf)
{
	DEBUG_OUTPUT( (" trg_fctl:info_len=%u\n",pConf->info_len)  );
	DEBUG_OUTPUT( (" trg_fctl:module_type=0x%x\n",pConf->module_type)  );
	DEBUG_OUTPUT( (" trg_fctl:Position=0x%x\n",pConf->Position)  );
	DEBUG_OUTPUT( (" trg_fctl:DatamoduleFlag=0x%x\n",pConf->DatamoduleFlag)  );
	DEBUG_OUTPUT( (" trg_fctl:L1_config=0x%x\n",pConf->L1_config)  );
	DEBUG_OUTPUT( (" trg_fctl:Input_en1=0x%x\n",pConf->Input_en1)  );
	DEBUG_OUTPUT( (" trg_fctl:Input_en2=0x%x\n",pConf->Input_en2)  );
	DEBUG_OUTPUT( (" trg_fctl:IntCount=0x%x\n",pConf->IntCount)  );
	DEBUG_OUTPUT( (" trg_fctl:IntTiming=0x%x\n",pConf->IntTiming)  );
	DEBUG_OUTPUT( (" trg_fctl:IntEnable=0x%x\n",pConf->IntEnable)  );
	DEBUG_OUTPUT( (" trg_fctl:SendFRST=0x%x\n",pConf->SendFRST)  );
	DEBUG_OUTPUT( (" trg_fctl:SubSystemInfo=0x%x\n",pConf->SubSystemInfo)  );	
}


void Show_trg_tfcf(trg_tfcf_t * pConf)
{
	DEBUG_OUTPUT( (" trg_tfcf:info_len=%u\n",pConf->info_len)  );
	DEBUG_OUTPUT( (" trg_tfcf:module_type=0x%x\n",pConf->module_type)  );
	DEBUG_OUTPUT( (" trg_tfcf:Position=0x%x\n",pConf->Position)  );
	DEBUG_OUTPUT( (" trg_tfcf:DatamoduleFlag=0x%x\n",pConf->DatamoduleFlag)  );
	DEBUG_OUTPUT( (" trg_tfcf:CSR=0x%x\n",pConf->CSR)  );
	DEBUG_OUTPUT( (" trg_tfcf:V_Err_T=0x%x\n",pConf->V_Err_T)  );
	DEBUG_OUTPUT( (" trg_tfcf:V_Err_C=0x%x\n",pConf->V_Err_C)  );
	DEBUG_OUTPUT( (" trg_tfcf:Per_ERR_12=0x%x\n",pConf->Per_ERR_12)  );
	DEBUG_OUTPUT( (" trg_tfcf:Per_ERR_34=0x%x\n",pConf->Per_ERR_34)  );
	DEBUG_OUTPUT( (" trg_tfcf:CO_IR=0x%x\n",pConf->CO_IR)  );
	DEBUG_OUTPUT( (" trg_tfcf:IO_PR=0x%x\n",pConf->IO_PR)  );
	DEBUG_OUTPUT( (" trg_tfcf:ENBL=0x%x\n",pConf->ENBL)  );
	DEBUG_OUTPUT( (" trg_tfcf:unused=0x%x\n",pConf->unused)  );
}

void Show_trg_clkf(trg_clkf_t * pConf)
{
	DEBUG_OUTPUT( (" trg_clkf:info_len=%u\n",pConf->info_len)  );
	DEBUG_OUTPUT( (" trg_clkf:module_type=0x%x\n",pConf->module_type)  );
	DEBUG_OUTPUT( (" trg_clkf:Position=0x%x\n",pConf->Position)  );
	DEBUG_OUTPUT( (" trg_clkf:DatamoduleFlag=0x%x\n",pConf->DatamoduleFlag)  );
	DEBUG_OUTPUT( (" trg_clkf:ClkSource=0x%x\n",pConf->ClkSource)  );
	DEBUG_OUTPUT( (" trg_clkf:unused=0x%x\n",pConf->unused)  );
}

void Show_trg_troc(trg_troc_t * pConf)
{
	DEBUG_OUTPUT( (" trg_troc:info_len=%u\n",pConf->info_len)  );
	DEBUG_OUTPUT( (" trg_troc:module_type=0x%x\n",pConf->module_type)  );
	DEBUG_OUTPUT( (" trg_troc:Position=0x%x\n",pConf->Position)  );
	DEBUG_OUTPUT( (" trg_troc:DatamoduleFlag=0x%x\n",pConf->DatamoduleFlag)  );
	DEBUG_OUTPUT( (" trg_troc:unused=0x%x\n",pConf->unused)  );
}


void Show_trg_eacc(trg_eacc_t * pConf)
{
	DEBUG_OUTPUT( (" trg_eacc:info_len=%u\n",pConf->info_len)  );
	DEBUG_OUTPUT( (" trg_eacc:module_type=0x%x\n",pConf->module_type)  );
	DEBUG_OUTPUT( (" trg_eacc:Position=0x%x\n",pConf->Position)  );
	DEBUG_OUTPUT( (" trg_eacc:DatamoduleFlag=0x%x\n",pConf->DatamoduleFlag)  );
	DEBUG_OUTPUT( (" trg_eacc:SimRealSel=0x%x\n",pConf->SimRealSel)  );
	DEBUG_OUTPUT( (" trg_eacc:L1Sel=0x%x\n",pConf->L1Sel)  );
	DEBUG_OUTPUT( (" trg_eacc:FiberAlignLatency=0x%x\n",pConf->FiberAlignLatency)  );
	DEBUG_OUTPUT( (" trg_eacc:FiberChannelSet=0x%x\n",pConf->FiberChannelSet)  );
	DEBUG_OUTPUT( (" trg_eacc:CSR1=0x%x\n",pConf->CSR1)  );
	DEBUG_OUTPUT( (" trg_eacc:CsumDepths=0x%x\n",pConf->CsumDepths)  );
	DEBUG_OUTPUT( (" trg_eacc:CsumWins=0x%x\n",pConf->CsumWins)  );
	DEBUG_OUTPUT( (" trg_eacc:DataLenPerEvent=0x%x\n",pConf->DataLenPerEvent)  );
	DEBUG_OUTPUT( (" trg_eacc:EtotDataSteps=0x%x\n",pConf->EtotDataSteps)  );
	DEBUG_OUTPUT( (" trg_eacc:EtotDataDepths=0x%x\n",pConf->EtotDataDepths)  );
	DEBUG_OUTPUT( (" trg_eacc:BarrelEndcapSel=0x%x\n",pConf->BarrelEndcapSel)  );
	
	int iThreshold = 0;
	for(;iThreshold<10;iThreshold++)
		DEBUG_OUTPUT( (" trg_eacc:Threshold[%d]=0x%x\n",iThreshold,pConf->Threshold[iThreshold])  );
	DEBUG_OUTPUT(("\n"));

	DEBUG_OUTPUT( (" trg_eacc:ErrSet=0x%x\n",pConf->ErrSet)  );
	DEBUG_OUTPUT( (" trg_eacc:DebugFlg=0x%x\n",pConf->DebugFlg)  );
	
	int iSimDataBlk = 0;
	for(;iSimDataBlk<32;iSimDataBlk++)	
		DEBUG_OUTPUT( (" trg_eacc:SimDataBlk[%d]=0x%x\n",iSimDataBlk,pConf->SimDataBlk[iSimDataBlk]) );
	DEBUG_OUTPUT(("\n"));
		
	DEBUG_OUTPUT( (" trg_eacc:CsumDepthsCal=0x%x\n",pConf->CsumDepthsCal)  );	
	DEBUG_OUTPUT( (" trg_eacc:EtotDepthCal=0x%x\n",pConf->EtotDepthCal)  );	
}


void Show_trg_saf(trg_saf_t * pConf)
{
	DEBUG_OUTPUT( (" trg_saf:info_len=%u\n",pConf->info_len)  );
	DEBUG_OUTPUT( (" trg_saf:module_type=0x%x\n",pConf->module_type)  );
	DEBUG_OUTPUT( (" trg_saf:Position=0x%x\n",pConf->Position)  );
	DEBUG_OUTPUT( (" trg_saf:DatamoduleFlag=0x%x\n",pConf->DatamoduleFlag)  );

	int iDelayOfChannel = 0;
	for(;iDelayOfChannel<16;iDelayOfChannel++)	
		DEBUG_OUTPUT( (" trg_saf:DelayOfChannel[%d]=0x%x\n",iDelayOfChannel,pConf->DelayOfChannel[iDelayOfChannel] )  );
	DEBUG_OUTPUT(("\n"));
	
	DEBUG_OUTPUT( (" trg_saf:UserOwnCSR=0x%x\n",pConf->UserOwnCSR)  );
	DEBUG_OUTPUT( (" trg_saf:CbltMcstCSR1=0x%x\n",pConf->CbltMcstCSR1)  );
	DEBUG_OUTPUT( (" trg_saf:TrgLatencyInput=0x%x\n",pConf->TrgLatencyInput)  );
	DEBUG_OUTPUT( (" trg_saf:TrgWindowWidthInput=0x%x\n",pConf->TrgWindowWidthInput)  );
	DEBUG_OUTPUT( (" trg_saf:TrgLatencyAligned=0x%x\n",pConf->TrgLatencyAligned)  );
	DEBUG_OUTPUT( (" trg_saf:TrgWindowWidthAligned=0x%x\n",pConf->TrgWindowWidthAligned)  );
	DEBUG_OUTPUT( (" trg_saf:FullGlobalBuffer=0x%x\n",pConf->FullGlobalBuffer)  );
	DEBUG_OUTPUT( (" trg_saf:FullGlobalORedBuffer=0x%x\n",pConf->FullGlobalORedBuffer)  );
	DEBUG_OUTPUT( (" trg_saf:FullNotGlobalBuffer=0x%x\n",pConf->FullNotGlobalBuffer)  );
	DEBUG_OUTPUT( (" trg_saf:FullNotGlobalORedBuffer=0x%x\n",pConf->FullNotGlobalORedBuffer)  );	
	DEBUG_OUTPUT( (" trg_saf:L1Selection=0x%x\n",pConf->L1Selection)  );
	DEBUG_OUTPUT( (" trg_saf:unused=0x%x\n",pConf->unused)  );
}


void Show_trg_gtl(trg_gtl_t * pConf)
{
	DEBUG_OUTPUT( (" trg_gtl:info_len=%u\n",pConf->info_len)  );
	DEBUG_OUTPUT( (" trg_gtl:module_type=0x%x\n",pConf->module_type)  );
	DEBUG_OUTPUT( (" trg_gtl:Position=0x%x\n",pConf->Position)  );
	DEBUG_OUTPUT( (" trg_gtl:DatamoduleFlag=0x%x\n",pConf->DatamoduleFlag)  );

	int iDelayOfChannel = 0;
	for(;iDelayOfChannel<48;iDelayOfChannel++)		
		DEBUG_OUTPUT( (" trg_gtl:DelayOfChannel[%d]=0x%x\n",iDelayOfChannel,pConf->DelayOfChannel[iDelayOfChannel])  );
	DEBUG_OUTPUT(("\n"));	
	
	DEBUG_OUTPUT( (" trg_gtl:InternalL1Delay=0x%x\n",pConf->InternalL1Delay)  );
	DEBUG_OUTPUT( (" trg_gtl:UserOwnCSR=0x%x\n",pConf->UserOwnCSR)  );
	DEBUG_OUTPUT( (" trg_gtl:CbltMcstCSR1=0x%x\n",pConf->CbltMcstCSR1)  );
	DEBUG_OUTPUT( (" trg_gtl:TrgWindows=0x%x\n",pConf->TrgWindows)  );
	DEBUG_OUTPUT( (" trg_gtl:GlobalBufferNotFullFull=0x%x\n",pConf->GlobalBufferNotFullFull)  );
	DEBUG_OUTPUT( (" trg_gtl:BarrelBhabhaPrescale=0x%x\n",pConf->BarrelBhabhaPrescale)  );
	DEBUG_OUTPUT( (" trg_gtl:EndcapBhabhaPrescale=0x%x\n",pConf->EndcapBhabhaPrescale)  );
	DEBUG_OUTPUT( (" trg_gtl:BackupRegister=0x%x\n",pConf->BackupRegister)  );
	DEBUG_OUTPUT( (" trg_gtl:TimingOutputBitsMdcEmcTof=0x%x\n",pConf->TimingOutputBitsMdcEmcTof)  );
	DEBUG_OUTPUT( (" trg_gtl:TofTimingORedBits=0x%x\n",pConf->TofTimingORedBits)  );
	DEBUG_OUTPUT( (" trg_gtl:EmcTimingORedBits=0x%x\n",pConf->EmcTimingORedBits)  );
	DEBUG_OUTPUT( (" trg_gtl:MdcTimingORedBits=0x%x\n",pConf->MdcTimingORedBits)  );
	DEBUG_OUTPUT( (" trg_gtl:TimeOutL1InternalWidth=0x%x\n",pConf->TimeOutL1InternalWidth)  );
	DEBUG_OUTPUT( (" trg_gtl:L1OutputWidthTimingSelectionMdcEmcTof=0x%x\n",pConf->L1OutputWidthTimingSelectionMdcEmcTof)  );
	DEBUG_OUTPUT( (" trg_gtl:L1ChkOutputControl=0x%x\n",pConf->L1ChkOutputControl)  );
	DEBUG_OUTPUT( (" trg_gtl:ChkDelayToL1=0x%x\n",pConf->ChkDelayToL1)  );
	DEBUG_OUTPUT( (" trg_gtl:TrgLatencyInput=0x%x\n",pConf->TrgLatencyInput)  );
	DEBUG_OUTPUT( (" trg_gtl:TrgLatencyAligned=0x%x\n",pConf->TrgLatencyAligned)  );
	DEBUG_OUTPUT( (" trg_gtl:TrgLatencyEvtType=0x%x\n",pConf->TrgLatencyEvtType)  );
	DEBUG_OUTPUT( (" trg_gtl:TrgChannelEnables=0x%x\n",pConf->TrgChannelEnables)  );	

	int iTrigTable = 0;
	for(;iTrigTable<64;iTrigTable++)		
		DEBUG_OUTPUT( (" trg_gtl:TrigTable[%d]=0x%x\n",iTrigTable,pConf->TrigTable[iTrigTable])  );
	DEBUG_OUTPUT(("\n"));	
	
	DEBUG_OUTPUT( (" trg_gtl:unused=0x%x\n",pConf->unused)  );		
}

void Show_trg_tdc(trg_tdc_t * pConf)
{
	DEBUG_OUTPUT( (" trg_tdc:info_len=%u\n",pConf->info_len)  );
	DEBUG_OUTPUT( (" trg_tdc:module_type=0x%x\n",pConf->module_type)  );
	DEBUG_OUTPUT( (" trg_tdc:Position=0x%x\n",pConf->Position)  );
	DEBUG_OUTPUT( (" trg_tdc:DatamoduleFlag=0x%x\n",pConf->DatamoduleFlag)  );

	int iTrigTable = 0;
	for(;iTrigTable<4;iTrigTable++)		
		DEBUG_OUTPUT( (" trg_tdc:ChannelEnable[%d]=0x%x\n",iTrigTable,pConf->ChannelEnable[iTrigTable])  );
	DEBUG_OUTPUT(("\n"));

	DEBUG_OUTPUT( (" trg_tdc:SetTriggerWindowWidth01=0x%x\n",pConf->SetTriggerWindowWidth01)  );
	DEBUG_OUTPUT( (" trg_tdc:SetTriggerWindowOffset01=0x%x\n",pConf->SetTriggerWindowOffset01)  );
	DEBUG_OUTPUT( (" trg_tdc:SetExtraSearchMargin01=0x%x\n",pConf->SetExtraSearchMargin01)  );
	DEBUG_OUTPUT( (" trg_tdc:SetRejectMargin01=0x%x\n",pConf->SetRejectMargin01)  );
	DEBUG_OUTPUT( (" trg_tdc:SetEdgeDetectionConfig01=0x%x\n",pConf->SetEdgeDetectionConfig01)  );
	DEBUG_OUTPUT( (" trg_tdc:SetLeadingTrailingEdge01=0x%x\n",pConf->SetLeadingTrailingEdge01)  );
	DEBUG_OUTPUT( (" trg_tdc:SetChannelDeadTimeOfHits01=0x%x\n",pConf->SetChannelDeadTimeOfHits01)  );
	DEBUG_OUTPUT( (" trg_tdc:SetMaxNumberOfHitsPerEvent01=0x%x\n",pConf->SetMaxNumberOfHitsPerEvent01)  );
	DEBUG_OUTPUT( (" trg_tdc:EnableTdcInternalErrorType01=0x%x\n",pConf->EnableTdcInternalErrorType01)  );
	DEBUG_OUTPUT( (" trg_tdc:SetEffectiveSizeOfReadoutFifo01=0x%x\n",pConf->SetEffectiveSizeOfReadoutFifo01)  );
	DEBUG_OUTPUT( (" trg_tdc:SetDllClock01=0x%x\n",pConf->SetDllClock01)  );
	DEBUG_OUTPUT( (" trg_tdc:SetControlRegister=0x%x\n",pConf->SetControlRegister)  );
	DEBUG_OUTPUT( (" trg_tdc:SetInterruptLevel=0x%x\n",pConf->SetInterruptLevel)  );
	DEBUG_OUTPUT( (" trg_tdc:SetInterruptVector=0x%x\n",pConf->SetInterruptVector)  );
	DEBUG_OUTPUT( (" trg_tdc:SetGeoAddress=0x%x\n",pConf->SetGeoAddress)  );
	DEBUG_OUTPUT( (" trg_tdc:SetCbltMcstBaseAddress=0x%x\n",pConf->SetCbltMcstBaseAddress)  );
	DEBUG_OUTPUT( (" trg_tdc:SetCbltMcstCSR=0x%x\n",pConf->SetCbltMcstCSR)  );
	DEBUG_OUTPUT( (" trg_tdc:SetAlmostFull=0x%x\n",pConf->SetAlmostFull)  );
	DEBUG_OUTPUT( (" trg_tdc:SetOutProgControl=0x%x\n",pConf->SetOutProgControl)  );
	DEBUG_OUTPUT( (" trg_tdc:unused=0x%x\n",pConf->unused)  );	
}


void Show_trg_tkc(trg_tkc_t * pConf)
{
	DEBUG_OUTPUT( (" trg_tkc:info_len=%u\n",pConf->info_len)  );
	DEBUG_OUTPUT( (" trg_tkc:module_type=0x%x\n",pConf->module_type)  );
	DEBUG_OUTPUT( (" trg_tkc:Position=0x%x\n",pConf->Position)  );
	DEBUG_OUTPUT( (" trg_tkc:DatamoduleFlag=0x%x\n",pConf->DatamoduleFlag)  );
	DEBUG_OUTPUT( (" trg_tkc:CBLTSet=0x%x\n",pConf->CBLTSet)  );
	DEBUG_OUTPUT( (" trg_tkc:L1Select=0x%x\n",pConf->L1Select)  );
	DEBUG_OUTPUT( (" trg_tkc:unused=0x%x\n",pConf->unused)  );
}


void Show_trg_tkf(trg_tkf_t * pConf)
{
	DEBUG_OUTPUT( (" trg_tkf:info_len=%u\n",pConf->info_len)  );
	DEBUG_OUTPUT( (" trg_tkf:module_type=0x%x\n",pConf->module_type)  );
	DEBUG_OUTPUT( (" trg_tkf:Position=0x%x\n",pConf->Position)  );
	DEBUG_OUTPUT( (" trg_tkf:DatamoduleFlag=0x%x\n",pConf->DatamoduleFlag)  );
	DEBUG_OUTPUT( (" trg_tkf:Type=0x%x\n",pConf->Type)  );
	DEBUG_OUTPUT( (" trg_tkf:ChainSet=0x%x\n",pConf->ChainSet)  );
	DEBUG_OUTPUT( (" trg_tkf:ReadoutEventNum=0x%x\n",pConf->ReadoutEventNum)  );
	DEBUG_OUTPUT( (" trg_tkf:ReadoutEventWindow=0x%x\n",pConf->ReadoutEventWindow)  );
	DEBUG_OUTPUT( (" trg_tkf:ProgramFull=0x%x\n",pConf->ProgramFull)  );
	DEBUG_OUTPUT( (" trg_tkf:ProgramFullDisable=0x%x\n",pConf->ProgramFullDisable)  );
	DEBUG_OUTPUT( (" trg_tkf:L1Select=0x%x\n",pConf->L1Select)  );
	DEBUG_OUTPUT( (" trg_tkf:ReadoutModeSelect=0x%x\n",pConf->ReadoutModeSelect)  );
	DEBUG_OUTPUT( (" trg_tkf:L1LatencyToHit=0x%x\n",pConf->L1LatencyToHit)  );
	DEBUG_OUTPUT( (" trg_tkf:TSFLatencyToHit=0x%x\n",pConf->TSFLatencyToHit)  );
	DEBUG_OUTPUT( (" trg_tkf:TrackLatencyToHit=0x%x\n",pConf->TrackLatencyToHit)  );
	DEBUG_OUTPUT( (" trg_tkf:FiberSyncLatencyToFRST=0x%x\n",pConf->FiberSyncLatencyToFRST)  );
	DEBUG_OUTPUT( (" trg_tkf:FiberChannelSelect=0x%x\n",pConf->FiberChannelSelect)  );

	int iTrigTable = 0;
	for(;iTrigTable<16;iTrigTable++)	
		DEBUG_OUTPUT( (" trg_tkf:HitPreSetChannel[%d]=0x%x\n",iTrigTable,pConf->HitPreSetChannel[iTrigTable])  );
	DEBUG_OUTPUT(("\n"));

	for(iTrigTable = 0;iTrigTable<5;iTrigTable++)	
		DEBUG_OUTPUT( (" trg_tkf:Spare[%d]=0x%x\n",iTrigTable,pConf->Spare[iTrigTable])  );
	DEBUG_OUTPUT(("\n"));

	
	DEBUG_OUTPUT( (" trg_tkf:DataSelect=0x%x\n",pConf->DataSelect)  );
	DEBUG_OUTPUT( (" trg_tkf:unused=0x%x\n",pConf->unused)  );
}

void Show_trg_tof(trg_tof_t * pConf)
{
	DEBUG_OUTPUT( (" trg_tof:info_len=%u\n",pConf->info_len)  );
	DEBUG_OUTPUT( (" trg_tof:module_type=0x%x\n",pConf->module_type)  );
	DEBUG_OUTPUT( (" trg_tof:Position=0x%x\n",pConf->Position)  );
	DEBUG_OUTPUT( (" trg_tof:DatamoduleFlag=0x%x\n",pConf->DatamoduleFlag)  );
	DEBUG_OUTPUT( (" trg_tof:WorkMode=0x%x\n",pConf->WorkMode)  );
	DEBUG_OUTPUT( (" trg_tof:FiberChannelShield1=0x%x\n",pConf->FiberChannelShield1)  );
	DEBUG_OUTPUT( (" trg_tof:FiberChannelShield2=0x%x\n",pConf->FiberChannelShield2)  );
	DEBUG_OUTPUT( (" trg_tof:SubstituteChannel1=0x%x\n",pConf->SubstituteChannel1)  );
	DEBUG_OUTPUT( (" trg_tof:SubstituteChannel2=0x%x\n",pConf->SubstituteChannel2)  );
	DEBUG_OUTPUT( (" trg_tof:SubstituteChannel3=0x%x\n",pConf->SubstituteChannel3)  );
	DEBUG_OUTPUT( (" trg_tof:ControlParameter=0x%x\n",pConf->ControlParameter)  );
	DEBUG_OUTPUT( (" trg_tof:DisposeParameter=0x%x\n",pConf->DisposeParameter)  );
	DEBUG_OUTPUT( (" trg_tof:PhysAddress=0x%x\n",pConf->PhysAddress)  );
	DEBUG_OUTPUT( (" trg_tof:CR_CSR=0x%x\n",pConf->CR_CSR)  );
	DEBUG_OUTPUT( (" trg_tof:TLK_ON=0x%x\n",pConf->TLK_ON)  );
	DEBUG_OUTPUT( (" trg_tof:unused=0x%x\n",pConf->unused)  );	
}

void Show_trg_etf(trg_etf_t * pConf)
{
	DEBUG_OUTPUT( (" trg_etf:info_len=%u\n",pConf->info_len)  );
	DEBUG_OUTPUT( (" trg_etf:module_type=0x%x\n",pConf->module_type)  );
	DEBUG_OUTPUT( (" trg_etf:Position=0x%x\n",pConf->Position)  );
	DEBUG_OUTPUT( (" trg_etf:DatamoduleFlag=0x%x\n",pConf->DatamoduleFlag)  );
	DEBUG_OUTPUT( (" trg_etf:readout_window=0x%x\n",pConf->readoutWindow)  );
	DEBUG_OUTPUT( (" trg_etf:CBLTSet=0x%x\n",pConf->CBLTSet)  );
	int iTrigTable = 0;
	for(;iTrigTable<18;iTrigTable++)		
		DEBUG_OUTPUT( (" trg_etf:map[%d]=0x%x\n",iTrigTable,pConf->map[iTrigTable])  );
	DEBUG_OUTPUT(("\n"));		

}

void Show_trg_mutl(trg_mutl_t * pConf)
{
	DEBUG_OUTPUT( (" trg_mutl:info_len=%u\n",pConf->info_len)  );
	DEBUG_OUTPUT( (" trg_mutl:module_type=0x%x\n",pConf->module_type)  );
	DEBUG_OUTPUT( (" trg_mutl:Position=0x%x\n",pConf->Position)  );
	DEBUG_OUTPUT( (" trg_mutl:DatamoduleFlag=0x%x\n",pConf->DatamoduleFlag)  );
	DEBUG_OUTPUT( (" trg_mutl:ChainSet=0x%x\n",pConf->ChainSet)  );
	DEBUG_OUTPUT( (" trg_mutl:ReadoutEventNum=0x%x\n",pConf->ReadoutEventNum)  );
	DEBUG_OUTPUT( (" trg_mutl:ReadoutEventWindow=0x%x\n",pConf->ReadoutEventWindow)  );
	DEBUG_OUTPUT( (" trg_mutl:ProgramFull=0x%x\n",pConf->ProgramFull)  );
	DEBUG_OUTPUT( (" trg_mutl:ProgramFullDisable=0x%x\n",pConf->ProgramFullDisable)  );
	DEBUG_OUTPUT( (" trg_mutl:L1Select=0x%x\n",pConf->L1Select)  );
	DEBUG_OUTPUT( (" trg_mutl:ReadoutModeSelect=0x%x\n",pConf->ReadoutModeSelect)  );
	DEBUG_OUTPUT( (" trg_mutl:L1LatencyToHit=0x%x\n",pConf->L1LatencyToHit)  );
	DEBUG_OUTPUT( (" trg_mutl:TSFLatencyToHit=0x%x\n",pConf->TSFLatencyToHit)  );
	DEBUG_OUTPUT( (" trg_mutl:TrackLatencyToHit=0x%x\n",pConf->TrackLatencyToHit)  );
	DEBUG_OUTPUT( (" trg_mutl:FiberSyncLatencyToFRST=0x%x\n",pConf->FiberSyncLatencyToFRST)  );
	DEBUG_OUTPUT( (" trg_mutl:FiberChannelSelect=0x%x\n",pConf->FiberChannelSelect)  );
	DEBUG_OUTPUT( (" trg_mutl:DataSelect=0x%x\n",pConf->DataSelect)  );
	DEBUG_OUTPUT( (" trg_mutl:unused=0x%x\n",pConf->unused)  );

}


void Show_trg_basg(trg_basg_t * pConf)
{
	DEBUG_OUTPUT( (" trg_basg:info_len=%u\n",pConf->info_len)  );
	DEBUG_OUTPUT( (" trg_basg:module_type=0x%x\n",pConf->module_type)  );
	DEBUG_OUTPUT( (" trg_basg:Position=0x%x\n",pConf->Position)  );
	DEBUG_OUTPUT( (" trg_basg:DatamoduleFlag=0x%x\n",pConf->DatamoduleFlag)  );

	int iTrigTable = 0;
	for(;iTrigTable<48;iTrigTable++)		
		DEBUG_OUTPUT( (" trg_basg:ParSetChannel[%d]=0x%x\n",iTrigTable,pConf->ParSetChannel[iTrigTable])  );
	DEBUG_OUTPUT(("\n"));	

	
	DEBUG_OUTPUT( (" trg_basg:UserCSR=0x%x\n",pConf->UserCSR)  );
	DEBUG_OUTPUT( (" trg_basg:ChannelMaskCSR01=0x%x\n",pConf->ChannelMaskCSR01)  );
	DEBUG_OUTPUT( (" trg_basg:ChannelMaskCSR02=0x%x\n",pConf->ChannelMaskCSR02)  );
	DEBUG_OUTPUT( (" trg_basg:BASWidth=0x%x\n",pConf->BASWidth)  );
	DEBUG_OUTPUT( (" trg_basg:ChannelSelForLight=0x%x\n",pConf->ChannelSelForLight)  );
	
	for(;iTrigTable<9;iTrigTable++)		
		DEBUG_OUTPUT( (" trg_basg:SetBackUp[%d]=0x%x\n",iTrigTable,pConf->SetBackUp[iTrigTable])  );
	DEBUG_OUTPUT(("\n"));	
	
	DEBUG_OUTPUT( (" trg_basg:unused=0x%x\n",pConf->unused)  );

}


void Show_trg_btm(trg_btm_t * pConf)
{
	DEBUG_OUTPUT( (" trg_btm:info_len=%u\n",pConf->info_len)  );
	DEBUG_OUTPUT( (" trg_btm:module_type=0x%x\n",pConf->module_type)  );
	DEBUG_OUTPUT( (" trg_btm:Position=0x%x\n",pConf->Position)  );
	DEBUG_OUTPUT( (" trg_btm:DatamoduleFlag=0x%x\n",pConf->DatamoduleFlag)  );
	DEBUG_OUTPUT( (" trg_btm:CBLTSet=0x%x\n",pConf->CBLTSet)  );
	DEBUG_OUTPUT( (" trg_btm:L1Select=0x%x\n",pConf->L1Select)  );
	DEBUG_OUTPUT( (" trg_btm:unused=0x%x\n",pConf->unused)  );
}

void Show_trg_etm(trg_etm_t * pConf)
{
	DEBUG_OUTPUT( (" trg_etm:info_len=%u\n",pConf->info_len)  );
	DEBUG_OUTPUT( (" trg_etm:module_type=0x%x\n",pConf->module_type)  );
	DEBUG_OUTPUT( (" trg_etm:Position=0x%x\n",pConf->Position)  );
	DEBUG_OUTPUT( (" trg_etm:DatamoduleFlag=0x%x\n",pConf->DatamoduleFlag)  );
	DEBUG_OUTPUT( (" trg_etm:CBLTSet=0x%x\n",pConf->CBLTSet)  );
	DEBUG_OUTPUT( (" trg_etm:L1Select=0x%x\n",pConf->L1Select)  );
	DEBUG_OUTPUT( (" trg_etm:unused=0x%x\n",pConf->unused)  );
}

void Show_trg_ets(trg_ets_t * pConf)
{
	DEBUG_OUTPUT( (" trg_ets:info_len=%u\n",pConf->info_len)  );
	DEBUG_OUTPUT( (" trg_ets:module_type=0x%x\n",pConf->module_type)  );
	DEBUG_OUTPUT( (" trg_ets:Position=0x%x\n",pConf->Position)  );
	DEBUG_OUTPUT( (" trg_ets:DatamoduleFlag=0x%x\n",pConf->DatamoduleFlag)  );
	DEBUG_OUTPUT( (" trg_ets:unused=0x%x\n",pConf->unused)  );

}

void Show_zdd_v1721(zdd_v1721_t * pConf)
{
	DEBUG_OUTPUT( (" zdd_v1721:info_len=%u\n",pConf->info_len)  );
	DEBUG_OUTPUT( (" zdd_v1721:module_type=0x%x\n",pConf->module_type)  );
	DEBUG_OUTPUT( (" zdd_v1721:Position=0x%x\n",pConf->Position)  );
	DEBUG_OUTPUT( (" zdd_v1721:DatamoduleFlag=0x%x\n",pConf->DatamoduleFlag)  );

	DEBUG_OUTPUT( (" zdd_v1721:BaseAddr=%u\n",pConf->BaseAddr)  );
	DEBUG_OUTPUT( (" zdd_v1721:customSize=0x%x\n",pConf->customSize)  );
	DEBUG_OUTPUT( (" zdd_v1721:customOut=0x%x\n",pConf->customOut)  );
	DEBUG_OUTPUT( (" zdd_v1721:initDACs=0x%x\n",pConf->initDACs)  );
	DEBUG_OUTPUT( (" zdd_v1721:compressData=0x%x\n",pConf->compressData)  );	

	int izddch = 0;
	for(;izddch<8;izddch++)		
		DEBUG_OUTPUT( (" zdd_v1721:baseLine[%d]=0x%x\n",izddch,pConf->baseLine[izddch])  );
	DEBUG_OUTPUT(("\n"));	

	for(;izddch<8;izddch++)		
		DEBUG_OUTPUT( (" zdd_v1721:threshold[%d]=0x%x\n",izddch,pConf->threshold[izddch])  );
	DEBUG_OUTPUT(("\n"));

	for(;izddch<8;izddch++)		
		DEBUG_OUTPUT( (" zdd_v1721:nLookBack[%d]=0x%x\n",izddch,pConf->nLookBack[izddch])  );
	DEBUG_OUTPUT(("\n"));

	for(;izddch<8;izddch++)		
		DEBUG_OUTPUT( (" zdd_v1721:nLookForward[%d]=0x%x\n",izddch,pConf->nLookForward[izddch])  );
	DEBUG_OUTPUT(("\n"));
	
	DEBUG_OUTPUT( (" zdd_v1721:unused=0x%x\n",pConf->unused)  );

}


void Show_zdd_v1190(zdd_v1190_t * pConf)
{
	DEBUG_OUTPUT( (" zdd_v1190:info_len=%u\n",pConf->info_len)  );
	DEBUG_OUTPUT( (" zdd_v1190:module_type=0x%x\n",pConf->module_type)  );
	DEBUG_OUTPUT( (" zdd_v1190:Position=0x%x\n",pConf->Position)  );
	DEBUG_OUTPUT( (" zdd_v1190:DatamoduleFlag=0x%x\n",pConf->DatamoduleFlag)  );

	DEBUG_OUTPUT( (" zdd_v1190:BaseAddr=%u\n",pConf->BaseAddr)  );
	DEBUG_OUTPUT( (" zdd_v1190:WindowOffset=%u\n",pConf->WindowOffset)  );
	DEBUG_OUTPUT( (" zdd_v1190:WindowWidth=%u\n",pConf->WindowWidth)  );

}

