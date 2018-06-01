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
#define HPTDC0_RAM_ADDR 0x10508      /*��FPGA�ڲ���HPTDC0���������ݵ�ƫ�Ƶ�ַ*/
#define HPTDC1_RAM_ADDR 0x10400      /*��FPGA�ڲ���HPTDC1���������ݵ�ƫ�Ƶ�ַ*/
#define HPTDC2_RAM_ADDR 0x10484      /*��FPGA�ڲ���HPTDC2���������ݵ�ƫ�Ƶ�ַ*/
#define HPTDC0_CONTROL_ADDR 0x1055C
#define HPTDC1_CONTROL_ADDR 0x10454
#define HPTDC2_CONTROL_ADDR 0x104D8

#define HPTDC_CHANNEL_BIT_OFFSET 5   /*ͨ��ѡ�����λ��Control Register �е�ƫ�Ƶ�ַ*/
#define HPTDC_KEEPTOKEN_BIT_OFFSET 35
#define HPTDC_REJECT_BIT_OFFSET  48  /*reject count����λ��Setup Register �е�ƫ�Ƶ�ַ*/
#define HPTDC_SEARCH_BIT_OFFSET 60   /*search window����λ��Setup Register �е�ƫ�Ƶ�ַ*/
#define HPTDC_MATCH_BIT_OFFSET 72    /*match window ����λ��Setup Register �е�ƫ�Ƶ�ַ*/
#define HPTDC_TRIG_LATE_OFFSET 138   /*trigger count����λ��Setup Register �е�ƫ�Ƶ�ַ*/
#define HPTDC_MAX_EVENT_OFFSET 116   /*max event size����λ��Setup Register �е�ƫ�Ƶ�ַ*/
#define HPTDC_MATCH_ENABLE_OFFSET 639 /*enable matching ����λ��Setup Register �е�ƫ�Ƶ�ַ*/
#define HPTDC_SETUP_PARITY_OFFSET 646
#define HPTDC_CONTROL_PARITY_OFFSET 39
	/*HPTDC2 ͨ����*/             
#define HPTDC2_CHANNEL_FEE0 	0  /*  ��Ӧ FEEͨ��0  */
#define HPTDC2_CHANNEL_FEE1 	3  /*  ��Ӧ FEEͨ��1  */
#define HPTDC2_CHANNEL_FEE2 	4  /*  ��Ӧ FEEͨ��2  */
#define HPTDC2_CHANNEL_FEE3 	7  /*  ��Ӧ FEEͨ��3  */
#define HPTDC2_CHANNEL_FEE4 	8  /*  ��Ӧ FEEͨ��4  */
#define HPTDC2_CHANNEL_FEE5 	11 /*  ��Ӧ FEEͨ��5  */
#define HPTDC2_CHANNEL_FEE6 	12 /*  ��Ӧ FEEͨ��6  */
#define HPTDC2_CHANNEL_FEE7 	15 /*  ��Ӧ FEEͨ��7  */
#define HPTDC2_CHANNEL_FEE8 	16 /*  ��Ӧ FEEͨ��8  */
#define HPTDC2_CHANNEL_FEE9 	19 /*  ��Ӧ FEEͨ��9  */
#define HPTDC2_CHANNEL_FEE10	20 /*  ��Ӧ FEEͨ��10  */
#define HPTDC2_CHANNEL_FEE11	23 /*  ��Ӧ FEEͨ��11  */
#define HPTDC2_CHANNEL_FEE12 	24 /*  ��Ӧ FEEͨ��12  */
#define HPTDC2_CHANNEL_FEE13 	27 /*  ��Ӧ FEEͨ��13  */
#define HPTDC2_CHANNEL_FEE14 	28 /*  ��Ӧ FEEͨ��14  */
#define HPTDC2_CHANNEL_FEE15 	31 /*  ��Ӧ FEEͨ��15  */
	/*HPTDC0 ͨ����*/ 
#define HPTDC0_CHANNEL_FEE0 0    /*  ��Ӧ FEEͨ��0  */
#define HPTDC0_CHANNEL_FEE1 4  	 /*  ��Ӧ FEEͨ��1  */
#define HPTDC0_CHANNEL_FEE2 8    /*  ��Ӧ FEEͨ��2  */
#define HPTDC0_CHANNEL_FEE3 12   /*  ��Ӧ FEEͨ��3  */
#define HPTDC0_CHANNEL_FEE4 16   /*  ��Ӧ FEEͨ��4  */
#define HPTDC0_CHANNEL_FEE5 20   /*  ��Ӧ FEEͨ��5  */
#define HPTDC0_CHANNEL_FEE6 24   /*  ��Ӧ FEEͨ��6  */
#define HPTDC0_CHANNEL_FEE7 28   /*  ��Ӧ FEEͨ��7  */
	/*HPTDC1 ͨ����*/ 
#define HPTDC1_CHANNEL_FEE8  0   /*  ��Ӧ FEEͨ��8  */
#define HPTDC1_CHANNEL_FEE9  4   /*  ��Ӧ FEEͨ��9  */
#define HPTDC1_CHANNEL_FEE10 8   /*  ��Ӧ FEEͨ��10  */
#define HPTDC1_CHANNEL_FEE11 12  /*  ��Ӧ FEEͨ��11  */
#define HPTDC1_CHANNEL_FEE12 16  /*  ��Ӧ FEEͨ��12  */
#define HPTDC1_CHANNEL_FEE13 20  /*  ��Ӧ FEEͨ��13  */
#define HPTDC1_CHANNEL_FEE14 24  /*  ��Ӧ FEEͨ��14  */
#define HPTDC1_CHANNEL_FEE15 28  /*  ��Ӧ FEEͨ��15  */

#endif

int FeeHptdcConfig(UINT32 uAdrs,UINT32 uChannelEnb,UINT32 bTrgEnb,UINT32 uRejectLatency,
	UINT32 uTrgLatency,UINT32 uTrgSearchWinT, UINT32 uTrgMatchWinT,
	UINT32 uTrgSearchWinQT, UINT32 uTrgMatchWinQT, UINT32 uHitCntPerEvtT, UINT32 uHitCntPerEvtQT);
int MoniHptdcConfigSpec(unsigned int uBaseAddr);

int Char2UInt32(char* p_cData,UINT32 * p_uData,int uLength);
int ParityCheck(char* p_cData);

int VME_INLConfig(UINT32 uBaseAddr,char * cInlval, unsigned uChnlSum);