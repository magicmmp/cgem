#ifndef __INCSDEMCHWh
#define __INCSDEMCHWh

#ifndef EMC_INT_LEVEL
#define EMC_INT_LEVEL 4
#endif
#ifndef EMC_INT_VEC
#define EMC_INT_VEC    64
#endif
#ifndef EMC_Q_SELF_TRIG_MODE             //for set use out triger or self triger
#define EMC_Q_SELF_TRIG_MODE 0	//0 out triger ,1 self triger
#endif

void EMCIntHandler(int nArg);
void EMCTCBAIntHandler(int nArg);

void EMCSetQThreshold(int nModuleAddr, int nMdlIdx) ;
void EMCInitQ(int nModuleAddr, int nModuleType,int nMdlIdx) ;

int 	EMCConfigIntCBLT();
int 	EMCConfigQ();
void EMCQClear();

int 	EMCSetRocSelfL1();
int 	EMCSetRocOutL1();
int	EMCConfigRoc();

int 	EMCSetFanoutHitMode();
int 	EMCSetFanoutMoniMode();
int 	EMCConfigFanout();

int 	EMCTcbaDCMReset();
int	EMCConfigTcba();

int	EMCConfigHW();

int	EMCClearEvents();

void EMCGenL1();
void StopGenL1();

int EMCPreAmpPlus();

int testReadVME(unsigned int ndelay);

#endif
