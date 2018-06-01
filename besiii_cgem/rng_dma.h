/* rng_dma Header */
/* this prevents this header from being included more than once */
#ifndef __INCrng_dmah
#define __INCrng_dmah

/*
    modification history
    --------------------
    sept 10,1996 lipeles  Added cache flush/invalidate, new documentation
                          and FastCopy for increased speed.
*/

/*  Documention -
    This file contains subroutines for creating, using and deleting
    a specialized ring buffer that can be used for DMAs and CBLTs.
    The key features that are different from the VxWorks ring buffer
    are that it can handle transfers where only the maximum size of
    the transfer is known and the memory block used needs to be
    contiguous, and that it handles the cache flushing and invalidation
    that keeps cache coherency.

    There are three kinds of functions provided: Instantiation, DMA
    read/write, and non-DMA buffer read/write. Both the instantiation
    and the non-DMA buffer read/write are compatible with the VxWorks
    commands.

    these rings are always identified by a DMA_RING_ID structure
    and each ring used must have a unique DMA_RING_ID defined.

    Instantiation:
      rng_dmaCreate - creates a ring buffer
      rng_dmaDelete - deletes a ring buffer

    DMA read/write:
      rng_dmaWriteDMA - Get pointer for DMA/CBLT
      rng_dmaWritten  - Update pointers after a read has been completed
                        requires specifacation of method for read to be
                        done for cache handling purposes.
      rng_dmaRead - get pointer to data for DMA read, requires
                    specifacation of method for read to be done for
                    cache handling purposes.
      rng_dmaFree - Update pointers after a write has been done.

    Non-DMA read/write:
      rng_dmaBufPut - copy a buffer into the ring buffer
      rng_dmaBufGet - read a buffer from the ring buffer

    Technical points on how it works:
      Full/Empty: to distiguish between a full buffer and an empty buffer,
                  both of which might written with equal read and write
                  pointers, the pointer that should not be used is set to
                  NULL. So when empty pRead = NULL since you should not read
                  from an empty ring buffer, and when full pWrite = NULL since
                  you should not write to a full ring buffer.

      DMA/CBLTs : There is an extra pointer inaddition to the the read, write,
                  end, and start pointer called pLastWrite which is used to 
                  make an imaginary buffer end so that if the distance to the
                  end of the buffer is not is not large enough for the requested
                  maximum transfer size then it will mark where the last write
                  ended with the LastWrite pointer and attempt to find the space
                  at the begining of the buffer. Then when read pointer gets to 
                  the lastwrite pointer it starts again at the begining of the
                  buffer and returns the lastwrite poiter to the buffer end.
*/


/* includes */
#include "vxWorks.h"
#include "semLib.h"

/* Definitions */
#define DMA_METHOD         1
#define CPU_METHOD         0

/* Declaration of Structs */

#ifdef __cplusplus
extern "C" {
#endif 

typedef struct		/* RING - ring buffer */
{
  char *bufBase;   /*appied address*/
  char *bufEnd; 	/* pointer to end of buffer */
  char *bufStart;	/* pointer to start of buffer ,the position %32*/
  char *pWrite;   	/* where to write next */
  char *pRead;  	/* where to read next */
  char *pLastWrite;	/* where the data was written before the write pointer was skipped to the bottom */
  int 	    nBufSize;
  SEM_ID semMutex;
}DMA_RING;
/* This pointer is used in rng_dma calls */
typedef DMA_RING * DMA_RING_ID;

/* forwards - see top of file for description */

/* instantiation */
DMA_RING_ID rng_dmaCreate(int nbytes);
STATUS rng_dmaDelete(DMA_RING_ID ring_name);

/* Empty/Full Status routines */
BOOL rng_dmaIsFull (DMA_RING_ID ring_name);
BOOL rng_dmaIsEmpty (DMA_RING_ID ring_name);
UINT32 rng_dmaBytesUsed(DMA_RING_ID ring_name);

/* DMA routines*/
char* rng_dmaApply(DMA_RING_ID ring_name, int nbytes,int method);
STATUS rng_dmaWritten(DMA_RING_ID ring_name, int nbytes);
char* rng_dmaRead(DMA_RING_ID ring_name, int nbytes,int method);
STATUS rng_dmaFree(DMA_RING_ID ring_name, int nbytes);

/* non-DMA routines */
STATUS rng_dmaBufPut(DMA_RING_ID ring_name,char* place, int nbytes);
STATUS rng_dmaBufGet(DMA_RING_ID ring_name,char* place, int nbytes);

/* Show Ring Pointers - for debuging only */
void RingShow(DMA_RING_ID ring);

#ifdef __cplusplus
}
#endif 

#endif /* __INCrng_dmah */




