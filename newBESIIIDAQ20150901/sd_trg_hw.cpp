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

#include "sd_trg.h"
#include "sd_trg_hw.h"

bool g_bTRGRocIntConed=FALSE;
	
int Timedelay(UINT32 timenum)
{
	UINT32 icount;
	icount=0;

	while (icount < timenum)
	{
		icount++;
	}
}

void TRGIntHandler(int nArg)
{
	//logMsg("interrupt receive OK,%d\n", nArg, 0, 0, 0, 0, 0);
	sysIntDisable(TRG_INT_LEVEL) ;
	semGive(g_sidInterrupt);
	g_nInterruptCounter+=g_nTrgPerInt;
}
int   TRGConfInterrupt()
{
	if(FALSE == g_bTRGRocIntConed) 
	{

		if(intConnect (INUM_TO_IVEC(TRG_INT_VEC), (VOIDFUNCPTR)TRGIntHandler, TRG_INT_VEC) == ERROR)
	  	{
	     		//int handler connect error\0");
	     		Push2ErrStack(PPCFATAL+TRG_FLG+ATOM_ERR+63,0,0,0,0);		/*trg error 63*/
			return  -1;
	  	}
		g_bTRGRocIntConed= TRUE ;
	}
	sysIntDisable(TRG_INT_LEVEL);	
	return 0;
}

int TRGInitCBLT() 
{
	if(CBLTinit(32)==-1)
	{
		printf("CBLT init error!\n");
		Push2ErrStack(PPCFATAL+TRG_FLG+ATOM_ERR+64,0,0,0,0);		/*trg error 64*/
		return -1;
	}
	return 0;
}

/*==========================================================*/
int WrtRdA24D8(unsigned uAddr,unsigned uData,char * abc,unsigned char uflag)
{
	unsigned char uReadTmp=0;
	WriteToVme8(uAddr, VA24, uData);
	ReadFrmVme8(uAddr, VA24, &uReadTmp);
//	printf("uReadTmp=%x,uData=%x\n ",uReadTmp,uData);
	if( (uReadTmp&uflag) != (uData&uflag) )
	{
		printf("WrtRdA24D8 : conf %s error\n",abc);
		Push2ErrStack(PPCFATAL+TRG_FLG+ATOM_ERR+65,0,0,0,0);		/*trg error 65*/
		return -1;
	}
	return 0;
}

int WrtRdA24D16(unsigned uAddr,unsigned uData,char * abc,unsigned short uflag)
{
	unsigned short uReadTmp=0;
	WriteToVme16(uAddr, VA24, uData);
	ReadFrmVme16(uAddr, VA24, &uReadTmp);
//	printf("uReadTmp=%x,uData=%x\n ",uReadTmp,uData);
	if( (uReadTmp&uflag) != (uData&uflag) )
	{
		printf("WrtRdA24D16 : conf %s error\n",abc);
		Push2ErrStack(PPCFATAL+TRG_FLG+ATOM_ERR+66,0,0,0,0);		/*trg error 66*/
		return -1;
	}
	return 0;
}
int WrtRdA24D32(unsigned uAddr,unsigned uData,char * abc,unsigned uflag)
{
	unsigned uReadTmp=0;
	WriteToVme32(uAddr, VA24, uData);
	ReadFrmVme32(uAddr, VA24, &uReadTmp);
//	printf("uReadTmp=%x,uData=%x\n ",uReadTmp,uData);
	if( (uReadTmp&uflag) != (uData&uflag) )
	{
		printf("WrtRdA24D32 : conf %s error\n",abc);
		Push2ErrStack(PPCFATAL+TRG_FLG+ATOM_ERR+67,0,0,0,0);		/*trg error 67*/
		return -1;
	}
	return 0;
}
int WrtRdA32D32(unsigned uAddr,unsigned uData,char * abc,unsigned uflag)
{
	unsigned uReadTmp=0;
	WriteToVme32(uAddr, VA32, uData);
	ReadFrmVme32(uAddr, VA32, &uReadTmp);
	//printf("uReadTmp=%x,uData=%x\n ",uReadTmp,uData);
	if( (uReadTmp&uflag) != (uData&uflag) )
	{
		printf("WrtRdA32D32 : conf %s error\n",abc);
		printf("uReadTmp=%x,uData=%x\n ",uReadTmp,uData);
		Push2ErrStack(PPCFATAL+TRG_FLG+ATOM_ERR+68,0,0,0,0);		/*trg error 68*/
		return -1;
	}
	return 0;
}


int WrtType1A24D16(unsigned uAddr,unsigned uData,char * abc)
{
	unsigned short uReadTmp=0;
	int	nCnt=0;
	taskDelay(30);	//jixl 080222
/*
	ReadFrmVme16(0x1030+(TrgTDCBSEL<<19), VA24, &uReadTmp);
	while((uReadTmp&0x1)!=0x1)
	{
		nCnt++;
		if(nCnt>10)
		{
			printf("WrtType1A24D16 : read for Write Type1 %s error\n",abc);
			Push2ErrStack(PPCFATAL+TRG_FLG+ATOM_ERR+69,0,0,0,0);		//trg error 69
			return -1;
		}
		taskDelay(30);
		ReadFrmVme16(0x1030+(TrgTDCBSEL<<19), VA24, &uReadTmp);
	}
*/	
	WriteToVme16(uAddr, VA24, uData);
	return 0;
}
int ReadType2A24D16(unsigned uAddr,unsigned short * uVal,char * abc)
{
	unsigned short uReadTmp=0;
	int	nCnt=0;
	taskDelay(30);	//jixl 080222
	
	ReadFrmVme16(0x1030+(TrgTDCBSEL<<19), VA24, &uReadTmp);
	while((uReadTmp&0x2)!=0x2)
	{
		nCnt++;
		if(nCnt>10)
		{
			printf("ReadType2A24D16 : read for read Type2 %s error\n",abc);
			Push2ErrStack(PPCFATAL+TRG_FLG+ATOM_ERR+70,0,0,0,0);		//trg error 70
			return -1;
		}
		taskDelay(30);
		ReadFrmVme16(0x1030+(TrgTDCBSEL<<19), VA24, &uReadTmp);
	}
	
	ReadFrmVme16(uAddr, VA24, uVal);
	return 0;
}

/*________________________________________CLKF__*/

int TRG_CLKF_preconf()
{
	if(g_TRGDBInfo.trg_clkf_header->ClkSource==0)
	{
		
	}
//	WriteToVme16(0xff0000, VA24, 0x0);//start 40M CLK
	//taskDelay(5);
	return 0;
}
int TRG_CLKF_syn2()
{
	if(g_TRGDBInfo.trg_clkf_header->ClkSource==0)
	{
		
	}
//	WriteToVme16(0xff0000, VA24, 0x0);//start 40M CLK
	//or taskDelay(5);
	return 0;
}

/*________________________________________MUTL__*/
int TRG_MUTL_preconf()
{
	//here may used to load fpga,normal no use

	UINT32 write_addr,write_data;
	FILE *rfp;
	char cha[10];
	unsigned int uReadTmp;

	ReadFrmVme32( TrgMUTLBSEL<<19, VA24,&uReadTmp);
	if((uReadTmp&0xffffff)!=0x801091)
	{
		if((rfp=fopen("./trigger/mutl.hex","r"))==NULL)
		{
			printf("Open file mutl.hex Error!\r\n");
			Push2ErrStack(PPCFATAL+TRG_FLG+ATOM_ERR+71,0,0,0,0);		/*trg error 71*/
			return -1;
		}

		write_addr = 0x40000+(TrgMUTLBSEL<<19);
		if(-1==WriteToVme16(write_addr, VA24, 0x00000000))//VA32, write_data
		{
			printf("mutl.hex:Write Reset Error !\n");
			Push2ErrStack(PPCFATAL+TRG_FLG+ATOM_ERR+72,0,0,0,0);		/*trg error 72*/
			return -1;
		}

		Timedelay(20000);

		write_addr  = 0x40000+(TrgMUTLBSEL<<19) + 6;//0x00000006;//0x00580006;

		while(!feof(rfp))
		{
			fgets(cha,5,rfp);
			cha[4]='\0';
			sscanf(cha,"%x",&write_data);
	    		if(-1==WriteToVme16(write_addr, VA24, write_data))//VA32, write_data
			{
				printf("mutl.hex:Write data Error !\n");
				Push2ErrStack(PPCFATAL+TRG_FLG+ATOM_ERR+73,0,0,0,0);		/*trg error 73*/
				return -1;
			}
		}
		
		fclose(rfp);                  
		write_data = 0x1b1b;
		int i;
		for(i=0;i<30;i++)
		{
			if(-1==WriteToVme16(write_addr, VA24, write_data))//VA32, write_data
				{
					printf("mutl.hex:Write End Mark Error !\n");
					Push2ErrStack(PPCFATAL+TRG_FLG+ATOM_ERR+74,0,0,0,0);		/*trg error 74*/
					return -1;
				}
		}
		
		ReadFrmVme32( TrgMUTLBSEL<<19, VA24,&uReadTmp);
		printf("uReadTmp=%x\n ",uReadTmp);
		if (uReadTmp==0xffffffff)
		{
			printf("MUTL preconf error\n");
			Push2ErrStack(PPCFATAL+TRG_FLG+ATOM_ERR+75,0,0,0,0);		/*trg error 75*/
			fclose(rfp);
			return -1;
		}
		
		printf("\r\nEnd the MUTL PreConfiguration!!!\r\n");
		
		fclose(rfp);
		printf("***********************************\n");	
	}
	return 0;
}

int TRG_MUTL_syn3()
{
	WriteToVme32(0xfc+(TrgMUTLBSEL<<19), VA24,0x1);//reset Global
	taskDelay(1);
	WriteToVme32(0xf8+(TrgMUTLBSEL<<19), VA24,0x1);//reset CBLT
	//chain set
	if(-1==WrtRdA24D32(0x4+(TrgMUTLBSEL<<19),g_TRGDBInfo.trg_mutl_header->ChainSet,"MUTL ChainSet",0xffff) )return -1;
	//ReadoutEventNum
	if(-1==WrtRdA24D32(0x8+(TrgMUTLBSEL<<19),g_nTrgPerInt-1,"MUTL g_nTrgPerInt",0xffffffff) )return -1;
	//ReadoutEventWindow
	if(-1==WrtRdA24D32(0xc+(TrgMUTLBSEL<<19),g_TRGDBInfo.trg_mutl_header->ReadoutEventWindow,"MUTL ReadoutEventWindow",0xffffffff) )return -1;
	//ProgramFull
	if(-1==WrtRdA24D32(0x10+(TrgMUTLBSEL<<19),g_TRGDBInfo.trg_mutl_header->ProgramFull,"MUTL ProgramFull",0xffffffff) )return -1;
	//ProgramFullDisable
	if(-1==WrtRdA24D32(0x14+(TrgMUTLBSEL<<19),g_TRGDBInfo.trg_mutl_header->ProgramFullDisable,"MUTL ProgramFullDisable",0xffffffff) )return -1;
	//L1Select
	if(-1==WrtRdA24D32(0x18+(TrgMUTLBSEL<<19),g_TRGDBInfo.trg_mutl_header->L1Select,"MUTL L1Select",0xffffffff) )return -1;
	//ReadoutModeSelect
	if(-1==WrtRdA24D32(0x28+(TrgMUTLBSEL<<19),g_TRGDBInfo.trg_mutl_header->ReadoutModeSelect,"MUTL ReadoutModeSelect",0xffffffff) )return -1;
	//L1LatencyToHit
	if(-1==WrtRdA24D32(0x24+(TrgMUTLBSEL<<19),g_TRGDBInfo.trg_mutl_header->L1LatencyToHit,"MUTL L1LatencyToHit",0xffffffff) )return -1;
	//TSFLatencyToHit
	if(-1==WrtRdA24D32(0x2c+(TrgMUTLBSEL<<19),g_TRGDBInfo.trg_mutl_header->TSFLatencyToHit,"MUTL TSFLatencyToHit",0xffffffff) )return -1;
	//TrackLatencyToHit
	if(-1==WrtRdA24D32(0x30+(TrgMUTLBSEL<<19),g_TRGDBInfo.trg_mutl_header->TrackLatencyToHit,"MUTL TrackLatencyToHit",0xf) )return -1;
	//FiberSyncLatencyToFRST
	if(-1==WrtRdA24D32(0x34+(TrgMUTLBSEL<<19),g_TRGDBInfo.trg_mutl_header->FiberSyncLatencyToFRST,"MUTL FiberSyncLatencyToFRST",0xffffffff) )return -1;
	//FiberChannelSelect
	if(-1==WrtRdA24D32(0x38+(TrgMUTLBSEL<<19),g_TRGDBInfo.trg_mutl_header->FiberChannelSelect,"MUTL FiberChannelSelect",0xffffffff) )return -1;
	//DataSelect
	if(-1==WrtRdA24D32(0x108+(TrgMUTLBSEL<<19),g_TRGDBInfo.trg_mutl_header->DataSelect,"MUTL DataSelect",0xffffffff) )return -1;

	unsigned int uReadTmp;	
	ReadFrmVme32( 0x4+(TrgMUTLBSEL<<19), VA24,&uReadTmp);
	printf("MUTL ChainSet: %x\n ",uReadTmp);	

	ReadFrmVme32( 0x18+(TrgMUTLBSEL<<19), VA24,&uReadTmp);
	printf("MUTL L1Select: %x\n ",uReadTmp);	

	ReadFrmVme32( 0x38+(TrgMUTLBSEL<<19), VA24,&uReadTmp);
	printf("MUTL FiberChannelSelect: %x\n ",uReadTmp);	

	return 0;
}


int TRG_MUTL_prep()
{
	WriteToVme32(0xf8+(TrgMUTLBSEL<<19), VA24,0x1);//reset CBLT
	WriteToVme32(0xf8+(TrgMUTLBSEL<<19), VA24,0x1);//reset CBLT

	return 0;

}

	
/*________________________________________TFCF__*/

int TRG_TFCF_syn2()
{
//	printf("write addr =0x%x\n",(TrgTFCFBSEL<<19));
	if(-1==WrtRdA24D16((TrgTFCFBSEL<<19),g_TRGDBInfo.trg_tfcf_header->CSR,"TFCF CSR",0xffff) ) return -1;
	if(-1==WrtRdA24D16(0x6+(TrgTFCFBSEL<<19),g_TRGDBInfo.trg_tfcf_header->V_Err_T,"TFCF V_Err_T",0xffff) ) return -1;	
	if(-1==WrtRdA24D16(0x8+(TrgTFCFBSEL<<19),g_TRGDBInfo.trg_tfcf_header->V_Err_C,"TFCF V_Err_C",0xff) ) return -1;//low 8 bit
	if(-1==WrtRdA24D16(0x5c+(TrgTFCFBSEL<<19),g_TRGDBInfo.trg_tfcf_header->Per_ERR_12,"TFCF Per_ERR_12",0xffff) ) return -1;
	if(-1==WrtRdA24D16(0x1e+(TrgTFCFBSEL<<19),g_nTrgPerInt,"TFCF g_nTrgPerInt",0xff) ) return -1;
	return 0;
}

int TRG_TFCF_syn1()
{
	if(-1==WrtRdA24D16((TrgTFCFBSEL<<19),g_TRGDBInfo.trg_tfcf_header->CSR,"TFCF CSR",0xffff) ) return -1;//CSR
	if(-1==WrtRdA24D16(0x2+(TrgTFCFBSEL<<19),g_TRGDBInfo.trg_tfcf_header->IO_PR,"TFCF IO_PR",0xffff) ) return -1;//IOPR
	if(-1==WrtRdA24D16(0x4+(TrgTFCFBSEL<<19),g_TRGDBInfo.trg_tfcf_header->ENBL,"TFCF ENBL",0xff) ) return -1;//WIER

	return 0;
}

int TRG_TFCF_prep()
{
	WriteToVme16(0xc+(TrgTFCFBSEL<<19), VA24, 0x0);//reset command
	WriteToVme16(0xe+(TrgTFCFBSEL<<19), VA24, 0x3);//clean err count
	WriteToVme16(0x1c+(TrgTFCFBSEL<<19), VA24, g_TRGDBInfo.trg_tfcf_header->CO_IR);//open or not open err interrupt
	WriteToVme16(0xc+(TrgTFCFBSEL<<19), VA24, 0x0);//reset command
	if(-1==WrtRdA24D16(0x2+(TrgTFCFBSEL<<19),g_TRGDBInfo.trg_tfcf_header->IO_PR,"TFCF IO_PR",0xffff) ) return -1;
	if(-1==WrtRdA24D16(0x4+(TrgTFCFBSEL<<19),g_TRGDBInfo.trg_tfcf_header->ENBL,"TFCF ENBL",0xff) ) return -1;
	return 0;
}

/*________________________________________FCTL__*/

int TRG_FCTL_syn2()
{
	unsigned short uChanlInitMask=0;
	unsigned short uChanlStat=0;
	unsigned short uReadTmp=0;

//	g_TRGDBInfo.trg_fctl_header->SubSystemInfo&=~0x10;//leigk

	for(int idx=0;idx<6;idx++)
	{
		if(g_TRGDBInfo.trg_fctl_header->SubSystemInfo&(1<<idx) )
		{
			uChanlInitMask+=3<<(idx*2);
			uChanlStat+=1<<(idx*2);	
		}
	}

	printf("FCTL g_TRGDBInfo.trg_fctl_header->SubSystemInfo=0x%x\n",g_TRGDBInfo.trg_fctl_header->SubSystemInfo);
	printf("FCTL uChanlInitMask=0x%x,uChanlStat=0x%x\n",uChanlInitMask,uChanlStat);
	
	WriteToVme16(0x26+(TrgFCTLBSEL<<19), VA24, 0x1);//reset clk
	taskDelay(1);
	WriteToVme16((TrgFCTLBSEL<<19), VA24, g_TRGDBInfo.trg_fctl_header->L1_config);// config L1 mode
	
	WriteToVme16(0xe+(TrgFCTLBSEL<<19), VA24, 0x0);//fiber init ,syn,low six bit effect
	taskDelay(1);
	ReadFrmVme16(0x10+(TrgFCTLBSEL<<19), VA24, &uReadTmp);//read for fiber init
	printf("uReadTmp=%x\n ",uReadTmp);

	if((uReadTmp&uChanlInitMask)!=0x0)//set channel init
	{
		printf("FCTL set channel fiber init fail\n");
		Push2ErrStack(PPCFATAL+TRG_FLG+ATOM_ERR+76,0,0,0,0);		/*trg error 76*/
		return -1;
	}
	// WriteToVme16(0xe+(TrgFCTLBSEL<<19), VA24, g_TRGDBInfo.trg_fctl_header->SubSystemInfo&0x3f);//fiber init ,transmit
	// kejun, for ZDD test, Dec. 3, 2011
	WriteToVme16(0xe+(TrgFCTLBSEL<<19), VA24, 0x3f);//fiber init ,transmit
	taskDelay(1);
	ReadFrmVme16(0x10+(TrgFCTLBSEL<<19), VA24, &uReadTmp);//read for fiber transmit
	printf("uReadTmp=%x\n ",uReadTmp);
	
	if((uReadTmp&uChanlInitMask)!=uChanlStat)//set channel tansit
	{
		printf("FCTL set channel fiber tansit init fail\n");
		Push2ErrStack(PPCFATAL+TRG_FLG+ATOM_ERR+77,0,0,0,0);		/*trg error 77*/
		return -1;
	}

	WriteToVme16(0xa+(TrgFCTLBSEL<<19), VA24, g_TRGDBInfo.trg_fctl_header->Input_en1);//fiber 
	WriteToVme16(0xc+(TrgFCTLBSEL<<19), VA24, g_TRGDBInfo.trg_fctl_header->Input_en2);//fiber 
	WriteToVme16(0x18+(TrgFCTLBSEL<<19), VA24, g_TRGDBInfo.trg_fctl_header->IntCount);//fiber 
	WriteToVme16(0x1a+(TrgFCTLBSEL<<19), VA24, g_TRGDBInfo.trg_fctl_header->IntTiming);//fiber 
	WriteToVme16(0x12+(TrgFCTLBSEL<<19), VA24, g_TRGDBInfo.trg_fctl_header->IntEnable);//fiber
	taskDelay(1);
	
	return 0;
}

int TRG_FCTL_syn4()
{
	WriteToVme16(0x34+(TrgFCTLBSEL<<19), VA24, g_TRGDBInfo.trg_fctl_header->SendFRST);//fiber
	printf("TRG_FCTL_syn4 : write 0x%x to 0x%x \n",g_TRGDBInfo.trg_fctl_header->SendFRST,0x34+(TrgFCTLBSEL<<19));
	taskDelay(1);

	WriteToVme16(0x8+(TrgFCTLBSEL<<19), VA24, 0xffff);//reset
	taskDelay(1);	
	//trg_status();
}

int TRG_FCTL_test()
{
	while(1)
		{

		WriteToVme16(0x00c80034, VA24, 0x0000007f);//fiber
		printf("write 0x%x to 0x%x \n",0x0000007f,0x00c80034);
			taskDelay(60);

		}
}

void trg_status()
{
	/*step 1 : mask all status bit*/
	WriteToVme16(0xa+(TrgFCTLBSEL<<19), VA24, 0);//Input_en1
	WriteToVme16(0xc+(TrgFCTLBSEL<<19), VA24, 0);//Input_en2
	taskDelay(1);

	/*step 2: clear status register*/
	WriteToVme16(0x8+(TrgFCTLBSEL<<19), VA24, 0xffff);
	taskDelay(1);
	
	/*step 3: unmask status bit*/
	WriteToVme16(0xa+(TrgFCTLBSEL<<19), VA24, g_TRGDBInfo.trg_fctl_header->Input_en1);//Input_en1 
	WriteToVme16(0xc+(TrgFCTLBSEL<<19), VA24, g_TRGDBInfo.trg_fctl_header->Input_en2);//Input_en2
	taskDelay(1);

	/*step 4: read status*/
	unsigned short uReadTmp=0;
	ReadFrmVme16(0x8+(TrgFCTLBSEL<<19), VA24, &uReadTmp);//read for status check
	printf("trg_status = %x (TRG|TOF2|TOF1|MU|EMC|MDC|REV)\n ",uReadTmp);
}


/*______________________________________TROC__*/

int TRG_TROC_prep()	//interrupt level 5, vector 248
{
	unsigned short uReadTmp;
	WriteToVme16(0xfff0+(TrgTROCBSEL<<19), VA24, (g_nTrgPerInt<<8)+0x80);// if 0 norm L1
	printf("TROC write 0x%x to 0x%x \n",(g_nTrgPerInt<<8)+0x80, 0xfff0+(TrgTROCBSEL<<19));
	taskDelay(1);
	ReadFrmVme16(0xfff6+(TrgTROCBSEL<<19), VA24, &uReadTmp);//clear L1 count
	ReadFrmVme16(0xfff8+(TrgTROCBSEL<<19), VA24, &uReadTmp);//clear Interupt count

	return 0;
}

/*______________________________________GTL__*/
int TRG_GTL_preconf()
{
	WriteToVme32(0x6108+(TrgGTLBSEL<<19), VA24, 0x0);//GTL stop L1
	return 0;
}



int TRG_GTL_conf()
{
	unsigned int uWriteAddr;

	unsigned int uDmaWillLen=0;
	unsigned int uDmaTransLen=0;
	unsigned int *pAddrDmaWR;
	
	WriteToVme32(0x18000+(TrgGTLBSEL<<19), VA24, 0x0);//reset DCM of GTL

	uWriteAddr=(TrgGTLBSEL<<19);
	for(int ibx=0;ibx<48;ibx++)//channel delay
	{
		if(-1==WrtRdA24D32(uWriteAddr,g_TRGDBInfo.trg_gtl_header->DelayOfChannel[ibx],"GTL Chnl Delay",0x3ff) ) return -1;
		uWriteAddr+=4;
	}
	if(-1==WrtRdA24D32(0x60c4 +(TrgGTLBSEL<<19),g_TRGDBInfo.trg_gtl_header->InternalL1Delay,"GTL InternalL1Delay",0x3ff) ) return -1;
	if(-1==WrtRdA24D32(0x60c8 +(TrgGTLBSEL<<19),g_TRGDBInfo.trg_gtl_header->UserOwnCSR,"GTL UserOwnCSR",0xffffffff) ) return -1;
	if(-1==WrtRdA24D32(0x60cc +(TrgGTLBSEL<<19),g_TRGDBInfo.trg_gtl_header->CbltMcstCSR1,"CbltMcstCSR1",0xffffffff) ) return -1;
	if(-1==WrtRdA24D32(0x60d4 +(TrgGTLBSEL<<19),(g_TRGDBInfo.trg_gtl_header->TrgWindows&0xffffff)+(g_nTrgPerInt<<24),"GTL trgwindows",0xffffffff) ) return -1;
	if(-1==WrtRdA24D32(0x60d8 +(TrgGTLBSEL<<19),g_TRGDBInfo.trg_gtl_header->GlobalBufferNotFullFull,"GTL GlobalBufferNotFullFull",0xffffffff) ) return -1;
	if(-1==WrtRdA24D32(0x60e4 +(TrgGTLBSEL<<19),g_TRGDBInfo.trg_gtl_header->BarrelBhabhaPrescale,"GTL BarrelBhabhaPrescale",0xffffffff) ) return -1;
	if(-1==WrtRdA24D32(0x60e8 +(TrgGTLBSEL<<19),g_TRGDBInfo.trg_gtl_header->EndcapBhabhaPrescale,"GTL EndcapBhabhaPrescale",0xffffffff) ) return -1;

	if(-1==WrtRdA24D32(0x60ec +(TrgGTLBSEL<<19),g_TRGDBInfo.trg_gtl_header->BackupRegister,"GTL BackupRegister",0xffffffff) ) return -1;
	if(-1==WrtRdA24D32(0x60f0 +(TrgGTLBSEL<<19),g_TRGDBInfo.trg_gtl_header->TimingOutputBitsMdcEmcTof,"GTL TimingOutputBitsMdcEmcTof",0xffffffff) ) return -1;
	if(-1==WrtRdA24D32(0x60f4 +(TrgGTLBSEL<<19),g_TRGDBInfo.trg_gtl_header->TofTimingORedBits,"GTL TofTimingORedBits",0xffffffff) ) return -1;
	if(-1==WrtRdA24D32(0x60f8 +(TrgGTLBSEL<<19),g_TRGDBInfo.trg_gtl_header->EmcTimingORedBits,"GTL EmcTimingORedBits",0xffffffff) ) return -1;
	if(-1==WrtRdA24D32(0x60fc +(TrgGTLBSEL<<19),g_TRGDBInfo.trg_gtl_header->MdcTimingORedBits,"GTL MdcTimingORedBits",0xffffffff) ) return -1;
	if(-1==WrtRdA24D32(0x6100 +(TrgGTLBSEL<<19),g_TRGDBInfo.trg_gtl_header->TimeOutL1InternalWidth,"GTL TimeOutL1InternalWidth",0xffffffff) ) return -1;
	if(-1==WrtRdA24D32(0x6104 +(TrgGTLBSEL<<19),g_TRGDBInfo.trg_gtl_header->L1OutputWidthTimingSelectionMdcEmcTof,"GTL L1OutputWidthTiming",0xffffffff) ) return -1;
	if(-1==WrtRdA24D32(0x6108 +(TrgGTLBSEL<<19),g_TRGDBInfo.trg_gtl_header->L1ChkOutputControl,"GTL L1ChkOutputControl",0xffffffff) ) return -1;
	if(-1==WrtRdA24D32(0x610c +(TrgGTLBSEL<<19),g_TRGDBInfo.trg_gtl_header->ChkDelayToL1,"GTL ChkDelayToL1",0xffff) ) return -1;
	if(-1==WrtRdA24D32(0x6110 +(TrgGTLBSEL<<19),g_TRGDBInfo.trg_gtl_header->TrgLatencyInput,"GTL TrgLatencyInput",0xffffffff) ) return -1;
	if(-1==WrtRdA24D32(0x6114 +(TrgGTLBSEL<<19),g_TRGDBInfo.trg_gtl_header->TrgLatencyAligned,"GTL TrgLatencyAligned",0xffffffff) ) return -1;
	if(-1==WrtRdA24D32(0x6118 +(TrgGTLBSEL<<19),g_TRGDBInfo.trg_gtl_header->TrgLatencyEvtType,"GTL TrgLatencyEvtType",0xffffffff) ) return -1;

	//for trg table
	uDmaWillLen=16;

	printf("++++TrgChannelEnables = 0x%x\n",g_TRGDBInfo.trg_gtl_header->TrgChannelEnables);
	
	if(-1==WrtRdA24D32(0x60e0 +(TrgGTLBSEL<<19),g_TRGDBInfo.trg_gtl_header->TrgChannelEnables,"GTL TrgChannelEnables",0xffffffff) ) return -1;	
	
	for(int itable = 0;itable<16;itable++)
	{
		unsigned *dmatmp=(unsigned*)memalign(32,uDmaWillLen);

		//for set trigger L1 freqeuncy adjust, normally should from online,
		//test stage change temporary. 2KHz -> X5161; 4KHz -> X28B0; 6KHz -> X1B20; 8KHz -> X1458;10k->0x1045, 12KHz -> X0D8E
//		g_TRGDBInfo.trg_gtl_header->TrigTable[36]=0x1324;

		pAddrDmaWR =( (g_TRGDBInfo.trg_gtl_header->TrigTable)+(itable*4) );
		
		memcpy(dmatmp,pAddrDmaWR,uDmaWillLen);

		DMAinit(D32,D32_BLOCK,VME_A24_D32_BLT,TO_VME);
		DMArun((UINT32)(0x4000+(TrgGTLBSEL<<19)+itable*16),(UINT32)dmatmp,uDmaWillLen);		

		uDmaTransLen=dmawait(uDmaWillLen);
		if(uDmaTransLen==uDmaWillLen)
			dmadisable();
		else 
		{
			printf("write dmawait result doesn't match uDmaWillLen\n");
			Push2ErrStack(PPCFATAL+TRG_FLG+ATOM_ERR+78,0,0,0,0);		/*trg error 78*/
			return -1;
		}
		
		unsigned *datatmp=(unsigned*)memalign(32,uDmaWillLen);
		pAddrDmaWR=datatmp;
		DMAinit(D32,D32_BLOCK,VME_A24_D32_BLT,TO_LOCAL);

		DMArun((UINT32)(0x4000+(TrgGTLBSEL<<19)+itable*16),(UINT32)pAddrDmaWR,uDmaWillLen);		

		uDmaTransLen=dmawait(uDmaWillLen);
		if(uDmaTransLen==uDmaWillLen)
		{
			for(int idx=0;idx<4;idx++)
			{
				printf("0x%08x ",*pAddrDmaWR);
				pAddrDmaWR++;
				if( (idx%4)==3 ) printf("\n");
			}
			dmadisable();
		}
		else 
		{
			printf("read dmawait result doesn't match uDmaWillLen\n");	//fatal error?
			Push2ErrStack(PPCFATAL+TRG_FLG+ATOM_ERR+79,0,0,0,0);		/*trg error 79*/
			return -1;
		}
		free(dmatmp);
		free(datatmp);
	
	}
	return 0;
}

int TRG_GTL_syn2()
{
	WriteToVme32(0x18000+(TrgGTLBSEL<<19), VA24, 0x0);//reset DCM of GTL
	return 0;
}
int TRG_GTL_prep()
{
	WriteToVme32(0x10000+(TrgGTLBSEL<<19), VA24, 0x0);//global reset of GTL
	return 0;
}
int TRG_GTL_satr()
{
	if(-1==WrtRdA24D32(0x6108+(TrgGTLBSEL<<19), 0x3,"gtl_satr",0xff) ) return -1;               //GTL start L1
	return 0;
}
int TRG_GTL_sptr()
{
	// WriteToVme32(0x6108+(TrgGTLBSEL<<19), VA24, 0x0);//GTL stop L1
	WriteToVme32(0x6108+(TrgGTLBSEL<<19), VA24, 0x2);//GTL stop L1
	return 0;
}

/*________________________________________SAF__*/

int TRG_SAF_conf()
{
	unsigned int uWriteAddr;

	for(int iSAF=0;iSAF<g_TRGDBInfo.trg_saf_sum;iSAF++)
	{
		printf("TrgSAFGeo[%d]=%d\n",iSAF,TrgSAFGeo[iSAF]);
	}
	
	for(iSAF=0;iSAF<g_TRGDBInfo.trg_saf_sum;iSAF++)
	{
		WriteToVme32(0x10000+(TrgSAFBSEL[iSAF]<<19), VA24, 0x0);//global reset of SAF
		WriteToVme32(0x18000+(TrgSAFBSEL[iSAF]<<19) , VA24, 0x0);//reset of DCM
		uWriteAddr = (TrgSAFBSEL[iSAF]<<19) ;
		for(int ibx=0;ibx<16;ibx++)//channel delay
		{
			if(-1==WrtRdA24D32(uWriteAddr ,g_TRGDBInfo.trg_saf_header[iSAF]->DelayOfChannel[ibx],"SAF DelayOfChannel",0xffffffff) ) return -1;
			uWriteAddr+=4;
		}
		if(-1==WrtRdA24D32(0x00048+(TrgSAFBSEL[iSAF]<<19) ,g_TRGDBInfo.trg_saf_header[iSAF]->UserOwnCSR,"SAF UserOwnCSR",0xffffffff) ) return -1;
		if(-1==WrtRdA24D32(0x0004c+(TrgSAFBSEL[iSAF]<<19) ,g_TRGDBInfo.trg_saf_header[iSAF]->CbltMcstCSR1,"SAF CbltMcstCSR1",0xffffffff) ) return -1;
		if(-1==WrtRdA24D32(0x00054+(TrgSAFBSEL[iSAF]<<19),g_nTrgPerInt,"SAF g_nTrgPerInt",0xffffffff) ) return -1;
		if(-1==WrtRdA24D32(0x00060+(TrgSAFBSEL[iSAF]<<19) ,g_TRGDBInfo.trg_saf_header[iSAF]->TrgLatencyInput,"SAF TrgLatencyInput",0xffffffff) ) return -1;
		if(-1==WrtRdA24D32(0x00064+(TrgSAFBSEL[iSAF]<<19),g_TRGDBInfo.trg_saf_header[iSAF]->TrgWindowWidthInput,"SAF TrgWindowWidthInput",0xffffffff) ) return -1;
		if(-1==WrtRdA24D32(0x00068+(TrgSAFBSEL[iSAF]<<19) ,g_TRGDBInfo.trg_saf_header[iSAF]->TrgLatencyAligned,"SAF TrgLatencyAligned",0xffffffff) ) return -1;
		if(-1==WrtRdA24D32(0x0006c+(TrgSAFBSEL[iSAF]<<19) ,g_TRGDBInfo.trg_saf_header[iSAF]->TrgWindowWidthAligned,"SAF TrgWindowWidthAligned",0xffffffff) ) return -1;
		if(-1==WrtRdA24D32(0x00070+(TrgSAFBSEL[iSAF]<<19) ,g_TRGDBInfo.trg_saf_header[iSAF]->FullGlobalBuffer,"SAF FullGlobalBuffer",0xffffffff) ) return -1;
		if(-1==WrtRdA24D32(0x00074+(TrgSAFBSEL[iSAF]<<19) ,g_TRGDBInfo.trg_saf_header[iSAF]->FullGlobalORedBuffer,"SAF FullGlobalORedBuffer",0xffffffff) ) return -1;
		if(-1==WrtRdA24D32(0x00078+(TrgSAFBSEL[iSAF]<<19) ,g_TRGDBInfo.trg_saf_header[iSAF]->FullNotGlobalBuffer,"SAF FullNotGlobalBuffer",0xffffffff) ) return -1;
		if(-1==WrtRdA24D32(0x0007c+(TrgSAFBSEL[iSAF]<<19) ,g_TRGDBInfo.trg_saf_header[iSAF]->FullNotGlobalORedBuffer,"SAF FullNotGlobalORedBuffer",0xffffffff) ) return -1;
		if(-1==WrtRdA24D32(0x00080+(TrgSAFBSEL[iSAF]<<19) ,g_TRGDBInfo.trg_saf_header[iSAF]->L1Selection,"SAF L1Selection",0xffffffff) ) return -1;
	}
	return 0;
}
int TRG_SAF_syn2()
{
	for(int iSAF=0;iSAF<g_TRGDBInfo.trg_saf_sum;iSAF++)
	{
		WriteToVme32(0x10000+(TrgSAFBSEL[iSAF]<<19), VA24, 0x0);//global reset of SAF
		WriteToVme32(0x18000+(TrgSAFBSEL[iSAF]<<19), VA24, 0x0);//reset DCM of SAFL
//		printf("*******global reset: addr = 0x%x*******\n",(0x10000+(TrgSAFBSEL[iSAF]<<19)));	
	}
	return 0;
}
int TRG_SAF_prep()
{
//	printf("g_TRGDBInfo.trg_saf_sum = %d\n",g_TRGDBInfo.trg_saf_sum);
	for(int iSAF=0;iSAF<g_TRGDBInfo.trg_saf_sum;iSAF++)
	{
		WriteToVme32(0x10000+(TrgSAFBSEL[iSAF]<<19), VA24, 0x0);//global reset of SAF
//		printf("*******global reset: addr = 0x%x*******\n",(0x10000+(TrgSAFBSEL[iSAF]<<19)));		
	}
	return 0;
}

/*________________________________________EACC__*/

int TRG_EACC_preconf()
{
	//used to load fpga
	UINT32 write_addr,write_data;
	FILE *rfp;
	char cha[10];
	unsigned int uReadTmp;
//	if(-1==WrtRdA24D32(0x2020+(TrgEaccBSEL<<19),g_TRGDBInfo.trg_eacc_header->EtotDataWins,"EACC FPGA check",0xffffffff) ) 
	{

		printf("\r\nStart the EACC PreConfiguration!!!\r\n");
	
		if((rfp=fopen("./trigger/eacc.hex","r"))==NULL)
		{
			printf("Open file eacc.hex Error!\r\n");
			Push2ErrStack(PPCFATAL+TRG_FLG+ATOM_ERR+80,0,0,0,0);		/*trg error 80*/
			return -1;
		}

		write_addr = 0x40000+(TrgEaccBSEL<<19);
		if(-1==WriteToVme16(write_addr, VA24, 0x00000000))//VA32, write_data
		{
			printf("eacc.hex:Write Reset Error !\n");
			Push2ErrStack(PPCFATAL+TRG_FLG+ATOM_ERR+81,0,0,0,0);		/*trg error 81*/
			return -1;
		}

		Timedelay(20000);

		write_addr  = 0x40000 +(TrgEaccBSEL<<19)+ 6;//0x00000006;//0x00580006;

  		while(!feof(rfp))
  		{
    			fgets(cha,5,rfp);
    			cha[4]='\0';
			sscanf(cha,"%x",&write_data);
		    	if(-1==WriteToVme16(write_addr, VA24, write_data))//VA32, write_data
				{
					printf("eacc.hex:Write data Error !\n");
					Push2ErrStack(PPCFATAL+TRG_FLG+ATOM_ERR+82,0,0,0,0);		/*trg error 82*/
					return -1;
				}
		}
		
 		fclose(rfp);                  
		write_data = 0x1b1b;
		int i;
		for(i=0;i<30;i++)
		{
			if(-1==WriteToVme16(write_addr, VA24, write_data))//VA32, write_data
				{
					printf("eacc.hex:Write End Mark Error !\n");
					Push2ErrStack(PPCFATAL+TRG_FLG+ATOM_ERR+83,0,0,0,0);		/*trg error 83*/
					return -1;
				}
		}
		
		printf("\r\nEnd the EACC PreConfiguration!!!\r\n");
		fclose(rfp);
		printf("***********************************\n");	
	}
		
	return 0;
}
int TRG_EACC_conf()
{
	unsigned int uWriteAddr;
	
	WriteToVme32(0x21e0+(TrgEaccBSEL<<19), VA24, 0x1);//reset of DCM

	if(-1==WrtRdA24D32(0x2060+(TrgEaccBSEL<<19),g_TRGDBInfo.trg_eacc_header->SimRealSel,"EACC SimRealSel",0xffffffff) ) return -1;
	if(-1==WrtRdA24D32(0x2040+(TrgEaccBSEL<<19),g_TRGDBInfo.trg_eacc_header->L1Sel,"EACC L1Sel",0xffffffff) ) return -1;
	if(-1==WrtRdA24D32(0x20b0+(TrgEaccBSEL<<19),g_TRGDBInfo.trg_eacc_header->FiberAlignLatency,"EACC FiberAlignLatency",0xffffffff) ) return -1;
	if(-1==WrtRdA24D32(0x20c0+(TrgEaccBSEL<<19),g_TRGDBInfo.trg_eacc_header->FiberChannelSet,"EACC FiberChannelSet",0xffffffff) ) return -1;
	if(-1==WrtRdA24D32(0x1000+(TrgEaccBSEL<<19),g_TRGDBInfo.trg_eacc_header->CSR1,"EACC CSR1",0xffffffff) ) return -1;
	if(-1==WrtRdA24D32(0x2010+(TrgEaccBSEL<<19),g_TRGDBInfo.trg_eacc_header->CsumDepths,"EACC CsumDepths",0xffffffff) ) return -1;
	if(-1==WrtRdA24D32(0x2000+(TrgEaccBSEL<<19),g_TRGDBInfo.trg_eacc_header->CsumWins,"EACC CsumWins",0xffffffff) ) return -1;
	if(-1==WrtRdA24D32(0x2050+(TrgEaccBSEL<<19),(g_nTrgPerInt<<16)+((g_TRGDBInfo.trg_eacc_header->DataLenPerEvent)&0xffff),"EACC DataLenPerEvent",0xffffffff) ) return -1;
	if(-1==WrtRdA24D32(0x20a0+(TrgEaccBSEL<<19),g_TRGDBInfo.trg_eacc_header->EtotDataSteps,"EACC EtotDataSteps",0xffffffff) ) return -1;
	if(-1==WrtRdA24D32(0x2030+(TrgEaccBSEL<<19),g_TRGDBInfo.trg_eacc_header->EtotDataDepths,"EACC EtotDataDepths",0xffffffff) ) return -1;
	if(-1==WrtRdA24D32(0x2020+(TrgEaccBSEL<<19),g_TRGDBInfo.trg_eacc_header->BarrelEndcapSel,"EACC BarrelEndcapSel",0xffffffff) ) return -1;

	uWriteAddr=0x2100+(TrgEaccBSEL<<19);
	for(int ibx=0;ibx<10;ibx++)//Threshold
	{
		if(-1==WrtRdA24D32(uWriteAddr,g_TRGDBInfo.trg_eacc_header->Threshold[ibx],"EACC Threshold",0xffffffff) ) return -1;
		uWriteAddr+=16;
	}
	if(-1==WrtRdA24D32(0x20f0+(TrgEaccBSEL<<19),g_TRGDBInfo.trg_eacc_header->ErrSet,"EACC ErrSet",0xffffffff) ) return -1;

	WriteToVme32(0x21d0+(TrgEaccBSEL<<19), VA24, 0xd7541000);

	return 0;
}

int TRG_EACC_syn2()
{
	WriteToVme32(0x21e0+(TrgEaccBSEL<<19), VA24, 0x1);// reset DCM of EACC
	return 0;
}
int TRG_EACC_prep()
{
	WriteToVme32(0x21e0+(TrgEaccBSEL<<19), VA24, 0x2);//global reset of EACC ,step 7

	unsigned int uReadTmp;
	ReadFrmVme32(0x21f0+(TrgEaccBSEL<<19), VA24, &uReadTmp);//read for fiber init
	printf("EACC prep uReadTmp=%x\n ",uReadTmp);


	if(TrgEaccSYNCheck)
	{
		printf("enter EACC TrgEaccSYNCheck \n" );
		if((uReadTmp>>16)!=g_TRGDBInfo.trg_eacc_header->FiberChannelSet)
		{
			printf("TRG EACC Fiber Channel ErrorA : 0x%x\n",uReadTmp);
			Push2ErrStack(PPCERROR+TRG_FLG+ATOM_ERR+122,0,0,0,0);		/*trg error 122*/
		}

		if(((uReadTmp&0xfff)&g_TRGDBInfo.trg_eacc_header->FiberChannelSet)!=
				(g_TRGDBInfo.trg_eacc_header->FiberChannelSet&0xfff))
		{
			printf("TRG EACC Fiber Channel ErrorB : 0x%x\n",uReadTmp);
			Push2ErrStack(PPCWARNING+TRG_FLG+ATOM_ERR+123,0,0,0,0);		/*trg error 123*/
		}	
	}


	return 0;
}

int TRG_EACC_sptr()	//n>=1~L1 time
{
	unsigned int uReadTmp;
	/*
	ReadFrmVme32(0x2080+(TrgEaccBSEL<<19), VA24, &uReadTmp);
	printf("EACC n>=1 to L1 time  =%x\n ",uReadTmp);
	
	WriteToVme32(0x20d0+(TrgEaccBSEL<<19), VA24, 0xf); 
	WriteToVme32(0x20e0+(TrgEaccBSEL<<19), VA24, 0x0); 

	ReadFrmVme32(0x2010+(TrgEaccBSEL<<19), VA24, &uReadTmp);
	printf("EACC calibration Csum  =%x\n ",uReadTmp);	

	ReadFrmVme32(0x2030+(TrgEaccBSEL<<19), VA24, &uReadTmp);
	printf("EACC calibration Etotal  =%x\n ",uReadTmp);	
	
	WriteToVme32(0x2090+(TrgEaccBSEL<<19), VA24, 0x0); //clear
*/
	ReadFrmVme32(0x21f0+(TrgEaccBSEL<<19), VA24, &uReadTmp);//read for fiber init
	printf("EACC sptr fiberalign state=%x\n ",uReadTmp);	


	
}

/*______________________________________TDC____*/

int TRG_TDC_conf()
{
	unsigned int uWriteVal;
	unsigned short uReadVal=0;
	unsigned short temp[4]={5,5,5,5};
	
	uWriteVal=0x00004000;

	printf("addr = 0x%x\n",TrgTDCBSEL<<19);
	WriteToVme16(0x1016+(TrgTDCBSEL<<19), VA24, 0x0);//software clear
	WriteToVme16(0x1018+(TrgTDCBSEL<<19), VA24, 0x0);//Software Event Reset	

	if(-1==WrtType1A24D16(0x102E+(TrgTDCBSEL<<19) ,0x4300,"TDC DisableAllTdcChannels") )return -1;

	if(-1==WrtType1A24D16(0x102E+(TrgTDCBSEL<<19) ,0x4600,"TDC0 Enable") )return -1;
	for(int idx=0;idx<2;idx++)
	{
		if(-1==WrtType1A24D16(0x102E+(TrgTDCBSEL<<19),g_TRGDBInfo.trg_tdc_header->ChannelEnable[idx],"TDC WriteTdcEnablePattern") ) return -1;
		taskDelay(30);
	}
	if(-1==WrtType1A24D16(0x102E+(TrgTDCBSEL<<19) ,0x4601,"TDC1 Enable") )return -1;
	for( idx=2;idx<4;idx++)
	{
		if(-1==WrtType1A24D16(0x102E+(TrgTDCBSEL<<19),g_TRGDBInfo.trg_tdc_header->ChannelEnable[idx],"TDC WriteTdcEnablePattern") ) return -1;
		taskDelay(30);
	}	
	if(-1==WrtType1A24D16(0x102E+(TrgTDCBSEL<<19) ,0x4500 ,"TDC ReadTdcEnablePattern") )return -1;
	
	for(idx=0;idx<4;idx++)
	{
		if(-1==ReadType2A24D16(0x102E+(TrgTDCBSEL<<19),&uReadVal,"TDC ReadTdcEnablePattern") ) return -1;
		if(uReadVal!=g_TRGDBInfo.trg_tdc_header->ChannelEnable[idx])
		{
			printf("uReadVal = 0x%x,TDC ReadTdcEnablePattern\n",uReadVal);
			return -1;
		}
	}

	if(-1==WrtType1A24D16(0x102E +(TrgTDCBSEL<<19),0x0,"TDC SetTriggerMatchingMode") )return -1;

	//ReadAcquisitionMode
	if(-1==WrtType1A24D16(0x102E+(TrgTDCBSEL<<19) ,0x200,"TDC ReadAcquisitionMode") )return -1;
	if(-1==ReadType2A24D16(0x102E+(TrgTDCBSEL<<19),&uReadVal,"TDC ReadAcquisitionMode") ) return -1;
	if((uReadVal&0x1) !=0x1)	//trig window width
	{
		printf("TDC ReadAcquisitionMode compare error\n");
		Push2ErrStack(PPCFATAL+TRG_FLG+ATOM_ERR+84,0,0,0,0);		/*trg error 84*/
		return -1;
	}
	
	//SetTriggerWindowWidth & 01
	if(-1==WrtType1A24D16(0x102E+(TrgTDCBSEL<<19) ,0x1000,"TDC SetTriggerWindowWidth") )return -1;
	if(-1==WrtType1A24D16(0x102E+(TrgTDCBSEL<<19) ,g_TRGDBInfo.trg_tdc_header->SetTriggerWindowWidth01,"TDC SetTriggerWindowWidth01") )return -1;
	//SetTriggerWindowOffset & 01
	if(-1==WrtType1A24D16(0x102E +(TrgTDCBSEL<<19),0x1100,"TDC SetTriggerWindowOffset") )return -1;
	if(-1==WrtType1A24D16(0x102E +(TrgTDCBSEL<<19),g_TRGDBInfo.trg_tdc_header->SetTriggerWindowOffset01,"TDC SetTriggerWindowOffset01") )return -1;
	//SetExtraSearchMargin & 01
	if(-1==WrtType1A24D16(0x102E+(TrgTDCBSEL<<19) ,0x1200,"TDC SetExtraSearchMargin") )return -1;
	if(-1==WrtType1A24D16(0x102E+(TrgTDCBSEL<<19) ,g_TRGDBInfo.trg_tdc_header->SetExtraSearchMargin01,"TDC SetExtraSearchMargin01") )return -1;
	//SetRejectMargin & 01
	if(-1==WrtType1A24D16(0x102E +(TrgTDCBSEL<<19),0x1300,"TDC SetRejectMargin") )return -1;
	if(-1==WrtType1A24D16(0x102E +(TrgTDCBSEL<<19),g_TRGDBInfo.trg_tdc_header->SetRejectMargin01,"TDC SetRejectMargin01") )return -1;

	//EnableSubtractionOfTriggerTime
	if(-1==WrtType1A24D16(0x102E+(TrgTDCBSEL<<19) ,0x1400,"TDC EnableSubtractionOfTriggerTime") )return -1;

	//ReadTriggerConfiguration
	if(-1==WrtType1A24D16(0x102E+(TrgTDCBSEL<<19) ,0x1600,"TDC ReadTriggerConfiguration") )return -1;
	if(-1==ReadType2A24D16(0x102E+(TrgTDCBSEL<<19),&uReadVal,"TDC ReadTriggerConfiguration01") ) return -1;
	if(uReadVal != (g_TRGDBInfo.trg_tdc_header->SetTriggerWindowWidth01) )	//trig window width
	{
		printf("uReadVal = 0x%x,TDC ReadTriggerConfiguration01 compare error\n",uReadVal);
		Push2ErrStack(PPCFATAL+TRG_FLG+ATOM_ERR+85,0,0,0,0);		/*trg error 85*/
		return -1;
	}
	if(-1==ReadType2A24D16(0x102E+(TrgTDCBSEL<<19),&uReadVal,"TDC ReadTriggerConfiguration02") ) return -1;
	if(uReadVal !=g_TRGDBInfo.trg_tdc_header->SetTriggerWindowOffset01)	//Window Offset
	{
		printf("TDC ReadTriggerConfiguration02 compare error\n");
		Push2ErrStack(PPCFATAL+TRG_FLG+ATOM_ERR+86,0,0,0,0);		/*trg error 86*/
		return -1;
	}
	if(-1==ReadType2A24D16(0x102E+(TrgTDCBSEL<<19),&uReadVal,"TDC ReadTriggerConfiguration03") ) return -1;
	if(uReadVal !=g_TRGDBInfo.trg_tdc_header->SetExtraSearchMargin01)	//SetExtraSearchMargin
	{
		printf("TDC ReadTriggerConfiguration03 compare error\n");
		Push2ErrStack(PPCFATAL+TRG_FLG+ATOM_ERR+87,0,0,0,0);		/*trg error 87*/
		return -1;
	}
	if(-1==ReadType2A24D16(0x102E+(TrgTDCBSEL<<19),&uReadVal,"TDC ReadTriggerConfiguration04") ) return -1;
	if(uReadVal !=g_TRGDBInfo.trg_tdc_header->SetRejectMargin01)	//SetRejectMargin
	{
		printf("TDC ReadTriggerConfiguration04 compare error\n");
		Push2ErrStack(PPCFATAL+TRG_FLG+ATOM_ERR+88,0,0,0,0);		/*trg error 88*/
		return -1;
	}
	if(-1==ReadType2A24D16(0x102E+(TrgTDCBSEL<<19),&uReadVal,"TDC ReadTriggerConfiguration05") ) return -1;
	if((uReadVal&0x1) !=0x1)
	{
		printf("TDC ReadTriggerConfiguration05 compare error\n");
		Push2ErrStack(PPCFATAL+TRG_FLG+ATOM_ERR+89,0,0,0,0);		/*trg error 89*/
		return -1;
	}
	//SetEdgeDetectionConfiguration
	if(-1==WrtType1A24D16(0x102E +(TrgTDCBSEL<<19),0x2200,"TDC SetEdgeDetectionConfiguration") )return -1;
	if(-1==WrtType1A24D16(0x102E+(TrgTDCBSEL<<19) ,g_TRGDBInfo.trg_tdc_header->SetEdgeDetectionConfig01,"TDC SetEdgeDetectionConfig01") )return -1;

	//ReadEdgeDetectionConfiguration
	if(-1==WrtType1A24D16(0x102E+(TrgTDCBSEL<<19) ,0x2300,"TDC ReadEdgeDetectionConfig") )return -1;
	if(-1==ReadType2A24D16(0x102E+(TrgTDCBSEL<<19),&uReadVal,"TDC ReadEdgeDetectionConfig") ) return -1;
	if((uReadVal&0x3) !=((g_TRGDBInfo.trg_tdc_header->SetEdgeDetectionConfig01)&0x3))	
	{
		printf("TDC ReadEdgeDetectionConfig compare error\n");
		Push2ErrStack(PPCFATAL+TRG_FLG+ATOM_ERR+90,0,0,0,0);		/*trg error 90*/
		return -1;
	}

	//SetLeadingTrailingEdge
	if(-1==WrtType1A24D16(0x102E+(TrgTDCBSEL<<19) ,0x2400,"TDC SetLeadingTrailingEdge") )return -1;
	if(-1==WrtType1A24D16(0x102E +(TrgTDCBSEL<<19),g_TRGDBInfo.trg_tdc_header->SetLeadingTrailingEdge01,"TDC SetLeadingTrailingEdge") )return -1;

	//ReadLeadingTrailingEdge
	if(-1==WrtType1A24D16(0x102E+(TrgTDCBSEL<<19) ,0x2600,"TDC ReadLeadingTrailingEdge") )return -1;
	if(-1==ReadType2A24D16(0x102E+(TrgTDCBSEL<<19),&uReadVal,"TDC ReadLeadingTrailingEdge") ) return -1;
	if((uReadVal&0x3) !=( (g_TRGDBInfo.trg_tdc_header->SetLeadingTrailingEdge01) &0x3))	
	{
		printf("TDC ReadLeadingTrailingEdge compare error\n");
		Push2ErrStack(PPCFATAL+TRG_FLG+ATOM_ERR+91,0,0,0,0);		/*trg error 91*/
		return -1;
	}

	//SetChannelDeadTimeOfHits
	if(-1==WrtType1A24D16(0x102E+(TrgTDCBSEL<<19) ,0x2800,"TDC SetChannelDeadTimeOfHits") )return -1;
	if(-1==WrtType1A24D16(0x102E+(TrgTDCBSEL<<19) ,g_TRGDBInfo.trg_tdc_header->SetChannelDeadTimeOfHits01,"TDC SetChannelDeadTimeOfHits") )return -1;

	//ReadChannelDeadTimeOfHits
	if(-1==WrtType1A24D16(0x102E+(TrgTDCBSEL<<19) ,0x2900,"TDC ReadChannelDeadTimeOfHits") )return -1;
	if(-1==ReadType2A24D16(0x102E+(TrgTDCBSEL<<19),&uReadVal,"TDC ReadChannelDeadTimeOfHits") ) return -1;
	if((uReadVal&0x3) !=( (g_TRGDBInfo.trg_tdc_header->SetChannelDeadTimeOfHits01) &0x3))
	{
		printf("TDC ReadChannelDeadTimeOfHits compare error\n");
		Push2ErrStack(PPCFATAL+TRG_FLG+ATOM_ERR+92,0,0,0,0);		/*trg error 92*/
		return -1;
	}
	
	//EnableTdcHeaderTrailerReadout
	if(-1==WrtType1A24D16(0x102E+(TrgTDCBSEL<<19) ,0x3000,"TDC EnableTdcHeaderTrailerReadout") )return -1;

	//ReadTdcHeaderTrailerStatus
	if(-1==WrtType1A24D16(0x102E+(TrgTDCBSEL<<19) ,0x3200,"TDC ReadTdcHeaderTrailerStatus") )return -1;
	if(-1==ReadType2A24D16(0x102E+(TrgTDCBSEL<<19),&uReadVal,"TDC ReadTdcHeaderTrailerStatus") ) return -1;
	if((uReadVal&0x1) !=0x1)
	{
		printf("TDC ReadTdcHeaderTrailerStatus compare error\n");
		Push2ErrStack(PPCFATAL+TRG_FLG+ATOM_ERR+93,0,0,0,0);		/*trg error 93*/
		return -1;
	}

	//SetMaxNumberOfHitsPerEvent
	if(-1==WrtType1A24D16(0x102E+(TrgTDCBSEL<<19) ,0x3300,"TDC SetMaxNumberOfHitsPerEvent") )return -1;
	if(-1==WrtType1A24D16(0x102E+(TrgTDCBSEL<<19) ,g_TRGDBInfo.trg_tdc_header->SetMaxNumberOfHitsPerEvent01,"TDC SetMaxNumberOfHitsPerEvent") )return -1;

	//ReadMaxNumberOfHitsPerEvent
	if(-1==WrtType1A24D16(0x102E +(TrgTDCBSEL<<19),0x3400,"TDC ReadMaxNumberOfHitsPerEvent") )return -1;
	if(-1==ReadType2A24D16(0x102E+(TrgTDCBSEL<<19),&uReadVal,"TDC ReadMaxNumberOfHitsPerEvent") ) return -1;
	if((uReadVal&0xf) !=( (g_TRGDBInfo.trg_tdc_header->SetMaxNumberOfHitsPerEvent01)&0xf))
	{
		printf("TDC ReadMaxNumberOfHitsPerEvent compare error\n");
		Push2ErrStack(PPCFATAL+TRG_FLG+ATOM_ERR+94,0,0,0,0);		/*trg error 94*/
		return -1;
	}
	//EnableTdcErrorMark
	if(-1==WrtType1A24D16(0x102E+(TrgTDCBSEL<<19) ,0x3500,"TDC EnableTdcErrorMark") )return -1;
	//DisableBypassTdcIfError
	if(-1==WrtType1A24D16(0x102E+(TrgTDCBSEL<<19) ,0x3800,"TDC DisableBypassTdcIfError") )return -1;
	//EnableTdcInternalErrorType
	if(-1==WrtType1A24D16(0x102E+(TrgTDCBSEL<<19) ,0x3900,"TDC EnableTdcInternalErrorType") )return -1;
	if(-1==WrtType1A24D16(0x102E+(TrgTDCBSEL<<19) ,g_TRGDBInfo.trg_tdc_header->EnableTdcInternalErrorType01,"TDC EnableTdcInternalErrorType") )return -1;

	//ReadEnabledTdcInternalErrorType
	if(-1==WrtType1A24D16(0x102E+(TrgTDCBSEL<<19) ,0x3a00,"TDC ReadEnabledTdcInternalErrorType") )return -1;
	if(-1==ReadType2A24D16(0x102E+(TrgTDCBSEL<<19),&uReadVal,"TDC ReadEnabledTdcInternalErrorType") ) return -1;
	if((uReadVal&0x7ff) !=((g_TRGDBInfo.trg_tdc_header->EnableTdcInternalErrorType01)&0x7ff))
	{
		printf("TDC ReadEnabledTdcInternalErrorType compare error\n");
		Push2ErrStack(PPCFATAL+TRG_FLG+ATOM_ERR+95,0,0,0,0);		/*trg error 95*/
		return -1;
	}

	//SetEffectiveSizeOfReadoutFifo
	if(-1==WrtType1A24D16(0x102E+(TrgTDCBSEL<<19) ,0x3B00,"TDC SetEffectiveSizeOfReadoutFifo") )return -1;
	if(-1==WrtType1A24D16(0x102E+(TrgTDCBSEL<<19) ,g_TRGDBInfo.trg_tdc_header->SetEffectiveSizeOfReadoutFifo01,"TDC SetEffectiveSizeOfReadoutFifo01") )return -1;

	//ReadEffectiveSizeOfReadoutFifo
	if(-1==WrtType1A24D16(0x102E+(TrgTDCBSEL<<19) ,0x3c00,"TDC ReadEffectiveSizeOfReadoutFifo") )return -1;
	if(-1==ReadType2A24D16(0x102E+(TrgTDCBSEL<<19),&uReadVal,"TDC ReadEffectiveSizeOfReadoutFifo") ) return -1;
	if((uReadVal&0x7) !=((g_TRGDBInfo.trg_tdc_header->SetEffectiveSizeOfReadoutFifo01)&0x7))
	{
		printf("TDC ReadEffectiveSizeOfReadoutFifo compare error\n");
		Push2ErrStack(PPCFATAL+TRG_FLG+ATOM_ERR+96,0,0,0,0);		/*trg error 96*/
		return -1;
	}

	//SetDllClock
	if(-1==WrtType1A24D16(0x102E+(TrgTDCBSEL<<19) ,0xc800,"TDC SetEffectiveSizeOfReadoutFifo") )return -1;
	if(-1==WrtType1A24D16(0x102E+(TrgTDCBSEL<<19) ,g_TRGDBInfo.trg_tdc_header->SetDllClock01,"TDC SetDllClock01") )return -1;

	//ResetPllDll
	if(-1==WrtType1A24D16(0x102E+(TrgTDCBSEL<<19) ,0x6200,"TDC ResetPllDll") )return -1;
	
	//SetControlRegister
	if(-1==WrtRdA24D16(0x1000+(TrgTDCBSEL<<19),g_TRGDBInfo.trg_tdc_header->SetControlRegister,"TDC SetControlRegister",0x3ff) ) return -1;
	//SetInterruptLevel
	if(-1==WrtRdA24D16(0x100A+(TrgTDCBSEL<<19),g_TRGDBInfo.trg_tdc_header->SetInterruptLevel,"TDC SetInterruptLevel",0x7) ) return -1;
	//SetInterruptVector
	if(-1==WrtRdA24D16(0x100C+(TrgTDCBSEL<<19),g_TRGDBInfo.trg_tdc_header->SetInterruptVector,"TDC SetInterruptVector",0xff) ) return -1;	
	//SetGeoAddress
	if(-1==WrtRdA24D16(0x100E+(TrgTDCBSEL<<19),g_TRGDBInfo.trg_tdc_header->SetGeoAddress,"TDC SetGeoAddress",0x1f) ) return -1;
	//SetMCST/CBLTBASEAddress
	if(-1==WrtRdA24D16(0x1010+(TrgTDCBSEL<<19),g_TRGDBInfo.trg_tdc_header->SetCbltMcstBaseAddress,"TDC SetCbltMcstBaseAddress",0xff) ) return -1;
	//SetCbltMcstCSR
	if(-1==WrtRdA24D16(0x1012+(TrgTDCBSEL<<19),g_TRGDBInfo.trg_tdc_header->SetCbltMcstCSR,"TDC SetCbltMcstCSR",0x3) ) return -1;
	//SetAlmostFull
	if(-1==WrtRdA24D16(0x1022+(TrgTDCBSEL<<19),g_TRGDBInfo.trg_tdc_header->SetAlmostFull,"TDC SetAlmostFull",0xffff) ) return -1;
	//SetNevtPerBltTransfer
	if(-1==WrtRdA24D16(0x1024+(TrgTDCBSEL<<19),g_nTrgPerInt,"TDC g_nTrgPerInt",0xff) ) return -1;
	//SetOutProgControl
	if(-1==WrtRdA24D16(0x102c+(TrgTDCBSEL<<19),g_TRGDBInfo.trg_tdc_header->SetOutProgControl,"TDC SetOutProgControl",0x7) ) return -1;
	return 0;
}

int  TRG_TDC_syn2()
{
	//ResetPllDll
	if(-1==WrtType1A24D16(0x102E +(TrgTDCBSEL<<19),0x6200,"TDC syn2 ResetPllDll") )return -1;
	return 0;
}
int TRG_TDC_prep()
{
	WriteToVme16(0x1016+(TrgTDCBSEL<<19), VA24, 0x0);//software clear
	WriteToVme16(0x1018+(TrgTDCBSEL<<19), VA24, 0x0);//Software Event Reset

	return 0;
}
/*______________________________________LTKC_____*/ 

int TRG_LTKC_syn3()
{	
	WriteToVme32(0x7000+(TrgLTKCBSEL<<19), VA32,0x0);//reset
	WriteToVme32(0x1000+(TrgLTKCBSEL<<19), VA32, (( g_TRGDBInfo.trg_ltkc_header->CBLTSet)&0xfffff0ff)|((g_nTrgPerInt-1)<<8));//CBLTSet
	WriteToVme32(0x2000+(TrgLTKCBSEL<<19), VA32, g_TRGDBInfo.trg_ltkc_header->L1Select);//L1Select
	
	unsigned int uReadTmp;
	ReadFrmVme32( 0x1000+(TrgLTKCBSEL<<19), VA32,&uReadTmp);
	printf("LTKC_syn3 0x1000 uReadTmp=0x%x\n ",uReadTmp);

	return 0;
}

int TRG_LTKC_prep()
{
	WriteToVme32(0x7000+(TrgLTKCBSEL<<19), VA32,0x0);//reset
	return 0;
}
/*______________________________________STKC_____*/

int TRG_STKC_syn3()
{	
	WriteToVme32(0x7000+(TrgSTKCBSEL<<19), VA32,0x0);//reset
	WriteToVme32(0x1000+(TrgSTKCBSEL<<19), VA32, (( g_TRGDBInfo.trg_stkc_header->CBLTSet)&0xfffff0ff)|((g_nTrgPerInt-1)<<8));//CBLTSet
	WriteToVme32(0x2000+(TrgSTKCBSEL<<19), VA32, g_TRGDBInfo.trg_stkc_header->L1Select);//L1Select

	unsigned int uReadTmp;
	ReadFrmVme32( 0x1000+(TrgSTKCBSEL<<19), VA32,&uReadTmp);
	printf("STKC_syn3 0x1000 uReadTmp=0x%x\n ",uReadTmp);
	
	return 0;
}

int TRG_STKC_prep()
{
	WriteToVme32(0x7000+(TrgSTKCBSEL<<19), VA32,0x0);//reset
	return 0;
}
/*_____________________________________TKF_____*/

int TRG_TKF_preconf()
{
	//here may used to load fpga,normal no use

	UINT32 write_addr,write_data;
	FILE *rfp;
	char cha[10];
	unsigned int uReadTmp;
	
	for (int iTKF=0; iTKF<g_TRGDBInfo.trg_tkf_sum; iTKF++)
	{
		printf("iTKF=%d\n",iTKF);
		
		ReadFrmVme32( TrgTKFBSEL[iTKF]<<19, VA24,&uReadTmp);
//		if((uReadTmp&0xffffff)!= 0x801141)
		if((uReadTmp&0xffffff)!= 0x801291)
		{
			/******** itkf board has seperate load file***************/
			if(0x12==TrgTKFBSEL[iTKF])	//ITKF GEO
			{
				if((rfp=fopen("./trigger/itkf.hex","r"))==NULL)
				{
					printf("Open file itkf.hex Error!\n");
					Push2ErrStack(PPCFATAL+TRG_FLG+ATOM_ERR+97,0,0,0,0);		/*trg error 97*/
					return -1;
				}
				printf("itkf.hex opened!\n");
			}
			else
			{
				if((rfp=fopen("./trigger/tkf.hex","r"))==NULL)
				{
					printf("Open file tkf.hex Error!\n");
					Push2ErrStack(PPCFATAL+TRG_FLG+ATOM_ERR+130,0,0,0,0);		/*trg error 130*/
					return -1;
				}
				printf(" tkf.hex opened!\n");
			}

			write_addr = 0x40000+(TrgTKFBSEL[iTKF]<<19);
			if(-1==WriteToVme16(write_addr, VA24, 0x00000000))//VA32, write_data
			{
				printf(" tkf.hex:Write Reset Error !\n");
				Push2ErrStack(PPCFATAL+TRG_FLG+ATOM_ERR+98,0,0,0,0);		/*trg error 98*/
				return -1;
			}
			Timedelay(20000);

			write_addr  = 0x40000+(TrgTKFBSEL[iTKF]<<19) + 6;//0x00000006;//0x00580006;
//			printf("TrgTKFBSEL = 0x%x\n",(TrgTKFBSEL[iTKF]<<19));
	  		while(!feof(rfp))
	  		{
	    			fgets(cha,5,rfp);
	    			cha[4]='\0';
				sscanf(cha,"%x",&write_data);
			    	if(-1==WriteToVme16(write_addr, VA24, write_data))//VA32, write_data
				{
					printf(" tkf.hex:Write data Error !\n");
					Push2ErrStack(PPCFATAL+TRG_FLG+ATOM_ERR+99,0,0,0,0);		/*trg error 99*/
					return -1;
				}
			}
	 		fclose(rfp);                  
			write_data = 0x1b1b;
			int i;
			for(i=0;i<30;i++)
			{
				if(-1==WriteToVme16(write_addr, VA24, write_data))//VA32, write_data
					{
						printf(" tkf.hex:Write End Mark Error !\n");
						Push2ErrStack(PPCFATAL+TRG_FLG+ATOM_ERR+100,0,0,0,0);		/*trg error 100*/
						return -1;
					}
			}
			
			ReadFrmVme32( TrgTKFBSEL[iTKF]<<19, VA24,&uReadTmp);
			printf("uReadTmp=%x\n ",uReadTmp);
			if (uReadTmp==0xffffffff)
			{
				printf("TKF preconf error\n");
				Push2ErrStack(PPCFATAL+TRG_FLG+ATOM_ERR+101,0,0,0,0);		/*trg error 101*/
				fclose(rfp);
				return -1;
			}
			
			printf("\r\nEnd the TKF PreConfiguration!!!\r\n");
			
			fclose(rfp);
			printf("***********************************\n");	
		}
	}	
	return 0;
}



int TRG_TKF_syn3()
{

	for(int iTKF=0;iTKF<g_TRGDBInfo.trg_tkf_sum;iTKF++)
	{
		WriteToVme32(0x0000fc+(TrgTKFBSEL[iTKF]<<19), VA24,0x1);//reset
		taskDelay(1);
		WriteToVme32(0x0000f8+(TrgTKFBSEL[iTKF]<<19), VA24,0x1);//reset CBLT
		//chain set
		if(-1==WrtRdA24D32(0x000004+(TrgTKFBSEL[iTKF]<<19),g_TRGDBInfo.trg_tkf_header[iTKF]->ChainSet,"TKF ChainSet",0xffff) )return -1;
		//ReadoutEventNum
		if(-1==WrtRdA24D32(0x000008+(TrgTKFBSEL[iTKF]<<19),g_nTrgPerInt-1,"TKF g_nTrgPerInt",0xffffffff) )return -1;
		//ReadoutEventWindow
		if(-1==WrtRdA24D32(0x00000c+(TrgTKFBSEL[iTKF]<<19),g_TRGDBInfo.trg_tkf_header[iTKF]->ReadoutEventWindow,"TKF ReadoutEventWindow",0xffffffff) )return -1;
		//ProgramFull
		if(-1==WrtRdA24D32(0x000010+(TrgTKFBSEL[iTKF]<<19),g_TRGDBInfo.trg_tkf_header[iTKF]->ProgramFull,"TKF ProgramFull",0xffffffff) )return -1;
		//ProgramFullDisable
		if(-1==WrtRdA24D32(0x000014+(TrgTKFBSEL[iTKF]<<19),g_TRGDBInfo.trg_tkf_header[iTKF]->ProgramFullDisable,"TKF ProgramFullDisable",0xffffffff) )return -1;
		//L1Select
		if(-1==WrtRdA24D32(0x000018+(TrgTKFBSEL[iTKF]<<19),g_TRGDBInfo.trg_tkf_header[iTKF]->L1Select,"TKF L1Select",0xffffffff) )return -1;
		//ReadoutModeSelect
		if(-1==WrtRdA24D32(0x000028+(TrgTKFBSEL[iTKF]<<19),g_TRGDBInfo.trg_tkf_header[iTKF]->ReadoutModeSelect,"TKF ReadoutModeSelect",0xffffffff) )return -1;
		//L1LatencyToHit
		if(-1==WrtRdA24D32(0x000024+(TrgTKFBSEL[iTKF]<<19),g_TRGDBInfo.trg_tkf_header[iTKF]->L1LatencyToHit,"TKF L1LatencyToHit",0xffffffff) )return -1;
		//TSFLatencyToHit
		if(-1==WrtRdA24D32(0x00002c+(TrgTKFBSEL[iTKF]<<19),g_TRGDBInfo.trg_tkf_header[iTKF]->TSFLatencyToHit,"TKF TSFLatencyToHit",0xffffffff) )return -1;
		//TrackLatencyToHit
		if(-1==WrtRdA24D32(0x000030+(TrgTKFBSEL[iTKF]<<19),g_TRGDBInfo.trg_tkf_header[iTKF]->TrackLatencyToHit,"TKF TrackLatencyToHit",0xf) )return -1;
		//FiberSyncLatencyToFRST
		if(-1==WrtRdA24D32(0x000034+(TrgTKFBSEL[iTKF]<<19),g_TRGDBInfo.trg_tkf_header[iTKF]->FiberSyncLatencyToFRST,"TKF FiberSyncLatencyToFRST",0xffffffff) )return -1;
		//FiberChannelSelect
		if(-1==WrtRdA24D32(0x000038+(TrgTKFBSEL[iTKF]<<19),g_TRGDBInfo.trg_tkf_header[iTKF]->FiberChannelSelect,"TKF FiberChannelSelect",0xffffffff) )return -1;
		//DataSelect
		if(-1==WrtRdA24D32(0x000108+(TrgTKFBSEL[iTKF]<<19),g_TRGDBInfo.trg_tkf_header[iTKF]->DataSelect,"TKF DataSelect",0xffffffff) )return -1;

	}
	return 0;
}


int TRG_TKF_prep()
{
	unsigned int uReadTmp;
	
	for(int iTKF=0;iTKF<g_TRGDBInfo.trg_tkf_sum;iTKF++)
	{
		WriteToVme32(0x000000f8+(TrgTKFBSEL[iTKF]<<19), VA24,0x1);//reset CBLT

		for(int i =0;i<12;i++)
		{
			ReadFrmVme32(0x3c+(TrgTKFBSEL[iTKF]<<19)+(i*4), VA24, &uReadTmp);//read for fiber init
			printf("TKF 0x%x prep uReadTmp=%x\n ",(0x3c+(TrgTKFBSEL[iTKF]<<19)+(i*4)), uReadTmp);
			if((0==i)&&(TrgTKFSYNCheck[iTKF]))
				if((uReadTmp&g_TRGDBInfo.trg_tkf_header[iTKF]->FiberChannelSelect)!=g_TRGDBInfo.trg_tkf_header[iTKF]->FiberChannelSelect)
				{
					printf("TRG Error TRG_TKF_prep,Error TKF GEO : %d\n",TrgTKFGeo[iTKF]);
					Push2ErrStack(PPCERROR+TRG_FLG+ATOM_ERR+124,0,0,0,0);		/*trg error 124*/
					return -1;
				}
		}

		ReadFrmVme32(0x108+(TrgTKFBSEL[iTKF]<<19), VA24, &uReadTmp);//read for fiber init
		printf("TRG TKF 0x%x prep dataselect=%x\n ",(0x108+(TrgTKFBSEL[iTKF]<<19)), uReadTmp);
		
	}
	return 0;
}
/*___________________________________TOF_______*/
int TRG_TOF_conf()
{
	unsigned uChnlShield1;
	unsigned uChnlShield2;
	unsigned uChnlMask;
	unsigned uOffset;
	unsigned int uReadTmp;
	FILE *fp;
	UINT8 rbfdata;

	ReadFrmVme32( 0x274+(TrgTOFBSEL<<19), VA32,&uReadTmp);
	printf("TOFT conf uReadTmp=%x\n ",uReadTmp);
	if((uReadTmp&0x8)!=0x8)
	{
		//FPGA configure
		printf("open the file toftrigger.rbf.\n");
		WriteToVme32(0x250+(TrgTOFBSEL<<19), VA32,0x5);//which FPGA to configure

		if((fp=fopen("./trigger/toftrigger.rbf","rb"))==NULL)
	    {
	        printf("Cannot open the file toftrigger.rbf.\n");
			Push2ErrStack(PPCFATAL+TRG_FLG+ATOM_ERR+102,0,0,0,0);		/*trg error 102*/
			return -1;
	    }

        while(!feof(fp))
        {
        	if(fread(&rbfdata,sizeof(UINT8),1,fp)!=1)
        	{
            	//printf("error !while reading the file!\n");
				if (!feof(fp))
				{
					fclose(fp);
					printf("toftrigger.rbf:error!");
					Push2ErrStack(PPCFATAL+TRG_FLG+ATOM_ERR+103,0,0,0,0);		/*trg error 103*/
                	return -1;
                }
            }
			if(-1==WriteToVme32(0x274+(TrgTOFBSEL<<19), VA32,rbfdata))
			{
				printf("toftrigger.rbf:write TOF FPGA VME32 error\n");
				Push2ErrStack(PPCFATAL+TRG_FLG+ATOM_ERR+104,0,0,0,0);		/*trg error 104*/
				return -1;
			}
	    }

		fclose(fp);

		ReadFrmVme32( 0x274+(TrgTOFBSEL<<19), VA32,&uReadTmp);
		printf("uReadTmp=%x\n ",uReadTmp);
		if(uReadTmp&0x8!=0x8)
		{
			printf("updata TOFT FPGA fail!\n");
			Push2ErrStack(PPCFATAL+TRG_FLG+ATOM_ERR+105,0,0,0,0);		/*trg error 105*/
			return -1;
		}
	    printf("********Process data from file to module DONE!**********\n");		
	}

	if(-1==WrtRdA32D32(0x250+(TrgTOFBSEL<<19),0xcc07,"TOFT_Reset",0xffff)) 
		return -1;
	WriteToVme32(0x254+(TrgTOFBSEL<<19), VA32,g_TRGDBInfo.trg_tof_header->WorkMode);
	uChnlShield1=g_TRGDBInfo.trg_tof_header->FiberChannelShield1;
	uChnlShield2=g_TRGDBInfo.trg_tof_header->FiberChannelShield2;
	uChnlMask=0x1;
	uOffset=0;

//add fiber channel choose here

	int FailChannel[6]={0,0,0,0,0,0};
	int BakChannel[6]={0,0,0,0,0,0};

	
	BakChannel[0] =( (g_TRGDBInfo.trg_tof_header->SubstituteChannel1)>>8) & 0xff ;
	BakChannel[1] =( (g_TRGDBInfo.trg_tof_header->SubstituteChannel1)>>24) & 0xff ;
	BakChannel[2] =( (g_TRGDBInfo.trg_tof_header->SubstituteChannel2)>>8) & 0xff ;
	BakChannel[3] =( (g_TRGDBInfo.trg_tof_header->SubstituteChannel2)>>24) & 0xff ;
	BakChannel[4] =( (g_TRGDBInfo.trg_tof_header->SubstituteChannel3)>>8) & 0xff ;
	BakChannel[5] =( (g_TRGDBInfo.trg_tof_header->SubstituteChannel3)>>24) & 0xff ;

	
	FailChannel[0] =( (g_TRGDBInfo.trg_tof_header->SubstituteChannel1)>>0) & 0xff ;
	FailChannel[1] =( (g_TRGDBInfo.trg_tof_header->SubstituteChannel1)>>16) & 0xff ;
	FailChannel[2] =( (g_TRGDBInfo.trg_tof_header->SubstituteChannel2)>>0) & 0xff ;
	FailChannel[3] =( (g_TRGDBInfo.trg_tof_header->SubstituteChannel2)>>16) & 0xff ;
	FailChannel[4] =( (g_TRGDBInfo.trg_tof_header->SubstituteChannel3)>>0) & 0xff ;
	FailChannel[5] =( (g_TRGDBInfo.trg_tof_header->SubstituteChannel3)>>16) & 0xff ;
	
	
	for(int ich=0;ich<6;ich++)
	{
		if( (FailChannel[ich]) != 0xff)
		{
			WriteToVme32(0x254+(TrgTOFBSEL<<19), VA32, (0xff090000+(BakChannel[ich])+(FailChannel[ich]<<8)));
			printf("Attention: substitution here ! ... BakChannel id : %d , while FailChannel id : %d , value : 0x%x\n",BakChannel[ich] , FailChannel[ich],(0xff090000+(BakChannel[ich])+(FailChannel[ich]<<8)));
		}
	}
/*
	if(FailChannel[0])
		WriteToVme32(0x254+(TrgTOFBSEL<<19), VA32, (0xff090020+(FailChannel[0]<<8)));
	if(FailChannel[1])
		WriteToVme32(0x254+(TrgTOFBSEL<<19), VA32, (0xff09001f+(FailChannel[1]<<8)));	
	if(FailChannel[2])
		WriteToVme32(0x254+(TrgTOFBSEL<<19), VA32, (0xff09001b+(FailChannel[2]<<8)));
	if(FailChannel[3])
		WriteToVme32(0x254+(TrgTOFBSEL<<19), VA32, (0xff090002+(FailChannel[3]<<8)));
	if(FailChannel[4])
		WriteToVme32(0x254+(TrgTOFBSEL<<19), VA32, (0xff090001+(FailChannel[4]<<8)));
	if(FailChannel[5])
		WriteToVme32(0x254+(TrgTOFBSEL<<19), VA32, (0xff090000+(FailChannel[5]<<8)));
*/


		
	//tmp
/*
	WriteToVme32(0x254+(TrgTOFBSEL<<19), VA32,0xff10005f);
	WriteToVme32(0x254+(TrgTOFBSEL<<19), VA32,0xff11005f);
	WriteToVme32(0x254+(TrgTOFBSEL<<19), VA32,0xff12005f);
*/
	//add 091117
	for(int idx=0;idx<88;idx++)
		WriteToVme32(0x254+(TrgTOFBSEL<<19), VA32,0xff330000+idx+1);
	
	for(idx=0;idx<34;idx++)
	{
		if(idx<32)
		{
			if((uChnlShield1&uChnlMask)==uChnlMask)
			{
				WriteToVme32(0x254+(TrgTOFBSEL<<19), VA32,0xff1000af+uOffset);//af no mask
				//printf("no mask channel : 0x%x \n",0xff1000af+uOffset);
			}
			else
			{
				WriteToVme32(0x254+(TrgTOFBSEL<<19), VA32,0xff10005f+uOffset);//5f mask
				printf("mask channel : 0x%x \n",0xff10005f+uOffset);
			}
			uChnlMask=uChnlMask<<1;
			if( idx==31 )
				uChnlMask=0x1;
		}
		else
		{
			
			if((uChnlShield2&uChnlMask)==uChnlMask)
			{
				WriteToVme32(0x254+(TrgTOFBSEL<<19), VA32,0xff1000af+uOffset);//af no mask
				//printf("no mask channel : 0x%x \n",0xff1000af+uOffset);
			}
			else
			{
				WriteToVme32(0x254+(TrgTOFBSEL<<19), VA32,0xff10005f+uOffset);//5f mask
				printf("mask channel : 0x%x \n",0xff10005f+uOffset);
			}
			uChnlMask=uChnlMask<<1;
		}
		uOffset+=0x10000;
	}

	
	
	if(-1==WrtRdA32D32(0x230+(TrgTOFBSEL<<19),g_TRGDBInfo.trg_tof_header->ControlParameter,"TOFT_ControlParameter",0xffffffff)) 
		return -1;
	if(-1==WrtRdA32D32(0x234+(TrgTOFBSEL<<19),(g_nTrgPerInt<<26)+(g_TRGDBInfo.trg_tof_header->DisposeParameter&0x3ffffff),"TOFT_DisposeParameter",0xffffffff) )
		return -1;
	//as trg step 6
	WriteToVme32(0x254+(TrgTOFBSEL<<19), VA32,g_TRGDBInfo.trg_tof_header->PhysAddress);
	//as trg ,step 7
	//if(norm mode)
	{
		if(-1==WrtRdA32D32(0x254+(TrgTOFBSEL<<19),0xff060055,"TOFT_FiberSyn",0xffffffff))
			return -1;
	}
	//else online test mode
	//{
	//	WriteToVme32(0x00fd0254, VA32,0xff220000);
	//	WriteToVme32(0x00fd0258, VA32,0x00001a01);
	//	WriteToVme32(0x00fd0254, VA32,0xff2200aa);
	//}
	WriteToVme8(0x7ffb+(TrgTOFGeo<<19), VA24,g_TRGDBInfo.trg_tof_header->CR_CSR);
	return 0;
}

int TRG_TOF_syn3() 
{
	WriteToVme32(0x254+(TrgTOFBSEL<<19), VA32,0xff0800aa);//clean Delay FIFO
}

int TRG_TOF_prep()
{
//	WriteToVme32(0x250+(TrgTOFBSEL<<19), VA32,0xcc07);//
	WriteToVme32(0x254+(TrgTOFBSEL<<19), VA32,0xff0800aa);//clean Delay FIFO
	
	//start process
	if(TrgTOFCBLT==1)
	{
		if(-1==WrtRdA32D32(0x254+(TrgTOFBSEL<<19),0xFF040055,"TOFT_StartProcess",0xffffffff))
			return -1;
	}

	WriteToVme32(0x25c+(TrgTOFBSEL<<19), VA32,0xff040000);
	unsigned int uReadTmp;	
	ReadFrmVme32( 0x21c+(TrgTOFBSEL<<19), VA32,&uReadTmp);
	printf("TRG TRG_TOF_prep fuerr state: %x\n ",uReadTmp);	
	if((uReadTmp&0x200)==0x200)
		printf("TRG TOFT Fiber Synchronization OK!\n");
	else
	{
		WriteToVme32(0x25c+(TrgTOFBSEL<<19), VA32,0xff020000);
		ReadFrmVme32( 0x214+(TrgTOFBSEL<<19), VA32,&uReadTmp);
		printf("TRG TRG_TOF_prep SR2 Data: %x\n ",uReadTmp);	
		
		WriteToVme32(0x25c+(TrgTOFBSEL<<19), VA32,0xff030000);
		ReadFrmVme32( 0x218+(TrgTOFBSEL<<19), VA32,&uReadTmp);
		printf("TRG TRG_TOF_prep SR3 Data: %x\n ",uReadTmp);	
		

		WriteToVme32(0x25c+(TrgTOFBSEL<<19), VA32,0xff040000);
		ReadFrmVme32( 0x21c+(TrgTOFBSEL<<19), VA32,&uReadTmp);
		printf("TRG TRG_TOF_prep SR4 Data: %x\n ",uReadTmp);	
		if((uReadTmp&0x40000)==0x0)
			printf("TRG TOFT Aligned Status OK!\n");
		else
		{
			if(TrgTOFSYNCheck)
			{
				printf("TRG TOFT Aligned Status Error!\n");
				Push2ErrStack(PPCERROR+TRG_FLG+ATOM_ERR+125,0,0,0,0);		/*trg error 125*/
				return -1;
			}
		}




		WriteToVme32(0x25c+(TrgTOFBSEL<<19), VA32,0xff050000);
		ReadFrmVme32( 0x220+(TrgTOFBSEL<<19), VA32,&uReadTmp);
		printf("TRG TRG_TOF_prep BTOFT SR Data5: %x\n ",uReadTmp);	
		
		WriteToVme32(0x25c+(TrgTOFBSEL<<19), VA32,0xff060000);
		ReadFrmVme32( 0x224+(TrgTOFBSEL<<19), VA32,&uReadTmp);
		printf("TRG TRG_TOF_prep BTOFT SR Data6: %x\n ",uReadTmp);	
	
	}


	return 0;
}

int TRG_ETF_conf()
{

	//unsigned int etft_readout_window ;
	unsigned int etft_L1_selection ;
	unsigned int etft_L1_Delay ;
	unsigned int etft_data_mode ; 	
	unsigned int etft_fiber_chnSet;

	//20160517 disable channel test
	unsigned int etft_Disable_AddrSet;
	unsigned int etft_Disable_DataSet;

	/**get temp config paras**/
	char comment[1000];
	int opt[10]={0};
	int iopt=0;
	FILE* fOpts;
	int icnt = 0;
	int nRetryTimes = 0;
	while(1)
	{
		if ((fOpts = fopen("./trigger/ETFTriggerParas.txt","r")) == NULL)
		{
			icnt++;
			taskDelay(60);
			if(icnt%5==0)
			{
				printf("open file ETFTriggerParas.txt failed %d times\n",icnt);
				/*break;*/
			}
		}
		else
		{
			printf("open file ETFTriggerParas.txt succ\n");
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
	
	etft_readout_window = opt[0];
	etft_L1_selection = opt[1];
	etft_L1_Delay = opt[2];
	etft_data_mode = opt[3]; 	
	etft_fiber_chnSet = opt[4];
	etft_Disable_AddrSet = opt[5];
	etft_Disable_DataSet = opt[6];
	
	printf("etft_readout_window is set to 0x%x \n",etft_readout_window);
	printf("etft_L1_selection is set to 0x%x \n",etft_L1_selection);
	printf("etft_L1_Delay is set to 0x%x \n",etft_L1_Delay);
	printf("etft_data_mode is set to 0x%x \n",etft_data_mode);
	printf("etft_fiber_chnSet is set to 0x%x \n",etft_fiber_chnSet);
	printf("etft_Disable_AddrSet is set to 0x%x \n",etft_Disable_AddrSet);
	printf("etft_Disable_DataSet is set to 0x%x \n",etft_Disable_DataSet);



	printf("ETF trigger module configured here \n");
	int map_data[18]={0x02010023, 0x06050403, 0x0a090807, 0x0e0d0c0b, 0x1211100f, 0x16151413,
						0x1a191817, 0x1e1d1c1b, 0x2221201f, 0x27262524, 0x2b2a2928, 0x2f2e2d2c,
						0x33323130, 0x37363534, 0x3b3a3938, 0x3f3e3d3c, 0x43424140, 0x47464544};

	WriteToVme32(0x10c+(TrgETFBSEL<<19), VA24, 0x0);	/*Global reset*/
	taskDelay(1);
	
	if(-1==WrtRdA24D32(0x4+(TrgETFBSEL<<19),0x82,"ETFT CBLT Chain",0xff)) //a2=first,82=mid,8a=last, aa=first&last,00=not in CBLT chain
		return -1;

	if(-1==WrtRdA24D32(0x38+(TrgETFBSEL<<19),etft_fiber_chnSet,"ETFT fiber channel set",0xffffffff)) 
		return -1;
	
	if(-1==WrtRdA24D32(0x160+(TrgETFBSEL<<19),0xffff3fff,"ETFT MRPC trigger channel disable",0xffffffff)) 
		return -1;

	printf("ETFT MRPC trigger channel disable: set data 0x%x to addr 0x%x \n",etft_Disable_DataSet,etft_Disable_AddrSet);
	if(-1==WrtRdA24D32(etft_Disable_AddrSet +(TrgETFBSEL<<19),etft_Disable_DataSet,"ETFT MRPC trigger channel disable",0xffffffff))//west 35-3 
		return -1;

	if(-1==WrtRdA24D32(0x8+(TrgETFBSEL<<19),0x0,"ETFT nEventperCBLT",0xffffffff)) 
	return -1;

	if(-1==WrtRdA24D32(0xc+(TrgETFBSEL<<19),etft_readout_window,"ETFT readout window",0xffffffff)) 
	return -1;	

	//set GEO ADDR
	if(-1==WrtRdA24D32(0x94+(TrgETFBSEL<<19),TrgETFGeo,"ETFT GEO ADDR SET",0x1f)) 
	return -1;	

	if(-1==WrtRdA24D32(0x18+(TrgETFBSEL<<19),etft_L1_selection,"ETFT L1 select",0xffffffff)) //0=outside,1=local
	return -1;

	if(-1==WrtRdA24D32(0x24+(TrgETFBSEL<<19),etft_L1_Delay,"ETFT L1 delay",0xffffffff)) 
	return -1;

	//WriteToVme32(0xfc+(TrgETFBSEL<<19), VA24, 0x0);	/*Global reset*/

	if(-1==WrtRdA24D32(0x28+(TrgETFBSEL<<19),etft_data_mode,"ETFT data mode select",0xffffffff)) //0=run,1=sim hit data test
	return -1;	

	/*map*/
	int index=0;
	for(unsigned int addr_map = 0x110; addr_map<=0x154;addr_map=addr_map+0x4 )
	{
		if(-1==WrtRdA24D32(addr_map+(TrgETFBSEL<<19),map_data[index],"ETFT map setting",0xffffffff)) 
			return -1;	
		index++;
	}

	if(-1==WrtRdA24D32(0x158+(TrgETFBSEL<<19),0x1,"ETFT hitMap reload",0xffffffff)) 
	return -1;	


	return 0;
}

int TRG_ETF_syn1()
{

	//change at 20170109 required by zhaojz
/*
	if(-1==TRG_ETF_conf() )
	{
		DEBUG_OUTPUT(("config TRG_ETF error.\n"));
		Push2ErrStack(PPCFATAL+TRG_FLG+ATOM_ERR+127,0,0,0,0);		
		return -1;
	}
	taskDelay(10);
	*/
	
	WriteToVme32(0xfc+(TrgETFBSEL<<19), VA24, 0x0);	/*partial reset*/
	taskDelay(1);

	//add on 2016-12-30 request by Jingzhou Zhao
	//WriteToVme32(0xf8+(TrgETFBSEL<<19), VA24, 0x0);	/*MGT FIFO clean*/
	//taskDelay(1);

	return 0;
}

int TRG_ETF_prep()
{
	
	
	unsigned int uReadTmp;	
	ReadFrmVme32( 0x3c+(TrgETFBSEL<<19), VA24,&uReadTmp);
	printf("TRG ETF Fiber alignment status: %x\n ",uReadTmp);	
	if(uReadTmp!=0x3ff)
	{
		printf("TRG ETF Aligned Status Error!\n");
		Push2ErrStack(PPCERROR+TRG_FLG+ATOM_ERR+126,0,0,0,0);		/*trg error 126*/
		return -1;
	}
	
	return 0;
}

int TRG_BASG_conf()
{
	unsigned int uWriteAddr;

	if(-1==WrtRdA24D32(0xc0 +(TrgBASGBSEL<<19),0,"BASG UserCSR",0xffffffff) ) return -1;
	//system reset
	WriteToVme32(0xfc+(TrgBASGBSEL<<19), VA24, 0x0);//data?
	
	uWriteAddr=(TrgBASGBSEL<<19);
	for(int ibx=0;ibx<48;ibx++)
	{
		if(-1==WrtRdA24D32(uWriteAddr,g_TRGDBInfo.trg_basg_header->ParSetChannel[ibx],"BASG ParSetChannel",0xffffffff) ) return -1;
		uWriteAddr+=4;
	}



	if(-1==WrtRdA24D32(0xc4 +(TrgBASGBSEL<<19),g_TRGDBInfo.trg_basg_header->ChannelMaskCSR01,"BASG ChannelMaskCSR01",0xffffffff) ) return -1;
	
	if(-1==WrtRdA24D32(0xc8 +(TrgBASGBSEL<<19),g_TRGDBInfo.trg_basg_header->ChannelMaskCSR02,"BASG ChannelMaskCSR02",0xffffffff) ) return -1;

	if(-1==WrtRdA24D32(0xcc +(TrgBASGBSEL<<19),g_TRGDBInfo.trg_basg_header->BASWidth,"BASG BASWidth",0xffffffff) ) return -1;

	if(-1==WrtRdA24D32(0xd0+(TrgBASGBSEL<<19),g_TRGDBInfo.trg_basg_header->ChannelSelForLight,"BASG ChannelSelForLight",0xffffffff) ) return -1;

	uWriteAddr=(TrgBASGBSEL<<19)+0xd4;
	for( ibx=0;ibx<9;ibx++)
	{
		if(-1==WrtRdA24D32(uWriteAddr,g_TRGDBInfo.trg_basg_header->SetBackUp[ibx],"BASG SetBackUp",0xffffffff) ) return -1;
		uWriteAddr+=4;
	}

	
}

int TRG_BASG_prep()
{
	if(-1==WrtRdA24D32(0xc0 +(TrgBASGBSEL<<19),g_TRGDBInfo.trg_basg_header->UserCSR,"BASG UserCSR",0xffffffff) ) return -1;
	//system reset
	WriteToVme32(0xfc+(TrgBASGBSEL<<19), VA24, 0x0);
	taskDelay(1);
	WriteToVme32(0xf8+(TrgBASGBSEL<<19), VA24, 0x0);
	
}


	
int TRG_BASG_sptr()
{
	if(-1==WrtRdA24D32(0xc0 +(TrgBASGBSEL<<19),0,"BASG UserCSR",0xffffffff) ) return -1;
	
}

int TRG_BTM_syn3()
{	
	printf("TRG_BTM_syn3 addr=0x%x,data=0x%x\n",0x1000+(TrgBTMBSEL<<19),g_TRGDBInfo.trg_btm_header->CBLTSet);
	printf("TRG_BTM_syn3 addr=0x%x,data=0x%x\n",0x2000+(TrgBTMBSEL<<19),g_TRGDBInfo.trg_btm_header->L1Select);
	WriteToVme32(0x7000+(TrgBTMBSEL<<19), VA32,0x0);//reset
	WriteToVme32(0x1000+(TrgBTMBSEL<<19), VA32,(( g_TRGDBInfo.trg_btm_header->CBLTSet)&0xfffff0ff)|((g_nTrgPerInt-1)<<8));//CBLTSet
	WriteToVme32(0x2000+(TrgBTMBSEL<<19), VA32, g_TRGDBInfo.trg_btm_header->L1Select);//L1Select
	
	return 0;
}
int TRG_BTM_prep()
{
	WriteToVme32(0x7000+(TrgBTMBSEL<<19), VA32,0x0);//reset
	return 0;
}
int TRG_ETM_syn3()
{	
	printf("TRG_ETM_syn3 addr=0x%x,data=0x%x\n",0x1000+(TrgETMBSEL<<19),g_TRGDBInfo.trg_etm_header->CBLTSet);
	printf("TRG_ETM_syn3 addr=0x%x,data=0x%x\n",0x2000+(TrgETMBSEL<<19),g_TRGDBInfo.trg_etm_header->L1Select);
	WriteToVme32(0x7000+(TrgETMBSEL<<19), VA32,0x0);//reset
	WriteToVme32(0x1000+(TrgETMBSEL<<19), VA32, (( g_TRGDBInfo.trg_etm_header->CBLTSet)&0xfffff0ff)|((g_nTrgPerInt-1)<<8));//CBLTSet
	WriteToVme32(0x2000+(TrgETMBSEL<<19), VA32, g_TRGDBInfo.trg_etm_header->L1Select);//L1Select
	
	return 0;
}
int TRG_ETM_prep()
{
	WriteToVme32(0x7000+(TrgETMBSEL<<19), VA32,0x0);//reset
	return 0;
}


int TRG_ETS_prep()
{
//	WriteToVme32(0X780008, VA32,0x1);//reset
	WriteToVme32(0x8+(TrgETSBSEL<<19),VA24,0x1);//reset
	return 0;	
}

//for test check 
//input TRIGTestCheck1 then input TRIGTestCheck2
void TRIGTestCheck1() 
{
	WriteToVme16((TrgFCTLBSEL<<19), VA24, 0x5);
	taskDelay(1);
}


void TRIGTestCheck2()
{
	WriteToVme16((TrgFCTLBSEL<<19), VA24, 0x0);
	taskDelay(1);
}
