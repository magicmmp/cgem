#ifndef __INCSDETFHWh
#define __INCSDETFHWh

#ifndef ETF_INT_LEVEL
#define ETF_INT_LEVEL 7
#endif
#ifndef ETF_INT_VEC
#define ETF_INT_VEC    64
#endif

void ETFIntHandler(int nArg);
int	ETFConfigIntCBLT();

int __inv_val(int ival, int w);


int ETFConfigCLK();
int ETFConfigTDIG();

int ETFGlobalReset();
int ETFConfigROC();
int ETFClearRocFEE();
int ETF_ROC_ajust_phase();

int CTTP_Config();

int ETFConfigCbltChain();
int ETFConfigAll();
int ETFSelfTestAmp_Setup(UINT32 SelfTestAmp);

void ETFGenL1();
void ETFRocStartL1();
void ETFRocStop();

int TDIG_config();
int TDIG_clrPurged();
void initial_convert_table_224();
void initial_convert_table_225();


void tttest();
//int resetch();
int ROC_halfway();
int ROC_linear();



#endif
