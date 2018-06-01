
#include "vxWorks.h"

#include "fragheaders.h"
#include "fw_roppc.h"
#include "fw_runtasks.h"
#include "fw_dcgvas.h"
#include "fw_define.h"
#include "fw_staterrspt.h"
#include "fw_configinfo.h"

#include "sd_tof.h"
#include "sd_tof_datapack.h"
#include "sd_tof_hw.h"


int  _TOFDataPackCore(UINT *nDestination ,UINT *nSource,int *nBytes2BeSend)
{
        register UINT *p_uMove=nSource;
        register UINT *p_uDesti=nDestination;
        register UINT *p_PointEveHead;
 
        UINT            uDataTemp;
        uDataTemp=*p_uMove;
        uDataTemp=(uDataTemp-8)>>2;
        //printf("uDataTemp=%d\n",uDataTemp);
 
        p_uMove=p_uMove+2;
        p_PointEveHead=p_uDesti;
        p_uDesti++;

 	g_fwRODHead.level1Id;
        g_fwRODHead.level1Id=g_nTrgCounter;
 
        memcpy((char *)p_uDesti,&g_fwRODHead,sizeof(RODHeader));
        p_uDesti=p_uDesti+(sizeof(RODHeader)>>2);
 
        for (int i=0;i<uDataTemp;i++)
        {
               // *p_uDesti=( ((*p_uMove)&0xffff)<<16)|( ((*p_uMove)&0xffff0000)>>16);	//for tof parse the cblt data
		*p_uDesti=LONGSWAP(*p_uMove);
		  p_uDesti++;
                p_uMove++;
        }
        *p_uDesti=0xffff;/*status info*/
        p_uDesti++;
		
 	g_fwRODTail.numberOfDataElements=uDataTemp;
	/*printf("nPerTrgLen=%d\n",nPerTrgLen);*/
	memcpy((char *)p_uDesti,&g_fwRODTail,sizeof(RODTrailer));
	p_uDesti=p_uDesti+(sizeof(RODTrailer)>>2);
	*p_PointEveHead=13*4+uDataTemp*4;
	g_nTrgCounter++;

	*nBytes2BeSend=13*4+uDataTemp*4+4;
 
}
		


int TOFDataPackCore(UINT *nDestination ,UINT *nSource,int *nBytes2BeSend)
{
	register UINT *p_uMove=nSource;
	register UINT *p_uDesti=nDestination;
	register UINT *p_PointEveHead;

	register int 	nModuIndex;
	register int 	nTrgIndex;
	UINT 		uDataTemp;
	UINT		uDataLength;
	UINT		nCbltModuleNum;
	UINT		nRecordTrgNum;
	UINT		nPerEventLen;
	UINT		nTotalLenRetn;
	UINT		nPossibleLenth=0;
	UINT *		p_uPerModulAddr[16];
	UINT		nBlankDataNum=0;
	UINT		nNumofStatus=0;
	UINT uAjustData;
	bool boverflow;
	float ftempdata;
	
	nCbltModuleNum=g_uTofCbltModuleSum;
	uDataTemp=*nSource;
	uDataLength = *nSource;
	
	//printf("TOF DataPack: uDataTemp=%d bytes\n", uDataTemp) ;
	
	if((uDataTemp < (8 + 16*nCbltModuleNum)) || (uDataTemp> (g_nMaxCbltBytes+8))) 
	{
		printf("TOF DataPack: %d bytes were CBLTed.\n", uDataTemp - 8) ;
		Push2ErrStack(PPCERROR+TOF_FLG+ATOM_ERR+95,0,0,0,0);		/*tof error 95*/	
		return (-1) ;
	}
	//printf("uDataTemp=%d\n",uDataTemp);//leigk 071106
	
	if((uDataTemp %8)!=0)                           			/*check wether can be divided by 8*/
	{
		Push2ErrStack(PPCERROR+TOF_FLG+ATOM_ERR+96,0,0,0,0);		/*tof error 96*/	
		printf("total length can not divid by 8\n");
		return(-1); 
	}
	p_uMove=p_uMove+(uDataTemp>>2);	              /*jump to the data tail*/			
	
	for(nModuIndex=(nCbltModuleNum-1);nModuIndex>=0;nModuIndex--)
	{
		for(nTrgIndex=0;nTrgIndex<g_nTrgPerInt;nTrgIndex++)
		{
			
			uDataTemp=*(p_uMove-1);
			/*printf("uDataTemp=%d\n",uDataTemp);*/
			if((uDataTemp%8)!=0)
			{
				Push2ErrStack(PPCERROR+TOF_FLG+ATOM_ERR+97,0,0,0,0);		/*tof error 97*/	
				printf("single length can not divid by 8\n");
				return(-1);
			}
			//printf("*(p_uMove-2)=%08x,geo=%d\n",*(p_uMove-2),g_TOFDBInfo.tof_FEE_header[nModuIndex]->geo);
			if(((*(p_uMove-2))>>27)!=g_uTofCbltGeo[nModuIndex])  /*tailer leigk*/
			{
				printf("module tail postion bit error : *(p_uMove-2)=%08x,geo=%d\n", *(p_uMove-2),g_uTofCbltGeo[nModuIndex]);
				Push2ErrStack(PPCERROR+TOF_FLG+GEO_ERR+98,((*(p_uMove-2))>>27),g_uTofCbltGeo[nModuIndex],0,0);		/*tof error 98*/	
				return (-1);
			}
			nPossibleLenth=nPossibleLenth+uDataTemp;  /*check the totle length*/
			if(nPossibleLenth>(*nSource-8))
			{
				Push2ErrStack(PPCERROR+TOF_FLG+ATOM_ERR+99,0,0,0,0);		/*tof error 99*/	
				printf("totle length can't fit\n");
				return( -1);
			}
			
			p_uMove=p_uMove-(uDataTemp>>2);   /*careful for tof special other>>2*/
			if(p_uMove<nSource)/*may be something wrong with it*/
			{
				Push2ErrStack(PPCERROR+TOF_FLG+ATOM_ERR+100,0,0,0,0);		/*tof error 100*/	
				printf("address out of range\n");
				return(-1);
			}	
			
			if(((*p_uMove)>>27)!=g_uTofCbltGeo[nModuIndex]) /*header leigk*/
			{
				
				printf("module header  postion error : True head:%x,Hope head:%x\n",(*p_uMove),g_uTofCbltGeo[nModuIndex]);		

				Push2ErrStack(PPCERROR+TOF_FLG+GEO_ERR+101,((*p_uMove)>>27),g_uTofCbltGeo[nModuIndex],0,0);		/*tof error 101*/	
				return(-1);  
			}
		}
		p_uPerModulAddr[nModuIndex]=p_uMove;
	}


	nRecordTrgNum=g_nTrgCounter;
	//if((nRecordTrgNum%100)==99)
	//printf("nRecordTrgNum=%d\n",nRecordTrgNum);//leigk 071106
	nTotalLenRetn=0;

	for(nTrgIndex=0;nTrgIndex<g_nTrgPerInt;nTrgIndex++)
	{
		p_PointEveHead=p_uDesti;
		p_uDesti++;

		g_fwRODHead.level1Id=nRecordTrgNum;//+g_nCalibL1OffSet
		if((g_pCrateConfigHeader->unused)&0x1)	//jixl080111
		{
			g_fwRODHead.detectorEventType = (g_fwRODHead.detectorEventType |0x1); // 1-calib;0-no calib
//			printf("g_fwRODHead.detectorEventType = 0x%x\n",g_fwRODHead.detectorEventType);

		}
		
		memcpy((char *)p_uDesti,&g_fwRODHead,sizeof(RODHeader));
		p_uDesti=p_uDesti+(sizeof(RODHeader)>>2);

		nPerEventLen=0;
		for(nModuIndex=0; nModuIndex< nCbltModuleNum ;nModuIndex++)	
		{
			p_uMove=p_uPerModulAddr[nModuIndex];
			int nNowModuPos=g_uTofCbltGeo[nModuIndex];
		
			if(((*p_uMove)&0xf8000000)!=0)
			{
				if(((*p_uMove)>>27)!=nNowModuPos)
				{
					Push2ErrStack(PPCERROR+TOF_FLG+GEO_ERR+102,((*p_uMove)>>27),nNowModuPos,0,0);		/*tof error 102*/
					printf("packing:module head position error\n");
					return (-1);
				}
				else
				{
					if(((*(p_uMove+1))&0xff)!=(nRecordTrgNum&0xff))
					{
						Push2ErrStack(PPCFATAL+TOF_FLG+L1_ERR+103,((*(p_uMove+1))&0xff),(nRecordTrgNum&0xff),0,0);		/*tof error 103*/
						printf("module %d triger num not match : *(p_uMove+1)=%d , nRecordTrgNum=%d\n",nModuIndex,(*(p_uMove+1))&0xff,nRecordTrgNum&0xff);
						return (-1);
					}
					p_uMove=p_uMove+2;
				}
			}
			//data
			/*
			if( (TDIG_boardEn)&&(TDIG_CBLTEn)&&(nModuIndex==(nCbltModuleNum-1)))//TDIG 
			{
				while( ((*p_uMove)&0xf0000000)==0)
				{
					//UINT nChannalNum=((*p_uMove)>>21)&0xf;
					*p_uDesti=(*p_uMove);
		
					p_uMove++;
					p_uDesti++;
					nPerEventLen++;
				}			
			}*/
			//else
			{
				while( ( ((*p_uMove)&0xfe000000)==0)||( ((*p_uMove)&0xfe000000)==0x02000000 ) )
				{
					if(((*p_uMove)&0xfe000000)==0)//norm data
					{
						UINT nChannalNum=((*p_uMove)>>21)&0xf;
						if( ((*p_uMove)&0x00100000 )==0x0 )//TDC no adjustment
						{
							*p_uDesti=((*p_uMove)&0x001fffff)+(g_nConvertTable[nNowModuPos][nChannalNum]&0x7fe00000);
							//printf("geo=%d,channal=%d\n",(*p_uDesti)>>25,((*p_uDesti)>>21)&0xf );
						}
						else//ADC adjust
						{
							uAjustData = (*p_uMove)&0x00001fff;

							if((g_pCrateConfigHeader->unused)&0x1)
							{
								if(( ( (*p_uMove)>>19)&0x1)==0x1)
									boverflow = true;
								else
									boverflow = false;

								if(false==boverflow)
								{
									uAjustData=(int)( ((g_fAjust_a2[nNowModuPos][nChannalNum]*uAjustData+g_fAjust_a1[nNowModuPos][nChannalNum])*
													uAjustData+g_fAjust_a0[nNowModuPos][nChannalNum])+0.5);
								}

								else
								{
									uAjustData = uAjustData + 0x2000;
									ftempdata=( (g_fAjust_a2[nNowModuPos][nChannalNum]*uAjustData+g_fAjust_a1[nNowModuPos][nChannalNum])*
													uAjustData+g_fAjust_a0[nNowModuPos][nChannalNum]);
									ftempdata = ftempdata+0.5;
									uAjustData = ((int)(ftempdata));
								}
							}
							*p_uDesti=(uAjustData&0x00001fff)+((*p_uMove)&0x001fe000)+(g_nConvertTable[nNowModuPos][nChannalNum]&0x7fe00000);
						}
						
					}
					else		//error info
					{
						//( |symbol | crate | GEO  | TDC num |errordata )
						*p_uDesti=0xfe000000|((g_nCrateID&0x1)<<24)|((nNowModuPos&0x1f)<<19)|
											(((*p_uMove)&0x01e00000)>>6)|((*p_uMove)&0x7fff);
					}
					p_uMove++;
					p_uDesti++;
					nPerEventLen++;
				}
				if(((*p_uMove)&0xfe000000)==0x06000000)	//blank data
				{
					p_uMove++;
					nBlankDataNum++;
				}				
			}

			if(((*p_uMove)&0xf8000000)!=0)
			{
				if(((*p_uMove)>>27)!=nNowModuPos)
				{
					Push2ErrStack(PPCERROR+TOF_FLG+GEO_ERR+104,((*p_uMove)>>27),nNowModuPos,0,0);		/*tof error 104*/
					printf("module position tailor error\n");
					return (-1);
				}
				else
				{
					p_uMove=p_uMove+2;
					p_uPerModulAddr[nModuIndex]=p_uMove;
				}
			}
		}
		if( g_nRunMode==MONITOR )
		{
			*p_uDesti=g_uTofLDPulseInterval;
//			printf("val=0x%08x\n",g_uTofLDPulseInterval);
			p_uDesti++;
			*p_uDesti=g_uTofLDPulseNum;
			p_uDesti++;
			*p_uDesti=g_uTofLDSolenoid;
			p_uDesti++;
			nNumofStatus=3;
		}
		else
		{
			*p_uDesti=g_uTofCalibSignAmp&0xffff;   /*status,now save calib amp infor*/
			p_uDesti++;
			nNumofStatus=1;
		}
		g_fwRODTail.numberOfStatusElements=nNumofStatus;
		g_fwRODTail.numberOfDataElements=nPerEventLen;
		/*printf("nPerTrgLen=%d\n",nPerTrgLen);*/
		memcpy((char *)p_uDesti,&g_fwRODTail,sizeof(RODTrailer));
		p_uDesti=p_uDesti+(sizeof(RODTrailer)>>2);
		nPerEventLen+=12+nNumofStatus;
		
		*p_PointEveHead=nPerEventLen<<2;  /*event head*/
		nTotalLenRetn+=nPerEventLen;

		//printf("nRecordTrgNum=%d\n",nRecordTrgNum);
		nRecordTrgNum++;
	}
	if((nTotalLenRetn-(12+nNumofStatus)*g_nTrgPerInt+2+4*g_nTrgPerInt*nCbltModuleNum+nBlankDataNum)!=((*nSource)>>2))
	{
		//printf("front:%d\n",nTotalLenRetn-13*g_nTrgPerInt+2+4*g_nTrgPerInt*(g_TOFDBInfo.tof_FEE_sum)+nBlankDataNum);
		//printf("behind:%d\n",(*nSource)>>2);
		Push2ErrStack(PPCERROR+TOF_FLG+ATOM_ERR+105,0,0,0,0);		/*tof error 105*/	
		printf("pack length error\n");
		return(-1);
	}
	g_nTrgCounter=nRecordTrgNum;
	/*printf("now trg num is %d\n",g_nTrgCounter);*/
	
	*nBytes2BeSend=(nTotalLenRetn+g_nTrgPerInt)<<2;
	return 0;
}

/*
void TOFTest()
{
	g_nTrgCounter=0;
	g_nMaxCbltBytes=4096;
	g_TOFDBInfo.tof_FEE_sum=1;
	g_nTrgPerInt=8;
	g_TOFDBInfo.tof_FEE_header[0]->geo=9;

	UINT * u_Point=(UINT*)0x10000000;
	*u_Point=136;
	u_Point++;
	*u_Point=0xffffffff;
	u_Point++;
	for(int nbegin=0;nbegin<8;nbegin++)
	{
		*u_Point=0x48000000;
		u_Point++;
		*u_Point=nbegin;
		u_Point++;
		*u_Point=0x48800000;
		u_Point++;
		*u_Point=0x00000004;
		u_Point++;
	}
	
	int nBytes2BeSend;
	TOFDataPackFunction((UINT*)0x11000000 ,(UINT*)0x10000000,&nBytes2BeSend);

	printf("the return bytes=%d\n",nBytes2BeSend);
}
*/
