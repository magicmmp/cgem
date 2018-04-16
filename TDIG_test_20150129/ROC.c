#include <time.h>
#include <stdio.h>
#include <stdlib.h>
#include "5100_test.h"
#include "ROC.h"

unsigned int clk_source;
UINT L1Latency;
UINT semiAutoL1Num;
UINT autoL1Interval;
int CTTPModNum;


/*for CTTP*/
int low_threshold_nino;
int high_threshold_nino;
int low_threshold_power;
int high_threshold_power;
int power_ena[3];
int CTTP_config_enable;
int cal_amp;


int TOFClearRocFEE()
{
	WriteToVme32(0xbc0270, VA32, 0x24);	/*clear FEE FIFO*/
	taskDelay(1);
}

void TOFRocStartL1()
{
	WriteToVme32(0xbc0270, VA32, 0x19);/*begin work*/
}

void TOFRocStop()
{
	WriteToVme32(0xbc0270, VA32, 0x18);
}

int config_clk_source()
{
	/*write fask control board, 0x16 inner clock,0x17 out clokc*/
	if(clk_source==0)
	{
		if(WriteToVme32(0xbc0270, VA32, 0x16)==0)/*inner clk,general use for hw test*/
			printf("ROC int clk  : Write 0x00000016 to 0x00bc0270 Successfully!\n");
		else
		{
			printf("ROC int clk  : Write 0x00000016 to 0x00bc0270 failed!\n");
			return -1;
		}
			
	}
	else if (clk_source==1)
	{
		if(WriteToVme32(0xbc0270, VA32, 0x17)==0)/*external clk,clk from clk module*/
			printf("ROC ext clk: Write 0x00000017 to 0x00bc0270 Successfully!\n");
		else
		{
			printf("ROC ext clk: Write 0x00000017 to 0x00bc0270 failed!\n");
			return -1;
		}
	}
	else
	{
		printf("clk source error\n");
		return -1;
	}
	
	taskDelay(1);

	return 0;
}

int config_normal_ROC()
{
	UINT uRead;
	UINT g_nTrgPerInt=1;
		

	/*mask L1*/
	if( -1==WriteToVme32(0xbc0270,VA32, 0x13) )
	{
		printf("error while write to VME during config_normal_ROC\n");
		return -1;
	}
	printf("config_normal_ROC : write data 0x%x to addr 0x%x \n", 0x13,0xbc0270);
	taskDelay(1);

	TOFClearRocFEE();
	taskDelay(1);

	/*set work mode*/
	if( -1==WriteToVme32(0xbc0270,VA32, 0) )
	{
		printf("error while write to VME during config_normal_ROC\n");
		return -1;
	}
	printf("config_normal_ROC : write data 0x%x to addr 0x%x \n", 0,0xbc0270);
	taskDelay(1);	

	/*set nperCBLT*/
	ReadFrmVme32(0x00bc0250,VA32,&uRead);
	printf("before conf roc nTrg/Int register:0x%08x\n",uRead);
	taskDelay(1);

	if(-1==WriteToVme32(0x00bc0250, VA32, (g_nTrgPerInt<<25)|(uRead&0x01ffffff) )  )
	{
		printf("ROC nTrg/Int register Conf:0x%08x error\n",(g_nTrgPerInt<<25)|(uRead&0x01ffffff));
		return -1;
	}	

	printf("roc_normal_config finished\n");

	/*config CTTP*/


	/*start working
	if( -1==WriteToVme32(0xbc0270,VA32, 0x19) )
	{
		printf("error while write to VME during config_normal_ROC\n");
		return -1;
	}
	printf("config_normal_ROC : write data 0x%x to addr 0x%x \n", 0x19,0xbc0270);
	taskDelay(1);*/	

	
	return 0;
}

int start_ROC()
{
	if( -1==WriteToVme32(0xbc0270,VA32, 0x19) )
	{
		printf("error while write to VME during config_test_ROC\n");
		return -1;
	}
/*	printf("config_test_ROC : write data 0x%x to addr 0x%x \n", 0x19,0xbc0270);*/
	/*taskDelay(1);	*/

	return 0;

}

int config_test_ROC()
{
	UINT g_nTrgPerInt=1;

	
	WriteToVme32(0xbc0270, VA32, 0x18);/*set cmd register 2*/
	taskDelay(1);
	TOFClearRocFEE();
		
	WriteToVme32(0xbc0270, VA32, 0x01);
	taskDelay(1);
	WriteToVme32(0xbc0270, VA32, 0x11);
	taskDelay(1);
	
	WriteToVme32(0xbc0274, VA32, 0x0);
	taskDelay(1);
	WriteToVme32(0xbc0278, VA32, autoL1Interval);/*0x4e20 0.5ms*/
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
		printf("Set semiAutoL1Num error\n");		
		return -1;
	}
	printf("----Write 0x%08x to 0x00bc0250 Successfully!\n",(g_nTrgPerInt<<25)+(L1Latency<<16)+semiAutoL1Num);
	WriteToVme32(0xbc0270, VA32, 0x18);/*set cmd register 2*/
	taskDelay(1);
	printf(">>VME:: Self Test Mode Configured Successfully!\n");

	/*config CTTP*/

	/*start working*/
/*
	if( -1==WriteToVme32(0xbc0270,VA32, 0x19) )
	{
		printf("error while write to VME during config_test_ROC\n");
		return -1;
	}
	printf("config_test_ROC : write data 0x%x to addr 0x%x \n", 0x19,0xbc0270);
	taskDelay(1);	
*/

	return 0;
}

int config_monitor_ROC()
{
	UINT uRead;
	UINT g_nTrgPerInt=1;
	
	WriteToVme32(0xbc0270, VA32, 0x18);/*set cmd register 2*/
	taskDelay(1);
	TOFClearRocFEE();
	
	ReadFrmVme32(0x00bc0250,VA32,&uRead);
	printf("before conf roc nTrg/Int register:0x%08x\n",uRead);
	if(-1==WriteToVme32(0x00bc0250, VA32, (g_nTrgPerInt<<25)|(uRead&0x01ffffff) )  )
	{
		printf("ROC nTrg/Int register Conf:0x%08x error\n",(g_nTrgPerInt<<25)|(uRead&0x01ffffff));	
		return -1;
	}
	taskDelay(1);

	/*set work mode*/
	if( -1==WriteToVme32(0xbc0270,VA32, 0x2) )
	{
		printf("error while write to VME during config_monitor_ROC\n");
		return -1;
	}
	printf("config_monitor_ROC : write data 0x%x to addr 0x%x \n", 0x2,0xbc0270);
	taskDelay(1);	

	/*config CTTP*/

	/*start working	
	if( -1==WriteToVme32(0xbc0270,VA32, 0x19) )
	{
		printf("error while write to VME during config_monitor_ROC\n");
		return -1;
	}
	printf("config_monitor_ROC : write data 0x%x to addr 0x%x \n", 0x19,0xbc0270);
	taskDelay(1);*/


	return 0;
}


int ROC_configCBLTChain(int position)/*1 first, 2 middle, 3 last, 4 only*/
{
	unsigned int geo = 8;
	unsigned int nadr ;
	nadr = (geo<<19) + 0x7ffb;

	if(position==1)
	{
		if ( -1 == WriteToVme8(nadr, VCSR, 0xa6 ) ) 
		{
			printf("Write ROC chain First Failed \n"); 
			return -1;
		}
		else
		{
			printf("ROC: first GEO Chain \n");
		}
	}
	else if(position==3)
	{
		if ( -1 == WriteToVme8(nadr, VCSR, 0x9a ) ) 
		{
			printf("Write ROC chain Last Failed \n"); 
			return -1;
		}
		else
		{
			printf("ROC:last GEO Chain %d\n");
		}
	}
	else if(position==4)
	{
		if ( -1 == WriteToVme8(nadr, VCSR, 0xaa ) ) 
		{
			printf("Write ROC chain only Failed \n"); 
			return -1;
		}
		else
		{
			printf("ROC:only GEO Chain %d\n");
		}
	}		
	else if(position==2)
	{
		if ( -1 == WriteToVme8(nadr, VCSR, 0x96 ) ) 
		{
			printf("Write ROC chain middle Failed\n"); 
			return -1;				
		}
		else
		{
			printf("ROC:mid GEO Chain\n");
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


int ROC_configCBLTChain2(int position, unsigned int geo)
{
	/*unsigned int geo = 8; */
	unsigned int nadr;
	nadr = (geo<<19) + 0x7ffb;

	if(position==1)
	{
		if ( -1 == WriteToVme8(nadr, VCSR, 0xa6 ) ) 
		{
			printf("Write ROC chain First Failed \n"); 
			return -1;
		}
		else
		{
			printf("ROC: first GEO Chain \n");
		}
	}
	else if(position==3)
	{
		if ( -1 == WriteToVme8(nadr, VCSR, 0x9a ) ) 
		{
			printf("Write ROC chain Last Failed \n"); 
			return -1;
		}
		else
		{
			printf("ROC:last GEO Chain %d\n");
		}
	}
	else if(position==4)
	{
		if ( -1 == WriteToVme8(nadr, VCSR, 0xaa ) ) 
		{
			printf("Write ROC chain only Failed \n"); 
			return -1;
		}
		else
		{
			printf("ROC:only GEO Chain %d\n");
		}
	}		
	else if(position==2)
	{
		if ( -1 == WriteToVme8(nadr, VCSR, 0x96 ) ) 
		{
			printf("Write ROC chain middle Failed\n"); 
			return -1;				
		}
		else
		{
			printf("ROC:mid GEO Chain\n");
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


int GlobalReset()
{
	WriteToVme32(0xbc0270, VA32, 0x2c);	/*global reset all the FEE& Monitor &ROC*/
	taskDelay(1);
	return 0;
}

int CTTP_Config()
{
	int data = 0;
	UINT uRead;
	int iStep;
	int i=0;
	
	CTTPModNum = 1;

	/*check connection**/
	data = (1<<31)+(CTTPModNum<<25)+(1<<17);
	WriteToVme32(0xbc0240, VA32, data);
	taskDelay(1);
	ReadFrmVme32(0xbc0244,VA32,&uRead);
	if((uRead&0xffff)!=0x0518)
	{
		printf("connection error! uRead = 0x%x \n",uRead);
		return -1;
	}

	/*config NINO low threshold**/
	for(i=0;i<9;i++)
	{
		data = (CTTPModNum<<25) + ((0x20+(i*4))<<16) +(low_threshold_nino <<4);
		WriteToVme32(0xbc0240, VA32, data);
		printf("write 0x%x to 0xbc0240 \n",data);
	}

	/*config NINO high threshold**/
	for(i=0;i<9;i++)
	{
		data = (CTTPModNum<<25) + ((0x22+(i*4))<<16) +(high_threshold_nino <<4);
		WriteToVme32(0xbc0240, VA32, data);
		printf("write 0x%x to 0xbc0240 \n",data);
	}

	/*config cal amp*/
	printf("CTTP config: config cal amp\n");
	for(i=0;i<9;i++)
	{
		data = (CTTPModNum<<25) + ((0x0A+(i*4))<<16) +(cal_amp <<4);
		WriteToVme32(0xbc0240, VA32, data);
		taskDelay(1);
		printf("write 0x%x to 0xbc0240 \n",data);

	}	

	

	/*config power high threshold**/
	for(i=0;i<36;i++)
	{
		data = (CTTPModNum<<25) + ((0xa0 +(i*4))<<16) +(high_threshold_power);
		WriteToVme32(0xbc0240, VA32, data);
		printf("write 0x%x to 0xbc0240 \n",data);
	}	

	/*enable power output**/
	data = (CTTPModNum<<25) + (0x44<<16) + power_ena[0];/*15:0*/
	WriteToVme32(0xbc0240, VA32, data);
	printf("write 0x%x to 0xbc0240 \n",data);

	data = (CTTPModNum<<25) + (0x46<<16) + power_ena[1];/*31:16*/
	WriteToVme32(0xbc0240, VA32, data);
	printf("write 0x%x to 0xbc0240 \n",data);

	data = (CTTPModNum<<25) + (0x48<<16) + power_ena[2];/*35:32*/
	WriteToVme32(0xbc0240, VA32, data);
	printf("write 0x%x to 0xbc0240 \n",data);


	/*config power low threshold**/
	for(i=0;i<36;i++)
	{
		data = (CTTPModNum<<25) + ((0x9E +(i*4))<<16) +(low_threshold_power);
		WriteToVme32(0xbc0240, VA32, data);
		printf("write 0x%x to 0xbc0240 \n",data);
	}	

	return 0;
	
}

int ROC_config()
{

	UINT32 i, id = 0 ;
	char comment[1000];
	int opt[100]={0};
	int iopt=0;
	FILE* fOpts;
	int icnt = 0;
	unsigned int work_mode;
	UINT writeTest;
	UINT uTemp;
	

	while(1)
	{
		if ((fOpts = fopen("ROCParas.txt","r")) == NULL)
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


	clk_source	= opt[0]; /*external 1 ; internal 0*/
	work_mode = opt [1]; /*normal 0; test 1; monitor 2*/
	L1Latency = opt[2];
	semiAutoL1Num = opt[3];
	autoL1Interval = opt[4];
	low_threshold_nino = opt[5];
	high_threshold_nino = opt[6];
	low_threshold_power = opt[7];
	high_threshold_power = opt[8];
	cal_amp = opt[9];
	power_ena[0] = opt[10];	
	power_ena[1] = opt[11];	
	power_ena[2] = opt[12];	
	CTTP_config_enable = opt[13];

	printf("clk_source = %d \n",clk_source);
	printf("CTTP_config_enable: 0x%x\n",CTTP_config_enable);
	printf("CTTP low_threshold_nino: 0x%x\n",low_threshold_nino);
	printf("CTTP high_threshold_nino: 0x%x\n",high_threshold_nino);
	printf("CTTP low_threshold_power: 0x%x\n",low_threshold_power);
	printf("CTTP high_threshold_power: 0x%x\n",high_threshold_power);
	printf("CTTP cal_amp: 0x%x\n",cal_amp);
	printf("CTTP power enable: 0x%x (35-32), 0x%x (31-16), 0x%x (15-0)\n"
			,power_ena[2],power_ena[1],power_ena[0]);


	GlobalReset();

	if(-1==config_clk_source())
	{
		printf("ROC config_clk_source error!\n");
		return -1;
	}


	writeTest=0xffffffff;
	WriteToVme32(0x00bc0290, VA32,writeTest);
	ReadFrmVme32(0x00bc0290,VA32,&uTemp);
	if(writeTest!=uTemp)
	{
		printf("ETFConfigROC: check test reg err,\n write data 0x%x while read data 0x%x \n",writeTest,uTemp);
		return -1;
	}


	writeTest=0x0;
	WriteToVme32(0x00bc0290, VA32,writeTest);
	ReadFrmVme32(0x00bc0290,VA32,&uTemp);
	if(writeTest!=uTemp)
	{
		printf("ETFConfigROC: check test reg err,\n write data 0x%x while read data 0x%x \n",writeTest,uTemp);
		return -1;
	}


	if (work_mode==0)
	{
		if(-1==config_normal_ROC())
		{
			printf("config_normal_ROC failed \n");
			return -1;
		}
	}
	else if (work_mode==1)
	{
		if(-1==config_test_ROC())
		{
			printf("config_test_ROC failed \n");
			return -1;
		}
	}
	else if (work_mode==2)
	{
		if(-1==config_monitor_ROC())
		{
			printf("config_monitor_ROC failed \n");
			return -1;
		}
	}
	else
	{
		printf("ROC work mode error!\n");
		return -1;
	}

	if(CTTP_config_enable)
	{
		if(-1==CTTP_Config())
		{
			printf("CTTP_Config failed!\n");
			return -1;
		}
	}
	
	return 0;

}

int ROC_sconfig()
{
	UINT32 uTemp;
	UINT32 writeTest;

	/* reset the fctrl */
	WriteToVme32(0x00bc0290, VA32, 0x2C);
	taskDelay(1);

	TOFClearRocFEE();
	/* test reg test BC0290, 0xFFFFFFFF, 0 */
	printf("before clock change test\n");
	writeTest=0xffffffff;
	WriteToVme32(0x00bc0290, VA32,writeTest);
	ReadFrmVme32(0x00bc0290,VA32,&uTemp);
	if(writeTest!=uTemp)
	{
		printf("ETFConfigROC: check test reg err,\n write data 0x%x while read data 0x%x \n",writeTest,uTemp);
		return -1;
	}


	writeTest=0x0;
	WriteToVme32(0x00bc0290, VA32,writeTest);
	ReadFrmVme32(0x00bc0290,VA32,&uTemp);
	if(writeTest!=uTemp)
	{
		printf("ETFConfigROC: check test reg err,\n write data 0x%x while read data 0x%x \n",writeTest,uTemp);
		return -1;
	}else{
		printf("clock change before RW test succeed!\n");
	}
	
	/* select clock source BC0270:0x17 */
	WriteToVme32(0x00bc0270, VA32, 0x17);
	taskDelay(1);
	
	printf("after clock change test\n");
	writeTest=0xffffffff;
	WriteToVme32(0x00bc0290, VA32,writeTest);
	ReadFrmVme32(0x00bc0290,VA32,&uTemp);
	if(writeTest!=uTemp)
	{
		printf("ETFConfigROC: check test reg err,\n write data 0x%x while read data 0x%x \n",writeTest,uTemp);
		return -1;
	}


	writeTest=0x0;
	WriteToVme32(0x00bc0290, VA32,writeTest);
	ReadFrmVme32(0x00bc0290,VA32,&uTemp);
	if(writeTest!=uTemp)
	{
		printf("ETFConfigROC: check test reg err,\n write data 0x%x while read data 0x%x \n",writeTest,uTemp);
		return -1;
	}else{
		printf("clock change after RW test succeed!\n");
	}
	
	/* configure BC0250:  ref config_normal_xx */

 	config_normal_ROC();
	printf("ROC_sconfig after config_normal_ROC\n");

	taskDelay(1);

	/* BC0270:0x21 */
	WriteToVme32(0x00bc0270, VA32, 0x21);
	taskDelay(1);

	

	return 0;
}

int shiftch_helper(UINT32 chl, UINT32 shift_val)
{
        UINT32 mask;
	UINT32 chladdr;
	UINT32 uRead;
	printf("channel %d shift value = %d \n",chl, shift_val);
        
        if(chl==0) mask=0x11E;
	else if(chl==1)	mask=0x11D;
	else if(chl==2) mask=0x11B;
        else if(chl==3) mask=0x117;
	else            mask=0x10F;

	chladdr = ((chl & 0xF) << 28) | mask;
	WriteToVme32(0xbc0280, VA32, chladdr);
	/* read it */
	ReadFrmVme32(0xbc0280,VA32,&uRead);
	printf("0xBC0280: %8x\n", uRead);

	taskDelay(1);
	WriteToVme32(0xbc0288, VA32, 0x0);
	taskDelay(1);
	WriteToVme32(0xbc028C, VA32, (shift_val & 0x7F));
	taskDelay(1);
	ReadFrmVme32(0xbc028C,VA32,&uRead);
	printf("0xBC028C: %8x\n", uRead);
	taskDelay(1);
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

	while(1)
	{
		if ((fOpts = fopen("ROC_shift.txt","r")) == NULL)
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


int resetch()
{
	WriteToVme32(0xbc0270, VA32, 0x32);
	taskDelay(1);
	return 0;
}

int ROC_pll()
{


	UINT32 sd = 0, sdd = 0;
	int cnt = 0;
	UINT32 limit = 0x700;
	UINT32 uRead;
	
	char chfind;
	char auto_man;
	int param_ok=0;
	/* Read and use the option file*/
	/*FILE* fOpts = fopen("configParas.txt","r");*/

	FILE *fp;
	fp = fopen("fctrl_lineraty.txt","w");
	if(!fp)
	{
			printf("Error in create file\n");
			return -1;
	}


	printf("Please choose auto or manual[a|m]:\n");
	while (!param_ok)
	{
		scanf("%c",&auto_man);
		if(auto_man=='a'||auto_man=='m')
			param_ok=1;
	}

	
	while(1) 
	{	
	
		sd = cnt & 0xFFF;
		sdd =sd|(0x7FF1<<12);


	    if(auto_man=='m')
		{
		WriteToVme32(0xbc0280, VA32, sdd);
		taskDelay(1);
	  
		WriteToVme32(0xbc0270, VA32, 0x33);
		taskDelay(1);

		ReadFrmVme32(0xbc0284,VA32,&uRead);
		taskDelay(1);
		fprintf(fp, "%d\t%08x\n",cnt,uRead);
		printf("%d\t%08x\n",cnt,uRead);	

		WriteToVme32(0xbc0270, VA32, 0x19);
		taskDelay(1);
	
		printf("please choose +,-,*,p(+10),n(-10),f(+50),b(-50)\n");
		scanf("%c",&chfind);
	 	printf("The input is %c\n",chfind);       
		if(chfind == '+')
		{
			++cnt;
   		     	if(cnt >= limit)
	 			cnt = 0;
		}
		else if(chfind == '-')
		{
			--cnt;
			if(cnt < 0)
				cnt = 0;
		}
		else if(chfind == 'p')
		{
			cnt=cnt+10;
			if(cnt >= limit)
				cnt = 0;
		}
		else if(chfind == 'n')
		{
			cnt=cnt-10;
			if(cnt <0)
				cnt = 0;
		}
		else if(chfind == 'f')
		{
			cnt=cnt+50;
			if(cnt >= limit)
				cnt=0;
		}
		else if(chfind == 'b')
		{
			cnt=cnt-50;
			if(cnt <= 50)
				cnt=0;
		}
		else if(chfind =='*')
		break;
		
	   }
           else
	   {
		WriteToVme32(0xbc0280, VA32, 0x7FF0660);
		taskDelay(1);
		WriteToVme32(0xbc0270, VA32, 0x33);
		taskDelay(200);
	  

		ReadFrmVme32(0xbc0284,VA32,&uRead);
		taskDelay(1);
		fprintf(fp, "%08x\n",uRead);
		printf( "%08x\n",uRead);
		break;	
	
	   }
	
	}
	return 0;
	
}


int ROC_pll2()
{


	UINT32 sd = 0, sdd = 0;
	int cnt = 0;
	UINT32 limit = 0xFFF;
	UINT32 uRead;
	
	char chfind;
	char auto_man;
	int param_ok=0;
	/* Read and use the option file*/
	/*FILE* fOpts = fopen("configParas.txt","r");*/

	FILE *fp;
	fp = fopen("fctrl_lineraty.txt","w");
	if(!fp)
	{
			printf("Error in create file\n");
			return -1;
	}


	printf("Please choose auto or manual[a|m]:\n");
	while (!param_ok)
	{
		scanf("%c",&auto_man);
		if(auto_man=='a'||auto_man=='m')
			param_ok=1;
	}

	
	while(1) 
	{	
	
		sd = cnt & 0xFFF;
		sdd =sd|(0x7FF1<<12);


	    if(auto_man=='m')
		{
		WriteToVme32(0xbc0280, VA32, sdd);
		taskDelay(1);
	  
		WriteToVme32(0xbc0270, VA32, 0x33);
		taskDelay(1);

		ReadFrmVme32(0xbc0284,VA32,&uRead);
		taskDelay(1);
		fprintf(fp, "%d\t%08x\n",cnt,uRead);
		printf("%d\t%08x\n",cnt,uRead);	

		WriteToVme32(0xbc0270, VA32, 0x19);
		taskDelay(1);
	
		printf("please choose +,-,*,p(+10),n(-10),f(+50),b(-50),](+200)[(-200)\n");
		scanf("%c",&chfind);
	 	printf("The input is %c\n",chfind);       
		if(chfind == '+')
		{
			++cnt;
   		     	if(cnt >= limit)
	 			cnt = 0;
		}
		else if(chfind == '-')
		{
			--cnt;
			if(cnt < 0)
				cnt = 0;
		}
		else if(chfind == 'p')
		{
			cnt=cnt+10;
			if(cnt >= limit)
				cnt = 0;
		}
		else if(chfind == 'n')
		{
			cnt=cnt-10;
			if(cnt <0)
				cnt = 0;
		}
		else if(chfind == 'f')
		{
			cnt=cnt+50;
			if(cnt >= limit)
				cnt=0;
		}
		else if(chfind == 'b')
		{
			cnt=cnt-50;
			if(cnt <= 0)
				cnt=0;
		}
		else if(chfind == ']')
		{
			cnt = cnt + 200;
			if(cnt >= limit)
				cnt = 0;
		}
		else if(chfind == '[')
		{
			cnt = cnt - 200;
			if(cnt < 0)
				cnt = 0;
		}
		else if(chfind =='*')
		break;
		
	   }
           else
	   {
		WriteToVme32(0xbc0280, VA32, sdd);
		taskDelay(1);
		printf("cnt = %d\n", cnt);
		WriteToVme32(0xbc0270, VA32, 0x33);
		taskDelay(1);
	  

		ReadFrmVme32(0xbc0284,VA32,&uRead);
		taskDelay(1);
		fprintf(fp, "%08x\n",uRead);

		++cnt;
		if(cnt == 0x1000)
			break;	
	
	   }
	
	}
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