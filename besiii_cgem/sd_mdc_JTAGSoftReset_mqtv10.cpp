#include "sd_mdc_mqtv24.h"
#include "fw_roppc.h"

int JTAGSoftReset(UINT32 address)
{
	int i,Check;
	UINT16 data_write;
	UINT16 data_read;

	//printf("Entering... JTAGSoftReset...\n") ;
	taskDelay(60) ;
	data_write=0x00000011;//at any status

	//goto "Test logic reset" status
	
	for(i=1;i<=5;i++)
	{
		Check=JTAGStep(address,data_write,&data_read);
		if(Check!=0) return Check;
	}
	
	data_write=0x00000001;	//at "Test logic reset" status
	Check=JTAGStep(address,data_write,&data_read);//goto "Run test/idle" status
	if(Check!=0) return Check;

	//printf("Exiting... JTAGSoftReset...\n") ;
	return 0;
}


