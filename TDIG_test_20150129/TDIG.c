#include <time.h>
#include <stdio.h>
#include <stdlib.h>
#include "5100_test.h"
#include "interrupt.h"
#include "TDIG.h"
#include "ROC.h"

#define	MCST_ADRS	0x3000000
#define CBLTADDR 0x02000000
#define MAXTRANSBYTES 1000000
#define nTrgPerInt 1

unsigned int CBLTDataBuffer[MAXTRANSBYTES] ;

int windowSet ;
int trigger_latency ;
int trigger_source;
int eventperCBLT;
int TDIG_interrupt_enable;
int ROC_interrupt_enable;
int ROC_CBLT_enable;
int P_number;
int event_saveNumber;

int errDataEnb;
int boardEnb;
int CBLTEnb;
int slot_board1;
int slot_board2;
int nCbltModuleNum = 0;
int g_uEtfCbltGeo[2];

/*unsigned int base_addr;*/

void dly(int ms)
{
        int it;
        double a1,a2,a3;
        for (it=0;it<=20000*ms;it++)
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
        for (i=0;i<=20*us;i++)
        {
                a1=a1+(1e-8);
                a2=a2-(1e-8);
                a3=a3+a1*a2;
        }
}

int configCBLTChain(unsigned int nGeo,int position)/*1 first, 2 middle, 3 last, 4 only*/
{
	unsigned int nadr = (nGeo<<19)+0x70043 ;
	if(position==1)
	{
		if ( -1 == WriteToVme8(nadr, VCSR, 0xa6 ) ) 
		{
			printf("Write GEO %d chain First Failed \n",nGeo); 
			return -1;
		}
		else
		{
			printf("first GEO Chain %d\n",nGeo);
		}
	}
	else if(position==3)
	{
		if ( -1 == WriteToVme8(nadr, VCSR, 0x9a ) ) 
		{
			printf("Write GEO %d chain Last Failed \n",nGeo); 
			return -1;
		}
		else
		{
			printf("last GEO Chain %d\n",nGeo);
		}
	}
	else if(position==4)
	{
		if ( -1 == WriteToVme8(nadr, VCSR, 0xaa ) ) 
		{
			printf("Write GEO %d chain only Failed \n",nGeo); 
			return -1;
		}
		else
		{
			printf("only GEO Chain %d\n",nGeo);
		}
	}		
	else if(position==2)
	{
		if ( -1 == WriteToVme8(nadr, VCSR, 0x96 ) ) 
		{
			printf("Write GEO %d chain middle Failed\n",nGeo); 
			return -1;				
		}
		else
		{
			printf("mid GEO Chain %d\n",nGeo);
		}
	}
	else
	{
		printf("module position error\n");
		return -1;
	}

	taskDelay(6);
	return 0;
}

int TOFConfigFeeBaseAddr(unsigned int nGeo)
{
	WriteToVme8((nGeo<<19)+0x7ffd8+0x3, VCSR, (nGeo&0x0f)<<4);  /*nGeo low 4 bit write to one register*/
	WriteToVme8((nGeo<<19)+0x7ffdc+0x3, VCSR, (nGeo&0xf0)>>4);	/*nGeo high 4 bit write to another register*/
	printf("write:0x%08x to address: 0x%08x\n",nGeo<<4,(nGeo<<19)+0x7ffdc+0x3);
	return 0;
}

int resetCPLD(unsigned int base_addr)
{

	if( -1==WriteToVme32(base_addr + 0x800cc,VA32, 5) )
	{
		printf("error while write to VME during resetCPLD\n");
		return -1;
	}
	printf("write data 0x%x to addr 0x%x succ \n",5,base_addr + 0x800cc);
	taskDelay(1);
	return 0;
	
}



int configFPGA(unsigned int base_addr)
{

	if( -1==WriteToVme32(base_addr + 0x80008,VA32, 6) )
	{
		printf("error while write to VME during configFPGA\n");
		return -1;
	}
	printf("write data 0x%x to addr 0x%x succ \n",6,base_addr + 0x80008);
	printf(" configFPGA SUCC\n");
	taskDelay(6);
	return 0;
	
}


int configINL(char *fname,unsigned int base_addr)
{
	FILE *fp_inl;
	UINT32 check_data = 0;


	unsigned int data = 0;
	unsigned int addr = 0;

	int i_cnt = 0;
	int err_cnt = 0;
	
	if ((fp_inl = fopen(fname,"r")) == NULL)
	{
		printf("cannot open file %s \n", fname);
		return -1;
	}
	else
	{
		printf("open file %s succ\n", fname);
	}

	fscanf(fp_inl,"%x %x",&data,&addr);
	while(!feof(fp_inl))
	{
		/*printf("data = 0x%x, addr = 0x%x\n",data,addr);*/
		/*write to VME*/
		
		if( -1==WriteToVme32(base_addr + addr,VA32, data) )
		{
			printf("error while write to VME during configINL\n");
			return -1;
		}
		/*taskDelay(1);*/
/*
		ReadFrmVme32(base_addr + addr ,VA32,&check_data);
		if(check_data!=data)
		{
			err_cnt++;
			printf("*****write data:0x%x to addr:0x%x failed..\n",data,base_addr + addr);
			printf("TDIG_configINL error!line %d : set value = 0x%x, while get value = 0x%x\n",i_cnt,data,check_data);
		}
		taskDelay(1);
		i_cnt++;		
*/		
		fscanf(fp_inl,"%x %x",&data,&addr);
		
	}
	
/*	printf("total error lines in this file: %d \n",err_cnt);*/
	taskDelay(1);
	fclose(fp_inl);
	return 0;

}

int TDIG_readcheckINL(char *fname,unsigned int base_addr)
{
	FILE *fp_inl;
	UINT32 check_data = 0;

	unsigned int data = 0;
	unsigned int addr = 0;
	int i_cnt = 0;
	int err_cnt = 0;
	
	if ((fp_inl = fopen(fname,"r")) == NULL)
	{
		printf("cannot open file %s \n", fname);
		return -1;
	}
	else
	{
		printf("open file %s succ\n", fname);
	}

	fscanf(fp_inl,"%x %x",&data,&addr);
	while(!feof(fp_inl))
	{
		ReadFrmVme32(base_addr + addr ,VA32,&check_data);
		if(check_data!=data)
		{
			err_cnt++;
			printf("write data:0x%x to addr:0x%x failed..\n",data,base_addr + addr);
			printf("TDIG_readcheckINL error!line %d : set value = 0x%x, while get value = 0x%x\n",i_cnt,data,check_data);
		}
		
		i_cnt++;
		fscanf(fp_inl,"%x %x",&data,&addr);
		
	}

	printf("total error lines : %d \n",err_cnt);

	fclose(fp_inl);
	
	return 0;

}

int configHPTDC(unsigned int base_addr)
{
	FILE *fp_data;
	FILE *fp_addr;
	unsigned int data = 0;
	unsigned int addr = 0;

	int i_cnt = 0;
	UINT32 check_data = 0;

   
	if ((fp_data = fopen("HPTDCdata.txt","r")) == NULL)
	{
		printf("cannot open file HPTDCdata.txt\n");
		return -1;
	}
	else
	{
		printf("open file HPTDCdata.txt succ\n");
	}	

	if ((fp_addr = fopen("HPTDCaddr.txt","r")) == NULL)
	{
		printf("cannot open file HPTDCaddr.txt\n");
		return -1;
	}
	else
	{
		printf("open file HPTDCaddr.txt succ\n");
	}

	fscanf(fp_addr,"%x",&addr);
	fscanf(fp_data,"%x",&data);
	i_cnt ++;
	while(!feof(fp_addr))
	{
		/*printf("data = 0x%x, addr = 0x%x\n",data,addr);*/
		/*
		switch(i_cnt)
		{
			case 2:
				data = ((data&0x0000ffff) + ((reject_cnt_offset&0xfff)<<16)+((search_window&0xf)<<28));
				printf("after : data = 0x%x, addr = 0x%x\n",data,addr);
				break;
			case 3:
				data = ((data&0xfff00000) + ((search_window>>4)&0xff)+((match_window&0xfff)<<8));
				printf("after : data = 0x%x, addr = 0x%x\n",data,addr);
				break;
			case 5:
				data =( (data&0xffc003ff) + ((trigger_latency&0xfff)<<10));
				printf("after : data = 0x%x, addr = 0x%x\n",data,addr);
				break;
			default: break;
		} */
		
		/*write to VME*/
		
		if( -1==WriteToVme32( base_addr + addr,VA32, data) )
		{
			printf("error while write to VME during configHPTDC\n");
			return -1;
		}
		
		dly(8);
		
		/*
		ReadFrmVme32(addr,VA32,&check_data);
		if(check_data!=data)
		{
			printf("config HPTDC error !id: %d ,write data is 0x%x ,while read data is 0x%x \n",i_cnt,data,check_data);
			return -1;
		}*/
		
		fscanf(fp_addr,"%x",&addr);
		fscanf(fp_data,"%x",&data);
		i_cnt ++;
		
		
	}	

	taskDelay(6);
	fclose(fp_data);
	fclose(fp_addr);
	return 0;

}


int readcheckHPTDC(unsigned int base_addr)
{

	FILE *fp_data;
	FILE *fp_addr;
	unsigned int data = 0;
	unsigned int addr = 0;

	int i_cnt = 0;

	UINT32 check_data = 0;

   
	if ((fp_data = fopen("HPTDCdata.txt","r")) == NULL)
	{
		printf("cannot open file HPTDCdata.txt\n");
		return -1;
	}
	else
	{
		printf("open file HPTDCdata.txt succ\n");
	}	

	if ((fp_addr = fopen("HPTDCaddr.txt","r")) == NULL)
	{
		printf("cannot open file HPTDCaddr.txt\n");
		return -1;
	}
	else
	{
		printf("open file HPTDCaddr.txt succ\n");
	}

	fscanf(fp_addr,"%x",&addr);
	fscanf(fp_data,"%x",&data);
	i_cnt ++;
	while(!feof(fp_addr))
	{
		/*printf("data = 0x%x, addr = 0x%x\n",data,addr);*/
		/* 
		switch(i_cnt)
		{
			case 2:
				data = ((data&0x0000ffff) + ((reject_cnt_offset&0xfff)<<16)+((search_window&0xf)<<28));
				printf("after : data = 0x%x, addr = 0x%x\n",data,addr);
				break;
			case 3:
				data = ((data&0xfff00000) + ((search_window>>4)&0xff)+((match_window&0xfff)<<8));
				printf("after : data = 0x%x, addr = 0x%x\n",data,addr);
				break;
			case 5:
				data =( (data&0xffc003ff) + ((trigger_latency&0xfff)<<10));
				printf("after : data = 0x%x, addr = 0x%x\n",data,addr);
				break;
			default: break;
		}*/

		ReadFrmVme32(base_addr + addr,VA32,&check_data);
		if(check_data!=data)
		{
			printf("config HPTDC error !id: %d\n ,write data is 0x%x ,while read data is 0x%x \n",i_cnt,data,check_data);
			/*return -1;*/
		}
		
		fscanf(fp_addr,"%x",&addr);
		fscanf(fp_data,"%x",&data);
		i_cnt ++;
		
		
	}	

	taskDelay(1);
	fclose(fp_data);
	fclose(fp_addr);	
	return 0;

}


int HPTDCenable(unsigned int base_addr)
{
	if( -1== WriteToVme32(base_addr + 0x0,VA32, 0x20) )
	{
		printf("error while write to VME during HPTDCenable\n");
		return -1;
	}

	taskDelay(6);
	return 0;
}

int INLenable(unsigned int base_addr)
{
	if( -1== WriteToVme32(base_addr + 0x3c,VA32, 0x1) )
	{
		printf("error while write to VME during INLenable\n");
		return -1;
	}

	taskDelay(6);
	return 0;
}


int configDataSet(unsigned int base_addr)
{
	if( -1==WriteToVme32(base_addr +0x38,VA32, 1) )
	{
		printf("error while write to VME during configDataSet 1\n");
		return -1;
	}
	taskDelay(6);
	if( -1==WriteToVme32(base_addr +0x3c,VA32, 1) )
	{
		printf("error while write to VME during configDataSet 2\n");
		return -1;
	}	
	taskDelay(6);
	return 0;
}

int configReadoutEvent(unsigned int base_addr)
{
	unsigned int data;
	data = 0x80000000+nTrgPerInt;/*bit [4:0]*/
	if( -1==WriteToVme32(base_addr +0xc,VA32, data) )
	{
		printf("error while write to VME during HPTDCopen 1\n");
		return -1;
	}
	taskDelay(6);
}



int ModifyParas(unsigned int base_addr)
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


int setInterrupt(unsigned int base_addr)
{
	if( -1==WriteToVme32(base_addr +0x48,VA32, 0x0) )
	{
		printf("error while write to VME during enableModify\n");
		return -1;
	}

	taskDelay(10);

	return 0;
}

int triggerSourceSelect(unsigned int base_addr)/*front 1; back 0*/
{
	int data;
	if (trigger_source==1)
		data = 0x20;
	else
		data = 0x4020;

	if( -1==WriteToVme32(base_addr +0x0,VA32, data) )
	{
		printf("error while write to VME during triggerSourceSelect\n");
		return -1;
	}
	printf("step 4 : write data 0x%x to addr 0x%x \n", data,base_addr +0x0);
	taskDelay(1);

	return 0;
	
}

int CBLTParasSet(unsigned int base_addr)
{
	int data;
	data = (TDIG_interrupt_enable<<6) + (errDataEnb<<5) + eventperCBLT;
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
	taskDelay(60);

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

int ETFClearFEE()
{
	WriteToVme32(0xbc0270, VA32, 0x24);	/*clear FEE FIFO*/
	taskDelay(1);
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

int TDIG_config(unsigned int base_addr)
{
	resetCPLD(base_addr);
	taskDelay(120);

	/*modify window and latency of HPTDC*/
	if (-1==ModifyParas(base_addr))
		return -1;


	/*enable the modify*/
	if (-1==enableModify(base_addr))
		return -1;

	/*choose trigger source */
	if (-1==triggerSourceSelect(base_addr))
		return -1;

	/*set CBLT register*/
	if (-1==CBLTParasSet(base_addr))
		return -1;

/*	setInterrupt(base_addr);*/

	/*HPTDC OPEN*/
	if (-1==HPTDCopen(base_addr))
		return -1;



		

	return 0;
}

void printData(int uDataLength,int triggerId)
{
	int i=0;
	
	printf ("OUT --> cblt size: %d, id=%d\n", uDataLength, triggerId) ;

	for (i=0; i<(uDataLength>>2); i++)
	{
		printf ("%#x\t", CBLTDataBuffer[i]) ;
		if ((i+1) % 4 == 0) 
			printf ("\n") ;
	}
	printf ("\n") ;
}



int TDIG_DataCheck(int uDataLength,int triggerId)
{
	int datum=0;
	UINT* p_uMove;
	int nModuIndex=0;
	int uDataTemp;

	if (uDataLength==0)
	{
		printf("CBLT size ==0 \n");
		return -1;
	}

	if((uDataLength%4)!=0)
	{
		printf("total length can not divid by 4\n");
		return(-1); 
	}	

	datum = (uDataLength>>2) ;
	p_uMove = CBLTDataBuffer;
	p_uMove=p_uMove+(uDataLength>>2);	
/*
	for(nModuIndex=(nCbltModuleNum-1);nModuIndex>=0;nModuIndex--)
	{
			
			uDataTemp=*(p_uMove-1);
			/*printf("uDataTemp=%d\n",uDataTemp);*/
/*			if((uDataTemp%8)!=0)
			{
				printf("single length can not divid by 8\n");
				return(-1);
			}
			if(((*(p_uMove-2))>>27)!=g_uEtfCbltGeo[nModuIndex])  /*tailer leigk*/
/*			{
				printf("nModuIndex = %d\n",nModuIndex);
				printf("module tail position flag error : *(p_uMove-2)=%08x,geo=%d\n", *(p_uMove-2),g_uEtfCbltGeo[nModuIndex]);
				
				return (-1);
			}
		
		
			p_uMove=p_uMove-(uDataTemp>>2);   /*careful for etf special other>>2*/
			
/*			if(((*p_uMove)>>27)!=g_uEtfCbltGeo[nModuIndex]) /*header leigk*/
/*			{
				
				printf("module header  position error : True head:%x,Hope head:%x\n",(*p_uMove),g_uEtfCbltGeo[nModuIndex]);		
				return(-1);  
			}
			
			if(((*(p_uMove+1))&0xff)!=(triggerId&0xff))
			{
				printf("module %d triger num not match : *(p_uMove+1)=%d , nRecordTrgNum=%d\n",nModuIndex,(*(p_uMove+1))&0xff,triggerId&0xff);
				return (-1);
			}	
	}

*/	

	
	if ((CBLTDataBuffer[0]>>27)!=slot_board1)
	{
		printf("slot number in data header (%d) != slot_board1(%d) \n",(CBLTDataBuffer[0]>>27),slot_board1);
		return -1;
	}
	

	if((CBLTDataBuffer[1]&0xff)!=(triggerId&0xff))
	{
		printf("trigger number not match, get 0x%x, while hope 0x%x\n",(CBLTDataBuffer[1]&0xff) , (triggerId&0xff));
		return -1;
	}


/*	if((CBLTDataBuffer[datum-1]&0x7ffffff)!=uDataLength)
	{
		printf("data length not match, length in data : %d, while count in software %d \n",(CBLTDataBuffer[datum-1]&0x7ffffff) ,uDataLength );
		return -1;
	}*/


	if ((CBLTDataBuffer[datum-2]>>27)!=slot_board2)
	{
		printf("slot number in data tailor (%d) != slot_board1(%d) \n",(CBLTDataBuffer[datum-2]>>27),slot_board2);
		return -1;
	}	
	

}


int TDIG_daq()
{
	UINT32 i, id = 0 ;
	int interrupt_id = 0;
	UINT32 byteRecv ;
	/* Read and use the option file*/
	/*FILE* fOpts = fopen("configParas.txt","r");*/
	char comment[1000];
	int opt[100]={0};
	int iopt=0;
	FILE *fp;
	FILE* fOpts;
	int icnt = 0;
	int nRetryTimes = 0;
	while(1)
	{
		if ((fOpts = fopen("configParas.txt","r")) == NULL)
		{
			/*printf("cannot open file configParas.txt\n");*/
			icnt++;
			taskDelay(60);
			if(icnt%5==0)
			{
				printf("open file configParas.txt failed %d times\n",icnt);
				/*break;*/
			}
		}
		else
		{
			printf("open file configParas.txt succ\n");
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

	windowSet = opt[0];
	trigger_latency = opt[1];
	trigger_source = opt[2];
	eventperCBLT = opt[3];
	errDataEnb = opt[4];
	TDIG_interrupt_enable = opt[5];
	ROC_interrupt_enable = opt[6];
	slot_board1 = opt[7];
	boardEnb = opt[8];
	CBLTEnb = opt [9];
	ROC_CBLT_enable = opt [10];
	P_number = opt [11];
	event_saveNumber = opt [12];

	printf("windowSet = %d ; trigger_latency = 0x%x ; trigger_source = 0x%x \n",windowSet,trigger_latency,trigger_source);
	printf("eventperCBLT = 0x%x ;errDataEnb = 0x%x \n",	eventperCBLT,errDataEnb );
	printf("TDIG_interrupt_enable = 0x%x ; ROC_interrupt_enable = 0x%x \n",TDIG_interrupt_enable,ROC_interrupt_enable );
	printf("slot_board = %d , boardEnb = 0x%x , CBLTEnb = 0x%x\n",slot_board1,boardEnb,CBLTEnb );
	printf("ROC_CBLT_enable = 0x%x \n",ROC_CBLT_enable);
	printf("print data by each %d trigger \n",P_number);
	printf("event_saveNumber: %d\n",event_saveNumber);

	slot_board2 = slot_board1;

	fp = fopen("TDIGdata.dat","w");
	if(!fp)
	{
			printf("Error in create file\n");
			return -1;
	}

	g_ntriggerNum = 0;
	nCbltModuleNum = 1;
	g_uEtfCbltGeo[0] = 13;

/*	if(TDIG_interrupt_enable + ROC_interrupt_enable)*/
		TDIG_InitInterrupt (ETF_INT_VEC, ETF_INT_LEVEL) ;

	sysIntDisable(ETF_INT_LEVEL);		
	taskDelay(10);
	
	if(CBLTinit(64)==-1)
	{
		printf("ERROR --> init cblt\n");
		return -1;
	}
	
	
	if(!ROC_CBLT_enable)
	{
		if(-1==configCBLTChain(slot_board1,4))
		{
			printf("configCBLTChain failed 1 \n");
			return -1;
		}
		printf("set TDIG as only board in CBLT chain \n ");
	}
	else
	{
		if(-1==configCBLTChain(slot_board1,3))/*1 first, 2 middle, 3 last, 4 only*/
		{
			printf("configCBLTChain failed 1 \n");
			return -1;
		}	
		printf("set TDIG as last board in CBLT chain \n ");
		
		
		if(-1==ROC_configCBLTChain(1))/*1 first, 2 middle, 3 last, 4 only*/
		{
			printf("ROC_configCBLTChain failed \n");
			return -1;
		}	
		printf("set ROC as first board in CBLT chain \n ");		
	}

	
	
	if(-1==TDIG_config(slot_board1<<20))
	{
		printf("TDIG_config failed\n");
		return -1;
	}

	if(ROC_CBLT_enable)
	{
		ETFClearFEE();

		if(-1==start_ROC())
			return -1;
	}
	else
	{
		if (-1== ClearFIFO(slot_board1<<20))
			return -1;
	}


/*	if(TDIG_interrupt_enable + ROC_interrupt_enable)*/
		
	IntEnable () ;
	
	
	while(1) 
	{	
	/*	if(TDIG_interrupt_enable + ROC_interrupt_enable)*/
		{
/*			wait_interrupt();*/
	

			nRetryTimes = 0 ;
			while(ERROR == semTake(Sem_Interrupt, 1) ) 
			{
				nRetryTimes ++ ;
				if( nRetryTimes > 180)/*3 sec*/
				{
					printf("semTake(Sem_Interrupt, %d) times out %d times\n", 1, nRetryTimes) ;/* waiting for interrupt...*/
					
					nRetryTimes=0;
				}
			}
			nRetryTimes = 0 ;

/*
			interrupt_id++;
			printf("interrupt_id : %d\n",interrupt_id);
			taskDelay(1);
*/			
		}

		/*printf("receive an interrupt, starting CBLT now!\n");*/
		
		/*taskDelay(60);*/
	
		if(-1==(byteRecv=CBLTrun(CBLTADDR,(UINT32)CBLTDataBuffer,MAXTRANSBYTES)))
		{
			printf ("ERROR --> CbltRun, id=%d\n", id) ;
			return -1;
		}
		else
		{
		
			if(-1==TDIG_DataCheck(byteRecv, id))
			{
				printData(byteRecv, id);
			}
			else
			{
				if((id%P_number)==(P_number-1))
				{
					printf("event id = %d\n",id);
				
					printData(byteRecv, id);
					
				}
			}
				
		}

		/*save data to file*/
		
	        if(1!= fwrite(CBLTDataBuffer,(sizeof(int))*(byteRecv>>2),1,fp))
	        {
	                printf("data write error\n");
					fclose(fp);
	                return -1;
	        }		
	

		if(-1==TDIG_clrPurged())
		{
			return -1;
		}

		/*taskDelay(1);*/

		
		id ++ ;
		
		if(id == event_saveNumber) 
			break;	
		
	}

/*	if(TDIG_interrupt_enable + ROC_interrupt_enable)*/
	{
		ReleaseInterrupt () ;
		sysIntDisable(ETF_INT_LEVEL);
	}
	printf("before file closed\n");
	fclose(fp);
	printf("file closed\n");
	return 0;
	
}

int TDIG_2board()
{
	UINT32 i, id = 0 ;
	int interrupt_id = 0;
	UINT32 byteRecv ;
	/* Read and use the option file*/
	/*FILE* fOpts = fopen("configParas.txt","r");*/
	char comment[1000];
	int opt[100]={0};
	int iopt=0;
	FILE *fp;
	FILE* fOpts;
	int icnt = 0;
	int nRetryTimes = 0;

	int one_or_two = 1;
	while(1)
	{
		if ((fOpts = fopen("configParas.txt","r")) == NULL)
		{
			/*printf("cannot open file configParas.txt\n");*/
			icnt++;
			taskDelay(60);
			if(icnt%5==0)
			{
				printf("open file configParas.txt failed %d times\n",icnt);
				/*break;*/
			}
		}
		else
		{
			printf("open file configParas.txt succ\n");
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

	windowSet = opt[0];
	trigger_latency = opt[1];
	trigger_source = opt[2];
	eventperCBLT = opt[3];
	errDataEnb = opt[4];
	TDIG_interrupt_enable = opt[5];
	ROC_interrupt_enable = opt[6];
	slot_board1 = opt[7];
	boardEnb = opt[8];
	CBLTEnb = opt [9];
	ROC_CBLT_enable = opt [10];
	P_number = opt [11];
	event_saveNumber = opt [12];

	/* slot configure */
	one_or_two = opt[13];
	slot_board2 = opt[14];

	printf("windowSet = %d ; trigger_latency = 0x%x ; trigger_source = 0x%x \n",windowSet,trigger_latency,trigger_source);
	printf("eventperCBLT = 0x%x ;errDataEnb = 0x%x \n",	eventperCBLT,errDataEnb );
	printf("TDIG_interrupt_enable = 0x%x ; ROC_interrupt_enable = 0x%x \n",TDIG_interrupt_enable,ROC_interrupt_enable );
	printf("slot_board = %d , boardEnb = 0x%x , CBLTEnb = 0x%x\n",slot_board1,boardEnb,CBLTEnb );
	printf("ROC_CBLT_enable = 0x%x \n",ROC_CBLT_enable);
	printf("print data by each %d trigger \n",P_number);
	printf("event_saveNumber: %d\n",event_saveNumber);
	printf("TDIG board slot a: %d\n", slot_board1);
	printf("TDIG board slot b: %d\n", slot_board2);

	fp = fopen("TDIGdata.dat","w");
	if(!fp)
	{
			printf("Error in create file\n");
			return -1;
	}

	g_ntriggerNum = 0;
	/*slot_board1 = slot_a;*/
	/*slot_board2 = slot_b;*/
	nCbltModuleNum = 2;
	g_uEtfCbltGeo[0] = slot_board1;
	g_uEtfCbltGeo[1] = slot_board2;

/*	if(TDIG_interrupt_enable + ROC_interrupt_enable)*/
		TDIG_InitInterrupt (ETF_INT_VEC, ETF_INT_LEVEL) ;

	sysIntDisable(ETF_INT_LEVEL);		
	taskDelay(10);
	
	if(CBLTinit(64)==-1)
	{
		printf("ERROR --> init cblt\n");
		return -1;
	}
	
	
	if(!ROC_CBLT_enable)
	{
		if(-1==configCBLTChain(slot_board1,1))
		{
			printf("configCBLTChain failed 1 \n");
			return -1;
		}
		printf("set TDIG1 as first board in CBLT chain \n ");

		if(-1==configCBLTChain(slot_board2,3))
		{
			printf("configCBLTChain failed 1 \n");
			return -1;
		}
		printf("set TDIG2 as last board in CBLT chain \n ");		
	}
	else
	{
		if(-1==configCBLTChain(slot_board1,2))/*1 first, 2 middle, 3 last, 4 only*/
		{
			printf("configCBLTChain failed 1 \n");
			return -1;
		}	
		printf("set TDIG1 as middle board in CBLT chain \n ");

	
		if(-1==configCBLTChain(slot_board2,3))/*1 first, 2 middle, 3 last, 4 only*/
		{
			printf("configCBLTChain failed 1 \n");
			return -1;
		}	
		printf("set TDIG2 as last board in CBLT chain \n ");
		
		if(-1==ROC_configCBLTChain(1))/*1 first, 2 middle, 3 last, 4 only*/
		{
			printf("ROC_configCBLTChain failed \n");
			return -1;
		}	
		printf("set ROC as first board in CBLT chain \n ");		
	}

	
	
	if(-1==TDIG_config(slot_board1<<20))
	{
		printf("TDIG_config failed\n");
		return -1;
	}
	if(-1==TDIG_config(slot_board2<<20))
	{
		printf("TDIG_config failed\n");
		return -1;
	}	

	if(ROC_CBLT_enable)
	{
		ETFClearFEE();

		if(-1==start_ROC())
			return -1;
	}
	else
	{
		if (-1== ClearFIFO(slot_board1<<20))
			return -1;
		if (-1== ClearFIFO(slot_board2<<20))
			return -1;
	}


/*	if(TDIG_interrupt_enable + ROC_interrupt_enable)*/
		
	IntEnable () ;
	
	
	while(1) 
	{	
	/*	if(TDIG_interrupt_enable + ROC_interrupt_enable)*/
		{
/*			wait_interrupt();*/
	

			nRetryTimes = 0 ;
			while(ERROR == semTake(Sem_Interrupt, 1) ) 
			{
				nRetryTimes ++ ;
				if( nRetryTimes > 180)/*3 sec*/
				{
					printf("semTake(Sem_Interrupt, %d) times out %d times\n", 1, nRetryTimes) ;/* waiting for interrupt...*/
					
					nRetryTimes=0;
				}
			}
			nRetryTimes = 0 ;

/*
			interrupt_id++;
			printf("interrupt_id : %d\n",interrupt_id);
			taskDelay(1);
*/			
		}

		/*printf("receive an interrupt, starting CBLT now!\n");*/
		
		/*taskDelay(60);*/
	
		if(-1==(byteRecv=CBLTrun(CBLTADDR,(UINT32)CBLTDataBuffer,MAXTRANSBYTES)))
		{
			printf ("ERROR --> CbltRun, id=%d\n", id) ;
			return -1;
		}
		else
		{
		
			if(-1==TDIG_DataCheck(byteRecv, id))
			{
				printData(byteRecv, id);
				return -1;
			}
			else
			{
				if((id%P_number)==(P_number-1))
				{
					printf("event id = %d\n",id);
				
					printData(byteRecv, id);
					
				}
			}
				
		}

		/*save data to file*/
		
	        if(1!= fwrite(CBLTDataBuffer,(sizeof(int))*(byteRecv>>2),1,fp))
	        {
	                printf("data write error\n");
					fclose(fp);
	                return -1;
	        }		
	

		if(-1==TDIG_clrPurged())
		{
			return -1;
		}

		/*taskDelay(1);*/

		
		id ++ ;
		
		if(id == event_saveNumber) 
			break;	
		
	}

/*	if(TDIG_interrupt_enable + ROC_interrupt_enable)*/
	{
		ReleaseInterrupt () ;
		sysIntDisable(ETF_INT_LEVEL);
	}
	printf("before file closed\n");
	fclose(fp);
	printf("file closed\n");
	return 0;
	
}

TDIG_2board_()
{
	UINT32 i, id = 0 ;
	UINT32 byteRecv ;
	/* Read and use the option file*/
	FILE* fOpts = fopen("configParas.txt","r");
	char comment[1000];
	int opt[100]={0};
	int iopt=0;
	while ( 1 ) {
	if ( ! fgets(comment,1000,fOpts) ) break;
	if ( comment[0]=='#' ) continue;
	sscanf(comment,"0x%x",&opt[iopt]);
	iopt++;
	}
	fclose(fOpts);
/*
	inlCorrection = opt[0];
	reject_cnt_offset = opt[1];
	search_window = opt[2];
	match_window = opt[3];
	trigger_latency = opt[4];
	configFPGA_enable = opt[5];
	interrupt_enable = opt[6];
	slot_board1 = opt[7];
	slot_board2 = opt [8];	
	
	printf("inlCorrection = %d ; reject_cnt_offset = 0x%x ; search_window = 0x%x \n; 
match_window = 0x%x ;trigger_latency = 0x%x ,configFPGA_enable = 0x%x \n;
interrupt_enable = 0x%x\n",
		inlCorrection,reject_cnt_offset,search_window,match_window,trigger_latency,configFPGA_enable,interrupt_enable);

	printf("slot_board1 = %d , slot_board2 = %d \n",slot_board1,slot_board2);
*/
	TOFConfigFeeBaseAddr(slot_board1);
	TOFConfigFeeBaseAddr(slot_board2);

	if(-1==configCBLTChain(slot_board1,2))/*1 first, 2 middle, 3 last, 4 only*/
	{
		printf("configCBLTChain failed 1 \n");
		return -1;
	}
	
	if(-1==configCBLTChain(slot_board2,3))
	{
		printf("configCBLTChain failed 2\n");
		return -1;
	}
	
	if(-1==TDIG_config(slot_board1<<20))
	{
		printf("TDIG_config failed 1 \n");
		return -1;
	}
	
	if(-1==TDIG_config(slot_board2<<20))
	{
		printf("TDIG_config failed 2\n");
		return -1;
	}



	sysIntDisable(7);		
	taskDelay(60);
	
	if(CBLTinit(64)==-1)
	{
		printf("ERROR --> init cblt\n");
		return -1;
	}

	while(1) 
	{		
		if(-1==(byteRecv=CBLTrun(CBLTADDR,(UINT32)CBLTDataBuffer,MAXTRANSBYTES)))
		{
			printf ("ERROR --> CbltRun, id=%d\n", id) ;
			return -1;
		}
		/*
		else
		{
			if(-1==TDIG_DataCheck(byteRecv, id))
			{
				printData(byteRecv, id);
			}
		}
		*/
		printData(byteRecv, id);
	
		if(-1==TDIG_clrPurged())
		{
			return -1;
		}

		taskDelay(60);
		
		if(id > 100000) 
			break;
		
		id ++ ;
/*
		if((id%600)==0)
			printf ("DEBUG OUT --> cblt size: %d, id=%d\n", byteRecv, id) ;
*/		
	}

	return 0;
}


void test()
{

	if(-1==CLK_config())
		return;

	if(-1==ROC_config())
		return;

	if(-1==TDIG_daq())
		return;
}

void test2()
{

	if(-1==CLK_config())
		return;

	if(-1==ROC_config())
		return;

	if(-1==TDIG_2board())
		return;
}

