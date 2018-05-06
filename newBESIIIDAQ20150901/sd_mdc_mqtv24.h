/* this prevents this header from being included more than once */
#ifndef __INCMQTV24h
#define __INCMQTV24h
#define MQT_PRINT
#undef MQT_PRINT /*zhuht*/
/****************************************************/
/*      global definition                           */ 
/****************************************************/
#include "5100_test.h"
#include "stdio.h"
#include "taskLib.h"

#include "sd_mdc.h"
/****************************************************/
/*        MCC module                                */
/****************************************************/
#define MCCBASEADDRESS 0x3
#define MCCRESET 0x0
#define MCCCALIBRATION 0x8000
#define MCCCLOCKENABLE 0x30000
#define MCCT0DELAY 0x18000
#define MCCRUN 0x20000

void mccinit(UINT32 nBase);
void mccstart(UINT32 nBase);
void mcct0set(UINT32 delaytime,UINT32 nBase);

/****************************************************/
/*        JTAG module                               */
/****************************************************/
int JTAGStep(UINT32 nVmeBusAdrs,UINT16 data_write,UINT16 *data_read);
int JTAGSoftReset(UINT32 address);
int JTAGSetIR(UINT32 address,UINT32 *input_IR,UINT32 *output_IR,UINT32 length_IR);
int JTAGSetDR(UINT32 address,UINT32 *input_DR,UINT32 *output_DR,UINT32 length_DR);
int JTAGSetData(UINT32 address, UINT32 *input_IR,UINT32 *output_IR,UINT32 length_IR,UINT32 *input_DR,UINT32 *output_DR,UINT32 length_DR);

/****************************************************/
/*      mqt module                                  */
/****************************************************/
/* parameters*/
#define QTRIGGERLATENCY 245 //0~255 1=25ns
#define QSAMPLEWINDOW 63 //0~63 1=25ns
#define QBASEAREA 0x0 //0~0xffff
#define QTHRESHOLD 0 //0~0xffff 
#define TENABLE 0xffffffff //All channel enable
#define TTRIGGERLATENCY 245 //1=25ns
#define TSAMPLEWINDOW 24 //1=25ns
#define TLOWTHRESHOLD 400 //0~0xfff
#define THIGHTHRESHOLD 500 //0~0xfff
#define PREAMPVOLTAGE 3000 //0~0xfff
#define TDATANUMBER 0x0
#define STATUSREADENABLE 0x0
#define TQMATCHENBLE 0x0
#define NUMBERINONECBLT 0x1
#define SINGLEBOARDCBLTCONTROL 0x55

/*mother board*/
//----------------------------------------------------
//addresses


#define MQTRESET 0x7f
#define MQTWORKENABLE 0x7e
#define MQTHPTDCREG 0x7d
#define MQTDACCONTROL 0x7c
#define MQTCRCSRMIRROR 0x7b
#define MQTSTATUSREG 0x7a
#define MQTQSBACCESS 0x79
#define MQTTDC 0x78
#define MQTVMEJTAG 0x77
#define MQTTRIGBOARD 0x76

//functions
void mqtboardreset(UINT32 nBaseAddress);
void readmqtstatusreg(UINT32 nBaseAddress, UINT32 *pData);
void setworkenable(UINT32 nBaseAddress);
void setworkdisable(UINT32 nBaseAddress);

/*HPTDC T parameter sub function address*/
//----------------------------------------------------
//functions
int hptdcupload(UINT32 address, UINT32 enable, UINT32 window, UINT32 trigger_latency);

/*dac sub function address*/
//----------------------------------------------------
//addresses
#define TLOWTHRESHOLDAD 0x1
#define THIGHTHRESHOLDAD 0x2
#define PREAMPVOLTAGEAD 0x3

//functions
void settlowthreshold(UINT32 nBaseAddress, UINT32 nData);
void setthighthreshold(UINT32 nBaseAddress, UINT32 nData);
void setpreampvoltage(UINT32 nBaseAddress, UINT32 nData);

/*crcsr mirror space sub function address*/
//----------------------------------------------------
//addresses
#define BOARDIDREG 0x3c
#define CBLTCONTROL1REG 0x84
#define CBLTCONTROL2REG 0x88
#define CBLTANDMCSTCRCSR1REG 0x7c
#define CBLTCSR2REG 0x80

//functions
int getcrateposition(UINT32 *singleposition, UINT32 allposition[]);
void setcbltcontrol1(UINT32 nBaseAddress, UINT32 nData);
void setcbltcontrol2(UINT32 nBaseAddress, UINT32 nData);
void setcbltandmcstcrcsr1(UINT32 nBaseAddress, UINT32 nData);
void readcbltcsr2(UINT32 nBaseAddress, UINT32 *pData);

/*test trigger board signal*/
//----------------------------------------------------
//addresses
#define TBWRLAD 0x1
#define TBWRHAD 0x2
#define TBWRCTLAD 0x3
#define TBRDCTLAD 0x4

//functions
void testtbwrl(UINT32 nBaseAddress, UINT32 nData);
void testtbwrh(UINT32 nBaseAddress, UINT32 nData);
void testtbwctl(UINT32 nBaseAddress, UINT32 nData);
void testtbrdctl(UINT32 nBaseAddress, UINT32 *pData);

/*q small board sub function address*/
//----------------------------------------------------
//addresses
#define QTRIGGERLATENCYREG 0x1
#define QSAMPLEWINDOWREG 0x2
#define QSBERRORREG 0x3
#define QTHRESHOLDREG 0x4
#define QBASEVALUEREG 0x5
#define QWAVEFORM 0x6
#define QDATA 0x7

//functions
void setqsbtriggerlatency(UINT32 nBaseAddress, UINT32 nBoardnumber, UINT32 nData);
void setqsbsamplewindow(UINT32 nBaseAddress, UINT32 nBoardnumber, UINT32 nData);
void readqsberrorreg(UINT32 nBaseAddress, UINT32 nBoardnumber, UINT32 *pData);
void setqsbthreshold(UINT32 nBaseAddress, UINT32 nChannel, UINT32 nData);
void setqsbbasevalue(UINT32 nBaseAddress, UINT32 nChannel, UINT32 nData);
void readqsbwaveformdata(UINT32 nBaseAddress, UINT32 nChannel, UINT32 *pData);
void readqsbqdata(UINT32 nBaseAddress, UINT32 nChannel, UINT32 *pData);

/***********************************************************/
/* function modules                                        */
/***********************************************************/
int getcrateposition(UINT32 *singleposition, UINT32 allposition[]);
void mqtv24init(UINT32 nBaseAddress);
void readt(UINT32 nBaseAddress,UINT32 timedelay);
void readwaveform(UINT32 nBaseAddress,UINT32 timedelay);
void cbltinit(UINT32 nBaseAddress);
void cbltwork(UINT32 nBaseAddress,UINT32 timedelay);
void mcst1(void);
void testtriggerboard(UINT32 nBaseAddress);

#endif /* __INCMQTV24h */
