/*-----------------------------
 fw_configinfo.h
 based renzy edit version
 begin                :  3th Dec 16:01:32 CST 2006
 copyright            : (C) 2004 by leigk
 email                : leigk@mail.ihep.ac.cn
 description: the information to config the VME crate
-----------------------------*/
#ifndef _CONFIGINFO_
#define _CONFIGINFO_

#define MDC_PPC		0x21
#define MDC_MQT		0x22
#define MDC_ROC		0x23
#define MDC_MCC		0x24
#define MDC_MTI 	0x25

#define TOF_PPC		0x41
#define TOF_FEE		0x42
#define TOF_ROC		0x43
#define TOF_CLK		0x44
#define TOF_MONITOR	0x45

#define EMC_PPC		0x61
#define EMC_Q		0x62
#define EMC_ROC		0x63
#define EMC_FANOUT  0x64
#define EMC_TRGTCBA 0x65
#define EMC_MONITOR 0x66

#define MUON_PPC	0x81
#define MUON_ROMO	0x82
#define MUON_ROC	0x83
#define MUON_MUDT	0x84

#define TRG_PPC		0xa1
#define TRG_EACC	0xa2
#define TRG_SAF		0xa3
#define TRG_GTL		0xa4
#define TRG_TROC	0xa5
#define TRG_FCTL	0xa6
#define TRG_TFCF	0xa7
#define TRG_CLKF	0xa8
#define TRG_TDC		0xa9
#define TRG_LTKC	0xaa
#define TRG_STKC	0xab
#define TRG_TKF		0xac
#define TRG_TOF		0xad
#define TRG_MUTL	0xae
#define TRG_BASG	0xaf
#define TRG_BTM		0xb0 
#define TRG_ETM		0xb1
#define TRG_ETS		0xb2
#define TRG_ETF		0xb3


#define ZDD_PPC 	0xc1
#define ZDD_V1721 	0xc2
#define ZDD_MCC		0xc3
#define ZDD_MTI 	0xc4
#define ZDD_V1190 	0xc5

#define ETF_PPC		0xE1
#define ETF_TDIG	0xE2
#define ETF_ROC		0xE3
#define ETF_CLK		0xE4
#define ETF_CTTP	0xE5



typedef struct Header
{
	unsigned int	All_len;//renzy:in bytes
	unsigned int	crate_id;
	unsigned int	crate_len;//renzy:in bytes
	unsigned int	trigger_type;
	unsigned int	module_num;
	unsigned int	events_per_read;
	unsigned int	run_times;
	unsigned int	unused;	//bit 0:whether need to calib: 0-no need/ 1-need
						//bit 1:whether AMP gain calib:0-no;1-yes
}Header_t __attribute__ ((packed));

void Show_Header(Header_t * pConf);

/**************************************
For Calibaration Information
--------define as bellow 

unsigned int  nCalibInforLenth;
unsigned int  unused;

N*	{
		unsigned int  DAC;
		unsigned int  T0Latency;
		unsigned int  runtimes;
	}

***************************************/
typedef struct GE_ppc		//general PPC structrue
{
	unsigned int	info_len;//leigk:in bytes
	unsigned int	module_type;//leigk:e.g GE-PPC
	unsigned int	geo;
	unsigned int	PPC_id;
	unsigned int	ROS_ip;
	unsigned int	ROS_port;
	unsigned int	NormModeSelfL1;	//0 norm mode, normal L1,1 norm mode Self L1
}GE_ppc_t __attribute__ ((packed));

void Show_ppc(GE_ppc_t * pConf);
/************************************************/


typedef struct mdc_mqt
{
	unsigned int	info_len;//renzy:in bytes
	unsigned int	module_type;//renzy:e.g MDC-MQT 0x22
	unsigned int	geo;//renzy:mod_Enable<<8+geo
	unsigned int	DAC;
	unsigned int	TimeThreshold_l;
	unsigned int	TimeThreshold_h;
	unsigned int	controlregister;	//leigk 0x8000,minus step value or not
	unsigned int	QT_hit_option;	//leigk QT double hit
	unsigned int	T_sample_win;
	unsigned int	T_trigger_latency;
	unsigned int	Q_sample_win;
	unsigned int	Q_trigger_latency;
	unsigned int	T_modules_Enable;
	unsigned int	layercell[32];
	unsigned int	QThreshold[32];
	float			adjust_a2[32];
	float			adjust_a1[32];
	float			adjust_a0[32];			
}mdc_mqt_t __attribute__ ((packed));


void Show_mdc_mqt(mdc_mqt_t * pConf);
/**************************************/


typedef struct mdc_roc
{
	unsigned int	info_len;//renzy:in bytes
	unsigned int	module_type;//renzy:e.g MDC-ROC 0x23
	unsigned int	geo;
	unsigned int	unused;
}mdc_roc_t __attribute__ ((packed));

void Show_mdc_roc(mdc_roc_t * proc);
/**************************************/

typedef struct mdc_mcc
{
	unsigned int	info_len;//renzy:in bytes
	unsigned int	module_type;//renzy:e.g MDC-MCC 0x24
	unsigned int	geo;
	unsigned int	T0_latency;
}mdc_mcc_t __attribute__ ((packed));

void Show_mdc_mcc(mdc_mcc_t* pmcc);
/**************************************/
typedef struct mdc_mti
{
	unsigned int	info_len;//leigk:in bytes
	unsigned int	module_type;//leigk:e.g MDC-MTI 0x25
	unsigned int	geo;
	unsigned int	unused;
}mdc_mti_t __attribute__ ((packed));

void Show_mdc_mti(mdc_mti_t* pmti);
/**************************************/

typedef struct emc_Q
{
	unsigned int	info_len;//renzy:in bytes
	unsigned int	module_type;//renzy:e.g EMC-Q 0x62
	unsigned int	geo;
	unsigned int	sample_win;
	unsigned int	trigger_latency;
	unsigned int  	outL1;
	unsigned int	unused;	
	unsigned int	map[32];
	unsigned int	QThreshold[32];
	unsigned int 	MainAmpGeo[32];	//but config throuth ROC module
	unsigned int 	GainValue[32];	//but config throuth ROC module
	float			low_adjust_a2[32];
	float			low_adjust_a1[32];
	float			low_adjust_a0[32];	
	float			mid_adjust_a2[32];
	float			mid_adjust_a1[32];
	float			mid_adjust_a0[32];	
	float			high_adjust_a2[32];
	float			high_adjust_a1[32];
	float			high_adjust_a0[32];
}emc_Q_t __attribute__ ((packed));

void Show_emc_Q(emc_Q_t* pConf);
/*****************************************/


typedef struct emc_roc
{
	unsigned int	info_len;		//renzy:in bytes leigk
	unsigned int	module_type;//renzy:e.g EMC-ROC 0x63
	unsigned int	geo;
	unsigned int	dac;
	unsigned int	cal;
	unsigned int	unused;
}emc_roc_t __attribute__ ((packed));

void Show_emc_roc(emc_roc_t* pConf);
/*****************************************/
typedef struct emc_fanout
{
	unsigned int	info_len;//leigk:in bytes
	unsigned int	module_type;//leigk:e.g EMC-FANOUT 0x64
	unsigned int	geo;
	unsigned int	dac;
	unsigned int	cal;
	unsigned int	unused;
}emc_fanout_t __attribute__ ((packed));

void Show_emc_fanout(emc_fanout_t* pConf);
/*****************************************/
typedef struct emc_monitor
{
	unsigned int	info_len;//renzy:in bytes
	unsigned int	module_type;//renzy:e.g EMC-Q 0x62
	unsigned int	geo;
	unsigned int	sample_win;
	unsigned int	trigger_latency;
	unsigned int  	outL1;
	unsigned int	unused;	
	unsigned int	map[32];
	unsigned int	QThreshold[32];
	unsigned int 	MainAmpGeo[32];	//but config throuth ROC module
	unsigned int 	GainValue[32];	//but config throuth ROC module
	float			low_adjust_a2[32];
	float			low_adjust_a1[32];
	float			low_adjust_a0[32];	
	float			mid_adjust_a2[32];
	float			mid_adjust_a1[32];
	float			mid_adjust_a0[32];	
	float			high_adjust_a2[32];
	float			high_adjust_a1[32];
	float			high_adjust_a0[32];	
}emc_monitor_t __attribute__ ((packed));

void Show_emc_monitor(emc_monitor_t* pConf);
/*****************************************/
typedef struct emc_trgtcba	//no data, in other crate,not trig self
{
	unsigned int	info_len;
	unsigned int	module_type;
	unsigned int	geo;
	unsigned int    DatamoduleFlag;	//bool for sign data module,1 data module
	unsigned int	WinLen;
	unsigned int	HL_Vth;
	unsigned int	L1Sel;	//bool for select out or inner L1
	unsigned int	IntEn;	// 1 enable
	unsigned int	ChTimeCompens[30];	//channel time compensate 30channel
	unsigned int	unused;
}emc_trgtcba_t __attribute__ ((packed));

void Show_emc_trgtcba(emc_trgtcba_t * pConf);

/*****************************************/


typedef struct muon_romo
{
	unsigned int	info_len;//leikg:in bytes
	unsigned int	module_type;//leigk 0x82
	unsigned int	Position;	//ModuleID<<12 +Geo
	unsigned int	data_source;
	unsigned int 	threshold;
							// for data get, generally set 0xef
	unsigned int  	ChainDisable;
}muon_romo_t __attribute__ ((packed));

void Show_muon_romo(muon_romo_t* pConf);
/*****************************************/


typedef struct muon_roc
{
	unsigned int	info_len;//leigk
	unsigned int	module_type;//leigk 0x83
	unsigned int	geo;
	unsigned int	triger_rate;
					// select 1,2,4,6,10
					// 1 =1000HZ , 2= 2000HZ ,4=4000HZ, 6=6000HZ, 10=10000HZ
}muon_roc_t __attribute__ ((packed));

void Show_muon_roc(muon_roc_t* pConf);
/*****************************************/


typedef struct muon_mudt
{
	unsigned int	info_len;
	unsigned int	module_type;
	unsigned int	Position;
	unsigned int    DatamoduleFlag;
	unsigned int 	ControlState;
}muon_mudt_t __attribute__ ((packed));

void Show_muon_mudt(muon_mudt_t* pConf);
/**********************************TOF settting****/

typedef struct tof_FEE
{
	unsigned int	info_len;//leigk:in bytes
	unsigned int	module_type;//leigk:e.g TOF-TQ 0x42
	unsigned int	geo;
	unsigned int  	barrel_end;	//for module distinguish barrel part or barrel_end,barrel=0, end=1
	//----------htpdc---------
	unsigned int	chanlEnable;
	unsigned int	trgEnable;	//bool
	unsigned int	rejectLaten;
	unsigned int	trgLatency;
	unsigned int	trgSearchWinT;
	unsigned int	trgMatchWinT;
	unsigned int	trgSearchWinQT;
	unsigned int	trgMatchWinQT;
	unsigned int	hitCntPerEvtT;
	unsigned int	hitCntPerEvtQT;
	//----------set fee enable-----
	unsigned int	highThresh;
	unsigned int	lowThresh;
	unsigned int	highThreshQT;
	unsigned int	lowThreshQT;
	unsigned int	constVoltage;
	unsigned int	selfTestSignAmp;
	//-----------------
	unsigned int	serialNum;	//identify the board, unique
	unsigned int	unused;
	unsigned int	map[16];
	float			adjust_a2[16];
	float			adjust_a1[16];
	float			adjust_a0[16];			
	char			feeINL[0];	//INL data
}tof_FEE_t  __attribute__ ((packed));

void Show_tof_FEE(tof_FEE_t* pConf);
/***************************************************/
typedef struct tof_monitor
{
	unsigned int	info_len;//leigk:in bytes
	unsigned int	module_type;//leigk:e.g TOF-TQ 0x45
	unsigned int	geo;
	unsigned int  	barrel_end;	//no use for monitor
	//----------htpdc---------
	unsigned int	chanlEnable;
	unsigned int	trgEnable;	//bool
	unsigned int	rejectLaten;
	unsigned int	trgLatency;
	unsigned int	trgSearchWinT;
	unsigned int	trgMatchWinT;
	unsigned int	trgSearchWinQT;
	unsigned int	trgMatchWinQT;
	unsigned int	hitCntPerEvtT;
	unsigned int	hitCntPerEvtQT;
	//----------set fee enable-----
	unsigned int	highThresh;
	unsigned int	lowThresh;
	unsigned int	highThreshQT;
	unsigned int	lowThreshQT;
	unsigned int	constVoltage;
	unsigned int	selfTestSignAmp;
	//-----------------
	unsigned int	LDPulseIntervalReg; 	//laser device, save to data
	unsigned int	LDPulseNumReg;		//Pulse num,save to data
	unsigned int	MonitorL1LatencyReg;
	unsigned int	LDPulseWidthReg;
	unsigned int	MonitorL1PulseWidthReg;
	unsigned int	L1DelayClk;
	//
	unsigned int	LDSolenoid;//use last 3 bit 2-0 ,save to data
	//------------------
	unsigned int	serialNum;	//identify the board, unique
	unsigned int	unused;
	char			feeINL[0];	//INL data
}tof_monitor_t  __attribute__ ((packed));

void Show_tof_monitor(tof_monitor_t* pConf);
/***************************************************/
typedef struct tof_roc
{
	unsigned int  info_len;//leigk:in bytes
	unsigned int  module_type;//TOF-ROC 0x43
	unsigned int  geo;
	unsigned int  clksource;
	unsigned int  autoL1Interval;
	unsigned int  semiAutoL1Num;          //test mode write one time give testL1
	unsigned int  L1Latency;
	unsigned int  unused;
}tof_roc_t __attribute__ ((packed));

void Show_tof_roc(tof_roc_t* pConf);
/***************************************************/
typedef struct tof_clk
{
	unsigned int	info_len;//leigk:in bytes
	unsigned int	module_type;//TOF-ROC 0x44
	unsigned int	geo;
	unsigned int	clksource;
	//0x0 inner clock, 0x1 divid clock, 0xc reserved clock, 0xe fiber input clock
}tof_clk_t __attribute__ ((packed));

void Show_tof_clk(tof_clk_t* pConf);


/***************ETF SYSTEM************************************/
typedef struct etf_tdig
{
	unsigned int	info_len;
	unsigned int	module_type;
	unsigned int	geo;
	unsigned int	windowSet;
	unsigned int	trigger_latency;
	unsigned int	trigger_source;
	unsigned int 	errDataEnb;
	unsigned int	map[72];
	unsigned int  	unused;
}etf_tdig_t __attribute__ ((packed));

void Show_etf_tdig(etf_tdig_t* pConf);

/***************************************/
typedef struct etf_roc
{
	unsigned int  info_len;
	unsigned int  module_type;
	unsigned int  geo;
	unsigned int  clksource;
	unsigned int  autoL1Interval;
	unsigned int  semiAutoL1Num;          //test mode write one time give testL1
	unsigned int  L1Latency;
	unsigned int clock_phase;
	unsigned int  cttp_enable; //temp used now jixl 20140310
	unsigned int  unused;
}etf_roc_t __attribute__ ((packed));

void Show_etf_roc(etf_roc_t* pConf);
/***************************************/
typedef struct etf_clk
{
	unsigned int	info_len;
	unsigned int	module_type;
	unsigned int	geo;
	unsigned int	clksource;
	//0x0 inner clock, 0x1 divid clock, 0xf slave clock
	unsigned int	clkLatency;
}etf_clk_t __attribute__ ((packed));

void Show_etf_clk(etf_clk_t* pConf);
/**************************************/
typedef struct etf_cttp
{
	unsigned int	info_len;
	unsigned int	module_type;
	unsigned int	mod_ID;
	unsigned int 	cttp_enable;
	unsigned int	nino_low_threshold;
	unsigned int	nino_high_threshold;
	unsigned int	power_low_threshold;
	unsigned int	power_high_threshold;
	unsigned int	cal_amp;
}etf_cttp_t __attribute__ ((packed));

void Show_etf_cttp(etf_cttp_t* pConf);




/****************TRG SYS**************************/

typedef struct trg_eacc
{
	unsigned int	info_len;
	unsigned int	module_type;
	unsigned int	Position;
	unsigned int    DatamoduleFlag;// data readout
	unsigned int    SimRealSel;	// 1 simulation data, 0 real data
	unsigned int    L1Sel;	// 0 outL1 ,1 innerL1
	unsigned int    FiberAlignLatency;	// 0x20 0x3a 0x40
	unsigned int    FiberChannelSet;		// set which channel work
	unsigned int    CSR1;
	unsigned int    CsumDepths;
	unsigned int    CsumWins;
	unsigned int    DataLenPerEvent;
	unsigned int    EtotDataSteps;
	unsigned int    EtotDataDepths;
	unsigned int    BarrelEndcapSel;
	unsigned int    Threshold[10];
	unsigned int    ErrSet;
	unsigned int    DebugFlg;
	unsigned int    SimDataBlk[32];
	unsigned int    CsumDepthsCal;
	unsigned int    EtotDepthCal;
}trg_eacc_t __attribute__ ((packed));
void Show_trg_eacc(trg_eacc_t * pConf);

/******************************************/
typedef struct trg_saf
{
	unsigned int	info_len;
	unsigned int	module_type;
	unsigned int	Position;
	unsigned int    DatamoduleFlag;//data module

	unsigned int    DelayOfChannel[16];
	unsigned int	UserOwnCSR;
	unsigned int	CbltMcstCSR1;
	unsigned int	TrgLatencyInput;
	unsigned int	TrgWindowWidthInput;
	unsigned int	TrgLatencyAligned;
	unsigned int	TrgWindowWidthAligned;
	unsigned int	FullGlobalBuffer;
	unsigned int	FullGlobalORedBuffer;
	unsigned int	FullNotGlobalBuffer;
	unsigned int	FullNotGlobalORedBuffer;
	unsigned int	L1Selection;
	unsigned int	unused;
}trg_saf_t __attribute__ ((packed));

void Show_trg_saf(trg_saf_t * pConf);
/******************************************/
typedef struct trg_gtl
{
	unsigned int	info_len;
	unsigned int	module_type;
	unsigned int	Position;
	unsigned int    DatamoduleFlag;//data module

	unsigned int    DelayOfChannel[48];
	unsigned int	InternalL1Delay;
	unsigned int	UserOwnCSR;
	unsigned int	CbltMcstCSR1;
	unsigned int	TrgWindows;	
	unsigned int	GlobalBufferNotFullFull;
	unsigned int	BarrelBhabhaPrescale;	
	unsigned int	EndcapBhabhaPrescale;
	unsigned int	BackupRegister;
	unsigned int	TimingOutputBitsMdcEmcTof;
	unsigned int	TofTimingORedBits;
	unsigned int	EmcTimingORedBits;
	unsigned int	MdcTimingORedBits;
	unsigned int	TimeOutL1InternalWidth;
	unsigned int	L1OutputWidthTimingSelectionMdcEmcTof;
	unsigned int	L1ChkOutputControl;
	unsigned int	ChkDelayToL1;
	unsigned int	TrgLatencyInput;
	unsigned int	TrgLatencyAligned;
	unsigned int	TrgLatencyEvtType;
	unsigned int	TrgChannelEnables;//for trig channel enable
	unsigned int	TrigTable[64];//dma write read
	unsigned int	unused;
}trg_gtl_t __attribute__ ((packed));

void Show_trg_gtl(trg_gtl_t * pConf);

/******************************************/
typedef struct trg_troc
{
	unsigned int	info_len;
	unsigned int	module_type;
	unsigned int	Position;
	unsigned int    DatamoduleFlag;
	unsigned int	unused;
}trg_troc_t __attribute__ ((packed));

void Show_trg_troc(trg_troc_t * pConf);
/******************************************/
typedef struct trg_fctl
{
	unsigned int	info_len;
	unsigned int	module_type;
	unsigned int	Position;
	unsigned int    DatamoduleFlag;//no data
	unsigned int	L1_config;		//three num,0,1,2
	unsigned int	Input_en1;
	unsigned int	Input_en2;
	unsigned int	IntCount;
	unsigned int	IntTiming;
	unsigned int	IntEnable;
	unsigned int	SendFRST;
	unsigned int	SubSystemInfo;			/*	 D05        D04        D03       D02        D01        D00     
									              TOF-2      TOF-1      MU       EMC        MDC       REV   */
}trg_fctl_t __attribute__ ((packed));

void Show_trg_fctl(trg_fctl_t * pConf);


/******************************************/
typedef struct trg_tfcf
{
	unsigned int	info_len;
	unsigned int	module_type;
	unsigned int	Position;
	unsigned int    DatamoduleFlag;//no data
	unsigned int	CSR;
	unsigned int	V_Err_T;
	unsigned int	V_Err_C;
	unsigned int	Per_ERR_12;
	unsigned int	Per_ERR_34;
	unsigned int	CO_IR;
	unsigned int	IO_PR;
	unsigned int	ENBL;
	unsigned int	unused;
}trg_tfcf_t __attribute__ ((packed));

void Show_trg_tfcf(trg_tfcf_t * pConf);
/******************************************/
typedef struct trg_clkf
{
	unsigned int	info_len;
	unsigned int	module_type;
	unsigned int	Position;
	unsigned int    DatamoduleFlag;//no data
	unsigned int    ClkSource;
	unsigned int	unused;
}trg_clkf_t __attribute__ ((packed));

void Show_trg_clkf(trg_clkf_t * pConf);
/******************************************/
typedef struct trg_tdc
{
	unsigned int	info_len;
	unsigned int	module_type;
	unsigned int	Position;
	unsigned int    DatamoduleFlag;

	unsigned int	ChannelEnable[4];
	unsigned int	SetTriggerWindowWidth01;
	unsigned int	SetTriggerWindowOffset01;
	unsigned int	SetExtraSearchMargin01;
	unsigned int	SetRejectMargin01;
	unsigned int	SetEdgeDetectionConfig01;
	unsigned int	SetLeadingTrailingEdge01;
	unsigned int	SetChannelDeadTimeOfHits01;
	unsigned int	SetMaxNumberOfHitsPerEvent01;
	unsigned int	EnableTdcInternalErrorType01;
	unsigned int	SetEffectiveSizeOfReadoutFifo01;
	unsigned int	SetDllClock01;
	unsigned int	SetControlRegister;
	unsigned int	SetInterruptLevel;
	unsigned int	SetInterruptVector;
	unsigned int	SetGeoAddress;
	unsigned int	SetCbltMcstBaseAddress;
	unsigned int	SetCbltMcstCSR;
	unsigned int	SetAlmostFull;
	unsigned int	SetOutProgControl;
	unsigned int	unused;
}trg_tdc_t __attribute__ ((packed));

void Show_trg_tdc(trg_tdc_t * pConf);

/******************************************/
typedef struct trg_tkc
{
	unsigned int	info_len;
	unsigned int	module_type;
	unsigned int	Position;
	unsigned int    DatamoduleFlag;//no data
	
	unsigned int    CBLTSet;
	unsigned int    L1Select;
	unsigned int	unused;
}trg_tkc_t __attribute__ ((packed));

void Show_trg_tkc(trg_tkc_t * pConf);

/******************************************/

typedef struct trg_tkf
{
	unsigned int	info_len;
	unsigned int	module_type;
	unsigned int	Position;
	unsigned int    DatamoduleFlag;//no data

	unsigned int    Type;//0:ITKF;1:TKF1-8
	unsigned int    ChainSet;
	unsigned int	ReadoutEventNum;
	unsigned int	ReadoutEventWindow;
	unsigned int    ProgramFull;
	unsigned int    ProgramFullDisable;
	unsigned int    L1Select;
	unsigned int    ReadoutModeSelect;
	unsigned int    L1LatencyToHit;
	unsigned int    TSFLatencyToHit;
	unsigned int    TrackLatencyToHit;
	unsigned int    FiberSyncLatencyToFRST;
	unsigned int    FiberChannelSelect;

	unsigned int	HitPreSetChannel[16];
	unsigned int	Spare[5];
	
	unsigned int    DataSelect;
	unsigned int	unused;
}trg_tkf_t __attribute__ ((packed));

void Show_trg_tkf(trg_tkf_t * pConf);

/******************************************/
typedef struct trg_tof
{
	unsigned int	info_len;
	unsigned int	module_type;
	unsigned int	Position;
	unsigned int    DatamoduleFlag;//no data
	
	unsigned int    WorkMode;
	unsigned int	FiberChannelShield1;//for channel 0-31
	unsigned int	FiberChannelShield2;//for channel 32\33 
	unsigned int	SubstituteChannel1;
	unsigned int	SubstituteChannel2;
	unsigned int	SubstituteChannel3;
	unsigned int    ControlParameter;//one bit represent one channel
	unsigned int    DisposeParameter;
	unsigned int    PhysAddress;
	unsigned int    CR_CSR;
	unsigned int    TLK_ON;
	unsigned int	unused;
}trg_tof_t __attribute__ ((packed));

void Show_trg_tof(trg_tof_t * pConf);

/******************************************/
typedef struct trg_etf
{
	unsigned int	info_len;
	unsigned int	module_type;
	unsigned int	Position;
	unsigned int    DatamoduleFlag;//no data
	
	unsigned int    readoutWindow;
	unsigned int    CBLTSet;
	unsigned int	map[18];
	unsigned int	unused;
}trg_etf_t __attribute__ ((packed));

void Show_trg_etf(trg_etf_t * pConf);


/******************************************/

/******************************************/

typedef struct trg_mutl
{
	unsigned int	info_len;
	unsigned int	module_type;
	unsigned int	Position;
	unsigned int    DatamoduleFlag;//no data
	
	unsigned int    ChainSet;
	unsigned int	ReadoutEventNum;
	unsigned int    ReadoutEventWindow;//one bit represent one channel
	unsigned int    ProgramFull;
	unsigned int    ProgramFullDisable;
	unsigned int    L1Select;
	unsigned int    ReadoutModeSelect;
	unsigned int	L1LatencyToHit;
	unsigned int    TSFLatencyToHit;
	unsigned int    TrackLatencyToHit;
	unsigned int    FiberSyncLatencyToFRST;
	unsigned int    FiberChannelSelect;
	unsigned int    DataSelect;
	unsigned int	unused;	
}trg_mutl_t __attribute__ ((packed));

void Show_trg_mutl(trg_mutl_t * pConf);


/******************************************/
typedef struct trg_basg
{
	unsigned int	info_len;
	unsigned int	module_type;
	unsigned int	Position;
	unsigned int    DatamoduleFlag;//no data
	
	unsigned int    ParSetChannel[48];
	unsigned int    UserCSR;
	unsigned int    ChannelMaskCSR01;
	unsigned int    ChannelMaskCSR02;
	unsigned int    BASWidth;
	unsigned int    ChannelSelForLight;
	unsigned int    SetBackUp[9];
	
	unsigned int	unused;
}trg_basg_t __attribute__ ((packed));

void Show_trg_basg(trg_basg_t * pConf);
/******************************************/
typedef struct trg_btm
{
	unsigned int	info_len;
	unsigned int	module_type;
	unsigned int	Position;
	unsigned int    DatamoduleFlag;//no data

	unsigned int    CBLTSet;
	unsigned int    L1Select;
	unsigned int	unused;	

}trg_btm_t __attribute__ ((packed));

void Show_trg_btm(trg_btm_t * pConf);

typedef struct trg_etm
{
	unsigned int	info_len;
	unsigned int	module_type;
	unsigned int	Position;
	unsigned int    DatamoduleFlag;//no data

	unsigned int    CBLTSet;
	unsigned int    L1Select;
	unsigned int	unused;	

}trg_etm_t __attribute__ ((packed));

void Show_trg_etm(trg_etm_t * pConf);


typedef struct trg_ets
{
	unsigned int	info_len;
	unsigned int	module_type;
	unsigned int	Position;
	unsigned int    DatamoduleFlag;//no data

//	unsigned int    CBLTSet;
	unsigned int	unused;	
}trg_ets_t __attribute__ ((packed));
void Show_trg_ets(trg_ets_t * pConf);

typedef struct zdd_v1721
{
	unsigned int	info_len;
	unsigned int	module_type;
	unsigned int	Position;
	unsigned int    DatamoduleFlag;//no data

	unsigned int BaseAddr ;             /* Base address of module*/
	unsigned int customSize;            /* Length of time history*/
	unsigned int customOut;             /* Length of buffer out*/
	unsigned int initDACs;              /* Initialize DACs at config time*/
	unsigned int compressData;          /* Data compress flag*/
	unsigned int baseLine[8];           /* Baselines*/
	unsigned int threshold[8];          /* Data compress thresholds*/
	unsigned int nLookBack[8];           /* Samples for data compression*/
	unsigned int nLookForward[8];           /* Samples for data compression*/
	unsigned int unused;	
}zdd_v1721_t __attribute__ ((packed));
void Show_zdd_v1721(zdd_v1721_t * pConf);

typedef struct zdd_v1190
{
	unsigned int	info_len;
	unsigned int	module_type;
	unsigned int	Position;
	unsigned int    DatamoduleFlag;//no data

	unsigned int BaseAddr ;             /* Base address of module*/
	unsigned int WindowOffset;
	unsigned int WindowWidth;
	unsigned int unused;	
}zdd_v1190_t __attribute__ ((packed));
void Show_zdd_v1190(zdd_v1190_t * pConf);



#endif
