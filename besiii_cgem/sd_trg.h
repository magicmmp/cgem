/*****sd_trg.h*****/
#ifndef __INCSDTRGYh
#define __INCSDTRGYh

#define TRG_NORM_DAQ 0

#define TRG_MAX_BYTES_PERMDL 512

struct trg_db_info
{
	int 	trg_ppc_sum;
	struct GE_ppc  *trg_ppc_header  ;
	
	int	trg_eacc_sum;
	struct trg_eacc *trg_eacc_header;
	
	int	trg_saf_sum;
	struct trg_saf *trg_saf_header[3];
	
	int 	trg_gtl_sum;
	struct trg_gtl *trg_gtl_header;
	
	int	trg_troc_sum;
	struct trg_troc *trg_troc_header;
	
	int	trg_fctl_sum;
	struct trg_fctl	  *trg_fctl_header;
	
	int	trg_tfcf_sum;
	struct trg_tfcf *trg_tfcf_header;
	
	int	trg_clkf_sum;
	struct trg_clkf *trg_clkf_header;

	int	trg_tdc_sum;
	struct trg_tdc	  *trg_tdc_header;

	int	trg_ltkc_sum;
	struct trg_tkc  *trg_ltkc_header;
	
	int	trg_stkc_sum;
	struct trg_tkc  *trg_stkc_header;
	
	int	trg_tkf_sum;
	struct trg_tkf	*trg_tkf_header[9];

	int	trg_tof_sum;
	struct trg_tof	 *trg_tof_header;

	int	trg_basg_sum;
	struct trg_basg *trg_basg_header;	

	int	trg_mutl_sum;
	struct trg_mutl *trg_mutl_header;	

	int trg_btm_sum;
	struct trg_btm *trg_btm_header;

	int trg_etm_sum;
	struct trg_etm *trg_etm_header;	

	int trg_ets_sum;
	struct trg_ets *trg_ets_header;		

	int	trg_etf_sum;
	struct trg_etf	 *trg_etf_header;
	
};

extern struct trg_db_info  g_TRGDBInfo;

extern UINT	uTrgCbltModGeo[16];//jixl 071130
extern int 	nTrgCbltModNum;
extern UINT	uTrgTDCCbltAdrs ;

extern unsigned short TrgFCTLGeo;
extern unsigned short TrgFCTLBSEL;

extern unsigned short TrgTFCFGeo;
extern unsigned short TrgTFCFBSEL;

extern unsigned short TrgCLKFGeo;
extern unsigned short TrgCLKFBSEL;

extern unsigned short TrgTROCGeo;
extern unsigned short TrgTROCBSEL;

extern unsigned short TrgEaccGeo;
extern unsigned short TrgEaccBSEL;
extern unsigned short TrgEaccSYNCheck;

extern unsigned short TrgSAFGeo[3];
extern unsigned short TrgSAFBSEL[3];

extern unsigned short TrgGTLGeo;
extern unsigned short TrgGTLBSEL;

extern unsigned short TrgTDCGeo;
extern unsigned short TrgTDCBSEL;

extern unsigned short TrgLTKCGeo;
extern unsigned short TrgLTKCBSEL;

extern unsigned short TrgSTKCGeo;
extern unsigned short TrgSTKCBSEL;	

extern unsigned short TrgTKFGeo[9];
extern unsigned short TrgTKFBSEL[9];
extern unsigned short TrgTKFSYNCheck[9];

extern unsigned short TrgMUTLGeo;
extern unsigned short TrgMUTLBSEL;

extern unsigned short TrgTOFGeo;
extern unsigned short TrgTOFBSEL;
extern unsigned short TrgTOFCBLT;
extern unsigned short TrgTOFSYNCheck;

extern unsigned short TrgETFGeo;
extern unsigned short TrgETFBSEL;



extern unsigned short TrgBASGGeo;
extern unsigned short TrgBASGBSEL;

extern unsigned short TrgBTMGeo;
extern unsigned short TrgBTMBSEL;

extern unsigned short TrgETMGeo;
extern unsigned short TrgETMBSEL;

extern unsigned short TrgETSGeo;
extern unsigned short TrgETSBSEL;
extern unsigned short TrgETSCbltEnable;

extern unsigned int etft_readout_window ;
	
void TRGLinkRtns() ;


#endif

