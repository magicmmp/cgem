/*****sd_trg_datapack.cpp*****/
#include "vxWorks.h"

#include "fragheaders.h"
#include "fw_roppc.h"
#include "fw_runtasks.h"
#include "fw_dcgvas.h"
#include "fw_staterrspt.h"
#include "fw_define.h"


#include "sd_trg.h"
#include "sd_trg_datapack.h"
#include "sd_trg_hw.h"
/*
------------
|event length|
------------
|RODHeader |
------------
|DATA          |
|DATA          |
------------
|STATUS      |
------------
|RODTrailer  |<-----real data length add In
------------
return should all of this segment*/


//leigk071123, this copy for proccessing multi events per interrupt,not complet
//jixl 071128
//080226 trigger new data format
int TRGDataPackCore(UINT *nDestination ,UINT *nSource,int *nBytes2BeSend)
{	


	//suggest that we have know how many module do cblt data readout /per crate
	register UINT *p_uMove=nSource;		//data source
	register UINT *p_uDesti=nDestination;	//destination address
	
	register UINT *p_PointEveHead;		//save per event length
	
	register UINT *p_uEnd;

	register int 	nModuIndex;			//index for
	register int 	nTrgIndex;				//index for
	
	UINT 		uDataTemp = 0;	
	UINT		uDataLength = 0;
	
	int 			nNowModuPos;	
	int 			nModuleNum;		//
	int			nRecordTrgNum;		//for temp save the trg num
	int			nPerTrgLen = 0;
	int			nTempLen;
	int 			nPerEventLen=0;
	int 			nTotalLenRetn=0;
	
	UINT *		p_uPerModulAddr[16];

	uDataLength=(*p_uMove)-8;//total CBLT bytes
	if(uDataLength==0)//if CBLT length eq 0
	{
		Push2ErrStack(PPCERROR+TRG_FLG+ATOM_ERR+131,0,0,0,0);		/*trg error 131*/
		printf("DataPack error: CBLT length ==0 \n");
		return(-1); 
	}
	if((uDataLength%4)!=0)//"total length can not divid by 4
	{
		Push2ErrStack(PPCERROR+TRG_FLG+ATOM_ERR+132,0,0,0,0);		/*trg error 132*/
		printf("DataPack error: total length can not divid by 4 \n");
		return(-1); 
	}

	p_uMove ++; //length
	p_uMove ++;//fffffff
	
	p_uMove = p_uMove+(uDataLength>>2);
	p_uEnd = p_uMove;
	if((*(p_uMove-1))==0xffffffff)//double tailer
	{	
		*nSource=(*nSource)-4;
		p_uEnd--;
	}

	p_uMove = p_uMove-(uDataLength>>2);
		

	//check and get each module data position
	for(nModuIndex=0; nModuIndex< nTrgCbltModNum; nModuIndex++)
	{
		p_uPerModulAddr[nModuIndex] = p_uMove;
		for(nTrgIndex=0;nTrgIndex<g_nTrgPerInt;nTrgIndex++)
		{
			//check GEO in head
			if(((*p_uMove)>>27)!=uTrgCbltModGeo[nModuIndex])
			{
				printf("head geo not match:src  = 0x%x,GEO=0x%x,g_nTrgCounter = %d\n",(*p_uMove),uTrgCbltModGeo[nModuIndex],g_nTrgCounter);
				Push2ErrStack(PPCFATAL+TRG_FLG+GEO_ERR+133,((*p_uMove)>>27),(uTrgCbltModGeo[nModuIndex]),0,0);		/*trg error 133*/	
				return (-1);
			}

			//check check in head1
			if( (((*p_uMove)>>8)&0x3)==0x3)
			{
				printf("check error: (*p_uMove) = 0x%x,nModuIndex=%d\n",(*p_uMove),nModuIndex);
				Push2ErrStack(PPCWARNING+TRG_FLG+ATOM_ERR+134,0,0,0,0);		/*trg error 134*/	
//				return (-1);
			}
			
			if( (((*p_uMove)>>26)&0x1) ==0x1)
			{
				// if(TrgTOFSYNCheck)//tmp 20100705

					printf("data pack FUERR error!head1 : 0x%x ,g_nTrgCounter = %d\n",(*p_uMove),g_nTrgCounter);
					Push2ErrStack(PPCERROR+TRG_FLG+ATOM_ERR+141,0,0,0,0);		//trg error 141
					return (-1);

			}

//			if ( (g_nTrgCounter%1000 )==999)
//				printf("head1 : 0x%x \n",(*p_uMove));
			nTempLen=((*p_uMove)>>16)&0x3ff;
//			printf("Now module data length = %d\n",nTempLen);

			//head2 check
			p_uMove++;
			if(((*p_uMove)&0xc0000000)!=0xc0000000)
			{
				printf("head 2 flag error: (*p_uMove) = 0x%x,nModuIndex=%d \n",(*p_uMove),nModuIndex);
				Push2ErrStack(PPCERROR+TRG_FLG+ATOM_ERR+135,0,0,0,0);		/*trg error 135*/	
				return -1;
			}
			else if( (((*p_uMove)>>14)&0x3ff)!=(nTempLen-1) )
			{
				printf("head 2 dataLength error: head1L = 0x%x,head2L = 0x%x \n",nTempLen,(((*p_uMove)>>14)&0x3ff) );
				Push2ErrStack(PPCERROR+TRG_FLG+ATOM_ERR+136,0,0,0,0);		/*trg error 136*/	
				return -1;
			}

			p_uMove = p_uMove-1+nTempLen;
		}
	}

	
	/*-----------------check the info and get the data index ok----------------*/
	

	nRecordTrgNum=g_nTrgCounter;
	//printf("record trger: %d\n",nRecordTrgNum); 2005-12-15 test
	
	for(nTrgIndex=0;nTrgIndex<g_nTrgPerInt;nTrgIndex++)
	{
		p_PointEveHead=p_uDesti;
		p_uDesti++;

		g_fwRODHead.level1Id=nRecordTrgNum;
		memcpy((char *)p_uDesti,&g_fwRODHead,sizeof(RODHeader));
		p_uDesti=p_uDesti+(sizeof(RODHeader)>>2);

		nPerTrgLen=0;
		for(nModuIndex=0;nModuIndex<nTrgCbltModNum;nModuIndex++)	
		{
			p_uMove=p_uPerModulAddr[nModuIndex];
			nNowModuPos = uTrgCbltModGeo[nModuIndex];
			
			if(((*p_uMove)&0xf8000000)!=0)//packing:module head geo error
			{
				if(((*p_uMove)>>27)!=nNowModuPos)
				{
					printf("head1 geo addr error:src Geo = 0x%x, nNowModuPos = 0x%x\n",((*p_uMove)>>27),nNowModuPos);
					Push2ErrStack(PPCFATAL+TRG_FLG+GEO_ERR+137,((*p_uMove)>>27),nNowModuPos,0,0);		/*trg error 137*/	
					return (-1);
				}
				else
				{
					if(((*p_uMove)&0xff)!=((nRecordTrgNum)&0xff))//triger num not match
					{					
						printf("Src:%d,Hope:%d,CBLT:%d,Mo:%d\n",((*p_uMove)&0xff),
												((nRecordTrgNum)&0xff),g_nTrgCounter,nNowModuPos);	
						Push2ErrStack(PPCFATAL+TRG_FLG+L1_ERR+138,((*p_uMove)&0xff),((nRecordTrgNum)&0xff),0,0);		/*trg error 138*/	
						return (-1);	
					}
					/*for ETFT tmp test, print readout data*/
					/*
					if((g_nTrgCounter%1000)==0)
					{
						if(((*p_uMove)>>27)==TrgETFGeo)//ETFT
						{
							printf("ETFT data for trigger number %d : ",g_nTrgCounter);
							for(int i=0;i<(6*(etft_readout_window*2+1));i++)
								printf(" 0x%x ",*(p_uMove+i));
							printf("\n");
						}
					}
					*/
					p_uMove++;
				}
			}
			
			if(((*p_uMove)&0xc0000000)!=0xc0000000)
			{
				printf("head 2 error: (*p_uMove) = 0x%x \n",(*p_uMove));
				Push2ErrStack(PPCERROR+TRG_FLG+ATOM_ERR+139,0,0,0,0);		/*trg error 139*/	
				return -1;
			}
			
			else
			{
				*p_uDesti=(*p_uMove);
				nPerTrgLen++;
				p_uDesti++;
				p_uMove++;
			}

			while((((*p_uMove)&0xf8000000)==0)&&(p_uMove!=p_uEnd))
			{
				*p_uDesti=(*p_uMove);
				nPerTrgLen++;
				p_uDesti++;
				p_uMove++;
			}
			p_uPerModulAddr[nModuIndex]=p_uMove;
		}
		*p_uDesti = 0;   /*status,add DAC INFOR*/
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

	if((nTotalLenRetn-13*g_nTrgPerInt+2+g_nTrgPerInt*nTrgCbltModNum)!=((*nSource)>>2)) //end check pack length error
	{
//		if((g_nTrgCounter%1000)==0)
			printf("pack length error.nTotalLenRetn = %d, (*nSource)>>2 = %d\n",nTotalLenRetn,(*nSource)>>2);
		Push2ErrStack(PPCERROR+TRG_FLG+ATOM_ERR+140,0,0,0,0);		//trg error 140	
		return(-1);
	}

	g_nTrgCounter=nRecordTrgNum;
	/*printf("now trg num is %d\n",g_nTrgCounter);*/
	
	*nBytes2BeSend=(nTotalLenRetn+g_nTrgPerInt)<<2;
	/*printf("send length is %d\n",*nBytes2BeSend);*/
	return 0;

}


