/* 5100_DMA header file */
/* Last Modified : aug 23,1996 */

/*                                                         */
/*  GENERAL USAGE:                                         */
/*                                                         */
/*  DMA:    DMAinit(...);                                  */
/*          DMA_run(...);                                  */
/*          dmawait();                                     */
/*          ...                                            */
/* 	 dmadisable();                                     */
/*                                                         */
/*  CBLT:   CBLTinit(32 or 64);                            */
/*          DMA_run(...);                                  */
/*          cbltwait();                                    */
/*          ...                                            */
/* 	 dmadisable();                                     */
/*                                                         */
/*                                                         */
/*  List   dma_command*  list_name;                        */
/*         list_name add_dma_command(...);                 */
/*         list_name add_dma_command(...);                 */
/*              ....                                       */
/*         list_name add_dma_command(...);                 */
/*         Run_DMA_list(list_name);                        */
/*         dma_list_wait(...);                             */
/*                                                         */



/* this prevents this header from being included more than once */
#ifndef __INCDMA_PACKh
#define __INCDMA_PACKh



/* includes */
#include "vxWorks.h"
#include "semLib.h"


extern int univBaseAdrs;
/* definitions */


#define VME_A32_D64_BLT   0x0C
#define VME_A32_D32_BLT   0x0F
#define VME_A24_D32_BLT   0X3F
#define VME_A24_D64_BLT   0x3C
#define TO_LOCAL          0
#define TO_VME            1
#define NO_BLOCK          0
#define D32_BLOCK         1
#define D64_BLOCK         3
#define D16               16
#define D32               32
#define D64		  64

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

/* struct for DMA link list */
typedef struct
{
  UINT32 DCTL;
  UINT32 DTBC;
  UINT32 DLA;
  UINT32 reserved1;
  UINT32 DVA;
  UINT32 reserved2;
  UINT32 DCPP;
  UINT32 reserved3;
} dma_command;

#define swap_CPU_of_PCI(x)  (LONGSWAP(x) & 0x7FFFFFF8)
#define end_list(x) (LONGSWAP(x) & 0x1)
#define DCPP_last           0x1
#define DCPP_processed      0x2

#ifdef __cplusplus 
extern "C" {
#endif

/* forward */
/* direct control DMA commands */
extern void DMAinit(int datum_size,
		    int BLT_mode,
		    UINT32 AM_CODE,
		    int direction);
/* wait for DMA interrupt */
extern int dmawait(UINT32 maxtrans);              

/* cblt DMA commands */
extern STATUS CBLTinit(int mode);   
extern int cbltwait(UINT32 maxtrans); /* wait for CBLT interupt */

/* things no regularly used */
extern void dmaintr();              /* dma interrupt code same for cblt */
extern void dmadisable();           /* frees memory for semaphore and disables
				interupt for both regular and cblt DMA */

/* use DMA_run to run both kinds of transfers but the size in 
   the CBLT case size is used to specify the maximum transfers size
   to avoid catastrophic errors writing past the available memory */
extern void DMArun(UINT32 pvme,UINT32 plocal,int size);
extern int CBLTrun(UINT32 pvme,UINT32 plocal,int maxsize);
extern void CBLTend();

/* Link List Commands */
dma_command* add_dma_command (int datum_size, 
			      int BLT_mode,
			      UINT32 AM_CODE,
			      int direction,
			      UINT32 pvme,
			      UINT32 plocal,
			      int size,
			      dma_command *dma_list_start);
STATUS Run_DMA_list(dma_command* dma_list_start); 
void dma_list_wait(int dissemble);

#ifdef __cplusplus
}
#endif

#endif /* __INCDMA_PACKh */



int LCLSIZE;

/*void ResetPowerPC();*/