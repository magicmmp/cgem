/*fw_dcgvas.cpp, Definitions of Common Global Variants for All Subsystems*/
#include "fw_dcgvas.h"

UINT32	g_nPPCIDOfSRH		=(*(UINT8 *)0xfef880e0);	/*value of the jump*/	
//UINT32	g_nPPCIDOfSRH		=224;
UINT32	g_nCrateID 			= 0 ;					/*Crate ID*/
UINT32	g_nSysID 			= SYSID_EMPTY ;		/*System ID*/
UINT32	g_nRunMode 		= 0;
UINT32 	g_nTrgPerInt 		= 1 ;				/*triggers per interrupt*/
UINT32 	g_nInterruptCounter = 0 ;  		/*interrupt counter*/
UINT32	g_nTrgCounter 		= 0 ;				/*trigger number*/
UINT32 	g_nEventsSent 		= 0 ;

UINT32 	g_nMdlSum 			= 0 ;
UINT32	g_nMaxCbltBytes 	= 0 ;
UINT32 	g_nCbltAdrs 			= 0 ;

UINT32	g_nmsgQnum1 = 0;
UINT32	g_nmsgQnum2 = 0;

/*framework actions*/
PFunc 	Actions[MAX_STATES_AMOUNT][MAX_CMD_AMOUNT] ;
/*global functions*/
PFunc 	g_pfnInitializedLOAD 	= NULL ;
PFunc 	g_pfnLoadedPRCF 		= NULL ;
PFunc 	g_pfnPreconfigedCONF 	= NULL ;
PFunc 	g_pfnConfigedSYN1 		= NULL ;
PFunc 	g_pfnSynchronized1SYN2 = NULL ;
PFunc 	g_pfnSynchronized2SYN3 = NULL ;
PFunc 	g_pfnSynchronized3SYN4 = NULL ;
PFunc 	g_pfnSynchronized4PREP = NULL ;
PFunc 	g_pfnReadySATR  		= NULL;
PFunc 	g_pfnRunningSPTR  		= NULL;/*pointer pointing to the function that exectues subsystem-dependent RunningSPTR*/
PFunc 	g_pfnReadySTOP  		= NULL;	/*pointer pointing to the function that exectues subsystem-dependent ReadySTOP*/
PFunc 	g_pfnConfigedUNCF 		= NULL;
PFunc 	g_pfnPreconfigedUNPC 	= NULL ;
PFunc 	g_pfnLoadedUNLD 		= NULL;
PFunc 	g_pfnAnyEXIT 			= NULL;			/*pointer pointing to the function that exectues subsystem-dependent LoadedConf*/

/*in g_pfnFWEnteringCbltTrans, 3 primary steps must be done: 1. evaluate g_nMaxCbltBytes, 2. sysIntEnable, 3. evaluate g_nCbltAdrs*/
PFunc 	g_pfnFWEnteringCbltTrans 	= NULL ;
PFunc 	g_pfnFWBeforeInterrupt		= NULL ;
PFunc 	g_pfnFWBeforeDMARun 		= NULL ;
PFunc 	g_pfnFWAfterDMARun 		= NULL ;

/*for socket connect*/
int 		g_sktListen = 0;				/*socket for listening clients*/
int   		g_sktCmdAck = 0;			/*socket for transfering command*/
int 		g_sktStaInfo = 0;			/*socket for transfering status*/
int 			g_sktData = 0; 				/*socket for transfering data*/
struct tcpcb*	g_DataTcpcb=NULL;				/*data socket pcb, together use with g_sktData*/

int 		  g_nCurnState = STID_Waiting ;
volatile int gv_nRunningCtrl ; /*running tasks control flag*/

SEM_ID 	g_sidSATR=NULL ;			/*waiting for SATR*/
SEM_ID	g_sidInterrupt=NULL ;		/*waiting for interrupt*/
SEM_ID 	g_sidBufEmpty=NULL ;		/*waiting for the network buffer to be empty*/

DMA_RING_ID	g_DmaRng=NULL;		/*a ring buffer storing raw data to be packed*/
DMA_RING_ID	g_NetRng=NULL ;		/*a ring buffer storing packed data to be sent to online*/

MSG_Q_ID		g_msgQcblt=NULL;		/*a message queue controlling g_DmaRng */
MSG_Q_ID		g_msgQnet=NULL ;			/*a message queue controlling g_NetRng*/

SEM_ID 			g_sidStaErrSend ;	      /*mutex for send error &status infor*/
SEM_ID 			g_sidStaErrTrig ;
WDOG_ID 		g_widStatusTimer ;
int				g_nReportOn;
bool				g_bStatusSend;
bool				g_bShutdownOpt;
	
RODHeader g_fwRODHead={0xee1234ee,9,0,0,0,0,0,0,0};
RODTrailer  g_fwRODTail={1,0,1};


const char *	g_strCmd[MAX_CMD_AMOUNT]={"LOAD","PRCF","CONF","SYN1","SYN2","SYN3","SYN4","PREP",
				"SATR","SPTR","STOP","UNCF","UNPC","UNLD","EXIT"};

const char *	g_strState[MAX_STATES_AMOUNT] ={"Waiting","Initialized","Loaded","Preconfiged","Configed",
				"Synchronized1","Synchronized2","Synchronized3","Synchronized4","Ready","Running" } ;

struct Header * g_pCrateConfigHeader = NULL;

UINT 		g_nConvertTable[MAX_SLOT_AMOUNT_PLUS_ONE][MAX_CHNL_AMOUNT];
float			g_low_fAjust_a2[MAX_SLOT_AMOUNT_PLUS_ONE][MAX_CHNL_AMOUNT];
float			g_low_fAjust_a1[MAX_SLOT_AMOUNT_PLUS_ONE][MAX_CHNL_AMOUNT];
float			g_low_fAjust_a0[MAX_SLOT_AMOUNT_PLUS_ONE][MAX_CHNL_AMOUNT];
float			g_mid_fAjust_a2[MAX_SLOT_AMOUNT_PLUS_ONE][MAX_CHNL_AMOUNT];
float			g_mid_fAjust_a1[MAX_SLOT_AMOUNT_PLUS_ONE][MAX_CHNL_AMOUNT];
float			g_mid_fAjust_a0[MAX_SLOT_AMOUNT_PLUS_ONE][MAX_CHNL_AMOUNT];
float			g_high_fAjust_a2[MAX_SLOT_AMOUNT_PLUS_ONE][MAX_CHNL_AMOUNT];
float			g_high_fAjust_a1[MAX_SLOT_AMOUNT_PLUS_ONE][MAX_CHNL_AMOUNT];
float			g_high_fAjust_a0[MAX_SLOT_AMOUNT_PLUS_ONE][MAX_CHNL_AMOUNT];
float			g_fAjust_a2[MAX_SLOT_AMOUNT_PLUS_ONE][MAX_CHNL_AMOUNT];
float			g_fAjust_a1[MAX_SLOT_AMOUNT_PLUS_ONE][MAX_CHNL_AMOUNT];
float			g_fAjust_a0[MAX_SLOT_AMOUNT_PLUS_ONE][MAX_CHNL_AMOUNT];

//for calibration
char*			g_cCalibMoni_Info;
char*			g_cCalibMoni_Info_ori;
unsigned int		g_nCalibMoni_Points;
unsigned int		g_nL1Count;		
unsigned int		g_nCalibL1OffSet;//special used for calibmode,normal mode =0
bool 			g_bTrgStop;

bool				g_bEMCMonitor;	// EMC monitor first 8step. oth crate.net trans should be started after SATR.
bool				g_bEMCMonictrl;//EMC monitor first 8step. oth crate.datapack should be ended after STOP.

//task ID for Kill
int	nCbltTaskId=0;
int	nPackTaskId=0;
int	nNetTaskId=0;
int	nStatErrTaskId=0;

int g_nBytesCblted;
UINT32 	g_nETSWrite ;
UINT32 	g_nZDDCBLTed ;
unsigned int nZDDEventSize;



