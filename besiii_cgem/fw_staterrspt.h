/*fw_errhandler.h*/
#ifndef __INCFWERRSTATEh
#define __INCFWERRSTATEh

typedef struct {
	unsigned int		InforLen;
	unsigned int		StaErrCode;
	unsigned int		CrateId;
	unsigned int		AvrgCpu_CuSta_SrcGeo_SrcL1;
	unsigned int		InstCpu_ReCmd_HopGeo_HopL1;
	unsigned int		IntrptNum;
	unsigned int		NetEvtNum;
}StaErrUnit;  /*becareful for net byte serial*/

//errcode= err level + err part+ err type +err num

//err level
#define PPCINFO		(0<<28)
#define PPCWARNING	(1<<28)
#define PPCERROR 	(2<<28)
#define PPCFATAL 	(3<<28)

/*err part*/
#define FRM_FLG		(0<<24)
#define MDC_FLG		(1<<24)	
#define TOF_FLG		(2<<24)
#define EMC_FLG		(3<<24)
#define MUC_FLG		(4<<24)
#define TRG_FLG		(5<<24)
#define ZDD_FLG		(6<<24)
#define ETF_FLG		(7<<24)

/*info type*/
#define ISTATUS		(0<<16)
/*err type*/
#define STAT_ERR 	(1<<16)
#define GEO_ERR		(2<<16)
#define L1_ERR 		(3<<16)
#define ATOM_ERR	(4<<16)


#define ERR_TIMER_TICKS		60

void FWStatusTimerEvent();
void FWReportStatus();
int	MyPrintf (const char *pFormatString, ...);
int 	StateErrParaCreate();
int 	StateErrParaFree();
int 	StateErrReportInit();
void Push2ErrStack(UINT errcode,UINT ACGL, UINT IRGL, UINT IntrptNum, UINT NetEvtNum);
void StaErrUnitSend();


#endif

