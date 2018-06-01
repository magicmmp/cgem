#include "vxWorks.h"
#include "5100_test.h"
#include "sockLib.h"
#include "sysLib.h"

#include "fw_define.h"
#include "fw_roppc.h"
#include "fw_dcgvas.h"
#include "fw_filefunc.h"

#include "spyLib.h"
#include "usrLib.h"
#include "private/funcBindP.h"
#include "logLib.h"
#include "ioctl.h"		//for ioctl
#include "ioLib.h"		//for ioctl

void TimeToFilename(char *pcFilename,int nFileNum)
{
	UCHAR cYear, cMonth, cDate, cHour, cMinute, cSecond ;
	int nYear, nMonth, nDate, nHour, nMinute, nSecond ;
	ULONG nOffset ;

	nOffset = 0x7FFF ;
	cYear = sysNvRead(nOffset) ;
	nYear = 10 * ((0xf0 & cYear)  >> 4)+ (0x0f & cYear) ;

	nOffset = 0x7FFe ;
	cMonth = sysNvRead(nOffset) ;
	nMonth = 10 * ((0x10 & cMonth) >> 4)+ (0x0f & cMonth) ;

	nOffset = 0x7FFd ;
	cDate = sysNvRead(nOffset) ;
	nDate = 10 * ((0x30 & cDate) >> 4) + (0x0f & cDate) ;

	nOffset = 0x7FFb ;
	cHour = sysNvRead(nOffset) ;
	nHour = 10 *( (0x30 & cHour) >> 4)+ (0x0f & cHour) ;

	nOffset = 0x7FFa ;
	cMinute = sysNvRead(nOffset) ;
	nMinute = 10 * ((0x70 & cMinute) >> 4)+ (0x0f & cMinute) ;

	nOffset = 0x7FF9 ;
	cSecond = sysNvRead(nOffset) & 0x7f;
	nSecond = 10 * ((0x70 & cSecond) >> 4) + (0xf & cSecond) ;

	sprintf(pcFilename, "Log%02d%02d%02d_%02d%02d%02d_%d.txt\0", nYear, nMonth, nDate, nHour, nMinute, nSecond,nFileNum) ;
}

void dToFile(void * adrs, int nunits, int width, int nOutPutFileFd)
{
	int STDFd;

	if (nOutPutFileFd != -1)
	{
		STDFd=ioTaskStdGet(0, STD_OUT);
	  	ioTaskStdSet(0, STD_OUT,nOutPutFileFd);
		d(adrs, nunits, width) ;
	  	ioTaskStdSet(0, STD_OUT,STDFd);
	}
}

void RawDataToFile(int nFileSaveNum,int nBytesCblted, char * p_cRawData) 
{	
	char pcFileName[64] = { 0*64} ;
	int nFileFd ;

	TimeToFilename(pcFileName,nFileSaveNum);

	nFileFd = creat(pcFileName, O_RDWR ) ;
	if(ERROR == nFileFd)
	{
		printf("Create file failed.\n") ;
	}
	else
	{
		dToFile(p_cRawData, nBytesCblted/4 + 2, 4, nFileFd) ;
		taskDelay(2) ;
		if(ERROR == close(nFileFd))
		{
			printf("Close file failed.\n") ;
		}
		else
		{
			printf("The raw data were logged to %s.\n", pcFileName) ;
		}
	}
}

void dSpecialDataToFile(void * adrs, int nunits, int width)
{
	int nFileFd ;
	int STDFd;
	char cFileName[20];
	sprintf(cFileName,  "data%d\n",g_nCrateID) ;
	nFileFd = creat(cFileName, O_RDWR ) ;
	if(ERROR == nFileFd)
	{
		printf("Create file failed.\n") ;
	}
	else
	{
		STDFd=ioTaskStdGet(0, STD_OUT);
	  	ioTaskStdSet(0, STD_OUT,nFileFd);
		d(adrs, nunits, width) ;
	  	ioTaskStdSet(0, STD_OUT,STDFd);
		taskDelay(60);
		if(ERROR == close(nFileFd))
		{
			printf("Close file failed.\n") ;
		}
		else
		{
			printf("The raw data were logged to %s.\n", cFileName) ;
		}
	}
}

/***for read&write test***/

//A32D32
void mywriteA32D32(unsigned Addr,unsigned val)
{
	WriteToVme32(Addr, VA32, val);
}
void myreadA32D32(unsigned Addr)
{
	unsigned  abc;
	ReadFrmVme32(Addr,VA32,&abc);
	printf("addr 0x%x = 0x%x\n",Addr,abc);
}

//A24D8
void mywriteA24D8(unsigned Addr,unsigned val)
{
	WriteToVme8(Addr, VA24, val);
}
void myreadA24D8(unsigned Addr)
{
	unsigned  char abc;
	ReadFrmVme8(Addr,VA24,&abc);
	printf("addr 0x%x = 0x%x\n",Addr,abc);
}

//A24D16
void mywriteA24D16(unsigned Addr,unsigned val)
{
	WriteToVme16(Addr, VA24, val);
}
void myreadA24D16(unsigned Addr)
{
	unsigned short abc;
	ReadFrmVme16(Addr,VA24,&abc);
	printf("addr 0x%x = 0x%x\n",Addr,abc);
}

//A24D32
void mywriteA24D32(unsigned Addr,unsigned val)
{
	WriteToVme32(Addr, VA24, val);
}
void myreadA24D32(unsigned Addr)
{
	unsigned  abc;
	ReadFrmVme32(Addr,VA24,&abc);
	printf("addr 0x%x = 0x%x\n",Addr,abc);
}

void mywriteCSRD8(unsigned Addr,unsigned val)
{
	WriteToVme8(Addr, VCSR, val);
}
void myreadCSRD8(unsigned Addr)
{
	unsigned  char abc;
	ReadFrmVme8(Addr,VCSR,&abc);
	printf("addr 0x%x = 0x%x\n",Addr,abc);
}

