#ifndef __INCSDMDCh
#define __INCSDMDCh

#include "fw_configinfo.h"

/*MQT amount*/
#define MDC_MQT_AMOUNT 			16
#define MDC_MAX_BYTES_PERMDL 		3072  //max bytes from a read out module

struct mdc_db_info
{
	int ppc_sum ;
	struct GE_ppc *	ppc_header  ;
	int mqt_sum ;
	struct mdc_mqt * 	mqt_header[MDC_MQT_AMOUNT]  ;
	int				mqt_enable[MDC_MQT_AMOUNT]  ;
	int roc_sum ;
	struct mdc_roc * 	roc_header ;
	int mcc_sum ;
	struct mdc_mcc * mcc_header ;
	int mti_sum;
	struct mdc_mti * 	mti_header;
};

struct mdc_hw_info
{
	int mqt_sum ;
	int mqt_geo[MDC_MQT_AMOUNT] ;
} ;


extern struct mdc_db_info  	g_MDCDBInfo ;
extern struct mdc_hw_info 	g_MDCHWInfo ;
extern int 				g_nMqtEnableSum;
extern int					g_nMqtEnableGeo[MDC_MQT_AMOUNT+1];

/*for calibration  leigk061027*/
extern unsigned int 	g_nMQTDAC;
extern unsigned int		g_nMCCT0Latency;

extern int 			nMDCGenL1TaskId;

void MDCLinkRtns() ;

#endif


