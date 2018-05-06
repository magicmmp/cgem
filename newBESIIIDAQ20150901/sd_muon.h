#ifndef __INCSDMUONh
#define __INCSDMUONh

#define MUON_ROMO_AMOUNT 16
#define MUON_MAX_BYTES_PERMDL 512

struct muon_db_info
{
	int ppc_sum ;
	struct GE_ppc  *ppc_header  ;

	int enabl_romo_sum ;
	struct muon_romo * enabl_romo_header[MUON_ROMO_AMOUNT]  ;

	int disabl_romo_sum ;
	struct muon_romo * disabl_romo_header[MUON_ROMO_AMOUNT]  ;

	int roc_sum ;
	struct muon_roc * roc_header ;

	int mudt_sum;
	struct muon_mudt * mudt_header[5];
};

struct muon_hw_info
{
	int romo_sum ;
	int romo_geo[MUON_ROMO_AMOUNT] ;
};

extern struct muon_db_info  g_MUONDBInfo;
extern struct muon_hw_info g_MUONHWInfo;

extern SEM_ID g_sidMuonSpecial; /*leigkleigk 060820*/

extern unsigned short gMucEnablRomoGeo[10];
extern unsigned short gMucEnablModuleID[10];
extern unsigned short gMucDisableChainID[10];
extern unsigned short gMucExchangeChainID[10];

extern unsigned short MucMUDTBSEL[5];
extern unsigned short MucMUDTGeo[5];

extern bool b_TrgErrFst;
extern bool b_ChkErrFst;


void MUONLinkRtns() ;


#endif

