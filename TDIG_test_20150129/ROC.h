
void TOFRocStop();

int ROC_config();

int GlobalReset();

int start_ROC();

int ROC_configCBLTChain(int position);
int ROC_configCBLTChain2(int position, unsigned int geo);

int ROC_sconfig();

int shiftch();
int resetch();
int ROC_pll();
int ROC_pll2();
int shiftch_helper(UINT32 chl,UINT32 shift_val);
int ROC_linear();
int ROC_halfway();