#include "sd_mdc_mqtv24.h"
#include "fw_roppc.h"
#include "sd_mdc.h"

int JTAGGetIDCODE(UINT32 address);
int JTAGSetSetup(UINT32 address, UINT32 window, UINT32 trigger_latency);
int HPTDCinitialization(UINT32 address, UINT32 enable);
int JTAGResetPLLandDLL(UINT32 address, UINT32 enable);
int JTAGPLLLocking(UINT32 address, UINT32 enable);
int JTAGDLLLocking(UINT32 address, UINT32 enable);
int JTAGGlobalReset(UINT32 address, UINT32 enable);
int JTAGEnabaleOutput(UINT32 address, UINT32 enable);
void JTAGReadStatus(UINT32 address);

int hptdcupload(UINT32 address, UINT32 enable, UINT32 window, UINT32 trigger_latency)
{
	UINT32 hptdc_jtag_setup;
	char n;
	UINT32 nFunctionAddress;
	int Check;

	nFunctionAddress=MQTVMEJTAG;
	hptdc_jtag_setup=((address)<<19) | (nFunctionAddress<<12);
	Check=JTAGSoftReset(hptdc_jtag_setup);
	if(Check!=0) return Check;

	Check=JTAGGetIDCODE(hptdc_jtag_setup);
	if(Check!=0) return Check;

	Check= JTAGSetSetup(hptdc_jtag_setup,window,trigger_latency);
	if(Check!=0) return Check;

	Check=HPTDCinitialization(hptdc_jtag_setup,enable);
	if(Check!=0) return Check;

	#ifdef MQT_PRINT
	printf("\n\nPress 'x' to quit, any others to continue...");
	n=getchar();	
	while(n!='x')
	{
		JTAGReadStatus(hptdc_jtag_setup);
		printf("\n\nPress 'x' to quit, any others to continue...");
		n=getchar();
	}

	#endif
	return 0;
}

int JTAGGetIDCODE(UINT32 address)
{
	int i;
	UINT32 input_IR[5],output_IR[5];
	UINT32 length_IR=5;
	UINT32 input_DR[32],output_DR[32];
	UINT32 length_DR=32;
	int Check;
	UINT32 ID13[32];	

	ID13[31]=1;
	ID13[30]=0;
	ID13[29]=0;
	ID13[28]=0;
	ID13[27]=0;
	ID13[26]=1;
	ID13[25]=0;
	ID13[24]=0;
	ID13[23]=0;
	ID13[22]=1;
	ID13[21]=1;
	ID13[20]=1;
	ID13[19]=0;
	ID13[18]=0;
	ID13[17]=0;
	ID13[16]=0;
	ID13[15]=1;
	ID13[14]=1;
	ID13[13]=0;
	ID13[12]=1;
	ID13[11]=1;
	ID13[10]=0;
	ID13[9]=1;
	ID13[8]=0;
	ID13[7]=1;
	ID13[6]=1;
	ID13[5]=0;
	ID13[4]=0;
	ID13[3]=1;
	ID13[2]=1;
	ID13[1]=1;
	ID13[0]=0;

	input_IR[4]=1;
	input_IR[3]=0;
	input_IR[2]=0;
	input_IR[1]=0;
	input_IR[0]=1;

	for(i=0;i<32;i++)
	{
		input_DR[i]=0;
		output_DR[i]=0;
	}

	Check=JTAGSetData(address,input_IR,output_IR,length_IR,input_DR,output_DR,length_DR);
	if(Check!=0) return Check;

	for(i=0;i<32;i++)
	{
		if(ID13[i]!=output_DR[i])
		{
			#ifdef MQT_PRINT
			printf("IDCODE check error\n");
			for(i=31;i>=0;i--)
			{
				printf("%1d",output_DR[i]);
			}
			printf("\n");
			printf("\n");
			#endif
			return -3;
		}
	}	

	#ifdef MQT_PRINT
	printf("IDCODE\n");
	for(i=31;i>=0;i--)
	{
		printf("%1d",output_DR[i]);
	}
	printf("\n");
	printf("\n");
	#endif


	return 0;
}

int JTAGSetSetup(UINT32 address,UINT32 window,UINT32 trigger_latency)
{
	int i;
	UINT32 x;
	UINT32 input_DR[647],output_DR[647];
	UINT32 length_DR=647;
	UINT32 input_IR[5],output_IR[5];
	UINT32 length_IR=5;
	int Check;

	UINT32 search_window;
	UINT32 match_window;
	UINT32 trigger_count_offset;
	UINT32 reject_count_offset;
	
	match_window=window;
	search_window=match_window+2;

	trigger_count_offset=0xfff-trigger_latency+1;
	reject_count_offset=0xfff-trigger_latency-1;

	input_IR[4]=1;
	input_IR[3]=1;
	input_IR[2]=0;
	input_IR[1]=0;
	input_IR[0]=0;
	
	UINT32 setup1[647];

    //test_select[3:0]--setup1[3:0] Selection of test output signal(1110)
                        setup1[0]=0;
                        setup1[1]=1;
                        setup1[2]=1;
                        setup1[3]=1;
    //enable_error_mark--setup1[4] Mark events with error if global error signal set(1)
                        setup1[4]=1;
    //enable_error_bypass--setup1[5] Bypass TDC chip if global error signal set(0)
                        setup1[5]=0;
    //enable_error[10:0]--setup1[16:6] Enable of internal error types for generation of global error signal(all set)
                        setup1[6]=1;
                        setup1[7]=1;
                        setup1[8]=1;
                        setup1[9]=1;
                        setup1[10]=1;
                        setup1[11]=1;
                        setup1[12]=1;
                        setup1[13]=1;
                        setup1[14]=1;
                        setup1[15]=1;
                        setup1[16]=1;
    //readout_single_cycle_speed[2:0]--setup1[19:17] Serial transmission speed in single cycle mode(0)
                        setup1[17]=0;
                        setup1[18]=0;
                        setup1[19]=0;
    //serial_delay[3:0]--setup1[23:20] Programmable delay of serial input(0)
                        setup1[20]=0;
                        setup1[21]=0;
                        setup1[22]=0;
                        setup1[23]=0;
    //strobe_select[1:0]--setup1[25:24] Selection of serial strobe type(0)
                        setup1[24]=0;
                        setup1[25]=0;
    //readout_speed_select--setup1[26] Selection of serial read-out speed(0)
                        setup1[26]=0;
    //token_delay[3:0]--setup1[30:27] Programmable delay of token input(0)
                        setup1[27]=0;
                        setup1[28]=0;
                        setup1[29]=0;
                        setup1[30]=0;
    //enable_local_trailer--setup1[31] Enable of local trailers in read-out(1)
                        setup1[31]=0;
    //enable_local_header--setup1[32] Enable of local headers in read-out(1)
                        setup1[32]=0;
    //enable_global_trailer--setup1[33] Enable of global trailers in read-out(1)
                        setup1[33]=1;
    //enable_global_header--setup1[34] Enable of global headers in read-out(1)
                        setup1[34]=1;
    //keep_token--setup1[35] Keep token until end of event or no more data(1) otherwise pass token after each word read. Mustbe enabled when using trigger matching
                        setup1[35]=1;
    //master--setup1[36] Master chip in token ring(0)
                        setup1[36]=1;
    //enable_bytewise--setup1[37] Enable of byte-wise readout(0)(setup1[38] must be 0)
                        setup1[37]=0;
    //enable_serial--setup1[38] Enable of serial read-out(otherwise paralle1 read-out)(0)
                        setup1[38]=0;
    //enable_jtag_readout--setup1[39] Enable of readout via JTAG(0)(overrides other readout modes)
                        setup1[39]=0;
    //tdc_id[3:0]--setup1[43:40] TDC identifier in readout
                        setup1[40]=0;
                        setup1[41]=0;
                        setup1[42]=0;
                        setup1[43]=0;
    //select_bypass_inputs--setup1[44] Select serial in and token in from bypass inputs(0)
                        setup1[44]=0;
    //readout_fifo_size[2:0]--setup1[47:45] Effective size of readout fifo(111)
                        setup1[45]=1;
                        setup1[46]=1;
                        setup1[47]=1;
    //reject_count_offset[11:0]--setup1[59:48] Reject counter offset defines reject latency together with coarse count offset
                        setup1[48]=reject_count_offset & 0x1;
                        setup1[49]=(reject_count_offset & 0x2)>>1;
                        setup1[50]=(reject_count_offset & 0x4)>>2;
                        setup1[51]=(reject_count_offset & 0x8)>>3;
                        setup1[52]=(reject_count_offset & 0x10)>>4;
                        setup1[53]=(reject_count_offset & 0x20)>>5;
                        setup1[54]=(reject_count_offset & 0x40)>>6;
                        setup1[55]=(reject_count_offset & 0x80)>>7;
                        setup1[56]=(reject_count_offset & 0x100)>>8;
                        setup1[57]=(reject_count_offset & 0x200)>>9;
                        setup1[58]=(reject_count_offset & 0x400)>>10;
                        setup1[59]=(reject_count_offset & 0x800)>>11;
    //search_window[11:0]--setup1[71:60] Search window in number of clock cycles
                        setup1[60]=search_window & 0x1;
                        setup1[61]=(search_window & 0x2)>>1;
                        setup1[62]=(search_window & 0x4)>>2;
                        setup1[63]=(search_window & 0x8)>>3;
                        setup1[64]=(search_window & 0x10)>>4;
                        setup1[65]=(search_window & 0x20)>>5;
                        setup1[66]=(search_window & 0x40)>>6;
                        setup1[67]=(search_window & 0x80)>>7;
                        setup1[68]=(search_window & 0x100)>>8;
                        setup1[69]=(search_window & 0x200)>>9;
                        setup1[70]=(search_window & 0x400)>>10;
                        setup1[71]=(search_window & 0x800)>>11;
    //match_window[11:0]--setup1[83:72] Matching window in number of clock cycles
                        setup1[72]=match_window & 0x1;
                        setup1[73]=(match_window & 0x2)>>1;
                        setup1[74]=(match_window & 0x4)>>2;
                        setup1[75]=(match_window & 0x8)>>3;
                        setup1[76]=(match_window & 0x10)>>4;
                        setup1[77]=(match_window & 0x20)>>5;
                        setup1[78]=(match_window & 0x40)>>6;
                        setup1[79]=(match_window & 0x80)>>7;
                        setup1[80]=(match_window & 0x100)>>8;
                        setup1[81]=(match_window & 0x200)>>9;
                        setup1[82]=(match_window & 0x400)>>10;
                        setup1[83]=(match_window & 0x800)>>11;
    //leading_resolution[2:0]--setup1[86:84] Resolution of leading/trailing edge(001)
                        setup1[84]=0;
                        setup1[85]=0;
                        setup1[86]=0;
    //fixed_pattern[27:0]--setup1[114:87] Fixed readout pattern(for debugging)
                        setup1[86]=1;
                        setup1[87]=1;
                        setup1[88]=1;
                        setup1[89]=1;
                        setup1[90]=1;
                        setup1[91]=1;
                        setup1[92]=1;
                        setup1[93]=1;
                        setup1[94]=1;
                        setup1[95]=1;
                        setup1[96]=1;
                        setup1[97]=1;
                        setup1[98]=1;
                        setup1[99]=1;
                        setup1[100]=1;
                        setup1[101]=1;
                        setup1[102]=1;
                        setup1[103]=1;
                        setup1[104]=1;
                        setup1[105]=1;
                        setup1[106]=1;
                        setup1[107]=1;
                        setup1[108]=1;
                        setup1[109]=1;
                        setup1[110]=1;
                        setup1[111]=0;
                        setup1[112]=1;
                        setup1[113]=0;
                        setup1[114]=1;
    //enable_fixed_pattern--setup1[115] Enable readout of fixed pattern(for debugging)
                        setup1[115]=0;
    //max_event_size[3:0]--setup1[119:116] Maximum number of hits per event(1001)(only valid when using trigger matching)
                        setup1[116]=1;
                        setup1[117]=0;
                        setup1[118]=0;
                        setup1[119]=1;
    //reject_readout_fifo_full--setup1[120] Reject hits when read-out fifo full(1)
                        setup1[120]=1;
    //enable_readout_occupancy--setup1[121] Enable readout of buffer occupancies(0)(for debugging)
                        setup1[121]=0;
    //enable_readout_separator--setup1[122] Enable readout of separators(0)(for debugging)
                        setup1[122]=0;
    //enable_overflow_detect--setup1[123] Enable overflow detect of L1 buffer(1)(should always be enabled)
                        setup1[123]=1;
    //enable_relative--setup1[124] Enable read-out of relative time to trigger time tag(only valid when using trigger matching)
                        setup1[124]=1;
    //enable_automatic_reject--setup1[125] Enable of automatic rejection(1)(should always be enabled if trigger matching)
                        setup1[125]=1;
    //event_count_offset[11:0]--setup1[137:126] Event number offset(0)
                        setup1[126]=0;
                        setup1[127]=0;
                        setup1[128]=0;
                        setup1[129]=0;
                        setup1[130]=0;
                        setup1[131]=0;
                        setup1[132]=0;
                        setup1[133]=0;
                        setup1[134]=0;
                        setup1[135]=0;
                        setup1[136]=0;
                        setup1[137]=0;
    //trigger_count_offset[11:0]--setup1[149:138] Trigger time tag counter offset(used to set effective trigger latency)
                        setup1[138]=trigger_count_offset & 0x1;
                        setup1[139]=(trigger_count_offset & 0x2)>>1;
                        setup1[140]=(trigger_count_offset & 0x4)>>2;
                        setup1[141]=(trigger_count_offset & 0x8)>>3;
                        setup1[142]=(trigger_count_offset & 0x10)>>4;
                        setup1[143]=(trigger_count_offset & 0x20)>>5;
                        setup1[144]=(trigger_count_offset & 0x40)>>6;
                        setup1[145]=(trigger_count_offset & 0x80)>>7;
                        setup1[146]=(trigger_count_offset & 0x100)>>8;
                        setup1[147]=(trigger_count_offset & 0x200)>>9;
                        setup1[148]=(trigger_count_offset & 0x400)>>10;
                        setup1[149]=(trigger_count_offset & 0x800)>>11;
    //enable_set_counters_on_bunch_reset--setup1[150] Enable all counters to be set on bunch count reset(1)
                        setup1[150]=1;
    //enable_master_reset_code--setup1[151] Enable master reset code on encoded_control(0)
                        setup1[151]=0;
    //enable_master_reset_on_event_reset--setup1[152] Enable master reset of whole TDC on event reset(0)
                        setup1[152]=0;
    //enable_reset_channel_buffer_when_separator--setup1[153] Enable reset channel buffers when separator(0)
                        setup1[153]=0;
    //enable_separator_on_event_reset--setup1[154] Enable generation of separator on event reset(0)
                        setup1[154]=0;
    //enable_separator_on_bunch_reset--setup1[155] Enable generation of separator on bunch reset(0)
                        setup1[155]=0;
    //enable_direct_event_reset--setup1[156] Enable of direct event reset input pin(1)(otherwise taken from encoded control)
                        setup1[156]=1;
    //enable_direct_bunch_reset--setup1[157] Enable of direct bunch reset input pin(1)(otherwise taken from encoded control)
                        setup1[157]=1;
    //enable_direct_trigger--setup1[158] Enable of direct trigger input pin(1)(otherwise taken from encoded control)
                        setup1[158]=1;
    //offset31[8:0]--setup1[167:159] Offset adjust for channel 31
                        setup1[159]=0;
                        setup1[160]=0;
                        setup1[161]=0;
                        setup1[162]=0;
                        setup1[163]=0;
                        setup1[164]=0;
                        setup1[165]=0;
                        setup1[166]=0;
                        setup1[167]=0;
    //offset30[8:0]--setup1[176:168] Offset adjust for channel 30
                        setup1[168]=0;
                        setup1[169]=0;
                        setup1[170]=0;
                        setup1[171]=0;
                        setup1[172]=0;
                        setup1[173]=0;
                        setup1[174]=0;
                        setup1[175]=0;
                        setup1[176]=0;
    //offset29[8:0]--setup1[185:177] Offset adjust for channel 29
                        setup1[177]=0;
                        setup1[178]=0;
                        setup1[179]=0;
                        setup1[180]=0;
                        setup1[181]=0;
                        setup1[182]=0;
                        setup1[183]=0;
                        setup1[184]=0;
                        setup1[185]=0;
    //offset28[8:0]--setup1[194:186] Offset adjust for channel 28
                        setup1[186]=0;
                        setup1[187]=0;
                        setup1[188]=0;
                        setup1[189]=0;
                        setup1[190]=0;
                        setup1[191]=0;
                        setup1[192]=0;
                        setup1[193]=0;
                        setup1[194]=0;
    //offset27[8:0]--setup1[203:195] Offset adjust for channel 27
                        setup1[195]=0;
                        setup1[196]=0;
                        setup1[197]=0;
                        setup1[198]=0;
                        setup1[199]=0;
                        setup1[200]=0;
                        setup1[201]=0;
                        setup1[202]=0;
                        setup1[203]=0;
    //offset26[8:0]--setup1[212:204] Offset adjust for channel 26
                        setup1[204]=0;
                        setup1[205]=0;
                        setup1[206]=0;
                        setup1[207]=0;
                        setup1[208]=0;
                        setup1[209]=0;
                        setup1[210]=0;
                        setup1[211]=0;
                        setup1[212]=0;
    //offset25[8:0]--setup1[221:213] Offset adjust for channel 25
                        setup1[213]=0;
                        setup1[214]=0;
                        setup1[215]=0;
                        setup1[216]=0;
                        setup1[217]=0;
                        setup1[218]=0;
                        setup1[219]=0;
                        setup1[220]=0;
                        setup1[221]=0;
    //offset24[8:0]--setup1[230:222] Offset adjust for channel 24
                        setup1[222]=0;
                        setup1[223]=0;
                        setup1[224]=0;
                        setup1[225]=0;
                        setup1[226]=0;
                        setup1[227]=0;
                        setup1[228]=0;
                        setup1[229]=0;
                        setup1[230]=0;
     //offset23[8:0]--setup1[239:231] Offset adjust for channel 23
                        setup1[231]=0;
                        setup1[232]=0;
                        setup1[233]=0;
                        setup1[234]=0;
                        setup1[235]=0;
                        setup1[236]=0;
                        setup1[237]=0;
                        setup1[238]=0;
                        setup1[239]=0;
     //offset22[8:0]--setup1[248:240] Offset adjust for channel 22
                        setup1[240]=0;
                        setup1[241]=0;
                        setup1[242]=0;
                        setup1[243]=0;
                        setup1[244]=0;
                        setup1[245]=0;
                        setup1[246]=0;
                        setup1[247]=0;
                        setup1[248]=0;
     //offset21[8:0]--setup1[257:249] Offset adjust for channel 21
                        setup1[249]=0;
                        setup1[250]=0;
                        setup1[251]=0;
                        setup1[252]=0;
                        setup1[253]=0;
                        setup1[254]=0;
                        setup1[255]=0;
                        setup1[256]=0;
                        setup1[257]=0;
     //offset20[8:0]--setup1[266:258] Offset adjust for channel 20
                        setup1[258]=0;
                        setup1[259]=0;
                        setup1[260]=0;
                        setup1[261]=0;
                        setup1[262]=0;
                        setup1[263]=0;
                        setup1[264]=0;
                        setup1[265]=0;
                        setup1[266]=0;
     //offset19[8:0]--setup1[275:267] Offset adjust for channel 19
                        setup1[267]=0;
                        setup1[268]=0;
                        setup1[269]=0;
                        setup1[270]=0;
                        setup1[271]=0;
                        setup1[272]=0;
                        setup1[273]=0;
                        setup1[274]=0;
                        setup1[275]=0;
     //offset18[8:0]--setup1[284:276] Offset adjust for channel 18
                        setup1[276]=0;
                        setup1[277]=0;
                        setup1[278]=0;
                        setup1[279]=0;
                        setup1[280]=0;
                        setup1[281]=0;
                        setup1[282]=0;
                        setup1[283]=0;
                        setup1[284]=0;
     //offset17[8:0]--setup1[293:285] Offset adjust for channel 17
                        setup1[285]=0;
                        setup1[286]=0;
                        setup1[287]=0;
                        setup1[288]=0;
                        setup1[289]=0;
                        setup1[290]=0;
                        setup1[291]=0;
                        setup1[292]=0;
                        setup1[293]=0;
     //offset16[8:0]--setup1[302:294] Offset adjust for channel 16
                        setup1[294]=0;
                        setup1[295]=0;
                        setup1[296]=0;
                        setup1[297]=0;
                        setup1[298]=0;
                        setup1[299]=0;
                        setup1[300]=0;
                        setup1[301]=0;
                        setup1[302]=0;
     //offset15[8:0]--setup1[311:303] Offset adjust for channel 15
                        setup1[303]=0;
                        setup1[304]=0;
                        setup1[305]=0;
                        setup1[306]=0;
                        setup1[307]=0;
                        setup1[308]=0;
                        setup1[309]=0;
                        setup1[310]=0;
                        setup1[311]=0;
     //offset14[8:0]--setup1[320:312] Offset adjust for channel 14
                        setup1[312]=0;
                        setup1[313]=0;
                        setup1[314]=0;
                        setup1[315]=0;
                        setup1[316]=0;
                        setup1[317]=0;
                        setup1[318]=0;
                        setup1[319]=0;
                        setup1[320]=0;
     //offset13[8:0]--setup1[329:321] Offset adjust for channel 13
                        setup1[321]=0;
                        setup1[322]=0;
                        setup1[323]=0;
                        setup1[324]=0;
                        setup1[325]=0;
                        setup1[326]=0;
                        setup1[327]=0;
                        setup1[328]=0;
                        setup1[329]=0;
     //offset12[8:0]--setup1[338:330] Offset adjust for channel 12
                        setup1[330]=0;
                        setup1[331]=0;
                        setup1[332]=0;
                        setup1[333]=0;
                        setup1[334]=0;
                        setup1[335]=0;
                        setup1[336]=0;
                        setup1[337]=0;
                        setup1[338]=0;
     //offset11[8:0]--setup1[347:339] Offset adjust for channel 11
                        setup1[339]=0;
                        setup1[340]=0;
                        setup1[341]=0;
                        setup1[342]=0;
                        setup1[343]=0;
                        setup1[344]=0;
                        setup1[345]=0;
                        setup1[346]=0;
                        setup1[347]=0;
     //offset10[8:0]--setup1[356:348] Offset adjust for channel 10
                        setup1[348]=0;
                        setup1[349]=0;
                        setup1[350]=0;
                        setup1[351]=0;
                        setup1[352]=0;
                        setup1[353]=0;
                        setup1[354]=0;
                        setup1[355]=0;
                        setup1[356]=0;
     //offset9[8:0]--setup1[365:357] Offset adjust for channel 9
                        setup1[357]=0;
                        setup1[358]=0;
                        setup1[359]=0;
                        setup1[360]=0;
                        setup1[361]=0;
                        setup1[362]=0;
                        setup1[363]=0;
                        setup1[364]=0;
                        setup1[365]=0;
     //offset8[8:0]--setup1[374:366] Offset adjust for channel 8
                        setup1[366]=0;
                        setup1[367]=0;
                        setup1[368]=0;
                        setup1[369]=0;
                        setup1[370]=0;
                        setup1[371]=0;
                        setup1[372]=0;
                        setup1[373]=0;
                        setup1[374]=0;
     //offset7[8:0]--setup1[383:375] Offset adjust for channel 7
                        setup1[375]=0;
                        setup1[376]=0;
                        setup1[377]=0;
                        setup1[378]=0;
                        setup1[379]=0;
                        setup1[380]=0;
                        setup1[381]=0;
                        setup1[382]=0;
                        setup1[383]=0;
     //offset6[8:0]--setup1[392:384] Offset adjust for channel 6
                        setup1[384]=0;
                        setup1[385]=0;
                        setup1[386]=0;
                        setup1[387]=0;
                        setup1[388]=0;
                        setup1[389]=0;
                        setup1[390]=0;
                        setup1[391]=0;
                        setup1[392]=0;
     //offset5[8:0]--setup1[401:393] Offset adjust for channel 5
                        setup1[393]=0;
                        setup1[394]=0;
                        setup1[395]=0;
                        setup1[396]=0;
                        setup1[397]=0;
                        setup1[398]=0;
                        setup1[399]=0;
                        setup1[400]=0;
                        setup1[401]=0;
     //offset4[8:0]--setup1[410:402] Offset adjust for channel 4
                        setup1[402]=0;
                        setup1[403]=0;
                        setup1[404]=0;
                        setup1[405]=0;
                        setup1[406]=0;
                        setup1[407]=0;
                        setup1[408]=0;
                        setup1[409]=0;
                        setup1[410]=0;
     //offset3[8:0]--setup1[419:411] Offset adjust for channel 3
                        setup1[411]=0;
                        setup1[412]=0;
                        setup1[413]=0;
                        setup1[414]=0;
                        setup1[415]=0;
                        setup1[416]=0;
                        setup1[417]=0;
                        setup1[418]=0;
                        setup1[419]=0;
     //offset2[8:0]--setup1[428:420] Offset adjust for channel 2
                        setup1[420]=0;
                        setup1[421]=0;
                        setup1[422]=0;
                        setup1[423]=0;
                        setup1[424]=0;
                        setup1[425]=0;
                        setup1[426]=0;
                        setup1[427]=0;
                        setup1[428]=0;
     //offset1[8:0]--setup1[437:429] Offset adjust for channel 1
                        setup1[429]=0;
                        setup1[430]=0;
                        setup1[431]=0;
                        setup1[432]=0;
                        setup1[433]=0;
                        setup1[434]=0;
                        setup1[435]=0;
                        setup1[436]=0;
                        setup1[437]=0;
     //offset0[8:0]--setup1[446:438] Offset adjust for channel 0
                        setup1[438]=0;
                        setup1[439]=0;
                        setup1[440]=0;
                        setup1[441]=0;
                        setup1[442]=0;
                        setup1[443]=0;
                        setup1[444]=0;
                        setup1[445]=0;
                        setup1[446]=0;
     //coarse_count_offset[11:0]--setup1[458:447] Offset for coarse time counter
                        setup1[447]=0;
                        setup1[448]=0;
                        setup1[449]=0;
                        setup1[450]=0;
                        setup1[451]=0;
                        setup1[452]=0;
                        setup1[453]=0;
                        setup1[454]=0;
                        setup1[455]=0;
                        setup1[456]=0;
                        setup1[457]=0;
                        setup1[458]=0;
     //dll_tap_adjust[95:0]--setup1[554:459] Adjustment of DLL taps(0,1,2,3,4,5,6,7)
                        //tap0
                        setup1[459]=0;
                        setup1[460]=0;
                        setup1[461]=0;
                        //tap1
                        setup1[462]=0;
                        setup1[463]=0;
                        setup1[464]=0;
                        //tap2
                        setup1[465]=0;
                        setup1[466]=0;
                        setup1[467]=0;
                        //tap3
                        setup1[468]=0;
                        setup1[469]=0;
                        setup1[470]=0;
                        //tap4
                        setup1[471]=1;
                        setup1[472]=0;
                        setup1[473]=0;
                        //tap5
                        setup1[474]=1;
                        setup1[475]=0;
                        setup1[476]=0;
                        //tap6
                        setup1[477]=1;
                        setup1[478]=0;
                        setup1[479]=0;
                        //tap7
                        setup1[480]=1;
                        setup1[481]=0;
                        setup1[482]=0;
                        //tap8
                        setup1[483]=0;
                        setup1[484]=1;
                        setup1[485]=0;
                        //tap9
                        setup1[486]=0;
                        setup1[487]=1;
                        setup1[488]=0;
                        //tap10
                        setup1[489]=0;
                        setup1[490]=1;
                        setup1[491]=0;
                        //tap11
                        setup1[492]=0;
                        setup1[493]=1;
                        setup1[494]=0;
                        //tap12
                        setup1[495]=1;
                        setup1[496]=1;
                        setup1[497]=0;
                        //tap13
                        setup1[498]=1;
                        setup1[499]=1;
                        setup1[500]=0;
                        //tap14
                        setup1[501]=1;
                        setup1[502]=1;
                        setup1[503]=0;
                        //tap15
                        setup1[504]=1;
                        setup1[505]=1;
                        setup1[506]=0;
                        //tap16
                        setup1[507]=0;
                        setup1[508]=0;
                        setup1[509]=1;
                        //tap17
                        setup1[510]=0;
                        setup1[511]=0;
                        setup1[512]=1;
                        //tap18
                        setup1[513]=0;
                        setup1[514]=0;
                        setup1[515]=1;
                        //tap19
                        setup1[516]=0;
                        setup1[517]=0;
                        setup1[518]=1;
                        //tap20
                        setup1[519]=1;
                        setup1[520]=0;
                        setup1[521]=1;
                        //tap21
                        setup1[522]=1;
                        setup1[523]=0;
                        setup1[524]=1;
                        //tap22
                        setup1[525]=1;
                        setup1[526]=0;
                        setup1[527]=1;
                        //tap23
                        setup1[528]=1;
                        setup1[529]=0;
                        setup1[530]=1;
                        //tap24
                        setup1[531]=0;
                        setup1[532]=1;
                        setup1[533]=1;
                        //tap25
                        setup1[534]=0;
                        setup1[535]=1;
                        setup1[536]=1;
                        //tap26
                        setup1[537]=0;
                        setup1[538]=1;
                        setup1[539]=1;
                        //tap27
                        setup1[540]=0;
                        setup1[541]=1;
                        setup1[542]=1;
                        //tap28
                        setup1[543]=1;
                        setup1[544]=1;
                        setup1[545]=1;
                        //tap29
                        setup1[546]=1;
                        setup1[547]=1;
                        setup1[548]=1;
                        //tap30
                        setup1[549]=1;
                        setup1[550]=1;
                        setup1[551]=1;
                        //tap31
                        setup1[552]=1;
                        setup1[553]=1;
                        setup1[554]=1;
     //rc_adjust[11:0]--setup1[566:555] Adjustment of R-C delay line(only needed in very high resolution mode)
                        setup1[555]=1;
                        setup1[556]=1;
                        setup1[557]=1;
                        setup1[558]=1;
                        setup1[559]=1;
                        setup1[560]=1;
                        setup1[561]=0;
                        setup1[562]=0;
                        setup1[563]=0;
                        setup1[564]=0;
                        setup1[565]=0;
                        setup1[566]=0;
     //not used--setup1[569:567]
                        setup1[567]=0;
                        setup1[568]=0;
                        setup1[569]=0;
     //low_power_mode--setup1[570] Low power mode of channel buffers(1)
                        setup1[570]=1;
     //width_select[3:0]--setup1[574:571] Pulse width resolution when paired measurements
                        setup1[571]=1;
                        setup1[572]=0;
                        setup1[573]=0;
                        setup1[574]=0;
     //vernier_offset[4:0]--setup1[579:575] Offset in vernier dedoding(fixed 0000)
                        setup1[575]=0;
                        setup1[576]=0;
                        setup1[577]=0;
                        setup1[578]=0;
			setup1[579]=0;
     //dll_control2[3:0]--setup1[583:580] Control of dll(0001)
                        setup1[580]=1;
                        setup1[581]=0;
                        setup1[582]=0;
                        setup1[583]=0;
     //dead_time[1:0]--setup1[585:584] Channel dead time between hits(0)
                        setup1[584]=1;
                        setup1[585]=1;//modified by leigk&jiangxs 20080117 for test,old value=0;
     //test_invert--setup1[586] Automatic inversion of test pattern(0)(only used during production testing)
                        setup1[586]=0;
     //test_mode--setup1[587] Test mode where hit data taken from coretest(0)(only used during production testing)
                        setup1[587]=0;
     //enable_trailing--setup1[588] Enable of trailing edges
                        setup1[588]=0;
     //enable_leading--setup1[589] Enable of leading edges
                        setup1[589]=1;
     //mode_rc_compression--setup1[590] Perform RC interpolation on-chip(only valid in very high resolution mode)
                        setup1[590]=0;
     //mode_rc--setup1[591] Enable of R-C delay line mode(very high res.mode)
                        setup1[591]=0;
     //dll_mode[1:0]--setup1[593:592] Selection of DLL speed mode
                        setup1[592]=1;
                        setup1[593]=0;
     //pll_control2[7:0]--setup1[601:594] Control of PLL
                        setup1[594]=0;
                        setup1[595]=0;
                        setup1[596]=1;
                        setup1[597]=0;
                        setup1[598]=0;
                        setup1[599]=0;
                        setup1[600]=0;
                        setup1[601]=0;
     //serial_clock_delay[3:0]--setup1[605:602] Delay of internal serial clock(0)
                        setup1[602]=0;
                        setup1[603]=0;
                        setup1[604]=0;
                        setup1[605]=0;
     //io_clock_delay[3:0]--setup1[609:606] Delay of internal io clock(0)
                        setup1[606]=0;
                        setup1[607]=0;
                        setup1[608]=0;
                        setup1[609]=0;
     //core_clock_delay[3:0]--setup1[613:610] Delay of internal core clock(0)
                        setup1[610]=0;
                        setup1[611]=0;
                        setup1[612]=0;
                        setup1[613]=0;
     //dll_clock_delay[3:0]--setup1[617:614] Delay of internal dll clock(0)
                        setup1[614]=0;
                        setup1[615]=0;
                        setup1[616]=0;
                        setup1[617]=0;
     //serial_clock_soure[1:0]--setup1[619:618] Selection of source for serial clock(0)
                        setup1[618]=0;
                        setup1[619]=0;
     //io_clock_soure[1:0]--setup1[621:620] Selection of clock source for IO signals(0)
                        setup1[620]=0;
                        setup1[621]=0;
     //core_clock_source[1:0]--setup1[623:622] Selection of clock source for internal logic(0)
                        setup1[622]=0;
                        setup1[623]=0;
     //dll_clock_source[2:0]--setup1[626:624] Selection of clock source for DLL
                        setup1[624]=0;
                        setup1[625]=1;
                        setup1[626]=0;
     //roll_over[11:0]--setup1[638:627] Counter roll over value(FFF hex) defines max count value from where counters will go to zero
                        setup1[627]=1;
                        setup1[628]=1;
                        setup1[629]=1;
                        setup1[630]=1;
                        setup1[631]=1;
                        setup1[632]=1;
                        setup1[633]=1;
                        setup1[634]=1;
                        setup1[635]=1;
                        setup1[636]=1;
                        setup1[637]=1;
                        setup1[638]=1;
     //enable_matching--setup1[639] Enable of trigger matching
                        setup1[639]=1;
     //enable_pair--setup1[640] Enable pairing of leading and trailing edges
                        setup1[640]=0;
     //enable_ttl_serial--setup1[641] Enable LVTTL input on: serial_in,serial_bypass_in,token_in,token_bypass_in(otherwise use LVDS input levels)
                        setup1[641]=1;
     //enable_ttl_control--setup1[642] Enable LVTTL input on: trigger,bunch_reset,event_reset_encoded_control(otherwise uses LVDS input levels)
                        setup1[642]=1;
     //enable_ttl_reset--setup1[643] Enable LVTTL input on: reset(otherwise uses LVDS input levels)
                        setup1[643]=1;
     //enable_ttl_clock--setup1[644] Enable LVTTL input on: clk,aux_clock(otherwise uses LVDS input levels)
                        setup1[644]=0;
     //enable_ttl_hit--setup1[645] Enable LVTTL input on: hit[31:0](otherwise uses LVDS input levels)
                        setup1[645]=0;
     //parity--setup1[646] Parity of setup data(even parity)

	x=0;
        for(i=0;i<646;i++)
	{
		x=x^setup1[i];
	}
	setup1[646]=x;

	for(i=0;i<647;i++) input_DR[i]=setup1[i];

	Check=JTAGSetData(address,input_IR,output_IR,length_IR,input_DR,output_DR,length_DR);
	if(Check!=0) return Check;
	Check=JTAGSetData(address,input_IR,output_IR,length_IR,input_DR,output_DR,length_DR);
	if(Check!=0) return Check;

	for(i=0;i<647;i++)
	{
		if(input_DR[i]!=output_DR[i])
		{
			#ifdef MQT_PRINT
			printf("Setup HPTDC check error\n");
			for(i=0;i<647;i++)
			{
				printf("%1d",output_DR[i]);
			}
			printf("\n");
			printf("\n");
			#endif
			return -4;
		}
	}
	
	#ifdef MQT_PRINT
	printf("Setup HPTDC\n");
	for(i=0;i<647;i++)
	{
		printf("%1d",output_DR[i]);
	}
	printf("\n");
	printf("\n");
	#endif

	return 0;
}

int HPTDCinitialization(UINT32 address, UINT32 enable)
{
	int Check;

	Check=JTAGResetPLLandDLL(address,enable);
	if(Check!=0) return Check;

	Check=JTAGPLLLocking(address,enable);
	if(Check!=0) return Check;

	Check=JTAGDLLLocking(address,enable);
	if(Check!=0) return Check;

	Check=JTAGGlobalReset(address,enable);
	if(Check!=0) return Check;

	Check=JTAGEnabaleOutput(address,enable);
	if(Check!=0) return Check;

	return 0;
}

int JTAGResetPLLandDLL(UINT32 address, UINT32 enable)
{
	int i;
	UINT32 x;
	UINT32 input_DR[40],output_DR[40];
	UINT32 length_DR=40;
	UINT32 input_IR[5],output_IR[5];
	UINT32 length_IR=5;
	int Check;	

	input_IR[4]=0;
	input_IR[3]=1;
	input_IR[2]=0;
	input_IR[1]=0;
	input_IR[0]=1;

	for(i=0;i<40;i++)
	{
		output_DR[i]=0;
	}

	UINT32 control1[40];
	
    //enable_pattern[3:0]--control1[3:0] See specification p.25
                        control1[0]=0;
                        control1[1]=0;
                        control1[2]=1;
                        control1[3]=0;
    //Global_reset--control1[4] Global reset of TDC via JTAG(must be set to one and returned to zero)
                        control1[4]=0;
    //enable_channel[31:0]--control1[36:5] Enable/Disable of individual channels
                        control1[5]=enable & 0x1;
                        control1[6]=(enable & 0x2)>>1;
                        control1[7]=(enable & 0x4)>>2;
                        control1[8]=(enable & 0x8)>>3;
                        control1[9]=(enable & 0x10)>>4;
                        control1[10]=(enable & 0x20)>>5;
                        control1[11]=(enable & 0x40)>>6;
                        control1[12]=(enable & 0x80)>>7;
                        control1[13]=(enable & 0x100)>>8;
                        control1[14]=(enable & 0x200)>>9;
                        control1[15]=(enable & 0x400)>>10;
                        control1[16]=(enable & 0x800)>>11;
                        control1[17]=(enable & 0x1000)>>12;
                        control1[18]=(enable & 0x2000)>>13;
                        control1[19]=(enable & 0x4000)>>14;
                        control1[20]=(enable & 0x8000)>>15;
                        control1[21]=(enable & 0x10000)>>16;
                        control1[22]=(enable & 0x20000)>>17;
                        control1[23]=(enable & 0x40000)>>18;
                        control1[24]=(enable & 0x80000)>>19;
                        control1[25]=(enable & 0x100000)>>20;
                        control1[26]=(enable & 0x200000)>>21;
                        control1[27]=(enable & 0x400000)>>22;
                        control1[28]=(enable & 0x800000)>>23;
                        control1[29]=(enable & 0x1000000)>>24;
                        control1[30]=(enable & 0x2000000)>>25;
                        control1[31]=(enable & 0x4000000)>>26;
                        control1[32]=(enable & 0x8000000)>>27;
                        control1[33]=(enable & 0x10000000)>>28;
                        control1[34]=(enable & 0x20000000)>>29;
                        control1[35]=(enable & 0x40000000)>>30;
                        control1[36]=(enable & 0x80000000)>>31;
    //dll_reset--control1[37] Reset of DLL(must be set to one and returned to zero)
                        control1[37]=1;
    //pll_reset--control1[38] Reset of PLL(must be set to one and returned to zero)
                        control1[38]=1;
    //control_parity--control1[39] Parity of control data(even parity)
        x=0;
        for(i=0;i<39;i++)
        {
                x=x^control1[i];
        }
        control1[39]=x;
	
	for(i=0;i<40;i++)
	{
		input_DR[i]=control1[i];
	}
	
	Check=JTAGSetData(address,input_IR,output_IR,length_IR,input_DR,output_DR,length_DR);
	if(Check!=0) return Check;
	Check=JTAGSetData(address,input_IR,output_IR,length_IR,input_DR,output_DR,length_DR);
	if(Check!=0) return Check;
	
	for(i=0;i<40;i++)
	{
		if(input_DR[i]!=output_DR[i])
		{
			#ifdef MQT_PRINT
			printf("reset PLL and DLL check error\n");
			printf("IR: ");
			for(i=0;i<5;i++)
			{
				printf("%1d",output_IR[i]);
			}
			printf("\nDR: ");
			for(i=0;i<40;i++)
			{
				printf("%1d",output_DR[i]);
			}
			printf("\n");
			#endif
			return -5;
		}
	}

	#ifdef MQT_PRINT
	printf("reset PLL and DLL\n");
	printf("IR: ");
	for(i=0;i<5;i++)
	{
		printf("%1d",output_IR[i]);
	}
	printf("\ninput_DR: ");
	for(i=0;i<40;i++)
	{
		printf("%1d",input_DR[i]);
	}
	printf("\noutput_DR: ");
	for(i=0;i<40;i++)
	{
		printf("%1d",output_DR[i]);
	}
	printf("\n");
	#endif
	return 0;
}

int JTAGPLLLocking(UINT32 address, UINT32 enable)
{

	int i;
	UINT32 x;
	UINT32 input_DR[40],output_DR[40];
	UINT32 length_DR=40;
	UINT32 input_IR[5],output_IR[5];
	UINT32 length_IR=5;
	int Check;	
	
	input_IR[4]=0;
	input_IR[3]=1;
	input_IR[2]=0;
	input_IR[1]=0;
	input_IR[0]=1;

	for(i=0;i<40;i++)
	{
		output_DR[i]=0;
	}

	UINT32 control1[40];
	
    //enable_pattern[3:0]--control1[3:0] See specification p.25
                        control1[0]=0;
                        control1[1]=0;
                        control1[2]=1;
                        control1[3]=0;
    //Global_reset--control1[4] Global reset of TDC via JTAG(must be set to one and returned to zero)
                        control1[4]=0;
    //enable_channel[31:0]--control1[36:5] Enable/Disable of individual channels
                        control1[5]=enable & 0x1;
                        control1[6]=(enable & 0x2)>>1;
                        control1[7]=(enable & 0x4)>>2;
                        control1[8]=(enable & 0x8)>>3;
                        control1[9]=(enable & 0x10)>>4;
                        control1[10]=(enable & 0x20)>>5;
                        control1[11]=(enable & 0x40)>>6;
                        control1[12]=(enable & 0x80)>>7;
                        control1[13]=(enable & 0x100)>>8;
                        control1[14]=(enable & 0x200)>>9;
                        control1[15]=(enable & 0x400)>>10;
                        control1[16]=(enable & 0x800)>>11;
                        control1[17]=(enable & 0x1000)>>12;
                        control1[18]=(enable & 0x2000)>>13;
                        control1[19]=(enable & 0x4000)>>14;
                        control1[20]=(enable & 0x8000)>>15;
                        control1[21]=(enable & 0x10000)>>16;
                        control1[22]=(enable & 0x20000)>>17;
                        control1[23]=(enable & 0x40000)>>18;
                        control1[24]=(enable & 0x80000)>>19;
                        control1[25]=(enable & 0x100000)>>20;
                        control1[26]=(enable & 0x200000)>>21;
                        control1[27]=(enable & 0x400000)>>22;
                        control1[28]=(enable & 0x800000)>>23;
                        control1[29]=(enable & 0x1000000)>>24;
                        control1[30]=(enable & 0x2000000)>>25;
                        control1[31]=(enable & 0x4000000)>>26;
                        control1[32]=(enable & 0x8000000)>>27;
                        control1[33]=(enable & 0x10000000)>>28;
                        control1[34]=(enable & 0x20000000)>>29;
                        control1[35]=(enable & 0x40000000)>>30;
                        control1[36]=(enable & 0x80000000)>>31;
    //dll_reset--control1[37] Reset of DLL(must be set to one and returned to zero)
                        control1[37]=1;
    //pll_reset--control1[38] Reset of PLL(must be set to one and returned to zero)
                        control1[38]=0;
    //control_parity--control1[39] Parity of control data(even parity)
        x=0;
        for(i=0;i<39;i++)
        {
                x=x^control1[i];
        }
        control1[39]=x;
	
	for(i=0;i<40;i++)
	{
		input_DR[i]=control1[i];
	}

	Check=JTAGSetData(address,input_IR,output_IR,length_IR,input_DR,output_DR,length_DR);
	if(Check!=0) return Check;
	Check=JTAGSetData(address,input_IR,output_IR,length_IR,input_DR,output_DR,length_DR);
	if(Check!=0) return Check;

	for(i=0;i<40;i++)
	{
		if(input_DR[i]!=output_DR[i])
		{
			#ifdef MQT_PRINT
			printf("PLL Locking check error\n");
			printf("IR: ");
			for(i=0;i<5;i++)
			{
				printf("%1d",output_IR[i]);
			}
			printf("\nDR: ");
			for(i=0;i<40;i++)
			{
				printf("%1d",output_DR[i]);
			}
			printf("\n");
			#endif
			return -6;
		}
	}

	#ifdef MQT_PRINT
	printf("PLL Locking\n");
	printf("IR: ");
	for(i=0;i<5;i++)
	{
		printf("%1d",output_IR[i]);
	}
	printf("\ninput_DR: ");
	for(i=0;i<40;i++)
	{
		printf("%1d",input_DR[i]);
	}
	printf("\noutput_DR: ");
	for(i=0;i<40;i++)
	{
		printf("%1d",output_DR[i]);
	}
	printf("\n");
	#endif
	return 0;
}

int JTAGDLLLocking(UINT32 address, UINT32 enable)
{
	int i;
	UINT32 x;
	UINT32 input_DR[40],output_DR[40];
	UINT32 length_DR=40;
	UINT32 input_IR[5],output_IR[5];
	UINT32 length_IR=5;
	int Check;	
	
	input_IR[4]=0;
	input_IR[3]=1;
	input_IR[2]=0;
	input_IR[1]=0;
	input_IR[0]=1;

	for(i=0;i<40;i++)
	{
		output_DR[i]=0;
	}

	UINT32 control1[40];
	
    //enable_pattern[3:0]--control1[3:0] See specification p.25
                        control1[0]=0;
                        control1[1]=0;
                        control1[2]=1;
                        control1[3]=0;
    //Global_reset--control1[4] Global reset of TDC via JTAG(must be set to one and returned to zero)
                        control1[4]=0;
    //enable_channel[31:0]--control1[36:5] Enable/Disable of individual channels
                        control1[5]=enable & 0x1;
                        control1[6]=(enable & 0x2)>>1;
                        control1[7]=(enable & 0x4)>>2;
                        control1[8]=(enable & 0x8)>>3;
                        control1[9]=(enable & 0x10)>>4;
                        control1[10]=(enable & 0x20)>>5;
                        control1[11]=(enable & 0x40)>>6;
                        control1[12]=(enable & 0x80)>>7;
                        control1[13]=(enable & 0x100)>>8;
                        control1[14]=(enable & 0x200)>>9;
                        control1[15]=(enable & 0x400)>>10;
                        control1[16]=(enable & 0x800)>>11;
                        control1[17]=(enable & 0x1000)>>12;
                        control1[18]=(enable & 0x2000)>>13;
                        control1[19]=(enable & 0x4000)>>14;
                        control1[20]=(enable & 0x8000)>>15;
                        control1[21]=(enable & 0x10000)>>16;
                        control1[22]=(enable & 0x20000)>>17;
                        control1[23]=(enable & 0x40000)>>18;
                        control1[24]=(enable & 0x80000)>>19;
                        control1[25]=(enable & 0x100000)>>20;
                        control1[26]=(enable & 0x200000)>>21;
                        control1[27]=(enable & 0x400000)>>22;
                        control1[28]=(enable & 0x800000)>>23;
                        control1[29]=(enable & 0x1000000)>>24;
                        control1[30]=(enable & 0x2000000)>>25;
                        control1[31]=(enable & 0x4000000)>>26;
                        control1[32]=(enable & 0x8000000)>>27;
                        control1[33]=(enable & 0x10000000)>>28;
                        control1[34]=(enable & 0x20000000)>>29;
                        control1[35]=(enable & 0x40000000)>>30;
                        control1[36]=(enable & 0x80000000)>>31;
    //dll_reset--control1[37] Reset of DLL(must be set to one and returned to zero)
                        control1[37]=0;
    //pll_reset--control1[38] Reset of PLL(must be set to one and returned to zero)
                        control1[38]=0;
    //control_parity--control1[39] Parity of control data(even parity)
        x=0;
        for(i=0;i<39;i++)
        {
                x=x^control1[i];
        }
        control1[39]=x;
	
	for(i=0;i<40;i++)
	{
		input_DR[i]=control1[i];
	}
	
	Check=JTAGSetData(address,input_IR,output_IR,length_IR,input_DR,output_DR,length_DR);
	if(Check!=0) return Check;
	Check=JTAGSetData(address,input_IR,output_IR,length_IR,input_DR,output_DR,length_DR);
	if(Check!=0) return Check;

	for(i=0;i<40;i++)
	{
		if(input_DR[i]!=output_DR[i])
		{
			#ifdef MQT_PRINT
			printf("DLL Locking check error\n");
			printf("IR: ");
			for(i=0;i<5;i++)
			{
				printf("%1d",output_IR[i]);
			}
			printf("\nDR: ");
			for(i=0;i<40;i++)
			{
				printf("%1d",output_DR[i]);
			}
			printf("\n");
			#endif
			return -7;
		}
	}	

	#ifdef MQT_PRINT
	printf("DLL Locking\n");
	printf("IR: ");
	for(i=0;i<5;i++)
	{
		printf("%1d",output_IR[i]);
	}
	printf("\ninput_DR: ");
	for(i=0;i<40;i++)
	{
		printf("%1d",input_DR[i]);
	}
	printf("\noutput_DR: ");
	for(i=0;i<40;i++)
	{
		printf("%1d",output_DR[i]);
	}
	printf("\n");
	#endif
	return 0;
}

int JTAGGlobalReset(UINT32 address, UINT32 enable)
{
	int i;
	UINT32 x;
	UINT32 input_DR[40],output_DR[40];
	UINT32 length_DR=40;
	UINT32 input_IR[5],output_IR[5];
	UINT32 length_IR=5;
	int Check;	
	
	input_IR[4]=0;
	input_IR[3]=1;
	input_IR[2]=0;
	input_IR[1]=0;
	input_IR[0]=1;

	for(i=0;i<40;i++)
	{
		output_DR[i]=0;
	}

	UINT32 control1[40];
	
	//enable_pattern[3:0]--control1[3:0] See specification p.25
                        control1[0]=0;
                        control1[1]=0;
                        control1[2]=1;
                        control1[3]=0;
    //Global_reset--control1[4] Global reset of TDC via JTAG(must be set to one and returned to zero)
                        control1[4]=1;
    //enable_channel[31:0]--control1[36:5] Enable/Disable of individual channels
                        control1[5]=enable & 0x1;
                        control1[6]=(enable & 0x2)>>1;
                        control1[7]=(enable & 0x4)>>2;
                        control1[8]=(enable & 0x8)>>3;
                        control1[9]=(enable & 0x10)>>4;
                        control1[10]=(enable & 0x20)>>5;
                        control1[11]=(enable & 0x40)>>6;
                        control1[12]=(enable & 0x80)>>7;
                        control1[13]=(enable & 0x100)>>8;
                        control1[14]=(enable & 0x200)>>9;
                        control1[15]=(enable & 0x400)>>10;
                        control1[16]=(enable & 0x800)>>11;
                        control1[17]=(enable & 0x1000)>>12;
                        control1[18]=(enable & 0x2000)>>13;
                        control1[19]=(enable & 0x4000)>>14;
                        control1[20]=(enable & 0x8000)>>15;
                        control1[21]=(enable & 0x10000)>>16;
                        control1[22]=(enable & 0x20000)>>17;
                        control1[23]=(enable & 0x40000)>>18;
                        control1[24]=(enable & 0x80000)>>19;
                        control1[25]=(enable & 0x100000)>>20;
                        control1[26]=(enable & 0x200000)>>21;
                        control1[27]=(enable & 0x400000)>>22;
                        control1[28]=(enable & 0x800000)>>23;
                        control1[29]=(enable & 0x1000000)>>24;
                        control1[30]=(enable & 0x2000000)>>25;
                        control1[31]=(enable & 0x4000000)>>26;
                        control1[32]=(enable & 0x8000000)>>27;
                        control1[33]=(enable & 0x10000000)>>28;
                        control1[34]=(enable & 0x20000000)>>29;
                        control1[35]=(enable & 0x40000000)>>30;
                        control1[36]=(enable & 0x80000000)>>31;
    //dll_reset--control1[37] Reset of DLL(must be set to one and returned to zero)
                        control1[37]=0;
    //pll_reset--control1[38] Reset of PLL(must be set to one and returned to zero)
                        control1[38]=0;
    //control_parity--control1[39] Parity of control data(even parity)
        x=0;
        for(i=0;i<39;i++)
        {
                x=x^control1[i];
        }
        control1[39]=x;
	
	for(i=0;i<40;i++)
	{
		input_DR[i]=control1[i];
	}
	
	Check=JTAGSetData(address,input_IR,output_IR,length_IR,input_DR,output_DR,length_DR);
	if(Check!=0) return Check;
	Check=JTAGSetData(address,input_IR,output_IR,length_IR,input_DR,output_DR,length_DR);
	if(Check!=0) return Check;

	for(i=0;i<40;i++)
	{
		if(input_DR[i]!=output_DR[i])
		{
			#ifdef MQT_PRINT
			printf("JTAG Global Reset check error\n");
			printf("IR: ");
			for(i=0;i<5;i++)
			{
				printf("%1d",output_IR[i]);
			}
			printf("\nDR: ");
			for(i=0;i<40;i++)
			{
				printf("%1d",output_DR[i]);
			}
			printf("\n");
			#endif
			return -8;
		}
	}	

	#ifdef MQT_PRINT
	printf("JTAG Global Reset1\n");
	printf("IR: ");
	for(i=0;i<5;i++)
	{
		printf("%1d",output_IR[i]);
	}
	printf("\ninput_DR: ");
	for(i=0;i<40;i++)
	{
		printf("%1d",input_DR[i]);
	}
	printf("\noutput_DR: ");
	for(i=0;i<40;i++)
	{
		printf("%1d",output_DR[i]);
	}
	printf("\n");
	#endif

    //Global_reset--control1[4] Global reset of TDC via JTAG(must be set to one and returned to zero)
                        control1[4]=0;
	x=0;
	for(i=0;i<39;i++)
	{
		x=x^control1[i];
	}
	control1[39]=x;

	for(i=0;i<40;i++)
	{
		input_DR[i]=control1[i];
	}
	
	Check=JTAGSetData(address,input_IR,output_IR,length_IR,input_DR,output_DR,length_DR);
	if(Check!=0) return Check;
	Check=JTAGSetData(address,input_IR,output_IR,length_IR,input_DR,output_DR,length_DR);
	if(Check!=0) return Check;

	for(i=0;i<40;i++)
	{
		if(input_DR[i]!=output_DR[i])
		{
			#ifdef MQT_PRINT
			printf("JTAG Global Reset check error\n");
			printf("IR: ");
			for(i=0;i<5;i++)
			{
				printf("%1d",output_IR[i]);
			}
			printf("\nDR: ");
			for(i=0;i<40;i++)
			{
				printf("%1d",output_DR[i]);
			}
			printf("\n");
			#endif
			return -10;
		}
	}	

	#ifdef MQT_PRINT
	printf("JTAG Global Reset0\n");
	printf("IR: ");
	for(i=0;i<5;i++)
	{
		printf("%1d",output_IR[i]);
	}
	printf("\ninput_DR: ");
	for(i=0;i<40;i++)
	{
		printf("%1d",input_DR[i]);
	}
	printf("\noutput_DR: ");
	for(i=0;i<40;i++)
	{
		printf("%1d",output_DR[i]);
	}
	printf("\n");
	#endif
	return 0;
}

int JTAGEnabaleOutput(UINT32 address, UINT32 enable)
{
	int i;
	UINT32 x;
	UINT32 input_DR[40],output_DR[40];
	UINT32 length_DR=40;
	UINT32 input_IR[5],output_IR[5];
	UINT32 length_IR=5;
	int Check;	
	
	input_IR[4]=0;
	input_IR[3]=1;
	input_IR[2]=0;
	input_IR[1]=0;
	input_IR[0]=1;

	for(i=0;i<40;i++)
	{
		output_DR[i]=0;
	}

	UINT32 control1[40];
	
    //enable_pattern[3:0]--control1[3:0] See specification p.25
                        control1[0]=1;
                        control1[1]=0;
                        control1[2]=1;
                        control1[3]=0;
    //Global_reset--control1[4] Global reset of TDC via JTAG(must be set to one and returned to zero)
                        control1[4]=0;
    //enable_channel[31:0]--control1[36:5] Enable/Disable of individual channels
                        control1[5]=enable & 0x1;
                        control1[6]=(enable & 0x2)>>1;
                        control1[7]=(enable & 0x4)>>2;
                        control1[8]=(enable & 0x8)>>3;
                        control1[9]=(enable & 0x10)>>4;
                        control1[10]=(enable & 0x20)>>5;
                        control1[11]=(enable & 0x40)>>6;
                        control1[12]=(enable & 0x80)>>7;
                        control1[13]=(enable & 0x100)>>8;
                        control1[14]=(enable & 0x200)>>9;
                        control1[15]=(enable & 0x400)>>10;
                        control1[16]=(enable & 0x800)>>11;
                        control1[17]=(enable & 0x1000)>>12;
                        control1[18]=(enable & 0x2000)>>13;
                        control1[19]=(enable & 0x4000)>>14;
                        control1[20]=(enable & 0x8000)>>15;
                        control1[21]=(enable & 0x10000)>>16;
                        control1[22]=(enable & 0x20000)>>17;
                        control1[23]=(enable & 0x40000)>>18;
                        control1[24]=(enable & 0x80000)>>19;
                        control1[25]=(enable & 0x100000)>>20;
                        control1[26]=(enable & 0x200000)>>21;
                        control1[27]=(enable & 0x400000)>>22;
                        control1[28]=(enable & 0x800000)>>23;
                        control1[29]=(enable & 0x1000000)>>24;
                        control1[30]=(enable & 0x2000000)>>25;
                        control1[31]=(enable & 0x4000000)>>26;
                        control1[32]=(enable & 0x8000000)>>27;
                        control1[33]=(enable & 0x10000000)>>28;
                        control1[34]=(enable & 0x20000000)>>29;
                        control1[35]=(enable & 0x40000000)>>30;
                        control1[36]=(enable & 0x80000000)>>31;
    //dll_reset--control1[37] Reset of DLL(must be set to one and returned to zero)
                        control1[37]=0;
    //pll_reset--control1[38] Reset of PLL(must be set to one and returned to zero)
                        control1[38]=0;
	//control_parity--control1[39] Parity of control data(even parity)
        x=0;
        for(i=0;i<39;i++)
        {
                x=x^control1[i];
        }
        control1[39]=x;
	
	
	for(i=0;i<40;i++)
	{
		input_DR[i]=control1[i];
	}
	
	Check=JTAGSetData(address,input_IR,output_IR,length_IR,input_DR,output_DR,length_DR);
	if(Check!=0) return Check;
	Check=JTAGSetData(address,input_IR,output_IR,length_IR,input_DR,output_DR,length_DR);
	if(Check!=0) return Check;

	for(i=0;i<40;i++)
	{
		if(input_DR[i]!=output_DR[i])
		{
			#ifdef MQT_PRINT
			printf("Enabale Output check error\n");
			printf("IR: ");
			for(i=0;i<5;i++)
			{
				printf("%1d",output_IR[i]);
			}
			printf("\nDR: ");
			for(i=0;i<40;i++)
			{
				printf("%1d",output_DR[i]);
			}
			printf("\n");
			#endif
			return -11;
		}
	}

	#ifdef MQT_PRINT
	printf("Enabale Output\n");
	printf("IR: ");
	for(i=0;i<5;i++)
	{
		printf("%1d",output_IR[i]);
	}
	printf("\ninput_DR: ");
	for(i=0;i<40;i++)
	{
		printf("%1d",input_DR[i]);
	}
	printf("\noutput_DR: ");
	for(i=0;i<40;i++)
	{
		printf("%1d",output_DR[i]);
	}
	printf("\n");
	#endif
	return 0;
}

void JTAGReadStatus(UINT32 address)
{
	int i,x;
	UINT32 input_DR[62],output_DR[62];
	UINT32 length_DR=62;
	UINT32 input_IR[5],output_IR[5];
	UINT32 length_IR=5;
	int Check;	
	
	input_IR[4]=0;
	input_IR[3]=1;
	input_IR[2]=0;
	input_IR[1]=1;
	input_IR[0]=0;

	for(i=0;i<62;i++)
	{
		input_DR[i]=0;
		output_DR[i]=0;
	}

	JTAGSetData(address,input_IR,output_IR,length_IR,input_DR,output_DR,length_DR);

	printf("\nHPTDC1:\n");
	printf(" [10:0] error:	");
	i=0;
	printf("0-vernier error : %1d\n",output_DR[i+0]);
	printf("		1-coarse error : %1d\n",output_DR[i+1]);
	printf("		2-channel select error : %1d\n",output_DR[i+2]);
	printf("		3-l1 buffer parity error : %1d\n",output_DR[i+3]);
	printf("		4-trigger fifo parity error : %1d\n",output_DR[i+4]);
	printf("		5-trigger matching state error : %1d\n",output_DR[i+5]);
	printf("		6-readout fifo parity error : %1d\n",output_DR[i+6]);
	printf("		7-readout state error : %1d\n",output_DR[i+7]);
	printf("		8-setup parity error : %1d\n",output_DR[i+8]);
	printf("		9-control parity error : %1d\n",output_DR[i+9]);
	printf("		10-jtag instruction error : %1d\n",output_DR[i+10]);
	printf(" [11] have_token: %1d\n",output_DR[i+11]);
	printf(" [19:12] readout_fifo_occupancy: ");
	for(x=0;x<8;x++)
	{
		printf("%1d",output_DR[i+19-x]);
	}
	printf("\n");
	printf(" [20] readout_fifo_full: %1d\n",output_DR[i+20]);
	printf(" [21] readout_fifo_empty: %1d\n",output_DR[i+21]);
	printf(" [29:22] group3_l1_occupancy: ");
	for(x=0;x<8;x++)
	{
		printf("%1d",output_DR[i+29-x]);
	}
//	printf("\n");
	printf(" [37:30] group2_l1_occupancy: ");
	for(x=0;x<8;x++)
	{
		printf("%1d",output_DR[i+37-x]);
	}
	printf("\n");
	printf(" [45:38] group1_l1_occupancy: ");
	for(x=0;x<8;x++)
	{
		printf("%1d",output_DR[i+45-x]);
	}
//	printf("\n");
	printf(" [53:46] group0_l1_occupancy: ");
	for(x=0;x<8;x++)
	{
		printf("%1d",output_DR[i+53-x]);
	}
	printf("\n");
	printf(" [57:54] trigger_fifo_occupancy: ");
	for(x=0;x<4;x++)
	{
		printf("%1d",output_DR[i+57-x]);
	}
	printf("\n");
	printf(" [58] trigger_fifo_full: %1d\n",output_DR[i+58]);
	printf(" [59] trigger_fifo_empty: %1d\n",output_DR[i+59]);
	printf(" [60] dll_lock: %1d\n",output_DR[i+60]);
	printf(" [61] inverted setup[601]: %1d",output_DR[i+61]);
}

