#include "vxWorks.h"
#include "stdlib.h"
#include "stdio.h"
#include "math.h"
#include "taskLib.h"
#include "time.h"
#include "sysLib.h"
#include "intLib.h"
#include "asm.h"

#include "fw_roppc.h"
#include "fw_dcgvas.h"
#include "fw_define.h"
#include "fw_runtasks.h"
#include "fw_filefunc.h"
#include "fw_staterrspt.h"

#include "sd_mdc.h"
#include "sd_mdc_datapack.h"
#include "sd_mdc_hw.h"

#define MDC_SAVE_DATA
#undef MDC_SAVE_DATA

/*assume the data structure  is that at the beginning of the CBLT package we use an UINT record the total lenth*/

int  MDCDataPackCore(UINT *nDestination, UINT * nSource, int nTrgPerInt, int nMdlAmount,int *MdlList,int * p_nReturn)
{
	 	register int 		iii;
		register int 		kkk;
		register UINT *	Move_Point= nSource;     	//first use to direct the source head ,then goto the end,at last direct data head
												
		register UINT *	Read_Six_Data; 
		UINT *			Per_Module_Point[17];  	//point to each module head

		register UINT *	pHead_Rod_Tri;    		//point to the head of per triger pack after trans
		UINT 			Total_len_Return=0;
		UINT 			Tar_Per_Tri_Len;   		//data length of per triger
		UINT 			Channel_Num;
		UINT 			Data_Temp;
		UINT 			nTrgCounterTemp; 		//record the event number temp value

		register UINT 		Record_Point_Valu;
		register UINT * 	Record_Point_Adr;
		register UINT 		MdlList_Now;
		int uAjustData;

		int    			nFileSaveNum=0;    		//the variable use to save error data ,leigk 20051014
	
		Data_Temp=*Move_Point;    				//modify by leigk 20051014
								/*(*Source) is the length CBLT bytes,should divide exactly by 8 	*/
								/*8 is the additional bytes, 16 is 8 bytes of head and 8 bytes of tail		*/
		if((Data_Temp < (8 + 16*nMdlAmount)) || (Data_Temp> g_nMaxCbltBytes)) 
		{
			nFileSaveNum++;
			#ifdef MDC_SAVE_DATA
			RawDataToFile(nFileSaveNum, *nSource, (char *)nSource );
			#endif
			Push2ErrStack(PPCERROR+MDC_FLG+ATOM_ERR+38,0,0,0,0);				/*mdc error 38*/
			DEBUG_OUTPUT(("CBLTed Length out of hope range,=%d bytes\n",*Move_Point - 8) );
			return -1 ;
		}

		if((Data_Temp%8)!=0)                           				 //check wether can be divided by 8
		{
			nFileSaveNum++;
			#ifdef MDC_SAVE_DATA
			RawDataToFile(nFileSaveNum, *nSource, (char *)nSource );
			#endif
			Push2ErrStack(PPCERROR+MDC_FLG+ATOM_ERR+39,0,0,0,0);				/*mdc error 39*/
			DEBUG_OUTPUT( ("data Length can't divide bye 8\n") );
			return -1; 
		}
		Move_Point=Move_Point+(Data_Temp>>2);	              //jump to the data tail			
		for(iii=nMdlAmount-1;iii>=0;iii--)                                //check the nmodule data
		{
			Move_Point=Move_Point-((*(Move_Point-1))>>2);
			if(Move_Point<nSource)                                        //jump to the address before the data segement
			{
				nFileSaveNum++;
				#ifdef MDC_SAVE_DATA
				RawDataToFile(nFileSaveNum, *nSource, (char *)nSource );
				#endif
				Push2ErrStack(PPCERROR+MDC_FLG+ATOM_ERR+40,0,0,0,0);				/*mdc error 40*/
				DEBUG_OUTPUT(("end ascend to head, head address out of range\n"));
				return -1; 
			}
			Per_Module_Point[iii]=Move_Point;                          //save per module data address head to the array
			if(((*Move_Point)>>27)==MdlList[iii])                  //check module position
			{
				Per_Module_Point[iii]++;
			}
			else
			{
				nFileSaveNum++;
				#ifdef MDC_SAVE_DATA
				RawDataToFile(nFileSaveNum, *nSource, (char *)nSource );
				#endif
				Push2ErrStack(PPCERROR+MDC_FLG+GEO_ERR+41,((*Move_Point)>>27),MdlList[iii],0,0);				/*mdc error 41*/
			       DEBUG_OUTPUT(("Hope to module geo  %d ,but now get module %d (front) \n",MdlList[iii],(*Move_Point)>>27));
				return(-1);  
			}
		}
/*-------------------------now begin to change the format----------------------------*/
		
		nTrgCounterTemp=g_nTrgCounter;
		
		Move_Point=nDestination;
		for(iii=1;iii<=nTrgPerInt;iii++)                       		//cycle the trigers per int
		{
			Tar_Per_Tri_Len=0;      							//set per triger 0
			pHead_Rod_Tri=Move_Point;
			Move_Point=Move_Point+1;
			                                              
			g_fwRODHead.level1Id=nTrgCounterTemp+g_nCalibL1OffSet;		 //write ROD head
			if((g_pCrateConfigHeader->unused)&0x1)	//jixl080111
			{
				g_fwRODHead.detectorEventType = (g_fwRODHead.detectorEventType |0x1); // 1-calib;0-no calib
	//			printf("g_fwRODHead.detectorEventType = 0x%x\n",g_fwRODHead.detectorEventType);

			}			
			memcpy((char *)Move_Point,&g_fwRODHead,sizeof(RODHeader));
			Move_Point=Move_Point+(sizeof(RODHeader)>>2);    //end of ROD head
				
 			/*--------Q head dispose --Q data--------------------------------------------------*/	
			  for(kkk=0;kkk<nMdlAmount;kkk++)
			  {
			  	Record_Point_Adr=Per_Module_Point[kkk];
			  	Record_Point_Valu=*Record_Point_Adr;
				if(( (Record_Point_Valu&0xff000000)!=0x03000000)||( (Record_Point_Valu&0x00020000)!=0x20000))	//data head 
				{
					nFileSaveNum++;
					#ifdef MDC_SAVE_DATA
					RawDataToFile(nFileSaveNum, *nSource, (char *)nSource );
					#endif
					Push2ErrStack(PPCERROR+MDC_FLG+ATOM_ERR+42,0,0,0,0);				/*mdc error 42*/
					DEBUG_OUTPUT( ("module %d has QHeaderror\n",MdlList[kkk]) );
					return -1;  
				}
				if( (Record_Point_Valu&0xff)!=(nTrgCounterTemp&0xff))		//triger number
				{
					nFileSaveNum++;
					#ifdef MDC_SAVE_DATA
					RawDataToFile(nFileSaveNum, *nSource, (char *)nSource );
					#endif
					Push2ErrStack(PPCERROR+MDC_FLG+L1_ERR+43,(Record_Point_Valu&0xff),(nTrgCounterTemp&0xff),0,0);		/*mdc error 43*/
					DEBUG_OUTPUT( ("module %d Q data,triger number not match,%d,%d\n",MdlList[kkk],
													Record_Point_Valu&0xff,nTrgCounterTemp&0xff) );
					return(-1);
				}		
				Record_Point_Adr=Record_Point_Adr+1; 
				Record_Point_Valu=*Record_Point_Adr;   
				MdlList_Now=MdlList[kkk];
    			  	while(   (Record_Point_Valu&0xff000000)==0x00)
				{
					Channel_Num=Record_Point_Valu>>19;
					if (0 != g_nConvertTable[MdlList_Now][Channel_Num])
					{
//						*Move_Point=(g_nConvertTable[MdlList_Now][Channel_Num]+(Record_Point_Valu&0x0003ffff));
						
						uAjustData = Record_Point_Valu&0x00007fff;
						if((g_pCrateConfigHeader->unused)&0x1)
						{
								uAjustData=(int)( (g_fAjust_a2[MdlList_Now][Channel_Num]*uAjustData+g_fAjust_a1[MdlList_Now][Channel_Num])*
												uAjustData+g_fAjust_a0[MdlList_Now][Channel_Num]+0.5);
						}
						*Move_Point=(g_nConvertTable[MdlList_Now][Channel_Num]+(uAjustData&0x00007fff)+(Record_Point_Valu&0x00038000));
	
						Tar_Per_Tri_Len=Tar_Per_Tri_Len+1;
						Move_Point=Move_Point+1;
					}
					Record_Point_Adr=Record_Point_Adr+1;
					Record_Point_Valu=*Record_Point_Adr;
				}
			    	Per_Module_Point[kkk]=Record_Point_Adr;
  			 }
			for(kkk=0;kkk<nMdlAmount;kkk++)
			{	
				Record_Point_Adr=Per_Module_Point[kkk];
				Record_Point_Valu=*Record_Point_Adr;
				while(   (Record_Point_Valu&0xff000000)==0x02000000)
				{
						if( (Record_Point_Valu&0x00040000)==0x00000000)   //main board error data
						{
							*Move_Point=0xf8000000+((Record_Point_Valu&0x0000ffff)+((g_nCrateID%16)<<21)+(MdlList[kkk]<<16));	
							// *Move_Point=LONGSWAP((Record_Point_Valu&0x0000ffff)+(10<<20)+(MdlList[kkk]<<16));
						}
						else		//sub board error data
						{
							*Move_Point=0xfa000000+((Record_Point_Valu&0x0000ffff)+((g_nCrateID%16)<<21)+(MdlList[kkk]<<16));
							//*Move_Point=LONGSWAP((3<<24)+(Record_Point_Valu&0x0000ffff)+(10<<20)+(MdlList[kkk]<<16));
						}
						Tar_Per_Tri_Len=Tar_Per_Tri_Len+1;	
						Move_Point=Move_Point+1;
						Record_Point_Adr=Record_Point_Adr+1;
						Record_Point_Valu=*Record_Point_Adr; 
				}	
				Per_Module_Point[kkk]=Record_Point_Adr;	  	
			}	
	
			/*------------------------------end Q data dispose- begin T--------*/	
  		
			for(kkk=0;kkk<nMdlAmount;kkk++)                      //proceed the modules data with same triger number
			{	
				Record_Point_Adr=Per_Module_Point[kkk];
				Record_Point_Valu=*Record_Point_Adr;
				if(( (Record_Point_Valu&0xff000000)!=0x03000000)||( (Record_Point_Valu&0x00020000)!=0x00))
				{
					Push2ErrStack(PPCERROR+MDC_FLG+ATOM_ERR+44,0,0,0,0);				/*mdc error 44*/
					DEBUG_OUTPUT( ("module %d has THeaderror\n",MdlList[kkk]) );
					return(-1);  
				}
				if( (Record_Point_Valu&0xff)!=(nTrgCounterTemp&0xff))
				{
					Push2ErrStack(PPCERROR+MDC_FLG+L1_ERR+45,(Record_Point_Valu&0xff),(nTrgCounterTemp&0xff),0,0);	/*mdc error 45*/
					DEBUG_OUTPUT( ("module %d T data,triger number not match,src:%d,hop:%d\n",MdlList[kkk],
													Record_Point_Valu&0xff,nTrgCounterTemp&0xff) );
					return(-1);  
				}		
				Record_Point_Adr=Record_Point_Adr+1;
				Record_Point_Valu=*Record_Point_Adr;    
				MdlList_Now=MdlList[kkk];
				while(   (Record_Point_Valu&0xff000000)==0x00)
				{
					Channel_Num=Record_Point_Valu>>19;
					if(0 != g_nConvertTable[MdlList_Now][Channel_Num])
					{
						*Move_Point=(g_nConvertTable[MdlList_Now][Channel_Num]+(Record_Point_Valu&0x0003ffff));
				
						Tar_Per_Tri_Len=Tar_Per_Tri_Len+1;						
						Move_Point=Move_Point+1;
					}
					Record_Point_Adr=Record_Point_Adr+1;	
					Record_Point_Valu=*Record_Point_Adr;  
				}
				Per_Module_Point[kkk]=Record_Point_Adr;
			}
				  
			for(kkk=0;kkk<nMdlAmount;kkk++)
			{
				Record_Point_Adr=Per_Module_Point[kkk];
				Record_Point_Valu=*Record_Point_Adr;
				while(   (Record_Point_Valu&0xff000000)==0x02000000)//hptdc error
				{	
					*Move_Point=0xfc000000+((Record_Point_Valu&0x0000ffff)+((g_nCrateID%16)<<21)+(MdlList[kkk]<<16));
					//*Move_Point=LONGSWAP((2<<24)+(Record_Point_Valu&0x0000ffff)+(MdlList[kkk]<<16));
					Tar_Per_Tri_Len=Tar_Per_Tri_Len+1;	
					Move_Point=Move_Point+1;
					Record_Point_Adr=Record_Point_Adr+1;
					Record_Point_Valu=*Record_Point_Adr;
				}
				Per_Module_Point[kkk]=Record_Point_Adr;
			}
			
/*--------------------------------------------------------------------end T data dispose-----------------*/             
			*Move_Point=0x00000000+(g_nMCCT0Latency<<16)+g_nMQTDAC;    		//write status before ROD tail			           
			Move_Point=Move_Point+1;
			
			g_fwRODTail.numberOfDataElements=Tar_Per_Tri_Len; //write ROD tail
			memcpy((char *)Move_Point,&g_fwRODTail,sizeof(RODTrailer));
			Move_Point=Move_Point+(sizeof(RODTrailer)>>2);		//end ROD tail
			  
			Tar_Per_Tri_Len=Tar_Per_Tri_Len+13	;
			*pHead_Rod_Tri=Tar_Per_Tri_Len<<2;
			Total_len_Return=Total_len_Return+Tar_Per_Tri_Len;
			
			nTrgCounterTemp=nTrgCounterTemp+1;	
		}
		
		for(kkk=0;kkk<nMdlAmount;kkk++)		//dispose the insert blank data*/
		{
			if( ((*Per_Module_Point[kkk])&0xff000000)==0x01000000)
			{
				Per_Module_Point[kkk]=Per_Module_Point[kkk]+1;
			}
		}
		for(kkk=0;kkk<nMdlAmount;kkk++)		//judge the GEO of data tail 
		{
			if( ((*Per_Module_Point[kkk])>>27)!=MdlList[kkk])
			{
				nFileSaveNum++;
				#ifdef MDC_SAVE_DATA
				RawDataToFile(nFileSaveNum, *nSource, (char *)nSource );
				#endif
				Push2ErrStack(PPCERROR+MDC_FLG+GEO_ERR+46,((*Per_Module_Point[kkk])>>27),MdlList[kkk],0,0);		/*mdc error 46*/
				DEBUG_OUTPUT( ("module GEO not match (data tail),src:%d,hop:%d\n",((*Per_Module_Point[kkk])>>27),MdlList[kkk]) );
				return(-1); 
			}
		}
	g_nTrgCounter=nTrgCounterTemp;  //leigkleigk060728 renew the processed event number
	*p_nReturn=(Total_len_Return+nTrgPerInt)<<2;
	return(0);
	
}


