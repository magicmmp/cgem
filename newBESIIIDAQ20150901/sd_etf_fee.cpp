/*sd_empty.cpp, test an empty system*/

#include "vxWorks.h"
#include "stdio.h"
#include "stdio.h"
#include "intLib.h"

#include "5100_test.h"
#include "fw_roppc.h"
#include "fw_dcgvas.h"
#include "fw_define.h"
#include "fw_configinfo.h"
#include "fw_staterrspt.h"

#include "sd_etf.h"
#include "sd_etf_fee.h"
//#include "sd_etf_hptdc_config.h"


int ETFConfigFeeBaseAddr()
{
	UINT nGeo;
	UINT nadr;
	UINT uRead;
	
	//etf fee use csr space to config A32D32 space on each board,for cblt or other control
/*	for(int nModuIndx=0;nModuIndx<g_ETFDBInfo.etf_tdig_sum;nModuIndx++)			//set base addr ,use a24d32
	{
		nGeo=g_ETFDBInfo.etf_TDIG_header[nModuIndx]->geo;
		WriteToVme8((nGeo<<19)+0x7ffd8+0x3, VCSR, (nGeo&0x0f)<<4);  //nGeo low 4 bit write to one register
		WriteToVme8((nGeo<<19)+0x7ffdc+0x3, VCSR, (nGeo&0xf0)>>4);	//nGeo high 4 bit write to another register
		printf("write:0x%08x to address: 0x%08x\n",nGeo<<4,(nGeo<<19)+0x7ffdc+0x3);
	}
*/
	for(int nModuIndx=0;nModuIndx<g_ETFDBInfo.etf_tdig_sum;nModuIndx ++)//add for check FEE in slot wether match  config infor
	{
			nGeo=g_ETFDBInfo.etf_TDIG_header[nModuIndx]->geo;
			nadr=(nGeo<<20);									//after set fee base addr,use a32d32 access		
			uRead=0;
			ReadFrmVme32(nadr,VA32,&uRead);
			if(uRead==0xffffffff)
			{
				printf("Config FEE BaseAddr error, Slot %d may have no module,pls chk config info\n",nGeo);
				Push2ErrStack(PPCWARNING+ETF_FLG+ATOM_ERR+45,0,0,0,0);		/*etf error 45*/
				//return -1;
			}
	}
	return 0;
}

/*
int ETFCheckFeeSerialNum()
{
	unsigned int SerialRead=0;
	for(int nIndx=0;nIndx<g_ETFDBInfo.etf_tdig_sum;nIndx ++)
	{
			unsigned int nGeo=g_ETFDBInfo.etf_TDIG_header[nIndx]->geo;
			unsigned int nadr=(nGeo<<20);
			SerialRead=0;
			//read SerialRead  Board ID
			ReadFrmVme32(nadr+0x28,VA32,&SerialRead);
			SerialRead=SerialRead&0x3f;
			if(SerialRead!=g_ETFDBInfo.etf_TDIG_header[nIndx]->serialNum)
			{
				printf("BoardID:%d, ID in DB: %d\n",SerialRead,g_ETFDBInfo.etf_TDIG_header[nIndx]->serialNum);
			}
	}
	return 0;
}
*/

/*
int  ETFConfigFeeWorkMode()
{
	//workmode==0 : Run mode ;workmode==1 Self Test Mode
	UINT32 nadr;
	UINT32 FeeConfigureDataTmp;

	for(int nModuIndx=0;nModuIndx<g_ETFDBInfo.etf_tdig_sum;nModuIndx++)  
	{
		UINT nGeo=g_ETFDBInfo.etf_TDIG_header[nModuIndx]->geo;
		nadr=(nGeo<<20);

		switch(g_nRunMode)
		{
			case NOMALMODE:
				if(g_ETFDBInfo.etf_ppc_header->NormModeSelfL1)
				{
					FeeConfigureDataTmp=0x84440;
				}
				else
				{
					FeeConfigureDataTmp=0x4440;//endcap	//no INL:0x4400;INL:0x4440
				}
				break;
			case PEDMODE:
			case MONITOR:
				FeeConfigureDataTmp=0x4440;//end_cap
				break;
			case CALIBRATION:
				FeeConfigureDataTmp=0x84440;
				break;
			case BASELINE:
			case WAVEMODE:		
				break ;
			default:
				Push2ErrStack(PPCFATAL+ETF_FLG+ATOM_ERR+9,0,0,0,0);		//etf error 9
				break ;
		}
	
		if (-1 == WriteToVme32(nadr, VA32, FeeConfigureDataTmp)) 
		{
			printf(">>VME:: ERROR!! VA32 Write 0x%08x to 0x%08x with Errors!\n",FeeConfigureDataTmp,nadr); 
		} 
		else 
		{
			//printf(">>VME:: Data Pack Mode : VA32 Write 0x%08x to 0x%08x Successfully!\n",FeeConfigureDataTmp,nadr); 
		}
		taskDelay(1);
	}
	return 0;
}
*/


