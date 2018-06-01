#ifndef __INCSDETFh
#define __INCSDETFh

#define ETF_MAX_BYTES_PERMDL 10240
#define MAXFEENUM			16

#define	MCST_ADRS	0x3000000
#define DATA_FORMAT_TRANS 1

struct etf_db_info
{
	int etf_ppc_sum;
	struct GE_ppc  *etf_ppc_header  ;
	
	int etf_tdig_sum;
	struct etf_tdig * etf_TDIG_header[MAXFEENUM];

	int etf_roc_sum;
	struct etf_roc * etf_roc_header ;

	int etf_clk_sum;
	struct etf_clk * etf_clk_header;

};

extern struct etf_db_info  g_ETFDBInfo;

extern unsigned int		g_uEtfCbltGeo[MAXFEENUM];
extern unsigned int		g_uEtfCbltModuleSum;
extern unsigned int		g_uEtfMonitorGeo;

extern unsigned int 	g_uEtfCalibSignAmp;

extern unsigned int 	g_uEtfLDPulseInterval;
extern unsigned int 	g_uEtfLDPulseNum;
extern unsigned int 	g_uEtfLDSolenoid;

extern int 			nETFGenL1TaskId;

extern unsigned int g_nConverTable[MAXFEENUM][9][8];	


void ETFLinkRtns() ;

#endif

