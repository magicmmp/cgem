#include "sd_mdc_mqtv24.h"
#include "fw_roppc.h"

int JTAGSetData(UINT32 address, UINT32 *input_IR,UINT32 *output_IR,UINT32 length_IR,UINT32 *input_DR,UINT32 *output_DR,UINT32 length_DR)
{
	int i=0,Check;
	UINT16 data_write;
	UINT16 data_read;
	
	//after soft reset, the TAP should stay at "Test logic reset" status
	//This function need the TAP stay at "Run test/idle" status

	data_write=0x00000011;//at "Run test/idle" status
	Check=JTAGStep(address,data_write,&data_read);//goto "Select DR scan" status
	if(Check!=0) return Check;

	data_write=0x00000011;//at "Select DR scan" status
	Check=JTAGStep(address,data_write,&data_read);//goto "Select IR scan" status
	if(Check!=0) return Check;

	data_write=0x00000001;//at "Select IR scan" status
	Check=JTAGStep(address,data_write,&data_read);//goto "Capture IR" status
	if(Check!=0) return Check;

	data_write=0x00000001;//at "Capture IR" status
	Check=JTAGStep(address,data_write,&data_read);//goto "Shift IR" status
	if(Check!=0) return Check;

	//at "Shift IR" status
	for(i=length_IR;i>1;i--)//writ new IR data
	{
		if((*input_IR)==1) data_write=0x00000001;
		else data_write=0x00000000;
		input_IR++;
		Check=JTAGStep(address,data_write,&data_read);
		*output_IR=(UINT32)data_read;
		output_IR++;
		if(Check!=0) return Check;
	}
	if((*input_IR)==1) data_write=0x00000011;
	else data_write=0x00000010;
	Check=JTAGStep(address,data_write,&data_read);//goto "Exit1 IR" status
	*output_IR=(UINT32)data_read;
	output_IR++;
	if(Check!=0) return Check;

	data_write=0x00000011;//at "Exit1 IR" status
	Check=JTAGStep(address,data_write,&data_read);//goto "Update IR" status
	if(Check!=0) return Check;
	
	data_write=0x00000011;//at "Update IR" status
	Check=JTAGStep(address,data_write,&data_read);//goto "Select DR scan" status
	if(Check!=0) return Check;

	data_write=0x00000001;//at "Select DR scan" status
	Check=JTAGStep(address,data_write,&data_read);//goto "Capture DR" status
	if(Check!=0) return Check;

	data_write=0x00000001;//at "Capture DR" status
	Check=JTAGStep(address,data_write,&data_read);//goto "Shift DR" status
	if(Check!=0) return Check;

	//at "Shift DR" status
	for(i=length_DR;i>1;i--)//writ new DR data
	{
		if((*input_DR)==1) data_write=0x00000001;
		else data_write=0x00000000;
		input_DR++;
		Check=JTAGStep(address,data_write,&data_read);
		*output_DR=(UINT32)data_read;
		output_DR++;
		if(Check!=0) return Check;
	}
	if((*input_DR)==1) data_write=0x00000011;
	else data_write=0x00000010;
	Check=JTAGStep(address,data_write,&data_read);//goto "Exit1 DR" status
	*output_DR=(UINT32)data_read;
	output_DR++;
	if(Check!=0) return Check;

	data_write=0x00000011;//at "Exit1 DR" status
	Check=JTAGStep(address,data_write,&data_read);//goto "Update DR" status
	if(Check!=0) return Check;

	data_write=0x00000001;//at "Update DR" status
	Check=JTAGStep(address,data_write,&data_read);//goto "Run test/idle" status
	if(Check!=0) return Check;

	return 0;
}

