#ifndef __INCFWFILEFUNCh
#define __INCFWFILEFUNCh

void TimeToFilename(char *pcFilename,int nFileNum) ;
void dToFile(void * adrs, int nunits, int width, int nOutPutFileFd) ;
void RawDataToFile(int nFileSaveNum,int nByteCblted, char * pRawData) ;
void dSpecialDataToFile(void * adrs, int nunits, int width);

#endif

