#ifndef __INCsd_zdd_datapack_h
#define __INCsd_zdd_datapack_h


/* include */
#include "vxWorks.h"
#include "stdlib.h"
#include "stdio.h"
#include "math.h"
#include "taskLib.h"
#include "time.h"
#include "sysLib.h"
#include "intLib.h"

int ZDDDataPackCore(UINT *nDestination ,UINT *nSource,int *nBytes2BeSend);

#endif
