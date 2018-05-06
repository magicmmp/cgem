
#include "vxWorks.h"

#include "fragheaders.h"
#include "fw_roppc.h"
#include "fw_runtasks.h"
#include "fw_dcgvas.h"
#include "fw_define.h"
#include "fw_staterrspt.h"
#include "fw_configinfo.h"

#include "sd_etf.h"
#include "sd_etf_datapack.h"
#include "sd_etf_hw.h"


int ETFDataPackCore(UINT *nDestination ,UINT *nSource,int *nBytes2BeSend)
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
	
	nCbltModuleNum=g_uEtfCbltModuleSum;
	uDataTemp=*nSource;
	uDataLength = *nSource;
	
	//printf("ETF DataPack: uDataTemp=%d bytes\n", uDataTemp) ;
	
	if((uDataTemp < (8 + 16*nCbltModuleNum)) || (uDataTemp> (g_nMaxCbltBytes+8))) 
	{
		printf("ETF DataPack: %d bytes were CBLTed.\n", uDataTemp - 8) ;
		Push2ErrStack(PPCERROR+ETF_FLG+ATOM_ERR+31,0,0,0,0);		/*etf error 31*/	
		return (-1) ;
	}
	//printf("uDataTemp=%d\n",uDataTemp);//leigk 071106
	
	if((uDataTemp %8)!=0)                           			/*check wether can be divided by 8*/
	{
		Push2ErrStack(PPCERROR+ETF_FLG+ATOM_ERR+32,0,0,0,0);		/*etf error 32*/	
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
				Push2ErrStack(PPCERROR+ETF_FLG+ATOM_ERR+33,0,0,0,0);		/*etf error 33*/	
				printf("single length can not divid by 8\n");
				return(-1);
			}
			//printf("*(p_uMove-2)=%08x,geo=%d\n",*(p_uMove-2),g_ETFDBInfo.etf_FEE_header[nModuIndex]->geo);
			if(((*(p_uMove-2))>>27)!=g_uEtfCbltGeo[nModuIndex])  /*tailer leigk*/
			{
				printf("nModuIndex = %d\n",nModuIndex);
				printf("module tail position flag error : *(p_uMove-2)=%08x,geo=%d\n", *(p_uMove-2),g_uEtfCbltGeo[nModuIndex]);
				Push2ErrStack(PPCERROR+ETF_FLG+GEO_ERR+34,((*(p_uMove-2))>>27),g_uEtfCbltGeo[nModuIndex],0,0);		/*etf error 34*/	
				return (-1);
			}
			nPossibleLenth=nPossibleLenth+uDataTemp;  /*check the totle length*/
			if(nPossibleLenth>(*nSource-8))
			{
				Push2ErrStack(PPCERROR+ETF_FLG+ATOM_ERR+35,0,0,0,0);		/*etf error 35*/	
				printf("totle length can't fit\n");
				return( -1);
			}
			
			p_uMove=p_uMove-(uDataTemp>>2);   /*careful for etf special other>>2*/
			if(p_uMove<nSource)/*may be something wrong with it*/
			{
				Push2ErrStack(PPCERROR+ETF_FLG+ATOM_ERR+36,0,0,0,0);		/*etf error 36*/	
				printf("address out of range\n");
				return(-1);
			}	
			
			if(((*p_uMove)>>27)!=g_uEtfCbltGeo[nModuIndex]) /*header leigk*/
			{
				
				printf("module header  position error : True head:%x,Hope head:%x\n",(*p_uMove),g_uEtfCbltGeo[nModuIndex]);		

				Push2ErrStack(PPCERROR+ETF_FLG+GEO_ERR+37,((*p_uMove)>>27),g_uEtfCbltGeo[nModuIndex],0,0);		/*etf error 37*/	
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
			int nNowModuPos=g_uEtfCbltGeo[nModuIndex];
		
			if(((*p_uMove)&0xf8000000)!=0)
			{
				if(((*p_uMove)>>27)!=nNowModuPos)
				{
					Push2ErrStack(PPCERROR+ETF_FLG+GEO_ERR+38,((*p_uMove)>>27),nNowModuPos,0,0);		/*etf error 38*/
					printf("packing:module head position error\n");
					return (-1);
				}
				else
				{
					if(((*(p_uMove+1))&0xff)!=(nRecordTrgNum&0xff))
					{
						Push2ErrStack(PPCFATAL+ETF_FLG+L1_ERR+39,((*(p_uMove+1))&0xff),(nRecordTrgNum&0xff),0,0);		/*etf error 39*/
						printf("module %d triger num not match : *(p_uMove+1)=%d , nRecordTrgNum=%d\n",nModuIndex,(*(p_uMove+1))&0xff,nRecordTrgNum&0xff);
						return (-1);
					}
					p_uMove=p_uMove+2;
				}
			}
			//data	
			while( ((*p_uMove)&0xf0000000)==0)
			{
				UINT nTempFlag = ((*p_uMove)>>24)&0xf;
				if(nTempFlag<=8) //true data: TDC nunber 0-8
				{
					if(!DATA_FORMAT_TRANS)
						*p_uDesti=(*p_uMove) + ((nModuIndex)<<28);
					else
					{
						int nTDC = nTempFlag;
						int nChannel = ((*p_uMove)>>21)&0x7 ;
						//printf("*p_uMove = 0x%x\n",(*p_uMove));
						*p_uDesti= ((*p_uMove)&0x7ffff) + ((((*p_uMove)>>20)&0x1)<<19) + (g_nConverTable[nModuIndex][nTDC][nChannel]<<20) ;
						//printf("nModuIndex = %d, nTDC = %d, nChannel=%d, g_nConverTable = 0x%x\n",nModuIndex,nTDC,nChannel,g_nConverTable[nModuIndex][nTDC][nChannel]<<20);
						//printf("*p_uDesti = 0x%x\n",(*p_uDesti));
					}
					p_uMove++;
					p_uDesti++;
					nPerEventLen++;
				}
				else if (nTempFlag==0xF)//blank data
				{
					p_uMove++;
					nBlankDataNum++;
				}
				else if (nTempFlag==0xE)//err data
				{
					if(!DATA_FORMAT_TRANS)
						*p_uDesti=(*p_uMove) + ((nModuIndex*2)<<28);
					else
						*p_uDesti= (0xfe000000) + ((*p_uMove)&0x7fff) + (((*p_uMove)>>20)&0xf)<<15 + (nNowModuPos<<19) ; 
		
					p_uMove++;
					p_uDesti++;
					nPerEventLen++;				
				}
			}				
	
			if(((*p_uMove)&0xf8000000)!=0)
			{
				if(((*p_uMove)>>27)!=nNowModuPos)
				{
					Push2ErrStack(PPCERROR+ETF_FLG+GEO_ERR+40,((*p_uMove)>>27),nNowModuPos,0,0);		/*etf error 40*/
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

		*p_uDesti=g_uEtfCalibSignAmp&0xffff;   /*status,now save calib amp infor*/
		p_uDesti++;
		nNumofStatus=1;

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
		//printf("front:%d\n",nTotalLenRetn-13*g_nTrgPerInt+2+4*g_nTrgPerInt*(g_ETFDBInfo.etf_FEE_sum)+nBlankDataNum);
		//printf("behind:%d\n",(*nSource)>>2);
		Push2ErrStack(PPCERROR+ETF_FLG+ATOM_ERR+41,0,0,0,0);		/*etf error 41*/	
		printf("pack length error\n");
		return(-1);
	}
	g_nTrgCounter=nRecordTrgNum;
	/*printf("now trg num is %d\n",g_nTrgCounter);*/
	
	*nBytes2BeSend=(nTotalLenRetn+g_nTrgPerInt)<<2;
	return 0;
}

