/* this prevents this header from being included more than once
lei try*/
#ifndef __INC5100_TESTh
#define __INC5100_TESTh



/* includes */
#include "vxWorks.h"


/* definitions */
/* definitions for vme rw */
#define VA16 0xfbff0000
#define VA24 0xfa000000
#define VA32 0x20000000
#define VCSR 0x30000000

/* definitions for CBLT */
#define VME_A32_D64_BLT   0x0C
#define VME_A32_D32_BLT   0x0F
#define VME_A24_D32_BLT   0X3F
#define VME_A24_D64_BLT   0x3C

#define TO_LOCAL          0
#define TO_VME            1
#define NO_BLOCK          0
#define D32_BLOCK         1
#define D64_BLOCK         3
#define D8               8
#define D16               16
#define D32               32
#define D64               64

/* AM_CODE "parse" macros and defines */
#define AM_ADRS_SIZE(x)   (((x) >> 3) & 0x7)
#define AM_A64            0
#define AM_A32            1
#define AM_USER1          2
#define AM_USER2          3
#define AM_A16            5
#define AM_A24            7
#define AM_DATA_TYPE(x)   ((x) & 0x3)
#define AM_BLT_64         0
#define AM_DATA           1
#define AM_PRGM           2
#define AM_BLT_32         3
#define AM_NONP           O
#define AM_SUPER          4

 
#ifdef __cplusplus 
extern "C" {
#endif

/* forward */
/* direct control DMA commands */
extern void DMAinit(int datum_size,
                    int BLT_mode,
                    UINT32 AM_CODE,
                    int direction);

extern int CBLTinit(int mode);

/* things no regularly used */
extern void dmaintr();              /* dma interrupt code same for cblt */


/* use CBLT_run to run both kinds of transfers but the size in 
   the CBLT case size is used to specify the maximum transfers size
   to avoid catastrophic errors writing past the available memory */
extern int CBLTrun(UINT32 pvme,UINT32 plocal,int maxsize);
extern int DMArun(UINT32 pvme,UINT32 plocal,int size);

extern void CBLTend();

 extern int InitISR(VOIDFUNCPTR interruptHandler,
				int IntNum,
				int IntLvl);

 extern int ReadFrmVme8(UINT32 nVmeBusAdrs, /*VME BUS address*/
					UINT32 nAdrsWidth,				/*Address type, only VA16, VA24, VA32 supported*/
					UINT8 *pData) ;

/* 16-bit read from VME*/
extern int ReadFrmVme16(UINT32 nVmeBusAdrs, 
					UINT32 nAdrsWidth, 
					UINT16 *pData) ;

/* 32-bit read from VME*/
extern int ReadFrmVme32(UINT32 nVmeBusAdrs, 
					UINT32 nAdrsWidth, 
					UINT32 *pData) ;

/* 8-bit write to VME*/
extern int WriteToVme8(UINT32 nVmeBusAdrs, 
				UINT32 nAdrsWidth, 
				UINT8 nData
				) ;

/* 16-bit write to VME*/
extern int WriteToVme16(UINT32 nVmeBusAdrs, 
				UINT32 nAdrsWidth, 
				UINT16 nData 
				) ;

/* 32-bit write to VME*/
extern int WriteToVme32(UINT32 nVmeBusAdrs, 
				UINT32 nAdrsWidth, 
				UINT32 nData 
				) ;

/* 32-bit MCST write */
extern int MCSTwrite(UINT32 nVmeBusAdrs,
						int  nDataWidth,
						UINT32 nData
						);
extern int cbltwait(UINT32 maxtrans); /* wait for CBLT interupt *//*zhuht*/

#ifdef __cplusplus
}
#endif

#endif /* __INC5100_TESTh */





