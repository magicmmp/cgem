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

#include "sd_tof.h"
#include "sd_tof_hw.h"
#include "sd_tof_hptdc_config.h"

#include "sd_tof_fee.h"
#include "sd_tof_monitor.h"

#include "sd_etf.h"
#include "sd_etf_hw.h"


bool g_bTOFRocIntConed=FALSE;

void dly(int ms)
{
        int it;
        double a1,a2,a3;
        for (it=0;it<=23000*ms;it++)
        {
                a1=a1+(1e-8);
                a2=a2-(1e-8);
                a3=a3+a1*a2;
        }
}

void dlyus(int us)
{
        int i;
        double a1,a2,a3;
        for (i=0;i<=23*us;i++)
        {
                a1=a1+(1e-8);
                a2=a2-(1e-8);
                a3=a3+a1*a2;
        }
}

void TOFIntHandler(int nArg)
{
	    //   logMsg("interrupt receive 64 OK,%d\n", 0, 0, 0, 0, 0, 0);
	semGive(g_sidInterrupt);
	g_nInterruptCounter+=g_nTrgPerInt;
}


int   TOFConfigIntCBLT()
{
	if(FALSE == g_bTOFRocIntConed) 
	{
		if(intConnect (INUM_TO_IVEC(TOF_INT_VEC), (VOIDFUNCPTR)TOFIntHandler, 0) == ERROR)
	  	{
	  		printf("int connection error\n");
			Push2ErrStack(PPCFATAL+TOF_FLG+ATOM_ERR+51,0,0,0,0);		/*tof error 51*/
			return  -1;
	  	}
		g_bTOFRocIntConed=TRUE;
	}
	
	
	sysIntDisable(TOF_INT_LEVEL);	
	
	if(CBLTinit(64)==-1)
	{
		printf("cblt init error\n");
		Push2ErrStack(PPCFATAL+TOF_FLG+ATOM_ERR+52,0,0,0,0);		/*tof error 52*/
		return -1;
	}
	return 0;
}


int TOFConfigCbltChain()//please check and recheck
{
	UINT32 nadr;
	UINT nGeo;
	UINT nFirstGeo=0,nLastGeo=0;
	UINT nCbltChainSum=0;
	//distinguish geo in cblt chain
	//which one is the first geo
	nFirstGeo=g_TOFDBInfo.tof_roc_header->geo;
	if( g_TOFDBInfo.tof_FEE_header[0]->geo < nFirstGeo)
	{
		nFirstGeo=g_TOFDBInfo.tof_FEE_header[0]->geo;
	}
	nLastGeo=g_TOFDBInfo.tof_roc_header->geo;
	if(g_TOFDBInfo.tof_FEE_header[g_TOFDBInfo.tof_FEE_sum-1]->geo> nLastGeo)
	{
		nLastGeo=g_TOFDBInfo.tof_FEE_header[g_TOFDBInfo.tof_FEE_sum-1]->geo;
	}
	if(g_TOFDBInfo.tof_monitor_sum==1)
	{
		if(g_TOFDBInfo.tof_monitor_header->geo< nFirstGeo)
		{
			nFirstGeo=g_TOFDBInfo.tof_monitor_header->geo;
		}
		if(g_TOFDBInfo.tof_monitor_header->geo> nLastGeo)
		{
			nLastGeo=g_TOFDBInfo.tof_monitor_header->geo;
		}
	}
	printf("nFirstGeo=%d,nLastGeo=%d\n",nFirstGeo,nLastGeo);
	if( (nFirstGeo==0)||(nLastGeo==0) )
	{
		printf("get first & last geo error\n");
		Push2ErrStack(PPCFATAL+TOF_FLG+ATOM_ERR+53,0,0,0,0);		/*tof error 53*/
		return -1;
	}
	nCbltChainSum=g_TOFDBInfo.tof_FEE_sum +1;
	if(g_TOFDBInfo.tof_monitor_sum==1)
	{
		nCbltChainSum++;
	}
	printf("nCbltChainSum=%d\n",nCbltChainSum);

	taskDelay(30);
	//total module num in chain=g_TOFDBInfo.tof_FEE_sum +1 , last indx use for roc module
	for(int nModuIndx=0;nModuIndx< nCbltChainSum ;nModuIndx++) 
	{
		if(nModuIndx< g_TOFDBInfo.tof_FEE_sum)
		{
			nGeo=g_TOFDBInfo.tof_FEE_header[nModuIndx]->geo;
			nadr=(nGeo<<19)+0x70043;
		}
		else if(nModuIndx ==g_TOFDBInfo.tof_FEE_sum)
		{
			nGeo=g_TOFDBInfo.tof_roc_header->geo;
			nadr=(nGeo<<19)+0x7ffb;
		}
		else if(nModuIndx ==(g_TOFDBInfo.tof_FEE_sum+1) )
		{
			nGeo=g_TOFDBInfo.tof_monitor_header->geo;
			nadr=(nGeo<<19)+0x70043;
		}
		//printf("nadr=0x%x\n",nadr);
		if(nGeo==nFirstGeo)
		{
			if ( -1 == WriteToVme8(nadr, VCSR, 0xa6 ) ) 
			{
				printf("Write GEO %d chain First Failed \n",nGeo); 
				Push2ErrStack(PPCFATAL+TOF_FLG+ATOM_ERR+54,0,0,0,0);		/*tof error 54*/
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

			//if(!TDIG_CBLTEn)//no TDIG
			{
				if ( -1 == WriteToVme8(nadr, VCSR, 0x9a ) ) 
				{
					printf("Write GEO %d chain Last Failed \n",nGeo); 
					Push2ErrStack(PPCFATAL+TOF_FLG+ATOM_ERR+55,0,0,0,0);		
					return -1;
				}
				else
				{
					printf("last GEO Chain %d\n",nGeo);
				}
			}
			/*
			else//TDIG insert
			{
				if ( -1 == WriteToVme8(nadr, VCSR, 0x96 ) ) 
				{
					printf("Write GEO %d chain middle Failed\n",nGeo); 
					Push2ErrStack(PPCFATAL+TOF_FLG+ATOM_ERR+56,0,0,0,0);		
					return -1;				
				}
				else
				{
					printf("mid GEO Chain %d\n",nGeo);
				}
			}*/
		}
		else
		{
			if ( -1 == WriteToVme8(nadr, VCSR, 0x96 ) ) 
			{
				printf("Write GEO %d chain middle Failed\n",nGeo); 
				Push2ErrStack(PPCFATAL+TOF_FLG+ATOM_ERR+56,0,0,0,0);		/*tof error 56*/
				return -1;				
			}
			else
			{
				//printf("mid GEO Chain %d\n",nGeo);
			}
		}
	}
	return 0;
}


int TOFConfigFeeMonitor()
{
	TOFGlobalReset();
	//TOFGlobalResetNew();
	
	TOFConfigFeeBaseAddr();
	TOFConfigMoniBaseAddr();

	if(-1==TOFCheckFeeSerialNum())
	{
		DEBUG_OUTPUT(("TOFCheckFeeSerialNum Failed \n"));
		Push2ErrStack(PPCFATAL+TOF_FLG+ATOM_ERR+57,0,0,0,0);		/*tof error 57*/
		return -1;
	}
	if(-1==TOFCheckMoniSerialNum())
	{
		DEBUG_OUTPUT(("TOFCheckMoniSerialNum Failed \n"));
		Push2ErrStack(PPCFATAL+TOF_FLG+ATOM_ERR+58,0,0,0,0);		/*tof error 58*/
		return -1;
	}

	TOFConfigFeeHPTDC();
	TOFConfigMoniHPTDC();
	
	
	TOFConfigFeeDAC();//set dac register etc
	
	TOFConfigMonitorDAC();
	TOFMonitorLaserSetup();
	
	TOFConfigFeeInlTable();
	TOFConfigMoniInlTable();

	printf("++++++\n");


	if(-1==TOFConfigCbltChain() )
	{
		DEBUG_OUTPUT(("TOFConfigCbltChain Failed \n"));
		Push2ErrStack(PPCFATAL+TOF_FLG+ATOM_ERR+59,0,0,0,0);		
		return -1;
	}
	
	printf("=======\n");
	taskDelay(30);
	if(-1==TOFConfigFeeWorkMode())
	{
		DEBUG_OUTPUT(("TOFConfigFeeWorkMode Failed \n"));
		Push2ErrStack(PPCFATAL+TOF_FLG+ATOM_ERR+60,0,0,0,0);		/*tof error 60*/		
		return -1;
	}
	
	if(-1==TOFConfigMoniWorkMode())
	{
		DEBUG_OUTPUT(("TOFConfigMoniWorkMode Failed \n"));
		Push2ErrStack(PPCFATAL+TOF_FLG+ATOM_ERR+61,0,0,0,0);		/*tof error 61*/		
		return -1;
	}
	return 0;
}


int TOFConfigCLK()
{
	if(g_nCrateID==0x40)
	{

		int clksource=g_TOFDBInfo.tof_clk_header->clksource;

		if(clksource)	
		{
			unsigned uRead;
			WriteToVme32(0x04f00048, VA32, 0x290);//delay register: 6800ps
			ReadFrmVme32(0x04f00048,VA32,&uRead);
			printf("synchronization signal delay value = 0x%x\n",uRead);

			WriteToVme32(0x04f0f070, VA32, 0x0);//start synchronization
			taskDelay(1);
			ReadFrmVme32(0x04f0f070,VA32,&uRead);
			if((uRead&0x3)!=0x2)
			{

					printf("start synchronization error!uRead = 0x%x\n",uRead);
					Push2ErrStack(PPCERROR+TOF_FLG+ATOM_ERR+106,0,0,0,0);		/*tof error 106*/	
					return -1;

			}
			WriteToVme32(0x04f0f0b0, VA32, 0x1);//reset synchronization flag register
			taskDelay(1);
			ReadFrmVme32(0x04f0f0b0,VA32,&uRead);
			if(uRead!=0x1)
			{

					printf("synchronization detect error!uRead = 0x%x\n",uRead);
					Push2ErrStack(PPCERROR+TOF_FLG+ATOM_ERR+107,0,0,0,0);		/*tof error 107*/		
					return -1;

			}		
		}
	
		printf(">>VME:: Clock Board Configure:\n");
		//write :0x0 inner clock, 0x1 divid clock, 0xc reserved clock, 0xe fiber input clock
		
		if((clksource>=0)&&(clksource<=1))
		{
			switch(clksource)
			{
				case 0:
					if(WriteToVme32(0x04f0f030, VA32, 0x0)==0) 
					printf("----Inner Clock      : Write to 0x04f0f030 Successfully!\n");
					break;
				case 1:
					if(WriteToVme32(0x04f0f030, VA32, 0x1)==0) 
					printf("----divid Clock      : Write to 0x04f0f030 Successfully!\n");
					break;
/* for test					
				case 2:
					if(WriteToVme32(0x04f0f030, VA32, 0xc)==0) 
					printf("----reserve Clock      : Write to 0x04f0f030 Successfully!\n");
					break;
				case 3:
					if(WriteToVme32(0x04f0f030, VA32, 0xe)==0) 
					printf("----fiber Clock      : Write to 0x04f0f030 Successfully!\n");
					break;
*/
			}
		}
		else
		{
			DEBUG_OUTPUT(("Clk srouce error\n"));
			Push2ErrStack(PPCFATAL+TOF_FLG+ATOM_ERR+62,0,0,0,0);		/*tof error 62*/		
			return -1;		
		}

	}	
	return 0;
}

int TOFConfigCLKTest()
{
	printf("********test clock configuration code here : \n");

	int base_addr;
	base_addr = 0x04f00000 ;
	unsigned int check_data;	
	
	//set to onboard clock
	WriteToVme32(base_addr +0xf030,VA32, 0x0);
	printf("----TOF onBoard Clock      : Write 0x%x to 0x%x Successfully!\n",0x0,(base_addr +0xf030));
	taskDelay(1);

	printf("test tt1\n");
	taskDelay(60);
	printf("test tt1\n");
	taskDelay(60);

/*
	ReadFrmVme32(base_addr +0xf030 ,VA32,&check_data);
	if((check_data&0xf)!=0x0)
	{
		printf("TOF CLK config: read check error, check_data = 0x%x, write data = 0x%x \n",check_data,0x0);
		Push2ErrStack(PPCERROR+TOF_FLG+ATOM_ERR+84,0,0,0,0);		 
		return -1;
	}
*/
	printf("test tt2\n");
	taskDelay(60);
	printf("test tt2\n");
	taskDelay(60);

	//synchronize
		ReadFrmVme32(base_addr +0xf020 ,VA32,&check_data);	
		
		printf("test tt3\n");
		
		taskDelay(300);
		printf("test tt3\n");

		ReadFrmVme32(base_addr +0xf0b0 ,VA32,&check_data);
		if( check_data!= 0x1)
		{
			printf("TOF CLK config: read check error, check_data (0x%x) != 0x1\n",check_data);
			Push2ErrStack(PPCERROR+TOF_FLG+ATOM_ERR+84,0,0,0,0);
			return -1;
		}
		
		printf("test tt4\n");
		taskDelay(60);
		printf("test tt4\n");
		taskDelay(60);


	//set divid clock
	WriteToVme32(base_addr +0xf030, VA32, 0x1); 
	printf("----TOF divid Clock      : Write to 0x%x Successfully!\n",(base_addr +0xf030));
	taskDelay(1);

	ReadFrmVme32(base_addr +0xf030 ,VA32,&check_data);
	if((check_data&0xf)!=0x1)
	{
		printf("TOF CLK config: read check error, check_data = 0x%x, write data = 0x%x \n",check_data,0x1);
		Push2ErrStack(PPCERROR+TOF_FLG+ATOM_ERR+84,0,0,0,0);
		return -1;
	}


	taskDelay(120);
	printf("TOF Config Clock completed\n");
	return 0;

}
	

int TOFConfigCLKNew()
{
	printf("********new clock configuration code here : \n");

	int base_addr;
	base_addr = 0x04f00000 ;
	int clkConfig = 0;
	printf(">>TOF Clock Board Configure:\n");
	//write :0x0 inner clock, 0x1 divid clock, 0xc reserved clock, 0xe fiber input clock


	int clksource=g_TOFDBInfo.tof_clk_header->clksource;

	unsigned int data = 0x0;
	unsigned int check_data;		
	switch(clksource)
	{
		case 0://onboard
			WriteToVme32(base_addr +0xf030,VA32, data);
			printf("----TOF onBoard Clock      : Write 0x%x to 0x%x Successfully!\n",data,(base_addr +0xf030));
			taskDelay(1);
			
			ReadFrmVme32(base_addr +0xf030 ,VA32,&check_data);
			if((check_data&0xf)!=0x0)
			{
				printf("TOF CLK config: read check error, check_data = 0x%x, write data = 0x%x \n",check_data,data);
				Push2ErrStack(PPCERROR+TOF_FLG+ATOM_ERR+84,0,0,0,0);		 
				return -1;
			}
			break;
		case 1://divid
		/*
			printf("test tt\n");
			taskDelay(60);
			printf("test tt\n");
			taskDelay(60);*/
			WriteToVme32(base_addr +0xf030, VA32, 0x1); 
			printf("----TOF divid Clock      : Write to 0x%x Successfully!\n",(base_addr +0xf030));
			taskDelay(1);
			/*
			printf("test tt1\n");
			taskDelay(60);
			printf("test tt1\n");
			taskDelay(60);*/
			ReadFrmVme32(base_addr +0xf030 ,VA32,&check_data);
			if((check_data&0xf)!=0x1)
			{
				printf("TOF CLK config: read check error, check_data = 0x%x, write data = 0x%x \n",check_data,0x1);
				Push2ErrStack(PPCERROR+TOF_FLG+ATOM_ERR+84,0,0,0,0);
				return -1;
			}
			/*
			printf("test tt2\n");
			taskDelay(60);
			printf("test tt2\n");
			taskDelay(60);*/
			break;
		case 0xf:
			data = 0xf;
			WriteToVme32(base_addr +0xf030,VA32, data);
			printf("----TOF Slave Clock      : Write to 0x%x Successfully!\n",(base_addr +0xf030));
			taskDelay(1);
			
			ReadFrmVme32(base_addr +0xf030 ,VA32,&check_data);
			if((check_data&0xf)!=0xf)
			{
				printf("TOF CLK config: read check error, check_data = 0x%x, write data = 0x%x \n",check_data,data);
				Push2ErrStack(PPCERROR+TOF_FLG+ATOM_ERR+84,0,0,0,0);
				return -1;
			}
			break;
		default:
			printf("clock source error ! \n");
			return -1;
	}



	if(clksource==1)
	{
		/**change the initial value**/
		//int iniValue = 0x158;
		int iniValue = 0x1B7;
		WriteToVme32(base_addr +0xf040,VA32, iniValue);
		printf("Configure Clock : set initial value to 0x%x\n", iniValue);


	
		ReadFrmVme32(base_addr +0xf020 ,VA32,&check_data);	
		
		//printf("test tt3\n");
		
		taskDelay(300);
		

		ReadFrmVme32(base_addr +0xf0b0 ,VA32,&check_data);
		if( check_data!= 0x1)
		{
			printf("TOF CLK config: read check error, check_data (0x%x) != 0x1\n",check_data);
			Push2ErrStack(PPCERROR+TOF_FLG+ATOM_ERR+84,0,0,0,0);
			return -1;
		}	

	}

	
	taskDelay(120);
	printf("TOF Config Clock completed\n");
	return 0;
}


int TOFGlobalReset()
{
	WriteToVme32(0xbc0270, VA32, 0x2c);	//global reset all the FEE& Monitor &ROC
	taskDelay(10);
	return 0;
}

int TOFGlobalResetNew()
{
	 UINT32 addr = 0xbc0270;
	 WriteToVme32(addr, VA32, 0x18); /* stop the module */
	 taskDelay(1);
	 WriteToVme32(addr, VA32, 0x24); /* clear the FEE fifo */
	 taskDelay(1);
	 WriteToVme32(addr, VA32, 0x25); /* clear L1/CHK count */
	 taskDelay(1);
	 MCSTwrite(0x3000000, 32, 0x1); /* clear the pending interrupt */
	 taskDelay(1);
	 WriteToVme32(addr, VA32, 0x26); /* enable L1/CHK count */

	 WriteToVme32(addr, VA32, 0x20); /* reset CR/CSR register */
	 taskDelay(1);
	 WriteToVme32(addr, VA32, 0x22); /* rest the tlk1501s */
	 taskDelay(1);
	 WriteToVme32(addr, VA32, 0x32); /* reset phase shift */
	 taskDelay(1);

	 return 0;

}


int TOFClearRocFEE()
{
	WriteToVme32(0xbc0270, VA32, 0x24);	//clear FEE FIFO
	taskDelay(1);
}

//Self Test VME Configuration: work mode 1
int ROCSelfTestConfig()
{
	UINT L1Latency=g_TOFDBInfo.tof_roc_header->L1Latency;
	UINT semiAutoL1Num=g_TOFDBInfo.tof_roc_header->semiAutoL1Num;
	UINT autoL1Interval=g_TOFDBInfo.tof_roc_header->autoL1Interval;
	//UINT semiAutoL1Num=1;
	
	WriteToVme32(0xbc0270, VA32, 0x18);//set cmd register 2
	taskDelay(1);
	TOFClearRocFEE();
		
	WriteToVme32(0xbc0270, VA32, 0x01);
	taskDelay(1);
	WriteToVme32(0xbc0270, VA32, 0x10);
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
		Push2ErrStack(PPCFATAL+TOF_FLG+ATOM_ERR+63,0,0,0,0);		/*tof error 63*/		
		return -1;
	}
	printf("----Write 0x%08x to 0x00bc0250 Successfully!\n",(g_nTrgPerInt<<25)+(L1Latency<<16)+semiAutoL1Num);
	WriteToVme32(0xbc0270, VA32, 0x18);//set cmd register 2
	taskDelay(1);
	printf(">>VME:: Self Test Mode Configured Successfully!\n");
	return 0;
}

int ROCMonitorConfig()
{
	UINT uRead;
	WriteToVme32(0xbc0270, VA32, 0x18);//set cmd register 2
	taskDelay(1);
	TOFClearRocFEE();
	
	ReadFrmVme32(0x00bc0250,VA32,&uRead);
	printf("before conf roc nTrg/Int register:0x%08x\n",uRead);
	if(-1==WriteToVme32(0x00bc0250, VA32, (g_nTrgPerInt<<25)|(uRead&0x01ffffff) )  )
	{
		printf("ROC nTrg/Int register Conf:0x%08x error\n",(g_nTrgPerInt<<25)|(uRead&0x01ffffff));
		Push2ErrStack(PPCFATAL+TOF_FLG+ATOM_ERR+64,0,0,0,0);		/*tof error 64*/		
		return -1;
	}
	taskDelay(1);
	WriteToVme32(0xbc0270, VA32, 0x2);
	taskDelay(1);
	
}
int ROCNormalConfig()
{
	UINT uRead;
	WriteToVme32(0xbc0270, VA32, 0x18);//disable ,mask L1
	taskDelay(1);
	TOFClearRocFEE();
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
	//set mode
}

//standalone function : for get CLK phase
unsigned get_mask(int dp)
{
	unsigned ret = 0u;
	int i;
	for(i = 0; i < dp; ++i){
		ret |= 1u << i;
	}
	return ret;
}


//-------------------------------------------------------------------------
//interprete phase status register
//-------------------------------------------------------------------------
void phase_status_mask(unsigned *p, int len)
{
	int single_bit = 1;
	p[PHASE_LOS]		= get_mask(single_bit);		//los
	p[PHASE_TOUT]		= get_mask(single_bit);		//tmout
	p[PHASE_RXER]		= get_mask(single_bit);		//rxer
	p[PHASE_TOUTSTEP]	= get_mask(PTAP_TOUT_STEP_W);	//tmout step
	p[PHASE_BUSY]		= get_mask(single_bit);		//busy
	p[PHASE_OPESTA]		= get_mask(single_bit);		//ope state
	p[PHASE_CURRECV]	= get_mask(single_bit);		//currecv
	p[PHASE_CURPLL]		= get_mask(single_bit);		//curpll
	p[PHASE_DEST]		= get_mask(single_bit);		//dest
	p[PHASE_PCNTNEW]	= get_mask(single_bit);		//new
	p[PHASE_PCNTEFF]	= get_mask(single_bit);		//effective
	p[PHASE_PCNT]		= get_mask(PTAP_PDIFFCNT_W);
}

int ROC_get_phase()
{
	printf("**ROC get CLK phase function here : \n");

	UINT uRead,dat;

	WriteToVme32(0xbc0270, VA32, 0x32);
	taskDelay(1);


	WriteToVme32(0xbc0280, VA32, 0x7FF0660);// configure phase ajust mode
	taskDelay(1);
	
	WriteToVme32(0xbc0270, VA32, 0x33);//start phase ajustment
	taskDelay(180);

	ReadFrmVme32(0xBC0284,VA32,&uRead);//readout status code
	printf("ROC_get_phase : read status register data :0x%x\n",uRead);
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


	//deal with these error information here:	

}

int shiftch()
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
		if ((fOpts = fopen("TOF_ROC_shift.txt","r")) == NULL)
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

int ROC_ajust_phase()
{
	printf("**ROC ajust CLK phase function here : \n");

	UINT uRead,dat;

	WriteToVme32(0xbc0270, VA32, 0x32);
	taskDelay(1);


	WriteToVme32(0xbc0280, VA32, 0x7FF1290);// 7FF1XXX 660-->290
	taskDelay(1);
	
	WriteToVme32(0xbc0270, VA32, 0x33);//start phase ajustment
	taskDelay(60);

	ReadFrmVme32(0xBC0284,VA32,&uRead);//readout status code
	printf("ROC_ajust_phase : read status register data :0x%x\n",uRead);
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
		printf("ROC_ajust_clock_phase error! \n");
		Push2ErrStack(PPCFATAL+TOF_FLG+ATOM_ERR+51,0,0,0,0);		/*tof error 51*/
		return -1;
	}

	

}


int ROCNormalConfigNew()
{
	printf("********new ROC normal configuration code here : \n");

	UINT uRead;

	//if (-1==ROC_ajust_phase())
	if(-1==shiftch())
	{
		printf("ROC configure failed !\n");
		return -1;
	}
	
	WriteToVme32(0xbc0270, VA32, 0x18);//disable ,mask L1
	taskDelay(1);

	//jixl20150129
	WriteToVme32(0xbc0270, VA32, 0x37);//disable  CTTP_FAULT
	taskDelay(1);
	//WriteToVme32(0xbc0270, VA32, 0x13);//disable  status signal
	taskDelay(1);
	WriteToVme32(0xbc025C, VA32, 0xFF9);//disable 
	taskDelay(1);
	
	TOFClearRocFEE();
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
	//set mode
}


/* helper function to retrive the configure parameters from the cf.
   @name: the name of the cf file
   @opt: the array store the retrived parames
   @return: the effective array length if succeed, else -1
*/
int TOFretrievePrams(const char *name, UINT32 *opt)
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
UINT32 TOFprintReg(unsigned addr)
{
	UINT32 d;
	ReadFrmVme32(addr,VA32,&d);
	printf("0x%08x = %08x\n", addr, d);
	return d;
}


int TOFROCmtlkConfig()
{
	UINT32 opt[6]; /* retrieve them form db or cf */
	UINT32 addr = 0xBC025C;
	UINT32 val = 0;

	char *filename;
	if(g_nCrateID == 64)
		filename = "TOF_ROC_mtlk_64.txt";
	else
		filename = "TOF_ROC_mtlk_65.txt";

	
	//if(retrievePrams("TOF_ROC_mtlk.txt", opt) != 6)
	if(TOFretrievePrams(filename, opt) != 6)
		return -1;
	
	val = (opt[0] & 0x7) |
		  (opt[1] & 0x7) << 3 |
		  (opt[2] & 0x7) << 6 |
		  (opt[3] & 0x7) << 9 |
		  (opt[4] & 0x7) << 12 |
		  (opt[5] & 0xF) << 15;

	printf("in mtlkConfig :opt[0] = 0x%x, opt[4] = 0x%x, opt[5] = 0x%x \n",opt[0],opt[4],opt[5]);
	WriteToVme32(addr, VA32, val); /* confgure it */
	TOFprintReg(addr);

	return 0;
}

/* ROC B1 fibre channel(receive trigger sys control signals) delay unc-remove */
int tofshiftch()
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
	if(g_nCrateID == 64)
		filename = "TOF_ROC_shift_64.txt";
	else
		filename = "TOF_ROC_shift_65.txt";

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
	TOFprintReg(0xBC0280);

	/* configure the shift value registers */
	shift_val = (opt[0] & 0xFF) | 
				((opt[1] & 0xFF)<<8) | 
				((opt[2] & 0xFF)<<16) | 
				((opt[3] & 0xFF)<<24);
	WriteToVme32(0xbc0288, VA32, shift_val);
	TOFprintReg(0xBC0288);
	taskDelay(1);

	shift_val = opt[4] & 0xFF;
	WriteToVme32(0xbc028C, VA32, shift_val);
	TOFprintReg(0xBC028C);
	taskDelay(1);
	
	/* shift it ! */
	WriteToVme32(0xbc0270, VA32, 0x33);
	taskDelay(1);

	return 0;
}


int TOFROCNormalConfigV3()
{
	printf("********V3 TOF ROC normal configuration code here : \n");

	UINT uRead;

	WriteToVme32(0xbc0270, VA32, 0x18);//stop the fctrl
	taskDelay(1);

	/* confgure the MTLK */
	TOFROCmtlkConfig();

	/* configure stlk uncertaintity logic and L1 output phase */
	if(-1==tofshiftch())
	{
		printf("ROC configure failed !\n");
		return -1;
	}

	WriteToVme32(0xBC0260, VA32, (0x1 << 10)); //disable CTTP fault
	taskDelay(1);

	//WriteToVme32(0xbc0270, VA32, 0x13);//disable  status signal	
	//WriteToVme32(0xbc025C, VA32, 0xFF9);//has been put in ROCmtlkConfig
	
	TOFClearRocFEE();
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



int TOFConfigROC()
{
	//WriteToVme32(0xbc0250, VA32, ((g_nTrgPerInt<<25)+(128<<16)+16));
	UINT uTemp;
	
	//write fask control board, 0x16 inner clock,0x17 out clokc
	if(g_TOFDBInfo.tof_roc_header->clksource==0)
	{
		if(WriteToVme32(0xbc0270, VA32, 0x16)==0)/*inner clk,general use for hw test*/
		printf("Clock    : Write 0x00000016 to 0x00bc0270 Successfully!\n");
	}
	else if (g_TOFDBInfo.tof_roc_header->clksource==1)
	{
		if(WriteToVme32(0xbc0270, VA32, 0x17)==0)/*external clk,clk from clk module*/
		printf("ROC ext clk: Write 0x00000017 to 0x00bc0270 Successfully!\n");
	}
	else
	{
		printf("config clk source error\n");
		Push2ErrStack(PPCFATAL+TOF_FLG+ATOM_ERR+66,0,0,0,0);		/*tof error 66*/		
		return -1;
	}

	UINT writeTest=1111;
	WriteToVme32(0x00bc0290, VA32,writeTest);
	ReadFrmVme32(0x00bc0290,VA32,&uTemp);
	if(writeTest!=uTemp)
	{
		printf("write&read ROC test register error\n");
		Push2ErrStack(PPCFATAL+TOF_FLG+ATOM_ERR+67,0,0,0,0);		/*tof error 67*/		
		return -1;
	}

	switch(g_nRunMode)
	{
		case NOMALMODE:
			if(g_TOFDBInfo.tof_ppc_header->NormModeSelfL1)
			{
				ROCSelfTestConfig();
			}
			else
			{
				if (-1==TOFROCNormalConfigV3())
				{
					printf("ROC Normal Mode Config Failed\n");
					return -1;
				}
					//if (-1==ROCNormalConfig())
			}
			break;
		case PEDMODE:
			ROCNormalConfig();
			break;
		case CALIBRATION:
			ROCSelfTestConfig();
			break;
		case MONITOR:
			ROCMonitorConfig();
			break;
		case BASELINE:
		case WAVEMODE:		
			break ;
		default:
			Push2ErrStack(PPCFATAL+TOF_FLG+ATOM_ERR+9,0,0,0,0);		/*tof error 9*/		
			break ;
	}
	
	return 0;
}


int TOFConfigAll()
{
	UINT nGeo;
	UINT nadr;

	//jixl 20150212 for test
	/*
	printf("test1\n");
	taskDelay(60);
	printf("test1\n");
	taskDelay(60);
	*/

	//jixl20150318
	//WriteToVme32(0xbc0270, VA32, 0x18);//stop work. disable ,mask L1
	//taskDelay(1);
	//WriteToVme32(0xbc0270, VA32, 0x16);//inner clock
	//taskDelay(1);

	
	/*
	printf("test2\n");
	taskDelay(60);
	printf("test2\n");
	taskDelay(60);
	*/
	
	if(g_TOFDBInfo.tof_clk_sum==1)
	{
		if(g_nCrateID==0x40)
		{
			if(-1==TOFConfigCLKNew())
			//if(-1==TOFConfigCLK())
			{
				DEBUG_OUTPUT(("TOFConfigCLK error\n"));
				Push2ErrStack(PPCFATAL+TOF_FLG+ATOM_ERR+69,0,0,0,0);		//tof error 69	
				return -1;
			}
		}
	}
	else
	{
		DEBUG_OUTPUT(("Have no Clk config infor, config fail\n"));
		Push2ErrStack(PPCFATAL+TOF_FLG+ATOM_ERR+70,0,0,0,0);		//tof error 70
		return -1;
	}
	/*
	printf("test3\n");
	taskDelay(60);
	printf("test3\n");
	taskDelay(60);
	*/
	if(-1==TOFConfigFeeMonitor())
	{
		DEBUG_OUTPUT(("TOFConfigFeeMonitor error\n"));
		Push2ErrStack(PPCFATAL+TOF_FLG+ATOM_ERR+71,0,0,0,0);		//tof error 71	
		return -1;
	}
	/*
	printf("test4\n");
	taskDelay(60);
	printf("test4\n");
	taskDelay(60);
	*/

	if(-1==TOFConfigROC())
	{
		DEBUG_OUTPUT(("TOFConfigROC error\n"));
		Push2ErrStack(PPCFATAL+TOF_FLG+ATOM_ERR+72,0,0,0,0);			
		return -1;
	}
	/*
	printf("test5\n");
	taskDelay(60);
	printf("test5\n");
	taskDelay(60);
	*/
/*
	if(-1==TDIG_function())
	{
		DEBUG_OUTPUT(("TDIG Config Failed !\n"));
		Push2ErrStack(PPCFATAL+TOF_FLG+ATOM_ERR+15,0,0,0,0);		
		return -1;
	}	
	*/
	TOFClearRocFEE();
	
	return 0;
}

/* check the downstream(i.e. 2 ETOF fctrl plus floor 1 bTOF fctrl) fibre channel
   connection condition. In the old system, the job is done by the Trig Fctrl,
   but now for the floor 3 TOF fctrl will seperate the connection between Trig
   Fctrl and the downstream, so ... 
   Note: here the 0xBC025C regs [20:25] indicate error type for E1,E2 and B2,
	     each one use 2bits and each bit indicate upstream fibre fail and
		 downstream fibre fail. if all ok then will return 0 else return non-
		 zero. the caller can interpreate the return value for easy understand
		 formats to the users.
*/
int ROCmtlkChk()
{
	UINT32 d, addr = 0xBC025C;
	WriteToVme32(0xBC0270, VA32, 0x21);  /* start check */
	taskDelay(2); /* check will cost ~300us */
	d = TOFprintReg(addr);
	
	/* return check conclusion */
	return ((d >> 20) & 0x3F);
}

/* only need to be called before start the card, this function is used to clear
   the pending mtlk unsync status. This may be possible, because when power up
   the pll are not locked and the fibre trace have delay about 6 cloks.
*/
void clearPendUsync()
{
	UINT32 d, addr = 0xBC025C;
	d = TOFprintReg(addr);
	d = (d & 0xFFFFF) | (0x7 << 20);
	WriteToVme32(addr, VA32, d);  /* clear them */	
}


int TOFFiberChk()
{
	/* check the fibre connection, when execute this function for floor 3 card,
	   must be sure the ETOF cards have been configured ok. So maybe this function
	   should be put into the next level configuration stage, i.e. in the DAQ
	   start run routing or just add a new stage between confgure routing and
	   start routing, execute the below two functions in the new stage.
	 */
	 UINT32 uRead;
	uRead = ROCmtlkChk();
	if(uRead){ /* means error happened, do something ... */
		printf("three TOF sub-fctrl fibre sync error: 0x%x \n",uRead);
		return -1;	
	}
	
	/* clear the mtlk pending unsync status, the run coundtion like above func */
	clearPendUsync();

	printf("TOFFiberChk SUCC\n");

	return 0;
}

int SelfTestAmp_Setup(UINT32 SelfTestAmp)
{
	//must calculate it
	unsigned int nBaseAddr=0;
	SelfTestAmp=SelfTestAmp*4096/(2500*2);
	
	for(int nModuIndx=0;nModuIndx<g_TOFDBInfo.tof_FEE_sum;nModuIndx++)  
	{   
			UINT nGeo=g_TOFDBInfo.tof_FEE_header[nModuIndx]->geo;
			nBaseAddr=(nGeo<<20);
			if ( -1 == WriteToVme32(nBaseAddr+0x4000c, VA32, SelfTestAmp) )  
			{
				//SelfTest Amp Setup fail
				printf(">>VME:: SelfTest Amp Setup: Write SelfTestAmp 0x%08x to 0x%08x fail \n",SelfTestAmp,nBaseAddr+0x4000c);
				Push2ErrStack(PPCFATAL+TOF_FLG+ATOM_ERR+73,0,0,0,0);		/*tof error 73*/	
				return -1;
			} 
			else 
			{
				printf(">>VME:: SelfTest Amp Setup: Write SelfTestAmp 0x%08x to 0x%08x success \n",SelfTestAmp,nBaseAddr+0x4000c);  
			}
			taskDelay(1);
	}
	//monitor change amp
	nBaseAddr=(g_uTofMonitorGeo<<20);
	if ( -1 == WriteToVme32(nBaseAddr+0x4000c, VA32, SelfTestAmp) )  
	{
		//SelfTest Amp Monitor Write fail
		printf(" SelfTest Amp Monitor Write 0x%08x to 0x%08x fail \n",SelfTestAmp,nBaseAddr+0x4000c);
		Push2ErrStack(PPCFATAL+TOF_FLG+ATOM_ERR+74,0,0,0,0);		/*tof error 74*/	
		return -1;
	} 
	return 0;
}

void TOFGenL1()//only for normal mode selfL1 and Calibration mode 
{
	int			nTimeIdx=0;
	int			nPointIdx=0;
	
	semTake(g_sidSATR, WAIT_FOREVER) ;
	g_bTrgStop=false;
	switch(g_nRunMode)
	{
		case NOMALMODE:
			if(g_TOFDBInfo.tof_ppc_header->NormModeSelfL1)
			{
				while(1)
				{
					WriteToVme32(0xbc0270, VA32, 0x19);
					taskDelay(1);
					if(g_bTrgStop)
					{
						TOFRocStop();
						break;
					}
				}
			}
			break;
		case CALIBRATION:
			while(1)
			{
				for(nTimeIdx=0;nTimeIdx<g_nL1Count;nTimeIdx+=g_TOFDBInfo.tof_roc_header->semiAutoL1Num)
				{
					if ( -1 == WriteToVme32(0xbc0270, VA32, 0x19) ) 
					{
						printf("\n Write 0xbc0270 gen L1 error\n");
						Push2ErrStack(PPCFATAL+TOF_FLG+ATOM_ERR+75,0,0,0,0);		/*tof error 75*/	
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
		default:
			DEBUG_OUTPUT(("TOF GenL1 only for normModeSelfL1 and Calibration\n"));
			Push2ErrStack(PPCFATAL+TOF_FLG+ATOM_ERR+76,0,0,0,0);		/*tof error 76*/	
			break ;
	}

	nTOFGenL1TaskId=0;
}

void TOFRocStartL1()
{
	WriteToVme32(0xbc0270, VA32, 0x19);//begin work
}

void TOFRocStop()
{
	WriteToVme32(0xbc0270, VA32, 0x18);
}

//specail scan for check,not run in stat chart,only run separately
void TOFScanBoardID()
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

void tttest(int geo)
{
	unsigned nAddr;
	unsigned abc;
	nAddr=geo<<20;
	
	ReadFrmVme32(nAddr,VA32,&abc);
	printf("0x%x=0x%x\n",nAddr,abc);
	abc=0xffffffff;
	ReadFrmVme32(nAddr+0x4,VA32,&abc);
	printf("0x%x=0x%x\n",nAddr+0x4,abc);
	abc=0xffffffff;
	ReadFrmVme32(nAddr+0x10550,VA32,&abc);
	printf("0x%x=0x%x\n",nAddr+0x10550,abc);
}





/**for TDIG module config**/
/*
int TDIG_resetCPLD(unsigned int base_addr)
{

	if( -1==WriteToVme32(base_addr + 0x800cc,VA32, 5) )
	{
		printf("TDIG error while write to VME during resetCPLD\n");
		Push2ErrStack(PPCFATAL+MRPC_FLG+ATOM_ERR+30,0,0,0,0);			
		return -1;
	}
	printf("resetCPLD : write data 0x%x to addr 0x%x succ \n",5,base_addr + 0x800cc);
	taskDelay(1);
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
			Push2ErrStack(PPCFATAL+MRPC_FLG+ATOM_ERR+31,0,0,0,0);		
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
			Push2ErrStack(PPCFATAL+MRPC_FLG+ATOM_ERR+32,0,0,0,0);		
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
			Push2ErrStack(PPCFATAL+MRPC_FLG+ATOM_ERR+33,0,0,0,0);		
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
			Push2ErrStack(PPCFATAL+MRPC_FLG+ATOM_ERR+34,0,0,0,0);		
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
		Push2ErrStack(PPCFATAL+MRPC_FLG+ATOM_ERR+35,0,0,0,0);		
		return -1;
	}

	taskDelay(6);
	return 0;
}

int TDIG_TOFConfigFeeBaseAddr(unsigned int nGeo)
{
	WriteToVme8((nGeo<<19)+0x7ffd8+0x3, VCSR, (nGeo&0x0f)<<4);  
	WriteToVme8((nGeo<<19)+0x7ffdc+0x3, VCSR, (nGeo&0xf0)>>4);	
	printf("write:0x%08x to address: 0x%08x\n",nGeo<<4,(nGeo<<19)+0x7ffdc+0x3);
	return 0;
}


int TDIG_configFPGA(unsigned int base_addr)
{

	if( -1==WriteToVme32(base_addr + 0x80008,VA32, 6) )
	{
		printf("TDIG error while write to VME during configFPGA\n");
		Push2ErrStack(PPCFATAL+MRPC_FLG+ATOM_ERR+36,0,0,0,0);		
		return -1;
	}
	printf("write data 0x%x to addr 0x%x succ \n",6,base_addr + 0x80008);
	printf(" configFPGA SUCC\n");
	taskDelay(6);
	return 0;
	
}
*/
/*
int INLenable(unsigned int base_addr)
{
	if( -1== WriteToVme32(base_addr + 0x3c,VA32, 0x1) )
	{
		printf("TDIG error while write to VME during INLenable\n");
		Push2ErrStack(PPCERROR+MRPC_FLG+ATOM_ERR+41,0,0,0,0);		
		return -1;
	}

	taskDelay(6);
	return 0;
}

int TDIG_configReadoutEvent(unsigned int base_addr)
{
	unsigned int data;
	data = 0x80000000+g_nTrgPerInt;
	if( -1==WriteToVme32(base_addr +0xc,VA32, data) )
	{
		printf("TDIG error while write to VME during TDIG_configReadoutEvent\n");
		Push2ErrStack(PPCERROR+MRPC_FLG+ATOM_ERR+42,0,0,0,0);		
		return -1;
	}
	taskDelay(60);
}

int TDIG_HPTDCopen(unsigned int base_addr)
{
	for(int i=0;i<2;i++)
	{
		if( -1==WriteToVme32(base_addr +0x8,VA32, 0x82) )
		{
			printf("TDIG error while write to VME during HPTDCopen\n");
			Push2ErrStack(PPCERROR+MRPC_FLG+ATOM_ERR+43,0,0,0,0);		
			return -1;
		}
		taskDelay(60);
	}
	return 0;
}

int TDIG_disable(unsigned int base_addr)
{
	if( -1==WriteToVme32(base_addr +0x8,VA32, 0x4) )
	{
		printf("TDIG error while write to VME during TDIG_disable\n");
		Push2ErrStack(PPCERROR+MRPC_FLG+ATOM_ERR+44,0,0,0,0);		
		return -1;
	}
	taskDelay(6);
	return 0;
}


int TDIG_setL1(unsigned int base_addr)
{

	if( -1==WriteToVme32(base_addr +0x0,VA32, 0x4000) )
	{
		printf("TDIG error while write to VME during TDIG_setL1\n");
		Push2ErrStack(PPCERROR+MRPC_FLG+ATOM_ERR+45,0,0,0,0);		//mrpc error 45
		return -1;
	}
	printf("TDIG_setL1 : write 0x4000 to 0x%x\n",base_addr+0x0);
	taskDelay(30);

	return 0;
}


int TDIG_Clear(unsigned int base_addr)
{

	if( -1==WriteToVme32(base_addr +0x8,VA32, 0x2) )
	{
		printf("TDIG error while write to VME during ClearFIFO\n");
		Push2ErrStack(PPCERROR+MRPC_FLG+ATOM_ERR+46,0,0,0,0);		//mrpc error 46
		return -1;
	}
	else
		printf("TDIG cleared!\n");
	taskDelay(1);

	return 0;
}

int TDIG_config(unsigned int base_addr)
{

	if(-1==TDIG_configFPGA(base_addr))
		return -1;
	
	if(inlCorrection)
	{
		if(-1==TDIG_configINL("ch1.txt",base_addr))
			return -1;
		if(-1==TDIG_configINL("ch2.txt",base_addr))
			return -1;
		if(-1==TDIG_configINL("ch3.txt",base_addr))
			return -1;	
		if(-1==TDIG_configINL("ch4.txt",base_addr))
			return -1;
		if(-1==TDIG_configINL("ch5.txt",base_addr))
			return -1;
		if(-1==TDIG_configINL("ch6.txt",base_addr))
			return -1;
		if(-1==TDIG_configINL("ch7.txt",base_addr))
			return -1;
		if(-1==TDIG_configINL("ch8.txt",base_addr))
			return -1;
		if(-1==TDIG_configINL("ch9.txt",base_addr))
			return -1;		

		if(-1==TDIG_readcheckINL("ch1.txt",base_addr))
			return -1;		
		if(-1==TDIG_readcheckINL("ch2.txt",base_addr))
			return -1;	
		if(-1==TDIG_readcheckINL("ch3.txt",base_addr))
			return -1;	
		if(-1==TDIG_readcheckINL("ch4.txt",base_addr))
			return -1;		
		if(-1==TDIG_readcheckINL("ch5.txt",base_addr))
			return -1;	
		if(-1==TDIG_readcheckINL("ch6.txt",base_addr))
			return -1;	
		if(-1==TDIG_readcheckINL("ch7.txt",base_addr))
			return -1;		
		if(-1==TDIG_readcheckINL("ch8.txt",base_addr))
			return -1;	
		if(-1==TDIG_readcheckINL("ch9.txt",base_addr))
			return -1;	
		
	}

	taskDelay(10);
	
	if(-1==TDIG_configHPTDC(base_addr))
		return -1;
	if(-1==TDIG_readcheckHPTDC(base_addr))
		return -1;
	
	if (-1==TDIG_HPTDCopen(base_addr))
		return -1;
	
	if (-1==INLenable(base_addr))
		return -1;	
	
	if(!TDIG_CBLTEn) //disable
	{
		if (-1==TDIG_disable(base_addr))
			return -1;	

		printf("**********disable TDIG module !!!**********\n");
	}

	return 0;
}

int TDIG_function()
{
	inlCorrection = 1;
	reject_cnt_offset = 0x557;//8550ns
	search_window = 0xa0;//2000ns
	match_window = 0xa0;
	trigger_latency = 0x557;
	TDIG_boardEn = 0;
	TDIG_CBLTEn = 0;
	TDIG_geo = 19;

	if(g_nRunMode==NOMALMODE)	
		TDIG_CBLTEn = (TDIG_CBLTEn&0x1);
	else
	{
		TDIG_CBLTEn = 0;
		printf("runmode != normalmode ,  set TDIG disabled! \n");
	}
	printf("inlCorrection = %d ; reject_cnt_offset = 0x%x ; search_window = 0x%x \n; 
		match_window = 0x%x ;trigger_latency = 0x%x \n",
		inlCorrection,reject_cnt_offset,search_window,match_window,trigger_latency);

	printf("TDIG_boardEn = %d ,TDIG_CBLTEn = %d , TDIG_geo = %d \n",TDIG_boardEn,TDIG_CBLTEn,TDIG_geo);

	if(TDIG_boardEn)
	{
		if(-1==TDIG_resetCPLD(TDIG_geo<<20))
			return -1;
		
		TDIG_TOFConfigFeeBaseAddr(TDIG_geo);
		printf("TDIG_TOFConfigFeeBaseAddr finished \n");
	}
	
	if(TDIG_boardEn&&TDIG_CBLTEn)
	{
		g_uTofCbltGeo[g_uTofCbltModuleSum] = TDIG_geo;
		g_uTofCbltModuleSum++;		
		
		printf("insert TDIG in CBLT chain ... then g_uTofCbltModuleSum = %d \n",g_uTofCbltModuleSum);
		for(int i=0;i<g_uTofCbltModuleSum;i++)
			printf("***g_uTofCbltGeo[%d] = %d\n",i,g_uTofCbltGeo[i]);
		
		if(-1==TDIG_configCBLTChain(TDIG_geo,"last"))
		{
			printf("TDIG configCBLTChain failed \n");
			Push2ErrStack(PPCERROR+MRPC_FLG+ATOM_ERR+47,0,0,0,0);		//mrpc error 47
			return -1;
		}
		else
		{
			printf("TDIG configCBLTChain succ \n");
		}
	}
	else if( (TDIG_boardEn)&&(!TDIG_CBLTEn))
	{
		if(-1==TDIG_configCBLTChain(TDIG_geo,"middle"))
		{
			printf("TDIG configCBLTChain failed 2\n");
			Push2ErrStack(PPCERROR+MRPC_FLG+ATOM_ERR+48,0,0,0,0);		//mrpc error 48
			return -1;
		}
		else
		{
			printf("TDIG configCBLTChain succ \n");
		}
	}
		
	if(TDIG_boardEn)
	{
		if(-1==TDIG_config(TDIG_geo<<20))
		{
			printf("TDIG_config failed\n");
			Push2ErrStack(PPCERROR+MRPC_FLG+ATOM_ERR+49,0,0,0,0);		//mrpc error 49
			return -1;
		}
		else
			printf("TDIG_config succ\n");
	}

	return 0;
}

*/



/* this file include the functions needed to configure the TOF ROC */
/* put function declarations here for easy copy to the header files */
int resetch(); /* reture phase of L1 to powerup position */
int clrshiftcfg(); /* clear the phase shift related register setup */
UINT32 printReg(unsigned addr); /* helper, printf and return the data of addr reg */
int enableTDC(int ena); /* enable the tdc module */

/* reset the phase shift module */
int resetch()
{
	WriteToVme32(0xbc0270, VA32, 0x32);
	taskDelay(1);
	return 0;
}

/* when you reset shift, the control regsiter of it will not change,
   you can clear them by this function */
int clrshiftcfg()
{
	printf("before clean, register value:\n");
	printReg(0xBC0280);
	printReg(0xBC0288);
	printReg(0xBC028C);
	WriteToVme32(0xbc0280, VA32, 0x0);
	WriteToVme32(0xbc0288, VA32, 0x0);
	WriteToVme32(0xbc028C, VA32, 0x0);
	printf("after clean, regsiter value:\n");
	printReg(0xBC0280);
	printReg(0xBC0288);
	printReg(0xBC028C);
	return 0;
}

/* helper to simplify the code */
UINT32 printReg(unsigned addr)
{
	UINT32 d;
	ReadFrmVme32(addr,VA32,&d);
	printf("0x%08x = %08x\n", addr, d);
	return d;
}

/* enable or disable TDC module */
int enableTDC(int ena)
{
	UINT32 val = 0;
	if(ena != 0 && ena != 1){
		printf("error: parameter should be 0 or 1(enable)\n");
		return -1;
	}
	val = (ena & 0x1) << 31;
	WriteToVme32(0xBC0220, VA32, val);
	return 0;
}

int shiftch_test()
{
	UINT32 opt[100]={0};
	int icnt = 0;
	UINT32 uRead;

	UINT32 shift_val = 0;
	UINT32 val = 0;
	UINT32 shift_set = 0;

	/* init the opt */
	opt[0] = shift_set;
	opt[1] = shift_set;
	opt[2] = 0xFF;
	opt[3] = 0xFF;
	opt[4] = 0x0;
	opt[5] = 0x0;
	opt[6] = 0x0;

	while(shift_set < 0xFF){
		resetch();
		clrshiftcfg();

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
		val = val | 0x100 | ((opt[5] & 0x1) << 12) | ((opt[6] & 0x7) << 14);
		WriteToVme32(0xbc0280, VA32, val);
		/* read it for debugging purpose */
		printReg(0xBC0280);
	
		/* configure the shift value registers */
		shift_val = (opt[0] & 0xFF) | 
					((opt[1] & 0xFF)<<8) | 
					((opt[2] & 0xFF)<<16) | 
					((opt[3] & 0xFF)<<24);
		WriteToVme32(0xbc0288, VA32, shift_val);
		printReg(0xBC0288);
		taskDelay(1);
	
		shift_val = opt[4] & 0xFF;
		WriteToVme32(0xbc028C, VA32, shift_val);
		printReg(0xBC028C);
		taskDelay(1);
		
		/* shift it ! */
		WriteToVme32(0xbc0270, VA32, 0x33);
		taskDelay(1);

		/* enable measure */
		enableTDC(1);
		taskDelay(16);
		printf("%x: ", shift_set);
		printReg(0xBC0220);
		taskDelay(16);
		printf("%x: ", shift_set);
		printReg(0xBC0220);
		taskDelay(16);
		printf("%x: ", shift_set);
		printReg(0xBC0220);

		enableTDC(0);
		shift_set += 0xF;
		opt[0] = shift_set;
		opt[1] = shift_set;
	}	
	return 0;
}


