
#include "vxWorks.h"

#include "fragheaders.h"
#include "fw_roppc.h"
#include "fw_runtasks.h"
#include "fw_dcgvas.h"
#include "fw_define.h"
#include "fw_staterrspt.h"
#include "fw_configinfo.h"

#include "sd_zdd.h"
#include "sd_zdd_datapack.h"
#include "sd_zdd_hw.h"


int ZDDDataPackCore(UINT *nDestination ,UINT *nSource,int *nBytes2BeSend)//for only one trigger readout
{	
	register UINT *p_uMove=nSource;
	register UINT *p_uDesti=nDestination;
	register UINT *p_PointEveHead;

	register int 	nModuIndex;
	register int 	nTrgIndex;

	UINT		uDataLength = 0;
	UINT 		uDataTemp;
	UINT 		nPossibleLenth=0;
	
	int nV1721ModuleNum;
	int nRomoModuleNum;
	int	nRecordTrgNum;
	int nPerEventLen;
	int nTotalLenRetn;

	UINT trgnum_module0 ;
	UINT trgnum_module1 ;
	UINT trgnum_smaller; 

	
	UINT *		p_uPerModulAddr[3];
	UINT dataLength[3];
	
	nV1721ModuleNum = g_ZDDDBInfo.v1721_sum ;
	if(V1190ENB)
		nRomoModuleNum = g_ZDDDBInfo.v1721_sum + g_ZDDDBInfo.v1190_sum;
	else
		nRomoModuleNum = g_ZDDDBInfo.v1721_sum;
		
	uDataTemp = *nSource;
	uDataLength = *nSource;
	UINT *p_uGlobalHeader ;

	delta_triggerNum = 0;
	unsigned int TrgNumInData = 0;

	UINT timeStamp;
	UINT timeStamp_mod0;
	UINT timeStamp_mod1;
	UINT boardID;
	UINT V1190_nCnt = 0;

	unsigned int V1190_L1ID;
	unsigned int uDataTotalLength;
	//uDataTotalLength = *nSource;// by  bytes

	bool b_64cut = false;


	if((uDataTemp> (g_nMaxCbltBytes+8)) || (uDataTemp<=8))
	{
		printf("ZDD DataPack: %d bytes were CBLTed.\n", uDataTemp - 8) ;	
		Push2ErrStack(PPCERROR+ZDD_FLG+ATOM_ERR+40,0,0,0,0);		/*zdd error 40*/	
		return (-1) ;
	}

	if((uDataTemp %4)!=0)                           			/*check weather can be divided by 4*/
	{
		printf("total length can not divid by 4:total length = %d\n",uDataTemp);
		Push2ErrStack(PPCERROR+ZDD_FLG+ATOM_ERR+41,0,0,0,0);		/*zdd error 41*/	
		return(-1); 
	}
/*
	if(uDataTemp == (4096+8))//check if CBLT Length == max size 4096. if this, next readout will not be an event
	{
		printf("this is not an full event ,trigger counter : %d\n",g_nTrgCounter);
		UINT dataLength =( ((*(p_uMove + 2))&0xfffffff)<<2 );
		printf("data length in data header(0x%x) is %d bytes\n",(*(p_uMove + 2)),dataLength);
		//printf("uDataTemp = %d\n",uDataTemp);
		//printf(" (p_uMove) = 0x%x\n",*(p_uMove));
		//printf(" *(p_uMove) = 0x%x , *(p_uMove+1) = 0x%x\n",*p_uMove,*(p_uMove+1));
		Push2ErrStack(PPCWARNING+ZDD_FLG+ATOM_ERR+49,0,0,0,0);		//zdd error 49	
		return(-1); 		
	}
*/
	
/* double tail issue */	
/*
	p_uMove=p_uMove+(uDataTemp>>2);	              //jump to the data tail			

	if((*(p_uMove-1)) == 0xffffffff)//double tail
	{
		
		//*nSource=(*nSource)-4;
	}

	p_uMove=p_uMove -(uDataTemp>>2);	
*/
	p_uMove = p_uMove + 2;	              /*jump to the data header*/
	//printf("first data header : 0x%x \n",(*p_uMove));

	nRecordTrgNum=g_nTrgCounter;


	/*******V1190**************/
	if(V1190ENB)
	{
		p_uPerModulAddr[0] = p_uMove; //V1190 header
		if(((*p_uMove)>>27)!=0x8)
		{
			printf("V1190 Global header flag error ! *p_uMove = 0x%x\n",*p_uMove);
			return (-1);
		}
		
		V1190_L1ID =  ((*p_uMove)>>5)&0x3fffff ;
		if ((V1190_L1ID&0x3fffff)!=(nRecordTrgNum&0x3fffff))
		{
			printf("V1190 trigger number not match.V1190_L1ID = 0x%x,  nRecordTrgNum = 0x%x\n",V1190_L1ID,nRecordTrgNum);
			return -1;
		}
		p_uGlobalHeader = p_uMove;
		
		//printf("V1190 trigger number :V1190_L1ID = 0x%x,  nRecordTrgNum = 0x%x\n",V1190_L1ID,nRecordTrgNum);
		while (((*p_uMove)>>27)!=0x10)
		{
			p_uMove++;
			V1190_nCnt++;
		}
		//trailer of V1190
		uDataTemp = (((*p_uMove)>>5 )& 0xffff); //by int
		//printf("uDataTemp = %d\n",uDataTemp);
		//printf("V1190_nCnt = %d\n",V1190_nCnt);
		if(uDataTemp != (V1190_nCnt+1) )
		{
			printf("V1190 data length not match!\n");
			return (-1);
		}
		dataLength[0] = uDataTemp; //int
		// add event size into Global header
		*p_uGlobalHeader = (((*p_uGlobalHeader)&0xf8000000) + uDataTemp);
		uDataTemp = uDataTemp <<2 ;//by bytes
		nPossibleLenth=nPossibleLenth+uDataTemp;  /*check the total length*/
		p_uMove++;
	}
	/*******V1721**************/
	for(nModuIndex=0;nModuIndex<nV1721ModuleNum;nModuIndex++)
	{
		uDataTemp=(((*p_uMove)&0xfffffff)<<2);
		
		if(V1190ENB)
			dataLength[nModuIndex+1] = uDataTemp>>2;
		else
			dataLength[nModuIndex] = uDataTemp>>2;
		//printf("uDataTemp=%d\n",uDataTemp);		
		
		if(((*p_uMove)&0xf0000000) != 0xa0000000)
		{
			Push2ErrStack(PPCERROR+ZDD_FLG+ATOM_ERR+42,0,0,0,0);		/*zdd error 42*/	
			printf("module header flag error : *(p_uMove)=0x%08x,not event head , g_nTrgCounter=%d, nModuIndex = %d\n",*(p_uMove),g_nTrgCounter,nModuIndex);
			printf("*(p_uMove)=0x%x, *(p_uMove -1)=0x%x \n",*(p_uMove),*(p_uMove-1));
			//printf("V1190_nCnt = %d\n",V1190_nCnt);
			return(-1);
		}
		
		boardID = ((*(p_uMove+1))>>27) ;
		TrgNumInData = ((*(p_uMove+2))&0xffffff);
		timeStamp = (*(p_uMove+3));

		/*for CHK signal check*/
		if(( (*(p_uMove+1))>>8)&0x1  )//recv check
		{
			//printf("recv CHK signal : \n");
			if((TrgNumInData&0xff)!=0)
			{
				//printf("check error 1\n");
				//printf("header1 : 0x%x , header2 : 0x%x , header3 : 0x%x \n",*p_uMove,*(p_uMove+1),*(p_uMove+2) );
				//Push2ErrStack(PPCWARNING+ZDD_FLG+ATOM_ERR+50,0,0,0,0);		/*zdd error 50*/	
			}
		}
		else // no check
		{
			if((TrgNumInData&0xff)==0)
			{
				//printf("check error 2\n");
				//printf("header1 : 0x%x , header2 : 0x%x , header3 : 0x%x \n",*p_uMove,*(p_uMove+1),*(p_uMove+2) );
				//Push2ErrStack(PPCWARNING+ZDD_FLG+ATOM_ERR+51,0,0,0,0);		/*zdd error 51*/	
			}		
		}

		//for debug output
		/*
		if(g_nTrgCounter==0)
		{
			printf("module %d , event counter : 0x%x , trigger time counter : 0x%x \n",nModuIndex,(*(p_uMove+2))&0xffffff,*(p_uMove+3) );
		}*/
		
		if(nModuIndex==0)
		{
			trgnum_module0 = TrgNumInData;
			timeStamp_mod0 = timeStamp;
		}
		else
		{
			trgnum_module1 = TrgNumInData;
			timeStamp_mod1 = timeStamp;
		}

		trgnum_smaller = trgnum_module0;

		if (g_ZDDDBInfo.v1721_sum == 2)		
		{
			if(nModuIndex==1)
			{
				if(trgnum_module0!=trgnum_module1)
				{
				/*
					if((nRecordTrgNum%1000)==0)
					{
						//printf("********nRecordTrgNum = %d, trigger number %d in module 0 != trigger number %d in module 1\n"
						//		,nRecordTrgNum+1,trgnum_module0,trgnum_module1);
						//printf("timeStamp in module 0 = 0x%x , timeStamp in module 1 = 0x%x \n",timeStamp_mod0,timeStamp_mod1);
					}
					*/
					
					if(trgnum_module0<=trgnum_module1)
					{
						trgnum_smaller = trgnum_module0  ;
						trigger_Diff = trgnum_module1 - trgnum_module0;
					}
					else
					{
						trgnum_smaller = trgnum_module1  ;
						trigger_Diff = trgnum_module0 - trgnum_module1;
					}
					
					if(trigger_Diff_last != trigger_Diff)
					{
						printf("*nRecordTrg = %d,TrgDiff = %d, trgNum %d in mod0 != trgNum %d in mod1\n"
								,nRecordTrgNum+1,trigger_Diff,trgnum_module0,trgnum_module1);					
					}
						
					trigger_Diff_last = trigger_Diff;
					if((nRecordTrgNum%10000)==0)
						Push2ErrStack(PPCWARNING+ZDD_FLG+ATOM_ERR+43,0,0,0,0);		//zdd error 43
					//return -1;
				}
			}
		}
		
		UINT index = 0;
		if (g_ZDDDBInfo.v1721_sum == 2)	
			index = 1;
		else
			index = 0;
		if(nModuIndex==index)
		{
			if(trgnum_smaller!=((nRecordTrgNum+1)&0xffffff))//need to insert blank events to fit trigger number
			{
				if(((nRecordTrgNum+1)&0xffffff) > (trgnum_smaller))
				{
					if((nRecordTrgNum%10000)==0)
					{
						printf("nRecordTrgNum(%d) > trgnum(%d) in data \n",((nRecordTrgNum+1)&0xffffff), trgnum_smaller);
						Push2ErrStack(PPCERROR+ZDD_FLG+ATOM_ERR+44,0,0,0,0);		/*zdd error 44*/	
					//return -1;
					}
				}
				else
				{
					delta_triggerNum = ( trgnum_smaller - ((nRecordTrgNum+1)&0xffffff) );
					if(delta_triggerNum>2)
					{
						printf("delta_triggerNum>2 , nModuIndex = %d ,nRecordTrgNum = %d, DataTrgNum = %d\n",nModuIndex,nRecordTrgNum+1, trgnum_smaller);
						Push2ErrStack(PPCWARNING+ZDD_FLG+ATOM_ERR+48,0,0,0,0);		/*zdd error 48*/
					}
				}
			}
		}
		if(V1190ENB)
			p_uPerModulAddr[nModuIndex+1] = p_uMove;
		else
			p_uPerModulAddr[nModuIndex] = p_uMove;
		
		nPossibleLenth=nPossibleLenth+uDataTemp;  /*check the total length*/
		
/*		
		if(nPossibleLenth>(uDataLength-8))
		{
			Push2ErrStack(PPCERROR+ZDD_FLG+ATOM_ERR+45,0,0,0,0);		//zdd error 45
			printf("totle length can't fit,g_nTrgCounter=%d\n",g_nTrgCounter);
			return( -1);
		}
*/
		
		if (g_ZDDDBInfo.v1721_sum == 2)
			if((nModuIndex==0)&&(nPossibleLenth==(uDataLength-8)))//only first module has data
			{
				if((TrgNumInData%1000)==1)
				{
					printf("only one module has data : boardID = %d, trigger number = %d\n",boardID,TrgNumInData );
				//printf("header1 : 0x%x , header2 : 0x%x , header3 : 0x%x \n",*p_uMove,*(p_uMove+1),*(p_uMove+2) );
					Push2ErrStack(PPCWARNING+ZDD_FLG+ATOM_ERR+47,0,0,0,0);		//zdd error 47
				//nRomoModuleNum = 1;
				//break;
				}
				return -1;
			}
		else if (g_ZDDDBInfo.v1721_sum == 1)
			if((nPossibleLenth!=(uDataLength-8)))
			{
				printf("data length not match :  trigger number = %d\n",TrgNumInData );
				Push2ErrStack(PPCERROR+ZDD_FLG+ATOM_ERR+49,0,0,0,0);		//zdd error 49
				return -1;
			}			
	
		p_uMove = p_uMove+(uDataTemp>>2); 		
			
	}


//	nRecordTrgNum=g_nTrgCounter;
	//printf("nRecordTrgNum=%d\n",nRecordTrgNum);
	
	nTotalLenRetn=0;

	if(delta_triggerNum)
	{
		for(int IndexIndex=0;IndexIndex<delta_triggerNum;IndexIndex++)
		{
			p_PointEveHead=p_uDesti;
			p_uDesti++;

			g_fwRODHead.level1Id=g_nTrgCounter;
			
			memcpy((char *)p_uDesti,&g_fwRODHead,sizeof(RODHeader));
			p_uDesti=p_uDesti+(sizeof(RODHeader)>>2);
			
			*p_uDesti=0;  //write status before tail
			p_uDesti++; 
			g_fwRODTail.numberOfStatusElements=1;//???
			g_fwRODTail.numberOfDataElements=0;
			memcpy((char *)p_uDesti,&g_fwRODTail,sizeof(RODTrailer));
			p_uDesti=p_uDesti+(sizeof(RODTrailer)>>2);
			*p_PointEveHead=13*4;
			nTotalLenRetn += 13;
			g_nTrgCounter++;
		}
		if(trgnum_smaller!= ((g_nTrgCounter+1)&0xffffff) )
		{
			printf("trigger number not match after blank data insert , TrgNumInData = %d , g_nTrgCounter = %d\n",
					trgnum_smaller,(g_nTrgCounter+1)&0xffffff);
			Push2ErrStack(PPCERROR+ZDD_FLG+ATOM_ERR+46,0,0,0,0);		/*zdd error 46*/	
			return -1;
		}
	}


	p_PointEveHead=p_uDesti;
	p_uDesti++;
		
	g_fwRODHead.level1Id=g_nTrgCounter;
	memcpy((char *)p_uDesti,&g_fwRODHead,sizeof(RODHeader));
	p_uDesti=p_uDesti+(sizeof(RODHeader)>>2);

	nPerEventLen=0;
	int itemp =0;
	int data_IntLength = 0;
	int actual_size = 0;
	for(nModuIndex=0; nModuIndex<nRomoModuleNum ;nModuIndex++)	
	{
		actual_size += dataLength[nModuIndex]; //int
	}
	if ((actual_size<<2)!= (uDataLength-8))
	{
		//dataLength[nRomoModuleNum-1]--;
		//*(p_uPerModulAddr[nRomoModuleNum-1] + dataLength[nRomoModuleNum-1] -1) = 0xfefefefe;
		b_64cut = true;
		dataLength[nRomoModuleNum-1]--;
	}
	
	for(nModuIndex=0; nModuIndex<nRomoModuleNum ;nModuIndex++)	
	{
		p_uMove=p_uPerModulAddr[nModuIndex];
		data_IntLength = dataLength[nModuIndex];
		
//		int nNowModuPos=gZddV1721Geo[nModuIndex];		

		for(int index=0; index<data_IntLength;index++)
		{
			*p_uDesti=(*p_uMove);
			p_uMove++;
			p_uDesti++;
			nPerEventLen++;
		}
		
		//p_uPerModulAddr[nModuIndex]=p_uMove;
	}
	if(b_64cut)
	{
		*p_uDesti= 0xfefefefe;
		p_uDesti++;
		nPerEventLen++;
	}

	
	*p_uDesti=0;   /*status*/
	p_uDesti++;
	g_fwRODTail.numberOfDataElements=nPerEventLen;
//		printf("nPerEventLen=%d\n",nPerEventLen);
	memcpy((char *)p_uDesti,&g_fwRODTail,sizeof(RODTrailer));
	p_uDesti=p_uDesti+(sizeof(RODTrailer)>>2);
	nPerEventLen+=13;
	
	*p_PointEveHead=nPerEventLen<<2;  /*length head*/
	nTotalLenRetn+=nPerEventLen;

	g_nTrgCounter++;

/*
	if((nTotalLenRetn-13*g_nTrgPerInt+2)!=((*nSource)>>2))
	{
		Push2ErrStack(PPCERROR+ZDD_FLG+ATOM_ERR+61,0,0,0,0);		
		printf("pack length error : source length=%d , calc length=%d\n",((*nSource)>>2),(nTotalLenRetn-13*g_nTrgPerInt+2));
		return(-1);
	}

*/
	
	*nBytes2BeSend=(nTotalLenRetn+g_nTrgPerInt + delta_triggerNum)<<2;
	return 0;
}



int ZDD_DataCheck(UINT *nSource)
{
	
	register UINT *p_uMove=nSource;

//	register UINT *p_PointEveHead;

	register int 	nModuIndex;
//	register int 	nTrgIndex;

	UINT uDataLength = 0;
	UINT uDataTemp;
	UINT nPossibleLenth=0;
	
	UINT nRomoModuleNum;
	UINT	nRecordTrgNum;
//	int 	nPerEventLen;
//	int 	nTotalLenRetn;

//	UINT trgnum_module0 ;
//	UINT trgnum_module1 ;
//	UINT trgnum_smaller; 
	
	UINT *		p_uPerModulAddr[2];
	
	nRomoModuleNum=g_ZDDDBInfo.v1721_sum;
	uDataTemp = *nSource;
	uDataLength = *nSource;

//	delta_triggerNum = 0;
	UINT TrgNumInData = 0;

//	UINT timeStamp;
//	UINT timeStamp_mod0;
//	UINT timeStamp_mod1;
	UINT boardID;

	b_singleBoard = false;
	
	if((uDataTemp> (g_nMaxCbltBytes+8)) || (uDataTemp<=8))
	{
		printf("ZDD DataPack: %d bytes were CBLTed.\n", uDataTemp - 8) ;	
		Push2ErrStack(PPCERROR+ZDD_FLG+ATOM_ERR+40,0,0,0,0);		/*zdd error 40*/	
		return (-1) ;
	}

	if((uDataTemp %4)!=0)                           			/*check weather can be divided by 4*/
	{
		printf("total length can not divid by 4:total length = %d\n",uDataTemp);
		Push2ErrStack(PPCERROR+ZDD_FLG+ATOM_ERR+41,0,0,0,0);		/*zdd error 41*/	
		return(-1); 
	}

	/* double tail issue */	

	p_uMove = p_uMove + 2;	              /*jump to the data header*/

	nRecordTrgNum=g_nTrgCounter;
	
	for(nModuIndex=0;nModuIndex<nRomoModuleNum;nModuIndex++)
	{
		uDataTemp=(((*p_uMove)&0xfffffff)<<2);
		//printf("uDataTemp=%d\n",uDataTemp);		
		
		if(((*p_uMove)&0xf0000000) != 0xa0000000)
		{
			Push2ErrStack(PPCERROR+ZDD_FLG+ATOM_ERR+42,0,0,0,0);		/*zdd error 42*/	
			printf("module header flag error : *(p_uMove)=0x%08x,not event head , g_nTrgCounter=%d, nModuIndex = %d\n",*(p_uMove),g_nTrgCounter,nModuIndex);
			return(-1);
		}


		
		boardID = ((*(p_uMove+1))>>27) ;
		TrgNumInData = ((*(p_uMove+2))&0xffffff);
//		timeStamp = (*(p_uMove+3));

		/*for CHK signal check*/
		if(( (*(p_uMove+1))>>8)&0x1  )//recv check
		{
			//printf("recv CHK signal : \n");
			if((TrgNumInData&0xff)!=0)
			{
				printf("check error 1\n");
				printf("header1 : 0x%x , header2 : 0x%x , header3 : 0x%x \n",*p_uMove,*(p_uMove+1),*(p_uMove+2) );
				Push2ErrStack(PPCWARNING+ZDD_FLG+ATOM_ERR+50,0,0,0,0);		/*zdd error 50*/	
			}
		}
		else // no check
		{
			if((TrgNumInData&0xff)==0)
			{
				printf("check error 2\n");
				printf("header1 : 0x%x , header2 : 0x%x , header3 : 0x%x \n",*p_uMove,*(p_uMove+1),*(p_uMove+2) );
				Push2ErrStack(PPCWARNING+ZDD_FLG+ATOM_ERR+51,0,0,0,0);		/*zdd error 51*/	
			}		
		}
		
		p_uPerModulAddr[nModuIndex] = p_uMove;
		
		nPossibleLenth = nPossibleLenth+uDataTemp;  /*check the total length*/
		
		if((nModuIndex==0)&&(nPossibleLenth==(uDataLength-8)))//only first module has data
		{
			printf("only one module has data : boardID = %d, trigger number = %d\n",boardID,TrgNumInData );
			//printf("header1 : 0x%x , header2 : 0x%x , header3 : 0x%x \n",*p_uMove,*(p_uMove+1),*(p_uMove+2) );
			Push2ErrStack(PPCWARNING+ZDD_FLG+ATOM_ERR+47,0,0,0,0);		//zdd error 47
		
			//set single module flag here
			b_singleBoard = true;
			break;
		}
	
		p_uMove = p_uMove+(uDataTemp>>2); 		
			
	}

	if((nPossibleLenth!=(uDataLength-8))&&(nPossibleLenth!=(uDataLength-12)))
	{
		Push2ErrStack(PPCERROR+ZDD_FLG+ATOM_ERR+45,0,0,0,0);		//zdd error 45
		printf("totle length can't fit,g_nTrgCounter=%d\n",g_nTrgCounter);
		return( -1);
	}
		
}

int ZDD_SrcToQ(UINT *nSource)
{
	register UINT *p_uMove=nSource;
	register int 	nModuIndex;
	UINT uDataTemp;	
	int nRomoModuleNum;

	UINT TrgNumInData[2];
	UINT boardID[2];	
	UINT dataLen[2];
	UINT *p_uPerModulAddr[2];

	char *	p_cLocalWrite ;
	UINT32 	nLocalWrite ;
	int nRetryTimes = 0;
	
	nRomoModuleNum=g_ZDDDBInfo.v1721_sum;
	uDataTemp = *nSource;

	p_uMove = p_uMove + 2;	              /*jump to the data header*/

	//get address for each board
	if(!b_singleBoard)
		for(nModuIndex=0;nModuIndex<nRomoModuleNum;nModuIndex++)
		{
			dataLen[nModuIndex]=(((*p_uMove)&0xfffffff)<<2);//bytes
			//printf("uDataTemp=%d\n",uDataTemp);		
			boardID[nModuIndex] = ((*(p_uMove+1))>>27) ;
			TrgNumInData[nModuIndex] = ((*(p_uMove+2))&0xffffff);
			p_uPerModulAddr[nModuIndex] = p_uMove;
			
			p_uMove = p_uMove+(uDataTemp>>2); 	
		}
	else
	{
		dataLen[0] =(((*p_uMove)&0xfffffff)<<2);//bytes
		boardID[0] = ((*(p_uMove+1))>>27) ;
		TrgNumInData[0] = ((*(p_uMove+2))&0xffffff);		
		p_uPerModulAddr[0] = p_uMove;
	}

	//move source CBLT data into ring buffer
	UINT index = 0;
	if (b_singleBoard)	
		index = 1;
	else
		index = 2;

	for(int i=0;i<index;i++)
	{
		if(NULL == (p_cLocalWrite= rng_dmaApply(g_ZDDRng[i],4096,CPU_METHOD)))
		{
			DEBUG_OUTPUT(("rng_dmaApply(g_ZDDRng[%d],4096) failed .\n", i)) ;
			break;
		}

		nLocalWrite = (UINT32)p_cLocalWrite ;
		memcpy((char*)((UINT32)(nLocalWrite)),(char*)(p_uPerModulAddr[i]), dataLen[i]);

		rng_dmaWritten(g_ZDDRng[i],dataLen[i]);
	}
}


int ZDD_pack(UINT *nDestination ,int *nBytes2BeSend)
{
	int nBytesToBePack[2];
	int nRetryTimes ;
	UINT trgNumInData[2];
	char *p_czddRng[2];
	UINT *p_uData[2];
	UINT *p_uMove;

	UINT nRecordTrgNum = 0;
	UINT nTotalLenRetn = 0;
	UINT nRomoModuleNum;
	UINT nPerEventLen;

	nRomoModuleNum=g_ZDDDBInfo.v1721_sum;
	

	register UINT *p_uDesti=nDestination;
	register UINT *p_PointEveHead;

	for(int i=0;i<2;i++)
	{
		if(NULL == (p_czddRng[i] = rng_dmaRead(g_ZDDRng[i],4096,CPU_METHOD)))
		{
			Push2ErrStack(PPCWARNING+ZDD_FLG+ATOM_ERR+50,0,0,0,0);		//error 50
		}
		else
		{
			p_uData[i] = (UINT32*)p_czddRng[i];
			//get trigger number in module 0
			trgNumInData[i] = ((*(p_uData[i]+2))&0xffffff);   
			nBytesToBePack[i] = (((*(p_uData[i]))&0xfffffff)<<2);
		}
	}

	//if trigger number in mod0== mod1 ,  then pack a whole event
	//if trigger number not match, throw the small trigger
	if(trgNumInData[0] < trgNumInData[1]) //throw data in Q0
	{
		if(ERROR == rng_dmaFree(g_ZDDRng[0],nBytesToBePack[0]))
		{
			printf("1:free g_ZDDRng[0] error \n");
		}
	}
	else if (trgNumInData[1] < trgNumInData[0]) //throw data in Q1
	{
		if(ERROR == rng_dmaFree(g_ZDDRng[1],nBytesToBePack[1]))
		{
			printf("1:free g_ZDDRng[1] error \n");
		}
	}
	else //equal, start an event pack
	{
		nTotalLenRetn=0;
		delta_triggerNum = 0;
		
		nRecordTrgNum=g_nTrgCounter;
		delta_triggerNum = ( trgNumInData[0] - ((nRecordTrgNum+1)&0xffffff) );

		if(delta_triggerNum)
		{
			for(int IndexIndex=0;IndexIndex<delta_triggerNum;IndexIndex++)
			{
				p_PointEveHead=p_uDesti;
				p_uDesti++;

				g_fwRODHead.level1Id=g_nTrgCounter;
				
				memcpy((char *)p_uDesti,&g_fwRODHead,sizeof(RODHeader));
				p_uDesti=p_uDesti+(sizeof(RODHeader)>>2);
				
				*p_uDesti=0;  //write status before tail
				p_uDesti++; 
				g_fwRODTail.numberOfStatusElements=1;//???
				g_fwRODTail.numberOfDataElements=0;
				memcpy((char *)p_uDesti,&g_fwRODTail,sizeof(RODTrailer));
				p_uDesti=p_uDesti+(sizeof(RODTrailer)>>2);
				*p_PointEveHead=13*4;
				nTotalLenRetn += 13;
				g_nTrgCounter++;
			}
			if(trgNumInData[0] != ((g_nTrgCounter+1)&0xffffff) )
			{
				printf("trgNum not match after blank data insert , TrgNumInData = %d , g_nTrgCounter = %d\n",
						trgNumInData[0],(g_nTrgCounter+1)&0xffffff);
				Push2ErrStack(PPCERROR+ZDD_FLG+ATOM_ERR+46,0,0,0,0);		/*zdd error 46*/	
				//return -1;
			}
		}


		p_PointEveHead=p_uDesti;
		p_uDesti++;
			
		g_fwRODHead.level1Id=g_nTrgCounter;
		memcpy((char *)p_uDesti,&g_fwRODHead,sizeof(RODHeader));
		p_uDesti=p_uDesti+(sizeof(RODHeader)>>2);

		nPerEventLen=0;
		int itemp =0;
		int data_IntLength = 0;
		for(int nModuIndex=0; nModuIndex<nRomoModuleNum ;nModuIndex++)	
		{
			p_uMove=p_uData[nModuIndex];
			data_IntLength = ((*p_uMove)&0xfffffff);	

			for(int index=0; index<data_IntLength;index++)
			{
				*p_uDesti=(*p_uMove);
				p_uMove++;
				p_uDesti++;
				nPerEventLen++;
			}
		}
		*p_uDesti=0;   /*status*/
		p_uDesti++;
		g_fwRODTail.numberOfDataElements=nPerEventLen;
	//		printf("nPerEventLen=%d\n",nPerEventLen);
		memcpy((char *)p_uDesti,&g_fwRODTail,sizeof(RODTrailer));
		p_uDesti=p_uDesti+(sizeof(RODTrailer)>>2);
		nPerEventLen+=13;
		
		*p_PointEveHead=nPerEventLen<<2;  /*length head*/
		nTotalLenRetn+=nPerEventLen;

		g_nTrgCounter++;
		
		*nBytes2BeSend=(nTotalLenRetn+g_nTrgPerInt + delta_triggerNum)<<2;

		if(ERROR == rng_dmaFree(g_ZDDRng[0],nBytesToBePack[0]))
		{
			printf("2:free g_ZDDRng[0] error \n");
		}		
		if(ERROR == rng_dmaFree(g_ZDDRng[1],nBytesToBePack[1]))
		{
			printf("2:free g_ZDDRng[1] error \n");
		}

	
	}
	return 0;
}


int _ZDDDataPackCore(UINT *nDestination ,UINT *nSource,int *nBytes2BeSend)//for only one trigger readout
{
	UINT *p_uMove=nSource;
	UINT *p_uDesti=nDestination;
	int dataLen ;
	
	if(-1==ZDD_DataCheck( (UINT*)p_uMove ))
	{
		//data pack error
		Push2ErrStack(PPCERROR+ZDD_FLG+ATOM_ERR+46,0,0,0,0);		/*zdd error 46*/	
		//return a data pack flag
	}
	ZDD_SrcToQ( (UINT*)p_uMove );
	ZDD_pack( (UINT*)p_uDesti ,&dataLen);	

	*nBytes2BeSend = dataLen;	
}
