#include "vxWorks.h"
#include "stdlib.h"
#include "spyLib.h"
#include "usrLib.h"
#include "private/funcBindP.h"
#include "logLib.h"
#include "ioctl.h"		//for ioctl
#include "ioLib.h"		//for ioctl
#include "5100_test.h"
#include "sockLib.h"
#include "sysLib.h"

#include "fw_define.h"
#include "fw_staterrspt.h"
#include "fw_dcgvas.h"
#include "fw_roppc.h"









int			nErrIn;
StaErrUnit	ErrStack[10];


int StateErrParaCreate()
{
	if(NULL == (g_sidStaErrTrig= semBCreate(SEM_Q_FIFO, SEM_EMPTY) ) )
	{
		printf("semBCreate g_sidStaErrTrig failed\n") ;
		Push2ErrStack(PPCWARNING+FRM_FLG+ATOM_ERR+61,0,0,0,0);		/*error 61*/
		return -1 ;
	}

	if(NULL ==(g_sidStaErrSend= semMCreate(SEM_Q_FIFO) ) ) /*g_sidErrorMutex must initialized first ,so can send error infor*/
	{
		printf("semMCreate g_sidStaErrSend failed\n") ;
		Push2ErrStack(PPCWARNING+FRM_FLG+ATOM_ERR+62,0,0,0,0);		/*error 62*/
		return -1;
	}
	if(NULL == (g_widStatusTimer = wdCreate()))
	{
		printf("watchdog Create failed!\n") ;
		Push2ErrStack(PPCWARNING+FRM_FLG+ATOM_ERR+63,0,0,0,0);		/*error 63*/
		return -1 ;
	}
	return 0;
}
int StateErrParaFree()
{
	if(g_sidStaErrTrig!=NULL)
	{
		if(-1 == semDelete(g_sidStaErrTrig))
		{
			DEBUG_OUTPUT(("semDelete(g_sidStaErrTrig) failed.\n")) ;
			Push2ErrStack(PPCWARNING+FRM_FLG+ATOM_ERR+64,0,0,0,0);		/*error 64*/
			return -1;
		}
		g_sidStaErrTrig=NULL;
	}
	if(g_sidStaErrSend!=NULL)
	{
		if(-1==semDelete(g_sidStaErrSend) )
		{
			DEBUG_OUTPUT(("semDelete(g_sidStaErrSend) failed.\n")) ;
			Push2ErrStack(PPCWARNING+FRM_FLG+ATOM_ERR+65,0,0,0,0);		/*error 65*/
			return -1;
		}
		g_sidStaErrSend=NULL;
	}
	if(g_widStatusTimer!=NULL)
	{
		if(-1 == wdDelete(g_widStatusTimer))
		{
			DEBUG_OUTPUT(("wdDelete(g_widStatusTimer) failed.\n")) ;
			Push2ErrStack(PPCWARNING+FRM_FLG+ATOM_ERR+66,0,0,0,0);		/*error 66*/
			return -1;
		}
		g_widStatusTimer=NULL;
	}
	return 0;
}

void FWStatusTimerEvent()
{
	/*Give a semphore here*/
	semGive(g_sidStaErrTrig) ;	
	wdStart(g_widStatusTimer, ERR_TIMER_TICKS, (FUNCPTR)FWStatusTimerEvent, 0) ;
}

int	MyPrintf (const char *pFormatString, ...)
{
	/**get current time**/
/*	char *currentTime;
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

	sprintf(currentTime, "Time:%02d%02d%02d_%02d%02d%02d\0", nYear, nMonth, nDate, nHour, nMinute, nSecond) ;

*/


	if(g_bStatusSend)	//if can send status infor, call spy report ,else do't send
	{
		va_list arg_ptr;
		char *pS1, *pS2, *pS3, *pS4 ;
		int n5,n6,n7 ;

		va_start(arg_ptr,pFormatString);
		
		pS1=va_arg(arg_ptr,char *);
		pS2=va_arg(arg_ptr,char *);
		pS3=va_arg(arg_ptr,char *);
		pS4=va_arg(arg_ptr,char *);
		
		n5=va_arg(arg_ptr,int);
		n6=va_arg(arg_ptr,int);
		n7=va_arg(arg_ptr,int);
		/*n5() n7() see spyReport: n5 average idle; n7 realtime idle*/

		if(!strcmp("IDLE", pS1))
		{
			Push2ErrStack(PPCINFO+ISTATUS,n5,n7,g_nInterruptCounter,g_nEventsSent) ;
//			printf("IDLE: %3d%%, %3d%%, Net: %d, Interrupt: %d,Crate:%d, %s\n", n5,n7, g_nEventsSent,g_nInterruptCounter,g_nCrateID,currentTime) ;  
 //20100302 jixl			printf("IDLE: %3d%%, %3d%%, Net: %d, Interrupt: %d,Crate:%d\n", n5,n7, g_nEventsSent,g_nInterruptCounter,g_nCrateID) ; 

			g_nmsgQnum1 = msgQNumMsgs(g_msgQcblt);
			g_nmsgQnum2 = msgQNumMsgs(g_msgQnet);


			printf("IDLE: %3d%%, %3d%%, Net: %d, Interrupt: %d,Crate:%d,msgQnum1:%d,msgQnum2:%d,\n", n5,n7, g_nEventsSent,g_nInterruptCounter,g_nCrateID,g_nmsgQnum1,g_nmsgQnum2) ;  
		}
		va_end(arg_ptr);
	}

}


void FWReportStatus()
{	
	int nSta = 0 ;
	spyClkStart(5000) ;
	if (_func_spyReport == NULL)  /*whether have spy module*/
	{
		printf ("spy not configured into this system.\n");
	}
	nErrIn=0;
	while(1)
	{
		if(1!= g_nReportOn)
		{
			break ;
		}
		if(-1==semTake(g_sidStaErrTrig, ERR_TIMER_TICKS*3) )
		{
			printf("attention!take g_sidStaErrTrig timeout\n");
			Push2ErrStack(PPCWARNING+FRM_FLG+ATOM_ERR+67,0,0,0,0);		/*error 67*/
		}
		nSta++;
		if(nSta==30)		// send state infor per 30 seconds
		{	
			(* _func_spyReport) ((FUNCPTR) MyPrintf);  /*call spy function*/
			nSta=0;
		}
		StaErrUnitSend();
	}
	spyClkStop();
	printf("FWReportStatus exited.\n") ;
	nStatErrTaskId=0;
}

int StateErrReportInit()
{
	if(StateErrParaCreate()==-1)
	{
		printf("StateErrParaCreate failed.\n") ;
		return -1;
	}
	g_nReportOn=1;	//keep tReportStatus task
	g_bStatusSend=FALSE;
	if(ERROR ==(nStatErrTaskId= taskSpawn("tReportStatus", PRI_REPORTSTATUS, 0, TASK_STACK_SIZE, (FUNCPTR)FWReportStatus, 
							0, 0, 0, 0, 0, 0, 0, 0, 0, 0))  )
	{
		printf("taskSpawn(FWReportStatus) failed.\n") ;
		Push2ErrStack(PPCWARNING+FRM_FLG+ATOM_ERR+68,0,0,0,0);		/*error 68*/
		return -1;
	}
	else
	{
		printf("tReportStatus spawned.\n");
	}
	if(ERROR == wdStart(g_widStatusTimer, ERR_TIMER_TICKS, (FUNCPTR)FWStatusTimerEvent, 0) )
	{
		printf("wdStart failed.\n") ;
		Push2ErrStack(PPCWARNING+FRM_FLG+ATOM_ERR+69,0,0,0,0);		/*error 69*/
		return -1;
	}
	return 0;
}

void Push2ErrStack(UINT errcode,UINT ACGL, UINT IRGL, UINT InterptNum, UINT NetEvntNum)
{
	int ii=0;
	semTake(g_sidStaErrSend,WAIT_FOREVER);
	if(nErrIn<=10)
	{
		for(ii=0;ii<nErrIn;ii++)
		{
			if(ErrStack[ii].StaErrCode==errcode)break;
		}
		if(ii==nErrIn)
		{	
			ErrStack[nErrIn].InforLen=28;//sizeof(StaErrUnit)
			ErrStack[nErrIn].StaErrCode=errcode;
			ErrStack[nErrIn].CrateId=g_nCrateID;
			ErrStack[nErrIn].AvrgCpu_CuSta_SrcGeo_SrcL1=ACGL;
			ErrStack[nErrIn].InstCpu_ReCmd_HopGeo_HopL1=IRGL;
			ErrStack[nErrIn].IntrptNum=InterptNum;
			ErrStack[nErrIn].NetEvtNum=NetEvntNum;
			nErrIn++;
		}
	}
	semGive(g_sidStaErrSend);
}

void StaErrUnitSend()
{
	semTake(g_sidStaErrSend,WAIT_FOREVER);
	if(nErrIn>0)
	{
		int nInfoLen=nErrIn*sizeof(StaErrUnit);
		if(nInfoLen == send(g_sktStaInfo, (char *)ErrStack, nInfoLen, 0))
		{
			nErrIn=0;
		}
		else
		{
			printf("Sending stat err unit  failed.\n") ;
			Push2ErrStack(PPCWARNING+FRM_FLG+ATOM_ERR+70,0,0,0,0);		/*error 70*/
		}
		/**/
		nErrIn=0;
	}
	semGive(g_sidStaErrSend);
}
/*Push2ErrStack(0,0,0,0,0);*/

