/*****sd_trg_runtasks.h*****/
#ifndef __INCSDTRGRUNTASKSh
#define __INCSDTRGRUNTASKSh

void TRGDataPack( ) ;

int TRGEnteringCbltTrans(int nArg);

int TRGBeforeInterrupt(int nArg);

int TRGBeforeDMARun(int nArg);

int TRGAfterDMARun(int nArg);

#endif