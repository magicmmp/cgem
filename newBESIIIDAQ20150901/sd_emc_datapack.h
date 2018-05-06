#ifndef __INCsd_emc_datapack_h
#define __INCsd_emc_datapack_h


/* include */
#include "vxWorks.h"
#include "stdlib.h"
#include "stdio.h"
#include "math.h"
#include "taskLib.h"
#include "time.h"
#include "sysLib.h"
#include "intLib.h"

int EMCDataPackCore(UINT *nDestination ,UINT *nSource,int *nBytes2BeSend);
int EMCMoniBlankPackCore(UINT *nDestination,UINT Count);
int _EMCDataPackCore(UINT *nDestination ,UINT *nSource,int *nBytes2BeSend);
int EMCMoniCrateDataPackCore(UINT *nDestination ,UINT *nSource,int *nBytes2BeSend);

#endif
