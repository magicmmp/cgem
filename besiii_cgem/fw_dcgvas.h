/*fw_dcgvas.h, Declarations of Common Global Variants for All Subsystems*/
#ifndef __INCFWDCGVASh
#define __INCFWDCGVASh

#include "msgQLib.h"
#include "rng_dma.h"
#include "fw_define.h"
#include "fragheaders.h"

typedef int (*PFunc) (int nArg1) ;

extern UINT32		g_nPPCIDOfSRH;				/*value of the jump*/	
extern UINT32		g_nCrateID ;					/*Crate ID*/
extern UINT32		g_nSysID ;		/*System ID*/
extern UINT32		g_nRunMode ;
extern UINT32 	g_nTrgPerInt  ;				/*triggers per interrupt*/
extern UINT32 	g_nInterruptCounter  ;  		/*interrupt counter*/
extern UINT32		g_nTrgCounter ;				/*trigger number*/
extern UINT32 	g_nEventsSent ;

extern UINT32 	g_nMdlSum ;
extern UINT32		g_nMaxCbltBytes  ;
extern UINT32 	g_nCbltAdrs ;

extern UINT32	g_nmsgQnum1 ;
extern UINT32	g_nmsgQnum2 ;

/*framework actions*/
extern PFunc 	Actions[MAX_STATES_AMOUNT][MAX_CMD_AMOUNT] ;
/*global functions*/
extern PFunc 	g_pfnInitializedLOAD ;
extern PFunc 	g_pfnLoadedPRCF ;
extern PFunc 	g_pfnPreconfigedCONF ;
extern PFunc 	g_pfnConfigedSYN1 ;
extern PFunc 	g_pfnSynchronized1SYN2  ;
extern PFunc 	g_pfnSynchronized2SYN3 ;
extern PFunc 	g_pfnSynchronized3SYN4 ;
extern PFunc 	g_pfnSynchronized4PREP  ;
extern PFunc 	g_pfnReadySATR;
extern PFunc 	g_pfnRunningSPTR ;/*pointer pointing to the function that exectues subsystem-dependent RunningSPTR*/
extern PFunc 	g_pfnReadySTOP ;	/*pointer pointing to the function that exectues subsystem-dependent ReadySTOP*/
extern PFunc 	g_pfnConfigedUNCF;
extern PFunc 	g_pfnPreconfigedUNPC ;
extern PFunc 	g_pfnLoadedUNLD ;
extern PFunc 	g_pfnAnyEXIT ;			/*pointer pointing to the function that exectues subsystem-dependent LoadedConf*/

/*in g_pfnFWEnteringCbltTrans, 3 primary steps must be done: 1. evaluate g_nMaxCbltBytes, 2. sysIntEnable, 3. evaluate g_nCbltAdrs*/
extern PFunc 		g_pfnFWEnteringCbltTrans ;
extern PFunc 		g_pfnFWBeforeInterrupt ;
extern PFunc 		g_pfnFWBeforeDMARun ;
extern PFunc 		g_pfnFWAfterDMARun  ;
/*for socket connect*/
extern int 		g_sktListen ;				/*socket for listening clients*/
extern int   		g_sktCmdAck;			/*socket for transfering command*/
extern int 		g_sktStaInfo;			/*socket for transfering status*/
extern int 			g_sktData; 				/*socket for transfering data*/
extern struct tcpcb*	g_DataTcpcb;

extern int 		g_nCurnState;
extern volatile int 	gv_nRunningCtrl ; /*running tasks control flag*/

extern SEM_ID 	g_sidSATR ;			/*waiting for SATR*/
extern SEM_ID 	g_sidInterrupt ;		/*waiting for interrupt*/
extern SEM_ID 	g_sidBufEmpty ;		/*waiting for the network buffer to be empty*/

extern SEM_ID 	g_sidStaErrSend ;	      /*mutex for send error infor*/
extern SEM_ID 	g_sidStaErrTrig ;
extern WDOG_ID 	g_widStatusTimer ;
extern int			g_nReportOn;
extern bool		g_bStatusSend;
extern bool		g_bShutdownOpt;

extern DMA_RING_ID	g_DmaRng;		/*a ring buffer storing raw data to be packed*/
extern DMA_RING_ID	g_NetRng ;		/*a ring buffer storing packed data to be sent to online*/

extern MSG_Q_ID		g_msgQcblt;		/*a message queue controlling g_DmaRng */
extern MSG_Q_ID		g_msgQnet ;			/*a message queue controlling g_NetRng*/

extern RODHeader g_fwRODHead;
extern RODTrailer  g_fwRODTail;


extern const char *g_strCmd[MAX_CMD_AMOUNT];
extern const char *g_strState[MAX_STATES_AMOUNT] ;

extern struct Header * g_pCrateConfigHeader ;

extern UINT 		g_nConvertTable[MAX_SLOT_AMOUNT_PLUS_ONE][MAX_CHNL_AMOUNT];
extern float		g_low_fAjust_a2[MAX_SLOT_AMOUNT_PLUS_ONE][MAX_CHNL_AMOUNT];
extern float		g_low_fAjust_a1[MAX_SLOT_AMOUNT_PLUS_ONE][MAX_CHNL_AMOUNT];
extern float		g_low_fAjust_a0[MAX_SLOT_AMOUNT_PLUS_ONE][MAX_CHNL_AMOUNT];
extern float		g_mid_fAjust_a2[MAX_SLOT_AMOUNT_PLUS_ONE][MAX_CHNL_AMOUNT];
extern float		g_mid_fAjust_a1[MAX_SLOT_AMOUNT_PLUS_ONE][MAX_CHNL_AMOUNT];
extern float		g_mid_fAjust_a0[MAX_SLOT_AMOUNT_PLUS_ONE][MAX_CHNL_AMOUNT];
extern float		g_high_fAjust_a2[MAX_SLOT_AMOUNT_PLUS_ONE][MAX_CHNL_AMOUNT];
extern float		g_high_fAjust_a1[MAX_SLOT_AMOUNT_PLUS_ONE][MAX_CHNL_AMOUNT];
extern float		g_high_fAjust_a0[MAX_SLOT_AMOUNT_PLUS_ONE][MAX_CHNL_AMOUNT];
extern float		g_fAjust_a2[MAX_SLOT_AMOUNT_PLUS_ONE][MAX_CHNL_AMOUNT];
extern float		g_fAjust_a1[MAX_SLOT_AMOUNT_PLUS_ONE][MAX_CHNL_AMOUNT];
extern float		g_fAjust_a0[MAX_SLOT_AMOUNT_PLUS_ONE][MAX_CHNL_AMOUNT];

extern char*			g_cCalibMoni_Info;
extern char*			g_cCalibMoni_Info_ori;
extern unsigned int 	g_nCalibMoni_Points;
extern unsigned int		g_nL1Count;		
extern unsigned int		g_nCalibL1OffSet;
extern bool 			g_bTrgStop;
extern bool			g_bEMCMonitor;
extern bool			g_bEMCMonictrl;

extern int		nCbltTaskId;
extern int		nPackTaskId;
extern int		nNetTaskId;
extern int		nStatErrTaskId;

extern int g_nBytesCblted;
extern UINT32 	g_nETSWrite ;
extern UINT32 	g_nZDDCBLTed ;
extern unsigned int nZDDEventSize;



#define MSB(x)  (((x) >> 8) & 0xff)       /* most signif byte of 2-byte integer */
#define LSB(x)  ((x) & 0xff)              /* least signif byte of 2-byte integer*/
#define MSW(x) (((x) >> 16) & 0xffff) /* most signif word of 2-word integer */
#define LSW(x) ((x) & 0xffff)             /* least signif byte of 2-word integer*/
 
/* swap the MSW with the LSW of a 32 bit integer */
#define WORDSWAP(x) (MSW(x) | (LSW(x) << 16))
 
#define LLSB(x) ((x) & 0xff)            /* 32bit word byte/word swap macros */
#define LNLSB(x) (((x) >> 8) & 0xff)
#define LNMSB(x) (((x) >> 16) & 0xff)
#define LMSB(x)  (((x) >> 24) & 0xff)

#define LONGSWAP(x) ((LLSB(x) << 24) | \
                     (LNLSB(x) << 16)| \
                     (LNMSB(x) << 8) | \
                     (LMSB(x)))
                     
#endif
