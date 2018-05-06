#ifndef __INCSDTOFh
#define __INCSDTOFh

#define TOF_MAX_BYTES_PERMDL 2048
#define MAXFEENUM			16

struct tof_db_info
{
	int tof_ppc_sum;
	struct GE_ppc  *tof_ppc_header  ;
	
	int tof_FEE_sum;
	struct tof_FEE * tof_FEE_header[16];

	int tof_roc_sum;
	struct tof_roc * tof_roc_header ;

	int tof_clk_sum;
	struct tof_clk * tof_clk_header;

	int tof_monitor_sum;
	struct tof_monitor* tof_monitor_header;
/*
	int tof_tdig_sum;
	struct tof_tdig* tof_tdig_header;	
*/	
};

extern struct tof_db_info  g_TOFDBInfo;

extern unsigned int		g_uTofCbltGeo[16];
extern unsigned int		g_uTofCbltModuleSum;
extern unsigned int		g_uTofMonitorGeo;

extern unsigned int 	g_uTofCalibSignAmp;

extern unsigned int 	g_uTofLDPulseInterval;
extern unsigned int 	g_uTofLDPulseNum;
extern unsigned int 	g_uTofLDSolenoid;

extern int 			nTOFGenL1TaskId;


/** for TDIG parameters**/
//extern int reject_cnt_offset ;/*48-59, data 2 bit 16-27*/
//extern int search_window ;/*60-71, data 2 bit 28-31, data 3 bit 0-7*/
//extern int match_window ;/*72-83, data 3 bit 8-19*/
//extern int trigger_latency ;/*138-149,data 5 bit 10-21*/
//extern int inlCorrection ;
//extern int configFPGA_enable;
//extern int interrupt_enable;
//extern unsigned int TDIG_geo;
//extern int TDIG_boardEn;
//extern int TDIG_CBLTEn;


void TOFLinkRtns() ;

#endif

