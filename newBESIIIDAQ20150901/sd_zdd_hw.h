#ifndef __INCSDZDDHWh
#define __INCSDZDDHWh

#ifndef ZDD_INT_LEVEL
#define ZDD_INT_LEVEL 0x7
#endif
#ifndef ZDD_INT_VEC
#define ZDD_INT_VEC    0x77
#endif

void ZDDIntHandler(int nArg);
int ZDDConfIntCBLT();
int V1721_configDAQControl( int nMdlIdx);
int V1721_configChanConf(  int nMdlIdx);
int V1721_configMemory(  int nMdlIdx);
int V1721_configZS(  int nMdlIdx);
int V1721_configDACs(  int nMdlIdx, UINT32 separate);
int V1721_configTrig(  int nMdlIdx, UINT32 trigMask, UINT32 locTrgCoinLvl);
int V1721_configVMEControl(  int nMdlIdx);
int V1721_config(  int nMdlIdx, UINT32 trigMask, UINT32 locTrgCoinLvl);
int V1721_wait4Me(  int nMdlIdx);
int V1721_resetAndChk(  int nMdlIdx);
int V1721_startRun(  int nMdlIdx);
int V1721_stopRun(  int nMdlIdx);
int V1721_sendSoftTrig(  int nMdlIdx);
int V1721_dataReady(  int nMdlIdx, int* isReady);
int V1721_VMEstatus(  int nMdlIdx, UINT32* VMEstatus);
int V1721_evStored(  int nMdlIdx, UINT32* evStored);
int event_stored();
int V1721_nextEvSize(  int nMdlIdx, UINT32* nextSize);
int ZDDV1721Config();


int ZDDConfigMCCSyn3(int geo,UINT uT0Latency);

int ZDDV1190Config();

int ZDDStartTrig();
int ZDDStopTrig();


#endif
