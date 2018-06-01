#include "sd_mdc_mqtv24.h"
#include "fw_roppc.h"

int JTAGStep(UINT32 nVmeBusAdrs,UINT16 data_write,UINT16 *data_read)
{

	UINT32 nAdrsWidth;
	UINT16 pData;
	UINT16 nData;
	
	nAdrsWidth=VA24;
	nData=data_write;
	//printf("Enterig.. JTAGStep.\n") ;

	if(WriteToVme16(nVmeBusAdrs,nAdrsWidth,nData)!=0)
	{
		return -1;
	}
	if(ReadFrmVme16(nVmeBusAdrs,nAdrsWidth,&pData)!=0)
	{
		return -2;
	}
	*data_read=pData;

	//printf("Exiting JTAGStep.\n") ;
	return 0;
}


