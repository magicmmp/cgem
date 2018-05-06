#ifndef __INCSDTOFFEEh
#define __INCSDTOFFEEh

int TOFConfigFeeBaseAddr();

int TOFCheckFeeSerialNum();

int TOFConfigFeeHPTDC();

int TOFConfigEachDAC(UINT32 InitAdrs,UINT32 HighThresh,UINT32 LowThresh,UINT32 ConstVoltage
								,UINT32 SelfTestSignAmp,UINT32 QTLowThresh,UINT32 QTHighThresh) ;
int TOFConfigFeeDAC();

int TOFConfigFeeInlTable();
//call CBLT chain config here
int TOFConfigFeeWorkMode();

#endif

