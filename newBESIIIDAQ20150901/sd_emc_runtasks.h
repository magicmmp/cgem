#ifndef __INCSDEMCRUNTASKSh
#define __INCSDEMCRUNTASKSh

void EMCMoniOtheNetTransP1();
void EMCMoniCrateNetTransP1();

void  EMCDataPack() ;
void  EMCMoniOthDataPackP1();
void  EMCMoniCrateDataPackP1();
	
void  EMCBaselinePack();

int EMCEnteringCbltTrans(int nArg);

int EMCBeforeInterrupt(int nArg);

int EMCBeforeDMARun(int nArg);

int EMCAfterDMARun(int nArg);

void EMCTCBADMARead();//byjak

#endif