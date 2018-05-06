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

#include "sd_zdd.h"
#include "sd_zdd_hw.h"
#include "sd_mdc_hw.h"
#include "sd_trg_hw.h"

bool g_bZDDRocIntConed=false;



void ZDDIntHandler(int nArg)
{
	//logMsg("interrupt receive OK,%d\n", nArg, 0, 0, 0, 0, 0);

	semGive(g_sidInterrupt);
	g_nInterruptCounter+=g_nTrgPerInt;
}

int   ZDDConfIntCBLT()
{
	
	if(FALSE == g_bZDDRocIntConed) 
	{
		if(intConnect (INUM_TO_IVEC(ZDD_INT_VEC), (VOIDFUNCPTR)ZDDIntHandler, 0) == ERROR)
	  	{
	     		printf("intConnect  failed.\n");
			Push2ErrStack(PPCFATAL+ZDD_FLG+ATOM_ERR+60,0,0,0,0);		/*zdd error 60*/	
			return  -1;
	  	}
		g_bZDDRocIntConed = TRUE ;
	}

 	printf("Initializing D32 cblt transfers \n"); 
	DMAinit(D32,D32_BLOCK,VME_A32_D32_BLT,TO_LOCAL);

	return 0;
}


int V1721_configDAQControl(int nMdlIdx) 
{
	UINT32 DAQCtrl = ctALL | rmREGISTERCONTROLLED;
	UINT32 DAQCtrlReg =  g_ZDDDBInfo.v1721_header[nMdlIdx]->BaseAddr+0x8100;
	UINT32 error0 = WriteToVme32(DAQCtrlReg, VA32, DAQCtrl);

	return(error0);
}

int V1721_configChanConf(int nMdlIdx) 
{
	/* ATTENTION: writing to 0x8000 msENABLED is NECESSARY!!*/
	UINT32 chanConf = zsZEROLENGTH | tiUNDERTHRESHOLD | msENABLED | tpDISABLED | toENABLED;
	UINT32 chanConfReg = g_ZDDDBInfo.v1721_header[nMdlIdx]->BaseAddr+0x8000;
	UINT32 error0 = WriteToVme32(chanConfReg,VA32,chanConf);
	return(error0);
}

int V1721_configZS(int nMdlIdx) //in ZLE mode
{
	UINT32 error = 0;

	UINT32 ChanZSThReg =g_ZDDDBInfo.v1721_header[nMdlIdx]->BaseAddr+0x1024;
	UINT32 ChanZSNsReg = g_ZDDDBInfo.v1721_header[nMdlIdx]->BaseAddr+0x1028;
	UINT32 chan=0;

	UINT32 chanConf = zsZEROLENGTH;
	UINT32 chanConfReg = g_ZDDDBInfo.v1721_header[nMdlIdx]->BaseAddr+0x8004;
	UINT32 error0 = 0;
  
	for ( chan=0; chan<8; chan++ ) 
	{
//		UINT32 mask = (g_ZDDDBInfo.v1721_header[nMdlIdx]->baseLine[chan]
//					-g_ZDDDBInfo.v1721_header[nMdlIdx]->threshold[chan]) | 0x80000000; /* bit 31 for negative logic*/

		UINT32 mask = 0xf ;
		error += WriteToVme32(ChanZSThReg,VA32,mask);
		ChanZSThReg += 0x0100;
		
		mask = ((g_ZDDDBInfo.v1721_header[nMdlIdx]->nLookBack[chan])<<16 ) 
				+(g_ZDDDBInfo.v1721_header[nMdlIdx]->nLookForward[chan]);  /* NLBK in bits 31-16, NLFW in bits 15-0*/
		error += WriteToVme32(ChanZSNsReg,VA32,mask);
		ChanZSNsReg += 0x0100;
  	}

	error0  = WriteToVme32(chanConfReg,VA32,chanConf);

	return(error+error0);
}

int V1721_configMemory(int nMdlIdx) 
{
	UINT32 BufOrg = 8; /*acceptable values 1024, 512, 256, .... 4, 2,1 0x0111 means 128 buffers of 16K samples*/
	UINT32 BufOrgReg = g_ZDDDBInfo.v1721_header[nMdlIdx]->BaseAddr+0x800C;
	UINT32 error0 = WriteToVme32(BufOrgReg,VA32,BufOrg);

	/*UINT32 CustomSize  = g_ZDDDBInfo.v1721_header[nMdlIdx]->customSize;*/ /* "locations" in manual: buffer size = 8*"locations" byte per channel*/
	UINT32 CustomSize  = 510; //200->510 20130129
//	if (nMdlIdx==1)
//		CustomSize = 200;
	UINT32 CustSizeReg = g_ZDDDBInfo.v1721_header[nMdlIdx]->BaseAddr+0x8020;
	UINT32 dataWin = CustomSize*8*2;
	printf("data memory is set to %d ns\n",dataWin);
	UINT32 error1 = WriteToVme32(CustSizeReg,VA32,CustomSize);
	
	CustomOut  = 25;  //jixl: change to 25 20150202
	NoSupDataLen = (CustomOut*8*8+16)*2;
	printf("NoSupDataLen = %d\n",NoSupDataLen);
	UINT32 CustOutReg = g_ZDDDBInfo.v1721_header[nMdlIdx]->BaseAddr+0x8040;
	UINT32 realdataWin = CustomOut*8*2;
	printf("real data window is set to %d ns\n",realdataWin);

	UINT32 error2 = WriteToVme32(CustOutReg,VA32,CustomOut);
	
	return(error0+error1+error2);
}

int V1721_configDACs(int nMdlIdx, UINT32 separate)
{
	UINT32 error = 0;
	UINT32 busy = 0;
	int cnt = 0;

	/* set DAC offsets for negative voltages;*/
	UINT32 DACvalues[8] = {0x2000,0x3000,0x4000,0x5000,0x6000,0x7000,0x8000,0x9000};
	UINT32 ChanDACReg = g_ZDDDBInfo.v1721_header[nMdlIdx]->BaseAddr+0x1098;
	UINT32 chan=0;
	
	for ( chan=0; chan<8; chan++ ) 
	{
		UINT32 mask = (separate? DACvalues[chan] : 0xF800); /* less 0x1000 means +20 counts*/
		error += WriteToVme32(ChanDACReg,VA32,mask);
		printf("in V1721_configDACs, write data 0x%x to register 0x%x\n",mask, ChanDACReg);
		taskDelay(1);
		ChanDACReg += 0x0100;
	}

  /* wait until DACs become stable*/
  
	do 
	{
		UINT32 ChanStatusReg = g_ZDDDBInfo.v1721_header[nMdlIdx]->BaseAddr+0x1088;
		UINT32 chan=0;
		for ( chan=0; chan<8; chan++ ) 
		{
			UINT32 status=0xFFFF;
			UINT32 thisDACbusy  = 0;
			error += ReadFrmVme32(ChanStatusReg,VA32,&status);
			thisDACbusy = status & 4; /* Bit 2=1, DAC still busy*/
			busy = busy || thisDACbusy;
			ChanStatusReg += 0x0100;
	    	}
		cnt++;
		taskDelay(1);
		if(cnt>=180)
		{
			printf("V1721_configDACs error \n");
			Push2ErrStack(PPCFATAL+ZDD_FLG+ATOM_ERR+69,0,0,0,0);		/*zdd error 69*/
			return 1;
		}
		
  	} while (busy);

	return(error);
}

int V1721_configTrig(int nMdlIdx, UINT32 trigMask, UINT32 locTrgCoinLvl) 
{
	UINT32 error = 0;
	UINT32 ThreshTimes[8] = {0};
	UINT32 OverUnderReg = g_ZDDDBInfo.v1721_header[nMdlIdx]->BaseAddr+0x1084;
	UINT32 TrigMaskReg =0;
	UINT32 theMask = 0;
	UINT32 OutMaskReg  =0;
	UINT32 outMask = 0;  
	/* download channel threshold (for a negative signal)*/
	UINT32 chanThreshold[8] = {230,230,230,230,230,230,230,230};
	UINT32 ChanThreshReg = g_ZDDDBInfo.v1721_header[nMdlIdx]->BaseAddr+0x1080;
	UINT32* chTh = chanThreshold;
	UINT32 chan=0;
	
	for ( chan=0; chan<8; chan++ ) 
	{
		UINT32 chThre = g_ZDDDBInfo.v1721_header[nMdlIdx]->baseLine[chan] - 4;
		error += WriteToVme32(ChanThreshReg,VA32,chThre);
		chTh++;
		taskDelay(1);
		ChanThreshReg += 0x0100;
	}

  /* configure times under threshold*/

	chTh = ThreshTimes;
	for ( chan=0; chan<8; chan++ ) 
	{
		error += WriteToVme32(OverUnderReg,VA32,*chTh);
		taskDelay(1);
		chTh++;
		OverUnderReg += 0x0100;
	}

	/* configure Trigger Source Enable mask*/
	TrigMaskReg = g_ZDDDBInfo.v1721_header[nMdlIdx]->BaseAddr+0x810C;
	theMask = trigMask + (locTrgCoinLvl << 24);
	error += WriteToVme32(TrigMaskReg,VA32,theMask);

	/* configure Front Panel Trigger Out Enable mask*/
	OutMaskReg  = g_ZDDDBInfo.v1721_header[nMdlIdx]->BaseAddr+0x8110;
	outMask = trigMask;
	error += WriteToVme32(OutMaskReg,VA32,outMask);

	return(error);
}

int V1721_configVMEControl(int nMdlIdx) 
{
	UINT32 error = 0;
	UINT32 VMECtrl=0;
	UINT32 VMECtrlReg = g_ZDDDBInfo.v1721_header[nMdlIdx]->BaseAddr+0xEF00;
	
  /* for CBLT, bus error only enabled in last module*/

  	if (g_ZDDDBInfo.v1721_sum == 2)
  	{
		if(nMdlIdx==1)//last module enable bus error
		{
			VMECtrl = (bmENABLE + Align64ENABLE);/* last module, ALIGN64 enabled*/
			
		}
		else//first module
		{
			VMECtrl = (imROAKMODE+ZDD_INT_LEVEL);//first module generate interrupt
		
		}
  	}
	else if (g_ZDDDBInfo.v1721_sum == 1)
		VMECtrl = (imROAKMODE+ZDD_INT_LEVEL+bmENABLE + Align64ENABLE );//ALIGN64 enabled

	error = WriteToVme32(VMECtrlReg,VA32,VMECtrl);

	return(error);
}

int V1721_configNEvents(int nMdlIdx)
{
	UINT32 error = 0;
	UINT32 NEventsReg = g_ZDDDBInfo.v1721_header[nMdlIdx]->BaseAddr+0xEF1C;
 	error = WriteToVme32(NEventsReg,VA32, g_nTrgPerInt);
	return (error);
}

int V1721_configpostTrigger(int nMdlIdx)
{
	UINT32 error = 0;
	UINT32 data = 50;
	UINT32 NPostTrgReg = g_ZDDDBInfo.v1721_header[nMdlIdx]->BaseAddr+0x8114;
 	error = WriteToVme32(NPostTrgReg,VA32, data);
	printf("V1721_configpostTrigger : write 0x%x to addr 0x%x\n",data,NPostTrgReg);
	return (error);
}

int V1721_configFPControlforCHK(int nMdlIdx)
{
	UINT32 reg =  g_ZDDDBInfo.v1721_header[nMdlIdx]->BaseAddr+0x811c;
	UINT32 status=0x0;
	ReadFrmVme32(reg,VA32,&status);
	UINT32 data = ((status&0xffff3d03)|0x80);
	WriteToVme32(reg,VA32, data);
	printf("V1721_configFPControlforCHK : write 0x%x to addr 0x%x\n",data,reg);
	return 0;
}

int V1721_configFullBuffer(int nMdlIdx)
{
	UINT32 error = 0;
	UINT32 reg = g_ZDDDBInfo.v1721_header[nMdlIdx]->BaseAddr+0x816C;
 	error = WriteToVme32(reg,VA32, 250);
	return (error);

}

int V1721_configBoardID(int nMdlIdx)//set geo in data content
{
	UINT32 error = 0;
	UINT32 BoardIDReg = g_ZDDDBInfo.v1721_header[nMdlIdx]->BaseAddr+0xef08;
	UINT32 data = g_ZDDDBInfo.v1721_header[nMdlIdx]->Position;
	error = WriteToVme32(BoardIDReg, VA32, data);
	return (error);
}

int V1721_configInterruptID(int nMdlIdx)
{
	UINT32 error = 0;
	UINT32 InterruptIDReg = g_ZDDDBInfo.v1721_header[nMdlIdx]->BaseAddr+0xef14;
	error = WriteToVme32(InterruptIDReg, VA32, 0x77777777);
	return (error);	

}

int V1721_configInterruptEventNum(int nMdlIdx)
{
	UINT32 error = 0;
	UINT32 InterruptENumReg = g_ZDDDBInfo.v1721_header[nMdlIdx]->BaseAddr+0xEF18;
	error = WriteToVme32(InterruptENumReg, VA32, INTERRUPTEVENTNUM);
	return (error);	

}

int V1721_configCBLT(int nMdlIdx)
{
	UINT32 error = 0;
	UINT32 CBLTReg = g_ZDDDBInfo.v1721_header[nMdlIdx]->BaseAddr+0xEF0C;
	int position;

	if(!V1190ENB)
	{
		if (g_ZDDDBInfo.v1721_sum == 2)
		{
			if (nMdlIdx==0)
				position = mpFIRSTBOARD;
			else
				position = mpLASTBOARD;
		}
		else
			position = mpFIRSTBOARD; //??
	}
	else//TDC first
	{
		if (g_ZDDDBInfo.v1721_sum == 2)
		{
			if (nMdlIdx==0)
				position = mpMIDDLEBOARD;
			else
				position = mpLASTBOARD;
		}
		else
			position = mpLASTBOARD; 

	}
	
	int data = (position<<8) + CBLTMSB;
 	error = WriteToVme32(CBLTReg,VA32, data);
	return (error);	
}

int V1721_config(int nMdlIdx, UINT32 trigMask, UINT32 locTrgCoinLvl) {

	int  error = 0;
	printf("in V1721_config nMdlIdx = %d\n",nMdlIdx);
	
	printf("g_ZDDDBInfo.v1721_header[%d]->BaseAddr = 0x%x\n",nMdlIdx, g_ZDDDBInfo.v1721_header[nMdlIdx]->BaseAddr);

	
	error += V1721_resetAndChk(nMdlIdx);
	taskDelay(6);
	
	error += V1721_configDAQControl(nMdlIdx);
	taskDelay(6);

	error += V1721_configChanConf(nMdlIdx);
	taskDelay(6);


	if ( g_ZDDDBInfo.v1721_header[nMdlIdx]->compressData == dmZEROLENGTH) 
		error += V1721_configZS( nMdlIdx);
	taskDelay(6);

	error += V1721_configMemory(nMdlIdx);
	taskDelay(6);


	if ( g_ZDDDBInfo.v1721_header[nMdlIdx]->initDACs )
	{
    		int separate = 0;
    		error += V1721_configDACs(nMdlIdx, separate);

		taskDelay(3); /* give DACs some time to stabilize*/
	}

	error += V1721_configTrig(nMdlIdx, trigMask, locTrgCoinLvl);
	taskDelay(6);

	error += V1721_configVMEControl(nMdlIdx);

	taskDelay(6); /* 50 ms of delay or there will be big problem*/

	error += V1721_configNEvents(nMdlIdx);
	taskDelay(6);


	error += V1721_configpostTrigger(nMdlIdx);
	taskDelay(6);

	
	error += V1721_configFPControlforCHK(nMdlIdx);
	taskDelay(6);

	error += V1721_configFullBuffer(nMdlIdx);
	taskDelay(6);
	
	error += V1721_configCBLT(nMdlIdx);
	taskDelay(6);

	error += V1721_configBoardID(nMdlIdx);
	taskDelay(6);

	if(nMdlIdx==0)/*first module generate interrupt*/
	{
		error +=V1721_configInterruptID(nMdlIdx);
		taskDelay(6);

		error +=V1721_configInterruptEventNum(nMdlIdx);
		taskDelay(6);
	}

	return(error);
}

int  V1721_startRun(int nMdlIdx) 
{
	UINT32 DAQctrlReg = g_ZDDDBInfo.v1721_header[nMdlIdx]->BaseAddr+0x8100;
	UINT32 DAQctrl = 0;

	int error = ReadFrmVme32(DAQctrlReg,VA32,&DAQctrl);

	DAQctrl |= 0x4;

	error = WriteToVme32(DAQctrlReg,VA32,DAQctrl);
	if ( error == 0 )
	{
		printf("DAQ started on FADC at %08X\n",g_ZDDDBInfo.v1721_header[nMdlIdx]->BaseAddr);
	} 
	else 
	{
		printf("Error %d starting DAQ on FADC at %08X\n",error,g_ZDDDBInfo.v1721_header[nMdlIdx]->BaseAddr);
	}

	return(error);
}

int V1721_stopRun(int nMdlIdx) 
{
	UINT32 DAQctrlReg = g_ZDDDBInfo.v1721_header[nMdlIdx]->BaseAddr+0x8100;
	UINT32 DAQctrl = 0;

	int error = ReadFrmVme32(DAQctrlReg,VA32,&DAQctrl);

	DAQctrl &= ~0x4;

	error = WriteToVme32(DAQctrlReg,VA32,DAQctrl);
	if ( error == 0 ) 
	{
		printf("DAQ stopped on FADC at %08X\n",g_ZDDDBInfo.v1721_header[nMdlIdx]->BaseAddr);
	} 
	else 
	{
		printf("Error %d stopping DAQ on FADC at %08X\n",error,g_ZDDDBInfo.v1721_header[nMdlIdx]->BaseAddr);
	}

	return(error);
}

int V1721_resetAndChk(int nMdlIdx) {

	int error = 0;
	int cnt = 0;
	UINT32 resetReg = g_ZDDDBInfo.v1721_header[nMdlIdx]->BaseAddr+0xEF24;
	UINT32 reset = 0;
	UINT32 DAQstatusReg = g_ZDDDBInfo.v1721_header[nMdlIdx]->BaseAddr+0x8104;
	UINT32 DAQstatus = 0;

	error = WriteToVme32(resetReg, VA32, reset);
	printf("in V1721_resetAndChk nMdlIdx = %d\n",nMdlIdx);
	printf("write 0x%x to addr 0x%x\n",reset,resetReg);

	do
	{
		taskDelay(1);
		error += ReadFrmVme32(DAQstatusReg,VA32,&DAQstatus);
		cnt++;
		if(cnt>=180)
		{
			printf("V1721_resetAndChk error \n");
			Push2ErrStack(PPCFATAL+ZDD_FLG+ATOM_ERR+68,0,0,0,0);		/*zdd error 68*/
			return 1;
		}
		
	}while ( (DAQstatus & 0x00000100 )==0);
	
	return(error);
  
}

int V1721_sendSoftTrig(int nMdlIdx) 
{

	UINT32 DAQtrigReg = g_ZDDDBInfo.v1721_header[nMdlIdx]->BaseAddr+0x8108;
	UINT32 DAQtrig = 1;
	if(-1== WriteToVme32(DAQtrigReg,VA32,DAQtrig))
	{
		printf("Error triggering minical on FADC at %08X, aborting\n",g_ZDDDBInfo.v1721_header[nMdlIdx]->BaseAddr);
		Push2ErrStack(PPCFATAL+ZDD_FLG+ATOM_ERR+61,0,0,0,0);		/*zdd error 61*/
		return 1;
	}

	return 0;
}

int V1721_dataReady(int nMdlIdx, int* isReady) 
{

	int error;
	UINT32 statusReg = g_ZDDDBInfo.v1721_header[nMdlIdx]->BaseAddr+0xEF04;
	UINT32 status;
	
	error = ReadFrmVme32(statusReg,VA32,&status);
	*isReady = status & 1;
	return(error);
}

int V1721_VMEstatus(int nMdlIdx, UINT32* VMEstatus) {

	/* this reading will also reset the Status Register*/
	UINT32 VMEstatusReg = g_ZDDDBInfo.v1721_header[nMdlIdx]->BaseAddr+0xEF04;

	int error = ReadFrmVme32(VMEstatusReg,VA32,VMEstatus);

	return(error);
}

int V1721_evStored(int nMdlIdx, UINT32* evStored) 
{
	UINT32 VMEstatusReg = g_ZDDDBInfo.v1721_header[nMdlIdx]->BaseAddr+0xEF04;
	UINT32 VMEstatus = 0;
	int error = V1721_VMEstatus(nMdlIdx,&VMEstatus);

	if ( VMEstatus & 1 ) /* if data ready*/
	{ 
		UINT32 eventStoredReg = g_ZDDDBInfo.v1721_header[nMdlIdx]->BaseAddr+0x812C;
		error = ReadFrmVme32(eventStoredReg,VA32,evStored);
	} 
	else 
	{
		*evStored = 0;
		error = 0;
	}
	return(error);
}

int event_stored()
{
	UINT32 eventStoredReg0 = 0x21812C;
	UINT32 eventStoredReg1 = 0x10812C;
	UINT32 eventStoredReg2 = 0x881020;
	UINT32 evStored0,evStored1;
	UINT16 evStored2;
	ReadFrmVme32(eventStoredReg0,VA32,&evStored0);
	ReadFrmVme32(eventStoredReg1,VA32,&evStored1);
	ReadFrmVme16(eventStoredReg2,VA32,&evStored2);
	printf("event stored in module0 is %d , event stored in module1 is %d.\n",evStored0,evStored1);
	printf("event stored in TDC is %d\n",evStored2);
}


int V1721_nextEvSize(int nMdlIdx, UINT32* evSize) 
{

	UINT32 nextEvSize = g_ZDDDBInfo.v1721_header[nMdlIdx]->BaseAddr+0x814C;

	int error = ReadFrmVme32(nextEvSize,VA32,evSize);

	return(error);
}

int ZDDV1721Config()
{
	UINT32 trigMask = ttSOFTTTG | ttEXTTRG; 
	UINT32 locTrgCoinLvl = 2; 

	if (g_ZDDDBInfo.v1721_sum == 1)
	{
		g_ZDDDBInfo.v1721_header[0] = (struct zdd_v1721*) malloc (sizeof(int)*100);

		g_ZDDDBInfo.v1721_header[0]->BaseAddr = 0x100000;
		printf("g_ZDDDBInfo.v1721_header[0]->BaseAddr = 0x%x\n",g_ZDDDBInfo.v1721_header[0]->BaseAddr);

		g_ZDDDBInfo.v1721_header[0]->compressData = dmZEROLENGTH;
		CompressionMode = g_ZDDDBInfo.v1721_header[0]->compressData;

		g_ZDDDBInfo.v1721_header[0]->initDACs =1;

		g_ZDDDBInfo.v1721_header[0]->Position = 6;

		int l=0;
		int bL0[8]={242,238,240,243,236,243,241,241};
		for (l=0; l<8; l++ ) g_ZDDDBInfo.v1721_header[0]->baseLine[l] = bL0[l];
		int th0[8]={160,160,160,160,200,200,200,200};
		for (l=0; l<8; l++ ) g_ZDDDBInfo.v1721_header[0]->threshold[l] = th0[l];
		int nLb0[8]={0x1,0x1,0x1,0x1,0x1,0x1,0x1,0x1};
		for (l=0; l<8; l++ ) g_ZDDDBInfo.v1721_header[0]->nLookBack[l] = nLb0[l];
		int nLf0[8]={0x1,0x1,0x1,0x1,0x1,0x1,0x1,0x1};  
		for (l=0; l<8; l++ ) g_ZDDDBInfo.v1721_header[0]->nLookForward[l] = nLf0[l];
	}
	else if (g_ZDDDBInfo.v1721_sum == 2)
	{
		g_ZDDDBInfo.v1721_header[0] = (struct zdd_v1721*) malloc (sizeof(int)*100);
		g_ZDDDBInfo.v1721_header[1] = (struct zdd_v1721*) malloc (sizeof(int)*100);

		g_ZDDDBInfo.v1721_header[0]->BaseAddr = 0x210000;
		g_ZDDDBInfo.v1721_header[1]->BaseAddr = 0x100000;
		printf("g_ZDDDBInfo.v1721_header[0]->BaseAddr = 0x%x\n",g_ZDDDBInfo.v1721_header[0]->BaseAddr);
		printf("g_ZDDDBInfo.v1721_header[1]->BaseAddr = 0x%x\n",g_ZDDDBInfo.v1721_header[1]->BaseAddr);

		g_ZDDDBInfo.v1721_header[0]->compressData = dmZEROLENGTH;
		g_ZDDDBInfo.v1721_header[1]->compressData = dmZEROLENGTH;
		CompressionMode = g_ZDDDBInfo.v1721_header[0]->compressData;

		g_ZDDDBInfo.v1721_header[0]->initDACs =1;
		g_ZDDDBInfo.v1721_header[1]->initDACs =1;

		g_ZDDDBInfo.v1721_header[0]->Position = 5;
		g_ZDDDBInfo.v1721_header[1]->Position = 6;

		int l=0;
		int bL0[8]={241,237,242,240,241,240,239,238};
		for (l=0; l<8; l++ ) g_ZDDDBInfo.v1721_header[0]->baseLine[l] = bL0[l];
		int th0[8]={160,160,160,160,200,200,200,200};
		for (l=0; l<8; l++ ) g_ZDDDBInfo.v1721_header[0]->threshold[l] = th0[l];
		int nLb0[8]={0x1,0x1,0x1,0x1,0x1,0x1,0x1,0x1};
		for (l=0; l<8; l++ ) g_ZDDDBInfo.v1721_header[0]->nLookBack[l] = nLb0[l];
		int nLf0[8]={0x1,0x1,0x1,0x1,0x1,0x1,0x1,0x1};  
		for (l=0; l<8; l++ ) g_ZDDDBInfo.v1721_header[0]->nLookForward[l] = nLf0[l];
	
		int bL1[8]={242,238,240,243,236,243,241,241};
		for (l=0; l<8; l++ ) g_ZDDDBInfo.v1721_header[1]->baseLine[l] = bL1[l];
		int th1[8]={160,160,160,160,200,200,200,200};
		for (l=0; l<8; l++ ) g_ZDDDBInfo.v1721_header[1]->threshold[l] = th1[l];
		int nLb1[8]={0x1,0x1,0x1,0x1,0x1,0x1,0x1,0x1};
		for (l=0; l<8; l++ ) g_ZDDDBInfo.v1721_header[1]->nLookBack[l] = nLb1[l];
		int nLf1[8]={0x1,0x1,0x1,0x1,0x1,0x1,0x1,0x1};  
		for (l=0; l<8; l++ ) g_ZDDDBInfo.v1721_header[1]->nLookForward[l] = nLf1[l];	
	}
	
	if( V1721_config(0,trigMask,locTrgCoinLvl)!=0)
  	{
    		printf("FADC0 Configuration failed\n");
			Push2ErrStack(PPCFATAL+ZDD_FLG+ATOM_ERR+62,0,0,0,0);		/*zdd error 62*/
    		return -1 ;
	} 
	else
		printf("FADC0 Configuration succ\n");

	if (g_ZDDDBInfo.v1721_sum == 2)
	{
		if( V1721_config(1,trigMask,locTrgCoinLvl)!=0)
	  	{
	    		printf("FADC1 Configuration failed\n");
				Push2ErrStack(PPCFATAL+ZDD_FLG+ATOM_ERR+63,0,0,0,0);		//zdd error 63
	    		return -1 ;
		} 
		else
			printf("FADC1 Configuration succ\n");
	}

}






int ZDDConfigMCCSyn3(int geo,UINT uT0Latency)
{
	printf("enter MDCConfigMCCSyn3\n");
	printf("MCC slot:%d.\n", geo) ;
	printf("MDCMCCReset.\n") ;
	MDCMCCReset(geo) ;
	taskDelay(1);

	printf(" MDCMCConline\n");
	MDCMCCOnline(geo) ;
	
	printf("MDCMCCCrystOn.\n") ;
	MDCMCCCrystOn(geo) ;
	taskDelay(4);
	printf("MDCMCCSetData, MCC T0 latency: %d.\n", uT0Latency) ;
	MDCMCCSetData(geo, uT0Latency) ;
	taskDelay(2);
	return 0;
}



/*****************TDIG configure process here*****************************/

int V1190_readMicro( int nMdlIdx, UINT16* datum) {

  int error = 0;

  UINT32 MicroReadyReg = g_ZDDDBInfo.v1190_header[nMdlIdx]->BaseAddr+0x1030;
  UINT16 ready=0;
  UINT32 MicroHandShakeReg;

  do {
    error += ReadFrmVme16(MicroReadyReg,VA32,&ready);
  } while ( 0 == (0x2 & ready) );

  MicroHandShakeReg = g_ZDDDBInfo.v1190_header[nMdlIdx]->BaseAddr+0x102E;
  error += ReadFrmVme16(MicroHandShakeReg,VA32,datum);

  return(error);
}

int V1190_writeMicro(int nMdlIdx, UINT16 datum) {

  int error = 0;

  UINT32 MicroHandshakeReg = g_ZDDDBInfo.v1190_header[nMdlIdx]->BaseAddr+0x1030;
  UINT16 ready=0;
  UINT32 MicroReg;

  do {
    error += ReadFrmVme16(MicroHandshakeReg,VA32,&ready);
  } while ( 0 == (0x1 & ready) );

  MicroReg = g_ZDDDBInfo.v1190_header[nMdlIdx]->BaseAddr+0x102E;
  error += WriteToVme16(MicroReg,VA32,datum);

  return(error);
}

int V1190_reset(int nMdlIdx) {

  int error = 0;

  UINT32 resetReg = g_ZDDDBInfo.v1190_header[nMdlIdx]->BaseAddr+0x1014;
  UINT16 readyGo=0;
  error += WriteToVme16(resetReg,VA32, readyGo);

  return(error);
}

int V1190_clear(int nMdlIdx) {

  int error = 0;

  UINT32 clearReg = g_ZDDDBInfo.v1190_header[nMdlIdx]->BaseAddr+0x1016;
  UINT16 readyGo=0;
  error += WriteToVme16(clearReg,VA32,readyGo);

  return(error);
}

typedef enum CBLTMode {
        DISABLED = 0x0,
		ACTIVEFIRST = 0x2,
		ACTIVELAST = 0x1,
		ACTIVEMIDDLE = 0x3
} cbltMode;

int V1190_configCBLT(int nMdlIdx){
	int error = 0;

	UINT32 CBLTControlReg = g_ZDDDBInfo.v1190_header[nMdlIdx]->BaseAddr+0x1012;

	UINT16 mode;
	if(V1190ENB)
	{
		mode = ACTIVEFIRST;
		printf("set V1190 module to first module in CBLT chain\n");
	}
	else
	{
		mode = DISABLED;
		printf("set V1190 module disabled in CBLT chain\n");
	}
	error += WriteToVme16(CBLTControlReg,VA32,mode);

  	return(error);

}

typedef enum runMode {
        TRIGMATCHMODE  = 0x0000,
        CONTINUOUSMODE = 0x0100
} runMode;

int V1190_setRunMode( int nMdlIdx) {

  int error = 0;
  UINT16 mode = TRIGMATCHMODE;
  UINT16 command;
  UINT16 moderb;
  
  error += V1190_writeMicro( nMdlIdx, mode);

  command = 0x200; /* read back run mode*/
  error += V1190_writeMicro( nMdlIdx, command);

  
  error += V1190_readMicro( nMdlIdx, &moderb);

  if ( moderb != 1 ) error += 256;

  return(error);
}

typedef enum transitionType {
        PAIRED       = 0x0000,
        ONLYTRAILING = 0x0001,
        ONLYLEADING  = 0x0002,
        BOTH         = 0x0003
} transitionType;

int V1190_setTransitionPairing(int nMdlIdx) {

  int error = 0;
  UINT16 mode;
  UINT16 typerb;
  UINT16 command = 0x2200;
  error += V1190_writeMicro( nMdlIdx, command);

  mode = ONLYLEADING;
  error += V1190_writeMicro( nMdlIdx, mode);

  command = 0x2300;
  error += V1190_writeMicro( nMdlIdx, command);

  
  error += V1190_readMicro( nMdlIdx, &typerb);

  if ( typerb != ONLYLEADING ) error += 256;

  return(error);
}

int V1190_setWindowWidthAndOffset( int nMdlIdx) {

  int error = 0;
  UINT16 command = 0x1000;
  UINT16 wwidth;
  UINT16 woffst;
  
  error += V1190_writeMicro( nMdlIdx, command);

  wwidth = g_ZDDDBInfo.v1190_header[nMdlIdx]->WindowWidth;                /* 280 ticks = 7 us*/
  error += V1190_writeMicro( nMdlIdx, wwidth);

  command = 0x1100;
  error += V1190_writeMicro( nMdlIdx, command);

  woffst = g_ZDDDBInfo.v1190_header[nMdlIdx]->WindowOffset;               /* -260 ticks = -6.5 us*/
  error += V1190_writeMicro( nMdlIdx, woffst);

  return(error);
}

typedef enum resolution {
        _800PSPERBIT = 0x0000,
        _200PSPERBIT = 0x0001,
        _100PSPERBIT = 0x0002
} resolution;

int V1190_setResolution( int nMdlIdx) {

  int error = 0;
  UINT16 command = 0x2400;
  UINT16 tres;
  UINT16 tresrb;
  
  error += V1190_writeMicro( nMdlIdx, command);

  tres = _100PSPERBIT;
  error += V1190_writeMicro( nMdlIdx, tres);

  command = 0x2600;
  error += V1190_writeMicro( nMdlIdx, command);

  
  error += V1190_readMicro( nMdlIdx, &tresrb);

  if ( tresrb != _100PSPERBIT ) error += 256;

  return(error);
}

int V1190_cleanBufferOn( int nMdlIdx) {

  int error = 0;
  UINT16 command = 0x3100;
  error += V1190_writeMicro( nMdlIdx, command);

  return(error);
}

/*enable TDC header & trailer */

int V1190_cleanBufferOff( int nMdlIdx) {

  int error = 0;
  UINT16 command = 0x3000;
  error += V1190_writeMicro( nMdlIdx, command);

  return(error);
}

int V1190_setMaxHits(int nMdlIdx) {

  int error = 0;
  UINT16 nHits;
  UINT16 typerb;
  
  UINT16 command = 0x3300;
  error += V1190_writeMicro( nMdlIdx, command);

  nHits = 8;
  error += V1190_writeMicro( nMdlIdx, nHits);

  command = 0x3400;
  error += V1190_writeMicro( nMdlIdx, command);

  
  error += V1190_readMicro( nMdlIdx, &typerb);

  if ( typerb != nHits ) error += 256;

  return(error);
}


int V1190_trigConfRBK( int nMdlIdx, UINT16* readback) {

  int error = 0;
  int loop;
  UINT16 command = 0x1600;
  error += V1190_writeMicro( nMdlIdx, command);

  
  for ( loop=0; loop<5; loop++ ) {
    error += V1190_readMicro( nMdlIdx, readback++);  
  }

  return(error);
}

typedef enum ctrlRegBits {
        BERREN  = 0x0001,
        EMPTY   = 0x0008,
        XTTT    = 0x0200
} ctrlRegBits;

int V1190_setControlReg( int nMdlIdx) {

  int error = 0;

  UINT32 ctrlReg = g_ZDDDBInfo.v1190_header[nMdlIdx]->BaseAddr+0x1000;
  UINT16 ctrl;
  error += ReadFrmVme16(ctrlReg,VA32,&ctrl);

  ctrl = ctrl | BERREN | EMPTY | XTTT;
  error += WriteToVme16(ctrlReg,VA32,ctrl);

  return(error);
}

UINT V1190_setBLTEventNum(int nMdlIdx)
{
	int error = 0;
	error += WriteToVme16(g_ZDDDBInfo.v1190_header[nMdlIdx]->BaseAddr+0x1024,VA32,0x1);
	return(error);
}

UINT V1190_setGeo(int nMdlIdx)
{
	int error = 0;
	error += WriteToVme16(g_ZDDDBInfo.v1190_header[nMdlIdx]->BaseAddr+0x100E,VA32,0x14);
	return(error);
}


int V1190_fastInit(int nMdlIdx) {

  int error = 0;
  UINT16 command = 0x1400;
  UINT16 readback[5] ;
	int i;
	
  error += V1190_reset(nMdlIdx);

  error += V1190_setRunMode(nMdlIdx);

  error += V1190_setTransitionPairing(nMdlIdx);

  error += V1190_setBLTEventNum(nMdlIdx);

  /* Enable Trigger Time Subtraction*/
  
  error += V1190_writeMicro( nMdlIdx, command);

  error += V1190_setWindowWidthAndOffset(nMdlIdx);

  error += V1190_setResolution(nMdlIdx);

  /* Prune headers/trailers from data out*/
  error += V1190_cleanBufferOn(nMdlIdx);

  error += V1190_setMaxHits(nMdlIdx);

  /* Read back Trigger Configuration*/
  
  error += V1190_trigConfRBK(nMdlIdx,readback);
 /* readback should be equal to: {Trig.Win.Width,Trig.Win.Off,8,4,1} 1 = trig time subtraction enabled*/

 for(i=0;i<5;i++)
 {
	printf("readback [%d] : 0x%x \n",i,readback[i]);
 }

  error += V1190_setControlReg(nMdlIdx);

  error += V1190_configCBLT(nMdlIdx);

  error += V1190_setGeo(nMdlIdx); 

  error += V1190_clear(nMdlIdx);

  return(error);
}

int V1190_dataReady( int nMdlIdx, int* isReady) {

  int error;
  UINT32 statusReg = g_ZDDDBInfo.v1190_header[nMdlIdx]->BaseAddr+0x1002;
  UINT16 status;
  error = ReadFrmVme16(statusReg,VA32, &status);
  *isReady = status & 1;
  return(error);
}

int V1190_startRun( int nMdlIdx) {

  int error;

  UINT32 ctrlReg = g_ZDDDBInfo.v1190_header[nMdlIdx]->BaseAddr+0x1000;
  UINT16 ctrl;
  error = ReadFrmVme16(ctrlReg,VA32, &ctrl);

  ctrl = ctrl & 0xFFBF; /* turn off bit 6, TEST FIFO ENABLE, TDC's are enabled*/
   
  error += WriteToVme16(ctrlReg,VA32,ctrl);

  return(error);
}

int V1190_stopRun( int nMdlIdx) {

  int error;

  UINT32 ctrlReg = g_ZDDDBInfo.v1190_header[nMdlIdx]->BaseAddr+0x1000;
  UINT16 ctrl;
  error = ReadFrmVme16(ctrlReg,VA32, &ctrl);

  ctrl = ctrl | 0x0040; /* turn on bit 6, TEST FIFO ENABLE, TDC's are disabled*/

  error += WriteToVme16(ctrlReg,VA32,ctrl);

  return(error);
}

int V1190_sendTrig(int nMdlIdx) {

  int error;

  UINT32 softTrigReg = g_ZDDDBInfo.v1190_header[nMdlIdx]->BaseAddr+0x101A;
  UINT16 trigPlease=1;
  error = WriteToVme16(softTrigReg,VA32,trigPlease);

  return(error);
}

int ZDDV1190Config()
{
	UINT error = 0;
	
	g_ZDDDBInfo.v1190_header[0]->BaseAddr  = 0x880000;
	g_ZDDDBInfo.v1190_header[0]->WindowOffset = 0xFFD8;
	g_ZDDDBInfo.v1190_header[0]->WindowWidth = 0x14;
	
	error = V1190_fastInit(0);

	printf("error = %d \n",error);

	return;

}


/*****************************/

int ZDDStartTrig()
{	

	//start trig		
	if (-1==V1190_startRun(0))
	{
		printf("TDC0 start run failed\n");
		Push2ErrStack(PPCFATAL+ZDD_FLG+ATOM_ERR+64,0,0,0,0);		/*zdd error 64*/
		return -1;
	}



	//start trig		
	if (-1==V1721_startRun(0))
	{
		printf("FADC0 start run failed\n");
		Push2ErrStack(PPCFATAL+ZDD_FLG+ATOM_ERR+64,0,0,0,0);		/*zdd error 64*/
		return -1;
	}
	
	if (g_ZDDDBInfo.v1721_sum == 2)	
	{
		if (-1==V1721_startRun(1))
		{
			printf("FADC1 start run failed\n");
			Push2ErrStack(PPCFATAL+ZDD_FLG+ATOM_ERR+65,0,0,0,0);		//zdd error 65
			return -1;
		}
	}
	
}

int ZDDStopTrig()
{	
	semTake(g_sidZddSpecial,WAIT_FOREVER);
	
	if (-1==V1721_stopRun(0))
	{
		printf("FADC0 stop run failed\n");
		Push2ErrStack(PPCFATAL+ZDD_FLG+ATOM_ERR+66,0,0,0,0);		/*zdd error 66*/
		return -1;
	}  

	if (g_ZDDDBInfo.v1721_sum == 2)
	{
		if (-1==V1721_stopRun(1))
		{
			printf("FADC1 stop run failed\n");
			Push2ErrStack(PPCFATAL+ZDD_FLG+ATOM_ERR+67,0,0,0,0);		//zdd error 67
			return -1;
		}
	}

	
	if (-1==V1190_stopRun(0))
	{
		printf("TDC0 stop run failed\n");
		Push2ErrStack(PPCFATAL+ZDD_FLG+ATOM_ERR+64,0,0,0,0);		/*zdd error 64*/
		return -1;
	}	

	semGive(g_sidZddSpecial);
	taskDelay(30);

}

