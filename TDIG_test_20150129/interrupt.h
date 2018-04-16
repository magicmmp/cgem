#ifndef MY_INTERRUPT_H
#define MY_INTERRUPT_H

#include "semLib.h"

#ifdef __cplusplus 
extern "C" {
#endif


int IntWait() ;
void wait_interrupt();
void TDIGIntTestHandler();
int TDIG_InitInterrupt (int intnum, int level);
void ReleaseInterrupt () ;
void IntEnable () ;

extern int g_ntriggerNum;
extern SEM_ID Sem_Interrupt;
extern int run_time;
#ifdef __cplusplus
}
#endif



#endif