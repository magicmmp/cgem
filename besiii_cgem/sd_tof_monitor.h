#ifndef __INCSDTOFMonitorh
#define __INCSDTOFMonitorh


int TOFConfigMoniBaseAddr();

int TOFCheckMoniSerialNum();

int TOFConfigMoniHPTDC();

int TOFConfigMonitorDAC() ;
int TOFMonitorLaserSetup();

int TOFConfigMoniInlTable();
//CBLT chain config here
int TOFConfigMoniWorkMode();

int TOFLaserDeviceOn();
int TOFLaserDeviceOff();
int TOFLaserPulseGen();
int TOFLaserPulseStop();

#endif

