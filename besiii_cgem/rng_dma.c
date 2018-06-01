/* includes */
#include "rng_dma.h"
#include "cacheLib.h"
/*#include "FastCopy.h"*/
#include "stdlib.h"
/*#include "semLib.h"*/
#include "logLib.h"

/*----------------------------------*/
/*    Buffer Create and Delete                   *//*modified by leigk version change for the DMA cache  */
/*----------------------------------*/

/* Create a new ring buffer */

DMA_RING_ID rng_dmaCreate(int nbytes)
{
	/* allocate memory for struct */
	DMA_RING_ID newring = (DMA_RING_ID)malloc(sizeof(DMA_RING));
	if (newring == NULL)
	{
		printf("Couldn't allocate memory for ring buffer\n");
		return NULL;
	}

	/* allocate the memory */
	newring->bufBase = (char*)cacheDmaMalloc(nbytes+1024);
	if (newring->bufBase == NULL)
	{
		printf("Couldn't allocate memory for ring buffer\n");
		return NULL;
	}
	newring->bufStart =(char*)( ((UINT)(newring->bufBase)& 0xffffffe0) + 32);

	/* fix starting values of the other pointers */
	newring->bufEnd = newring->bufStart + nbytes;
	newring->pWrite = newring->bufStart;
	newring->pRead = NULL;                   /* empty - nothing to read */
	newring->pLastWrite = newring->bufEnd;
	newring->nBufSize=(newring->bufEnd)-(newring->bufStart);
	newring->semMutex = semBCreate(SEM_Q_FIFO,SEM_FULL);

	return newring;
}

STATUS rng_dmaDelete(DMA_RING_ID ring_name)
{
	semDelete(ring_name->semMutex) ;
	if (ring_name->bufBase == NULL)
	{
		printf("Ring Buffer Delete Failed \n");
		return ERROR;
	}
	else
	{
		cacheDmaFree((void*)(ring_name->bufBase)); 
		ring_name->bufBase=NULL;
		free(ring_name);
		ring_name = NULL;
	}
	return OK;
}

/*--------------------------------*/
/*   Full/Empty Status routines   */
/*--------------------------------*/

BOOL rng_dmaIsEmpty (DMA_RING_ID ring_name)
{
	if (ring_name->pRead == NULL)
	{
		return TRUE;
	}
	else
	{
		return FALSE;
	}
}

BOOL rng_dmaIsFull (DMA_RING_ID ring_name)
{
	if (ring_name->pWrite == NULL)
	{
		return TRUE;
	}
	else
	{
		return FALSE;
	}
}

UINT32 rng_dmaBytesUsed(DMA_RING_ID ring_name)
{
	/* if empty */
	if (ring_name->pRead == NULL) return 0;
	/* if full */
	if (ring_name->pWrite == NULL) return ring_name->bufEnd - ring_name->bufStart;

	/* if write ahead of read */
	if (ring_name->pWrite > ring_name->pRead)return ring_name->pWrite - ring_name->pRead;

	/* if read ahead of write */
	if (ring_name->pRead > ring_name->pWrite)
	{
		return  ((ring_name->pLastWrite - ring_name->pRead)+ (ring_name->pWrite -  ring_name->bufStart));
	}
	/* shouldn't get here */
	printf("Error in rng_bytesUsed\n");
	return 0;
}

/*----------------------------------*/
/*   DMA buffer handling routines   */
/*----------------------------------*/

char* rng_dmaApply(DMA_RING_ID ring_name, int nbytes,int method)
{
 	semTake(ring_name->semMutex, WAIT_FOREVER) ; 

	if((nbytes<0)||(nbytes>ring_name->nBufSize)) /* if apply size outof range*/
	{
		printf("apply size %d <0 or >total ringbuf\n",nbytes);
		return NULL;
	}
	
	if(nbytes&0x1f)
	{
		nbytes=(nbytes&0xffffffe0)+32;
	}
	
	if (ring_name->pWrite == NULL)		 /* if ringbuf full return NULL */
	{
		semGive(ring_name->semMutex);
		return NULL;
	}
	
	if ((int)(ring_name->pRead - ring_name->pWrite) >= 0)   /* if read is ahead of write */
	{
		
		if ((int)(ring_name->pRead - ring_name->pWrite) >= nbytes) /* if enough space */
		{
			semGive(ring_name->semMutex);
			if (method == DMA_METHOD)		/* If DMA write then invalidate the cache for that memory */
			{
			 	CACHE_DMA_INVALIDATE(ring_name->pWrite,nbytes);
			}
			return ring_name->pWrite;	/*32bit use to save block real data infor*/
		}
		else
		{
			semGive(ring_name->semMutex);
			return NULL;
		}
	}
	else		/* else - write ahead of read */
	{
		
		if((int)(ring_name->bufEnd - ring_name->pWrite) >= nbytes)  /* if space before bufEnd */
		{
			semGive(ring_name->semMutex);
			if (method == DMA_METHOD)		/* If DMA write then invalidate the cache for that memory */
			{
			 	CACHE_DMA_INVALIDATE(ring_name->pWrite,nbytes);
			}
			return ring_name->pWrite;  /*32bit use to save block real data infor*/
		}
		else /* else - check space at bufStart */
		{
			if (((int)(ring_name->pRead - ring_name->bufStart) >= nbytes) ||
												((ring_name->pRead == NULL)))
			{
				ring_name->pLastWrite = ring_name->pWrite;
				ring_name->pWrite = ring_name->bufStart;
				semGive(ring_name->semMutex);
				if (method == DMA_METHOD)			/* If DMA write then invalidate the cache for that memory */
				{
				 	CACHE_DMA_INVALIDATE(ring_name->pWrite,nbytes);
				}
				return ring_name->pWrite;
			}
			else 
			{
				/*printf("apply ring size error\n");*/
				semGive(ring_name->semMutex);
				return NULL;
			}
		}
	}
}



/* After a dma (or other write process notify ring buffer of actual
   number of bytes written - note pwrite should not have been modified */

STATUS rng_dmaWritten(DMA_RING_ID ring_name, int nbytes)
{
	semTake(ring_name->semMutex, WAIT_FOREVER) ; 

	if(nbytes<0)
	{
		printf("writen size %d <0 to ringbuf\n",nbytes);
		return ERROR;
	}
	
	if(nbytes&0x1f)
	{
		nbytes=(nbytes&0xffffffe0)+32;
	}
	
	if ((ring_name->pRead == NULL)&&(nbytes>0))		/* if the buffer was empty then set read -*/
	{												        /*pointer to where the write started */
		ring_name->pRead=ring_name->pWrite;
		ring_name->pLastWrite=ring_name->bufEnd;
	}
	
	ring_name->pWrite+=nbytes;  /* advance the write pointer by nybtes marking */
	
	if (ring_name->pWrite >= ring_name->bufEnd)/* handle the possiblty of either being at the end of the
											physical space or being full */
	{
		ring_name->pWrite -= ring_name->bufEnd - ring_name->bufStart;
	}
	if (ring_name->pWrite == ring_name->pRead)
	{
		ring_name->pWrite = NULL;
	}
	semGive(ring_name->semMutex);
	return OK;
}


/*----------*/
/* DMA READ */
/*----------*/


char* rng_dmaRead(DMA_RING_ID ring_name, int nbytes,int method)
{
	semTake(ring_name->semMutex, WAIT_FOREVER) ;
	if(nbytes&0x1f)
	{
		nbytes=(nbytes&0xffffffe0)+32;
	}
	if (ring_name->pRead == NULL)	/* If ring buffer is empty */
	{
/*		printf("not enough data written to get1\n");*/
		semGive(ring_name->semMutex);
		return NULL;      
	}
	if ((int)(ring_name->pWrite - ring_name->pRead)>=0)/* Possibilities if Read is before Write pointer */
	{
		if ((int)(ring_name->pWrite - ring_name->pRead)>=nbytes)
		{
			if (method == DMA_METHOD)
			{
				CACHE_DMA_FLUSH(ring_name->pRead,nbytes);
			}
			semGive(ring_name->semMutex);
			return  ring_name->pRead;
		}
		else
		{ 
			printf ("not enough data written to get2\n"); 
			semGive(ring_name->semMutex);
			return NULL;
		}
	}

	if ((int)(ring_name->pLastWrite - ring_name->pRead)>=nbytes) /* Possibilities if Write is before Read pointer */
	{ 																  /* if enough data before Last Write pointer */
		if (method == DMA_METHOD)
		{
			CACHE_DMA_FLUSH(ring_name->pRead,nbytes);
		}
		semGive(ring_name->semMutex);
		return ring_name->pRead; 
	}
	/* else - Read is before Write */
	else 
	{ 
		printf ("not enough data written to get3\n");
		semGive(ring_name->semMutex);
		return NULL;
	}
}


/* this function moves the read pointer, freeing nbytes worth 
   of space */

STATUS rng_dmaFree(DMA_RING_ID ring_name, int nbytes)
{
	semTake(ring_name->semMutex, WAIT_FOREVER) ; 
	if(nbytes&0x1f)
	{
		nbytes=(nbytes&0xffffffe0)+32;
	}
	if (ring_name->pWrite == NULL)/* if buffer was full set pwrite to where the freed memory starts */
	{
		ring_name->pWrite = ring_name->pRead;
	}
	
	ring_name->pRead += nbytes;/* move up read pointer */
	
	/* if you have read to lastwrite then start at the beginning
	of the buffer and put lastwrite to end, and if empty putread to null */
	if (ring_name->pRead >= ring_name->pLastWrite)
	{
		ring_name->pRead -= ring_name->pLastWrite - ring_name->bufStart;
		ring_name->pLastWrite = ring_name->bufEnd;
	}
	if (ring_name->pRead == ring_name->pWrite)
	{
		ring_name->pRead = NULL;
	}
	semGive(ring_name->semMutex);
	return OK;
}



/*----------------------------------*/
/* NON DMA buffer handling routines */
/*----------------------------------*/

/*----------------------------------*/
/* Show DMA buffer routines */
/*----------------------------------*/

void RingShow (DMA_RING_ID ring)
{
	logMsg("Start: %x \n",(UINT32)ring->bufStart,0,0,0,0,0);
	logMsg("End: %x \n",(UINT32)ring->bufEnd,0,0,0,0,0);
	logMsg("pWrite: %x \n",(UINT32)ring->pWrite,0,0,0,0,0);
	logMsg("pRead: %x \n",(UINT32)ring->pRead,0,0,0,0,0);
	logMsg("pLastWrite: %x \n",(UINT32)ring->pLastWrite,0,0,0,0,0);
}

