
#include "vxWorks.h"

#include "fragheaders.h"
#include "fw_roppc.h"

#include "fw_dcgvas.h"
#include "fw_staterrspt.h"
#include "fw_configinfo.h"
#include "fw_define.h"

#include "sd_emc.h"
#include "sd_emc_datapack.h"
#include "sd_emc_hw.h"

#ifdef DEBUG_OUTPUT
#undef DEBUG_OUTPUT
#define DEBUG_OUTPUT(x)  printf x 
#endif


int EMCDataPackCore(UINT *nDestination ,UINT *nSource,int *nBytes2BeSend)
{
	
	register int nModuIndex;
	register int nTrgIndex;
	UINT uDataTemp;
	
	int nCoundLength=0;
	int nDoubleTail=0;
	UINT *p_uPerModulAddr[16];
	int nChannalNum;
	int nNowModuPos;	
	int nTotalLenRetn=0;
	int nPerTrgLen=0;
	int nRecordTrgNum;
	
	register UINT *p_uMove=nSource;
	register UINT *p_uDesti=nDestination;
	register UINT *p_PointEveHead;	

	int uAjustData;

	bool boverFlow;
	
	uDataTemp=(*p_uMove);
	if(uDataTemp==8)//if CBLT length eq 0
	{
		Push2ErrStack(PPCERROR+EMC_FLG+ATOM_ERR+22,0,0,0,0);		/*emc error 22*/
		DEBUG_OUTPUT(("%s::%d\n", __FILE__, __LINE__) );
		return(-1); 
	}
	if((uDataTemp%4)!=0)//"total length can not divid by 4
	{
		Push2ErrStack(PPCERROR+EMC_FLG+ATOM_ERR+23,0,0,0,0);		/*emc error 23*/
		DEBUG_OUTPUT(("%s::%d\n", __FILE__, __LINE__) );
		return(-1); 
	}
	p_uMove=p_uMove+(uDataTemp>>2);
	if((*(p_uMove-1))==(*(p_uMove-2)))
	{	
		*nSource=(*nSource)-4;
		p_uMove=p_uMove-1;			//double tailer
	}
	for(nModuIndex=(g_EMCDBInfo.emc_Q_sum-1);nModuIndex>=0;nModuIndex--)
	{
		for(nTrgIndex=0;nTrgIndex<g_nTrgPerInt;nTrgIndex++)
		{
			
			uDataTemp=(*(p_uMove-1))&0x0000ffff;
			if((uDataTemp%4)!=0)//single length can not divid by 4
			{			
			
				Push2ErrStack(PPCERROR+EMC_FLG+ATOM_ERR+24,0,0,0,0);		/*emc error 24*/
				DEBUG_OUTPUT(("%s::%d\n", __FILE__, __LINE__) );
				d((UINT32)(nSource), ((*nSource)+16)/4, 4) ;

				return(-1);
			}
			if(((*(p_uMove-1))>>27)!=g_EMCDBInfo.emc_Q_header[nModuIndex]->geo)	//module tail geo not match
			{
			
				Push2ErrStack(PPCERROR+EMC_FLG+GEO_ERR+25,((*(p_uMove-1))>>27),
								g_EMCDBInfo.emc_Q_header[nModuIndex]->geo,0,0);/*emc error 25*/
				printf("module tail geo not match:0x%x ; 0x%x\n",((*(p_uMove-1))>>27),g_EMCDBInfo.emc_Q_header[nModuIndex]->geo);
//				DEBUG_OUTPUT(("%s::%d\n", __FILE__, __LINE__) );
				return (-1);
			}
			nCoundLength=nCoundLength+uDataTemp;  /*check the totle length*/
			if(nCoundLength>(*nSource-8))//totle length not fit
			{	
				Push2ErrStack(PPCERROR+EMC_FLG+ATOM_ERR+26,0,0,0,0);		/*emc error 26*/
				DEBUG_OUTPUT(("%s::%d\n", __FILE__, __LINE__) );
				return( -1);
			}
			
			p_uMove=p_uMove-(uDataTemp>>2);
			if(p_uMove<nSource)/*may be something wrong with it*/
			{
					/*----------for test ,if have wrong here,we can stop the simulate L1 signal immediately
					
					-------------------------*/
				Push2ErrStack(PPCERROR+EMC_FLG+ATOM_ERR+27,0,0,0,0);		/*emc error 27*/
				DEBUG_OUTPUT(("%s::%d\n", __FILE__, __LINE__) );
				//address out of range
				return(-1);
			}	
			
			if(((*p_uMove)>>27)!=g_EMCDBInfo.emc_Q_header[nModuIndex]->geo) //module header geo not match
			{
				Push2ErrStack(PPCERROR+EMC_FLG+GEO_ERR+28,((*p_uMove)>>27),
								g_EMCDBInfo.emc_Q_header[nModuIndex]->geo,0,0);		/*emc error 28*/
				DEBUG_OUTPUT(("%s::%d\n", __FILE__, __LINE__) );
				d((UINT32)(nSource), ((*nSource)+16)/4, 4) ;

				return(-1);  
			}
		}
		p_uPerModulAddr[nModuIndex]=p_uMove;
	}
	if((nCoundLength+nDoubleTail)!=(*nSource-8)) //count cblt length error
	{
		Push2ErrStack(PPCERROR+EMC_FLG+ATOM_ERR+29,0,0,0,0);		/*emc error 29*/
		DEBUG_OUTPUT(("%s::%d\n", __FILE__, __LINE__) );
		return (-1);
	}
	/*---------------------------------------check the info and get the data index ok------*/
	

	nRecordTrgNum=g_nTrgCounter;
	//printf("record trger: %d\n",nRecordTrgNum); 2005-12-15 test
	
	for(nTrgIndex=0;nTrgIndex<g_nTrgPerInt;nTrgIndex++)
	{
		p_PointEveHead=p_uDesti;
		p_uDesti++;

		g_fwRODHead.level1Id=nRecordTrgNum;
		
		if((g_pCrateConfigHeader->unused)&0x1)	//jixl080111
		{
			g_fwRODHead.detectorEventType = (g_fwRODHead.detectorEventType |0x1); // 1-calib;0-no calib
//			printf("g_fwRODHead.detectorEventType = 0x%x\n",g_fwRODHead.detectorEventType);

		}
		
		memcpy((char *)p_uDesti,&g_fwRODHead,sizeof(RODHeader));
		p_uDesti=p_uDesti+(sizeof(RODHeader)>>2);

		nPerTrgLen=0;
		for(nModuIndex=0;nModuIndex<g_EMCDBInfo.emc_Q_sum;nModuIndex++)	
		{
			p_uMove=p_uPerModulAddr[nModuIndex];
			nNowModuPos=g_EMCDBInfo.emc_Q_header[nModuIndex]->geo;
			
			if(((*p_uMove)&0xf8000000)!=0)//packing:module head geo error
			{
				if(((*p_uMove)>>27)!=nNowModuPos)
				{
					Push2ErrStack(PPCERROR+EMC_FLG+GEO_ERR+30,((*p_uMove)>>27),nNowModuPos,0,0);		/*emc error 30*/
					DEBUG_OUTPUT(("%s::%d\n", __FILE__, __LINE__) );
					return (-1);
				}
				else
				{
					if(((*p_uMove)&0xff)!=((nRecordTrgNum)&0xff))//triger num not match
					{					
						Push2ErrStack(PPCFATAL+EMC_FLG+L1_ERR+31,((*p_uMove)&0xff),
												((nRecordTrgNum)&0xff),0,0);				/*emc error 31*/
						
						printf("Src:%d,Hope:%d,CBLT:%d,Mo:%d\n",((*p_uMove)&0xff),
												((nRecordTrgNum)&0xff),g_nTrgCounter,nNowModuPos);	
						d((UINT32)(nSource), ((*nSource)+16)/4, 4) ;

						return (-1);	
					}
					p_uMove++;
				}
			}
			while(((*p_uMove)&0xf8000000)==0)
			{
				nChannalNum=((*p_uMove)&0x00ffffff)>>19;
			
				boverFlow = false;
				if( (g_nConvertTable[nNowModuPos][nChannalNum]&0xfff80000)!=0)
				{
					uAjustData= (*p_uMove)&0x7ff;
//					 if((g_nConvertTable[nNowModuPos][nChannalNum]&0xfff80000)==0x74400000)
//					 {
//					 	DEBUG_OUTPUT(("g_mid_fAjust_a1[%d][%d]=%f\n",nNowModuPos,nChannalNum,g_mid_fAjust_a1[nNowModuPos][nChannalNum]));
//						printf("before calc uAjustData= 0x%x    ",uAjustData);
//					 }
					
					if((g_pCrateConfigHeader->unused)&0x1)
					{
						if( ((*p_uMove)&0x1800)==0x0)//low
							uAjustData=(int)( (g_low_fAjust_a2[nNowModuPos][nChannalNum]*uAjustData+g_low_fAjust_a1[nNowModuPos][nChannalNum])*
											uAjustData+g_low_fAjust_a0[nNowModuPos][nChannalNum]+0.5);
						else if ( ((*p_uMove)&0x1800)==0x800)//middle
						{
							uAjustData=(int)( (g_mid_fAjust_a2[nNowModuPos][nChannalNum]*uAjustData+g_mid_fAjust_a1[nNowModuPos][nChannalNum])*
											uAjustData+g_mid_fAjust_a0[nNowModuPos][nChannalNum]+0.5);						
						}
						else if( ((*p_uMove)&0x1800)==0x1000)//high
						{
							if (uAjustData==0x3ff)		//overflow!!! set info bit 11
							{
								 boverFlow = true;
							}
							uAjustData=(int)( (g_high_fAjust_a2[nNowModuPos][nChannalNum]*uAjustData+g_high_fAjust_a1[nNowModuPos][nChannalNum])*
											uAjustData+g_high_fAjust_a0[nNowModuPos][nChannalNum]+0.5);
						}
						else
						{
							printf("EMC Range Infor Error\n");
							Push2ErrStack(PPCERROR+EMC_FLG+ATOM_ERR+66,0,0,0,0);		//emc error 66
							return -1;
						}
					
//					 if((g_nConvertTable[nNowModuPos][nChannalNum]&0xfff80000)==0x74400000)
//						printf("******after calc uAjustData= 0x%x\n",uAjustData);

						if(uAjustData<0)
	//						printf("********uAjustData*********<0\n");
							uAjustData=0;
					}	
					if(boverFlow)
						*p_uDesti=((*p_uMove)&0x0007e000)+0x1800+(g_nConvertTable[nNowModuPos][nChannalNum]&0xfff80000) + (uAjustData&0x7ff);
					else
						*p_uDesti=((*p_uMove)&0x0007f800)+(g_nConvertTable[nNowModuPos][nChannalNum]&0xfff80000) + (uAjustData&0x7ff);
					if((((*p_uMove)&0x7e000)!=(0<<13))&&(((*p_uMove)&0x7e000)!=(60<<13)))
					{
						nPerTrgLen++;
						p_uDesti++;
					}
				}
				p_uMove++;
			}
			if(((*p_uMove)&0xf8000000)!=0)
			{
				if(((*p_uMove)>>27)!=nNowModuPos)//pack:module position tailor error
				{
					Push2ErrStack(PPCERROR+EMC_FLG+GEO_ERR+32,((*p_uMove)>>27),nNowModuPos,0,0);		/*emc error 32*/
					DEBUG_OUTPUT(("%s::%d\n", __FILE__, __LINE__) );
					return (-1);
				}
				else
				{
					p_uMove++;
					p_uPerModulAddr[nModuIndex]=p_uMove;
				}
			}
		}
		*p_uDesti=g_nEmcDAC;   /*status,add DAC INFOR*/
		p_uDesti++;
		
		g_fwRODTail.numberOfDataElements=nPerTrgLen;
		/*printf("nPerTrgLen=%d\n",nPerTrgLen);*/
		memcpy((char *)p_uDesti,&g_fwRODTail,sizeof(RODTrailer));
		p_uDesti=p_uDesti+(sizeof(RODTrailer)>>2);
		nPerTrgLen+=13;
		
		*p_PointEveHead=nPerTrgLen<<2;  /*event head*/
		nTotalLenRetn+=nPerTrgLen;
		
		nRecordTrgNum++;
	}
/*	
	if((nTotalLenRetn-13*g_nTrgPerInt+2+2*g_nTrgPerInt*(g_EMCDBInfo.emc_Q_sum))!=((*nSource)>>2)) //end check pack length error
	{
		Push2ErrStack(PPCERROR+EMC_FLG+ATOM_ERR+33,((*p_uMove)>>27),nNowModuPos,0,0);		//emc error 33
		DEBUG_OUTPUT(("%s::%d\n", __FILE__, __LINE__) );	
		return(-1);
	}
*/
	g_nTrgCounter=nRecordTrgNum;
	/*printf("now trg num is %d\n",g_nTrgCounter);*/
	
	*nBytes2BeSend=(nTotalLenRetn+g_nTrgPerInt)<<2;
	/*printf("send length is %d\n",*nBytes2BeSend);*/
	return 0;
}



// for only one module (monitor) in CBLT.
int EMCMoniCrateDataPackCore(UINT *nDestination ,UINT *nSource,int *nBytes2BeSend)
{
	
	register int nTrgIndex;
	UINT uDataTemp;
	
	int nCoundLength=0;
	int nDoubleTail=0;
	UINT *p_uPerModulAddr;
	int nChannalNum;
	int nNowModuPos;	
	int nTotalLenRetn=0;
	int nPerTrgLen=0;
	int nRecordTrgNum;
	
	register UINT *p_uMove=nSource;
	register UINT *p_uDesti=nDestination;
	register UINT *p_PointEveHead;	

	int uAjustData;
	
	uDataTemp=(*p_uMove);
	if(uDataTemp==8)//if CBLT length eq 0
	{
		Push2ErrStack(PPCERROR+EMC_FLG+ATOM_ERR+22,0,0,0,0);		/*emc error 22*/
		DEBUG_OUTPUT(("%s::%d\n", __FILE__, __LINE__) );
		return(-1); 
	}
	if((uDataTemp%4)!=0)//"total length can not divid by 4
	{
		Push2ErrStack(PPCERROR+EMC_FLG+ATOM_ERR+23,0,0,0,0);		/*emc error 23*/
		DEBUG_OUTPUT(("%s::%d\n", __FILE__, __LINE__) );
		return(-1); 
	}
	p_uMove=p_uMove+(uDataTemp>>2);
	if((*(p_uMove-1))==(*(p_uMove-2)))
	{	
		*nSource=(*nSource)-4;
		p_uMove=p_uMove-1;			//double tailer
	}

		for(nTrgIndex=0;nTrgIndex<g_nTrgPerInt;nTrgIndex++)
		{
			
			uDataTemp=(*(p_uMove-1))&0x0000ffff;
			if((uDataTemp%4)!=0)//single length can not divid by 4
			{			
			
				Push2ErrStack(PPCERROR+EMC_FLG+ATOM_ERR+24,0,0,0,0);		/*emc error 24*/
				DEBUG_OUTPUT(("%s::%d\n", __FILE__, __LINE__) );
				return(-1);
			}
			if(((*(p_uMove-1))>>27)!=g_EMCDBInfo.emc_monitor_header->geo)	//module tail geo not match
			{
			
				Push2ErrStack(PPCERROR+EMC_FLG+GEO_ERR+25,((*(p_uMove-1))>>27),
								g_EMCDBInfo.emc_monitor_header->geo,0,0);/*emc error 25*/
				DEBUG_OUTPUT(("module tail geo not match! source = 0x%x, hope = 0x%x\n", ((*(p_uMove-1))>>27),g_EMCDBInfo.emc_monitor_header->geo));
				return (-1);
			}
			nCoundLength=nCoundLength+uDataTemp;  /*check the totle length*/
			if(nCoundLength>(*nSource-8))//totle length not fit
			{	
				Push2ErrStack(PPCERROR+EMC_FLG+ATOM_ERR+26,0,0,0,0);		/*emc error 26*/
				DEBUG_OUTPUT(("%s::%d\n", __FILE__, __LINE__) );
				return( -1);
			}
			
			p_uMove=p_uMove-(uDataTemp>>2);
			if(p_uMove<nSource)/*may be something wrong with it*/
			{
					/*----------for test ,if have wrong here,we can stop the simulate L1 signal immediately
					
					-------------------------*/
				Push2ErrStack(PPCERROR+EMC_FLG+ATOM_ERR+27,0,0,0,0);		/*emc error 27*/
				DEBUG_OUTPUT(("%s::%d\n", __FILE__, __LINE__) );
				//address out of range
				return(-1);
			}	
			
			if(((*p_uMove)>>27)!=g_EMCDBInfo.emc_monitor_header->geo) //module header geo not match
			{
				Push2ErrStack(PPCERROR+EMC_FLG+GEO_ERR+28,((*p_uMove)>>27),
								g_EMCDBInfo.emc_monitor_header->geo,0,0);		/*emc error 28*/
				DEBUG_OUTPUT(("%s::%d\n", __FILE__, __LINE__) );

				return(-1);  
			}
		}
		p_uPerModulAddr=p_uMove;

	if((nCoundLength+nDoubleTail)!=(*nSource-8)) //count cblt length error
	{
		Push2ErrStack(PPCERROR+EMC_FLG+ATOM_ERR+29,0,0,0,0);		/*emc error 29*/
		DEBUG_OUTPUT(("%s::%d\n", __FILE__, __LINE__) );
		return (-1);
	}
	/*---------------------------------------check the info and get the data index ok------*/
	

	nRecordTrgNum=g_nTrgCounter;
	//printf("record trger: %d\n",nRecordTrgNum); 2005-12-15 test
	
	for(nTrgIndex=0;nTrgIndex<g_nTrgPerInt;nTrgIndex++)
	{
		p_PointEveHead=p_uDesti;
		p_uDesti++;

		g_fwRODHead.level1Id=nRecordTrgNum;
		
		if((g_pCrateConfigHeader->unused)&0x1)	//jixl080111
		{
			g_fwRODHead.detectorEventType = (g_fwRODHead.detectorEventType |0x1); // 1-calib;0-no calib
//			printf("g_fwRODHead.detectorEventType = 0x%x\n",g_fwRODHead.detectorEventType);

		}
		
		memcpy((char *)p_uDesti,&g_fwRODHead,sizeof(RODHeader));
		p_uDesti=p_uDesti+(sizeof(RODHeader)>>2);

		nPerTrgLen=0;

			p_uMove=p_uPerModulAddr;
			nNowModuPos=g_EMCDBInfo.emc_monitor_header->geo;
			
			if(((*p_uMove)&0xf8000000)!=0)//packing:module head geo error
			{
				if(((*p_uMove)>>27)!=nNowModuPos)
				{
					Push2ErrStack(PPCERROR+EMC_FLG+GEO_ERR+30,((*p_uMove)>>27),nNowModuPos,0,0);		/*emc error 30*/
					DEBUG_OUTPUT(("%s::%d\n", __FILE__, __LINE__) );
					return (-1);
				}
				else
				{
					if(((*p_uMove)&0xff)!=((nRecordTrgNum)&0xff))//triger num not match
					{					
						Push2ErrStack(PPCFATAL+EMC_FLG+L1_ERR+31,((*p_uMove)&0xff),
												((nRecordTrgNum)&0xff),0,0);				/*emc error 31*/
						
						printf("Src:%d,Hope:%d,CBLT:%d,Mo:%d\n",((*p_uMove)&0xff),
												((nRecordTrgNum)&0xff),g_nTrgCounter,nNowModuPos);	
						return (-1);	
					}
					p_uMove++;
				}
			}
			while(((*p_uMove)&0xf8000000)==0)
			{
				nChannalNum=((*p_uMove)&0x00ffffff)>>19;
			

				if( (g_nConvertTable[nNowModuPos][nChannalNum]&0xfff80000)!=0)
				{
					uAjustData= (*p_uMove)&0x7ff;

					if( ((*p_uMove)&0x1800)==0x0)
						uAjustData=(int)( (g_low_fAjust_a2[nNowModuPos][nChannalNum]*uAjustData+g_low_fAjust_a1[nNowModuPos][nChannalNum])*
										uAjustData+g_low_fAjust_a0[nNowModuPos][nChannalNum]);
					else if ( ((*p_uMove)&0x1800)==0x800)
					{
						uAjustData=(int)( (g_mid_fAjust_a2[nNowModuPos][nChannalNum]*uAjustData+g_mid_fAjust_a1[nNowModuPos][nChannalNum])*
										uAjustData+g_mid_fAjust_a0[nNowModuPos][nChannalNum]);						
					}
					else if( ((*p_uMove)&0x1800)==0x1000)
					{
						uAjustData=(int)( (g_high_fAjust_a2[nNowModuPos][nChannalNum]*uAjustData+g_high_fAjust_a1[nNowModuPos][nChannalNum])*
										uAjustData+g_high_fAjust_a0[nNowModuPos][nChannalNum]);
					}
					else
					{
						printf("EMC Range Infor Error\n");
						Push2ErrStack(PPCERROR+EMC_FLG+ATOM_ERR+66,0,0,0,0);		/*emc error 66*/
						return -1;
					}
					//printf("after calc uAjustData= %d\n",uAjustData);

				
//					*p_uDesti=((*p_uMove)&0x0007f800)+(g_nConvertTable[nNowModuPos][nChannalNum]&0xfff80000) + (uAjustData&0x7ff);
					*p_uDesti=((*p_uMove)&0x0007f800)+(g_nConvertTable[nNowModuPos][nChannalNum]&0xfff80000) + ((*p_uMove)&0x7ff);
					nPerTrgLen++;
					p_uDesti++;
				}
				p_uMove++;
			}
			if(((*p_uMove)&0xf8000000)!=0)
			{
				if(((*p_uMove)>>27)!=nNowModuPos)//pack:module position tailor error
				{
					Push2ErrStack(PPCERROR+EMC_FLG+GEO_ERR+32,((*p_uMove)>>27),nNowModuPos,0,0);		/*emc error 32*/
					DEBUG_OUTPUT(("%s::%d\n", __FILE__, __LINE__) );
					return (-1);
				}
				else
				{
					p_uMove++;
					p_uPerModulAddr=p_uMove;
				}
			}
//		}
		*p_uDesti=g_nEmcDAC;   /*status,add DAC INFOR*/
		p_uDesti++;
		
		g_fwRODTail.numberOfDataElements=nPerTrgLen;
		/*printf("nPerTrgLen=%d\n",nPerTrgLen);*/
		memcpy((char *)p_uDesti,&g_fwRODTail,sizeof(RODTrailer));
		p_uDesti=p_uDesti+(sizeof(RODTrailer)>>2);
		nPerTrgLen+=13;
		
		*p_PointEveHead=nPerTrgLen<<2;  /*event head*/
		nTotalLenRetn+=nPerTrgLen;
		
		nRecordTrgNum++;
	}
/*	
	if((nTotalLenRetn-13*g_nTrgPerInt+2+2*g_nTrgPerInt*(g_EMCDBInfo.emc_Q_sum))!=((*nSource)>>2)) //end check pack length error
	{
		Push2ErrStack(PPCERROR+EMC_FLG+ATOM_ERR+33,((*p_uMove)>>27),nNowModuPos,0,0);		//emc error 33
		DEBUG_OUTPUT(("%s::%d\n", __FILE__, __LINE__) );	
		return(-1);
	}
*/
	g_nTrgCounter=nRecordTrgNum;
	/*printf("now trg num is %d\n",g_nTrgCounter);*/
	
	*nBytes2BeSend=(nTotalLenRetn+g_nTrgPerInt)<<2;
	/*printf("send length is %d\n",*nBytes2BeSend);*/
	return 0;
}



int EMCMoniBlankPackCore(UINT *nDestination,UINT Count)
{
	int IndexIndex;
	register UINT *p_uDesti=nDestination;
	register UINT *p_PointEveHead;

	for(IndexIndex=0;IndexIndex<g_nTrgPerInt;IndexIndex++)
	{
		p_PointEveHead=p_uDesti;
		p_uDesti++;

		g_fwRODHead.level1Id=Count;
		
		memcpy((char *)p_uDesti,&g_fwRODHead,sizeof(RODHeader));
		p_uDesti=p_uDesti+(sizeof(RODHeader)>>2);
		
		*p_uDesti=0;  //write status before tail
		p_uDesti++; 
		g_fwRODTail.numberOfStatusElements=1;//leigk071106
		g_fwRODTail.numberOfDataElements=0;
		memcpy((char *)p_uDesti,&g_fwRODTail,sizeof(RODTrailer));
		p_uDesti=p_uDesti+(sizeof(RODTrailer)>>2);
		*p_PointEveHead=13*4;
	}
	return(g_nTrgPerInt*14*4);
}


//for send both adjust and original data
int _EMCDataPackCore(UINT *nDestination ,UINT *nSource,int *nBytes2BeSend)	
{
	
	register int nModuIndex;
	register int nTrgIndex;
	UINT uDataTemp;
	
	int nCoundLength=0;
	int nDoubleTail=0;
	UINT *p_uPerModulAddr[16];
	int nChannalNum;
	int nNowModuPos;	
	int nTotalLenRetn=0;
	int nPerTrgLen=0;
	int nRecordTrgNum;
	
	register UINT *p_uMove=nSource;
	register UINT *p_uDesti=nDestination;
	register UINT *p_PointEveHead;	

	int uAjustData;
	
	uDataTemp=(*p_uMove);
	if(uDataTemp==8)//if CBLT length eq 0
	{
		Push2ErrStack(PPCERROR+EMC_FLG+ATOM_ERR+22,0,0,0,0);		/*emc error 22*/
		DEBUG_OUTPUT(("%s::%d\n", __FILE__, __LINE__) );
		return(-1); 
	}
	if((uDataTemp%4)!=0)//"total length can not divid by 4
	{
		Push2ErrStack(PPCERROR+EMC_FLG+ATOM_ERR+23,0,0,0,0);		/*emc error 23*/
		DEBUG_OUTPUT(("%s::%d\n", __FILE__, __LINE__) );
		return(-1); 
	}
	p_uMove=p_uMove+(uDataTemp>>2);
	if((*(p_uMove-1))==(*(p_uMove-2)))
	{	
		*nSource=(*nSource)-4;
		p_uMove=p_uMove-1;			//double tailer
	}
	for(nModuIndex=(g_EMCDBInfo.emc_Q_sum-1);nModuIndex>=0;nModuIndex--)
	{
		for(nTrgIndex=0;nTrgIndex<g_nTrgPerInt;nTrgIndex++)
		{
			
			uDataTemp=(*(p_uMove-1))&0x0000ffff;
			if((uDataTemp%4)!=0)//single length can not divid by 4
			{			
			
				Push2ErrStack(PPCERROR+EMC_FLG+ATOM_ERR+24,0,0,0,0);		/*emc error 24*/
				DEBUG_OUTPUT(("%s::%d\n", __FILE__, __LINE__) );
				return(-1);
			}
			if(((*(p_uMove-1))>>27)!=g_EMCDBInfo.emc_Q_header[nModuIndex]->geo)	//module tail geo not match
			{
			
				Push2ErrStack(PPCFATAL+EMC_FLG+GEO_ERR+25,((*(p_uMove-1))>>27),
								g_EMCDBInfo.emc_Q_header[nModuIndex]->geo,0,0);/*emc error 25*/
				DEBUG_OUTPUT(("%s::%d\n", __FILE__, __LINE__) );
				return (-1);
			}
			nCoundLength=nCoundLength+uDataTemp;  /*check the totle length*/
			if(nCoundLength>(*nSource-8))//totle length not fit
			{	
				Push2ErrStack(PPCERROR+EMC_FLG+ATOM_ERR+26,0,0,0,0);		/*emc error 26*/
				DEBUG_OUTPUT(("%s::%d\n", __FILE__, __LINE__) );
				return( -1);
			}
			
			p_uMove=p_uMove-(uDataTemp>>2);
			if(p_uMove<nSource)/*may be something wrong with it*/
			{
					/*----------for test ,if have wrong here,we can stop the simulate L1 signal immediately
					
					-------------------------*/
				Push2ErrStack(PPCERROR+EMC_FLG+ATOM_ERR+27,0,0,0,0);		/*emc error 27*/
				DEBUG_OUTPUT(("%s::%d\n", __FILE__, __LINE__) );
				//address out of range
				return(-1);
			}	
			
			if(((*p_uMove)>>27)!=g_EMCDBInfo.emc_Q_header[nModuIndex]->geo) //module header geo not match
			{
				Push2ErrStack(PPCFATAL+EMC_FLG+GEO_ERR+28,((*p_uMove)>>27),
								g_EMCDBInfo.emc_Q_header[nModuIndex]->geo,0,0);		/*emc error 28*/
				DEBUG_OUTPUT(("%s::%d\n", __FILE__, __LINE__) );

				return(-1);  
			}
		}
		p_uPerModulAddr[nModuIndex]=p_uMove;
	}
	if((nCoundLength+nDoubleTail)!=(*nSource-8)) //count cblt length error
	{
		Push2ErrStack(PPCERROR+EMC_FLG+ATOM_ERR+29,0,0,0,0);		/*emc error 29*/
		DEBUG_OUTPUT(("%s::%d\n", __FILE__, __LINE__) );
		return (-1);
	}
	/*---------------------------------------check the info and get the data index ok------*/
	

	nRecordTrgNum=g_nTrgCounter;
	//printf("record trger: %d\n",nRecordTrgNum); 2005-12-15 test
	
	for(nTrgIndex=0;nTrgIndex<g_nTrgPerInt;nTrgIndex++)
	{
		p_PointEveHead=p_uDesti;
		p_uDesti++;

		g_fwRODHead.level1Id=nRecordTrgNum;
		
		if((g_pCrateConfigHeader->unused)&0x1)	//jixl080111
			g_fwRODHead.detectorEventType = (g_fwRODHead.detectorEventType |0x1); // 1-calib;0-no calib
		
		memcpy((char *)p_uDesti,&g_fwRODHead,sizeof(RODHeader));
		p_uDesti=p_uDesti+(sizeof(RODHeader)>>2);

		nPerTrgLen=0;
		for(nModuIndex=0;nModuIndex<g_EMCDBInfo.emc_Q_sum;nModuIndex++)	
		{
			p_uMove=p_uPerModulAddr[nModuIndex];
			nNowModuPos=g_EMCDBInfo.emc_Q_header[nModuIndex]->geo;
			
			if(((*p_uMove)&0xf8000000)!=0)//packing:module head geo error
			{
				if(((*p_uMove)>>27)!=nNowModuPos)
				{
					Push2ErrStack(PPCFATAL+EMC_FLG+GEO_ERR+30,((*p_uMove)>>27),nNowModuPos,0,0);		/*emc error 30*/
					DEBUG_OUTPUT(("%s::%d\n", __FILE__, __LINE__) );
					return (-1);
				}
				else
				{
					if(((*p_uMove)&0xff)!=((nRecordTrgNum)&0xff))//triger num not match
					{					
						Push2ErrStack(PPCFATAL+EMC_FLG+L1_ERR+31,((*p_uMove)&0xff),
												((nRecordTrgNum)&0xff),0,0);				/*emc error 31*/
						
						printf("Src:%d,Hope:%d,CBLT:%d,Mo:%d\n",((*p_uMove)&0xff),
												((nRecordTrgNum)&0xff),g_nInterruptCounter,nNowModuPos);	
						return (-1);	
					}
					p_uMove++;
				}
			}
			while(((*p_uMove)&0xf8000000)==0)
			{
				nChannalNum=((*p_uMove)&0x00ffffff)>>19;
			

				if( (g_nConvertTable[nNowModuPos][nChannalNum]&0xfff80000)!=0)
				{

					//int uAjustData=(*p_uMove)&0x7ff;
					//printf("before %d\n",uAjustData);
					//uAjustData= (g_fAjust_a2[nNowModuPos][nChannalNum]*uAjustData+g_fAjust_a1[nNowModuPos][nChannalNum])*
					//					uAjustData+g_fAjust_a0[nNowModuPos][nChannalNum];
					//*p_uDesti=(g_nConvertTable[nNowModuPos][nChannalNum]&0xfff80000)+
					//				((*p_uMove)&0x0007f800)+(uAjustData&0x7ff);

					uAjustData= (*p_uMove)&0x7ff;
					//printf("before calc uAjustData= %d\n",uAjustData);
//					printf("(*p_uMove)= 0x%x\n",(*p_uMove));
					if( ((*p_uMove)&0x1800)==0x0)
						uAjustData=(int)( (g_low_fAjust_a2[nNowModuPos][nChannalNum]*uAjustData+g_low_fAjust_a1[nNowModuPos][nChannalNum])*
										uAjustData+g_low_fAjust_a0[nNowModuPos][nChannalNum]);
					else if ( ((*p_uMove)&0x1800)==0x800)
					{
						uAjustData=(int)( (g_mid_fAjust_a2[nNowModuPos][nChannalNum]*uAjustData+g_mid_fAjust_a1[nNowModuPos][nChannalNum])*
										uAjustData+g_mid_fAjust_a0[nNowModuPos][nChannalNum]);						
					}
					else if( ((*p_uMove)&0x1800)==0x1000)
					{
						uAjustData=(int)( (g_high_fAjust_a2[nNowModuPos][nChannalNum]*uAjustData+g_high_fAjust_a1[nNowModuPos][nChannalNum])*
										uAjustData+g_high_fAjust_a0[nNowModuPos][nChannalNum]);
					}
					else
					{
						printf("Range Infor Error\n");
						return -1;
					}
					//printf("after calc uAjustData= %d\n",uAjustData);
					
					*p_uDesti=((*p_uMove)&0x0007f800)+(g_nConvertTable[nNowModuPos][nChannalNum]&0xfff80000) + (uAjustData&0x7ff);
					nPerTrgLen++;
					p_uDesti++;
					*p_uDesti=((*p_uMove)&0x0007f800)+(g_nConvertTable[nNowModuPos][nChannalNum]&0xfff80000) + ((*p_uMove)&0x7ff);
					nPerTrgLen++;
					p_uDesti++;
					
				}
				p_uMove++;
			}
			if(((*p_uMove)&0xf8000000)!=0)
			{
				if(((*p_uMove)>>27)!=nNowModuPos)//pack:module position tailor error
				{
					Push2ErrStack(PPCFATAL+EMC_FLG+GEO_ERR+32,((*p_uMove)>>27),nNowModuPos,0,0);		/*emc error 32*/
					DEBUG_OUTPUT(("%s::%d\n", __FILE__, __LINE__) );
					return (-1);
				}
				else
				{
					p_uMove++;
					p_uPerModulAddr[nModuIndex]=p_uMove;
				}
			}
		}
		*p_uDesti=g_nEmcDAC;   /*status,add DAC INFOR*/
		p_uDesti++;
		
		g_fwRODTail.numberOfDataElements=nPerTrgLen;
		/*printf("nPerTrgLen=%d\n",nPerTrgLen);*/
		memcpy((char *)p_uDesti,&g_fwRODTail,sizeof(RODTrailer));
		p_uDesti=p_uDesti+(sizeof(RODTrailer)>>2);
		nPerTrgLen+=13;
		
		*p_PointEveHead=nPerTrgLen<<2;  /*event head*/
		nTotalLenRetn+=nPerTrgLen;
		
		nRecordTrgNum++;
	}
/*	
	if((nTotalLenRetn-13*g_nTrgPerInt+2+2*g_nTrgPerInt*(g_EMCDBInfo.emc_Q_sum))!=((*nSource)>>2)) //end check pack length error
	{
		Push2ErrStack(PPCFATAL+EMC_FLG+ATOM_ERR+33,((*p_uMove)>>27),nNowModuPos,0,0);		//emc error 33
		DEBUG_OUTPUT(("%s::%d\n", __FILE__, __LINE__) );	
		return(-1);
	}
*/
	g_nTrgCounter=nRecordTrgNum;
	/*printf("now trg num is %d\n",g_nTrgCounter);*/
	
	*nBytes2BeSend=(nTotalLenRetn+g_nTrgPerInt)<<2;
	/*printf("send length is %d\n",*nBytes2BeSend);*/
	return 0;
}

