/*****sd_trg_hw.h*****/
#ifndef __INCSDTRGHWh
#define __INCSDTRGHWh

#ifndef TRG_INT_LEVEL
#define TRG_INT_LEVEL 5
#else
#undef TRG_INT_LEVEL
#define TRG_INT_LEVEL 5
#endif

#ifndef TRG_INT_VEC
#define TRG_INT_VEC    248
#else
#undef TRG_INT_VEC
#define TRG_INT_VEC    248
#endif

void TRGIntHandler(int nArg);
int   TRGConfInterrupt();
int	TRGInitCBLT();

int WrtRdA24D16(unsigned uAddr,unsigned uData,char * abc,unsigned short uflag);

int TRG_CLKF_preconf();
int TRG_CLKF_syn2();

int TRG_MUTL_preconf();
int TRG_MUTL_syn3();
int TRG_MUTL_prep();

int TRG_TFCF_syn2();
int TRG_TFCF_syn1();
int TRG_TFCF_prep();

int TRG_FCTL_syn2();
int TRG_FCTL_syn4();
int TRG_FCTL_test();

void trg_status();


int TRG_TROC_prep();

int TRG_GTL_preconf();
int TRG_GTL_conf();
int TRG_GTL_syn2();
int TRG_GTL_prep();
int TRG_GTL_satr();
int TRG_GTL_sptr();

int TRG_SAF_conf();
int TRG_SAF_syn2();
int TRG_SAF_prep();

int TRG_EACC_preconf();
int TRG_EACC_conf();
int TRG_EACC_syn2();
int TRG_EACC_prep();
int TRG_EACC_sptr();

int TRG_TDC_conf();
int TRG_TDC_syn2();
int TRG_TDC_prep();

int TRG_LTKC_syn3();
int TRG_LTKC_prep();

int TRG_STKC_syn3();
int TRG_STKC_prep();

int TRG_TKF_preconf();
int TRG_TKF_syn3();
int TRG_TKF_prep();

int TRG_TOF_conf();
int TRG_TOF_syn3() ;
int TRG_TOF_prep();

int TRG_ETF_conf();
int TRG_ETF_prep();
int TRG_ETF_syn1();


int TRG_BASG_conf();
int TRG_BASG_prep();
int TRG_BASG_sptr();

int TRG_BTM_syn3();
int TRG_ETM_syn3();

int TRG_BTM_prep();
int TRG_ETM_prep();

int TRG_ETS_conf();
int TRG_ETS_prep();

#endif
