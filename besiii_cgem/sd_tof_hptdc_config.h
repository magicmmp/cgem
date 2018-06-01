/* include */
#include "5100_test.h"
#include "stdio.h"
#include "string.h"
#include "stdlib.h"
#include "taskLib.h"


#ifndef HPTDC_CONFIGURE_AREA
#define HPTDC_CONFIGURE_AREA

/* define */
#define HPTDC_RAM_OFFSET  0x10000    
#define HPTDC0_RAM_ADDR 0x10508      /*在FPGA内部，HPTDC0的配置数据的偏移地址*/
#define HPTDC1_RAM_ADDR 0x10400      /*在FPGA内部，HPTDC1的配置数据的偏移地址*/
#define HPTDC2_RAM_ADDR 0x10484      /*在FPGA内部，HPTDC2的配置数据的偏移地址*/
#define HPTDC0_CONTROL_ADDR 0x1055C
#define HPTDC1_CONTROL_ADDR 0x10454
#define HPTDC2_CONTROL_ADDR 0x104D8

#define HPTDC_CHANNEL_BIT_OFFSET 5   /*通道选择比特位在Control Register 中的偏移地址*/
#define HPTDC_KEEPTOKEN_BIT_OFFSET 35
#define HPTDC_REJECT_BIT_OFFSET  48  /*reject count比特位在Setup Register 中的偏移地址*/
#define HPTDC_SEARCH_BIT_OFFSET 60   /*search window比特位在Setup Register 中的偏移地址*/
#define HPTDC_MATCH_BIT_OFFSET 72    /*match window 比特位在Setup Register 中的偏移地址*/
#define HPTDC_TRIG_LATE_OFFSET 138   /*trigger count比特位在Setup Register 中的偏移地址*/
#define HPTDC_MAX_EVENT_OFFSET 116   /*max event size比特位在Setup Register 中的偏移地址*/
#define HPTDC_MATCH_ENABLE_OFFSET 639 /*enable matching 比特位在Setup Register 中的偏移地址*/
#define HPTDC_SETUP_PARITY_OFFSET 646
#define HPTDC_CONTROL_PARITY_OFFSET 39
	/*HPTDC2 通道号*/             
#define HPTDC2_CHANNEL_FEE0 	0  /*  对应 FEE通道0  */
#define HPTDC2_CHANNEL_FEE1 	3  /*  对应 FEE通道1  */
#define HPTDC2_CHANNEL_FEE2 	4  /*  对应 FEE通道2  */
#define HPTDC2_CHANNEL_FEE3 	7  /*  对应 FEE通道3  */
#define HPTDC2_CHANNEL_FEE4 	8  /*  对应 FEE通道4  */
#define HPTDC2_CHANNEL_FEE5 	11 /*  对应 FEE通道5  */
#define HPTDC2_CHANNEL_FEE6 	12 /*  对应 FEE通道6  */
#define HPTDC2_CHANNEL_FEE7 	15 /*  对应 FEE通道7  */
#define HPTDC2_CHANNEL_FEE8 	16 /*  对应 FEE通道8  */
#define HPTDC2_CHANNEL_FEE9 	19 /*  对应 FEE通道9  */
#define HPTDC2_CHANNEL_FEE10	20 /*  对应 FEE通道10  */
#define HPTDC2_CHANNEL_FEE11	23 /*  对应 FEE通道11  */
#define HPTDC2_CHANNEL_FEE12 	24 /*  对应 FEE通道12  */
#define HPTDC2_CHANNEL_FEE13 	27 /*  对应 FEE通道13  */
#define HPTDC2_CHANNEL_FEE14 	28 /*  对应 FEE通道14  */
#define HPTDC2_CHANNEL_FEE15 	31 /*  对应 FEE通道15  */
	/*HPTDC0 通道号*/ 
#define HPTDC0_CHANNEL_FEE0 0    /*  对应 FEE通道0  */
#define HPTDC0_CHANNEL_FEE1 4  	 /*  对应 FEE通道1  */
#define HPTDC0_CHANNEL_FEE2 8    /*  对应 FEE通道2  */
#define HPTDC0_CHANNEL_FEE3 12   /*  对应 FEE通道3  */
#define HPTDC0_CHANNEL_FEE4 16   /*  对应 FEE通道4  */
#define HPTDC0_CHANNEL_FEE5 20   /*  对应 FEE通道5  */
#define HPTDC0_CHANNEL_FEE6 24   /*  对应 FEE通道6  */
#define HPTDC0_CHANNEL_FEE7 28   /*  对应 FEE通道7  */
	/*HPTDC1 通道号*/ 
#define HPTDC1_CHANNEL_FEE8  0   /*  对应 FEE通道8  */
#define HPTDC1_CHANNEL_FEE9  4   /*  对应 FEE通道9  */
#define HPTDC1_CHANNEL_FEE10 8   /*  对应 FEE通道10  */
#define HPTDC1_CHANNEL_FEE11 12  /*  对应 FEE通道11  */
#define HPTDC1_CHANNEL_FEE12 16  /*  对应 FEE通道12  */
#define HPTDC1_CHANNEL_FEE13 20  /*  对应 FEE通道13  */
#define HPTDC1_CHANNEL_FEE14 24  /*  对应 FEE通道14  */
#define HPTDC1_CHANNEL_FEE15 28  /*  对应 FEE通道15  */

#endif

int FeeHptdcConfig(UINT32 uAdrs,UINT32 uChannelEnb,UINT32 bTrgEnb,UINT32 uRejectLatency,
	UINT32 uTrgLatency,UINT32 uTrgSearchWinT, UINT32 uTrgMatchWinT,
	UINT32 uTrgSearchWinQT, UINT32 uTrgMatchWinQT, UINT32 uHitCntPerEvtT, UINT32 uHitCntPerEvtQT);
int MoniHptdcConfigSpec(unsigned int uBaseAddr);

int Char2UInt32(char* p_cData,UINT32 * p_uData,int uLength);
int ParityCheck(char* p_cData);

int VME_INLConfig(UINT32 uBaseAddr,char * cInlval, unsigned uChnlSum);