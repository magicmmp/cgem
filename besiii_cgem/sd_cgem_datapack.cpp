
#include "vxWorks.h"

#include "fragheaders.h"
#include "fw_roppc.h"
#include "fw_runtasks.h"
#include "fw_dcgvas.h"
#include "fw_define.h"
#include "fw_staterrspt.h"
#include "fw_configinfo.h"

#include "sd_cgem_datapack.h"


int cgemDataPackCore(UINT *nDestination ,UINT *nSource,int *nBytes2BeSend)
{
	register UINT *p_uMove=nSource;
	register UINT *p_uDesti=nDestination;
	register UINT *p_PointEveHead;
        int 	 nPerEventLen=0;
        int	 nRecordTrgNum;
	        nRecordTrgNum=g_nTrgCounter;
                p_PointEveHead=p_uDesti;
		p_uDesti++;
		g_fwRODHead.level1Id=nRecordTrgNum;
		memcpy((char *)p_uDesti,&g_fwRODHead,sizeof(RODHeader));
		p_uDesti=p_uDesti+(sizeof(RODHeader)>>2); /**p_uDesti is int pointer   */
		nPerEventLen=nPerEventLen+sizeof(RODHeader)+4;
		memcpy((char *)p_uDesti,(char*)p_uMove,*p_uMove) ;
        p_uDesti=p_uDesti+((*p_uMove)>>2);
        nPerEventLen=nPerEventLen+*p_uMove;
		*p_uDesti=0;   /*status*/
		p_uDesti++;
		g_fwRODTail.numberOfDataElements=nPerEventLen;
		memcpy((char *)p_uDesti,&g_fwRODTail,sizeof(RODTrailer));
		p_uDesti=p_uDesti+(sizeof(RODTrailer)>>2);
		nPerEventLen+=14;
		*p_PointEveHead=nPerEventLen;  /*event head*/
		nRecordTrgNum++;
	   g_nTrgCounter=nRecordTrgNum;
	   *nBytes2BeSend=nPerEventLen;
	return 0;
}

