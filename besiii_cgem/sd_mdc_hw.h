#ifndef __INCSDMDCHWh
#define __INCSDMDCHWh

/* Macros for MCC*/
#define MDC_MCC_RESET_ADRS		0X000000
#define MDC_MCC_CRYST_ON_ADRS 	0X030000
#define MDC_MCC_RESET_TDC_ADRS	0X010000
#define MDC_MCC_CALIB_ADRS		0X008000
#define MDC_MCC_SET_DATA_ADRS	0X018000
#define MDC_MCC_START_ADRS		0X020000
#define MDC_MCC_SET_CHECK_ADRS	0X028000
#define MDC_MCC_ONLINE_ADRS		0X040000
#define MDC_MCC_RELOAD_ADRS		0X038000

/*Macros for ROC*/
#define MDC_ROC_RESET_ADRS	0X070000 //0X060000
#define MDC_ROC_SET_N_ADRS	0X060000 //0X020000
#define MDC_ROC_READ_N_ADRS	0X060000 //0X020000
#define MDC_ROC_ENABLE_INT	0X050000 //0X040000
#define MDC_ROC_INT_LEVEL		5
#define MDC_ROC_INT_VECTOR	0X40

/*Macros for MTI*/
#define MDC_MTI_RESET_ADRS 0X060000
#define MDC_MTI_ENABLE_ADRS 0X020000
#define MDC_MTI_FULL_ADRS 0X040000

/*Macros for MQT*/
#define MDC_MQT_FUNC_ADRS(x)			((x) << 12) 	/*generate the function address*/
#define MDC_MQT_FUNC_SUB_ADRS(x)		((x) << 4) 	/*generate the function sub address*/
#define MDC_MQT_QSB_FUNC_ADRS(x) 		((x) << 9) 	/*generate the Qsb function  address*/
#define MDC_MQT_QSB_FUNC_SUB_ADRS(x)	((x) << 4)	/*generate the Qsb function sub address*/
#define MDC_MQT_SUB_AMOUNT 4 /*max amount of sub boards per MQT*/

#define MDC_MQT_CONFIG_DELAY_TICKS	2

/*MQT registers*/
#define MDC_MQT_RESET_ADDR 0X7F
#define MDC_MQT_STATUS_ADDR 0X7A

#define MDC_MQTVMEJTAG 0x77

#define MDC_MQT_WORKENABLE 0x7e 
#define MDC_MQT_STATUS 0X7A

#define MDC_MQT_CBLT_ADRS 0x01000000 
#define MDC_MQT_MCST_CBLT_REST 0x02000000 
#define MDC_MQT_MCST_CRATE_RESET 0x03000000 
#define MDC_MQT_MCST_WORKENABLE 0x04000000 

#define MDC_MQT_CRCSR_SPACE 0x7b
#define MDC_MQT_ID_REG 0X3c
#define MDC_MQT_CBLT_REG1 0X84
#define MDC_MQT_CBLT_REG2 0X88
#define MDC_MQT_CBLT_REG3 0X7C

#define MDC_MQT_DAC_ACCESS 0x7c
#define MDC_MQT_DAC_LOW 0X1
#define MDC_MQT_DAC_HIGH 0X2
#define MDC_MQT_DAC_PREAMP 0X3

#define MDC_MQT_SUB_ACCESS_ADDR 0x79
#define MDC_MQT_SUB_SET_QL1 0X01
#define MDC_MQT_SUB_SAMPLE_WINDOW 0X02
#define MDC_MQT_SUB_ERR_REG 0x3
#define MDC_MQT_SUB_BASELINE 0x5
#define MDC_MQT_SUB_THRESHOLD 0x4
#define MDC_MQT_WAVE_READ 0x06


#define MDC_ADRS_WITH_GEO(SlotNum, VmeAdrs)		(((SlotNum) << 19)|(VmeAdrs))

void 	MDCRocIntHandler() ;
int 		MDCConfigIntCBLT();

void 	MDCMCCReset(int nSlot) ;

void 	MDCMCCSetData(int nSlot, UINT32 nValue) ;

void 	MDCMCCCalib(int nSlot) ;

void 	MDCMCCCrystOn(int nSlot) ;

void 	MDCMCCResetTDC(int nSlot) ;

void 	MDCMCCOnline(int nSlot) ;

void 	MDCMCCReload(int nSlot) ;

void 	MDCMCCStart(int nSlot) ;

void 	MDCMCCResetCheck(int nSlot);


void 	MDCROCReset(int nSlot) ;

void 	MDCROCSetN(int nSlot, UINT32 nValue) ;

UINT32  	MDCROCReadN(int nSlot) ;

void 	MDCROCEnableInt(int nSlot) ;
/*
int		MDCMQTResetMainBd();
int		MDCMQTEnsureClockInput();
int		MDCMQTHptdcInit();
int		MDCMQTSetupQL1latency();
int		MDCMQTSubBdQSampWin();
int		MDCMQTSubBdBaseLine();
int		MDCMQTSubBdQthresh();
int		MDCMQTSetCBLTReg();
int		MDCMQTSetTimeThresh();
int		MDCMQTPreAmpVolDAC();
int		MDCMQTSetWorkEnable();
int		MDCMQTChkWorkEnReg();
*/
int 	MDCConfigROC();
int 	MDCMQTConfigAll();
int 	MDCConfigMCC(int geo,UINT uT0Latency);
void 	MDCConfigMTI(int geo);

int 	MDCConfigMCCLoad(int geo,UINT uT0Latency);
int 	MDCConfigMCCSyn1(int geo,UINT uT0Latency);
int 	MDCConfigMCCSyn3(int geo,UINT uT0Latency);
void 	MDCConfigMTILoad(int geo);

int 	MDCSetMQTNewDAC(int nMQTPosition);
int 	MDCClrMQTL1(int nMQTPosition);
#endif
