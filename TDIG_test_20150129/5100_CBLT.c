/*includes*/
#include "5100_CBLT.h"
/*#include <config.h>*/
#include "config.h"
#include <vme.h>
/*#include <universe.h>*/
#include "universe.h"
#include <intLib.h>
#include <stdlib.h>
#include <stdio.h>

/* Forwards because I can't find the vxworks one */
/*extern int univBaseAdrs;*/
extern STATUS sysUnivIntConnect
    (
    int		univIntType,  /* the interrupt type to connect with */
    VOIDFUNCPTR	routine,      /* routine to be called */
    int		parameter     /* parameter to be passed to routine */
    );
extern STATUS sysUnivIntEnable
    (
    int univIntType	/* interrupt type */
    );
extern STATUS sysUnivIntDisable
    (
    int univIntType /* interrupt type */
    );
extern STATUS sysUniverseInit (void);


/*------------------*/
/* Global Variables */
/*------------------*/
static SEM_ID dmaint_sem = NULL; 
static long dma_status = 0x0; 
static int DMA_interrupt_connected = 0;
static dma_command* list_running = NULL;

/*-----------------*/
/* Direct Mode DMA */
/*-----------------*/

/* Initialize vmechip2 and CPU for DMA with interupt */
void DMAinit (int datum_size, int BLT_mode, UINT32 AM_CODE, int direction) 
{
  /* Initialize the chip for dma */

  *UNIVERSE_DGCS = LONGSWAP( DGCS_VON_DONE      /* Transfer count until done*/
			     | DGCS_VOFF_0      /* Time off VME bus0 us */
			     | DGCS_INT_STOP    /* Interrupt stop enable */
			     | DGCS_INT_HALT    /* Interrupt halt enable */
			     | DGCS_INT_DONE    /* Interrupt LERR enable */
			     | DGCS_INT_VERR    /* Interrupt VERR enable */
			     | DGCS_INT_P_ERR   /* Interrupt protocol enable */
			     );

  /* zero this register so that we can just "add" what we need */
  *UNIVERSE_DCTL = 0x0;

  /* enable 64 bit PCI */
  /**UNIVERSE_DCTL |= LONGSWAP(DCTL_LD64EN);*/
  *UNIVERSE_DCTL &= LONGSWAP(DCTL_LD64EN_32);
  

  /* set direction */
  if(direction == TO_VME) *UNIVERSE_DCTL |= LONGSWAP(DCTL_L2V);
  if(direction == TO_LOCAL) *UNIVERSE_DCTL |= LONGSWAP(DCTL_L2V_VME_PCI);

  /* set data size */
  if(datum_size == D16) *UNIVERSE_DCTL |= LONGSWAP(DCTL_VDW_16);
  if(datum_size == D32) *UNIVERSE_DCTL |= LONGSWAP(DCTL_VDW_32);
  if(datum_size == D64) *UNIVERSE_DCTL |= LONGSWAP(DCTL_VDW_64);

  /* set BLT mode */
  switch(BLT_mode)
    {
       case D32_BLOCK:
	 *UNIVERSE_DCTL |= LONGSWAP(DCTL_VDW_32);
	 *UNIVERSE_DCTL |= LONGSWAP(DCTL_VCT_EN);
	 break;
      
       case D64_BLOCK:
	 *UNIVERSE_DCTL |= LONGSWAP(DCTL_VDW_64);
	 *UNIVERSE_DCTL |= LONGSWAP(DCTL_VCT_EN);
	 break;

       default:
	 break;
    }

  switch (AM_ADRS_SIZE(AM_CODE))
    {
       case AM_A16:
	 *UNIVERSE_DCTL |= LONGSWAP(DCTL_VAS_A16);
	 break;

       case AM_A24:
	 *UNIVERSE_DCTL |= LONGSWAP(DCTL_VAS_A24);
	 printf("DMA:A24 modifier\n");
	 break;
	 
       case AM_A32:
	 *UNIVERSE_DCTL |= LONGSWAP(DCTL_VAS_A32);
	 break;

       case AM_A64:
	 printf("DMA: the universe does not support A64\n");
	 break;

       case AM_USER1:
	 *UNIVERSE_DCTL |= LONGSWAP(DCTL_VAS_USER1);
	 break;

       case AM_USER2:
	 *UNIVERSE_DCTL |= LONGSWAP(DCTL_VAS_USER2);
	 break;

       default:
	 printf("\t AM CODE is UNKNOWN = %hx \n", AM_CODE);   
	 break;
    }

  switch (AM_DATA_TYPE(AM_CODE))
    {
      case AM_BLT_32:
 	if(BLT_mode != D32_BLOCK)  
 	  printf("DMA: AM_CODE not consistent with D32 BLT \n"); 
	break;

      case AM_BLT_64:
	if(BLT_mode != D64_BLOCK) 
	  printf("DMA: AM_CODE not consistent with D64 BLT \n");
	break;

      case AM_PRGM:
	*UNIVERSE_DCTL |= LONGSWAP(DCTL_PGM_PRGM);
	break;
     
      case AM_DATA:
	*UNIVERSE_DCTL |= LONGSWAP(DCTL_PGM_DATA);
	break;
    }
/*
	printf(">>The DCTL after configure is %x\n",LONGSWAP(*UNIVERSE_DCTL));
	printf(">>The MISC status reg before modify is %x\n",LONGSWAP(*UNIVERSE_MISC_STAT));
	printf(">>The MISC Control reg before modify is %x\n",LONGSWAP(*UNIVERSE_MISC_CTL));

	printf(">>The VSI1_CTL reg is %x\n",LONGSWAP(*UNIVERSE_VSI1_CTL));	
	
	printf(">>The VCSR_SET reg is %x\n",LONGSWAP(*UNIVERSE_VCSR_SET));
*/	
	LCLSIZE = 0x0;
	LCLSIZE = LONGSWAP(*UNIVERSE_MISC_STAT & LONGSWAP(MISC_STAT_LCL_SIZE_64));
/*	printf(">>the LCLSIZE in MISC status reg is %x\n",LCLSIZE);*/
	if(LCLSIZE==0x40000000)
	{
		*UNIVERSE_VSI0_CTL &= LONGSWAP(~(VSI_CTL_LD64EN));
		*UNIVERSE_VSI1_CTL &= LONGSWAP(~(VSI_CTL_LD64EN));
		*UNIVERSE_VSI2_CTL &= LONGSWAP(~(VSI_CTL_LD64EN));
		*UNIVERSE_VSI3_CTL &= LONGSWAP(~(VSI_CTL_LD64EN));
		*UNIVERSE_VSI4_CTL &= LONGSWAP(~(VSI_CTL_LD64EN));
		*UNIVERSE_VSI5_CTL &= LONGSWAP(~(VSI_CTL_LD64EN));
		*UNIVERSE_VSI6_CTL &= LONGSWAP(~(VSI_CTL_LD64EN));
		*UNIVERSE_VSI7_CTL &= LONGSWAP(~(VSI_CTL_LD64EN));

		/**UNIVERSE_MISC_CTL |= LONGSWAP(MISC_CTL_SW_LRST);*/  /*Reset Universe II from PCI bus,no use to change PCI bus wide*/
		/**UNIVERSE_VCSR_SET |= LONGSWAP(VCSR_SET_RESET);*/
		*UNIVERSE_VCSR_CLR |= LONGSWAP(VCSR_CLR_RESET);
/*		printf(">>The VCSR_SET reg after modify is %x\n",LONGSWAP(*UNIVERSE_VCSR_SET));


		printf(">>The VSI1_CTL reg after modify is %x\n",LONGSWAP(*UNIVERSE_VSI1_CTL));
*/
		/**UNIVERSE_MISC_CTL |= LONGSWAP(MISC_CTL_SYSRESET);*/


	} 
/*	printf(">>The MISC status reg after modify is %x\n",LONGSWAP(*UNIVERSE_MISC_STAT));
	printf(">>The MISC Control reg after modify is %x\n",LONGSWAP(*UNIVERSE_MISC_CTL));	*/
  /* intialize dma semaphore */
    dmaint_sem = semBCreate(SEM_Q_FIFO,SEM_EMPTY);


  /* intialize interupt handler in CPU */
    /* connect the handler routine dmaintr */
  if (DMA_interrupt_connected == 0)
    {
      DMA_interrupt_connected = 1;
      if(sysUnivIntConnect(UNIVERSE_DMA_INT,dmaintr,0)==ERROR)
	{
	  printf("ERROR in intConnect");
	}
    }

  /* intialize universe DMA interupter */
    sysUnivIntEnable(UNIVERSE_DMA_INT);

}

/* Run a normal DMA to Local memory */
void DMArun(UINT32 pvme,UINT32 plocal,int size)
{
  
  /* set addresses and size */
  /* note DRAM addresses on PCI start at 0x80000000 */
  *UNIVERSE_DLA =  LONGSWAP((int)plocal + PCI2DRAM_BASE_ADRS);
  
  *UNIVERSE_DVA = LONGSWAP((int)pvme);
  
  *UNIVERSE_DTBC = LONGSWAP(size);
  
  /* Make sure chaining mode is off */

  *UNIVERSE_DGCS &= LONGSWAP(~DGCS_CHAIN);
  
  /* Do DMA */
  *UNIVERSE_DGCS |= LONGSWAP(DGCS_GO);
  
}

/* This is the interupt handler code and is the same
   for all kinds of DMA's (CBLT or not) */
void dmaintr(int param)
{
  /* clear interupt */
  *UNIVERSE_LINT_STAT |= LONGSWAP(LINT_STAT_DMA);

/*   printf("dma interrupt received \n"); */


  /* get status of the termination */
  dma_status = LONGSWAP(*UNIVERSE_DGCS);

  /* give semaphore to task level handling */
  semGive(dmaint_sem);
}


/* 
   This is a task level program that waits for a 
   DMA interupt and prints error messages if all
   did not go well, the calling task will pend
   until an interupt is received 
*/
int dmawait(UINT32 maxtrans)
{

  /* pend/wait for semaphore from interupt handler */
  semTake(dmaint_sem,WAIT_FOREVER);
  /* check status, and print related message */
  if (dma_status & DGCS_DONE)
  {
/*      printf("dma interrupt: DMA done\n");  */
    return  (maxtrans - LONGSWAP(*UNIVERSE_DTBC));
  }
  else
  {
    printf("dma completion error: %x",(UINT32)dma_status);
/*     printf("dma completion error:"); */
    if (dma_status & DGCS_VERR) printf("VME Bus Error\n");
    if (dma_status & DGCS_LERR) printf("PCI Bus Error\n");
    if (dma_status & DGCS_P_ERR) printf("Protocol Error\n");
    if (dma_status & DGCS_HALT) printf("user halted\n");
    if (dma_status & DGCS_STOP) printf("user stopped\n");
  }
  return ERROR;
}

/* Disable the DMA interupt and Delete DMA semaphore
   should be called if DMA will not be used again in
   the program */
void dmadisable()
{
  semDelete(dmaint_sem);
  
  /* Disable DMA interupt */  
  sysUnivIntDisable(UNIVERSE_DMA_INT);

  return;
}

/*----------------------*/
/* CBLT setup and tools */
/*----------------------*/

/* Initialize vmechip2 for CBLT */
STATUS CBLTinit (int mode)
{
  /* return value variable */
  STATUS stat = OK;

  /* Set up ussual DMA (note the datum size is specified by
     the block transfer mode not the first term) */
  switch(mode)
    {
      case 32:
 	printf("Initializing D32 cblt transfers \n"); 
	DMAinit(D32,D32_BLOCK,VME_A32_D32_BLT,TO_LOCAL);
	break;

      case 64:
	printf(">>Initializing D64 CBLT Transfers \n");
	DMAinit(0,D64_BLOCK,VME_A32_D64_BLT,TO_LOCAL);
	break;
	
      default:
	printf("Invlaid CBLT mode \n");	
	stat = ERROR;
	break;
    }

   return stat;
}

/* this is the same one dmawait except that now there is an
   error if the dma does not end on a VME bus error */
int cbltwait(UINT32 maxtrans)
{

  /* pend/wait for semaphore from interupt handler */
  semTake(dmaint_sem,WAIT_FOREVER);

  /* check status, and print related message */
  if (dma_status & DGCS_VERR)
  {
/*      printf("CBLT completed in VME bus error\n"); */
    return  (maxtrans - LONGSWAP(*UNIVERSE_DTBC));
  }
  else
  {
    printf("dma completion error: %x",(UINT32)dma_status);
/*     printf("dma completion error:"); */
    if (dma_status & DGCS_DONE) printf("CBLT did not complete in VME bus error");
    if (dma_status & DGCS_LERR) printf("PCI Bus Error\n");
    if (dma_status & DGCS_P_ERR) printf("Protocol Error\n");
    if (dma_status & DGCS_HALT) printf("user halted\n");
    if (dma_status & DGCS_STOP) printf("user stopped\n");
  }
  return ERROR;
}

/* Run a normal DMA to Local memory */
int CBLTrun(UINT32 pvme,UINT32 plocal,int maxsize)
{
  int stat = 0;

  /* set addresses and size */
  /* note DRAM addresses on PCI start at 0x80000000 */
  *UNIVERSE_DLA =  LONGSWAP((int)plocal + PCI2DRAM_BASE_ADRS);

  *UNIVERSE_DVA = LONGSWAP((int)pvme);

  *UNIVERSE_DTBC = LONGSWAP(maxsize);

  /* Make sure chaining mode is off */

  *UNIVERSE_DGCS &= LONGSWAP(~DGCS_CHAIN);

  /* Do DMA */
  *UNIVERSE_DGCS |= LONGSWAP(DGCS_GO);
  
  /* pend/wait for semaphore from interupt handler */
  semTake(dmaint_sem,WAIT_FOREVER);

  /* check status, and print related message */
  if (dma_status & DGCS_VERR)
  {
     /*printf("CBLT completed in VME bus error!\n"); */
    stat = maxsize - LONGSWAP(*UNIVERSE_DTBC);
  }
  else
  {
    printf("dma completion error: %x\n",(UINT32)dma_status);
    if (dma_status & DGCS_DONE) printf("CBLT did not complete in VME bus error\n");
    if (dma_status & DGCS_LERR) printf("PCI Bus Error\n");
    if (dma_status & DGCS_P_ERR) printf("Protocol Error\n");
    if (dma_status & DGCS_HALT) printf("user halted\n");
    if (dma_status & DGCS_STOP) printf("user stopped\n");
    stat = -1;
  }

 /* semDelete(dmaint_sem);*/

  /* Disable DMA interupt */
 /* sysUnivIntDisable(UNIVERSE_DMA_INT);*/
  return stat;
}

void CBLTend()
{
  semDelete(dmaint_sem);

  /* Disable DMA interupt */
  sysUnivIntDisable(UNIVERSE_DMA_INT);
}

/*************************/
/* Command Chaining Mode */
/*************************/

dma_command* add_dma_command (int datum_size,                                         /*definition started*/
			int BLT_mode,
			UINT32 AM_CODE,
			int direction,
			UINT32 pvme,
			UINT32 plocal,
			int size,
			dma_command *dma_list_start)
{
  dma_command* new_command;
  dma_command* last_command;


  /* Get 8 byte aligned memory for next element of linked list */

  if((new_command = (dma_command*) memalign(8,32))==NULL)                             /***********************************/
    {
      printf("Couldn't malloc new for dma command/n");
      return NULL;
    }


  /* Create DCTL entry for new linked list element */
  
  /* zero this register so that we can just "add" what we need */
  new_command->DCTL = 0x0;                                                            /***************************************/

  /* enable 64 bit PCI */
  new_command->DCTL |= LONGSWAP(DCTL_LD64EN);

  /* set direction */
  if(direction == TO_VME) new_command->DCTL |= LONGSWAP(DCTL_L2V);

  /* set data size */
  if(datum_size == D16) new_command->DCTL |= LONGSWAP(DCTL_VDW_16);
  if(datum_size == D32) new_command->DCTL |= LONGSWAP(DCTL_VDW_32);
  
  /* set BLT mode */
  switch(BLT_mode)
    {
       case D32_BLOCK:
	 new_command->DCTL |= LONGSWAP(DCTL_VDW_32);
	 new_command->DCTL |= LONGSWAP(DCTL_VCT_EN);
	 break;
      
       case D64_BLOCK:
	 new_command->DCTL |= LONGSWAP(DCTL_VDW_64);
	 new_command->DCTL |= LONGSWAP(DCTL_VCT_EN);
	 break;

       default:
	 break;
    }

  switch (AM_ADRS_SIZE(AM_CODE))
    {
       case AM_A16:
	 new_command->DCTL |= LONGSWAP(DCTL_VAS_A16);
	 break;

       case AM_A24:
	 new_command->DCTL |= LONGSWAP(DCTL_VAS_A24);
	 break;
	 
       case AM_A32:
	 new_command->DCTL |= LONGSWAP(DCTL_VAS_A32);
	 break;

       case AM_A64:
	 printf("DMA: the universe does not support A64\n");
	 break;

       case AM_USER1:
	 new_command->DCTL |= LONGSWAP(DCTL_VAS_USER1);
	 break;

       case AM_USER2:
	 new_command->DCTL |= LONGSWAP(DCTL_VAS_USER2);
	 break;

       default:
	 printf("\t AM CODE is UNKNOWN = %hx \n", AM_CODE);   
	 break;
    }

  switch (AM_DATA_TYPE(AM_CODE))
    {
      case AM_BLT_32:
	if(BLT_mode != D32_BLOCK) 
	  printf("DMA: AM_CODE not consistent with D32 BLT \n");

      case AM_BLT_64:
	if(BLT_mode != D64_BLOCK) 
	  printf("DMA: AM_CODE not consistent with D64 BLT \n");
	
      case AM_PRGM:
	new_command->DCTL |= LONGSWAP(DCTL_PGM_PRGM);
	break;
     
      case AM_DATA:
	new_command->DCTL |= LONGSWAP(DCTL_PGM_DATA);
	break;
    }

  /* create DTBC (size) entry for new command */
  new_command->DTBC = LONGSWAP(size);
  
  /* create DLA (local addr) entry for new command (PCI2DRAM_BASE_ADRS in mv5100.h)*/
  new_command->DLA = LONGSWAP((int)PCI2DRAM_BASE_ADRS +(int)plocal);

  /* create DVA (vme addr) entry for new command */
  new_command->DVA = LONGSWAP((int)pvme);

  /* Set DCPP (next command) to end of list signal */                                  /*******************************************/
  new_command->DCPP = LONGSWAP(DCPP_last);

  /* Determine begining/end of linked list 
     and make appropriate pointer to beginning*/

  if(dma_list_start == NULL)
    {
      dma_list_start = new_command;
    }
  else
    {
      last_command = dma_list_start;
      while(!end_list(last_command->DCPP)) 
	last_command = (dma_command*)swap_CPU_of_PCI(last_command->DCPP);                   /*******************************************/
      if(LONGSWAP(last_command->DCPP) & DCPP_processed)
	{
	  printf("table already proccessed: can't add to list\n");
	  free((void*)new_command);
	  return (NULL);
	}
      last_command->DCPP = LONGSWAP((UINT32)new_command + PCI2DRAM_BASE_ADRS);
    }
  
  return dma_list_start;
}                                                                                               /*definition end*/

/* Start the assembled table */
STATUS Run_DMA_list(dma_command* dma_list_start) 
{
  /* Check that the Universe Chip is not doing a DMA */
  if ((*UNIVERSE_DGCS & DGCS_ACT)||(list_running!=NULL))
    {
      printf("Cannot start DMA one is already started \n");
      return ERROR;
    }

  /* intialize dma semaphore */
  if (dmaint_sem == NULL) dmaint_sem = semBCreate(SEM_Q_FIFO,SEM_EMPTY);

  /* basic setup for bus grant, interrupt ... */
  *UNIVERSE_DGCS = LONGSWAP( DGCS_VON_DONE      /* Transfer count until done*/
			     | DGCS_VOFF_0      /* Time off VME bus0 us */
			     | DGCS_INT_STOP    /* Interrupt stop enable */
			     | DGCS_INT_HALT    /* Interrupt halt enable */
			     | DGCS_INT_DONE    /* Interrupt LERR enable */
			     | DGCS_INT_VERR    /* Interrupt VERR enable */
			     | DGCS_INT_P_ERR   /* Interrupt protocol enable */
			     );

  /* intialize interupt handler in CPU */
    /* connect the handler routine dmaintr */
  if (DMA_interrupt_connected == 0)
    {
      DMA_interrupt_connected = 1;
      if(sysUnivIntConnect(UNIVERSE_DMA_INT,dmaintr,0)==ERROR)
	{
	  printf("ERROR in intConnect");
	}
    }

  /* make pointer to first command */
  *UNIVERSE_DCPP = LONGSWAP((UINT32)dma_list_start + PCI2DRAM_BASE_ADRS);                          /*******************************************/

  /* intialize universe DMA interupter */
  sysUnivIntEnable(UNIVERSE_DMA_INT);
  
  /* do dma */
  *UNIVERSE_DGCS |= LONGSWAP(DGCS_CHAIN);
  *UNIVERSE_DGCS |= LONGSWAP(DGCS_GO);  

  /* set list running global */
  list_running = dma_list_start;

  return OK;

}


void dma_list_wait(int disassemble)
{
  dma_command* next;

  /* pend/wait for semaphore from interupt handler */
  semTake(dmaint_sem,WAIT_FOREVER);

  /* check status, and print related message */
  if (dma_status & DGCS_DONE)
  {
/*      printf("dma interrupt: DMA done\n");  */
  }
  else
  {
    printf("dma completion error:");
    if (dma_status & DGCS_VERR) printf("VME Bus Error\n");
    if (dma_status & DGCS_LERR) printf("PCI Bus Error\n");
    if (dma_status & DGCS_P_ERR) printf("Protocol Error\n");
    if (dma_status & DGCS_HALT) printf("user halted\n");
    if (dma_status & DGCS_STOP) printf("user stopped\n");
  }

  /* disassemble linked list and check that everything
     has been processed */
  if (disassemble)
    {
      while(list_running!=NULL)
	{
	  if(~(LONGSWAP(list_running->DCPP) & DCPP_processed))
	    printf("Error not all elements in dma list processed ?!\n");
	  
	  if(LONGSWAP(list_running->DCPP) & DCPP_last)
	    {
	      next = NULL;
	    }
	  else
	    {
	      next = (dma_command*)(LONGSWAP(list_running->DCPP) & 0x7FFFFFF8);
	    }
	  
	  free(list_running);
	  
	  list_running = next;
	}
    }
  else
    {
      list_running = NULL;
    }
  return;
}


void dmastatus()
{
  int local_status = LONGSWAP(*UNIVERSE_DGCS);

  if (local_status & DGCS_DONE) printf("DMA done\n"); 
  if (local_status & DGCS_VERR) printf("VME Bus Error\n");
  if (local_status & DGCS_LERR) printf("PCI Bus Error\n");
  if (local_status & DGCS_P_ERR) printf("Protocol Error\n");
  if (local_status & DGCS_HALT) printf("user halted\n");
  if (local_status & DGCS_STOP) printf("user stopped\n");
  return;
}



/*void ResetPowerPC()
{
	*UNIVERSE_MISC_CTL |= LONGSWAP(MISC_CTL_SW_LRST);	
}*/