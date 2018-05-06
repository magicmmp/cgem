/*****sd_trg_actions.cpp*****/
#include "5100_test.h"
#include "vxWorks.h"
#include "rng_dma.h"
#include "intLib.h"
#include "sysLib.h"
#include "sockLib.h"
#include "taskLib.h"
#include "msgQLib.h"

#include "fw_configinfo.h"
#include "fw_dcgvas.h"
#include "fragheaders.h"
#include "fw_filefunc.h"
#include "fw_tcpfunc.h"
#include "fw_staterrspt.h"
#include "fw_actions.h"
#include "fw_runtasks.h"

#include "fw_define.h"

#include "sd_trg.h"
#include "sd_trg_hw.h"
#include "sd_trg_runtasks.h"
#include "sd_trg_actions.h"

int	TRGInitializedLOAD 		(int nArg) 
{
	char * pcMdlHeader ;
	int nMdlType;
	int nMdlLen;
	int nMdlIdx = 0 ;
	
	sysIntDisable(TRG_INT_LEVEL) ;
	/**/
	if((g_nCrateID<0xa0)||(g_nCrateID>=0xc0))
	{
		DEBUG_OUTPUT(("TRG CrateID out of 0xa0-0xc0\n")) ;
		Push2ErrStack(PPCFATAL+TRG_FLG+ATOM_ERR+1,0,0,0,0);		/*trg error 1*/
		return -1;
	}
	

	pcMdlHeader= ((char*)g_pCrateConfigHeader) + g_pCrateConfigHeader->crate_len ;
	g_TRGDBInfo.trg_ppc_sum=0;
	g_TRGDBInfo.trg_eacc_sum=0;
	g_TRGDBInfo.trg_saf_sum=0;
	g_TRGDBInfo.trg_gtl_sum=0;
	g_TRGDBInfo.trg_troc_sum=0;
	g_TRGDBInfo.trg_tdc_sum=0;
	g_TRGDBInfo.trg_fctl_sum=0;
	g_TRGDBInfo.trg_tfcf_sum=0;
	g_TRGDBInfo.trg_clkf_sum=0;
	g_TRGDBInfo.trg_tof_sum=0;
	g_TRGDBInfo.trg_tkf_sum=0;
	g_TRGDBInfo.trg_mutl_sum=0;
	g_TRGDBInfo.trg_stkc_sum=0;
	g_TRGDBInfo.trg_ltkc_sum=0;
	g_TRGDBInfo.trg_basg_sum=0;
	g_TRGDBInfo.trg_btm_sum=0;
	g_TRGDBInfo.trg_etm_sum=0;
	g_TRGDBInfo.trg_ets_sum=0;
	TrgETSCbltEnable = 0;
	g_TRGDBInfo.trg_etf_sum = 0;


	unsigned int iSAF= 0; //jixl
	unsigned int iTKF= 0;

	for(int idx=0;idx<16;idx++)uTrgCbltModGeo[idx]=0; 		//jixl 071130
	nTrgCbltModNum = 0;

	
	for(nMdlIdx=0;nMdlIdx<g_nMdlSum;nMdlIdx++) 
	{
		nMdlType =  (UINT)(*(UINT*)(pcMdlHeader + 4)) ;
		DEBUG_OUTPUT(("Module Type: 0x%x\n", nMdlType) ) ;
		switch(nMdlType)
		{
			case TRG_PPC:
			{
				g_TRGDBInfo.trg_ppc_header=(struct GE_ppc *)pcMdlHeader;	

				Show_ppc(g_TRGDBInfo.trg_ppc_header);
				
				g_TRGDBInfo.trg_ppc_sum++;
				if(g_TRGDBInfo.trg_ppc_sum>1)
				{
					DEBUG_OUTPUT(("PPC sum>1,trg_ppc_sum=%d\n",g_TRGDBInfo.trg_ppc_sum ) );
					Push2ErrStack(PPCFATAL+TRG_FLG+ATOM_ERR+2,0,0,0,0);		/*trg error 2*/
					return -1;
				}
				break;
			}
			case TRG_EACC:
			{
				g_TRGDBInfo.trg_eacc_header=(struct trg_eacc *)pcMdlHeader;
				Show_trg_eacc(g_TRGDBInfo.trg_eacc_header);
				
				TrgEaccBSEL = (g_TRGDBInfo.trg_eacc_header->Position>>19)&0x1f;
				TrgEaccGeo = (g_TRGDBInfo.trg_eacc_header->Position&0x1ff);
				TrgEaccSYNCheck = (g_TRGDBInfo.trg_eacc_header->Position>>15)&0x1;
				if(TrgEaccGeo<256)
				{
					DEBUG_OUTPUT(("A EACC module config disable\n") );
					Push2ErrStack(PPCWARNING+TRG_FLG+ATOM_ERR+3,0,0,0,0);		/*trg error 3*/
				}
				if(TrgEaccGeo>=256)
				{
					TrgEaccGeo&=0xff;
					g_TRGDBInfo.trg_eacc_sum++;
					if(TrgEaccGeo!=0)
					{
						uTrgCbltModGeo[nTrgCbltModNum] = TrgEaccGeo;
						nTrgCbltModNum++;
					}
					
				}
				break;
			}
			case TRG_SAF:
			{
				g_TRGDBInfo.trg_saf_header[iSAF]=(struct trg_saf *)pcMdlHeader;
				Show_trg_saf(g_TRGDBInfo.trg_saf_header[iSAF]);
				
				TrgSAFBSEL[iSAF] = (g_TRGDBInfo.trg_saf_header[iSAF]->Position>>19)&0x1f;
				TrgSAFGeo[iSAF] = (g_TRGDBInfo.trg_saf_header[iSAF]->Position&0x1ff);
				if(TrgSAFGeo[iSAF]<256)
				{
					DEBUG_OUTPUT(("SAF module [%d] config disable\n",iSAF) );
					Push2ErrStack(PPCWARNING+TRG_FLG+ATOM_ERR+4,0,0,0,0);		/*trg error 4*/
				}
				if(TrgSAFGeo[iSAF]>=256)
				{
					TrgSAFGeo[iSAF]&=0xff;
					g_TRGDBInfo.trg_saf_sum++;
					if(TrgSAFGeo[iSAF]!=0)
					{
						uTrgCbltModGeo[nTrgCbltModNum] =TrgSAFGeo[iSAF];
						 nTrgCbltModNum++;
					}
					iSAF++;
				}
				break;
			}
			case TRG_GTL:
			{
				g_TRGDBInfo.trg_gtl_header=(struct trg_gtl *)pcMdlHeader;
				Show_trg_gtl(g_TRGDBInfo.trg_gtl_header);
				
				TrgGTLBSEL = (g_TRGDBInfo.trg_gtl_header->Position>>19)&0x1f;
				TrgGTLGeo = (g_TRGDBInfo.trg_gtl_header->Position&0x1ff);
				if(TrgGTLGeo<256)
				{
					DEBUG_OUTPUT(("A GTL module config disable\n") );
					Push2ErrStack(PPCWARNING+TRG_FLG+ATOM_ERR+5,0,0,0,0);		/*trg error 5*/
				}
				if(TrgGTLGeo>=256)
				{
					TrgGTLGeo&=0xff;
					g_TRGDBInfo.trg_gtl_sum++;

					if(TrgGTLGeo!=0)
					{
						uTrgCbltModGeo[nTrgCbltModNum] =TrgGTLGeo;
						 nTrgCbltModNum++;
					}
				}
				break;
			}
			case TRG_TROC:
			{
				g_TRGDBInfo.trg_troc_header=(struct trg_troc *)pcMdlHeader;
				Show_trg_troc(g_TRGDBInfo.trg_troc_header);
				
				TrgTROCBSEL = (g_TRGDBInfo.trg_troc_header->Position>>19)&0x1f;
				TrgTROCGeo = (g_TRGDBInfo.trg_troc_header->Position&0x1ff);
				if(TrgTROCGeo<256)
				{
					DEBUG_OUTPUT(("A TROC module config disable\n") );
					Push2ErrStack(PPCWARNING+TRG_FLG+ATOM_ERR+6,0,0,0,0);		/*trg error 6*/
				}
				if(TrgTROCGeo>=256)
				{
					TrgTROCGeo&=0xff;
					g_TRGDBInfo.trg_troc_sum++;
					
					if(TrgTROCGeo!=0)
					{
						uTrgCbltModGeo[nTrgCbltModNum] =TrgTROCGeo;
						 nTrgCbltModNum++;
					}					
				}
				break;
			}
			case TRG_FCTL:
			{
				g_TRGDBInfo.trg_fctl_header=(struct trg_fctl *)pcMdlHeader;
				Show_trg_fctl(g_TRGDBInfo.trg_fctl_header);
				
				TrgFCTLBSEL = (g_TRGDBInfo.trg_fctl_header->Position>>19)&0x1f;
				TrgFCTLGeo = (g_TRGDBInfo.trg_fctl_header->Position&0x1ff);
				if(TrgFCTLGeo<256)
				{
					DEBUG_OUTPUT(("A FCTL module config disable\n") );
					Push2ErrStack(PPCWARNING+TRG_FLG+ATOM_ERR+7,0,0,0,0);		/*trg error 7*/
				}
				if(TrgFCTLGeo>=256)
				{
					TrgFCTLGeo&=0xff;
					g_TRGDBInfo.trg_fctl_sum++;

					if(TrgFCTLGeo!=0)
					{
						uTrgCbltModGeo[nTrgCbltModNum] = TrgFCTLGeo;
						 nTrgCbltModNum++;
					}
				}
				
				break;
			}
			case TRG_TFCF:
			{
				g_TRGDBInfo.trg_tfcf_header=(struct trg_tfcf *)pcMdlHeader;
				Show_trg_tfcf(g_TRGDBInfo.trg_tfcf_header);
				
				TrgTFCFBSEL = (g_TRGDBInfo.trg_tfcf_header->Position>>19)&0x1f;
				TrgTFCFGeo = (g_TRGDBInfo.trg_tfcf_header->Position&0x1ff);
				if(TrgTFCFGeo<256)
				{
					DEBUG_OUTPUT(("A TFCF module config disable\n") );
					Push2ErrStack(PPCWARNING+TRG_FLG+ATOM_ERR+8,0,0,0,0);		/*trg error 8*/
				}
				if(TrgTFCFGeo>=256)
				{
					TrgTFCFGeo&=0xff;
					g_TRGDBInfo.trg_tfcf_sum++;

					if(TrgTFCFGeo!=0)
					{
						uTrgCbltModGeo[nTrgCbltModNum] =TrgTFCFGeo;
						 nTrgCbltModNum++;
					}
				}
				break;
			}
			case TRG_CLKF:
			{
				g_TRGDBInfo.trg_clkf_header=(struct trg_clkf *)pcMdlHeader;
				Show_trg_clkf(g_TRGDBInfo.trg_clkf_header);
				
				TrgCLKFBSEL = (g_TRGDBInfo.trg_clkf_header->Position>>19)&0x1f;
				TrgCLKFGeo = (g_TRGDBInfo.trg_clkf_header->Position&0x1ff);
				
				if(TrgCLKFGeo<256)
				{
					DEBUG_OUTPUT(("A CLKF module config disable\n") );
					Push2ErrStack(PPCWARNING+TRG_FLG+ATOM_ERR+9,0,0,0,0);		/*trg error 9*/
				}
				if(TrgCLKFGeo>=256)
				{
					TrgCLKFGeo&=0xff;
					g_TRGDBInfo.trg_clkf_sum++;

					if(TrgCLKFGeo!=0)
					{
						uTrgCbltModGeo[nTrgCbltModNum] =TrgCLKFGeo;
						 nTrgCbltModNum++;
					}					
				}
				break;
			}
			case TRG_TDC:
			{
				g_TRGDBInfo.trg_tdc_header=(struct trg_tdc *)pcMdlHeader;
				Show_trg_tdc(g_TRGDBInfo.trg_tdc_header);
				
				TrgTDCBSEL = (g_TRGDBInfo.trg_tdc_header->Position>>19)&0x1f;
				printf("TrgTDCBSEL = 0x%x\n",TrgTDCBSEL);
				TrgTDCGeo = (g_TRGDBInfo.trg_tdc_header->Position&0x1ff);
				uTrgTDCCbltAdrs =  (g_TRGDBInfo.trg_tdc_header->Position&0xffff0000);
				if(TrgTDCGeo<256)
				{
					DEBUG_OUTPUT(("A TDC module config disable\n") );
					Push2ErrStack(PPCWARNING+TRG_FLG+ATOM_ERR+10,0,0,0,0);		/*trg error 10*/
				}
				if(TrgTDCGeo>=256)
				{
					TrgTDCGeo&=0xff;
					g_TRGDBInfo.trg_tdc_sum++;

					if(TrgTDCGeo!=0)
					{
						uTrgCbltModGeo[nTrgCbltModNum] = TrgTDCGeo;
						 nTrgCbltModNum++;
					}
				}
				break;
			}
			case TRG_TOF:
			{
				g_TRGDBInfo.trg_tof_header=(struct trg_tof *)pcMdlHeader;
				Show_trg_tof(g_TRGDBInfo.trg_tof_header);
				
				TrgTOFBSEL = (g_TRGDBInfo.trg_tof_header->Position>>19)&0x1ff;
				TrgTOFGeo = (g_TRGDBInfo.trg_tof_header->Position&0x1ff);
				TrgTOFCBLT = (g_TRGDBInfo.trg_tof_header->Position>>9)&0x1;
				TrgTOFSYNCheck = (g_TRGDBInfo.trg_tof_header->Position>>15)&0x1;
				if(TrgTOFGeo<256)
				{
					DEBUG_OUTPUT(("TRGTOF module config disable\n") );
					Push2ErrStack(PPCWARNING+TRG_FLG+ATOM_ERR+11,0,0,0,0);		/*trg error 11*/
				}
				if(TrgTOFGeo>=256)
				{
					TrgTOFGeo&=0xff;
					g_TRGDBInfo.trg_tof_sum++;

					if(TrgTOFGeo!=0)
					{
						uTrgCbltModGeo[nTrgCbltModNum] = TrgTOFGeo;
						 nTrgCbltModNum++;
					}
				}
				break;
			}
			case TRG_ETF:
			{
				g_TRGDBInfo.trg_etf_header=(struct trg_etf *)pcMdlHeader;
				Show_trg_etf(g_TRGDBInfo.trg_etf_header);
				
				TrgETFBSEL = (g_TRGDBInfo.trg_etf_header->Position>>19)&0x1ff;
				TrgETFGeo = (g_TRGDBInfo.trg_etf_header->Position&0x1ff);

				if(TrgETFGeo<256)
				{
					DEBUG_OUTPUT(("TRGETF module config disable\n") );
					Push2ErrStack(PPCWARNING+TRG_FLG+ATOM_ERR+11,0,0,0,0);		/*trg error 11*/
				}
				else
				{
					TrgETFGeo&=0xff;
					g_TRGDBInfo.trg_etf_sum++;

					if(TrgETFGeo!=0)
					{
						uTrgCbltModGeo[nTrgCbltModNum] = TrgETFGeo;
						 nTrgCbltModNum++;
					}
				}
				break;
			}
						
			case TRG_TKF:
			{
				g_TRGDBInfo.trg_tkf_header[iTKF]=(struct trg_tkf *)pcMdlHeader;
				Show_trg_tkf(g_TRGDBInfo.trg_tkf_header[iTKF]);
				
				TrgTKFBSEL[iTKF] = (g_TRGDBInfo.trg_tkf_header[iTKF]->Position>>19)&0x1f;
				TrgTKFGeo[iTKF] = (g_TRGDBInfo.trg_tkf_header[iTKF]->Position&0x1ff);
				TrgTKFSYNCheck[iTKF] = (g_TRGDBInfo.trg_tkf_header[iTKF]->Position>>15)&0x1;
				if(TrgTKFGeo[iTKF] <256)
				{
					DEBUG_OUTPUT(("TRGTKF module config disable\n") );
					Push2ErrStack(PPCWARNING+TRG_FLG+ATOM_ERR+12,0,0,0,0);		/*trg error 12*/
				}
				if(TrgTKFGeo[iTKF] >=256)
				{
					TrgTKFGeo[iTKF] &=0xff;
					g_TRGDBInfo.trg_tkf_sum++;

					if(TrgTKFGeo[iTKF] !=0)
					{
						uTrgCbltModGeo[nTrgCbltModNum] = TrgTKFGeo[iTKF] ;
						 nTrgCbltModNum++;
					}
					iTKF++;
				}
				break;
			}	
			
			case TRG_MUTL:
			{
				g_TRGDBInfo.trg_mutl_header=(struct trg_mutl *)pcMdlHeader;
				Show_trg_mutl(g_TRGDBInfo.trg_mutl_header);
				
				TrgMUTLBSEL = (g_TRGDBInfo.trg_mutl_header->Position>>19)&0x1f;
				TrgMUTLGeo = (g_TRGDBInfo.trg_mutl_header->Position&0x1ff);
				if(TrgMUTLGeo <256)
				{
					DEBUG_OUTPUT(("TRGMUTL module config disable\n") );
					Push2ErrStack(PPCWARNING+TRG_FLG+ATOM_ERR+13,0,0,0,0);		/*trg error 13*/
				}
				if(TrgMUTLGeo >=256)
				{
					TrgMUTLGeo &=0xff;
					g_TRGDBInfo.trg_mutl_sum++;

					if(TrgMUTLGeo !=0)
					{
						uTrgCbltModGeo[nTrgCbltModNum] = TrgMUTLGeo ;
						 nTrgCbltModNum++;
					}
				}
				break;
			}	


			case TRG_LTKC:
			{
				g_TRGDBInfo.trg_ltkc_header=(struct trg_tkc *)pcMdlHeader;
				Show_trg_tkc(g_TRGDBInfo.trg_ltkc_header);
				TrgLTKCBSEL = (g_TRGDBInfo.trg_ltkc_header->Position>>19)&0x1ff;
				TrgLTKCGeo = (g_TRGDBInfo.trg_ltkc_header->Position&0x1ff);
				if(TrgLTKCGeo<256)
				{
					DEBUG_OUTPUT(("TRGLTKC module config disable\n") );
					Push2ErrStack(PPCWARNING+TRG_FLG+ATOM_ERR+14,0,0,0,0);		/*trg error 14*/
				}
				if(TrgLTKCGeo>=256)
				{
					TrgLTKCGeo&=0xff;
					g_TRGDBInfo.trg_ltkc_sum++;

					if(TrgLTKCGeo!=0)
					{
						uTrgCbltModGeo[nTrgCbltModNum] = TrgLTKCGeo;
						 nTrgCbltModNum++;
					}
				}
				break;
			}

			case TRG_STKC:
			{
				g_TRGDBInfo.trg_stkc_header=(struct trg_tkc *)pcMdlHeader;
				Show_trg_tkc(g_TRGDBInfo.trg_stkc_header);
				TrgSTKCBSEL = (g_TRGDBInfo.trg_stkc_header->Position>>19)&0x1ff;
				TrgSTKCGeo = (g_TRGDBInfo.trg_stkc_header->Position&0x1ff);
				if(TrgSTKCGeo<256)
				{
					DEBUG_OUTPUT(("TRGSTKC module config disable\n") );
					Push2ErrStack(PPCWARNING+TRG_FLG+ATOM_ERR+15,0,0,0,0);		/*trg error 15*/
				}
				if(TrgSTKCGeo>=256)
				{
					TrgSTKCGeo&=0xff;
					g_TRGDBInfo.trg_stkc_sum++;

					if(TrgSTKCGeo!=0)
					{
						uTrgCbltModGeo[nTrgCbltModNum] = TrgSTKCGeo;
						 nTrgCbltModNum++;
					}
				}
				break;
			}
			
			case TRG_BASG:
			{
				g_TRGDBInfo.trg_basg_header=(struct trg_basg *)pcMdlHeader;
				Show_trg_basg(g_TRGDBInfo.trg_basg_header);
				
				TrgBASGBSEL = (g_TRGDBInfo.trg_basg_header->Position>>19)&0x1f;
				TrgBASGGeo = (g_TRGDBInfo.trg_basg_header->Position&0x1ff);
				if(TrgBASGGeo<256)
				{
					DEBUG_OUTPUT(("A BASG module config disable\n") );
					Push2ErrStack(PPCWARNING+TRG_FLG+ATOM_ERR+114,0,0,0,0);		/*trg error 114*/
				}
				if(TrgBASGGeo>=256)
				{
					TrgBASGGeo&=0xff;
					g_TRGDBInfo.trg_basg_sum++;
					if(TrgBASGGeo!=0)
					{
						uTrgCbltModGeo[nTrgCbltModNum] = TrgBASGGeo;
						nTrgCbltModNum++;
					}
					
				}
				break;
			}


			case TRG_BTM:
			{
				g_TRGDBInfo.trg_btm_header=(struct trg_btm *)pcMdlHeader;
				Show_trg_btm(g_TRGDBInfo.trg_btm_header);
				TrgBTMBSEL = (g_TRGDBInfo.trg_btm_header->Position>>19)&0x1ff;
				TrgBTMGeo = (g_TRGDBInfo.trg_btm_header->Position&0x1ff);
				if(TrgBTMGeo<256)
				{
					DEBUG_OUTPUT(("TRGBTM module config disable\n") );
					Push2ErrStack(PPCWARNING+TRG_FLG+ATOM_ERR+150,0,0,0,0);		/*trg error 150*/
				}
				if(TrgBTMGeo>=256)
				{
					TrgBTMGeo&=0xff;
					g_TRGDBInfo.trg_btm_sum++;

					if(TrgBTMGeo!=0)
					{
						uTrgCbltModGeo[nTrgCbltModNum] = TrgBTMGeo;
						 nTrgCbltModNum++;
					}
				}
				break;
			}

			case TRG_ETM:
			{
				g_TRGDBInfo.trg_etm_header=(struct trg_etm *)pcMdlHeader;
				Show_trg_etm(g_TRGDBInfo.trg_etm_header);
				TrgETMBSEL = (g_TRGDBInfo.trg_etm_header->Position>>19)&0x1ff;
				TrgETMGeo = (g_TRGDBInfo.trg_etm_header->Position&0x1ff);
				if(TrgETMGeo<256)
				{
					DEBUG_OUTPUT(("TRGETM module config disable\n") );
					Push2ErrStack(PPCWARNING+TRG_FLG+ATOM_ERR+151,0,0,0,0);		/*trg error 151*/
				}
				if(TrgETMGeo>=256)
				{
					TrgETMGeo&=0xff;
					g_TRGDBInfo.trg_etm_sum++;

					if(TrgETMGeo!=0)
					{
						uTrgCbltModGeo[nTrgCbltModNum] = TrgETMGeo;
						 nTrgCbltModNum++;
					}
				}
				break;
			}			

			case TRG_ETS:
			{
				g_TRGDBInfo.trg_ets_header=(struct trg_ets *)pcMdlHeader;
				Show_trg_ets(g_TRGDBInfo.trg_ets_header);
//				g_TRGDBInfo.trg_ets_header->Position = 0xf00310;
				TrgETSBSEL = (g_TRGDBInfo.trg_ets_header->Position>>19)&0x1ff;
				TrgETSGeo = (g_TRGDBInfo.trg_ets_header->Position&0x1ff);	
				TrgETSCbltEnable = (g_TRGDBInfo.trg_ets_header->Position>>9)&0x1;
				if(TrgETSGeo<256)
				{
					DEBUG_OUTPUT(("TRGETS module config disable\n") );
					Push2ErrStack(PPCWARNING+TRG_FLG+ATOM_ERR+159,0,0,0,0);		//trg error 159
				}
				if(TrgETSGeo>=256)
				{
					TrgETSGeo&=0xff;
					g_TRGDBInfo.trg_ets_sum++;
				}
				break;
			}
			
			default :
			{
				DEBUG_OUTPUT( ("Unknown module type\n") );
				Push2ErrStack(PPCFATAL+TRG_FLG+ATOM_ERR+16,0,0,0,0);		//trg error 16
				return (-1);
			}
		}

		
		pcMdlHeader = pcMdlHeader + (UINT)(*(UINT*)pcMdlHeader) ;
		
	}

	/*add ETF MODULE HERE*/
	
	if(g_nCrateID==161)
	{
		g_TRGDBInfo.trg_etf_sum = 1;
		TrgETFBSEL = 0x9;
		TrgETFGeo = 13;
		uTrgCbltModGeo[nTrgCbltModNum] = TrgETFGeo;
		nTrgCbltModNum++;
	}


	//jixl 071130 
	UINT uTmpGeo=0;
	printf("nTrgCbltModNum=%d\n",nTrgCbltModNum);	
	for(int nGeoi=0; nGeoi<nTrgCbltModNum-1;nGeoi++)
	{
		for(int nGeoj=nGeoi+1;nGeoj<nTrgCbltModNum;nGeoj++)
		{
			if(uTrgCbltModGeo[nGeoi] > uTrgCbltModGeo[nGeoj])
			{
				uTmpGeo = uTrgCbltModGeo[nGeoi];
				uTrgCbltModGeo[nGeoi] = uTrgCbltModGeo[nGeoj];
				uTrgCbltModGeo[nGeoj] = uTmpGeo;
			}
		}
	}
	for(int iTemp=0;iTemp<nTrgCbltModNum;iTemp++)
	{
		printf("uTrgCbltModGeo[%d] = %d\n",iTemp,uTrgCbltModGeo[iTemp]);
	}

	if ( g_nPPCIDOfSRH!= g_TRGDBInfo.trg_ppc_header->PPC_id)
	{
		DEBUG_OUTPUT( ("PPC id not equal SRH,ppcid=%d\n",g_TRGDBInfo.trg_ppc_header->PPC_id) );
		Push2ErrStack(PPCFATAL+TRG_FLG+ATOM_ERR+17,0,0,0,0);		/*trg error 17*/
		return -1 ;
	}
	
	/*queue some config infor,also other*/
	
	switch(g_nRunMode)
	{
		case NOMALMODE:
		case PEDMODE:
		case CALIBRATION:
		case MONITOR:
		case BASELINE:
		case WAVEMODE:	
			if(TRGConfInterrupt()==-1)
			{
				//config interrupt error\0");
				Push2ErrStack(PPCFATAL+TRG_FLG+ATOM_ERR+18,0,0,0,0);		/*trg error 18*/
				return -1;
			}
			break ;
		default:
			//unknown running mode 1\0");
			Push2ErrStack(PPCFATAL+TRG_FLG+ATOM_ERR+19,0,0,0,0);		/*trg error 19*/
			return -1;
	}
	
	printf("TRGInitializedLOAD OK\n") ;
	return 0;
}
int  	TRGLoadedPRCF 			(int nArg) 
{
	if(nTrgCbltModNum>0)
	{
		unsigned short nDataPort = g_TRGDBInfo.trg_ppc_header->ROS_port ;
		unsigned int 	 nRemoteIP = g_TRGDBInfo.trg_ppc_header->ROS_ip;
		DEBUG_OUTPUT(("ROS IP = %x, PORT = %d.\n",nRemoteIP, nDataPort)) ;
		if(DataSockConnect(nRemoteIP,nDataPort)==-1)
		{
			DEBUG_OUTPUT(("connect data socket error\n"));
			Push2ErrStack(PPCFATAL+TRG_FLG+ATOM_ERR+20,0,0,0,0);		/*trg error 20*/
			CloseDataSocket();
			return -1; 		}
	}

	if(g_TRGDBInfo.trg_clkf_sum>0)
	{
		if(-1==TRG_CLKF_preconf())
		{
			DEBUG_OUTPUT(("preconf clock error\n"));
			Push2ErrStack(PPCFATAL+TRG_FLG+ATOM_ERR+21,0,0,0,0);		/*trg error 21*/
			return -1;
		}
	}

	if(g_TRGDBInfo.trg_eacc_sum>0)
	{
		if(-1==TRG_EACC_preconf())
		{
			DEBUG_OUTPUT(("preconf eacc error\n"));
			Push2ErrStack(PPCFATAL+TRG_FLG+ATOM_ERR+22,0,0,0,0);		/*trg error 22*/
			return -1;
		}
	}

	if(g_TRGDBInfo.trg_tkf_sum>0)
	{
		if(-1==TRG_TKF_preconf() )
		{
			DEBUG_OUTPUT(("preconfig tkf error.\n"));
			Push2ErrStack(PPCFATAL+TRG_FLG+ATOM_ERR+23,0,0,0,0);		/*trg error 23*/
			return -1;
		}
	}	

	if(g_TRGDBInfo.trg_gtl_sum>0)
	{
		if(-1==TRG_GTL_preconf() )
		{
			DEBUG_OUTPUT(("preconfig gtl error.\n"));
			Push2ErrStack(PPCFATAL+TRG_FLG+ATOM_ERR+24,0,0,0,0);		/*trg error 24*/
			return -1;
		}
	}

	if(g_TRGDBInfo.trg_mutl_sum>0)
	{
		if(-1==TRG_MUTL_preconf() )
		{
			DEBUG_OUTPUT(("preconfig mutl error.\n"));
			Push2ErrStack(PPCFATAL+TRG_FLG+ATOM_ERR+25,0,0,0,0);		/*trg error 25*/
			return -1;
		}
	}	
	printf("TRGLoadedPRCF OK\n") ;
	return 0;
}
int  	TRGPreconfigedCONF 	(int nArg) 
{
	switch(g_nRunMode)
	{
		case NOMALMODE:
			if((g_nTrgPerInt<1)||(g_nTrgPerInt>16))
			{
				DEBUG_OUTPUT(("normal mode,TrgPerInt must equal 1-16.\n"));
				Push2ErrStack(PPCFATAL+TRG_FLG+ATOM_ERR+26,0,0,0,0);		/*trg error 26*/
				return -1;
			}
			break;
		case PEDMODE:
		case CALIBRATION:
		case MONITOR:
		case BASELINE:
		case WAVEMODE:	
			if(g_nTrgPerInt!=1)
			{
				DEBUG_OUTPUT(("other mode,TrgPerInt must equal 1.\n"));
				Push2ErrStack(PPCFATAL+TRG_FLG+ATOM_ERR+27,0,0,0,0);		/*trg error 27*/
				return -1;
			}
			break ;
		default:
			//unknown running mode 2\0");
			Push2ErrStack(PPCFATAL+TRG_FLG+ATOM_ERR+19,0,0,0,0);		/*trg error 19*/
			return -1;
	}
	
	if(g_TRGDBInfo.trg_eacc_sum>0)
	{
		if(-1==TRG_EACC_conf() )
		{
			DEBUG_OUTPUT(("config eacc error.\n"));
			Push2ErrStack(PPCFATAL+TRG_FLG+ATOM_ERR+29,0,0,0,0);		/*trg error 29*/
			return -1;
		}
	}

	
	if(g_TRGDBInfo.trg_saf_sum>0)
	{
		if(-1==TRG_SAF_conf() )
		{
			DEBUG_OUTPUT(("config saf error.\n"));
			Push2ErrStack(PPCFATAL+TRG_FLG+ATOM_ERR+30,0,0,0,0);		/*trg error 30*/
			return -1;
		}
	}

	if(g_TRGDBInfo.trg_gtl_sum>0)
	{
		if(-1==TRG_GTL_conf() )
		{
			DEBUG_OUTPUT(("config gtl error.\n"));
			Push2ErrStack(PPCFATAL+TRG_FLG+ATOM_ERR+31,0,0,0,0);		/*trg error 31*/
			return -1;
		}
	}
	if(g_TRGDBInfo.trg_tof_sum>0)
	{
		if(-1==TRG_TOF_conf() )
		{
			DEBUG_OUTPUT(("config TRG_TOF error.\n"));
			Push2ErrStack(PPCFATAL+TRG_FLG+ATOM_ERR+32,0,0,0,0);		/*trg error 32*/
			return -1;
		}
	}
	
	if(g_TRGDBInfo.trg_etf_sum>0)
	{
		if(-1==TRG_ETF_conf() )
		{
			DEBUG_OUTPUT(("config TRG_ETF error.\n"));
			Push2ErrStack(PPCFATAL+TRG_FLG+ATOM_ERR+127,0,0,0,0);		/*trg error 127*/
			return -1;
		}
	}	
	
	if(g_TRGDBInfo.trg_tdc_sum>0)
	{
		if(-1==TRG_TDC_conf() )
		{
			DEBUG_OUTPUT(("config TRG_TDC error.\n"));
			Push2ErrStack(PPCFATAL+TRG_FLG+ATOM_ERR+115,0,0,0,0);		/*trg error 115*/
			return -1;
		}
	}		

	if(g_TRGDBInfo.trg_basg_sum>0)
	{
		if(-1==TRG_BASG_conf() )
		{
			DEBUG_OUTPUT(("config TRG_BASG error.\n"));
			Push2ErrStack(PPCFATAL+TRG_FLG+ATOM_ERR+116,0,0,0,0);		/*trg error 116*/
			return -1;
		}
	}					
	
	//-------------------------------------------------
	g_nTrgCounter = 0 ;    
	printf("TRGPreconfiged OK\n") ;
	return 0;
}
int  	TRGConfigedSYN1 		(int nArg) 
{

	if(g_TRGDBInfo.trg_tfcf_sum>0)
	{
		if(-1==TRG_TFCF_syn1())
			{
				DEBUG_OUTPUT(("syn1 tfcf error\n"));
				Push2ErrStack(PPCFATAL+TRG_FLG+ATOM_ERR+155,0,0,0,0);		/*trg error 155*/
				return -1;
			}
	}

	if(g_TRGDBInfo.trg_etf_sum>0)
	{
		if(-1==TRG_ETF_syn1() )
		{
			DEBUG_OUTPUT(("syn1 TRG_ETF error.\n"));
			Push2ErrStack(PPCFATAL+TRG_FLG+ATOM_ERR+52,0,0,0,0);		/*trg error 52*/
			return -1;
		}
	}


	printf("TRGConfigedSYN1 OK\n") ;
	return 0;
}
int  	TRGSynchronized1SYN2 	(int nArg) 
{
	if(g_TRGDBInfo.trg_clkf_sum>0)
	{
		if(-1==TRG_CLKF_syn2() )
		{
			DEBUG_OUTPUT(("syn2 clock error\n"));
			Push2ErrStack(PPCFATAL+TRG_FLG+ATOM_ERR+33,0,0,0,0);		/*trg error 33*/
			return -1;
		}
	}
	if(g_TRGDBInfo.trg_tfcf_sum>0)
	{
		if(-1==TRG_TFCF_syn2())
			{
				DEBUG_OUTPUT(("syn2 tfcf error\n"));
				Push2ErrStack(PPCFATAL+TRG_FLG+ATOM_ERR+34,0,0,0,0);		/*trg error 34*/
				return -1;
			}
	}
	if(g_TRGDBInfo.trg_fctl_sum>0)
	{
		if(-1==TRG_FCTL_syn2())
			{
				DEBUG_OUTPUT(("syn2  fctl error\n"));
				Push2ErrStack(PPCFATAL+TRG_FLG+ATOM_ERR+35,0,0,0,0);		/*trg error 35*/
				return -1;
			}
	}
	if(g_TRGDBInfo.trg_gtl_sum>0)
	{
		if(-1==TRG_GTL_syn2() )
		{
			DEBUG_OUTPUT(("syn2 GTL error.\n"));
			Push2ErrStack(PPCFATAL+TRG_FLG+ATOM_ERR+36,0,0,0,0);		/*trg error 36*/
			return -1;
		}
	}
	if(g_TRGDBInfo.trg_saf_sum>0)
	{
		if(-1==TRG_SAF_syn2() )
		{
			DEBUG_OUTPUT(("syn2 SAF error.\n"));
			Push2ErrStack(PPCFATAL+TRG_FLG+ATOM_ERR+37,0,0,0,0);		/*trg error 37*/
			return -1;
		}
	}
	if(g_TRGDBInfo.trg_eacc_sum>0)
	{
		if(-1==TRG_EACC_syn2() )
		{
			DEBUG_OUTPUT(("syn2 eacc error.\n"));
			Push2ErrStack(PPCFATAL+TRG_FLG+ATOM_ERR+38,0,0,0,0);		/*trg error 38*/
			return -1;
		}
	}
	if(g_TRGDBInfo.trg_tdc_sum>0)
	{
		if(-1==TRG_TDC_syn2() )
		{
			DEBUG_OUTPUT(("syn2 tdc error.\n"));
			Push2ErrStack(PPCFATAL+TRG_FLG+ATOM_ERR+117,0,0,0,0);		/*trg error 117*/
			return -1;
		}
	}		
	printf("TRGSynchronized1SYN2 OK\n") ;
	return 0;
}
int  	TRGSynchronized2SYN3 	(int nArg) 
{	
	if(g_TRGDBInfo.trg_tkf_sum>0)
	{
		if(-1==TRG_TKF_syn3() )
		{
			DEBUG_OUTPUT(("syn3 TKF error.\n"));
			Push2ErrStack(PPCFATAL+TRG_FLG+ATOM_ERR+39,0,0,0,0);		/*trg error 39*/
			return -1;
		}	
	}	

	if(g_TRGDBInfo.trg_mutl_sum>0)
	{
		if(-1==TRG_MUTL_syn3() )
		{
			DEBUG_OUTPUT(("syn3 MUTL error.\n"));
			Push2ErrStack(PPCFATAL+TRG_FLG+ATOM_ERR+40,0,0,0,0);		/*trg error 40*/
			return -1;
		}	
	}	

	if(g_TRGDBInfo.trg_ltkc_sum>0)
	{
		if(-1==TRG_LTKC_syn3() )
		{
			DEBUG_OUTPUT(("syn3 LTKC error.\n"));
			Push2ErrStack(PPCFATAL+TRG_FLG+ATOM_ERR+41,0,0,0,0);		/*trg error 41*/
			return -1;
		}
	}
	
	if(g_TRGDBInfo.trg_stkc_sum>0)
	{
		if(-1==TRG_STKC_syn3() )
		{
			DEBUG_OUTPUT(("syn3 STKC error.\n"));
			Push2ErrStack(PPCFATAL+TRG_FLG+ATOM_ERR+42,0,0,0,0);		/*trg error 42*/
			return -1;
		}
	}

	if(g_TRGDBInfo.trg_btm_sum>0)
	{
		if(-1==TRG_BTM_syn3() )
		{
			DEBUG_OUTPUT(("syn3 BTM error.\n"));
			Push2ErrStack(PPCFATAL+TRG_FLG+ATOM_ERR+152,0,0,0,0);		/*trg error 152*/
			return -1;
		}
	}

	if(g_TRGDBInfo.trg_etm_sum>0)
	{
		if(-1==TRG_ETM_syn3() )
		{
			DEBUG_OUTPUT(("syn3 ETM error.\n"));
			Push2ErrStack(PPCFATAL+TRG_FLG+ATOM_ERR+153,0,0,0,0);		/*trg error 153*/
			return -1;
		}
	}	
	
	if(g_TRGDBInfo.trg_tof_sum>0)
	{
		if(-1==TRG_TOF_syn3() )
		{
			DEBUG_OUTPUT(("syn3 TRG_TOF error.\n"));
			Push2ErrStack(PPCFATAL+TRG_FLG+ATOM_ERR+154,0,0,0,0);		/*trg error 154*/
			return -1;
		}
	}	



	printf("TRGSynchronized2SYN3 OK\n") ;
	return 0;
}
int  	TRGSynchronized3SYN4 	(int nArg) 
{
	if(g_TRGDBInfo.trg_fctl_sum>0)
	{
		if(-1==TRG_FCTL_syn4() )
		{
			DEBUG_OUTPUT(("syn4 FCTL error.\n"));
			Push2ErrStack(PPCFATAL+TRG_FLG+ATOM_ERR+43,0,0,0,0);		/*trg error 43*/
			return -1;
		}
	}
	
	TRGInitCBLT();
	printf("TRGSynchronized3SYN4 OK\n") ;
	return 0;
}
int  	TRGSynchronized4PREP 	(int nArg) 
{
	if(g_TRGDBInfo.trg_troc_sum>0)
	{
		if(-1==TRG_TROC_prep())
		{
			DEBUG_OUTPUT(("prep troc error\n"));
			Push2ErrStack(PPCFATAL+TRG_FLG+ATOM_ERR+44,0,0,0,0);		/*trg error 44*/
			return -1;
		}
	}
	if(g_TRGDBInfo.trg_tdc_sum>0)
	{
		if(-1==TRG_TDC_prep())
		{
			DEBUG_OUTPUT(("prep tdc error\n"));
			Push2ErrStack(PPCFATAL+TRG_FLG+ATOM_ERR+118,0,0,0,0);		/*trg error 118*/
			return -1;
		}
	}
	if(g_TRGDBInfo.trg_tkf_sum>0)
	{
		if(-1==TRG_TKF_prep())
		{
			DEBUG_OUTPUT(("prep tkf error\n"));
			Push2ErrStack(PPCFATAL+TRG_FLG+ATOM_ERR+45,0,0,0,0);		/*trg error 45*/
			return -1;
		}
	}

	if(g_TRGDBInfo.trg_ltkc_sum>0)
	{
		if(-1==TRG_LTKC_prep() )
		{
			DEBUG_OUTPUT(("prep LTKC error.\n"));
			Push2ErrStack(PPCFATAL+TRG_FLG+ATOM_ERR+46,0,0,0,0);		/*trg error 46*/
			return -1;
		}
	}
	
	if(g_TRGDBInfo.trg_stkc_sum>0)
	{
		if(-1==TRG_STKC_prep() )
		{
			DEBUG_OUTPUT(("prep STKC error.\n"));
			Push2ErrStack(PPCFATAL+TRG_FLG+ATOM_ERR+47,0,0,0,0);		/*trg error 47*/
			return -1;
		}
	}

	if(g_TRGDBInfo.trg_btm_sum>0)
	{
		if(-1==TRG_BTM_prep() )
		{
			DEBUG_OUTPUT(("prep BTM error.\n"));
			Push2ErrStack(PPCFATAL+TRG_FLG+ATOM_ERR+154,0,0,0,0);		/*trg error 154*/
			return -1;
		}
	}

	if(g_TRGDBInfo.trg_etm_sum>0)
	{
		if(-1==TRG_ETM_prep() )
		{
			DEBUG_OUTPUT(("prep ETM error.\n"));
			Push2ErrStack(PPCFATAL+TRG_FLG+ATOM_ERR+155,0,0,0,0);		/*trg error 155*/
			return -1;
		}
	}	
	
	if(g_TRGDBInfo.trg_tfcf_sum>0)
	{
		if(-1==TRG_TFCF_prep())
		{
			DEBUG_OUTPUT(("prep tfcf error\n"));
			Push2ErrStack(PPCFATAL+TRG_FLG+ATOM_ERR+48,0,0,0,0);		/*trg error 48*/
			return -1;
		}
	}

	if(g_TRGDBInfo.trg_gtl_sum>0)
	{
		if(-1==TRG_GTL_prep() )
		{
			DEBUG_OUTPUT(("prep GTL error\n"));
			Push2ErrStack(PPCFATAL+TRG_FLG+ATOM_ERR+50,0,0,0,0);		/*trg error 50*/
			return -1;
		}
	}
	if(g_TRGDBInfo.trg_saf_sum>0)
	{
		if(-1==TRG_SAF_prep() )
		{
			DEBUG_OUTPUT(("prep SAF error\n"));
			Push2ErrStack(PPCFATAL+TRG_FLG+ATOM_ERR+51,0,0,0,0);		/*trg error 51*/
			return -1;
		}
	}
	if(g_TRGDBInfo.trg_tof_sum>0)
	{
		if(-1==TRG_TOF_prep() )
		{
			DEBUG_OUTPUT(("prep TRG_TOF error.\n"));
			Push2ErrStack(PPCFATAL+TRG_FLG+ATOM_ERR+52,0,0,0,0);		/*trg error 52*/
			return -1;
		}
	}
	if(g_TRGDBInfo.trg_etf_sum>0)
	{
		if(-1==TRG_ETF_prep() )
		{
			DEBUG_OUTPUT(("prep TRG_ETF error.\n"));
			Push2ErrStack(PPCFATAL+TRG_FLG+ATOM_ERR+128,0,0,0,0);		/*trg error 128*/
			return -1;
		}
	}	
	if(g_TRGDBInfo.trg_mutl_sum>0)
	{
		if(-1==TRG_MUTL_prep() )
		{
			DEBUG_OUTPUT(("prep MUTL error.\n"));
			Push2ErrStack(PPCFATAL+TRG_FLG+ATOM_ERR+53,0,0,0,0);		/*trg error 53*/
			return -1;
		}	
	}	

	if(g_TRGDBInfo.trg_basg_sum>0)
	{
		if(-1==TRG_BASG_prep() )
		{
			DEBUG_OUTPUT(("PREP TRG_BASG error.\n"));
			Push2ErrStack(PPCFATAL+TRG_FLG+ATOM_ERR+119,0,0,0,0);		/*trg error 119*/
			return -1;
		}
	}		

	if(g_TRGDBInfo.trg_eacc_sum>0)
	{
		if(-1==TRG_EACC_prep())
		{
			DEBUG_OUTPUT(("PREP eacc error.\n"));
			Push2ErrStack(PPCFATAL+TRG_FLG+ATOM_ERR+54,0,0,0,0);		/*trg error 54*/
			return -1;
		}
	}

	if(g_TRGDBInfo.trg_ets_sum>0)
//	if(g_nCrateID==160)
	{
		if(-1==TRG_ETS_prep() )
		{
			DEBUG_OUTPUT(("PREP TRG_ETS error.\n"));
			Push2ErrStack(PPCFATAL+TRG_FLG+ATOM_ERR+161,0,0,0,0);		//trg error 161
			return -1;
		}
	}	
	
	
	if(nTrgCbltModNum>0)
	{
		switch(g_nRunMode)
		{
			case NOMALMODE:
			case PEDMODE:
			case CALIBRATION:
			case MONITOR:
			case BASELINE:
			case WAVEMODE:	
				if(-1 == (nCbltTaskId= taskSpawn("tFWCbltTrans", PRI_CBLTTRANS, 0, TASK_STACK_SIZE, (FUNCPTR)FWCbltTrans, 
										0, 0, 0, 0, 0, 0, 0, 0, 0, 0) )  )
				{
					//CbltTrans taskSpawn failed\0");
					Push2ErrStack(PPCFATAL+TRG_FLG+ATOM_ERR+55,0,0,0,0);		/*trg error 55*/
					return -1;
				}
				if(-1 == (nPackTaskId= taskSpawn("tTRGDataPack", PRI_DATAPACK, 0, TASK_STACK_SIZE, (FUNCPTR)TRGDataPack, 
										0, 0, 0, 0, 0, 0, 0, 0, 0, 0) )  )
				{
					//Pack taskSpawn failed\0");
					Push2ErrStack(PPCFATAL+TRG_FLG+ATOM_ERR+56,0,0,0,0);		/*trg error 56*/
					return -1;
				}
				if(-1 == (nNetTaskId= taskSpawn("tFWNetTrans", PRI_NETTRANS, 0, TASK_STACK_SIZE, (FUNCPTR)FWNetTrans, 
										0, 0, 0, 0, 0, 0, 0, 0, 0, 0) ) )
				{
					//NetTrans taskSpawn failed\0");
					Push2ErrStack(PPCFATAL+TRG_FLG+ATOM_ERR+57,0,0,0,0);		/*trg error 57*/
					return -1;
				}
				break ;
			default:
				Push2ErrStack(PPCFATAL+TRG_FLG+ATOM_ERR+19,0,0,0,0);		/*trg error 19*/
				break ;
		}

	}
	printf("TRGSynchronized4PREP OK\n") ;
	return 0;
}
int  	TRGReadySATR  			(int nArg)
{
	switch(g_nRunMode)
	{
		case NOMALMODE:

			if(g_TRGDBInfo.trg_gtl_sum>0)
			{
				if(-1==TRG_GTL_satr())
				{
					DEBUG_OUTPUT(("SATR gtl error.\n"));
					Push2ErrStack(PPCFATAL+TRG_FLG+ATOM_ERR+59,0,0,0,0);		/*trg error 59*/
					return -1;
				}
			}
		
			
			break;
		case PEDMODE:
		case CALIBRATION:
		case MONITOR:
		case BASELINE:
		case WAVEMODE:		
			break ;
		default:
			Push2ErrStack(PPCFATAL+TRG_FLG+ATOM_ERR+19,0,0,0,0);		/*trg error 19*/
			break ;
	}
	printf("TRGReadySATR OK\n") ;
	return 0;
}
int  	TRGRunningSPTR  		(int nArg)
{
	switch(g_nRunMode)
	{
		case NOMALMODE:
			if(g_TRGDBInfo.trg_gtl_sum>0)
			{
				if(-1==TRG_GTL_sptr())
				{
					DEBUG_OUTPUT(("SPTR gtl error.\n"));
					Push2ErrStack(PPCFATAL+TRG_FLG+ATOM_ERR+61,0,0,0,0);		/*trg error 61*/
					return -1;
				}
			}
			if(g_TRGDBInfo.trg_basg_sum>0)
			{
				if(-1==TRG_BASG_sptr())
				{
					DEBUG_OUTPUT(("SPTR basg error.\n"));
					Push2ErrStack(PPCFATAL+TRG_FLG+ATOM_ERR+120,0,0,0,0);		/*trg error 120*/
					return -1;
				}
			}		
			if(g_TRGDBInfo.trg_eacc_sum>0)
			{
				if(-1==TRG_EACC_sptr())
				{
					DEBUG_OUTPUT(("SPTR eacc error.\n"));
					Push2ErrStack(PPCFATAL+TRG_FLG+ATOM_ERR+121,0,0,0,0);		/*trg error 121*/
					return -1;
				}
			}				
			break;
		case PEDMODE:
		case CALIBRATION:
		case MONITOR:
		case BASELINE:
		case WAVEMODE:		
			break ;
		default:
			Push2ErrStack(PPCFATAL+TRG_FLG+ATOM_ERR+19,0,0,0,0);		/*trg error 19*/
			break ;
	}
	
	printf("TRGRunningSPTR OK\n") ;
	return 0;
}
int  	TRGReadySTOP  			(int nArg)
{

	if(nTrgCbltModNum==0)//no data crate need give bufempty signal
	{
		semGive(g_sidBufEmpty) ;
	}
/*	
	if(g_TRGDBInfo.trg_fctl_sum>0)
	{
		trg_status();
	}	
	*/
	printf("TRGReadySTOP OK\n") ;
	return 0;
}
int  	TRGConfigedUNCF 		(int nArg)
{
	printf("TRGConfigedUNCF OK\n") ;
	return 0;
}
int  	TRGPreconfigedUNPC 	(int nArg) 
{
	printf("TRGPreconfigedUNPC OK\n") ;
	return 0;
}
int  	TRGLoadedUNLD 			(int nArg)
{
	printf("TRGLoadedUNLD OK\n") ;
	return 0;
}
int  	TRGAnyEXIT 			(int nArg)
{
	printf("TRGAnyEXIT OK\n") ;
	return 0;
}

