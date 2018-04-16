/*includes*/
#include "stdio.h"
#include "5100_test.h"
#include "vmerw.h"
/*-------------------------*/
/* VME read  test codes */
/*-------------------------*/
int ReadFrmVme8(UINT32 nVmeBusAdrs, 
				UINT32 nAdrsWidth, 
				UINT8 *pData)
{
	switch(nAdrsWidth)
	{
	case VA16:
		if((nVmeBusAdrs >=0x00010000)||(nVmeBusAdrs <0))
		{
			printf("exceeded the normal range of A16 Window that less than 64KB\n");
			return -1;
		}
	break;
	case VA24:
		if((nVmeBusAdrs >=0x01000000)||(nVmeBusAdrs <0))
		{
			printf("exceeded the normal range of A24 Window that less than 16MB\n");
			return -1;
		}
	break;
	case VA32:
		if((nVmeBusAdrs >=0x08000000)||(nVmeBusAdrs <0))
		{
			printf("exceeded the normal range of A32 Window that less than 128MB\n");
			return -1;
		}
	break;
	case VCSR:
		if((nVmeBusAdrs >=0x01000000)||(nVmeBusAdrs <0))   /*24bit 0x30000000-0x30ffffff*/
		{
			printf("exceeded the normal range of VCSR Window that less than 16MB\n");
			return -1;
		}
	break;
	default:
		printf("Unknown address type, only VA16, VA24, VA32, VCSR supported\n") ;
		return -1 ;
	}
	
	*pData = *(UINT8*)(nVmeBusAdrs + nAdrsWidth) ;	
	printf ("READ 8 --> %#x, %#x, %#x\n", nAdrsWidth, nVmeBusAdrs, *pData) ;
	return 0 ;
}

int ReadFrmVme16(UINT32 nVmeBusAdrs, 
				UINT32 nAdrsWidth, 
				UINT16 *pData)
{
	switch(nAdrsWidth)
	{
	case VA16:
		if((nVmeBusAdrs >=0x00010000)||(nVmeBusAdrs <0))
		{
			printf("exceeded the normal range of A16 Window that less than 64KB\n");
			return -1;
		}
	break;
	case VA24:
		if((nVmeBusAdrs >=0x01000000)||(nVmeBusAdrs <0))
		{
			printf("exceeded the normal range of A24 Window that less than 16MB\n");
			return -1;
		}
	break;
	case VA32:
		if((nVmeBusAdrs >=0x08000000)||(nVmeBusAdrs <0))
		{
			printf("exceeded the normal range of A32 Window that less than 128MB\n");
			return -1;
		}
	break;
	case VCSR:
		if((nVmeBusAdrs >=0x01000000)||(nVmeBusAdrs <0))   /*24bit 0x30000000-0x30ffffff*/
		{
			printf("exceeded the normal range of VCSR Window that less than 16MB\n");
			return -1;
		}
	break;
	default:
		printf("Unknown address type, only VA16, VA24, VA32 ,VCSR supported\n") ;
		return -1 ;
	}

	*pData = *(UINT16*)(nVmeBusAdrs + nAdrsWidth) ;
	printf ("READ 16 --> %#x, %#x, %#x\n", nAdrsWidth, nVmeBusAdrs, *pData) ;
	return 0 ;
}

int ReadFrmVme32(UINT32 nVmeBusAdrs, 
				UINT32 nAdrsWidth, 
				UINT32 *pData)
{
	switch(nAdrsWidth)
	{
	case VA16:
		if((nVmeBusAdrs >=0x00010000)||(nVmeBusAdrs <0))
		{
			printf("exceeded the normal range of A16 Window that less than 64KB\n");
			return -1;
		}
	break;
	case VA24:
		if((nVmeBusAdrs >=0x01000000)||(nVmeBusAdrs <0))
		{
			printf("exceeded the normal range of A24 Window that less than 16MB\n");
			return -1;
		}
	break;
	case VA32:
		if((nVmeBusAdrs >=0x08000000)||(nVmeBusAdrs <0))
		{
			printf("exceeded the normal range of A32 Window that less than 128MB\n");
			return -1;
		}
	break;
	case VCSR:
		if((nVmeBusAdrs >=0x01000000)||(nVmeBusAdrs <0))   /*24bit 0x30000000-0x30ffffff*/
		{
			printf("exceeded the normal range of VCSR Window that less than 16MB\n");
			return -1;
		}
	break;
	default:
		printf("Unknown address type, only VA16, VA24, VA32 ,VCSR supported\n") ;
		return -1 ;
	}
	*pData = *(UINT32*)(nVmeBusAdrs + nAdrsWidth) ;
	/*printf ("READ 32 --> %#x, %#x, %#x\n", nAdrsWidth, nVmeBusAdrs, *pData) ;*/
	return 0 ;
}


/*-------------------------*/
/* VME write  test codes */
/*-------------------------*/
int WriteToVme8(UINT32 nVmeBusAdrs, 
				UINT32 nAdrsWidth, 
				UINT8 nData
				)
{

	UINT8 *pVmeAdrs8 = (UINT8*)(nVmeBusAdrs + nAdrsWidth) ;

	switch(nAdrsWidth)
	{
		case VA16:
			if((nVmeBusAdrs >=0x00010000)||(nVmeBusAdrs <0))
			{
				printf("exceeded the normal range of A16 Window that less than 64KB\n");
				return -1;
			}
		break;
		case VA24:
			if((nVmeBusAdrs >=0x01000000)||(nVmeBusAdrs <0))
			{
				printf("exceeded the normal range of A24 Window that less than 16MB\n");
				return -1;
			}
		break;
		case VA32:
			if((nVmeBusAdrs >=0x08000000)||(nVmeBusAdrs <0))
			{
				printf("exceeded the normal range of A32 Window that less than 128MB\n");
				return -1;
			}
		break;
		case VCSR:
			if((nVmeBusAdrs >=0x01000000)||(nVmeBusAdrs <0))   /*24bit 0x30000000-0x30ffffff*/
			{
			printf("exceeded the normal range of VCSR Window that less than 16MB\n");
			return -1;
			}
		break;
		default:
			printf("Unknown address type, only VA16, VA24, VA32,VCSR supported\n") ;
			return -1 ;
	}
	*pVmeAdrs8 = nData ;
	printf ("WRITE 8 --> %#x, %#x, %#x\n", nAdrsWidth, nVmeBusAdrs, nData) ;
	return 0 ;
}

int WriteToVme16(UINT32 nVmeBusAdrs, 
				UINT32 nAdrsWidth, 
				UINT16 nData
				)
{

	UINT16 *pVmeAdrs16 = (UINT16*)(nVmeBusAdrs + nAdrsWidth) ;

	switch(nAdrsWidth)
	{
		case VA16:
			if((nVmeBusAdrs >=0x00010000)||(nVmeBusAdrs <0))
			{
				printf("exceeded the normal range of A16 Window that less than 64KB\n");
				return -1;
			}
		break;
		case VA24:
			if((nVmeBusAdrs >=0x01000000)||(nVmeBusAdrs <0))
			{
				printf("exceeded the normal range of A24 Window that less than 16MB\n");
				return -1;
			}
		break;
		case VA32:
			if((nVmeBusAdrs >=0x08000000)||(nVmeBusAdrs <0))
			{
				printf("exceeded the normal range of A32 Window that less than 128MB\n");
				return -1;
			}
		break;
		case VCSR:
			if((nVmeBusAdrs >=0x01000000)||(nVmeBusAdrs <0))   /*24bit 0x30000000-0x30ffffff*/
			{
			printf("exceeded the normal range of VCSR Window that less than 16MB\n");
			return -1;
			}
		break;
		default:
			printf("Unknown address type, only VA16, VA24, VA32 ,VCSR supported\n") ;
			return -1 ;
	}
	*pVmeAdrs16 = nData ;
	printf ("WRITE 16 --> %#x, %#x, %#x\n", nAdrsWidth, nVmeBusAdrs, nData) ;
	return 0 ;
}

int WriteToVme32(UINT32 nVmeBusAdrs, 
				UINT32 nAdrsWidth, 
				UINT32 nData
				)
{

	UINT32 *pVmeAdrs32 = (UINT32*)(nVmeBusAdrs + nAdrsWidth) ;

	switch(nAdrsWidth)
	{
		case VA16:
			if((nVmeBusAdrs >=0x00010000)||(nVmeBusAdrs <0))
			{
				printf("exceeded the normal range of A16 Window that less than 64KB\n");
				return -1;
			}
		break;
		case VA24:
			if((nVmeBusAdrs >=0x01000000)||(nVmeBusAdrs <0))
			{
				printf("exceeded the normal range of A24 Window that less than 16MB\n");
				return -1;
			}
		break;
		case VA32:
			if((nVmeBusAdrs >=0x08000000)||(nVmeBusAdrs <0))
			{
				printf("exceeded the normal range of A32 Window that less than 128MB\n");
				return -1;
			}
		break;
		case VCSR:
			if((nVmeBusAdrs >=0x01000000)||(nVmeBusAdrs <0))   /*24bit 0x30000000-0x30ffffff*/
			{
				printf("exceeded the normal range of VCSR Window that less than 16MB\n");
				return -1;
			}
		break;
		default:
			printf("Unknown address type, only VA16, VA24, VA32 ,VCSR supported\n") ;
			return -1 ;
	}
	*pVmeAdrs32 = nData ;
/*	printf ("WRITE 32 --> %#x, %#x, %#x\n", nAdrsWidth, nVmeBusAdrs, nData) ;*/
	return 0 ;
}

/*------------------*/
/* MCST test codes */
/*------------------*/
int MCSTwrite(UINT32 nVmeBusAdrs,
			int  nDataWidth,
			UINT32 nData)
{
	if((nVmeBusAdrs >=0x08000000)||(nVmeBusAdrs <0))
	{
		printf("exceeded the normal range of A32 Window that less than 128M\n");
		return -1;
		}
		
	switch(nDataWidth)
	{
		case 8:
			if((nData>0xff)||(nData<0))
				printf("warning:the data 0x%x exceeded the range of 8bit\n",nData);
			return WriteToVme8(nVmeBusAdrs, VA32, (UINT8)nData);
			break;
		case 16:
			if((nData>0xffff)||(nData<0))
				printf("warning:the data 0x%x exceeded the range of 16bit\n",nData);
			return WriteToVme16(nVmeBusAdrs, VA32, (UINT16)nData);
			break;
		case 32:
			return WriteToVme32(nVmeBusAdrs, VA32, nData);
			break;
		default:
			printf("Unknow Data Width, only 8,16,32 supported\n");
			return -1;
		}

	return 0;
}
