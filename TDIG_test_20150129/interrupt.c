#include "interrupt.h"
#include "vxworks.h"

#include <sysLib.h>
#include "netdb.h"
#include "stdlib.h"
#include "sockLib.h"
#include "inetLib.h"
#include "taskLib.h"
#include "stdioLib.h"
#include "strLib.h"
#include "stdio.h"
#include "ioLib.h"
#include "fioLib.h"
#include "semLib.h"
#include "msgQLib.h"
#include "sys/socket.h"
#include "netinet/in.h"
#include "netinet/tcp.h"
#include <intlib.h>
#include <logLib.h>
#include "interrupt.h"

#define IVEC_TO_INUM(intVec)	((int) (intVec))
#define INUM_TO_IVEC(intNum)	((VOIDFUNCPTR *) (intNum))

int g_intLevel ;
SEM_ID Sem_Interrupt;

#define WAIT_CBLT_ISR_SEM 1000
#define PPC_MS sysClkRateGet()/1000

int g_ntriggerNum;

static int run_time = 0;
int IntWait()
{
	if(semTake(Sem_Interrupt,WAIT_CBLT_ISR_SEM*PPC_MS))
	{
		printf ("WARNING --> timeout wait interrupt\n") ;
		return -1 ;
	}
	return 0;
}

void wait_interrupt()
{
	semTake(Sem_Interrupt,WAIT_FOREVER);
}



void TDIGIntTestHandler()
{
	/*sysIntDisable(7);*/
	/*printf("\n **recv an interrupt \n");*/
    semGive(Sem_Interrupt);
/*	g_ntriggerNum++;*/

}



int TDIG_InitInterrupt (int intnum, int level)
{
	g_intLevel = level ;

	if(!run_time)
	{
		if(intConnect (INUM_TO_IVEC(intnum), (VOIDFUNCPTR)TDIGIntTestHandler, 0) == ERROR)
		{
			printf("int connection error\n");
			return  -1;
		}
		printf("interrupt handler insert succ \n");
		run_time++;
	}
	else
	{
		printf("interrupt handler already exist \n");
	}
	if(semDelete(Sem_Interrupt)==ERROR)
	{
		printf("Sem_Interrupt doesn/t need to delete\n");
	}
	Sem_Interrupt = NULL;
	/*Sem_Interrupt = semBCreate(SEM_Q_PRIORITY, SEM_EMPTY);*/
	Sem_Interrupt = semCCreate(SEM_Q_PRIORITY, 0);	
	printf("*****Sem_Interrupt created*****\n");
	sysIntDisable(level);

	printf("interrupt initialized\n");
	
	return 0 ;	
}

void IntEnable ()
{
	sysIntEnable (g_intLevel) ;
}

void ReleaseInterrupt ()
{
	if(semDelete(Sem_Interrupt)==ERROR)
	{
		printf("ERROR!! Failed to delete the semphore <Sem_Interrupt>\n");
	}
	Sem_Interrupt = NULL;
}