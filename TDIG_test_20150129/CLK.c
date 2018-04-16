#include <time.h>
#include <stdio.h>
#include <stdlib.h>
#include "5100_test.h"
#include "CLK.h"

unsigned int baseAddr;
unsigned int clk_source;
unsigned int clk_latency;
UINT DelayRstData;


int internal_config()
{
	unsigned int data = 0x0;
	unsigned int check_data;
	
	if( -1==WriteToVme32(0x4f0f030,VA32, data) )
	{
		printf("error while write to VME during internal_config\n");
		return -1;
	}
	printf("internal_config : write data 0x%x to addr 0x%x \n", data,0x4f0f030);
	taskDelay(1);


	ReadFrmVme32(0x4f0f030 ,VA32,&check_data);
	if((check_data&0xf)!=(data&0xf))
	{
		printf("read check error in internal_config, check_data = 0x%x, write data = 0x%x \n",check_data,data);
		return -1;
	}


	return 0;
}

int external_config()
{
	/*step 1*/
	unsigned int data = 0x1;
	unsigned int check_data;
	
	if( -1==WriteToVme32(baseAddr +0xf030,VA32, data) )
	{
		printf("error while write to VME during external_config step1\n");
		return -1;
	}
	printf("external_config  step1: write data 0x%x to addr 0x%x \n", data,baseAddr +0xf030);
	taskDelay(1);

	ReadFrmVme32(baseAddr +0xf030 ,VA32,&check_data);
	if(check_data!=data)
	{
		printf("read check error in external_config step1, check_data = 0x%x, write data = 0x%x \n",check_data,data);
		return -1;
	}	

	/*step 2*/
	if( -1==WriteToVme32(baseAddr +0x0,VA32, DelayRstData) )
	{
		printf("error while write to VME during external_config step3\n");
		return -1;
	}
	printf("external_config  step2.1: write data 0x%x to addr 0x%x \n", DelayRstData,baseAddr +0x0);
	taskDelay(1);	
/*	
	ReadFrmVme32(baseAddr +0x0 ,VA32,&check_data);
	if(check_data!=DelayRstData)
	{
		printf("read check error in external_config step2.1, check_data = 0x%x, write data = 0x%x \n",check_data,data );
		return -1;
	}	

*/
	
	if( -1==WriteToVme32(baseAddr +0xf070,VA32, 0x1) )
	{
		printf("error while write to VME during external_config step2\n");
		return -1;
	}
	printf("external_config  step2.2: write data 0x%x to addr 0x%x \n", 0x1,baseAddr +0xf070);
	taskDelay(1);	

	
	if( -1==WriteToVme32(baseAddr +0x0,VA32, clk_latency) )
	{
		printf("error while write to VME during external_config step3\n");
		return -1;
	}
	printf("external_config  step2.3: write data 0x%x to addr 0x%x \n", clk_latency,baseAddr +0x0);
	taskDelay(1);	
/*	
	ReadFrmVme32(baseAddr +0x0 ,VA32,&check_data);
	if(check_data!=clk_latency)
	{
		printf("read check error in external_config step2.3, check_data = 0x%x, write data = 0x%x \n",check_data,data );
		return -1;
	}	
*/
	ReadFrmVme32(baseAddr +0xf070 ,VA32,&check_data);
	if((check_data&0x3)!=0x2)
	{
		printf("read check error in external_config step2.4, check_data = 0x%x\n",check_data );
		return -1;
	}	

	/*step 3*/
	if( -1==WriteToVme32(baseAddr +0xf0b0,VA32, 0x1) )
	{
		printf("error while write to VME during external_config step3\n");
		return -1;
	}
	printf("external_config  step3: write data 0x%x to addr 0x%x \n", 0x1,baseAddr +0xf0b0);
	taskDelay(1);	

	ReadFrmVme32(baseAddr +0xf0b0 ,VA32,&check_data);
	if(check_data!=0x1)
	{
		printf("read check error in external_config step3, check_data = 0x%x\n",check_data );
		return -1;
	}	

	
	return 0;
}

int slaveClk_config()
{
	unsigned int data = 0xf;
	unsigned int check_data;
	
	if( -1==WriteToVme32(baseAddr +0xf030,VA32, data) )
	{
		printf("error while write to VME during slaveClk_config\n");
		return -1;
	}
	printf("slaveClk_config : write data 0x%x to addr 0x%x \n", data,baseAddr +0xf030);
	taskDelay(1);

	ReadFrmVme32(baseAddr +0xf030 ,VA32,&check_data);
	if(check_data!=data)
	{
		printf("read check error in slaveClk_config, check_data = 0x%x, write data = 0x%x \n",check_data,data);
		return -1;
	}

	return 0;	

}

int masterClk_config()
{
	if(clk_source)
	{
		if(-1==internal_config())
		{
			printf("internal_config failed \n");
			return -1;
		}
	}
	else
	{
		if(-1==external_config())
		{
			printf("external_config failed \n");
			return -1;	
		}
	}

	return 0;
}


int CLK_config()
{
	UINT32 i, id = 0 ;
	char comment[1000];
	int opt[100]={0};
	int iopt=0;
	FILE* fOpts;
	int icnt = 0;
	unsigned int clk_slot;
	unsigned int clk_type;

	
	
	
	while(1)
	{
		if ((fOpts = fopen("CLKParas.txt","r")) == NULL)
		{
			icnt++;
			taskDelay(60);
			if(icnt%5==0)
			{
				printf("open file CLKParas.txt failed %d times\n",icnt);
				/*break;*/
			}
		}
		else
		{
			printf("open file CLKParas.txt succ\n");
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

	clk_slot = opt[0];
	clk_type = opt[1];/*master 0 ; slave 1*/
	clk_source	= opt[2]; /*external 0 ; internal 1*/
	clk_latency = opt[3];
	DelayRstData = opt[4];

	printf("clk_slot = %d \n",clk_slot);	
	printf("clk_type = %d \n",clk_type);
	printf("clk_source = %d \n",clk_source);
	printf("clk_latency = 0x%x \n",clk_latency);
	printf("DelayRstData = 0x%x \n",DelayRstData);

	baseAddr = (clk_slot<<20);
	printf("base address of clk board is 0x%x \n",baseAddr);

	if(clk_type)
	{
		if(-1==slaveClk_config())
		{
			printf("slaveClk_config failed\n");
			return -1;
		}
	}
	else
	{
		if(-1==masterClk_config())
		{
			printf("masterClk_config failed\n");
			return -1;
		}			
	}

	taskDelay(120);
	
	return 0;	
}

int CLK_configNew()
{
	UINT32 i, id = 0 ;
	char comment[1000];
	int opt[100]={0};
	int iopt=0;
	FILE* fOpts;
	int icnt = 0;
	unsigned int clk_slot;
	int clksource;
	int base_addr;
	int clkConfig = 0;	
	unsigned int data = 0x0;
	unsigned int check_data;	
	int clock_output =1;/*whether to ouput clock to other boards*/


	while(1)
	{
		if ((fOpts = fopen("CLKParas.txt","r")) == NULL)
		{
			icnt++;
			taskDelay(60);
			if(icnt%5==0)
			{
				printf("open file CLKParas.txt failed %d times\n",icnt);
				/*break;*/
			}
		}
		else
		{
			printf("open file CLKParas.txt succ\n");
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

	clk_slot = opt[0];
	
	clk_source	= opt[2]; /*onBoard 0 ;divid 1 ; slave 0xf*/
	clk_latency = opt[3];
	DelayRstData = opt[4];

	printf("clk_slot = %d \n",clk_slot);	
	printf("clk_source = %d \n",clk_source);
	printf("clk_latency = 0x%x \n",clk_latency);
	printf("DelayRstData = 0x%x \n",DelayRstData);

	baseAddr = 0x04f00000;
	printf("base address of clk board is 0x%x \n",baseAddr);
	clksource = clk_source;
	base_addr = baseAddr;

	if(clksource==1)/*divid clock	*/
	{
		unsigned uRead;
		ReadFrmVme32(base_addr +0xf0b0,VA32,&uRead);
		if(uRead!=0x1)
		{
			printf("ETF clock synchronization error!uRead = 0x%x\n",uRead);
			clkConfig = 1;
		}		
	}

	
	switch(clksource)
	{
		case 0:
			WriteToVme32(base_addr +0xf030,VA32, data);
			printf("----onBoard Clock	   : Write to 0x%x Successfully!\n",(base_addr +0xf030));
			taskDelay(1);
			
			ReadFrmVme32(base_addr +0xf030 ,VA32,&check_data);
			if((check_data&0xf)!=0x0)
			{
				printf("CLK config: read check error, check_data = 0x%x, write data = 0x%x \n",check_data,data);
				return -1;
			}
			
			break;
		case 1:
			if(clkConfig)
			{
				WriteToVme32(base_addr +0xf030, VA32, 0x1); 
				printf("----divid Clock 	 : Write to 0x%x Successfully!\n",(base_addr +0xf030));
				taskDelay(1);
				ReadFrmVme32(base_addr +0xf030 ,VA32,&check_data);
				if((check_data&0xf)!=0x1)
				{
					printf("CLK config: read check error, check_data = 0x%x, write data = 0x%x \n",check_data,0x1);
					return -1;
				}
			}
			break;
		case 0xf:
			data = 0xf;
			WriteToVme32(base_addr +0xf030,VA32, data);
			printf("----Slave Clock 	 : Write to 0x%x Successfully!\n",(base_addr +0xf030));
			taskDelay(1);
			
			ReadFrmVme32(base_addr +0xf030 ,VA32,&check_data);
			if((check_data&0xf)!=0xf)
			{
				printf("CLK config: read check error, check_data = 0x%x, write data = 0x%x \n",check_data,data);
				return -1;
			}
			break;
		default:
			printf("clock source error ! \n");
			return -1;

	}


	if(clkConfig)
	{
		if((clksource==1)&&(clock_output))
		{
			WriteToVme32(base_addr +0xf020,VA32, 0x0);
			printf("ETFConfigCLK : write data 0x%x to addr 0x%x \n", 0x0,base_addr +0xf020);
			taskDelay(1);
			
			ReadFrmVme32(base_addr +0xf020 ,VA32,&check_data);	
			taskDelay(300);

			ReadFrmVme32(base_addr +0xf0b0 ,VA32,&check_data);
			if( check_data!= 0x1)
			{
				printf("CLK config: read check error, check_data (0x%x) != 0x1\n",check_data);
				return -1;
			}
				
		}
		if((clksource==1)&&(!clock_output))
		{
			WriteToVme32(base_addr +0xf020,VA32, 0x1); 
			printf("ETFConfigCLK : write data 0x%x to addr 0x%x \n", 0x1,base_addr +0xf020);
			taskDelay(1);		
		}
	}
	taskDelay(120);

	

}

