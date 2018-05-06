/*
Name： FeeHptdcConfig
Function：将HPTDC的配置数据放到ram中，HPTDC的ID分别为0， 1，2。HPTDC0和HPTDC1用于测量时间，HPTDC2用于测量幅度。
在jtag链中他们的顺序是0->2->1。所以在RAM的存放顺序HPTDC1的配置文件放在RAM的低地址区域。

Definition：
int FeeHptdcConfig (UINT32 uAdrs, UINT32 uChannelEnb, bool bTrgEnb, UINT32 uRejectLatency, UINT32 uTrgLatency, UINT32 uTrgSearchWin, UINT32 uTrgMatchWin,);

Parameter：
	uAdrs: Fee板的基地址。（HPTDC config 存放的Ram的地址的offset应该是0x10000）
	uchannelEnb: 通道使能，共16个通道。Bit15－bit0对应channel15到channel0。1为使能，0为关闭。
		同时控制相应HPTDC的通道。FeeV3.0中，Fee通道和HPTDC通道对应关系为：

Fee通道	  HPDC通道
0	        HPTDC0：c0-3，   HPTDC2：0
1	        HPTDC0：c4-7。   HPTDC2：3
2	        HPTDC0：c8-11。  HPTDC2：4
3	        HPTDC0：c12-15。 HPTDC2：7
4	        HPTDC0：c16-19。 HPTDC2：8
5	        HPTDC0：c20-23。 HPTDC2：11
6	        HPTDC0：c24-27。 HPTDC2：12
7	        HPTDC0：c28-31。 HPTDC2：15
8	        HPTDC1：c0-3。   HPTDC2：16
9	        HPTDC1：c4-7。   HPTDC2：19
10	      HPTDC1：c8-11。  HPTDC2：20
11	      HPTDC1：c12-15。 HPTDC2：23
12	      HPTDC1：c16-19 ，HPTDC2：24
13	      HPTDC1：c20-23。 HPTDC2：27
14	      HPTDC1：c24-27。 HPTDC2：28
15	      HPTDC1：c28-31。 HPTDC2：31
	
bTrgEnb：是否使HPTDC处于trigger模式。1：是。0：否。
uRejectLatency: 触发匹配时抛弃hit的延迟，即该hit时间在这个值以前，就把它丢弃。
	一般设置uRejectLatency比uTrgLatency大4（以25ns为单位）最大值4095

uTrgLatency：触发延迟（以25ns为单位）
uTrgSearchWinT：时间测量电路Trigger Serch Windows，对应HPTDC0和1。一般比uTrgMatchWinT大8。（以25ns为单位）最大值4095
uTrgMatchWinT,：时间测量电路Trigger Match Windows，对应HPTDC0和1。触发匹配窗口（以25ns为单位）最大值4095
uTrgSearchWinQT：电荷测量电路Trigger Search Windows，对应HPTDC2。一般比uTrgMatchWinQT大8。（以25ns为单位）最大值4095
uTrgMatchWinQT,：电荷测量电路Trigger Search Widows ，对应HPTD2。触发匹配窗口（以25ns为单位）最大值4095
uHitCntPerEvtT: 设置时间测量电路每个event的hit个数(2的uHitCntPerEvtT次方) uHitCntPerEvtT最大值9
uHitCntPerEvtQT: 设置电荷测量电路每个event的hit个数(2的uHitCntPerEvtQT次方) uHitCntPerEvtQT最大值9

*/
#include "sd_tof_hptdc_config.h"
#include "fw_staterrspt.h"

#include "stdio.h"
#include "string.h"


#define NEW_HPTDC_CONFIGURE

int FeeHptdcConfig(UINT32 uAdrs,UINT32 uChannelEnb,UINT32 bTrgEnb,UINT32 uRejectLatency,
	UINT32 uTrgLatency,UINT32 uTrgSearchWinT, UINT32 uTrgMatchWinT,
	UINT32 uTrgSearchWinQT, UINT32 uTrgMatchWinQT, UINT32 uHitCntPerEvtT, UINT32 uHitCntPerEvtQT)
{
	char Q_val;
	if(bTrgEnb&0xff)
		Q_val='1';
	else
		Q_val='0';
	//Q_flag=0 Disable Q channel;1 Enable Q Channel;
	//uChannelEnb=0xffff;//lei
	int i;		
	UINT32 uBit;	
	UINT32 uVmeAddr;
	UINT32 uTemp1;
/*	FILE*  fpConfigFile;*/
	
	char cHptdc0SetupData[648], cHptdc0CtrlData[41];
	char cHptdc1SetupData[648], cHptdc1CtrlData[41];
	char cHptdc2SetupData[648], cHptdc2CtrlData[41];
 	UINT32 uHptdc0Setup[21],uHptdc0Ctrl[2];
 	UINT32 uHptdc1Setup[21],uHptdc1Ctrl[2];
 	UINT32 uHptdc2Setup[21],uHptdc2Ctrl[2];		

	cHptdc0SetupData[647]='\0';
	cHptdc1SetupData[647]='\0';
	cHptdc2SetupData[647]='\0';
	cHptdc0CtrlData[40]  ='\0';
	cHptdc1CtrlData[40]  ='\0';
	cHptdc2CtrlData[40]  ='\0';
	/* read hptdc configuratio file
	   HptdcConfigTrg1.hpf for HPTDC0 
	   HptdcConfigTrg2.hpf for HPTDC2
	   HptdcConfigTrg3.hpf for HPTDC1
	*/
/*	fpConfigFile = fopen("HptdcConfigTrg1.hpf","r");
	if(fpConfigFile == NULL)
	{
		printf("Error when opening HptdcConfigTrg1.hpf.\n");
		return -1;
	}
	if(ReadHptdcFile(cHptdc0SetupData,cHptdc0CtrlData,fpConfigFile))
		return -1;
	fclose(fpConfigFile);

	fpConfigFile = fopen("HptdcConfigTrg3.hpf","r");
	if(fpConfigFile == NULL)
	{
		printf("Error when opening HptdcConfigTrg3.hpf.\n");
		return -1;
	}

*/

	#ifndef NEW_HPTDC_CONFIGURE

	cHptdc0SetupData="01111011111111111000000000000000011110000000011110\
00111101110011110000001110110000000000000000000000\
01111010000001001001000101000000000000011011110111\
10000011100000000000000000000000000000000000000000\
00000000000000000000000000000000000000000000000000\
00000000000000000000000000000000000000000000000000\
00000000000000000000000000000000000000000000000000\
00000000000000000000000000000000000000000000000000\
00000000000000000000000000000000000000000000000000\
00000000000000000000010010010010001001001001011011\
01101100010010010011011011011010110110110111111111\
11111111111001010000100000000010000000011101001000\
00000000000000000000000011011111111111110111000";

	cHptdc0CtrlData="1010011111111111111111111111111111111000";

	cHptdc2SetupData="01111011111111111000000000000000000100000100011100\
10001111110011010000000001010000000001111011110110\
01111010101000001001000101000000000000011000111111\
10000011100000000000000000000000000000000000000000\
00000000000000000000000000000000000000000000000000\
00000000000000000000000000000000000000000000000000\
00000000000000000000000000000000000000000000000000\
00000000000000000000000000000000000000000000000000\
00000000000000000000000000000000000000000000000000\
00000000000000000000010010010010001001001001011011\
01101100010010010011011011011010110110110111111111\
11111111111001010000100000000010000000110001001000\
00000000000000000000000011011111111111110111000";

	cHptdc2CtrlData="1010011111111111111111111111111111111000";

	cHptdc1SetupData="0111101111111111100000000000000000010000100001111000111101111110110000001110110000000001111011110110\
01111010101101001001000101000000000000011011110111\
10000011100000000000000000000000000000000000000000\
00000000000000000000000000000000000000000000000000\
00000000000000000000000000000000000000000000000000\
00000000000000000000000000000000000000000000000000\
00000000000000000000000000000000000000000000000000\
00000000000000000000000000000000000000000000000000\
00000000000000000000010010010010001001001001011011\
01101100010010010011011011011010110110110111111111\
11111111111001010000100000000010000000011101001000\
00000000000000000000000011011111111111110111000";

	cHptdc1CtrlData="1010011111111111111111111111111111111000";

	#else



	cHptdc0SetupData="01111011111111111000000000000000011110000000011100\
11110111110011000000000001000000000000000000000000\
01111010000001001001000101000000000000011111011111\
10000011100000000000000000000000000000000000000000\
00000000000000000000000000000000000000000000000000\
00000000000000000000000000000000000000000000000000\
00000000000000000000000000000000000000000000000000\
00000000000000000000000000000000000000000000000000\
00000000000000000000000000000000000000000000000000\
00000000000000000000010010010010001001001001011011\
01101100010010010011011011011010110110110111111111\
11111111111001010000100000000010000000011101001000\
00000000000000000000000011011111111111110111000";

	cHptdc0CtrlData="1010011111111111111111111111111111111000";

	cHptdc2SetupData="01111011111111111000000000000000000100000100011100\
11110111110111010000000101010000000001111011110110\
01111010101000001001000101000000000000011111011111\
10000011100000000000000000000000000000000000000000\
00000000000000000000000000000000000000000000000000\
00000000000000000000000000000000000000000000000000\
00000000000000000000000000000000000000000000000000\
00000000000000000000000000000000000000000000000000\
00000000000000000000000000000000000000000000000000\
00000000000000000000010010010010001001001001011011\
01101100010010010011011011011010110110110111111111\
11111111111001010000100000000010000000110001001000\
00000000000000000000000011011111111111110111000";

	cHptdc2CtrlData="1010011111111111111111111111111111111000";

	cHptdc1SetupData="01111011111111111000000000000000000100001000011100\
11110111110011000000000001000000000001111011110110\
01111010101101001001000101000000000000011111011111\
10000011100000000000000000000000000000000000000000\
00000000000000000000000000000000000000000000000000\
00000000000000000000000000000000000000000000000000\
00000000000000000000000000000000000000000000000000\
00000000000000000000000000000000000000000000000000\
00000000000000000000000000000000000000000000000000\
00000000000000000000010010010010001001001001011011\
01101100010010010011011011011010110110110111111111\
11111111111001010000100000000010000000011101001000\
00000000000000000000000011011111111111110111000";

	cHptdc1CtrlData="1010011111111111111111111111111111111000";


	#endif

	if(bTrgEnb&0xff00)//leigk 080814 for T value relative
	{
		//leigk 070805 for t hit history
		cHptdc0SetupData[124]='1';
		cHptdc1SetupData[124]='1';
		cHptdc2SetupData[124]='1';
	}
	
	/* close all channel */
	for(i=0;i<32;i++)
	{
		cHptdc0CtrlData[i+ HPTDC_CHANNEL_BIT_OFFSET] = '0';
		cHptdc1CtrlData[i+ HPTDC_CHANNEL_BIT_OFFSET] = '0';
		cHptdc2CtrlData[i+ HPTDC_CHANNEL_BIT_OFFSET] = '0';
	}
	/* change channel choice*/	 
	if(uChannelEnb & 0x1)/*Fee channel 0 */
	{
		cHptdc0CtrlData[HPTDC_CHANNEL_BIT_OFFSET + HPTDC0_CHANNEL_FEE0] = '1';
		cHptdc0CtrlData[HPTDC_CHANNEL_BIT_OFFSET + HPTDC0_CHANNEL_FEE0+1] = '1';
		cHptdc0CtrlData[HPTDC_CHANNEL_BIT_OFFSET + HPTDC0_CHANNEL_FEE0+2] = '1';
		cHptdc0CtrlData[HPTDC_CHANNEL_BIT_OFFSET + HPTDC0_CHANNEL_FEE0+3] = '1';
		
		cHptdc2CtrlData[HPTDC_CHANNEL_BIT_OFFSET + HPTDC2_CHANNEL_FEE0] = Q_val;
	}
	if(uChannelEnb & 0x2)/*Fee channel 1 */
	{
		cHptdc0CtrlData[HPTDC_CHANNEL_BIT_OFFSET + HPTDC0_CHANNEL_FEE1] = '1';
		cHptdc0CtrlData[HPTDC_CHANNEL_BIT_OFFSET + HPTDC0_CHANNEL_FEE1+1] = '1';
		cHptdc0CtrlData[HPTDC_CHANNEL_BIT_OFFSET + HPTDC0_CHANNEL_FEE1+2] = '1';
		cHptdc0CtrlData[HPTDC_CHANNEL_BIT_OFFSET + HPTDC0_CHANNEL_FEE1+3] = '1';

		cHptdc2CtrlData[HPTDC_CHANNEL_BIT_OFFSET + HPTDC2_CHANNEL_FEE1] = Q_val;
	}
	if(uChannelEnb & 0x4)/*Fee channel 2 */
	{
		cHptdc0CtrlData[HPTDC_CHANNEL_BIT_OFFSET + HPTDC0_CHANNEL_FEE2] = '1';
		cHptdc0CtrlData[HPTDC_CHANNEL_BIT_OFFSET + HPTDC0_CHANNEL_FEE2+1] = '1';
		cHptdc0CtrlData[HPTDC_CHANNEL_BIT_OFFSET + HPTDC0_CHANNEL_FEE2+2] = '1';
		cHptdc0CtrlData[HPTDC_CHANNEL_BIT_OFFSET + HPTDC0_CHANNEL_FEE2+3] = '1';

		cHptdc2CtrlData[HPTDC_CHANNEL_BIT_OFFSET + HPTDC2_CHANNEL_FEE2] = Q_val;
	}
	if(uChannelEnb & 0x8)/*Fee channel 3 */
	{
		cHptdc0CtrlData[HPTDC_CHANNEL_BIT_OFFSET + HPTDC0_CHANNEL_FEE3] = '1';
		cHptdc0CtrlData[HPTDC_CHANNEL_BIT_OFFSET + HPTDC0_CHANNEL_FEE3+1] = '1';
		cHptdc0CtrlData[HPTDC_CHANNEL_BIT_OFFSET + HPTDC0_CHANNEL_FEE3+2] = '1';
		cHptdc0CtrlData[HPTDC_CHANNEL_BIT_OFFSET + HPTDC0_CHANNEL_FEE3+3] = '1';

		cHptdc2CtrlData[HPTDC_CHANNEL_BIT_OFFSET + HPTDC2_CHANNEL_FEE3] = Q_val;
	}
	if(uChannelEnb & 0x10)/*Fee channel 4 */
	{
		cHptdc0CtrlData[HPTDC_CHANNEL_BIT_OFFSET + HPTDC0_CHANNEL_FEE4] = '1';
		cHptdc0CtrlData[HPTDC_CHANNEL_BIT_OFFSET + HPTDC0_CHANNEL_FEE4+1] = '1';
		cHptdc0CtrlData[HPTDC_CHANNEL_BIT_OFFSET + HPTDC0_CHANNEL_FEE4+2] = '1';
		cHptdc0CtrlData[HPTDC_CHANNEL_BIT_OFFSET + HPTDC0_CHANNEL_FEE4+3] = '1';

		cHptdc2CtrlData[HPTDC_CHANNEL_BIT_OFFSET + HPTDC2_CHANNEL_FEE4] = Q_val;
	}
	if(uChannelEnb & 0x20)/*Fee channel 5 */
	{
		cHptdc0CtrlData[HPTDC_CHANNEL_BIT_OFFSET + HPTDC0_CHANNEL_FEE5] = '1';
		cHptdc0CtrlData[HPTDC_CHANNEL_BIT_OFFSET + HPTDC0_CHANNEL_FEE5+1] = '1';
		cHptdc0CtrlData[HPTDC_CHANNEL_BIT_OFFSET + HPTDC0_CHANNEL_FEE5+2] = '1';
		cHptdc0CtrlData[HPTDC_CHANNEL_BIT_OFFSET + HPTDC0_CHANNEL_FEE5+3] = '1';


		cHptdc2CtrlData[HPTDC_CHANNEL_BIT_OFFSET + HPTDC2_CHANNEL_FEE5] = Q_val;
	}
	if(uChannelEnb & 0x40)/*Fee channel 6 */
	{
		cHptdc0CtrlData[HPTDC_CHANNEL_BIT_OFFSET + HPTDC0_CHANNEL_FEE6] = '1';
		cHptdc0CtrlData[HPTDC_CHANNEL_BIT_OFFSET + HPTDC0_CHANNEL_FEE6+1] = '1';
		cHptdc0CtrlData[HPTDC_CHANNEL_BIT_OFFSET + HPTDC0_CHANNEL_FEE6+2] = '1';
		cHptdc0CtrlData[HPTDC_CHANNEL_BIT_OFFSET + HPTDC0_CHANNEL_FEE6+3] = '1';


		cHptdc2CtrlData[HPTDC_CHANNEL_BIT_OFFSET + HPTDC2_CHANNEL_FEE6] = Q_val;
	}
	if(uChannelEnb & 0x80)/*Fee channel 7 */
	{
		cHptdc0CtrlData[HPTDC_CHANNEL_BIT_OFFSET + HPTDC0_CHANNEL_FEE7] = '1';
		cHptdc0CtrlData[HPTDC_CHANNEL_BIT_OFFSET + HPTDC0_CHANNEL_FEE7+1] = '1';
		cHptdc0CtrlData[HPTDC_CHANNEL_BIT_OFFSET + HPTDC0_CHANNEL_FEE7+2] = '1';
		cHptdc0CtrlData[HPTDC_CHANNEL_BIT_OFFSET + HPTDC0_CHANNEL_FEE7+3] = '1';

		cHptdc2CtrlData[HPTDC_CHANNEL_BIT_OFFSET + HPTDC2_CHANNEL_FEE7] = Q_val;
	}
	
	if(uChannelEnb & 0x100)/*Fee channel 8 */
	{
		cHptdc1CtrlData[HPTDC_CHANNEL_BIT_OFFSET + HPTDC1_CHANNEL_FEE8] = '1';
		cHptdc1CtrlData[HPTDC_CHANNEL_BIT_OFFSET + HPTDC1_CHANNEL_FEE8+1] = '1';
		cHptdc1CtrlData[HPTDC_CHANNEL_BIT_OFFSET + HPTDC1_CHANNEL_FEE8+2] = '1';
		cHptdc1CtrlData[HPTDC_CHANNEL_BIT_OFFSET + HPTDC1_CHANNEL_FEE8+3] = '1';

		cHptdc2CtrlData[HPTDC_CHANNEL_BIT_OFFSET + HPTDC2_CHANNEL_FEE8] = Q_val;
	}
	if(uChannelEnb & 0x200)/*Fee channel 9 */
	{
		cHptdc1CtrlData[HPTDC_CHANNEL_BIT_OFFSET + HPTDC1_CHANNEL_FEE9] = '1';
		cHptdc1CtrlData[HPTDC_CHANNEL_BIT_OFFSET + HPTDC1_CHANNEL_FEE9+1] = '1';
		cHptdc1CtrlData[HPTDC_CHANNEL_BIT_OFFSET + HPTDC1_CHANNEL_FEE9+2] = '1';
		cHptdc1CtrlData[HPTDC_CHANNEL_BIT_OFFSET + HPTDC1_CHANNEL_FEE9+3] = '1';

		cHptdc2CtrlData[HPTDC_CHANNEL_BIT_OFFSET + HPTDC2_CHANNEL_FEE9] = Q_val;
	}
	if(uChannelEnb & 0x400)/*Fee channel 10 */
	{
		cHptdc1CtrlData[HPTDC_CHANNEL_BIT_OFFSET + HPTDC1_CHANNEL_FEE10] = '1';
		cHptdc1CtrlData[HPTDC_CHANNEL_BIT_OFFSET + HPTDC1_CHANNEL_FEE10+1] = '1';
		cHptdc1CtrlData[HPTDC_CHANNEL_BIT_OFFSET + HPTDC1_CHANNEL_FEE10+2] = '1';
		cHptdc1CtrlData[HPTDC_CHANNEL_BIT_OFFSET + HPTDC1_CHANNEL_FEE10+3] = '1';

		cHptdc2CtrlData[HPTDC_CHANNEL_BIT_OFFSET + HPTDC2_CHANNEL_FEE10] = Q_val;
	}
	if(uChannelEnb & 0x800)/*Fee channel 11 */
	{
		cHptdc1CtrlData[HPTDC_CHANNEL_BIT_OFFSET + HPTDC1_CHANNEL_FEE11] = '1';
		cHptdc1CtrlData[HPTDC_CHANNEL_BIT_OFFSET + HPTDC1_CHANNEL_FEE11+1] = '1';
		cHptdc1CtrlData[HPTDC_CHANNEL_BIT_OFFSET + HPTDC1_CHANNEL_FEE11+2] = '1';
		cHptdc1CtrlData[HPTDC_CHANNEL_BIT_OFFSET + HPTDC1_CHANNEL_FEE11+3] = '1';

		cHptdc2CtrlData[HPTDC_CHANNEL_BIT_OFFSET + HPTDC2_CHANNEL_FEE11] = Q_val;
	}
	if(uChannelEnb & 0x1000)/*Fee channel 12 */
	{
		cHptdc1CtrlData[HPTDC_CHANNEL_BIT_OFFSET + HPTDC1_CHANNEL_FEE12] = '1';
		cHptdc1CtrlData[HPTDC_CHANNEL_BIT_OFFSET + HPTDC1_CHANNEL_FEE12+1] = '1';
		cHptdc1CtrlData[HPTDC_CHANNEL_BIT_OFFSET + HPTDC1_CHANNEL_FEE12+2] = '1';
		cHptdc1CtrlData[HPTDC_CHANNEL_BIT_OFFSET + HPTDC1_CHANNEL_FEE12+3] = '1';

		cHptdc2CtrlData[HPTDC_CHANNEL_BIT_OFFSET + HPTDC2_CHANNEL_FEE12] = Q_val;;
	}
	if(uChannelEnb & 0x2000)/*Fee channel 13 */
	{
		cHptdc1CtrlData[HPTDC_CHANNEL_BIT_OFFSET + HPTDC1_CHANNEL_FEE13] = '1';
		cHptdc1CtrlData[HPTDC_CHANNEL_BIT_OFFSET + HPTDC1_CHANNEL_FEE13+1] = '1';
		cHptdc1CtrlData[HPTDC_CHANNEL_BIT_OFFSET + HPTDC1_CHANNEL_FEE13+2] = '1';
		cHptdc1CtrlData[HPTDC_CHANNEL_BIT_OFFSET + HPTDC1_CHANNEL_FEE13+3] = '1';

		cHptdc2CtrlData[HPTDC_CHANNEL_BIT_OFFSET + HPTDC2_CHANNEL_FEE13] = Q_val;
	}
	if(uChannelEnb & 0x4000)/*Fee channel 14 */
	{
		cHptdc1CtrlData[HPTDC_CHANNEL_BIT_OFFSET + HPTDC1_CHANNEL_FEE14] = '1';
		cHptdc1CtrlData[HPTDC_CHANNEL_BIT_OFFSET + HPTDC1_CHANNEL_FEE14+1] = '1';
		cHptdc1CtrlData[HPTDC_CHANNEL_BIT_OFFSET + HPTDC1_CHANNEL_FEE14+2] = '1';
		cHptdc1CtrlData[HPTDC_CHANNEL_BIT_OFFSET + HPTDC1_CHANNEL_FEE14+3] = '1';

		cHptdc2CtrlData[HPTDC_CHANNEL_BIT_OFFSET + HPTDC2_CHANNEL_FEE14] = Q_val;
	}
	if(uChannelEnb & 0x8000)/*Fee channel 15 */
	{
		cHptdc1CtrlData[HPTDC_CHANNEL_BIT_OFFSET + HPTDC1_CHANNEL_FEE15] = '1';
		cHptdc1CtrlData[HPTDC_CHANNEL_BIT_OFFSET + HPTDC1_CHANNEL_FEE15+1] = '1';
		cHptdc1CtrlData[HPTDC_CHANNEL_BIT_OFFSET + HPTDC1_CHANNEL_FEE15+2] = '1';
		cHptdc1CtrlData[HPTDC_CHANNEL_BIT_OFFSET + HPTDC1_CHANNEL_FEE15+3] = '1';

		cHptdc2CtrlData[HPTDC_CHANNEL_BIT_OFFSET + HPTDC2_CHANNEL_FEE15] = Q_val;
	}
	

	/* change enable matching bit */
	if(bTrgEnb)
	{
		cHptdc0SetupData[HPTDC_MATCH_ENABLE_OFFSET]='1';
		cHptdc0SetupData[HPTDC_KEEPTOKEN_BIT_OFFSET]='1';
		cHptdc1SetupData[HPTDC_MATCH_ENABLE_OFFSET]='1';
		cHptdc1SetupData[HPTDC_KEEPTOKEN_BIT_OFFSET]='1';
		cHptdc2SetupData[HPTDC_MATCH_ENABLE_OFFSET]='1';
		cHptdc2SetupData[HPTDC_KEEPTOKEN_BIT_OFFSET]='1';
	}
	else
	{
		cHptdc0SetupData[HPTDC_MATCH_ENABLE_OFFSET]='0';
		cHptdc0SetupData[HPTDC_KEEPTOKEN_BIT_OFFSET]='0';
		cHptdc1SetupData[HPTDC_MATCH_ENABLE_OFFSET]='0';
		cHptdc1SetupData[HPTDC_KEEPTOKEN_BIT_OFFSET]='0';
		cHptdc2SetupData[HPTDC_MATCH_ENABLE_OFFSET]='0';
		cHptdc2SetupData[HPTDC_KEEPTOKEN_BIT_OFFSET]='0';
	}

	/* change reject count offset*/
	uBit = 0x1;
	for(i=0;i<12;i++)
	{
		if((uBit&uRejectLatency)==uBit)
		{
			cHptdc0SetupData[i+HPTDC_REJECT_BIT_OFFSET] = '1';
			cHptdc1SetupData[i+HPTDC_REJECT_BIT_OFFSET] = '1';
			cHptdc2SetupData[i+HPTDC_REJECT_BIT_OFFSET] = '1';
		}
		else
		{
		 	cHptdc0SetupData[i+HPTDC_REJECT_BIT_OFFSET] = '0';
		  	cHptdc1SetupData[i+HPTDC_REJECT_BIT_OFFSET] = '0';
		  	cHptdc2SetupData[i+HPTDC_REJECT_BIT_OFFSET] = '0';
		}
		 uBit = uBit << 1;			
	}
	/* change search window */
	  /* Hptdc0 Hptdc1*/
	uBit = 0x1;
	for(i=0;i<12;i++)
	{
		if((uBit&uTrgSearchWinT)==uBit)
		{
			cHptdc0SetupData[i+HPTDC_SEARCH_BIT_OFFSET] = '1';
			cHptdc1SetupData[i+HPTDC_SEARCH_BIT_OFFSET] = '1';			
		}
		else
		{
		  	cHptdc0SetupData[i+HPTDC_SEARCH_BIT_OFFSET] = '0';
		  	cHptdc1SetupData[i+HPTDC_SEARCH_BIT_OFFSET] = '0';		  
		}
		 uBit = uBit << 1;			
	}
		/* Hptdc2 */
	uBit = 0x1;
	for(i=0;i<12;i++)
	{
		if((uBit&uTrgSearchWinQT)==uBit)
		{
			cHptdc2SetupData[i+HPTDC_SEARCH_BIT_OFFSET] = '1';			
		}
		else
		{
		  	cHptdc2SetupData[i+HPTDC_SEARCH_BIT_OFFSET] = '0';		 	  
		}
		 uBit = uBit << 1;			
	}
	/* change match window */		
	  /* Hptdc0 Hptdc1*/
	uBit = 0x1;
	for(i=0;i<12;i++)
	{
		if((uBit&uTrgMatchWinT)==uBit)
		{
			cHptdc0SetupData[i+HPTDC_MATCH_BIT_OFFSET] = '1';
			cHptdc1SetupData[i+HPTDC_MATCH_BIT_OFFSET] = '1';			
		}
		else
		{
		  	cHptdc0SetupData[i+HPTDC_MATCH_BIT_OFFSET] = '0';
		  	cHptdc1SetupData[i+HPTDC_MATCH_BIT_OFFSET] = '0';		  
		}
		 uBit = uBit << 1;			
	}
		/* Hptdc2 */
	uBit = 0x1;
	for(i=0;i<12;i++)
	{
		if((uBit&uTrgMatchWinQT)==uBit)
		{
			cHptdc2SetupData[i+HPTDC_MATCH_BIT_OFFSET] = '1';			
		}
		else
		{
		  	cHptdc2SetupData[i+HPTDC_MATCH_BIT_OFFSET] = '0';		 	  
		}
		 uBit = uBit << 1;			
	}
	
		 
	
	/* change trigger latency */	
  	uBit = 0x1;
	for(i=0;i<12;i++)
	{
		if((uBit&uTrgLatency)==uBit)
		{
			cHptdc0SetupData[i+HPTDC_TRIG_LATE_OFFSET] = '1';
			cHptdc1SetupData[i+HPTDC_TRIG_LATE_OFFSET] = '1';
			cHptdc2SetupData[i+HPTDC_TRIG_LATE_OFFSET] = '1';
		}
		else
		{
		 	cHptdc0SetupData[i+HPTDC_TRIG_LATE_OFFSET] = '0';
		  	cHptdc1SetupData[i+HPTDC_TRIG_LATE_OFFSET] = '0';
		  	cHptdc2SetupData[i+HPTDC_TRIG_LATE_OFFSET] = '0';
		}
		 uBit = uBit << 1;			
	}
	/* change max event size */
	  /*Hptdc2*/
	if(uHitCntPerEvtQT > 9)
		uTemp1 =9;
	else
		uTemp1 = uHitCntPerEvtQT ;	


	uBit = 0x1;
	for(i=0;i<4;i++)
	{
		if((uBit&uTemp1)==uBit)
		{
			cHptdc2SetupData[i+HPTDC_MAX_EVENT_OFFSET] = '1';					
		}
		else
		{
		  	cHptdc2SetupData[i+HPTDC_MAX_EVENT_OFFSET] = '0';		 		  
		}
		 uBit = uBit << 1;			
	}	
		/*Hptdc0 Hptdc1 */
	if(uHitCntPerEvtT > 9)
		uTemp1 =9;
	else
		uTemp1 = uHitCntPerEvtT ;	
	
	uBit = 0x1;
	for(i=0;i<4;i++)
	{
		if((uBit&uTemp1)==uBit)
		{
			cHptdc0SetupData[i+HPTDC_MAX_EVENT_OFFSET] = '1';
			cHptdc1SetupData[i+HPTDC_MAX_EVENT_OFFSET] = '1';			
		}
		else
		{
		  	cHptdc0SetupData[i+HPTDC_MAX_EVENT_OFFSET] = '0';
		  	cHptdc1SetupData[i+HPTDC_MAX_EVENT_OFFSET] = '0';		  
		}
		 uBit = uBit << 1;			
	}
	/* change parity bit of setup register data*/
	if(ParityCheck(cHptdc0SetupData))
	{
		if(cHptdc0SetupData[646]=='1')
			cHptdc0SetupData[646]='0';
		else
			cHptdc0SetupData[646]='1';
	}
	if(ParityCheck(cHptdc1SetupData))
	{
		if(cHptdc1SetupData[646]=='1')
			cHptdc1SetupData[646]='0';
		else
			cHptdc1SetupData[646]='1';
	}
	if(ParityCheck(cHptdc2SetupData))
	{
		if(cHptdc2SetupData[646]=='1')
			cHptdc2SetupData[646]='0';
		else
			cHptdc2SetupData[646]='1';
	}	

	/*printf("setup data HPTDC0:\n%s\n",cHptdc0SetupData);
	printf("setup data HPTDC1:\n%s\n",cHptdc1SetupData);
	printf("setup data HPTDC2:\n%s\n",cHptdc2SetupData);*/
	
	/* char to data UINT32 
	  write setup register data to board */


	Char2UInt32(cHptdc0SetupData,uHptdc0Setup,647);
	Char2UInt32(cHptdc1SetupData,uHptdc1Setup,647);
	Char2UInt32(cHptdc2SetupData,uHptdc2Setup,647);
	
	uVmeAddr = uAdrs + HPTDC0_RAM_ADDR;/* VME Address*/
	for(i = 0;i<21;i++)
	{
		WriteToVme32(uVmeAddr,VA32,uHptdc0Setup[i]);
		UINT testdata;
		//printf("write uHptdc0Setup 0x%08x to 0x%08x!!\n",uHptdc0Setup[i],uVmeAddr);
		uVmeAddr = uVmeAddr +4;
	}
	uVmeAddr = uAdrs + HPTDC1_RAM_ADDR;
	for(i = 0;i<21;i++)
	{
		WriteToVme32(uVmeAddr,VA32,uHptdc1Setup[i]);
		//printf("write hptdc 0x%x in 0x%x\n",uHptdc1Setup[i],uVmeAddr);	
	/*	printf("hptdc test  %x\n",uHptdc0Setup[i]);*/
		UINT32 testdata;
		int u;
		ReadFrmVme32(uVmeAddr,VA32,&testdata);
		//printf("hptdc read test:0x%x in 0x%x\n",testdata,uVmeAddr);

		uVmeAddr = uVmeAddr +4;
	}
	
	uVmeAddr = uAdrs + HPTDC2_RAM_ADDR;
	for(i = 0;i<21;i++)
	{
		WriteToVme32(uVmeAddr,VA32,uHptdc2Setup[i]);
		//printf("write uHptdc2Setup 0x%08x to 0x%08x!!\n",uHptdc2Setup[i],uVmeAddr);
		UINT32 testdata;
		ReadFrmVme32(uVmeAddr,VA32,&testdata);
		//printf("hptdc read test:0x%x in 0x%x\n",testdata,uVmeAddr);

		uVmeAddr = uVmeAddr +4;
	}
	
	/* char to data UINT32 
	  write control register data to board 
	  在FPGA的RAM中保存6份稍有差别的 Control register 配置数据*/
	/*printf("Control data HPTDC0: \n%s\n",cHptdc0CtrlData);	  */
	/* Hptdc0 control register */ 
	cHptdc0CtrlData[0]='0';
	cHptdc0CtrlData[1]='0';
	cHptdc0CtrlData[2]='1';
	cHptdc0CtrlData[3]='0';
	cHptdc0CtrlData[4]='0';
	cHptdc0CtrlData[37]='1';
	cHptdc0CtrlData[38]='1';
	cHptdc0CtrlData[39]='0';
	if(ParityCheck(cHptdc0CtrlData))
		cHptdc0CtrlData[39] = '1';	
	Char2UInt32(cHptdc0CtrlData,uHptdc0Ctrl,40);
	uVmeAddr = uAdrs + HPTDC0_CONTROL_ADDR;
	WriteToVme32(uVmeAddr,VA32,uHptdc0Ctrl[0]);
	//printf("write hptdc uHptdc0Ctrl[0] 0x%x in 0x%x\n",uHptdc0Ctrl[0],uVmeAddr);//	
	WriteToVme32(uVmeAddr+4,VA32,uHptdc0Ctrl[1]);	  
	//printf("write hptdc uHptdc0Ctrl[1] 0x%x in 0x%x\n",uHptdc0Ctrl[1],uVmeAddr);
	cHptdc0CtrlData[38]='0';
	if(cHptdc0CtrlData[39]=='1')
		cHptdc0CtrlData[39] = '0';
	else
		cHptdc0CtrlData[39] = '1';
	Char2UInt32(cHptdc0CtrlData,uHptdc0Ctrl,40);
	uVmeAddr = uVmeAddr + 8;
	WriteToVme32(uVmeAddr,VA32,uHptdc0Ctrl[0]);
	WriteToVme32(uVmeAddr+4,VA32,uHptdc0Ctrl[1]);		

	cHptdc0CtrlData[37]='0';	
	if(cHptdc0CtrlData[39]=='1')
		cHptdc0CtrlData[39] = '0';
	else
		cHptdc0CtrlData[39] = '1';
	Char2UInt32(cHptdc0CtrlData,uHptdc0Ctrl,40);
	uVmeAddr = uVmeAddr + 8;
	WriteToVme32(uVmeAddr,VA32,uHptdc0Ctrl[0]);
	WriteToVme32(uVmeAddr+4,VA32,uHptdc0Ctrl[1]);	  
	//printf("write hptdc 0x%x in 0x%x\n",uHptdc0Ctrl[1],uVmeAddr+4);
	cHptdc0CtrlData[4]='1';
	if(cHptdc0CtrlData[39]=='1')
		cHptdc0CtrlData[39] = '0';
	else
		cHptdc0CtrlData[39] = '1';
	Char2UInt32(cHptdc0CtrlData,uHptdc0Ctrl,40);
	uVmeAddr = uVmeAddr + 8;
	WriteToVme32(uVmeAddr,VA32,uHptdc0Ctrl[0]);
	WriteToVme32(uVmeAddr+4,VA32,uHptdc0Ctrl[1]);		 

	cHptdc0CtrlData[4]='0';
	if(cHptdc0CtrlData[39]=='1')
		cHptdc0CtrlData[39] = '0';
	else
		cHptdc0CtrlData[39] = '1';
	Char2UInt32(cHptdc0CtrlData,uHptdc0Ctrl,40);
	uVmeAddr = uVmeAddr + 8;
	WriteToVme32(uVmeAddr,VA32,uHptdc0Ctrl[0]);
	WriteToVme32(uVmeAddr+4,VA32,uHptdc0Ctrl[1]);	 

	cHptdc0CtrlData[0]='1';
	if(cHptdc0CtrlData[39]=='1')
		cHptdc0CtrlData[39] = '0';
	else
		cHptdc0CtrlData[39] = '1';
	Char2UInt32(cHptdc0CtrlData,uHptdc0Ctrl,40);
	uVmeAddr = uVmeAddr + 8;
	WriteToVme32(uVmeAddr,VA32,uHptdc0Ctrl[0]);
	WriteToVme32(uVmeAddr+4,VA32,uHptdc0Ctrl[1]);	  

	/* Hptdc1 control register */ 
	cHptdc1CtrlData[0]='0';
	cHptdc1CtrlData[1]='0';
	cHptdc1CtrlData[2]='1';
	cHptdc1CtrlData[3]='0';
	cHptdc1CtrlData[4]='0';
	cHptdc1CtrlData[37]='1';
	cHptdc1CtrlData[38]='1';
	cHptdc1CtrlData[39]='0';
	if(ParityCheck(cHptdc1CtrlData))
			cHptdc1CtrlData[39] = '1';	
	Char2UInt32(cHptdc1CtrlData,uHptdc1Ctrl,40);
	uVmeAddr = uAdrs + HPTDC1_CONTROL_ADDR;
	WriteToVme32(uVmeAddr,VA32,uHptdc1Ctrl[0]);
	WriteToVme32(uVmeAddr+4,VA32,uHptdc1Ctrl[1]);	

	cHptdc1CtrlData[38]='0';
	if(cHptdc1CtrlData[39]=='1')
		cHptdc1CtrlData[39] = '0';
	else
		cHptdc1CtrlData[39] = '1';
	Char2UInt32(cHptdc1CtrlData,uHptdc1Ctrl,40);
	uVmeAddr = uVmeAddr + 8;
	WriteToVme32(uVmeAddr,VA32,uHptdc1Ctrl[0]);
	WriteToVme32(uVmeAddr+4,VA32,uHptdc1Ctrl[1]);	

	cHptdc1CtrlData[37]='0';
	if(cHptdc1CtrlData[39]=='1')
		cHptdc1CtrlData[39] = '0';
	else
		cHptdc1CtrlData[39] = '1';
	Char2UInt32(cHptdc1CtrlData,uHptdc1Ctrl,40);
	uVmeAddr = uVmeAddr + 8;
	WriteToVme32(uVmeAddr,VA32,uHptdc1Ctrl[0]);
	WriteToVme32(uVmeAddr+4,VA32,uHptdc1Ctrl[1]);	

	cHptdc1CtrlData[4]='1';
	if(cHptdc1CtrlData[39]=='1')
		cHptdc1CtrlData[39] = '0';
	else
		cHptdc1CtrlData[39] = '1';
	Char2UInt32(cHptdc1CtrlData,uHptdc1Ctrl,40);
	uVmeAddr = uVmeAddr + 8;
	WriteToVme32(uVmeAddr,VA32,uHptdc1Ctrl[0]);
	WriteToVme32(uVmeAddr+4,VA32,uHptdc1Ctrl[1]);	
	
	cHptdc1CtrlData[4]='0';
	if(cHptdc1CtrlData[39]=='1')
		cHptdc1CtrlData[39] = '0';
	else
		cHptdc1CtrlData[39] = '1';
	Char2UInt32(cHptdc1CtrlData,uHptdc1Ctrl,40);
	uVmeAddr = uVmeAddr + 8;
	WriteToVme32(uVmeAddr,VA32,uHptdc1Ctrl[0]);
	WriteToVme32(uVmeAddr+4,VA32,uHptdc1Ctrl[1]);	
	
	cHptdc1CtrlData[0]='1';
	if(cHptdc1CtrlData[39]=='1')
		cHptdc1CtrlData[39] = '0';
	else
		cHptdc1CtrlData[39] = '1';
	Char2UInt32(cHptdc1CtrlData,uHptdc1Ctrl,40);
	uVmeAddr = uVmeAddr + 8;
	WriteToVme32(uVmeAddr,VA32,uHptdc1Ctrl[0]);
	WriteToVme32(uVmeAddr+4,VA32,uHptdc1Ctrl[1]);

	uVmeAddr = uAdrs + HPTDC1_RAM_ADDR;
	for(i = 0;i<21;i++)
	{
		
	
		UINT32 testdata;
		int u;
		ReadFrmVme32(uVmeAddr,VA32,&testdata);
		//printf("hptdc read test:0x%x in 0x%x\n",testdata,uVmeAddr);

		uVmeAddr = uVmeAddr +4;
	}/**/

	/* Hptdc2 control register */ 
	cHptdc2CtrlData[0]='0';
	cHptdc2CtrlData[1]='0';
	cHptdc2CtrlData[2]='1';
	cHptdc2CtrlData[3]='0';
	cHptdc2CtrlData[4]='0';
	cHptdc2CtrlData[37]='1';
	cHptdc2CtrlData[38]='1';
	cHptdc2CtrlData[39]='0';
	if(ParityCheck(cHptdc2CtrlData))
		cHptdc2CtrlData[39] = '1';	
	Char2UInt32(cHptdc2CtrlData,uHptdc2Ctrl,40);
	uVmeAddr = uAdrs + HPTDC2_CONTROL_ADDR;
	WriteToVme32(uVmeAddr,VA32,uHptdc2Ctrl[0]);
	WriteToVme32(uVmeAddr+4,VA32,uHptdc2Ctrl[1]);

	cHptdc2CtrlData[38]='0';
	if(cHptdc2CtrlData[39]=='1')
		cHptdc2CtrlData[39] = '0';
	else
		cHptdc2CtrlData[39] = '1';
	Char2UInt32(cHptdc2CtrlData,uHptdc2Ctrl,40);
	uVmeAddr = uVmeAddr + 8;
	WriteToVme32(uVmeAddr,VA32,uHptdc2Ctrl[0]);
	WriteToVme32(uVmeAddr+4,VA32,uHptdc2Ctrl[1]);	
	//printf("*****write hptdc uHptdc0Ctrl[0] 0x%x in 0x%x\n",uHptdc2Ctrl[1],uVmeAddr+4);

	UINT32 testdata;
	ReadFrmVme32(uVmeAddr+4,VA32,&testdata);
	//printf("hptdc read test:0x%x in 0x%x\n",testdata,uVmeAddr+4);



	cHptdc2CtrlData[37]='0';
	if(cHptdc2CtrlData[39]=='1')
		cHptdc2CtrlData[39] = '0';
	else
		cHptdc2CtrlData[39] = '1';
	Char2UInt32(cHptdc2CtrlData,uHptdc2Ctrl,40);
	uVmeAddr = uVmeAddr + 8;
	WriteToVme32(uVmeAddr,VA32,uHptdc2Ctrl[0]);
	WriteToVme32(uVmeAddr+4,VA32,uHptdc2Ctrl[1]);	

	cHptdc2CtrlData[4]='1';
	if(cHptdc2CtrlData[39]=='1')
		cHptdc2CtrlData[39] = '0';
	else
		cHptdc2CtrlData[39] = '1';
	Char2UInt32(cHptdc2CtrlData,uHptdc2Ctrl,40);
	uVmeAddr = uVmeAddr + 8;
	WriteToVme32(uVmeAddr,VA32,uHptdc2Ctrl[0]);
	WriteToVme32(uVmeAddr+4,VA32,uHptdc2Ctrl[1]);	

	cHptdc2CtrlData[4]='0';
	if(cHptdc2CtrlData[39]=='1')
		cHptdc2CtrlData[39] = '0';
	else
		cHptdc2CtrlData[39] = '1';
	Char2UInt32(cHptdc2CtrlData,uHptdc2Ctrl,40);
	uVmeAddr = uVmeAddr + 8;
	WriteToVme32(uVmeAddr,VA32,uHptdc2Ctrl[0]);
	WriteToVme32(uVmeAddr+4,VA32,uHptdc2Ctrl[1]);	

	cHptdc2CtrlData[0]='1';
	if(cHptdc2CtrlData[39]=='1')
		cHptdc2CtrlData[39] = '0';
	else
		cHptdc2CtrlData[39] = '1';
	Char2UInt32(cHptdc2CtrlData,uHptdc2Ctrl,40);
	uVmeAddr = uVmeAddr + 8;
	WriteToVme32(uVmeAddr,VA32,uHptdc2Ctrl[0]);
	WriteToVme32(uVmeAddr+4,VA32,uHptdc2Ctrl[1]);
	uVmeAddr = uAdrs + HPTDC1_RAM_ADDR;
	for(i = 0;i<21;i++)
	{
		
	
		UINT32 testdata;
		int u;
		ReadFrmVme32(uVmeAddr,VA32,&testdata);
		//printf("hptdc read test:0x%x in 0x%x\n",testdata,uVmeAddr);

		uVmeAddr = uVmeAddr +4;
	}
	
	taskDelay(10);
	if ( -1 == WriteToVme32(uAdrs, VA32, 0x20) )
	{  
		//VME ERROR!Write 0x20 to 0x%08x error 1
		printf("VME:: ERROR!! Write 0x00000020 to 0x%08x with errors!\n",uAdrs);
		Push2ErrStack(PPCFATAL+TOF_FLG+ATOM_ERR+43,0,0,0,0);		/*tof error 43*/
		return -1;
	}
	//else 
		//printf("VME:: Write 0x00000020 to 0x%08x Successfully!\n",uAdrs);
	taskDelay(10);
	
	if ( -1 == WriteToVme32(uAdrs+8, VA32, 0x1) ) //as set running mode,before running
	{  
		//VME ERROR!Write 0x01 to 0x%08x error 1
		printf("VME:: ERROR!! Write 0x00000001 to 0x%08x with errors!\n",uAdrs+8);
		Push2ErrStack(PPCFATAL+TOF_FLG+ATOM_ERR+44,0,0,0,0);		/*tof error 44*/
		return -1;
	}
	//else 
		//printf("VME:: Write 0x00000001 to 0x%08x Successfully!\n",uAdrs+8);

	return 0;
}
int MoniHptdcConfigSpec(unsigned int uBaseAddr)//for monitor special config
{
	UINT32 MonitorHptdcAddr1=uBaseAddr+0x10550;
	UINT32 MonitorHptdcAddr2=uBaseAddr+0x10558;

	printf("MonitorHptdcAddr1=0x%x\n",MonitorHptdcAddr1);

	UINT32 datatmp=0xffffffff;       
	ReadFrmVme32(MonitorHptdcAddr1, VA32, &datatmp);
	if(datatmp==0xffffffff)
	{
		printf("read Monitor Hptdc config reg 0x10550 err\n");
		Push2ErrStack(PPCFATAL+TOF_FLG+ATOM_ERR+45,0,0,0,0);		/*tof error 45*/
		return -1;
	}
	printf("monitor spec 0x%x, 0x10550=0x%x\n",uBaseAddr,datatmp);
	
	WriteToVme32(MonitorHptdcAddr1, VA32, 0x80000|datatmp);  //Enable Leading and Trailing edge    
	taskDelay(1);	

	datatmp=0xffffffff;
	ReadFrmVme32(MonitorHptdcAddr1, VA32, &datatmp);
	printf("monitor spec 0x10550=0x%x\n",datatmp);
	
	datatmp=0xffffffff;
	ReadFrmVme32(MonitorHptdcAddr2, VA32, &datatmp);
	if(datatmp==0xffffffff)
	{
		printf("read Monitor Hptdc config reg 0x10558 err\n");
		Push2ErrStack(PPCFATAL+TOF_FLG+ATOM_ERR+46,0,0,0,0);		/*tof error 46*/
		return -1;
	}
	taskDelay(1);
	WriteToVme32(MonitorHptdcAddr2, VA32, 0x02000000^datatmp); //Change Parity Bit     
	taskDelay(1);
	if ( -1 == WriteToVme32(uBaseAddr, VA32, 0x20) )//Enb HptdcJtag
	{  
		//VME ERROR!Write 0x20 to 0x%08x error 2
		printf("VME:: ERROR!! Write 0x00000020 to 0x%08x with errors!\n",uBaseAddr);
		Push2ErrStack(PPCFATAL+TOF_FLG+ATOM_ERR+47,0,0,0,0);		/*tof error 47*/
		return -1;
	}
	taskDelay(2);
	
	if ( -1 == WriteToVme32(uBaseAddr+8, VA32, 0x1) ) //as set running mode,before running//Re Configure Hptdc
	{  
		//VME ERROR!Write 0x01 to 0x%08x error 2
		printf("VME:: ERROR!! Write 0x00000001 to 0x%08x with errors!\n",uBaseAddr+8);
		Push2ErrStack(PPCFATAL+TOF_FLG+ATOM_ERR+48,0,0,0,0);		/*tof error 48*/
		return -1;
	}
	taskDelay(2);
	return 0;

}


/* Char2UInt32
   将字符串转换为无符号整数，每隔32个字符按2进制转换为一个无符号整数 
   如果成功，则返回 0，如果失败，则返回 -1
   */
int Char2UInt32(char* p_cData,UINT32* p_uData,int uLength)
{
	char cBuf[33];	
	int i;
	int j;
	cBuf[32]='\0';
	if(p_cData==NULL)
		return -1;
	j = 0;
	for(i=0;i<uLength-32;)
	{
		strncpy(cBuf,&p_cData[i],32);	
		p_uData[j++]=(UINT32)strtoul(cBuf,NULL,2);
		i+=32;
	}
	strncpy(cBuf,&p_cData[i],uLength-i);
	for(i=uLength-i;i<32;i++)
		cBuf[i] = '0';
	p_uData[j] = (UINT32)strtoul(cBuf,NULL,2);
	return 0;	
}
/* Parity Check 
   统计字符串 p_cData中'1'的个数，
   如果是偶数个'1'，则返回0，如果是奇数个'1'，则返回1
   */
int  ParityCheck(char* p_cData)
{
	int i,j;
	j = 0;	
	for(i=0;i<(int)strlen(p_cData);i++)
	{
		if(p_cData[i]=='1')
			j = j+1;
	}
	j = j%2;	
	return j;
}

int VME_INLConfig(UINT32 uBaseAddr,char * cInlval,unsigned uChnlSum)
{	

	UINT32 nRamAdrs;
	UINT    Inlvalue;
	if (-1==WriteToVme32(uBaseAddr, VA32, 0x0)) 
	{
		//VME ERROR When cancel protection
		printf("VME:: ERROR!! When canceling the protection of the ram for INL LUT!\n"); 
		Push2ErrStack(PPCFATAL+TOF_FLG+ATOM_ERR+49,0,0,0,0);		/*tof error 49*/
		return -1;
	} 	
	nRamAdrs=uBaseAddr+0x30000;
	//printf("nRamAdrs=%08x\n",nRamAdrs);

	int j = 0, k = 0;
	int DataMask = 0;
	int Cnt = 0;
	for (j = 0; j < 1024*uChnlSum; j++) 
	{
		/*由于ram中只取低8bit的数据,低9bit为奇偶校验bit*/
		/*奇偶位计算，要统计1的个数*/
		Inlvalue=cInlval[j];
		Cnt = 0;
		DataMask = 1;
		for (k = 0; k < 8; k ++) 
              {
			if (DataMask & Inlvalue)
				Cnt ++;
			DataMask = DataMask * 2;					
		}
		if (0 == (Cnt % 2)) 
		{ /*偶数,a negative number if nDivisor is negative. */
			
			Inlvalue= Inlvalue | 256; /*第9bit为1)*/
			/*printf("%d \n", InlValue);*/
		}
		else 
		{
			Inlvalue= Inlvalue & 255; /*第9bit为0)*/
			/*printf("%d \n", InlValue);*/
		}			
		/*Inl的值应该在7bit以内*/
		Inlvalue= Inlvalue & 511;						
		 /*写入VME*/

		if ( -1 == WriteToVme32(nRamAdrs, VA32, Inlvalue) ) 
		{
			printf("\n");
			printf(" A32 Write fail"); 
			Push2ErrStack(PPCFATAL+TOF_FLG+ATOM_ERR+50,0,0,0,0);		/*tof error 50*/
			return -1;
		} 

		nRamAdrs = nRamAdrs + 4;
	} 
	//printf("VME::One INL Configured Success!\n");
	return 0;
}


