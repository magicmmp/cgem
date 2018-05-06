#ifndef __INCSDEMCh
#define __INCSDEMCh

//hi,jak

extern SEM_ID	g_sidTCBAInterrupt ;		/*waiting for TCBA interrupt*///byjak
#ifndef EMC_TCBA_INT_LEVEL
#define EMC_TCBA_INT_LEVEL 6//byjak 4->6
#endif
#ifndef EMC_TCBA_INT_VEC
#define EMC_TCBA_INT_VEC    128//byjak 64->128
#endif
extern UINT32 	g_nMaxTCBADMABytes;
extern UINT32 	g_nTCBADMAAdrs ;
//bye,jak
extern UINT32 	g_nTCheckAmpMode ;

#define EMC_MAX_BYTES_PERMDL 512

struct emc_db_info
{
	int emc_ppc_sum;
	struct GE_ppc  *emc_ppc_header  ;	
	int emc_Q_sum;
	struct emc_Q * emc_Q_header[16];
	int emc_roc_sum;
	struct emc_roc * emc_roc_header ;
	int emc_fanout_sum;
	struct emc_fanout * emc_fanout_header;
	int emc_trgtcba_sum;
	struct emc_trgtcba* emc_trgtcba_header;
	int emc_monitor_sum;
	struct emc_monitor * emc_monitor_header;	
};

extern struct emc_db_info  g_EMCDBInfo;

extern unsigned int		g_nEmcDAC;

extern int 			nEMCGenL1TaskId;

extern int nMonitorStep ;	//for EMCmonitor
extern int nLEDCount ;

extern unsigned int QDisableGeo[16];
extern unsigned int nDisableIdx ;

void EMCLinkRtns() ;

#endif

