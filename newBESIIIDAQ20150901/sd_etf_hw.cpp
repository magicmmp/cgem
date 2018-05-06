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

#include "sd_etf.h"
#include "sd_etf_hw.h"
#include "sd_tof_hw.h"

//#include "sd_etf_hptdc_config.h"

#include "sd_etf_fee.h"
//#include "sd_etf_monitor.h"

bool g_bETFRocIntConed=FALSE;

int t_ROC_clkSource;
int t_ROC_L1Latency ;
int t_ROC_semiAutoL1Num ;
int t_ROC_autoL1Interval ;		
int t_TDIG_windowSet ;
int t_TDIG_trigger_latency;
int t_NINO_low_thres ;
int t_NINO_high_thres ;
int t_power_low_thres ;
int t_power_high_thres ;
int t_cal_amp;
int t_CLK_clkSource ;
int t_normModeSelfL1;
int t_CTTP_enable = 1;
int t_TDIG_boardEn;
int t_TDIG_CBLTEn;
int t_CLK_source;
int t_CTTP_PowerEnable[12];





void ETFIntHandler(int nArg)
{
	    //   logMsg("interrupt receive 64 OK,%d\n", 0, 0, 0, 0, 0, 0);
	semGive(g_sidInterrupt);
	g_nInterruptCounter+=g_nTrgPerInt;
}


int   ETFConfigIntCBLT()
{
	if(FALSE == g_bETFRocIntConed) 
	{
		if(intConnect (INUM_TO_IVEC(ETF_INT_VEC), (VOIDFUNCPTR)ETFIntHandler, 0) == ERROR)
	  	{
	  		printf("int connection error\n");
			Push2ErrStack(PPCFATAL+ETF_FLG+ATOM_ERR+51,0,0,0,0);		/*etf error 51*/
			return  -1;
	  	}
		g_bETFRocIntConed=TRUE;
	}
	
	
	sysIntDisable(ETF_INT_LEVEL);	
	
	if(CBLTinit(64)==-1)
	{
		printf("cblt init error\n");
		Push2ErrStack(PPCFATAL+ETF_FLG+ATOM_ERR+52,0,0,0,0);		/*etf error 52*/
		return -1;
	}
	return 0;
}


int ETFConfigCbltChain()//please check and recheck
{
	UINT32 nadr;
	UINT nGeo;
	UINT nFirstGeo=0,nLastGeo=0;
	UINT nCbltChainSum=0;
	//distinguish geo in cblt chain
	//which one is the first geo
	nFirstGeo=g_ETFDBInfo.etf_roc_header->geo;
	if( g_ETFDBInfo.etf_TDIG_header[0]->geo < nFirstGeo)
	{
		nFirstGeo=g_ETFDBInfo.etf_TDIG_header[0]->geo;
	}
	nLastGeo=g_ETFDBInfo.etf_roc_header->geo;
	if(g_ETFDBInfo.etf_TDIG_header[g_ETFDBInfo.etf_tdig_sum-1]->geo > nLastGeo)
	{
		nLastGeo=g_ETFDBInfo.etf_TDIG_header[g_ETFDBInfo.etf_tdig_sum-1]->geo;
	}
/*	
	if(g_ETFDBInfo.etf_monitor_sum==1)
	{
		if(g_ETFDBInfo.etf_monitor_header->geo< nFirstGeo)
		{
			nFirstGeo=g_ETFDBInfo.etf_monitor_header->geo;
		}
		if(g_ETFDBInfo.etf_monitor_header->geo> nLastGeo)
		{
			nLastGeo=g_ETFDBInfo.etf_monitor_header->geo;
		}
	}
*/
	printf("nFirstGeo=%d,nLastGeo=%d\n",nFirstGeo,nLastGeo);
	if( (nFirstGeo==0)||(nLastGeo==0) )
	{
		printf("get first & last geo error\n");
		Push2ErrStack(PPCFATAL+ETF_FLG+ATOM_ERR+53,0,0,0,0);		/*etf error 53*/
		return -1;
	}
	nCbltChainSum=g_ETFDBInfo.etf_tdig_sum +1;
/*	
	if(g_ETFDBInfo.etf_monitor_sum==1)
	{
		nCbltChainSum++;
	}
*/	
	printf("nCbltChainSum=%d\n",nCbltChainSum);

	taskDelay(1);
	//total module num in chain=g_ETFDBInfo.etf_tdig_sum +1 , last indx use for roc module
	for(int nModuIndx=0;nModuIndx< nCbltChainSum ;nModuIndx++) 
	{
		if(nModuIndx< g_ETFDBInfo.etf_tdig_sum)
		{
			nGeo=g_ETFDBInfo.etf_TDIG_header[nModuIndx]->geo;
			nadr=(nGeo<<19)+0x70043;
		}
		else if(nModuIndx ==g_ETFDBInfo.etf_tdig_sum)
		{
			nGeo=g_ETFDBInfo.etf_roc_header->geo;
			nadr=(nGeo<<19)+0x7ffb;
		}
		printf("nadr=0x%x\n",nadr);
		if(nGeo==nFirstGeo)
		{
			if ( -1 == WriteToVme8(nadr, VCSR, 0xa6 ) ) 
			{
				printf("Write GEO %d chain First Failed \n",nGeo); 
				Push2ErrStack(PPCFATAL+ETF_FLG+ATOM_ERR+54,0,0,0,0);		/*etf error 54*/
				return -1;
			}
			else
			{
				printf("first GEO Chain %d\n",nGeo);
			}
		}
		else if(nGeo==nLastGeo)
		{
			//if no monitor ,change here
			if ( -1 == WriteToVme8(nadr, VCSR, 0x9a ) ) 
			{
				printf("Write GEO %d chain Last Failed \n",nGeo); 
				Push2ErrStack(PPCFATAL+ETF_FLG+ATOM_ERR+55,0,0,0,0);		/*etf error 55*/
				return -1;
			}
			else
			{
				printf("last GEO Chain %d\n",nGeo);
			}
		}
		else
		{
			if ( -1 == WriteToVme8(nadr, VCSR, 0x96 ) ) 
			{
				printf("Write GEO %d chain middle Failed\n",nGeo); 
				Push2ErrStack(PPCFATAL+ETF_FLG+ATOM_ERR+56,0,0,0,0);		/*etf error 56*/
				return -1;				
			}
			else
			{
				printf("mid GEO Chain %d\n",nGeo);
			}
		}
	}
	return 0;
}


int ETFConfigTDIG()
{
	ETFGlobalReset();
	
	ETFConfigFeeBaseAddr();

	
//	ETFConfigMoniBaseAddr();
/*
	if(-1==ETFCheckFeeSerialNum())
	{
		DEBUG_OUTPUT(("ETFCheckFeeSerialNum Failed \n"));
		Push2ErrStack(PPCFATAL+ETF_FLG+ATOM_ERR+57,0,0,0,0);		//etf error 57
		return -1;
	}
*/	
/*
	if(-1==ETFCheckMoniSerialNum())
	{
		DEBUG_OUTPUT(("ETFCheckMoniSerialNum Failed \n"));
		Push2ErrStack(PPCFATAL+ETF_FLG+ATOM_ERR+58,0,0,0,0);		//etf error 58
		return -1;
	}
*/
//	ETFConfigFeeHPTDC();
//	ETFConfigMoniHPTDC();
	
	
//	ETFConfigFeeDAC();//set dac register etc
	
//	ETFConfigMonitorDAC();
//	ETFMonitorLaserSetup();
	
//	ETFConfigFeeInlTable();
//	ETFConfigMoniInlTable();


	if(-1==ETFConfigCbltChain() )
	{
		DEBUG_OUTPUT(("ETFConfigCbltChain Failed \n"));
		Push2ErrStack(PPCFATAL+ETF_FLG+ATOM_ERR+59,0,0,0,0);		/*etf error 59*/
		return -1;
	}
	
	taskDelay(10);

	if(-1==TDIG_config())
	{
		printf("TDIG_config failed\n");
		Push2ErrStack(PPCFATAL+ETF_FLG+ATOM_ERR+60,0,0,0,0);/*etf error 60*/
		return -1;
	}

	
/*	
	if(-1==ETFConfigFeeWorkMode())
	{
		DEBUG_OUTPUT(("ETFConfigFeeWorkMode Failed \n"));
		Push2ErrStack(PPCFATAL+ETF_FLG+ATOM_ERR+60,0,0,0,0);		//etf error 60		
		return -1;
	}
*/
/*	if(-1==ETFConfigMoniWorkMode())
	{
		DEBUG_OUTPUT(("ETFConfigMoniWorkMode Failed \n"));
		Push2ErrStack(PPCFATAL+ETF_FLG+ATOM_ERR+61,0,0,0,0);		//etf error 61		
		return -1;
	}
*/
	return 0;
}



int ETFConfigCLK()
{
	int base_addr;
	base_addr = 0x04f00000 ;
	printf("ETF : Clock Board Configure:\n");
	//write :0x0 inner clock, 0x1 divid clock, 0xc reserved clock, 0xe fiber input clock

	int clksource=g_ETFDBInfo.etf_clk_header->clksource;

	unsigned int data = 0x0;
	unsigned int check_data;		
	switch(clksource)
	{
		case 0:
			WriteToVme32(base_addr +0xf030,VA32, data);
			printf("----onBoard Clock      : Write to 0x%x Successfully!\n",(base_addr +0xf030));
			taskDelay(1);
			
			ReadFrmVme32(base_addr +0xf030 ,VA32,&check_data);
			if((check_data&0xf)!=0x0)
			{
				printf("CLK config: read check error, check_data = 0x%x, write data = 0x%x \n",check_data,data);
				Push2ErrStack(PPCERROR+ETF_FLG+ATOM_ERR+63,0,0,0,0);		//etf error 63
				return -1;
			}
			
			break;
		case 1:
			WriteToVme32(base_addr +0xf030, VA32, 0x1); 
			printf("----divid Clock      : Write to 0x%x Successfully!\n",(base_addr +0xf030));
			taskDelay(1);
			ReadFrmVme32(base_addr +0xf030 ,VA32,&check_data);
			if((check_data&0xf)!=0x1)
			{
				printf("CLK config: read check error, check_data = 0x%x, write data = 0x%x \n",check_data,0x1);
				Push2ErrStack(PPCERROR+ETF_FLG+ATOM_ERR+63,0,0,0,0);		/*etf error 63*/
				return -1;
			}

			break;
		case 0xf:
			data = 0xf;
			WriteToVme32(base_addr +0xf030,VA32, data);
			printf("----Slave Clock      : Write to 0x%x Successfully!\n",(base_addr +0xf030));
			taskDelay(1);
			
			ReadFrmVme32(base_addr +0xf030 ,VA32,&check_data);
			if((check_data&0xf)!=0xf)
			{
				printf("CLK config: read check error, check_data = 0x%x, write data = 0x%x \n",check_data,data);
				Push2ErrStack(PPCERROR+ETF_FLG+ATOM_ERR+63,0,0,0,0);		/*etf error 63*/
				return -1;
			}
			break;
		default:
			printf("clock source error ! \n");
			Push2ErrStack(PPCERROR+ETF_FLG+ATOM_ERR+66,0,0,0,0);		/*etf error 66*/
			return -1;

	}


	if(clksource==1)
	{
		ReadFrmVme32(base_addr +0xf020 ,VA32,&check_data);	
		taskDelay(300);

		ReadFrmVme32(base_addr +0xf0b0 ,VA32,&check_data);
		if( check_data!= 0x1)
		{
			printf("CLK config: read check error, check_data (0x%x) != 0x1\n",check_data);
			Push2ErrStack(PPCERROR+ETF_FLG+ATOM_ERR+63,0,0,0,0);		/*etf error 63*/
			return -1;
		}	
	}

	taskDelay(120);
	printf("ETF Config Clock completed\n");
	return 0;
}


int ETFGlobalReset()
{
	WriteToVme32(0xbc0270, VA32, 0x2c);	//global reset all the FEE& Monitor &ROC
	taskDelay(1);
	return 0;
}

int ETFClearRocFEE()
{
	WriteToVme32(0xbc0270, VA32, 0x24);	//clear FEE FIFO
	taskDelay(1);
}

//Self Test VME Configuration: work mode 1
int ETFROCSelfTestConfig()
{
	UINT L1Latency=g_ETFDBInfo.etf_roc_header->L1Latency;
	UINT semiAutoL1Num=g_ETFDBInfo.etf_roc_header->semiAutoL1Num;
	UINT autoL1Interval=g_ETFDBInfo.etf_roc_header->autoL1Interval;
	//UINT semiAutoL1Num=1;
	
	WriteToVme32(0xbc0270, VA32, 0x18);//set cmd register 2
	taskDelay(1);
	ETFClearRocFEE();
		
	WriteToVme32(0xbc0270, VA32, 0x01);
	taskDelay(1);
	WriteToVme32(0xbc0270, VA32, 0x10);//half auto mode
	taskDelay(1);
	
	WriteToVme32(0xbc0274, VA32, 0x0);
	taskDelay(1);
	WriteToVme32(0xbc0278, VA32, autoL1Interval);//0x4e20 0.5ms
	taskDelay(1);
	WriteToVme32(0xbc0274, VA32, 0x55);
	taskDelay(1);

	WriteToVme32(0xbc0274, VA32, 0x1);
	taskDelay(1);
	WriteToVme32(0xbc0278, VA32, autoL1Interval);
	taskDelay(1);
	WriteToVme32(0xbc0274, VA32, 0x55);
	taskDelay(1);

	WriteToVme32(0xbc0274, VA32, 0x2);
	taskDelay(1);
	WriteToVme32(0xbc0278, VA32, autoL1Interval);
	taskDelay(1);
	WriteToVme32(0xbc0274, VA32, 0x55);
	taskDelay(1);

	WriteToVme32(0xbc0274, VA32, 0x3);
	taskDelay(1);
	WriteToVme32(0xbc0278, VA32, autoL1Interval);
	taskDelay(1);
	WriteToVme32(0xbc0274, VA32, 0x55);
	taskDelay(1);

	if(-1==WriteToVme32(0x00bc0250, VA32, (g_nTrgPerInt<<25)+(L1Latency<<16)+semiAutoL1Num ))
	{
		DEBUG_OUTPUT(("Set semiAutoL1Num error\n"));
		return -1;
	}
	printf("----Write 0x%08x to 0x00bc0250 Successfully!\n",(g_nTrgPerInt<<25)+(L1Latency<<16)+semiAutoL1Num);
	WriteToVme32(0xbc0270, VA32, 0x18);//set cmd register 2
	taskDelay(1);
	printf(">>VME:: Self Test Mode Configured Successfully!\n");
	return 0;
}


int ETFROCSelfTestConfig_AutoMode()
{
	UINT L1Latency=g_ETFDBInfo.etf_roc_header->L1Latency;
	UINT semiAutoL1Num=g_ETFDBInfo.etf_roc_header->semiAutoL1Num;
	UINT autoL1Interval=g_ETFDBInfo.etf_roc_header->autoL1Interval;
	//UINT semiAutoL1Num=1;
	
	WriteToVme32(0xbc0270, VA32, 0x18);//set cmd register 2
	taskDelay(1);
	ETFClearRocFEE();
		
	WriteToVme32(0xbc0270, VA32, 0x01);
	taskDelay(1);
	WriteToVme32(0xbc0270, VA32, 0x11);//full auto mode
	taskDelay(1);
	
	WriteToVme32(0xbc0274, VA32, 0x0);
	taskDelay(1);
	WriteToVme32(0xbc0278, VA32, autoL1Interval);//0x4e20 0.5ms
	taskDelay(1);
	WriteToVme32(0xbc0274, VA32, 0x55);
	taskDelay(1);

	WriteToVme32(0xbc0274, VA32, 0x1);
	taskDelay(1);
	WriteToVme32(0xbc0278, VA32, autoL1Interval);
	taskDelay(1);
	WriteToVme32(0xbc0274, VA32, 0x55);
	taskDelay(1);

	WriteToVme32(0xbc0274, VA32, 0x2);
	taskDelay(1);
	WriteToVme32(0xbc0278, VA32, autoL1Interval);
	taskDelay(1);
	WriteToVme32(0xbc0274, VA32, 0x55);
	taskDelay(1);

	WriteToVme32(0xbc0274, VA32, 0x3);
	taskDelay(1);
	WriteToVme32(0xbc0278, VA32, autoL1Interval);
	taskDelay(1);
	WriteToVme32(0xbc0274, VA32, 0x55);
	taskDelay(1);

	if(-1==WriteToVme32(0x00bc0250, VA32, (g_nTrgPerInt<<25)+(L1Latency<<16)+semiAutoL1Num ))
	//if(-1==WriteToVme32(0x00bc0250, VA32, (g_nTrgPerInt<<25)+(L1Latency<<20)+semiAutoL1Num ))
	{
		DEBUG_OUTPUT(("Set semiAutoL1Num error\n"));
		return -1;
	}
	printf("----Write 0x%08x to 0x00bc0250 Successfully!\n",(g_nTrgPerInt<<25)+(L1Latency<<16)+semiAutoL1Num);
	WriteToVme32(0xbc0270, VA32, 0x18);//set cmd register 2
	taskDelay(1);
	printf(">>VME:: Self Test Mode Configured Successfully!\n");
	return 0;
}



int ETFROCMonitorConfig()
{
	UINT uRead;
	WriteToVme32(0xbc0270, VA32, 0x18);//set cmd register 2
	taskDelay(1);
	ETFClearRocFEE();
	
	ReadFrmVme32(0x00bc0250,VA32,&uRead);
	printf("before conf roc nTrg/Int register:0x%08x\n",uRead);
	if(-1==WriteToVme32(0x00bc0250, VA32, (g_nTrgPerInt<<25)|(uRead&0x01ffffff) )  )
	{
		printf("ROC nTrg/Int register Conf:0x%08x error\n",(g_nTrgPerInt<<25)|(uRead&0x01ffffff));	
		return -1;
	}
	taskDelay(1);
	WriteToVme32(0xbc0270, VA32, 0x2);
	taskDelay(1);

	printf("ETFROCMonitorConfig succ \n");
	
}

int ETFROCNormalConfig()
{
	UINT uRead;
	WriteToVme32(0xbc0270, VA32, 0x18);//disable ,mask L1
	taskDelay(1);
	ETFClearRocFEE();
	taskDelay(1);
	WriteToVme32(0xbc0270, VA32, 0x0);//normal data mode
	taskDelay(1);
	ReadFrmVme32(0x00bc0250,VA32,&uRead);
	printf("before conf roc nTrg/Int register:0x%08x\n",uRead);
	if(-1==WriteToVme32(0x00bc0250, VA32, (g_nTrgPerInt<<25)|(uRead&0x01ffffff) )  )
	{
		printf("ROC nTrg/Int register Conf:0x%08x error\n",(g_nTrgPerInt<<25)|(uRead&0x01ffffff));		
		return -1;
	}
	WriteToVme32(0xbc0270, VA32, 0x18);//set cmd register 2
	taskDelay(1);

	printf("ETFROCNormalConfig succ \n");
	//set mode
}

int ETF_ROC_ajust_phase()
{
	printf("**ETF ROC ajust CLK phase function here : \n");

	UINT uRead,dat;

	WriteToVme32(0xbc0270, VA32, 0x32);
	taskDelay(1);


	WriteToVme32(0xbc0280, VA32, 0x7FF1660);// 7FF1XXX
	taskDelay(1);
	
	WriteToVme32(0xbc0270, VA32, 0x33);//start phase ajustment
	taskDelay(60);

	ReadFrmVme32(0xBC0284,VA32,&uRead);//readout status code
	printf("ETF_ROC_ajust_phase : read status register data :0x%x\n",uRead);
	dat = uRead;

	unsigned int par[PTAP_STATUS_SEGNUM], p[12];
	phase_status_mask(par, PTAP_STATUS_SEGNUM);

	p[PHASE_LOS]	  = (dat >> PTAP_LOS_B) & par[PHASE_LOS];
	p[PHASE_TOUT]	  = (dat >> PTAP_TIMEOUT_B) & par[PHASE_TOUT];
	p[PHASE_RXER]	  = (dat >> PTAP_RXER_B) & par[PHASE_RXER];
	p[PHASE_TOUTSTEP] = (dat >> PTAP_TOUT_STEP_B) & par[PHASE_TOUTSTEP];
	p[PHASE_BUSY]	  = (dat >> PTAP_BUSY_B) & par[PHASE_BUSY]; 
	p[PHASE_OPESTA]   = (dat >> PTAP_OPE_STA_B) & par[PHASE_OPESTA]; 
	p[PHASE_CURRECV]  = (dat >> PTAP_CURRECV_B) & par[PHASE_CURRECV]; 
	p[PHASE_CURPLL]   = (dat >> PTAP_CURPLL_B) & par[PHASE_CURPLL]; 
	p[PHASE_DEST]	  = (dat >> PTAP_DEST_B) & par[PHASE_DEST]; 
	p[PHASE_PCNTNEW]  = (dat >> PTAP_PCNTNEW_B) & par[PHASE_PCNTNEW]; 
	p[PHASE_PCNTEFF]  = (dat >> PTAP_PCNTEFF_B) & par[PHASE_PCNTEFF]; 
	p[PHASE_PCNT]	  = (dat >> PTAP_PDIFFCNT_B) & par[PHASE_PCNT];

	printf("is now busy = %d\n", p[PHASE_BUSY]); 
	printf("operaton state = %d\n", p[PHASE_OPESTA]); 
	printf("currecv = %d\n", p[PHASE_CURRECV]); 
	printf("curpll = %d\n", p[PHASE_CURPLL]); 
	printf("dest = %d\n", p[PHASE_DEST]); 
	printf("is new = %d\n", p[PHASE_PCNTNEW]); 
	printf("is effective = %d\n", p[PHASE_PCNTEFF]); 
	printf("phasediff cnt = %d\n", p[PHASE_PCNT]); 
	printf("------------------------------------------\n");

	if((p[PHASE_BUSY]==0)&&(p[PHASE_OPESTA]==1))
	{
		WriteToVme32(0xbc0270, VA32, 0x21);//start TLK1501 synchronization
		taskDelay(1);
	}
	else
	{
		printf("ETF ROC_ajust_clock_phase error! \n");
		Push2ErrStack(PPCERROR+ETF_FLG+ATOM_ERR+65,0,0,0,0);		/*etf error 65*/	
		return -1;
	}

}

int ETF_shiftch()
{
	UINT32 i, id = 0 ;
	char comment[1000];
	int opt[100]={0};
	int iopt=0;
	FILE* fOpts;
	int icnt = 0;
	UINT32 uRead;

	UINT32 chl = 4;

	UINT32 shift_val = 0;
	UINT32 chladdr = 0;

	WriteToVme32(0xbc0270, VA32, 0x32);
	taskDelay(1);

	while(1)
	{
		if ((fOpts = fopen("ETF_ROC_shift.txt","r")) == NULL)
		{
			icnt++;
			taskDelay(60);
			if(icnt%5==0)
			{
				printf("open file ROCParas.txt failed %d times\n",icnt);
				/*break;*/
			}
		}
		else
		{
			printf("open file ROCParas.txt succ\n");
			break;
		}
	}


	while ( 1 ) {
	if ( ! fgets(comment,1000,fOpts) ) break;
	if ( comment[0]=='#' ) continue;
	sscanf(comment,"0x%x",&opt[iopt]);
	iopt++;
	}
	fclose(fOpts);

       
		/*shift_val = opt[i];*/

        	/*shiftch_helper(i,shift_val); */


	/* configure the configure register */
	chladdr = 0x100;
	WriteToVme32(0xbc0280, VA32, chladdr);
	/* read it */
	ReadFrmVme32(0xbc0280,VA32,&uRead);
	printf("0xBC0280: %8x\n", uRead);
	taskDelay(1);
	shift_val = (opt[0] & 0xFF) | ((opt[1] & 0xFF)<<8) | ((opt[2] & 0xFF)<<16) | ((opt[3] & 0xFF)<<24);
	WriteToVme32(0xbc0288, VA32, shift_val);
	taskDelay(1);
	ReadFrmVme32(0xbc0288,VA32,&uRead);
	printf("0xBC0288: %8x\n", uRead);
	taskDelay(1);

	shift_val = opt[4] & 0xFF;
	WriteToVme32(0xbc028C, VA32, shift_val);
	taskDelay(1);
	ReadFrmVme32(0xbc028C,VA32,&uRead);
	printf("0xBC028C: %8x\n", uRead);
	taskDelay(1);
	
	/* shift it ! */
	WriteToVme32(0xbc0270, VA32, 0x33);
	taskDelay(1);

	return 0;
}


int ETFROCNormalConfigNew()
{
	UINT uRead;
	
	/** add new clock phase ajust function here***/
	//if (-1==ETF_ROC_ajust_phase())
	if(-1==ETF_shiftch())
	{
		printf("ROC configure failed !\n");
		return -1;
	}

	
	WriteToVme32(0xbc0270, VA32, 0x18);//disable ,mask L1
	taskDelay(1);

	//jixl20150129
	WriteToVme32(0xbc0270, VA32, 0x37);//disable  CTTP_FAULT
	taskDelay(1);
	WriteToVme32(0xbc0270, VA32, 0x13);//disable  status signal
	taskDelay(1);

	
	ETFClearRocFEE();
	taskDelay(1);
	WriteToVme32(0xbc0270, VA32, 0x0);//normal data mode
	taskDelay(1);
	ReadFrmVme32(0x00bc0250,VA32,&uRead);
	printf("before conf roc nTrg/Int register:0x%08x\n",uRead);
	if(-1==WriteToVme32(0x00bc0250, VA32, (g_nTrgPerInt<<25)|(uRead&0x01ffffff) )  )
	{
		printf("ROC nTrg/Int register Conf:0x%08x error\n",(g_nTrgPerInt<<25)|(uRead&0x01ffffff));		
		return -1;
	}
	WriteToVme32(0xbc0270, VA32, 0x18);//set cmd register 2
	taskDelay(1);

	printf("ETFROCNormalConfig succ \n");
	//set mode
}

/* helper function to retrive the configure parameters from the cf.
   @name: the name of the cf file
   @opt: the array store the retrived parames
   @return: the effective array length if succeed, else -1
*/
int ETFretrievePrams(const char *name, UINT32 *opt)
{
	char comment[1000];
	FILE* fOpts;
	int icnt = 0;

	while(1)
	{
		if ((fOpts = fopen(name,"r")) == NULL)
		{
			icnt++;
			taskDelay(60);
			if(icnt%5==0)
			{
				printf("open file %s %d times\n", name, icnt);
				/*break;*/
			}
		}
		else
		{
			printf("open file %s succ\n", name);
			break;
		}
	}

	icnt = 0;
	while(1) {
		if( !fgets(comment,1000,fOpts) ) break;
		if( comment[0]=='#' ) continue;
		sscanf(comment,"0x%x",&opt[icnt]);
		icnt++;
	}
	fclose(fOpts);
	
	return icnt;	
}

/* helper to simplify the code */
UINT32 ETFprintReg(unsigned addr)
{
	UINT32 d;
	ReadFrmVme32(addr,VA32,&d);
	printf("0x%08x = %08x\n", addr, d);
	return d;
}




int ETFROCmtlkConfig()
{
	UINT32 opt[6]; /* retrieve them form db or cf */
	UINT32 addr = 0xBC025C;
	UINT32 val = 0;

	char *filename;
	if(g_nCrateID == 224)
		filename = "ETF_ROC_mtlk_224.txt";
	else
		filename = "ETF_ROC_mtlk_225.txt";

	
	//if(retrievePrams("TOF_ROC_mtlk.txt", opt) != 6)
	if(ETFretrievePrams(filename, opt) != 6)
		return -1;
	
	val = (opt[0] & 0x7) |
		  (opt[1] & 0x7) << 3 |
		  (opt[2] & 0x7) << 6 |
		  (opt[3] & 0x7) << 9 |
		  (opt[4] & 0x7) << 12 |
		  (opt[5] & 0xF) << 15;

	printf("in mtlkConfig :opt[5] = 0x%x \n",opt[5]);
	WriteToVme32(addr, VA32, val); /* confgure it */
	ETFprintReg(addr);

	return 0;
}

/* ROC B1 fibre channel(receive trigger sys control signals) delay unc-remove */
int ETFshiftch()
{
	char comment[1000];
	UINT32 opt[100]={0};
	int iopt=0;
	FILE* fOpts = NULL;
	int icnt = 0;
	UINT32 uRead;

	UINT32 shift_val = 0;
	UINT32 val = 0;

	char *filename;
	if(g_nCrateID == 224)
		filename = "ETF_ROC_shift_224.txt";
	else
		filename = "ETF_ROC_shift_225.txt";

	while(1)
	{
		//if ((fOpts = fopen("TOF_ROC_shift.txt","r")) == NULL)
		if ((fOpts = fopen(filename,"r")) == NULL)
		{
			icnt++;
			taskDelay(60);
			if(icnt%5==0)
			{
				printf("open file TOF_ROC_shift.txt failed %d times\n",icnt);
				/*break;*/
			}
		}
		else
		{
			printf("open file TOF_ROC_shift.txt succ\n");
			break;
		}
	}


	while ( 1 ) {
	if ( ! fgets(comment,1000,fOpts) ) break;
	if ( comment[0]=='#' ) continue;
	sscanf(comment,"0x%x",&opt[iopt]);
		iopt++;
	}
	fclose(fOpts);

	/* configure the configure register */
	ReadFrmVme32(0xbc0280,VA32,&uRead);
	/* first get the mask */
	for(icnt=0; icnt < 5; ++icnt){
		if(opt[icnt] == 0xFF){ /* means current channel no shift */
			opt[icnt] = (uRead >> 8*icnt) & 0xFF; /* reserve previous value */
			val = val | (1u << icnt);	/* mask the shift function */
		}
	}

	/* pack the configure register */
	for(int i=0;i<7;i++)
		printf("in shiftch :opt[%d] = 0x%x \n",i,opt[i]);

	val = val | 0x100 | ((opt[5] & 0x1) << 12) | ((opt[6] & 0xff) << 14);
	WriteToVme32(0xbc0280, VA32, val);
	/* read it for debugging purpose */
	ETFprintReg(0xBC0280);

	/* configure the shift value registers */
	shift_val = (opt[0] & 0xFF) | 
				((opt[1] & 0xFF)<<8) | 
				((opt[2] & 0xFF)<<16) | 
				((opt[3] & 0xFF)<<24);
	WriteToVme32(0xbc0288, VA32, shift_val);
	ETFprintReg(0xBC0288);
	taskDelay(1);

	shift_val = opt[4] & 0xFF;
	WriteToVme32(0xbc028C, VA32, shift_val);
	ETFprintReg(0xBC028C);
	taskDelay(1);
	
	/* shift it ! */
	WriteToVme32(0xbc0270, VA32, 0x33);
	taskDelay(1);

	return 0;
}



int ETFROCNormalConfigV3()
{
	printf("********V3 ETF ROC normal configuration code here : \n");

	UINT uRead;

	WriteToVme32(0xbc0270, VA32, 0x18);//stop the fctrl
	taskDelay(1);

	/* confgure the MTLK */
	ETFROCmtlkConfig();

	/* configure stlk uncertaintity logic and L1 output phase */
	if(-1==ETFshiftch())
	{
		printf("ROC configure failed !\n");
		return -1;
	}

	//WriteToVme32(0xBC0260, VA32, (0x1 << 10));//disable CTTP fault
	//taskDelay(1);

	//WriteToVme32(0xbc0270, VA32, 0x13);//disable  status signal	
	//WriteToVme32(0xbc025C, VA32, 0xFF9);//has been put in ROCmtlkConfig
	
	ETFClearRocFEE();
	taskDelay(1);
	WriteToVme32(0xbc0270, VA32, 0x0);//normal data mode
	taskDelay(1);
	ReadFrmVme32(0x00bc0250,VA32,&uRead);
	printf("before conf roc nTrg/Int register:0x%08x\n",uRead);
	if(-1==WriteToVme32(0x00bc0250, VA32, (g_nTrgPerInt<<25)|(uRead&0x01ffffff) )  )
	{
		printf("ROC nTrg/Int register Conf:0x%08x error\n",(g_nTrgPerInt<<25)|(uRead&0x01ffffff));
		Push2ErrStack(PPCFATAL+TOF_FLG+ATOM_ERR+65,0,0,0,0);		/*tof error 65*/		
		return -1;
	}
	WriteToVme32(0xbc0270, VA32, 0x18);//set cmd register 2
	taskDelay(1);

	return 0;


}

int ETFConfigROC()
{
	//WriteToVme32(0xbc0250, VA32, ((g_nTrgPerInt<<25)+(128<<16)+16));
	UINT uTemp;
	
	//write fask control board, 0x16 inner clock,0x17 out clokc
	if(g_ETFDBInfo.etf_roc_header->clksource==0)
	{
		if(WriteToVme32(0xbc0270, VA32, 0x16)==0)/*inner clk,general use for hw test*/
		printf("ETFConfigROC inner Clock    : Write 0x00000016 to 0x00bc0270 Successfully!\n");
	}
	else if (g_ETFDBInfo.etf_roc_header->clksource==1)
	{
		if(WriteToVme32(0xbc0270, VA32, 0x17)==0)/*external clk,clk from clk module*/
		printf("ETFConfigROC ext clk: Write 0x00000017 to 0x00bc0270 Successfully!\n");
	}
	else
	{
		printf("config clk source error\n");
		Push2ErrStack(PPCFATAL+ETF_FLG+ATOM_ERR+64,0,0,0,0);		/*etf error 64*/		
		return -1;
	}

	UINT writeTest=0xeeffffff;
	WriteToVme32(0x00bc0290, VA32,writeTest);
	ReadFrmVme32(0x00bc0290,VA32,&uTemp);
	if(writeTest!=uTemp)
	{
		printf("ETFConfigROC: check test reg err,\n write data 0x%x while read data 0x%x \n",writeTest,uTemp);
		Push2ErrStack(PPCERROR+ETF_FLG+ATOM_ERR+67,0,0,0,0);		/*etf error 67*/		
		return -1;
	}

	switch(g_nRunMode)
	{
		case NOMALMODE:
			if(g_ETFDBInfo.etf_ppc_header->NormModeSelfL1)
			{
				ETFROCSelfTestConfig_AutoMode();
				printf("ETFROCSelfTestConfig \n");
			}
			else
			{
				ETFROCNormalConfigV3();
			}
			break;
		case PEDMODE:
			break;
		case CALIBRATION:
			ETFROCSelfTestConfig();
			break;
		case MONITOR:
			ETFROCSelfTestConfig_AutoMode();
			printf("ETFROCSelfTestConfig \n");
			break;
		case BASELINE:
		case WAVEMODE:		
			break ;
		default:
			Push2ErrStack(PPCFATAL+ETF_FLG+ATOM_ERR+9,0,0,0,0);		/*etf error 9*/		
			break ;
	}

	if(g_ETFDBInfo.etf_roc_header->cttp_enable)
	{
		if(-1==CTTP_Config())
		{
			printf("CTTP_config failed\n");
			Push2ErrStack(PPCERROR+ETF_FLG+ATOM_ERR+68,0,0,0,0);		/*etf error 68*/
			return -1;
		}
	}
	
	return 0;
}

/* this function is used to get the HPTDC trig lattency setup value */
int __inv_val(int ival, int w)
{
	int fval = 0, i = 0;
	/* pre-process the input value */
	ival = ival & ((1 << w)-1);
	for(i = 0; i < w; ++i)
		fval |= ((ival >> i) & 0x1) << (w-1-i);

	return fval;
}


int ETFConfigAll()
{

	/**get temp config paras**/
	char comment[1000];
	int opt[100]={0};
	int iopt=0;
	FILE* fOpts;
	int icnt = 0;
	int nRetryTimes = 0;
	while(1)
	{
		if(g_nCrateID==224)
		{
			if ((fOpts = fopen("ETFParas_224.txt","r")) == NULL)
			{
				icnt++;
				taskDelay(60);
				if(icnt%5==0)
				{
					printf("open file ETFParas_224.txt failed %d times\n",icnt);
					/*break;*/
				}
			}
			else
			{
				printf("open file ETFParas_224.txt succ\n");
				break;
			}
		}
		else if(g_nCrateID==225)
		{
			if ((fOpts = fopen("ETFParas_225.txt","r")) == NULL)
			{
				icnt++;
				taskDelay(60);
				if(icnt%5==0)
				{
					printf("open file ETFParas_225.txt failed %d times\n",icnt);
					/*break;*/
				}
			}
			else
			{
				printf("open file ETFParas_225.txt succ\n");
				break;
			}
		}
		else
		{
			printf("crate ID error! \n");
			return -1;
		}
	}

	
	while ( 1 ) {
	if ( ! fgets(comment,1000,fOpts) ) break;
	if ( comment[0]=='#' ) continue;
	sscanf(comment,"0x%x",&opt[iopt]);
	iopt++;
	}
	fclose(fOpts);
	
	t_ROC_clkSource = opt[0];
	t_ROC_L1Latency = opt[1];
	t_ROC_semiAutoL1Num = opt[2];
	t_ROC_autoL1Interval = opt[3]; 	
	t_TDIG_windowSet = opt[4];
	t_TDIG_trigger_latency = opt[5];
	t_CTTP_enable = opt[6];
	t_NINO_low_thres = opt[7];
	t_NINO_high_thres = opt[8];
	t_power_low_thres = opt[9];
	t_power_high_thres = opt[10];
	t_cal_amp = opt[11];
	t_normModeSelfL1 = opt[12];
	t_TDIG_boardEn = opt[13];
	t_TDIG_CBLTEn = opt[14];
	t_CLK_source = opt[15];	
	for(int i=0;i<12;i++)
	{
		t_CTTP_PowerEnable[i] = opt[16+i];
	}



	//g_ETFDBInfo.etf_ppc_header->NormModeSelfL1 = t_normModeSelfL1;
	printf("t_normModeSelfL1 = %d\n",g_ETFDBInfo.etf_ppc_header->NormModeSelfL1);
	if(g_ETFDBInfo.etf_ppc_header->NormModeSelfL1)//calib mode
	{
			icnt = 0;
			iopt = 0;
			if ((fOpts = fopen("ETFParas_CALIB.txt","r")) == NULL)
			{
				icnt++;
				taskDelay(60);
				if(icnt%5==0)
				{
					printf("open file ETFParas_CALIB.txt failed %d times\n",icnt);
					/*break;*/
				}
			}
			else
			{
				printf("open file ETFParas_CALIB.txt succ\n");
			}
			
			while ( 1 ) {
			if ( ! fgets(comment,1000,fOpts) ) break;
			if ( comment[0]=='#' ) continue;
			sscanf(comment,"0x%x",&opt[iopt]);
			iopt++;
			}
			fclose(fOpts);

			t_NINO_low_thres = opt[0];
			t_NINO_high_thres = opt[1];
			t_cal_amp = opt[2];
			t_CLK_source = opt[3];	
			/**/
			t_TDIG_windowSet = 0x1194;
			t_TDIG_trigger_latency = 0x1194;	
			printf("in calib mode: low_thr = 0x%x, high_thr = 0x%x, cal_amp = 0x%x, clk_source = 0x%x \n",
				t_NINO_low_thres, t_NINO_high_thres, t_cal_amp, t_CLK_source);
			
	
	}

	


	g_ETFDBInfo.etf_roc_header->clksource = t_ROC_clkSource;
	g_ETFDBInfo.etf_roc_header->L1Latency = t_ROC_L1Latency;
	g_ETFDBInfo.etf_roc_header->semiAutoL1Num = t_ROC_semiAutoL1Num;
	g_ETFDBInfo.etf_roc_header->autoL1Interval = t_ROC_autoL1Interval;//4KHZ	
	printf("ROC semiAutoL1Num is set to %d\n",g_ETFDBInfo.etf_roc_header->semiAutoL1Num);
	unsigned int ROC_freq;
	ROC_freq = (1000000000/(g_ETFDBInfo.etf_roc_header->autoL1Interval * 25)) ;
	printf("ROC L1 frequency is set to %d\n",ROC_freq);
	g_ETFDBInfo.etf_roc_header->cttp_enable = t_CTTP_enable;
	printf("CTTP enable set to %d \n",t_CTTP_enable);

	
	int ival, fval;
	/*for trigger latency transfer*/
	unsigned int delay;
	delay = t_TDIG_trigger_latency ;
	printf("TDIG trigger_latency data value set to 0x%x\n", delay);
	ival = 4096 - (delay / 25);
	ival = ival & 0xFFF;
	/* reverse the bit order */
	fval = __inv_val(ival, 12);
	/* print the conclusion */
	printf("TDIG trigger_latency register value set to 0x%x\n", fval);
	t_TDIG_trigger_latency = fval;


	/*for sample window transfer*/
	unsigned int window;
	window = t_TDIG_windowSet;
	printf("TDIG window data value set to 0x%x\n", window);
	ival = (window / 25);
	ival = ival & 0xFFF;
	/* reverse the bit order */
	fval = __inv_val(ival, 12);
	/* print the conclusion */
	printf("TDIG window register value set to 0x%x\n", fval);
	t_TDIG_windowSet = fval;

	

	//DEBUG_OUTPUT(("TDIG windowSet :0x%x\n",g_ETFDBInfo.etf_TDIG_header[0]->windowSet));

	for(int moduleIndx=0;moduleIndx<g_ETFDBInfo.etf_tdig_sum;moduleIndx++)
	{
		g_ETFDBInfo.etf_TDIG_header[moduleIndx]->trigger_latency = t_TDIG_trigger_latency ;
		g_ETFDBInfo.etf_TDIG_header[moduleIndx]->windowSet = t_TDIG_windowSet;

	}


	g_ETFDBInfo.etf_clk_header->clksource = t_CLK_source;
	DEBUG_OUTPUT(("CLK clkSource :%d\n",g_ETFDBInfo.etf_clk_header->clksource));


/**read paras end***/	

	if(g_ETFDBInfo.etf_clk_sum==1)
	{
		if(-1==ETFConfigCLK())
		{
			DEBUG_OUTPUT(("ETFConfigCLK error\n"));
			Push2ErrStack(PPCFATAL+ETF_FLG+ATOM_ERR+69,0,0,0,0);		//etf error 69	
			return -1;
		}
	}
	else
	{
		DEBUG_OUTPUT(("Have no Clk config infor, config fail\n"));
		Push2ErrStack(PPCWARNING+ETF_FLG+ATOM_ERR+70,0,0,0,0);		//etf error 70
		//return -1;
	}
	
	if(-1==ETFConfigTDIG())
	{
		DEBUG_OUTPUT(("ETFConfigTDIG error\n"));
		Push2ErrStack(PPCFATAL+ETF_FLG+ATOM_ERR+71,0,0,0,0);		//etf error 71	
		return -1;
	}
	

	if(-1==ETFConfigROC())
	{
		DEBUG_OUTPUT(("ETFConfigROC error\n"));
		Push2ErrStack(PPCFATAL+ETF_FLG+ATOM_ERR+72,0,0,0,0);		//etf error 72		
		return -1;
	}

	ETFClearRocFEE();
	
	return 0;
}


int ETFSelfTestAmp_Setup(UINT32 SelfTestAmp)
{
	//must calculate it
	unsigned int nBaseAddr=0;
	SelfTestAmp=SelfTestAmp*4096/(2500*2);
	
	for(int nModuIndx=0;nModuIndx<g_ETFDBInfo.etf_tdig_sum;nModuIndx++)  
	{   
			UINT nGeo=g_ETFDBInfo.etf_TDIG_header[nModuIndx]->geo;
			nBaseAddr=(nGeo<<20);
			if ( -1 == WriteToVme32(nBaseAddr+0x4000c, VA32, SelfTestAmp) )  
			{
				//SelfTest Amp Setup fail
				printf(">>VME:: SelfTest Amp Setup: Write SelfTestAmp 0x%08x to 0x%08x fail \n",SelfTestAmp,nBaseAddr+0x4000c);
				return -1;
			} 
			else 
			{
				printf(">>VME:: SelfTest Amp Setup: Write SelfTestAmp 0x%08x to 0x%08x success \n",SelfTestAmp,nBaseAddr+0x4000c);  
			}
			taskDelay(1);
	}
	//monitor change amp
	nBaseAddr=(g_uEtfMonitorGeo<<20);
	if ( -1 == WriteToVme32(nBaseAddr+0x4000c, VA32, SelfTestAmp) )  
	{
		//SelfTest Amp Monitor Write fail
		printf(" SelfTest Amp Monitor Write 0x%08x to 0x%08x fail \n",SelfTestAmp,nBaseAddr+0x4000c);	
		return -1;
	} 
	return 0;
}

void ETFGenL1()//only for normal mode selfL1 and Calibration mode 
{
	int			nTimeIdx=0;
	int			nPointIdx=0;
	
	semTake(g_sidSATR, WAIT_FOREVER) ;
	g_bTrgStop=false;
	switch(g_nRunMode)
	{
		case NOMALMODE:
/*			
			if(g_ETFDBInfo.etf_ppc_header->NormModeSelfL1)
			{
				while(1)
				{
					WriteToVme32(0xbc0270, VA32, 0x19);
					taskDelay(1);
					if(g_bTrgStop)
					{
						ETFRocStop();
						break;
					}
				}
			}
*/	
			if(g_ETFDBInfo.etf_ppc_header->NormModeSelfL1)
			{
				WriteToVme32(0xbc0270, VA32, 0x19);
				while(1)
				{
					
					taskDelay(1);
					if(g_bTrgStop)
					{
						ETFRocStop();
						break;
					}
				}
			}

			break;
		case CALIBRATION:
			while(1)
			{
				for(nTimeIdx=0;nTimeIdx<g_nL1Count;nTimeIdx+=g_ETFDBInfo.etf_roc_header->semiAutoL1Num)
				{
					if ( -1 == WriteToVme32(0xbc0270, VA32, 0x19) ) 
					{
						printf("\n Write 0xbc0270 gen L1 error\n");
						return ; 
					} 
					taskDelay(2);
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
			WriteToVme32(0xbc0270, VA32, 0x19);
			while(1)
			{
				
				taskDelay(1);
				if(g_bTrgStop)
				{
					ETFRocStop();
					break;
				}
			}
			break;			
		default:
			DEBUG_OUTPUT(("ETF GenL1 only for normModeSelfL1 and Calibration\n"));
			Push2ErrStack(PPCFATAL+ETF_FLG+ATOM_ERR+73,0,0,0,0);		/*etf error 73*/	
			break ;
	}

	nETFGenL1TaskId=0;
}

void ETFRocStartL1()
{
	WriteToVme32(0xbc0270, VA32, 0x19);//begin work
}

void ETFRocStop()
{
	WriteToVme32(0xbc0270, VA32, 0x18);
}

//specail scan for check,not run in stat chart,only run separately
void ETFScanBoardID()
{
	UINT nAddr;
	UINT nData;
	for(int nGeo=3;nGeo<21;nGeo++)
	{
		WriteToVme8((nGeo<<19)+0x7ffd8+0x3, VCSR, (nGeo&0x0f)<<4);  
		WriteToVme8((nGeo<<19)+0x7ffdc+0x3, VCSR, (nGeo&0xf0)>>4);
		taskDelay(2);
		nAddr=nGeo<<20;
		nData=0;
		ReadFrmVme32(nAddr+0x40008,VA32,&nData);//distinguish wether is FEE board,read register
		if(nData!=0xffffffff)
		{
			nData=0;
			ReadFrmVme32(nAddr+0x28,VA32,&nData);
			printf("FEE GEO=%d,BdID=0x%08x\n",nGeo,nData);
		}
	}
}

/***For CTTP configure****/
int CTTP_Config()
{
	int data = 0;
	UINT uRead;
	int iStep;
	int i=0;

//	int CTTPModNum = 1;


	int low_threshold_nino = t_NINO_low_thres;
	int high_threshold_nino = t_NINO_high_thres;
	int low_threshold_power = t_power_low_thres;
	int high_threshold_power = t_power_high_thres;
	int cal_amp = t_cal_amp;
	printf("low_threshold_nino : 0x%x\n",low_threshold_nino);
	printf("high_threshold_nino : 0x%x\n",high_threshold_nino);
	printf("low_threshold_power : 0x%x\n",low_threshold_power);
	printf("high_threshold_power : 0x%x\n",high_threshold_power);
	printf("cal_amp : 0x%x\n",cal_amp);

	if((t_NINO_low_thres>0xfff)||(t_NINO_high_thres>0xfff))
	{
		printf("NINO threshold couldn't be larger than 0xfff \n");
		return -1;
	}
	if((t_power_low_thres>0xffff)||(t_power_high_thres>0xffff))
	{
		printf("power threshold couldn't be larger than 0xffff \n");
		return -1;
	}
	if(cal_amp > 0xfff)
	{
		printf("cal_amp couldn't be larger than 0xfff \n");
		return -1;
	}

	int power_ena[12];
	for(i=0;i<12;i++)
	{
		power_ena[i] = t_CTTP_PowerEnable[i];
		//if( (power_ena[i]) !=0xffff)
			printf("power enable channel in CTTP : power_ena[%d] = 0x%x \n ",i,power_ena[i]);
	}
	
	int CTTPModNum = 0;

	for(CTTPModNum = 0; CTTPModNum<4;CTTPModNum++)
	{
		/*check connection**/
		data = (1<<31)+(CTTPModNum<<25)+(1<<17);
		WriteToVme32(0xbc0240, VA32, data);
		taskDelay(60);
		ReadFrmVme32(0xbc0244,VA32,&uRead);
		if((uRead&0xffff)!=0x0518)
		{
			printf("CTTP_Config : connection error! uRead = 0x%x \n",uRead);
			Push2ErrStack(PPCERROR+ETF_FLG+ATOM_ERR+74,0,0,0,0);		/*etf error 74*/	
			return -1;
		}

		/*config NINO low threshold**/
		printf("CTTP config: config NINO low threshold\n");
		for(i=0;i<9;i++)
		{
			data = (CTTPModNum<<25) + ((0x20+(i*4))<<16) +(low_threshold_nino <<4);
			WriteToVme32(0xbc0240, VA32, data);
			taskDelay(1);
			printf("write 0x%x to 0xbc0240 \n",data);

			/*check*/
/*			
			data = 0x82000000 + ((0x20+(i*4))<<16);
			WriteToVme32(0xbc0240, VA32, data);
			printf("check : write 0x%x to 0xbc0240 \n",data);
			taskDelay(1);
			ReadFrmVme32(0xbc0244,VA32,&uRead);
			printf("read data is 0x%x\n",uRead);*/
			
		}

		/*config NINO high threshold**/
		printf("CTTP config: config NINO high threshold\n");
		for(i=0;i<9;i++)
		{
			data = (CTTPModNum<<25) + ((0x22+(i*4))<<16) +(high_threshold_nino <<4);
			WriteToVme32(0xbc0240, VA32, data);
			taskDelay(1);
			printf("write 0x%x to 0xbc0240 \n",data);

			/*check*/
			/*
			data = 0x82000000 + ((0x22+(i*4))<<16);
			WriteToVme32(0xbc0240, VA32, data);
			printf("check : write 0x%x to 0xbc0240 \n",data);
			taskDelay(1);
			ReadFrmVme32(0xbc0244,VA32,&uRead);
			printf("read data is 0x%x\n",uRead);		*/
		}	


		/*config cal amp*/
		printf("CTTP config: config cal amp\n");
		for(i=0;i<9;i++)
		{
			data = (CTTPModNum<<25) + ((0x0A+(i*2))<<16) +(cal_amp <<4);
			WriteToVme32(0xbc0240, VA32, data);
			taskDelay(1);
			printf("write 0x%x to 0xbc0240 \n",data);

			/*check*/
			/*
			data = 0x82000000 + ((0x0A+(i*2))<<16);
			WriteToVme32(0xbc0240, VA32, data);
			printf("check : write 0x%x to 0xbc0240 \n",data);
			taskDelay(1);
			ReadFrmVme32(0xbc0244,VA32,&uRead);
			printf("read data is 0x%x\n",uRead);	*/	
		}	

		/*config power high threshold**/
		printf("CTTP config: config power high threshold\n");
		for(i=0;i<36;i++)
		{
			data = (CTTPModNum<<25) + ((0xa0 +(i*4))<<16) +(high_threshold_power);
			WriteToVme32(0xbc0240, VA32, data);
			//printf("write 0x%x to 0xbc0240 \n",data);
		}	

		/*enable power output**/
		printf("CTTP config: enable power output\n");
		data = (CTTPModNum<<25) + (0x44<<16) + power_ena[0+(3*CTTPModNum)];/*15:0*/
		WriteToVme32(0xbc0240, VA32, data);
		//printf("write 0x%x to 0xbc0240 \n",data);

		data = (CTTPModNum<<25) + (0x46<<16) + power_ena[1+(3*CTTPModNum)];/*31:16*/
		WriteToVme32(0xbc0240, VA32, data);
		//printf("write 0x%x to 0xbc0240 \n",data);

		data = (CTTPModNum<<25) + (0x48<<16) + power_ena[2+(3*CTTPModNum)];/*35:32*/
		WriteToVme32(0xbc0240, VA32, data);
		//printf("write 0x%x to 0xbc0240 \n",data);

		//20171127 add to disable CTTP power alarm
		/*disable power alarm**/
		printf("CTTP config: disable power alarm\n");
		data = (CTTPModNum<<25) + (0x92<<16) + power_ena[0+(3*CTTPModNum)];/*15:0*/
		WriteToVme32(0xbc0240, VA32, data);
		//printf("write 0x%x to 0xbc0240 \n",data);

		data = (CTTPModNum<<25) + (0x94<<16) + power_ena[1+(3*CTTPModNum)];/*31:16*/
		WriteToVme32(0xbc0240, VA32, data);
		//printf("write 0x%x to 0xbc0240 \n",data);

		data = (CTTPModNum<<25) + (0x96<<16) + power_ena[2+(3*CTTPModNum)];/*35:32*/
		WriteToVme32(0xbc0240, VA32, data);
		//printf("write 0x%x to 0xbc0240 \n",data);

		/*clear power alarm**/
		printf("CTTP config: clear power alarm\n");
		data = (CTTPModNum<<25) + (0x98<<16) + 0;/*15:0*/
		WriteToVme32(0xbc0240, VA32, data);
		//printf("write 0x%x to 0xbc0240 \n",data);

		data = (CTTPModNum<<25) + (0x9a<<16) + 0;/*31:16*/
		WriteToVme32(0xbc0240, VA32, data);
		//printf("write 0x%x to 0xbc0240 \n",data);

		data = (CTTPModNum<<25) + (0x9c<<16) + 0;/*35:32*/
		WriteToVme32(0xbc0240, VA32, data);
		//printf("write 0x%x to 0xbc0240 \n",data);		

		


		/*config power low threshold**/
		printf("CTTP config: config power low threshold\n");
		for(i=0;i<36;i++)
		{
			data = (CTTPModNum<<25) + ((0x9E +(i*4))<<16) +(low_threshold_power);
			WriteToVme32(0xbc0240, VA32, data);
			//printf("write 0x%x to 0xbc0240 \n",data);
		}	
		taskDelay(120);
	}
	return 0;
	
}


/**for TDIG module config**/

int TDIG_resetCPLD(unsigned int base_addr)
{

	if( -1==WriteToVme32(base_addr + 0x800cc,VA32, 5) )
	{
		printf("TDIG error while write to VME during resetCPLD\n");
		return -1;
	}
	printf("resetCPLD : write data 0x%x to addr 0x%x succ \n",5,base_addr + 0x800cc);
	//taskDelay(120);
	return 0;
	
}


int TDIG_configCBLTChain(unsigned int nGeo,char *position)
{
	unsigned int nadr = (nGeo<<19)+0x70043 ;
	if(position=="first")
	{
		if ( -1 == WriteToVme8(nadr, VCSR, 0xa6 ) ) 
		{
			printf("TDIG Write GEO %d chain First Failed \n",nGeo); 
			return -1;
		}
		else
		{
			printf("first GEO Chain %d\n",nGeo);
		}
	}
	else if(position=="last")
	{
		if ( -1 == WriteToVme8(nadr, VCSR, 0x9a ) ) 
		{
			printf("TDIG Write GEO %d chain Last Failed \n",nGeo); 
			return -1;
		}
		else
		{
			printf("last GEO Chain %d\n",nGeo);
		}
	}
	else if(position=="only")
	{
		if ( -1 == WriteToVme8(nadr, VCSR, 0xaa ) ) 
		{
			printf("TDIG Write GEO %d chain only Failed \n",nGeo); 
			return -1;
		}
		else
		{
			printf("only GEO Chain %d\n",nGeo);
		}
	}		
	else if(position=="middle")
	{
		if ( -1 == WriteToVme8(nadr, VCSR, 0x96 ) ) 
		{
			printf("TDIG Write GEO %d chain middle Failed\n",nGeo); 
			return -1;				
		}
		else
		{
			printf("***********mid GEO Chain %d\n",nGeo);
		}
	}
	else
	{
		printf("TDIG module position error\n");
		Push2ErrStack(PPCFATAL+ETF_FLG+ATOM_ERR+75,0,0,0,0);		/*etf error 75*/
		return -1;
	}

	taskDelay(6);
	return 0;
}

int TDIG_ETFConfigFeeBaseAddr(unsigned int nGeo)
{
	WriteToVme8((nGeo<<19)+0x7ffd8+0x3, VCSR, (nGeo&0x0f)<<4);  /*nGeo low 4 bit write to one register*/
	WriteToVme8((nGeo<<19)+0x7ffdc+0x3, VCSR, (nGeo&0xf0)>>4);	/*nGeo high 4 bit write to another register*/
	printf("write:0x%08x to address: 0x%08x\n",nGeo<<4,(nGeo<<19)+0x7ffdc+0x3);
	return 0;
}

int ModifyParas(unsigned int base_addr, UINT trigger_latency, UINT windowSet)
{
	int data = (trigger_latency<<12) + windowSet + 0x9000000;
	if( -1==WriteToVme32(base_addr +0x6c,VA32, data) )
	{
		printf("error while write to VME during ModifyParas\n");
		return -1;
	}
	printf("step 2 : write data 0x%x to addr 0x%x \n", data,base_addr +0x6c);
	taskDelay(1);

	return 0;	

}


int enableModify(unsigned int base_addr)
{
	if( -1==WriteToVme32(base_addr +0x8,VA32, 0x85) )
	{
		printf("error while write to VME during enableModify\n");
		return -1;
	}
	printf("step 3 : write data 0x%x to addr 0x%x \n", 0x85,base_addr +0x8);
	taskDelay(1);

	return 0;
}


int triggerSourceSelect(unsigned int base_addr, UINT trigger_source)/*front 1; back 0*/
{
	int data;
	int uTemp;
	if (trigger_source==1)
		data = 0x20;
	else
		data = 0x4020;

	if( -1==WriteToVme32(base_addr +0x0,VA32, data) )
	{
		printf("error while write to VME during triggerSourceSelect\n");
		return -1;
	}

	ReadFrmVme32(base_addr +0x0,VA32,&uTemp);
	if(uTemp!=data)
	{
		printf("triggerSourceSelect err, write data 0x%x while read data 0x%x \n",data,uTemp);	
		return -1;
	}

	
	printf("step 4 : write data 0x%x to addr 0x%x \n", data,base_addr +0x0);
	taskDelay(1);

	return 0;
	
}

int CBLTParasSet(unsigned int base_addr, UINT interrupt_enable, UINT errDataEnb)
{
	int data;
	data = (interrupt_enable<<6) + (errDataEnb<<5) + g_nTrgPerInt;
	if( -1==WriteToVme32(base_addr +0xc,VA32, data) )
	{
		printf("error while write to VME during CBLTParasSet\n");
		return -1;
	}	

	printf("step 5 : write data 0x%x to addr 0x%x \n", data,base_addr +0xc);
	taskDelay(1);

	return 0;

	
}

int HPTDCopen(unsigned int base_addr)
{
	if( -1==WriteToVme32(base_addr +0x8,VA32, 0x82) )
	{
		printf("error while write to VME during HPTDCopen\n");
		return -1;
	}
	printf("step 6 : write data 0x%x to addr 0x%x \n", 0x82,base_addr +0x8);
	taskDelay(10);

	return 0;
}

int ClearFIFO(unsigned int base_addr)
{
	if( -1==WriteToVme32(base_addr +0x8,VA32, 0x2) )
	{
		printf("error while write to VME during ClearFIFO\n");
		return -1;
	}
	taskDelay(6);
	return 0;
}

int TDIG_clrPurged()
{
	/*clear purged */
	if ( -1 == MCSTwrite(MCST_ADRS, 32, 0x1) ) 
	{ 
		printf("\n TDIG_clrPurged failed");
		return -1;
	}
	return 0;
}


int TDIG_config()
{
	UINT32 base_addr;
	
	for(int nModuIndx=0;nModuIndx<g_ETFDBInfo.etf_tdig_sum;nModuIndx++)  
	{
		UINT nGeo = g_ETFDBInfo.etf_TDIG_header[nModuIndx]->geo;
		base_addr = (nGeo<<20);
		if (-1==TDIG_resetCPLD(base_addr))
		{
			printf("TDIG_resetCPLD failed\n");
			Push2ErrStack(PPCFATAL+ETF_FLG+ATOM_ERR+76,0,0,0,0);		
			return -1;
		}	
	}
	taskDelay(120);
	
	for(nModuIndx=0;nModuIndx<g_ETFDBInfo.etf_tdig_sum;nModuIndx++)  
	{
		UINT nGeo = g_ETFDBInfo.etf_TDIG_header[nModuIndx]->geo;
		base_addr = (nGeo<<20);
		UINT trgLat =  g_ETFDBInfo.etf_TDIG_header[nModuIndx]->trigger_latency;
		UINT winSet = g_ETFDBInfo.etf_TDIG_header[nModuIndx]->windowSet;
		UINT trgSrc = g_ETFDBInfo.etf_TDIG_header[nModuIndx]->trigger_source;
		UINT errDSet = g_ETFDBInfo.etf_TDIG_header[nModuIndx]->errDataEnb;

		printf("TDIG configuration : module index = %d, slot number = %d\n",nModuIndx,nGeo);

/*
		if (-1==TDIG_resetCPLD(base_addr))
		{
			printf("TDIG_resetCPLD failed\n");
			Push2ErrStack(PPCFATAL+ETF_FLG+ATOM_ERR+76,0,0,0,0);		
			return -1;
		}
*/		
		/*modify window and latency of HPTDC*/
		if (-1==ModifyParas(base_addr,trgLat,winSet))
		{
			printf("TDIG ModifyParas failed\n");
			Push2ErrStack(PPCFATAL+ETF_FLG+ATOM_ERR+77,0,0,0,0);		/*etf error 77*/		
			return -1;
		}

		/*enable the modify*/
		if (-1==enableModify(base_addr))
		{
			printf("TDIG enableModify failed\n");
			Push2ErrStack(PPCFATAL+ETF_FLG+ATOM_ERR+78,0,0,0,0);		/*etf error 78*/			
			return -1;
		}
		
		/*choose trigger source */
		if (-1==triggerSourceSelect(base_addr,trgSrc))
		{
			printf("TDIG triggerSourceSelect failed\n");
			Push2ErrStack(PPCFATAL+ETF_FLG+ATOM_ERR+79,0,0,0,0);		/*etf error 79*/			
			return -1;
		}
		
		/*set CBLT register*/
		if (-1==CBLTParasSet(base_addr,0,errDSet))
		{
			printf("TDIG CBLTParasSet failed\n");
			Push2ErrStack(PPCFATAL+ETF_FLG+ATOM_ERR+80,0,0,0,0);		/*etf error 80*/				
			return -1;
		}
		
		/*HPTDC OPEN*/
		if (-1==HPTDCopen(base_addr))
		{
			printf("TDIG HPTDCopen failed\n");
			Push2ErrStack(PPCFATAL+ETF_FLG+ATOM_ERR+81,0,0,0,0);		/*etf error 81*/			
			return -1;
		}
		
		ETFClearRocFEE();

	}
	
	return 0;
}

void initial_convert_table_225()//west
{
	int moduleIndx;
	int detecIndx;
	for(moduleIndx=0; moduleIndx<MAXFEENUM; moduleIndx=moduleIndx
+2)
	{
		detecIndx = (moduleIndx*3) ;
		g_nConverTable[moduleIndx][0][0] = (0x1<<11) + (detecIndx<<5) + ((6)<<1) + 0;
		g_nConverTable[moduleIndx][0][1] = (0x1<<11) + (detecIndx<<5) + ((4)<<1) + 0;
		g_nConverTable[moduleIndx][0][2] = (0x1<<11) + (detecIndx<<5) + ((8)<<1) + 0;
		g_nConverTable[moduleIndx][0][3] = (0x1<<11) + (detecIndx<<5) + ((2)<<1) + 0;
		g_nConverTable[moduleIndx][0][4] = (0x1<<11) + (detecIndx<<5) + ((10)<<1) + 0;
		g_nConverTable[moduleIndx][0][5] = (0x1<<11) + (detecIndx<<5) + ((0)<<1) + 0;
		g_nConverTable[moduleIndx][0][6] = (0x1<<11) + (detecIndx<<5) + ((11)<<1) + 0;
		g_nConverTable[moduleIndx][0][7] = (0x1<<11) + (detecIndx<<5) + ((1)<<1) + 0;
		g_nConverTable[moduleIndx][1][0] = (0x1<<11) + (detecIndx<<5) + ((9)<<1 )+ 0;
		g_nConverTable[moduleIndx][1][1] = (0x1<<11) + (detecIndx<<5) + ((3)<<1) + 0;
		g_nConverTable[moduleIndx][1][2] = (0x1<<11) + (detecIndx<<5) + ((7)<<1) + 0;
		g_nConverTable[moduleIndx][1][3] = (0x1<<11) + (detecIndx<<5) + ((5)<<1) + 0;
		g_nConverTable[moduleIndx][1][4] = (0x1<<11) + (detecIndx<<5) + ((6)<<1) + 1;
		g_nConverTable[moduleIndx][1][5] = (0x1<<11) + (detecIndx<<5) + ((4)<<1) + 1;
		g_nConverTable[moduleIndx][1][6] = (0x1<<11) + (detecIndx<<5) + ((8)<<1) + 1;
		g_nConverTable[moduleIndx][1][7] = (0x1<<11) + (detecIndx<<5) + ((2)<<1) + 1;
		g_nConverTable[moduleIndx][2][0] = (0x1<<11) + (detecIndx<<5) + ((10)<<1) + 1;
		g_nConverTable[moduleIndx][2][1] = (0x1<<11) + (detecIndx<<5) + ((0)<<1) + 1;
		g_nConverTable[moduleIndx][2][2] = (0x1<<11) + (detecIndx<<5) + ((11)<<1) + 1;
		g_nConverTable[moduleIndx][2][3] = (0x1<<11) + (detecIndx<<5) + ((1)<<1) + 1;
		g_nConverTable[moduleIndx][2][4] = (0x1<<11) + (detecIndx<<5) + ((9)<<1) + 1;
		g_nConverTable[moduleIndx][2][5] = (0x1<<11) + (detecIndx<<5) + ((3)<<1) + 1;
		g_nConverTable[moduleIndx][2][6] = (0x1<<11) + (detecIndx<<5) + ((7)<<1) + 1;
		g_nConverTable[moduleIndx][2][7] = (0x1<<11) + (detecIndx<<5) + ((5)<<1) + 1;
		g_nConverTable[moduleIndx][3][0] = (0x1<<11) + ((detecIndx+1)<<5) + ((6)<<1) + 1;
		g_nConverTable[moduleIndx][3][1] = (0x1<<11) + ((detecIndx+1)<<5) + ((4)<<1) + 1;
		g_nConverTable[moduleIndx][3][2] = (0x1<<11) + ((detecIndx+1)<<5) + ((8)<<1) + 1;
		g_nConverTable[moduleIndx][3][3] = (0x1<<11) + ((detecIndx+1)<<5) + ((2)<<1) + 1;
		g_nConverTable[moduleIndx][3][4] = (0x1<<11) + ((detecIndx+1)<<5) + ((10)<<1) + 1;
		g_nConverTable[moduleIndx][3][5] = (0x1<<11) + ((detecIndx+1)<<5) + ((0)<<1) + 1;
		g_nConverTable[moduleIndx][3][6] = (0x1<<11) + ((detecIndx+1)<<5) + ((11)<<1) + 1;
		g_nConverTable[moduleIndx][3][7] = (0x1<<11) + ((detecIndx+1)<<5) + ((1)<<1) + 1;
		g_nConverTable[moduleIndx][4][0] = (0x1<<11) + ((detecIndx+1)<<5) + ((9)<<1) + 1;
		g_nConverTable[moduleIndx][4][1] = (0x1<<11) + ((detecIndx+1)<<5) + ((3)<<1) + 1;
		g_nConverTable[moduleIndx][4][2] = (0x1<<11) + ((detecIndx+1)<<5) + ((7)<<1) + 1;
		g_nConverTable[moduleIndx][4][3] = (0x1<<11) + ((detecIndx+1)<<5) + ((5)<<1) + 1;
		g_nConverTable[moduleIndx][4][4] = (0x1<<11) + ((detecIndx+1)<<5) + ((6)<<1) + 0;
		g_nConverTable[moduleIndx][4][5] = (0x1<<11) + ((detecIndx+1)<<5) + ((4)<<1) + 0;
		g_nConverTable[moduleIndx][4][6] = (0x1<<11) + ((detecIndx+1)<<5) + ((8)<<1) + 0;
		g_nConverTable[moduleIndx][4][7] = (0x1<<11) + ((detecIndx+1)<<5) + ((2)<<1) + 0;
		g_nConverTable[moduleIndx][5][0] = (0x1<<11) + ((detecIndx+1)<<5) + ((10)<<1) + 0;
		g_nConverTable[moduleIndx][5][1] = (0x1<<11) + ((detecIndx+1)<<5) + ((0)<<1) + 0;
		g_nConverTable[moduleIndx][5][2] = (0x1<<11) + ((detecIndx+1)<<5) + ((11)<<1) + 0;
		g_nConverTable[moduleIndx][5][3] = (0x1<<11) + ((detecIndx+1)<<5) + ((1)<<1) + 0;
		g_nConverTable[moduleIndx][5][4] = (0x1<<11) + ((detecIndx+1)<<5) + ((9)<<1) + 0;
		g_nConverTable[moduleIndx][5][5] = (0x1<<11) + ((detecIndx+1)<<5) + ((3)<<1) + 0;
		g_nConverTable[moduleIndx][5][6] = (0x1<<11) + ((detecIndx+1)<<5) + ((7)<<1) + 0;
		g_nConverTable[moduleIndx][5][7] = (0x1<<11) + ((detecIndx+1)<<5) + ((5)<<1) + 0;

		//
		g_nConverTable[moduleIndx][6][0] = (0x1<<11) + ((detecIndx+2)<<5) + ((6)<<1) + 0;
		g_nConverTable[moduleIndx][6][1] = (0x1<<11) + ((detecIndx+2)<<5) + ((4)<<1) + 0;
		g_nConverTable[moduleIndx][6][2] = (0x1<<11) + ((detecIndx+2)<<5) + ((8)<<1) + 0;
		g_nConverTable[moduleIndx][6][3] = (0x1<<11) + ((detecIndx+2)<<5) + ((2)<<1) + 0;
		g_nConverTable[moduleIndx][6][4] = (0x1<<11) + ((detecIndx+2)<<5) + ((10)<<1) + 0;
		g_nConverTable[moduleIndx][6][5] = (0x1<<11) + ((detecIndx+2)<<5) + ((0)<<1) + 0;
		g_nConverTable[moduleIndx][6][6] = (0x1<<11) + ((detecIndx+2)<<5) + ((11)<<1) + 0;
		g_nConverTable[moduleIndx][6][7] = (0x1<<11) + ((detecIndx+2)<<5) + ((1)<<1) + 0;
		g_nConverTable[moduleIndx][7][0] = (0x1<<11) + ((detecIndx+2)<<5) + ((9)<<1 )+ 0;
		g_nConverTable[moduleIndx][7][1] = (0x1<<11) + ((detecIndx+2)<<5) + ((3)<<1) + 0;
		g_nConverTable[moduleIndx][7][2] = (0x1<<11) + ((detecIndx+2)<<5) + ((7)<<1) + 0;
		g_nConverTable[moduleIndx][7][3] = (0x1<<11) + ((detecIndx+2)<<5) + ((5)<<1) + 0;
		g_nConverTable[moduleIndx][7][4] = (0x1<<11) + ((detecIndx+2)<<5) + ((6)<<1) + 1;
		g_nConverTable[moduleIndx][7][5] = (0x1<<11) + ((detecIndx+2)<<5) + ((4)<<1) + 1;
		g_nConverTable[moduleIndx][7][6] = (0x1<<11) + ((detecIndx+2)<<5) + ((8)<<1) + 1;
		g_nConverTable[moduleIndx][7][7] = (0x1<<11) + ((detecIndx+2)<<5) + ((2)<<1) + 1;
		g_nConverTable[moduleIndx][8][0] = (0x1<<11) + ((detecIndx+2)<<5) + ((10)<<1) + 1;
		g_nConverTable[moduleIndx][8][1] = (0x1<<11) + ((detecIndx+2)<<5) + ((0)<<1) + 1;
		g_nConverTable[moduleIndx][8][2] = (0x1<<11) + ((detecIndx+2)<<5) + ((11)<<1) + 1;
		g_nConverTable[moduleIndx][8][3] = (0x1<<11) + ((detecIndx+2)<<5) + ((1)<<1) + 1;
		g_nConverTable[moduleIndx][8][4] = (0x1<<11) + ((detecIndx+2)<<5) + ((9)<<1) + 1;
		g_nConverTable[moduleIndx][8][5] = (0x1<<11) + ((detecIndx+2)<<5) + ((3)<<1) + 1;
		g_nConverTable[moduleIndx][8][6] = (0x1<<11) + ((detecIndx+2)<<5) + ((7)<<1) + 1;
		g_nConverTable[moduleIndx][8][7] = (0x1<<11) + ((detecIndx+2)<<5) + ((5)<<1) + 1;
	}


	for(moduleIndx=1; moduleIndx<MAXFEENUM; moduleIndx=moduleIndx
+2)
	{
		detecIndx = (moduleIndx*3) ;
		g_nConverTable[moduleIndx][0][0] = (0x1<<11) + (detecIndx<<5) + ((6)<<1) + 1;
		g_nConverTable[moduleIndx][0][1] = (0x1<<11) + (detecIndx<<5) + ((4)<<1) + 1;
		g_nConverTable[moduleIndx][0][2] = (0x1<<11) + (detecIndx<<5) + ((8)<<1) + 1;
		g_nConverTable[moduleIndx][0][3] = (0x1<<11) + (detecIndx<<5) + ((2)<<1) + 1;
		g_nConverTable[moduleIndx][0][4] = (0x1<<11) + (detecIndx<<5) + ((10)<<1) + 1;
		g_nConverTable[moduleIndx][0][5] = (0x1<<11) + (detecIndx<<5) + ((0)<<1) + 1;
		g_nConverTable[moduleIndx][0][6] = (0x1<<11) + (detecIndx<<5) + ((11)<<1) + 1;
		g_nConverTable[moduleIndx][0][7] = (0x1<<11) + (detecIndx<<5) + ((1)<<1) + 1;
		g_nConverTable[moduleIndx][1][0] = (0x1<<11) + (detecIndx<<5) + ((9)<<1 )+ 1;
		g_nConverTable[moduleIndx][1][1] = (0x1<<11) + (detecIndx<<5) + ((3)<<1) + 1;
		g_nConverTable[moduleIndx][1][2] = (0x1<<11) + (detecIndx<<5) + ((7)<<1) + 1;
		g_nConverTable[moduleIndx][1][3] = (0x1<<11) + (detecIndx<<5) + ((5)<<1) + 1;
		g_nConverTable[moduleIndx][1][4] = (0x1<<11) + (detecIndx<<5) + ((6)<<1) + 0;
		g_nConverTable[moduleIndx][1][5] = (0x1<<11) + (detecIndx<<5) + ((4)<<1) + 0;
		g_nConverTable[moduleIndx][1][6] = (0x1<<11) + (detecIndx<<5) + ((8)<<1) + 0;
		g_nConverTable[moduleIndx][1][7] = (0x1<<11) + (detecIndx<<5) + ((2)<<1) + 0;
		g_nConverTable[moduleIndx][2][0] = (0x1<<11) + (detecIndx<<5) + ((10)<<1) + 0;
		g_nConverTable[moduleIndx][2][1] = (0x1<<11) + (detecIndx<<5) + ((0)<<1) + 0;
		g_nConverTable[moduleIndx][2][2] = (0x1<<11) + (detecIndx<<5) + ((11)<<1) + 0;
		g_nConverTable[moduleIndx][2][3] = (0x1<<11) + (detecIndx<<5) + ((1)<<1) + 0;
		g_nConverTable[moduleIndx][2][4] = (0x1<<11) + (detecIndx<<5) + ((9)<<1) + 0;
		g_nConverTable[moduleIndx][2][5] = (0x1<<11) + (detecIndx<<5) + ((3)<<1) + 0;
		g_nConverTable[moduleIndx][2][6] = (0x1<<11) + (detecIndx<<5) + ((7)<<1) + 0;
		g_nConverTable[moduleIndx][2][7] = (0x1<<11) + (detecIndx<<5) + ((5)<<1) + 0;
		g_nConverTable[moduleIndx][3][0] = (0x1<<11) + ((detecIndx+1)<<5) + ((6)<<1) + 0;
		g_nConverTable[moduleIndx][3][1] = (0x1<<11) + ((detecIndx+1)<<5) + ((4)<<1) + 0;
		g_nConverTable[moduleIndx][3][2] = (0x1<<11) + ((detecIndx+1)<<5) + ((8)<<1) + 0;
		g_nConverTable[moduleIndx][3][3] = (0x1<<11) + ((detecIndx+1)<<5) + ((2)<<1) + 0;
		g_nConverTable[moduleIndx][3][4] = (0x1<<11) + ((detecIndx+1)<<5) + ((10)<<1) + 0;
		g_nConverTable[moduleIndx][3][5] = (0x1<<11) + ((detecIndx+1)<<5) + ((0)<<1) + 0;
		g_nConverTable[moduleIndx][3][6] = (0x1<<11) + ((detecIndx+1)<<5) + ((11)<<1) + 0;
		g_nConverTable[moduleIndx][3][7] = (0x1<<11) + ((detecIndx+1)<<5) + ((1)<<1) + 0;
		g_nConverTable[moduleIndx][4][0] = (0x1<<11) + ((detecIndx+1)<<5) + ((9)<<1) + 0;
		g_nConverTable[moduleIndx][4][1] = (0x1<<11) + ((detecIndx+1)<<5) + ((3)<<1) + 0;
		g_nConverTable[moduleIndx][4][2] = (0x1<<11) + ((detecIndx+1)<<5) + ((7)<<1) + 0;
		g_nConverTable[moduleIndx][4][3] = (0x1<<11) + ((detecIndx+1)<<5) + ((5)<<1) + 0;
		g_nConverTable[moduleIndx][4][4] = (0x1<<11) + ((detecIndx+1)<<5) + ((6)<<1) + 1;
		g_nConverTable[moduleIndx][4][5] = (0x1<<11) + ((detecIndx+1)<<5) + ((4)<<1) + 1;
		g_nConverTable[moduleIndx][4][6] = (0x1<<11) + ((detecIndx+1)<<5) + ((8)<<1) + 1;
		g_nConverTable[moduleIndx][4][7] = (0x1<<11) + ((detecIndx+1)<<5) + ((2)<<1) + 1;
		g_nConverTable[moduleIndx][5][0] = (0x1<<11) + ((detecIndx+1)<<5) + ((10)<<1) + 1;
		g_nConverTable[moduleIndx][5][1] = (0x1<<11) + ((detecIndx+1)<<5) + ((0)<<1) + 1;
		g_nConverTable[moduleIndx][5][2] = (0x1<<11) + ((detecIndx+1)<<5) + ((11)<<1) + 1;
		g_nConverTable[moduleIndx][5][3] = (0x1<<11) + ((detecIndx+1)<<5) + ((1)<<1) + 1;
		g_nConverTable[moduleIndx][5][4] = (0x1<<11) + ((detecIndx+1)<<5) + ((9)<<1) + 1;
		g_nConverTable[moduleIndx][5][5] = (0x1<<11) + ((detecIndx+1)<<5) + ((3)<<1) + 1;
		g_nConverTable[moduleIndx][5][6] = (0x1<<11) + ((detecIndx+1)<<5) + ((7)<<1) + 1;
		g_nConverTable[moduleIndx][5][7] = (0x1<<11) + ((detecIndx+1)<<5) + ((5)<<1) + 1;

		//
		g_nConverTable[moduleIndx][6][0] = (0x1<<11) + ((detecIndx+2)<<5) + ((6)<<1) + 1;
		g_nConverTable[moduleIndx][6][1] = (0x1<<11) + ((detecIndx+2)<<5) + ((4)<<1) + 1;
		g_nConverTable[moduleIndx][6][2] = (0x1<<11) + ((detecIndx+2)<<5) + ((8)<<1) + 1;
		g_nConverTable[moduleIndx][6][3] = (0x1<<11) + ((detecIndx+2)<<5) + ((2)<<1) + 1;
		g_nConverTable[moduleIndx][6][4] = (0x1<<11) + ((detecIndx+2)<<5) + ((10)<<1) + 1;
		g_nConverTable[moduleIndx][6][5] = (0x1<<11) + ((detecIndx+2)<<5) + ((0)<<1) + 1;
		g_nConverTable[moduleIndx][6][6] = (0x1<<11) + ((detecIndx+2)<<5) + ((11)<<1) + 1;
		g_nConverTable[moduleIndx][6][7] = (0x1<<11) + ((detecIndx+2)<<5) + ((1)<<1) + 1;
		g_nConverTable[moduleIndx][7][0] = (0x1<<11) + ((detecIndx+2)<<5) + ((9)<<1 )+ 1;
		g_nConverTable[moduleIndx][7][1] = (0x1<<11) + ((detecIndx+2)<<5) + ((3)<<1) + 1;
		g_nConverTable[moduleIndx][7][2] = (0x1<<11) + ((detecIndx+2)<<5) + ((7)<<1) + 1;
		g_nConverTable[moduleIndx][7][3] = (0x1<<11) + ((detecIndx+2)<<5) + ((5)<<1) + 1;
		g_nConverTable[moduleIndx][7][4] = (0x1<<11) + ((detecIndx+2)<<5) + ((6)<<1) + 0;
		g_nConverTable[moduleIndx][7][5] = (0x1<<11) + ((detecIndx+2)<<5) + ((4)<<1) + 0;
		g_nConverTable[moduleIndx][7][6] = (0x1<<11) + ((detecIndx+2)<<5) + ((8)<<1) + 0;
		g_nConverTable[moduleIndx][7][7] = (0x1<<11) + ((detecIndx+2)<<5) + ((2)<<1) + 0;
		g_nConverTable[moduleIndx][8][0] = (0x1<<11) + ((detecIndx+2)<<5) + ((10)<<1) + 0;
		g_nConverTable[moduleIndx][8][1] = (0x1<<11) + ((detecIndx+2)<<5) + ((0)<<1) + 0;
		g_nConverTable[moduleIndx][8][2] = (0x1<<11) + ((detecIndx+2)<<5) + ((11)<<1) + 0;
		g_nConverTable[moduleIndx][8][3] = (0x1<<11) + ((detecIndx+2)<<5) + ((1)<<1) + 0;
		g_nConverTable[moduleIndx][8][4] = (0x1<<11) + ((detecIndx+2)<<5) + ((9)<<1) + 0;
		g_nConverTable[moduleIndx][8][5] = (0x1<<11) + ((detecIndx+2)<<5) + ((3)<<1) + 0;
		g_nConverTable[moduleIndx][8][6] = (0x1<<11) + ((detecIndx+2)<<5) + ((7)<<1) + 0;
		g_nConverTable[moduleIndx][8][7] = (0x1<<11) + ((detecIndx+2)<<5) + ((5)<<1) + 0;
	}	

}


void initial_convert_table_224()//east
{
	int moduleIndx;
	int detecIndx;
	for(moduleIndx=0; moduleIndx<MAXFEENUM; moduleIndx=moduleIndx
+2)
	{
		detecIndx = (moduleIndx*3) ;
		g_nConverTable[moduleIndx][0][0] = (0x0<<11) + (detecIndx<<5) + ((6)<<1) + 1;
		g_nConverTable[moduleIndx][0][1] = (0x0<<11) + (detecIndx<<5) + ((4)<<1) + 1;
		g_nConverTable[moduleIndx][0][2] = (0x0<<11) + (detecIndx<<5) + ((8)<<1) + 1;
		g_nConverTable[moduleIndx][0][3] = (0x0<<11) + (detecIndx<<5) + ((2)<<1) + 1;
		g_nConverTable[moduleIndx][0][4] = (0x0<<11) + (detecIndx<<5) + ((10)<<1) + 1;
		g_nConverTable[moduleIndx][0][5] = (0x0<<11) + (detecIndx<<5) + ((0)<<1) + 1;
		g_nConverTable[moduleIndx][0][6] = (0x0<<11) + (detecIndx<<5) + ((11)<<1) + 1;
		g_nConverTable[moduleIndx][0][7] = (0x0<<11) + (detecIndx<<5) + ((1)<<1) + 1;
		g_nConverTable[moduleIndx][1][0] = (0x0<<11) + (detecIndx<<5) + ((9)<<1 )+ 1;
		g_nConverTable[moduleIndx][1][1] = (0x0<<11) + (detecIndx<<5) + ((3)<<1) + 1;
		g_nConverTable[moduleIndx][1][2] = (0x0<<11) + (detecIndx<<5) + ((7)<<1) + 1;
		g_nConverTable[moduleIndx][1][3] = (0x0<<11) + (detecIndx<<5) + ((5)<<1) + 1;
		g_nConverTable[moduleIndx][1][4] = (0x0<<11) + (detecIndx<<5) + ((6)<<1) + 0;
		g_nConverTable[moduleIndx][1][5] = (0x0<<11) + (detecIndx<<5) + ((4)<<1) + 0;
		g_nConverTable[moduleIndx][1][6] = (0x0<<11) + (detecIndx<<5) + ((8)<<1) + 0;
		g_nConverTable[moduleIndx][1][7] = (0x0<<11) + (detecIndx<<5) + ((2)<<1) + 0;
		g_nConverTable[moduleIndx][2][0] = (0x0<<11) + (detecIndx<<5) + ((10)<<1) + 0;
		g_nConverTable[moduleIndx][2][1] = (0x0<<11) + (detecIndx<<5) + ((0)<<1) + 0;
		g_nConverTable[moduleIndx][2][2] = (0x0<<11) + (detecIndx<<5) + ((11)<<1) + 0;
		g_nConverTable[moduleIndx][2][3] = (0x0<<11) + (detecIndx<<5) + ((1)<<1) + 0;
		g_nConverTable[moduleIndx][2][4] = (0x0<<11) + (detecIndx<<5) + ((9)<<1) + 0;
		g_nConverTable[moduleIndx][2][5] = (0x0<<11) + (detecIndx<<5) + ((3)<<1) + 0;
		g_nConverTable[moduleIndx][2][6] = (0x0<<11) + (detecIndx<<5) + ((7)<<1) + 0;
		g_nConverTable[moduleIndx][2][7] = (0x0<<11) + (detecIndx<<5) + ((5)<<1) + 0;
		g_nConverTable[moduleIndx][3][0] = (0x0<<11) + ((detecIndx+1)<<5) + ((6)<<1) + 0;
		g_nConverTable[moduleIndx][3][1] = (0x0<<11) + ((detecIndx+1)<<5) + ((4)<<1) + 0;
		g_nConverTable[moduleIndx][3][2] = (0x0<<11) + ((detecIndx+1)<<5) + ((8)<<1) + 0;
		g_nConverTable[moduleIndx][3][3] = (0x0<<11) + ((detecIndx+1)<<5) + ((2)<<1) + 0;
		g_nConverTable[moduleIndx][3][4] = (0x0<<11) + ((detecIndx+1)<<5) + ((10)<<1) + 0;
		g_nConverTable[moduleIndx][3][5] = (0x0<<11) + ((detecIndx+1)<<5) + ((0)<<1) + 0;
		g_nConverTable[moduleIndx][3][6] = (0x0<<11) + ((detecIndx+1)<<5) + ((11)<<1) + 0;
		g_nConverTable[moduleIndx][3][7] = (0x0<<11) + ((detecIndx+1)<<5) + ((1)<<1) + 0;
		g_nConverTable[moduleIndx][4][0] = (0x0<<11) + ((detecIndx+1)<<5) + ((9)<<1) + 0;
		g_nConverTable[moduleIndx][4][1] = (0x0<<11) + ((detecIndx+1)<<5) + ((3)<<1) + 0;
		g_nConverTable[moduleIndx][4][2] = (0x0<<11) + ((detecIndx+1)<<5) + ((7)<<1) + 0;
		g_nConverTable[moduleIndx][4][3] = (0x0<<11) + ((detecIndx+1)<<5) + ((5)<<1) + 0;
		g_nConverTable[moduleIndx][4][4] = (0x0<<11) + ((detecIndx+1)<<5) + ((6)<<1) + 1;
		g_nConverTable[moduleIndx][4][5] = (0x0<<11) + ((detecIndx+1)<<5) + ((4)<<1) + 1;
		g_nConverTable[moduleIndx][4][6] = (0x0<<11) + ((detecIndx+1)<<5) + ((8)<<1) + 1;
		g_nConverTable[moduleIndx][4][7] = (0x0<<11) + ((detecIndx+1)<<5) + ((2)<<1) + 1;
		g_nConverTable[moduleIndx][5][0] = (0x0<<11) + ((detecIndx+1)<<5) + ((10)<<1) + 1;
		g_nConverTable[moduleIndx][5][1] = (0x0<<11) + ((detecIndx+1)<<5) + ((0)<<1) + 1;
		g_nConverTable[moduleIndx][5][2] = (0x0<<11) + ((detecIndx+1)<<5) + ((11)<<1) + 1;
		g_nConverTable[moduleIndx][5][3] = (0x0<<11) + ((detecIndx+1)<<5) + ((1)<<1) + 1;
		g_nConverTable[moduleIndx][5][4] = (0x0<<11) + ((detecIndx+1)<<5) + ((9)<<1) + 1;
		g_nConverTable[moduleIndx][5][5] = (0x0<<11) + ((detecIndx+1)<<5) + ((3)<<1) + 1;
		g_nConverTable[moduleIndx][5][6] = (0x0<<11) + ((detecIndx+1)<<5) + ((7)<<1) + 1;
		g_nConverTable[moduleIndx][5][7] = (0x0<<11) + ((detecIndx+1)<<5) + ((5)<<1) + 1;

		//
		g_nConverTable[moduleIndx][6][0] = (0x0<<11) + ((detecIndx+2)<<5) + ((6)<<1) + 1;
		g_nConverTable[moduleIndx][6][1] = (0x0<<11) + ((detecIndx+2)<<5) + ((4)<<1) + 1;
		g_nConverTable[moduleIndx][6][2] = (0x0<<11) + ((detecIndx+2)<<5) + ((8)<<1) + 1;
		g_nConverTable[moduleIndx][6][3] = (0x0<<11) + ((detecIndx+2)<<5) + ((2)<<1) + 1;
		g_nConverTable[moduleIndx][6][4] = (0x0<<11) + ((detecIndx+2)<<5) + ((10)<<1) + 1;
		g_nConverTable[moduleIndx][6][5] = (0x0<<11) + ((detecIndx+2)<<5) + ((0)<<1) + 1;
		g_nConverTable[moduleIndx][6][6] = (0x0<<11) + ((detecIndx+2)<<5) + ((11)<<1) + 1;
		g_nConverTable[moduleIndx][6][7] = (0x0<<11) + ((detecIndx+2)<<5) + ((1)<<1) + 1;
		g_nConverTable[moduleIndx][7][0] = (0x0<<11) + ((detecIndx+2)<<5) + ((9)<<1 )+ 1;
		g_nConverTable[moduleIndx][7][1] = (0x0<<11) + ((detecIndx+2)<<5) + ((3)<<1) + 1;
		g_nConverTable[moduleIndx][7][2] = (0x0<<11) + ((detecIndx+2)<<5) + ((7)<<1) + 1;
		g_nConverTable[moduleIndx][7][3] = (0x0<<11) + ((detecIndx+2)<<5) + ((5)<<1) + 1;
		g_nConverTable[moduleIndx][7][4] = (0x0<<11) + ((detecIndx+2)<<5) + ((6)<<1) + 0;
		g_nConverTable[moduleIndx][7][5] = (0x0<<11) + ((detecIndx+2)<<5) + ((4)<<1) + 0;
		g_nConverTable[moduleIndx][7][6] = (0x0<<11) + ((detecIndx+2)<<5) + ((8)<<1) + 0;
		g_nConverTable[moduleIndx][7][7] = (0x0<<11) + ((detecIndx+2)<<5) + ((2)<<1) + 0;
		g_nConverTable[moduleIndx][8][0] = (0x0<<11) + ((detecIndx+2)<<5) + ((10)<<1) + 0;
		g_nConverTable[moduleIndx][8][1] = (0x0<<11) + ((detecIndx+2)<<5) + ((0)<<1) + 0;
		g_nConverTable[moduleIndx][8][2] = (0x0<<11) + ((detecIndx+2)<<5) + ((11)<<1) + 0;
		g_nConverTable[moduleIndx][8][3] = (0x0<<11) + ((detecIndx+2)<<5) + ((1)<<1) + 0;
		g_nConverTable[moduleIndx][8][4] = (0x0<<11) + ((detecIndx+2)<<5) + ((9)<<1) + 0;
		g_nConverTable[moduleIndx][8][5] = (0x0<<11) + ((detecIndx+2)<<5) + ((3)<<1) + 0;
		g_nConverTable[moduleIndx][8][6] = (0x0<<11) + ((detecIndx+2)<<5) + ((7)<<1) + 0;
		g_nConverTable[moduleIndx][8][7] = (0x0<<11) + ((detecIndx+2)<<5) + ((5)<<1) + 0;
	}


	for(moduleIndx=1; moduleIndx<MAXFEENUM; moduleIndx=moduleIndx
+2)
	{
		detecIndx = (moduleIndx*3) ;
		g_nConverTable[moduleIndx][0][0] = (0x0<<11) + (detecIndx<<5) + ((6)<<1) + 0;
		g_nConverTable[moduleIndx][0][1] = (0x0<<11) + (detecIndx<<5) + ((4)<<1) + 0;
		g_nConverTable[moduleIndx][0][2] = (0x0<<11) + (detecIndx<<5) + ((8)<<1) + 0;
		g_nConverTable[moduleIndx][0][3] = (0x0<<11) + (detecIndx<<5) + ((2)<<1) + 0;
		g_nConverTable[moduleIndx][0][4] = (0x0<<11) + (detecIndx<<5) + ((10)<<1) + 0;
		g_nConverTable[moduleIndx][0][5] = (0x0<<11) + (detecIndx<<5) + ((0)<<1) + 0;
		g_nConverTable[moduleIndx][0][6] = (0x0<<11) + (detecIndx<<5) + ((11)<<1) + 0;
		g_nConverTable[moduleIndx][0][7] = (0x0<<11) + (detecIndx<<5) + ((1)<<1) + 0;
		g_nConverTable[moduleIndx][1][0] = (0x0<<11) + (detecIndx<<5) + ((9)<<1 )+ 0;
		g_nConverTable[moduleIndx][1][1] = (0x0<<11) + (detecIndx<<5) + ((3)<<1) + 0;
		g_nConverTable[moduleIndx][1][2] = (0x0<<11) + (detecIndx<<5) + ((7)<<1) + 0;
		g_nConverTable[moduleIndx][1][3] = (0x0<<11) + (detecIndx<<5) + ((5)<<1) + 0;
		g_nConverTable[moduleIndx][1][4] = (0x0<<11) + (detecIndx<<5) + ((6)<<1) + 1;
		g_nConverTable[moduleIndx][1][5] = (0x0<<11) + (detecIndx<<5) + ((4)<<1) + 1;
		g_nConverTable[moduleIndx][1][6] = (0x0<<11) + (detecIndx<<5) + ((8)<<1) + 1;
		g_nConverTable[moduleIndx][1][7] = (0x0<<11) + (detecIndx<<5) + ((2)<<1) + 1;
		g_nConverTable[moduleIndx][2][0] = (0x0<<11) + (detecIndx<<5) + ((10)<<1) + 1;
		g_nConverTable[moduleIndx][2][1] = (0x0<<11) + (detecIndx<<5) + ((0)<<1) + 1;
		g_nConverTable[moduleIndx][2][2] = (0x0<<11) + (detecIndx<<5) + ((11)<<1) + 1;
		g_nConverTable[moduleIndx][2][3] = (0x0<<11) + (detecIndx<<5) + ((1)<<1) + 1;
		g_nConverTable[moduleIndx][2][4] = (0x0<<11) + (detecIndx<<5) + ((9)<<1) + 1;
		g_nConverTable[moduleIndx][2][5] = (0x0<<11) + (detecIndx<<5) + ((3)<<1) + 1;
		g_nConverTable[moduleIndx][2][6] = (0x0<<11) + (detecIndx<<5) + ((7)<<1) + 1;
		g_nConverTable[moduleIndx][2][7] = (0x0<<11) + (detecIndx<<5) + ((5)<<1) + 1;
		g_nConverTable[moduleIndx][3][0] = (0x0<<11) + ((detecIndx+1)<<5) + ((6)<<1) + 1;
		g_nConverTable[moduleIndx][3][1] = (0x0<<11) + ((detecIndx+1)<<5) + ((4)<<1) + 1;
		g_nConverTable[moduleIndx][3][2] = (0x0<<11) + ((detecIndx+1)<<5) + ((8)<<1) + 1;
		g_nConverTable[moduleIndx][3][3] = (0x0<<11) + ((detecIndx+1)<<5) + ((2)<<1) + 1;
		g_nConverTable[moduleIndx][3][4] = (0x0<<11) + ((detecIndx+1)<<5) + ((10)<<1) + 1;
		g_nConverTable[moduleIndx][3][5] = (0x0<<11) + ((detecIndx+1)<<5) + ((0)<<1) + 1;
		g_nConverTable[moduleIndx][3][6] = (0x0<<11) + ((detecIndx+1)<<5) + ((11)<<1) + 1;
		g_nConverTable[moduleIndx][3][7] = (0x0<<11) + ((detecIndx+1)<<5) + ((1)<<1) + 1;
		g_nConverTable[moduleIndx][4][0] = (0x0<<11) + ((detecIndx+1)<<5) + ((9)<<1) + 1;
		g_nConverTable[moduleIndx][4][1] = (0x0<<11) + ((detecIndx+1)<<5) + ((3)<<1) + 1;
		g_nConverTable[moduleIndx][4][2] = (0x0<<11) + ((detecIndx+1)<<5) + ((7)<<1) + 1;
		g_nConverTable[moduleIndx][4][3] = (0x0<<11) + ((detecIndx+1)<<5) + ((5)<<1) + 1;
		g_nConverTable[moduleIndx][4][4] = (0x0<<11) + ((detecIndx+1)<<5) + ((6)<<1) + 0;
		g_nConverTable[moduleIndx][4][5] = (0x0<<11) + ((detecIndx+1)<<5) + ((4)<<1) + 0;
		g_nConverTable[moduleIndx][4][6] = (0x0<<11) + ((detecIndx+1)<<5) + ((8)<<1) + 0;
		g_nConverTable[moduleIndx][4][7] = (0x0<<11) + ((detecIndx+1)<<5) + ((2)<<1) + 0;
		g_nConverTable[moduleIndx][5][0] = (0x0<<11) + ((detecIndx+1)<<5) + ((10)<<1) + 0;
		g_nConverTable[moduleIndx][5][1] = (0x0<<11) + ((detecIndx+1)<<5) + ((0)<<1) + 0;
		g_nConverTable[moduleIndx][5][2] = (0x0<<11) + ((detecIndx+1)<<5) + ((11)<<1) + 0;
		g_nConverTable[moduleIndx][5][3] = (0x0<<11) + ((detecIndx+1)<<5) + ((1)<<1) + 0;
		g_nConverTable[moduleIndx][5][4] = (0x0<<11) + ((detecIndx+1)<<5) + ((9)<<1) + 0;
		g_nConverTable[moduleIndx][5][5] = (0x0<<11) + ((detecIndx+1)<<5) + ((3)<<1) + 0;
		g_nConverTable[moduleIndx][5][6] = (0x0<<11) + ((detecIndx+1)<<5) + ((7)<<1) + 0;
		g_nConverTable[moduleIndx][5][7] = (0x0<<11) + ((detecIndx+1)<<5) + ((5)<<1) + 0;

		//
		g_nConverTable[moduleIndx][6][0] = (0x0<<11) + ((detecIndx+2)<<5) + ((6)<<1) + 0;
		g_nConverTable[moduleIndx][6][1] = (0x0<<11) + ((detecIndx+2)<<5) + ((4)<<1) + 0;
		g_nConverTable[moduleIndx][6][2] = (0x0<<11) + ((detecIndx+2)<<5) + ((8)<<1) + 0;
		g_nConverTable[moduleIndx][6][3] = (0x0<<11) + ((detecIndx+2)<<5) + ((2)<<1) + 0;
		g_nConverTable[moduleIndx][6][4] = (0x0<<11) + ((detecIndx+2)<<5) + ((10)<<1) + 0;
		g_nConverTable[moduleIndx][6][5] = (0x0<<11) + ((detecIndx+2)<<5) + ((0)<<1) + 0;
		g_nConverTable[moduleIndx][6][6] = (0x0<<11) + ((detecIndx+2)<<5) + ((11)<<1) + 0;
		g_nConverTable[moduleIndx][6][7] = (0x0<<11) + ((detecIndx+2)<<5) + ((1)<<1) + 0;
		g_nConverTable[moduleIndx][7][0] = (0x0<<11) + ((detecIndx+2)<<5) + ((9)<<1 )+ 0;
		g_nConverTable[moduleIndx][7][1] = (0x0<<11) + ((detecIndx+2)<<5) + ((3)<<1) + 0;
		g_nConverTable[moduleIndx][7][2] = (0x0<<11) + ((detecIndx+2)<<5) + ((7)<<1) + 0;
		g_nConverTable[moduleIndx][7][3] = (0x0<<11) + ((detecIndx+2)<<5) + ((5)<<1) + 0;
		g_nConverTable[moduleIndx][7][4] = (0x0<<11) + ((detecIndx+2)<<5) + ((6)<<1) + 1;
		g_nConverTable[moduleIndx][7][5] = (0x0<<11) + ((detecIndx+2)<<5) + ((4)<<1) + 1;
		g_nConverTable[moduleIndx][7][6] = (0x0<<11) + ((detecIndx+2)<<5) + ((8)<<1) + 1;
		g_nConverTable[moduleIndx][7][7] = (0x0<<11) + ((detecIndx+2)<<5) + ((2)<<1) + 1;
		g_nConverTable[moduleIndx][8][0] = (0x0<<11) + ((detecIndx+2)<<5) + ((10)<<1) + 1;
		g_nConverTable[moduleIndx][8][1] = (0x0<<11) + ((detecIndx+2)<<5) + ((0)<<1) + 1;
		g_nConverTable[moduleIndx][8][2] = (0x0<<11) + ((detecIndx+2)<<5) + ((11)<<1) + 1;
		g_nConverTable[moduleIndx][8][3] = (0x0<<11) + ((detecIndx+2)<<5) + ((1)<<1) + 1;
		g_nConverTable[moduleIndx][8][4] = (0x0<<11) + ((detecIndx+2)<<5) + ((9)<<1) + 1;
		g_nConverTable[moduleIndx][8][5] = (0x0<<11) + ((detecIndx+2)<<5) + ((3)<<1) + 1;
		g_nConverTable[moduleIndx][8][6] = (0x0<<11) + ((detecIndx+2)<<5) + ((7)<<1) + 1;
		g_nConverTable[moduleIndx][8][7] = (0x0<<11) + ((detecIndx+2)<<5) + ((5)<<1) + 1;
	}	

}

/*
int resetch()
{
	WriteToVme32(0xbc0270, VA32, 0x32);
	taskDelay(1);
	return 0;
}
*/
int ROC_halfway()
{
	TOFRocStop();
	WriteToVme32(0xbc0270, VA32, 0x18);	/*clear FEE FIFO*/
	taskDelay(1);
	WriteToVme32(0xbc0270, VA32, 0x25);	/*clear FEE FIFO*/
	taskDelay(1);
	WriteToVme32(0xbc0270, VA32, 0x26);	/*clear FEE FIFO*/
	taskDelay(1);
	WriteToVme32(0xbc0270, VA32, 0x24);	/*clear FEE FIFO*/
	taskDelay(1);

	MCSTwrite(0x3000000, 32, 0x1);
	return 0;
}

int ROC_linear()
{
	UINT32 sd = 0, td = 0;
	int cnt = 0;
	UINT32 limit = 121;
	UINT32 uRead;
	FILE *fp;

	char chfind;
	int chl = 0;
	int chladdr = 0;
	int read_ctrl = 0;
	char auto_man;
	int param_ok=0;

	fp = fopen("fctrl_lineraty.txt","w");
	if(!fp)
	{
			printf("Error in create file\n");
			return -1;
	}


	printf("this is for the fctrl lineraty test!!!\n");
	printf("Please choose auto or manual[a|m]:\n");
	while (!param_ok)
	{
		scanf("%c",&auto_man);
		if(auto_man=='a'||auto_man=='m')
			param_ok=1;
	}
	param_ok=0;
	while(!param_ok)
	{
		printf("Please choose which channel(0,1,2,3,4) you want to test:\n");
		scanf("%d",&chl);
		if(chl>=0&&chl<=4)
			param_ok=1;
	}
	if(auto_man== 'm'){
		if(chl==0)
		{
			chladdr=0x0000011E;
			printf("choose channel 0\n");
		}
		else if(chl==1)
		{
			chladdr=0x1000011D;
			printf("choose channel 1\n");
		}
		else if(chl==2)
		{
			chladdr=0x2000011B;
			printf("choose channel 2\n");
		}
		else if(chl==3)
		{
			chladdr=0x30000117;
			printf("choose channel 3\n");
		}
		else if(chl==4)
		{
			chladdr=0x4000010F;
			printf("choose channel 4\n");
		}
		else 
		{
			printf("error!\n");
  				chl=5; 
		}
		limit = 256;
	}else{
		chladdr = ((chl & 0xF) << 28) | 0x100;
		limit = 256;
	}

	read_ctrl = (chl & 0xF) << 28;
	printf("chl=%d\n",chl);
	while(1) 
	{	
	
		WriteToVme32(0xbc0270, VA32, 0x0);


		sd = cnt & 0xFF;
		td = sd | (sd << 8) | (sd << 16) | (sd << 24);
 

		WriteToVme32(0xbc0280, VA32, chladdr);
		/* read it */
		/*ReadFrmVme32(0xbc0280,VA32,&uRead);*/
		/*printf("0xBC0280: %8x\n", uRead);*/

		taskDelay(1);
		WriteToVme32(0xbc0288, VA32, td);
		taskDelay(1);
		WriteToVme32(0xbc028C, VA32, sd);
		taskDelay(1);
		WriteToVme32(0xbc0270, VA32, 0x33);
		taskDelay(1);


		WriteToVme32(0xbc0280, VA32, read_ctrl);
		taskDelay(1);
		WriteToVme32(0xbc0270, VA32, 0x33);
		taskDelay(1);


		ReadFrmVme32(0xbc0220,VA32,&uRead);
		taskDelay(1);
		fprintf(fp, "%d\t%02x\t%02x\t%02x\t%02x\t", cnt, uRead&0xFF, (uRead >> 8) & 0xFF, (uRead >> 16)&0xFF, (uRead >> 24)&0xFF);
		ReadFrmVme32(0xbc0224,VA32,&uRead);
		taskDelay(1);
		fprintf(fp, "%2x\n", uRead & 0xFF);
		
	if(auto_man == 'm'){
		printf("please choose +,-,*,p(+10),n(-10)\n");
		scanf("%c",&chfind);
	 	printf("The input is %c\n",chfind);       
		if(chfind == '+')
		{
			++cnt;
   		     	if(cnt >= limit)
	 			cnt = 0;
                	printf("cnt=%d\n",cnt);
		}
		else if(chfind == '-')
		{
			--cnt;
			if(cnt < 0)
				cnt = 0;
			printf("cnt=%d\n",cnt);
		}
		else if(chfind == 'p')
		{
			cnt=cnt+10;
			if(cnt >= limit)
				cnt = 0;
			printf("cnt=%d\n",cnt);
		}
		else if(chfind == 'n')
		{
			cnt=cnt-10;
			if(cnt <0)
				cnt = 0;
			printf("cnt=%d\n",cnt);
		}
		
		else if(chfind =='*')
		break;
		
	}
	else
	{
				
		++cnt;
		if(cnt >= limit)
			break;
	}
	}

	printf("before file closed\n");
	fclose(fp);
	printf("file closed\n");
	return 0;
	
}




