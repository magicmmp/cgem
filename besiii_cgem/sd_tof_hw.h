#ifndef __INCSDTOFHWh
#define __INCSDTOFHWh

#ifndef TOF_INT_LEVEL
#define TOF_INT_LEVEL 7
#endif
#ifndef TOF_INT_VEC
#define TOF_INT_VEC    64
#endif

void TOFIntHandler(int nArg);
int	TOFConfigIntCBLT();

int TOFConfigCLK();
int TOFConfigFeeMonitor();

int TOFGlobalReset();
int TOFConfigROC();
int ROCNormalConfigNew();
unsigned get_mask(int dp);
void phase_status_mask(unsigned *p, int len);
int ROC_get_phase();
int ROC_ajust_phase();
	

int TOFClearRocFEE();

int TOFConfigCbltChain();
int TOFConfigAll();
int TOFFiberChk();

int SelfTestAmp_Setup(UINT32 SelfTestAmp);

void TOFGenL1();
void TOFRocStartL1();
void TOFRocStop();

void tttest();


/**for ROC phase get function**/
//phase align status register defination
#define PTAP_STATUS_R_W 32									// status register width
#define PTAP_STATUS_SEGNUM 12								// phase status register segment number
#define PTAP_LOS_B 0										// (0)1 indicate current channel fibre los
#define PTAP_TIMEOUT_B PTAP_LOS_B+1							// (1)1 indicate operation timeout
#define PTAP_RXER_B PTAP_TIMEOUT_B+1						// (2)1 indicate current channel tlk has not synced
#define PTAP_TOUT_STEP_B PTAP_RXER_B+1						// (3)time out steps start positon
#define PTAP_TOUT_STEP_W 3									// time out step width
#define PTAP_BUSY_B PTAP_TOUT_STEP_B+PTAP_TOUT_STEP_W		// (6)1 indicate now the phase operation is just going
#define PTAP_OPE_STA_B PTAP_BUSY_B+1						// (7)1 indicate operation succeed, 0 indicate failed
#define PTAP_CURRECV_B PTAP_OPE_STA_B+1						// (8)1 indicate yes
#define PTAP_CURPLL_B PTAP_CURRECV_B+1						// (9)1 indicate yes
#define PTAP_DEST_B PTAP_CURPLL_B+1							// (10)1 indicate yes
#define PTAP_PCNTNEW_B PTAP_DEST_B+1						// (11)indicate got a new dest and haven't been read by vme
#define PTAP_PCNTEFF_B PTAP_PCNTNEW_B+1						// (12)indicate the current taps is effective or not
#define PTAP_PDIFFCNT_B 16									// phase different cnter value start bit
#define PTAP_PDIFFCNT_W 12						// phase different cnter width

//bit position defines
//status register defines
#define PHASE_SEG_NUM PTAP_STATUS_SEGNUM
#define PHASE_LOS 0			//is_los
#define PHASE_TOUT 1			//is_tout
#define PHASE_RXER 2			//is_rxer
#define PHASE_TOUTSTEP 3		//toutstep
#define PHASE_BUSY 4			//is_busy
#define PHASE_OPESTA 5		//ope_state
#define PHASE_CURRECV 6		
#define PHASE_CURPLL 7
#define PHASE_DEST 8
#define PHASE_PCNTNEW 9
#define PHASE_PCNTEFF 10
#define PHASE_PCNT 11			//pcnt

//int TDIG_Clear(unsigned int base_addr);
//int TDIG_function();
#endif
