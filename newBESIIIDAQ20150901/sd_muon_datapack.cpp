
#include "vxWorks.h"

#include "fragheaders.h"
#include "fw_roppc.h"
#include "fw_runtasks.h"
#include "fw_dcgvas.h"
#include "fw_define.h"
#include "fw_staterrspt.h"
#include "fw_configinfo.h"

#include "sd_muon.h"
#include "sd_muon_datapack.h"
#include "sd_muon_hw.h"


int MUONDataPackCore(UINT *nDestination ,UINT *nSource,int *nBytes2BeSend)
{	
	register UINT *p_uMove=nSource;
	register UINT *p_uDesti=nDestination;
	register UINT *p_PointEveHead;

	register int 	nModuIndex;
	register int 	nTrgIndex;

	UINT		uDataLength = 0;
	UINT 		uDataTemp;
	int 			nPossibleLenth=0;
	
	int 			nRomoModuleNum;
	int			nRecordTrgNum;
	int 			nPerEventLen;
	int 			nTotalLenRetn;

	
	UINT *		p_uPerModulAddr[16];
	
	nRomoModuleNum=g_MUONDBInfo.enabl_romo_sum;
	uDataTemp=*nSource;
	uDataLength = *nSource;
	
	if((uDataTemp < (8 + 12*nRomoModuleNum)) || (uDataTemp> (g_nMaxCbltBytes+8))) 
	{
		printf("MUON DataPack: %d bytes were CBLTed.\n", uDataTemp - 8) ;	
		Push2ErrStack(PPCERROR+MUC_FLG+ATOM_ERR+49,0,0,0,0);		/*muc error 49*/	
		return (-1) ;
	}


	if((uDataTemp %4)!=0)                           			/*check wether can be divided by 8*/
	{
		printf("total length can not divid by 4:total length = %d\n",uDataTemp);
		Push2ErrStack(PPCERROR+MUC_FLG+ATOM_ERR+50,0,0,0,0);		/*muc error 50*/	
		return(-1); 
	}
	p_uMove=p_uMove+(uDataTemp>>2);	              /*jump to the data tail*/			

	if((*(p_uMove-1))==(*(p_uMove-2)))//double tail
	{
		
		*nSource=(*nSource)-4;
		p_uMove=p_uMove-1;
	}
	
	for(nModuIndex=(nRomoModuleNum-1);nModuIndex>=0;nModuIndex--)
	{
			uDataTemp=(*(p_uMove-1))&0xffff;
			//printf("uDataTemp=0x%08x\n",*(p_uMove-1));		
			if((uDataTemp%4)!=0)
			{
				Push2ErrStack(PPCERROR+MUC_FLG+ATOM_ERR+51,0,0,0,0);		/*muc error 51*/	
				printf("single length can not divid by 4:length = %d,nModuIndex = %d\n",uDataTemp,nModuIndex);
				return(-1);
			}
			if(((*(p_uMove-1))>>27)!=gMucEnablRomoGeo[nModuIndex])  /*tailer leigk*/
			{
				printf("module tail postion bit error : *(p_uMove-1)=0x%x,geo=%d\n",*(p_uMove-1),gMucEnablRomoGeo[nModuIndex]);

				Push2ErrStack(PPCERROR+MUC_FLG+GEO_ERR+52,((*(p_uMove-1))>>27),gMucEnablRomoGeo[nModuIndex],0,0);		/*muc error 52*/	
				return (-1);
			}
			else
			{
				if(((*(p_uMove-1))&0x00800000)!=0x00800000)
				{
					printf("*(p_uMove-1)=0x%08x,event tail flag error,nModuIndex = %d\n",*(p_uMove-1),nModuIndex);
					Push2ErrStack(PPCERROR+MUC_FLG+ATOM_ERR+53,0,0,0,0);		/*muc error 53*/	
					return -1;
				}
			}
			nPossibleLenth=nPossibleLenth+uDataTemp;  /*check the totle length*/
			if(nPossibleLenth>(*nSource-8))
			{
				Push2ErrStack(PPCERROR+MUC_FLG+ATOM_ERR+54,0,0,0,0);		/*muc error 54*/	
				printf("totle length can't fit,g_nTrgCounter=%d\n",g_nTrgCounter);
				return( -1);
			}
			
			p_uMove=p_uMove-(uDataTemp>>2);   /*careful for tof special other>>2*/
			if(p_uMove<nSource)/*may be something wrong with it*/
			{
				Push2ErrStack(PPCERROR+MUC_FLG+ATOM_ERR+55,0,0,0,0);		/*muc error 55*/	
				printf("address out of range,g_nTrgCounter=%d\n",g_nTrgCounter);
				return(-1);
			}	
			
			if(((*p_uMove)>>27)!=gMucEnablRomoGeo[nModuIndex]) /*header leigk*/
			{	
				Push2ErrStack(PPCERROR+MUC_FLG+GEO_ERR+56,((*(p_uMove))>>27),gMucEnablRomoGeo[nModuIndex],0,0);		/*muc error 56*/	
				printf("module header  postion error,(*p_uMove) = 0x%x,g_nTrgCounter=%d,nModuIndex = %d\n",(*p_uMove),g_nTrgCounter,nModuIndex);
				return(-1);  
			}
			else
			{
				if(((*(p_uMove))&0x00800000)!=0x00000000)
				{
					printf("module header flag error : *(p_uMove)=0x%08x,not event head , g_nTrgCounter=%d, nModuIndex = %d\n",*(p_uMove),g_nTrgCounter,nModuIndex);
					Push2ErrStack(PPCWARNING+MUC_FLG+ATOM_ERR+57,0,0,0,0);		/*muc error 57*/	
					return(-1);  				
				}
			}
			p_uPerModulAddr[nModuIndex]=p_uMove;
	}


	nRecordTrgNum=g_nTrgCounter;
	//printf("nRecordTrgNum=%d\n",nRecordTrgNum);
	nTotalLenRetn=0;

	for(nTrgIndex=0;nTrgIndex<g_nTrgPerInt;nTrgIndex++)
	{
		p_PointEveHead=p_uDesti;
		p_uDesti++;
			
		g_fwRODHead.level1Id=nRecordTrgNum;
		memcpy((char *)p_uDesti,&g_fwRODHead,sizeof(RODHeader));
		p_uDesti=p_uDesti+(sizeof(RODHeader)>>2);

		nPerEventLen=0;
		int itemp =0;
		for(nModuIndex=0; nModuIndex<nRomoModuleNum ;nModuIndex++)	
		{
			p_uMove=p_uPerModulAddr[nModuIndex];
			int nNowModuPos=gMucEnablRomoGeo[nModuIndex];
			
			if(((*p_uMove)&0xf8000000)!=0)
			{
				if(((*p_uMove)>>27)!=nNowModuPos)
				{
					Push2ErrStack(PPCERROR+MUC_FLG+GEO_ERR+58,((*(p_uMove))>>27),nNowModuPos,0,0);		/*muc error 58*/	
					printf("packing:module head position error:(*p_uMove) = 0x%x, nNowModuPos = %d\n",(*p_uMove),nNowModuPos );
					return (-1);
				}
				else
				{
					if(((*(p_uMove))&0x300)!=0x0)
					{
						if(((*(p_uMove))&0x300)==0x300)//chk error
							{
								itemp++;
								if( (g_nTrgCounter%25600)==0 )
								{
									printf("**nModuIndex[%d] check error : 0x%x,itemp= %d,g_nTrgCounter=%d**\n",nModuIndex,*(p_uMove),itemp,g_nTrgCounter);
										
								}
								if(!b_ChkErrFst)//only report once in one run
								{
									Push2ErrStack(PPCWARNING+MUC_FLG+ATOM_ERR+59,0,0,0,0);		/*muc error 59*/
									b_ChkErrFst = true;
									printf("**nModuIndex[%d] check error : 0x%x,itemp= %d,g_nTrgCounter=%d**\n",nModuIndex,*(p_uMove),itemp,g_nTrgCounter);
								}
							}
					}
					if(((*(p_uMove))&0xff)!=(nRecordTrgNum&0xff))//trigger number error
					{
						if(!b_TrgErrFst)//only report once in one run
						{
							Push2ErrStack(PPCWARNING+MUC_FLG+L1_ERR+60,((*(p_uMove))&0xff),(nRecordTrgNum&0xff),0,0);	/*muc error 60*/	
							b_TrgErrFst = true;
							printf("triger num not match : *(p_uMove)=%d , nRecordTrgNum=%d,g_nTrgCounter : %d,nModuIndex = %d\n",(*(p_uMove))&0xff,nRecordTrgNum&0xff,g_nTrgCounter,nModuIndex);
						}
						//if((0xff==(nRecordTrgNum&0xff))||(0x0==(nRecordTrgNum&0xff)))
						if( (g_nTrgCounter%25600)==1 )
						{
							printf("triger num not match : *(p_uMove)=%d , nRecordTrgNum=%d,g_nTrgCounter : %d,nModuIndex = %d\n",(*(p_uMove))&0xff,nRecordTrgNum&0xff,g_nTrgCounter,nModuIndex);
							printf("triger num not match2 : *(p_uMove)=0x%x, nModuIndex = %d\n",(*(p_uMove)),nModuIndex);
						}							

						*(p_PointEveHead+9) |= (1<<(16+nModuIndex)) ;	//g_fwRODHead.detectorEventType


						//return (-1);
					}
					p_uMove=p_uMove+1;
				}
			}

			while((((*p_uMove)&0xffc00000)==0)||(((*p_uMove)&0xffc00000)==0x07c00000))
			{
				if (((*p_uMove)&0xffc00000)==0)//data
				{
					unsigned int MuonChanID = (((*p_uMove)>>20)&0x3);

					if( ((gMucExchangeChainID[nModuIndex])>>MuonChanID)&0x1)//exchange chain
					{
						*p_uDesti= (gMucEnablModuleID[nModuIndex]<<27) + ((*p_uMove)&0xffff3fff) 
									+((((*p_uMove)>>14)&0x1)<<15) +((((*p_uMove)>>15)&0x1)<<14);
					}
					else
						*p_uDesti=(*p_uMove)+(gMucEnablModuleID[nModuIndex]<<27);					
				}
				else
					*p_uDesti=(*p_uMove)+(gMucEnablModuleID[nModuIndex]<<27);
				p_uMove++;
				p_uDesti++;
				nPerEventLen++;
			}
			
			p_uPerModulAddr[nModuIndex]=p_uMove;
		}
		*p_uDesti=0;   /*status*/
		p_uDesti++;
		g_fwRODTail.numberOfDataElements=nPerEventLen;
//		printf("nPerEventLen=%d\n",nPerEventLen);
		memcpy((char *)p_uDesti,&g_fwRODTail,sizeof(RODTrailer));
		p_uDesti=p_uDesti+(sizeof(RODTrailer)>>2);
		nPerEventLen+=13;
		
		*p_PointEveHead=nPerEventLen<<2;  /*event head*/

		nTotalLenRetn+=nPerEventLen;

		nRecordTrgNum++;
	}

	if((nTotalLenRetn-13*g_nTrgPerInt+2+(g_nTrgPerInt+1)*(g_MUONDBInfo.enabl_romo_sum))!=((*nSource)>>2))
	{
		Push2ErrStack(PPCERROR+MUC_FLG+ATOM_ERR+61,0,0,0,0);		/*muc error 61*/	
		printf("pack length error : source length=%d , calc length=%d\n",((*nSource)>>2),(nTotalLenRetn-13*g_nTrgPerInt+2+(g_nTrgPerInt+1)*(g_MUONDBInfo.enabl_romo_sum)));
		return(-1);
	}
	
	g_nTrgCounter=nRecordTrgNum;
	
	*nBytes2BeSend=(nTotalLenRetn+g_nTrgPerInt)<<2;
	return 0;
}

