
#define ETF_INT_LEVEL 7

#define ETF_INT_VEC    64


int configFPGA(unsigned int base_addr);
int configINL(char *fname,unsigned int base_addr);
int configHPTDC(unsigned int base_addr);
int HPTDCenable(unsigned int base_addr);
int configDataSet(unsigned int base_addr);
int configReadoutEvent(unsigned int base_addr);
int HPTDCopen(unsigned int base_addr);
int ClearFIFO(unsigned int base_addr);
int TDIG_config(unsigned int base_addr);
int TDIG_daq();
void test();

