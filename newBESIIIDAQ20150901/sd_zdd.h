#ifndef __INCSDZDDh
#define __INCSDZDDh

#include "msgQLib.h"
#include "rng_dma.h"

#define V1190ENB 0
#define ZDD_V1721_AMOUNT 2
#define ZDD_V1190_AMOUNT 1

#define ZDD_MAX_BYTES_PERMDL 10240

#define MAXNUM_ZDDMSGS 500
#define ZDDRNG_SIZE 200*4096

struct zdd_db_info
{
	int ppc_sum ;
	struct GE_ppc  *ppc_header  ;

	int v1721_sum ;
	struct zdd_v1721 *v1721_header[ZDD_V1721_AMOUNT]  ;

	int v1190_sum ;
	struct zdd_v1190 *v1190_header[ZDD_V1190_AMOUNT]  ;
	
	int mcc_sum ;
	struct mdc_mcc * mcc_header ;
	int mti_sum;
	struct mdc_mti * 	mti_header;	
};

struct zdd_hw_info
{
	int v1721_sum ;
	int v1721_geo[ZDD_V1721_AMOUNT] ;
	int v1190_sum ;
	int v1190_geo[ZDD_V1190_AMOUNT] ;	
};

extern struct zdd_db_info  g_ZDDDBInfo;
extern struct zdd_hw_info g_ZDDHWInfo;

extern SEM_ID g_sidZddSpecial; /*leigkleigk 060820*/

extern unsigned short gZddV1721Geo[ZDD_V1721_AMOUNT];
extern unsigned short gZddV1190Geo[ZDD_V1190_AMOUNT];


extern unsigned int delta_triggerNum;

extern UINT32 CustomOut;
extern UINT32 CompressionMode;
extern UINT32 NoSupDataLen;


extern bool b_singleBoard;	

extern DMA_RING_ID	g_ZDDRng[2];		
extern MSG_Q_ID	g_msgQZDD[2];

extern UINT trigger_Diff ;
extern UINT trigger_Diff_last ;


void ZDDLinkRtns() ;

#define INTERRUPTEVENTNUM 20

#define CBLTMSB 0xaa

#define CBLT_ENABLE 1
#define INTERRUPT_ENABLE 1

typedef enum cdCompressData {
        zsNOZEROSUPP = 0x00000000,
        zsZEROLENGTH = 0x00020000,
        zsZEROSAMP   = 0x00030000
} cdCompressData;

typedef enum trOnInput {
        tiOVERTHRESHOLD = 0x00000000,
        tiUNDERTHRESHOLD= 0x00000040
} trOnInput;

typedef enum memSeqAccess {
        msDISABLED = 0x00000000,
        msENABLED  = 0x00000010
} memSeqAccess;

typedef enum testPattern {
        tpDISABLED = 0x00000000,
        tpENABLED  = 0x00000008
} testPattern;

typedef enum triggerOverlap {
        toDISABLED = 0x00000000,
        toENABLED  = 0x00000002
} triggerOverlap;

typedef enum RunMode {
        rmREGISTERCONTROLLED = 0x00,
        rmS_INC_ONTROLLED    = 0x01,
        rmS_IN_GATDE         = 0x02,
        rmMULTI_BOARD_SYNC   = 0x03
} RunMode;

typedef enum isRunning {
        rnRUNNING = 0x00,
        rnSTOPPED = 0x04
} isRunning;

typedef enum countTrigs {
        ctACCEPTED = 0x00,
        ctALL      = 0x08
} countTrigs;

typedef enum BerrMode{
        bmENABLE = 0x10,
        bmDISABLE  = 0x00
} BerrMode;

typedef enum AlignMode{
        Align64ENABLE = 0x20,
        Align64DISABLE  = 0x00
} AlignMode;


typedef enum InterruptMode{
	imRORAMODE = 0x00,
	imROAKMODE = 0x80
}InterruptMode;

typedef enum ModulePosition{
	mpFIRSTBOARD = 0x2,
	mpMIDDLEBOARD = 0x3,
	mpLASTBOARD = 0x1,
	mpDISABLEBOARD = 0x0
}ModulePosition;

typedef enum DataMode {
        dmNOZEROSUPP = 0x0,
        dmZEROLENGTH = 0x1,
        dmZEROSAMP   = 0x2
} DataMode;

typedef enum TriggerType{
	ttSOFTTTG = 0x80000000,
	ttEXTTRG = 0x40000000
}TriggerType;

#endif

