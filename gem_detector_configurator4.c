#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>
#define BUFSIZE 4096 
#define HOST_IP "192.168.1.200"
#define HOST_PORT  54831
#define HOST_PORT_RECEIVE  58927
#define DEST_IP_ADDRESS  "192.168.1.23"
#define DEST_PORT_NO  58913

      unsigned int TriggerMode_default_TP_Enabled;
      unsigned int command_code_shift = 11;
      unsigned int target_TIGER_ID_shift = 8;
      unsigned int gemroc_TARGET_GEMROC_ID;
      unsigned int gemroc_parameter_array[35]={0, 0, 0, 0, 0, 16, 15, 1023, 2048, 1536, 358, 0, 0, 0, 0, 0, 0, 63, 511, 511, 511, 511, 511, 511, 511, 511, 511, 511, 511, 511, 3, 3, 3, 3, 1};
      unsigned int gemroc_EN_TM_TCAM_pattern;
      unsigned int gemroc_AUTO_TP_EN_pattern;
      unsigned int gemroc_AUTO_L1_EN_pattern;
      unsigned int gemroc_Periodic_TP_EN_pattern;
      unsigned int gemroc_Periodic_L1_EN_pattern;
      unsigned int gemroc_TP_period;
      unsigned int gemroc_TP_width;
      unsigned int gemroc_L1_period;
      unsigned int gemroc_LowerDataScanWindowOffset;
      unsigned int gemroc_UpperDataScanWindowOffset;
      unsigned int gemroc_L1_latency;
      unsigned int gemroc_FEB_PWR_EN_pattern;
      unsigned int gemroc_FEB_OVC_EN_pattern;
      unsigned int gemroc_FEB_OVV_EN_pattern;
      unsigned int gemroc_FEB_OVT_EN_pattern;
      unsigned int gemroc_ROC_OVT_EN;
      unsigned int gemroc_TL_nTM_ACQ;
      unsigned int gemroc_ROC_OVT_LIMIT;
      unsigned int gemroc_OVC_LIMIT_FEB0;
      unsigned int gemroc_OVV_LIMIT_FEB0;
      unsigned int gemroc_OVT_LIMIT_FEB0;
      unsigned int gemroc_OVC_LIMIT_FEB1;
      unsigned int gemroc_OVV_LIMIT_FEB1;
      unsigned int gemroc_OVT_LIMIT_FEB1;
      unsigned int gemroc_OVC_LIMIT_FEB2;
      unsigned int gemroc_OVV_LIMIT_FEB2;
      unsigned int gemroc_OVT_LIMIT_FEB2;
      unsigned int gemroc_OVC_LIMIT_FEB3;
      unsigned int gemroc_OVV_LIMIT_FEB3;
      unsigned int gemroc_OVT_LIMIT_FEB3;
      unsigned int gemroc_TIMING_DLY_FEB0;
      unsigned int gemroc_TIMING_DLY_FEB1;
      unsigned int gemroc_TIMING_DLY_FEB2;
      unsigned int gemroc_TIMING_DLY_FEB3;
      unsigned int gemroc_TP_Pos_nNeg;
      char* gemroc_command_string;
      unsigned int gemroc_target_TCAM_ID;
      unsigned int gemroc_to_ALL_TCAM_enable;
      unsigned int gemroc_number_of_repetitions;
      char* gemroc_command_list[13]={
                                  "NONE",
                                  "WR",
                                  "RD",
                                  "TIGER_SYNCH_RST",
                                  "TCAM_SYNCH_RST",
                                  "TP_GEN",
                                  "L1_GEN",
                                  "XCVR_LOOPBACK_TEST_EN",
                                  "ACK_FRAME_SEQ_ERR_FLAG",
                                  "IVT_UPDATE",
                                  "IVT_READ",
                                  "CMD_GEMROC_TIGER_CFG_RESET",
                                  "FEB_TIMING_DELAYS_UPDATE"  };
      unsigned int gemroc_is_a_write = 0x1;
      unsigned int gemroc_command_words[11];
      unsigned int gemroc_gemroc_cmd_code;	

      unsigned int g_reg_TARGET_GEMROC_ID;
      char*g_reg_cfg_filename;
      unsigned int g_reg_parameter_array[38] ={0, 0, 0, 59, 19, 10, 5, 0, 5, 13, 54, 8, 59, 0, 0, 59, 19, 1, 0, 13, 29, 57, 1, 1, 0, 0, 9, 1, 6, 0, 0, 7, 0, 0, 0, 2, 0, 1};
      unsigned int g_reg_BufferBias;
      unsigned int g_reg_TDCVcasN;
      unsigned int g_reg_TDCVcasP; 
      unsigned int g_reg_TDCVcasPHyst; 
      unsigned int g_reg_DiscFE_Ibias; 
      unsigned int g_reg_BiasFE_PpreN; 
      unsigned int g_reg_AVcasp_global;
      unsigned int g_reg_TDCcompVcas; 
      unsigned int g_reg_TDCIres; 
      unsigned int g_reg_DiscVcasN; 
      unsigned int g_reg_IntegVb1; 
      unsigned int g_reg_BiasFE_A1;
      unsigned int g_reg_Vcasp_Vth;
      unsigned int g_reg_TAC_I_LSB;
      unsigned int g_reg_TDCcompVbias;
      unsigned int g_reg_Vref_Integ; 
      unsigned int g_reg_IBiasTPcal;
      unsigned int g_reg_TP_Vcal; 
      unsigned int g_reg_ShaperIbias;
      unsigned int g_reg_IPostamp; 
      unsigned int g_reg_TP_Vcal_ref;
      unsigned int g_reg_Vref_integ_diff;
      unsigned int g_reg_Sig_pol; 
      unsigned int g_reg_FE_TPEnable;
      unsigned int g_reg_CompactDataFormat; 
      unsigned int g_reg_DataClkDiv; 
      unsigned int g_reg_TACrefreshPeriod; 
      unsigned int g_reg_TACrefreshEnable; 
      unsigned int g_reg_CounterPeriod; 
      unsigned int g_reg_CounterEnable; 
      unsigned int g_reg_StopRampEnable; 
      unsigned int g_reg_RClkEnable; 
      unsigned int g_reg_TDCClkdiv; 
      unsigned int g_reg_VetoMode; 
      unsigned int g_reg_Ch_DebugMode;
      unsigned int g_reg_TxMode; 
      unsigned int g_reg_TxDDR; 
      unsigned int g_reg_TxLinks;
      unsigned int g_reg_is_a_write = 0x1;
      unsigned int g_reg_target_TIGER_ID = 0x1;
      unsigned int g_reg_command_code = 0x9;
      unsigned int g_reg_command_words[12];

      unsigned int ch_reg_TARGET_GEMROC_ID;
//    char* ch_reg_cfg_filename;
      unsigned int ch_reg_parameter_array[30]={1, 2, 2, 0, 0, 0, 0, 0, 0, 1, 0, 0, 1, 6, 6, 1, 0, 0, 0, 0, 0, 0, 0, 10, 31, 0, 0, 0, 0, 0};
      
      unsigned int ch_reg_cmd_header;
      unsigned int ch_reg_cmd_word0;
      unsigned int ch_reg_cmd_word1;
      unsigned int ch_reg_cmd_word2;
      unsigned int ch_reg_cmd_word3;
      unsigned int ch_reg_cmd_word4;
      unsigned int ch_reg_cmd_word5;
      unsigned int ch_reg_cmd_word6;
      unsigned int ch_reg_cmd_word7;
      unsigned int ch_reg_cmd_word8;     
      unsigned int ch_reg_DisableHyst; 
      unsigned int ch_reg_T2Hyst;
      unsigned int ch_reg_T1Hyst;
      unsigned int ch_reg_Ch63ObufMSB; 
      unsigned int ch_reg_TP_disable_FE; 
      unsigned int ch_reg_TDC_IB_E; 
      unsigned int ch_reg_TDC_IB_T; 
      unsigned int ch_reg_Integ; 
      unsigned int ch_reg_PostAmpGain; 
      unsigned int ch_reg_FE_delay; 
      unsigned int ch_reg_Vth_T2; 
      unsigned int ch_reg_Vth_T1; 
      unsigned int ch_reg_QTx2Enable; 
      unsigned int ch_reg_MaxIntegTime; 
      unsigned int ch_reg_MinIntegTime; 
      unsigned int ch_reg_TriggerBLatched; 
      unsigned int ch_reg_QdcMode; 
      unsigned int ch_reg_BranchEnableT; 
      unsigned int ch_reg_BranchEnableEQ; 
      unsigned int ch_reg_TriggerMode2B; 
      unsigned int ch_reg_TriggerMode2Q; 
      unsigned int ch_reg_TriggerMode2E; 
      unsigned int ch_reg_TriggerMode2T; 
      unsigned int ch_reg_TACMinAge; 
      unsigned int ch_reg_TACMaxAge; 
      unsigned int ch_reg_CounterMode; 
      unsigned int ch_reg_DeadTime; 
      unsigned int ch_reg_SyncChainLen; 
      unsigned int ch_reg_Ch_DebugMode; 
      unsigned int ch_reg_TriggerMode; 
      unsigned int ch_reg_is_a_write = 0x1;
      unsigned int ch_reg_target_TIGER_ID = 0x1;
      unsigned int ch_reg_command_code = 0x9;
      unsigned int ch_reg_TO_ALL_enable = 0x1;
      unsigned int ch_reg_channel_ID = 0x3;
      unsigned int ch_reg_command_words[10];
      unsigned int TriggerMode_default_TP_Disabled;
void ch_reg_init(int TARGET_GEMROC_ID_param)
{     unsigned int header_tag;
      unsigned int Channel_cfg_cmd_ID;
      unsigned int ccfg_cmd_tag;
      unsigned int ccfg_cmd_word_count;
      ch_reg_TARGET_GEMROC_ID = TARGET_GEMROC_ID_param;
//      ch_reg_cfg_filename = cfg_filename_param
      ch_reg_DisableHyst = ch_reg_parameter_array[0];
      ch_reg_T2Hyst = ch_reg_parameter_array[1];
      ch_reg_T1Hyst = ch_reg_parameter_array[2];
      ch_reg_Ch63ObufMSB = ch_reg_parameter_array[3];
      ch_reg_TP_disable_FE = ch_reg_parameter_array[4];
      ch_reg_TDC_IB_E = ch_reg_parameter_array[5];
      ch_reg_TDC_IB_T = ch_reg_parameter_array[6];
      ch_reg_Integ = ch_reg_parameter_array[7];
      ch_reg_PostAmpGain = ch_reg_parameter_array[8];
      ch_reg_FE_delay = ch_reg_parameter_array[9];
      ch_reg_Vth_T2 = ch_reg_parameter_array[10];
      ch_reg_Vth_T1 = ch_reg_parameter_array[11];
      ch_reg_QTx2Enable = ch_reg_parameter_array[12];
      ch_reg_MaxIntegTime = ch_reg_parameter_array[13];
      ch_reg_MinIntegTime = ch_reg_parameter_array[14];
      ch_reg_TriggerBLatched = ch_reg_parameter_array[15];
      ch_reg_QdcMode = ch_reg_parameter_array[16];
      ch_reg_BranchEnableT = ch_reg_parameter_array[17];
      ch_reg_BranchEnableEQ = ch_reg_parameter_array[18];
      ch_reg_TriggerMode2B = ch_reg_parameter_array[19];
      ch_reg_TriggerMode2Q = ch_reg_parameter_array[20];
      ch_reg_TriggerMode2E = ch_reg_parameter_array[21];
      ch_reg_TriggerMode2T = ch_reg_parameter_array[22];
      ch_reg_TACMinAge = ch_reg_parameter_array[23];
      ch_reg_TACMaxAge = ch_reg_parameter_array[24];
      ch_reg_CounterMode = ch_reg_parameter_array[25];
      ch_reg_DeadTime = ch_reg_parameter_array[26];
      ch_reg_SyncChainLen = ch_reg_parameter_array[27];
      ch_reg_Ch_DebugMode = ch_reg_parameter_array[28];
      ch_reg_TriggerMode = ch_reg_parameter_array[29];
      ch_reg_is_a_write = 0x1;
      ch_reg_target_TIGER_ID = 0x1;
      ch_reg_command_code = 0x9;
      ch_reg_TO_ALL_enable = 0x1;
      ch_reg_channel_ID = 0x3;
      header_tag = 0x8 << 28;
      Channel_cfg_cmd_ID = 0xE;
      ccfg_cmd_tag = Channel_cfg_cmd_ID << 24;
      ccfg_cmd_word_count = 9;
      ch_reg_cmd_header = header_tag + (ch_reg_TARGET_GEMROC_ID << 16) + ccfg_cmd_tag + ccfg_cmd_word_count;
      ch_reg_cmd_word8 = ((ch_reg_DisableHyst & 0x1) << 24) + ((ch_reg_T2Hyst & 0x7) << 16) + ((ch_reg_T1Hyst & 0x7) << 8) + ((ch_reg_Ch63ObufMSB & 0x1));
      ch_reg_cmd_word7 = ((ch_reg_TP_disable_FE & 0x1) << 24) + ((ch_reg_TDC_IB_E & 0xF) << 16) + ((ch_reg_TDC_IB_T & 0xF) << 8) + ((ch_reg_Integ & 0x1));
      ch_reg_cmd_word6 = ((ch_reg_PostAmpGain & 0x3) << 24) + ((ch_reg_FE_delay & 0x1F) << 16) + ((ch_reg_Vth_T2 & 0x3F) << 8) + ((ch_reg_Vth_T1 & 0x3F));
      ch_reg_cmd_word5 = ((ch_reg_QTx2Enable & 0x1) << 24) + ((ch_reg_MaxIntegTime & 0x7F) << 16) + ((ch_reg_MinIntegTime & 0x7F) << 8) + ((ch_reg_TriggerBLatched & 0x1));
      ch_reg_cmd_word4 = ((ch_reg_QdcMode & 0x1) << 24) + ((ch_reg_BranchEnableT & 0x1) << 16) + ((ch_reg_BranchEnableEQ & 0x1) << 8) + ((ch_reg_TriggerMode2B & 0x7));
      ch_reg_cmd_word3 = ((ch_reg_TriggerMode2Q & 0x3) << 24) + ((ch_reg_TriggerMode2E & 0x7) << 16) + ((ch_reg_TriggerMode2T & 0x3) << 8) + ((ch_reg_TACMinAge & 0x1F));
      ch_reg_cmd_word2 = ((ch_reg_TACMaxAge & 0x1F) << 24) + ((ch_reg_CounterMode & 0xF) << 16) + ((ch_reg_DeadTime & 0x3F) << 8) + ((ch_reg_SyncChainLen & 0x3));
      ch_reg_cmd_word1 = ((ch_reg_Ch_DebugMode & 0x3) << 24) + ((ch_reg_TriggerMode & 0x3) << 16);
      ch_reg_cmd_word0 = ((ch_reg_command_code & 0xF) << 11) + ((ch_reg_target_TIGER_ID & 0x7) << 8) + ((ch_reg_is_a_write & 0x1) << 7) + ((ch_reg_TO_ALL_enable & 0x1) << 6) + (ch_reg_channel_ID & 0x3F);  

ch_reg_command_words[9]=ch_reg_cmd_word0;
ch_reg_command_words[8]=ch_reg_cmd_word1;
ch_reg_command_words[7]=ch_reg_cmd_word2;
ch_reg_command_words[6]=ch_reg_cmd_word3;
ch_reg_command_words[5]=ch_reg_cmd_word4;
ch_reg_command_words[4]=ch_reg_cmd_word5;
ch_reg_command_words[3]=ch_reg_cmd_word6;
ch_reg_command_words[2]=ch_reg_cmd_word7;
ch_reg_command_words[1]=ch_reg_cmd_word8;
ch_reg_command_words[0]=ch_reg_cmd_header;

 }


int ch_reg_cmd_words_array_size()
{
return sizeof(ch_reg_command_words);
}

void ch_reg_print_command_words()
{
int i;
for(i=0;i<10;i++)
printf("%08X\n",ch_reg_command_words[i]);
}

void ch_reg_set_target_GEMROC(int GEMROC_ID_param)
{
      ch_reg_TARGET_GEMROC_ID = GEMROC_ID_param & 0x1f;
      ch_reg_cmd_header = (ch_reg_cmd_header & 0xFF00FFFF) + (ch_reg_TARGET_GEMROC_ID << 16);
      ch_reg_command_words[0]=ch_reg_cmd_header;
}

void ch_reg_set_target_TIGER(int target_TIGER_param)
{
      ch_reg_target_TIGER_ID = target_TIGER_param & 0x7;
      ch_reg_cmd_word0= ((ch_reg_command_code & 0xF) << 12) + ((ch_reg_target_TIGER_ID & 0x7) << 8) + ((ch_reg_is_a_write & 0x1) << 7) + ((ch_reg_TO_ALL_enable & 0x1) << 6) + (ch_reg_channel_ID & 0x1F);
      ch_reg_command_words[9]=ch_reg_cmd_word0;
}

void ch_reg_set_target_channel(int target_channel_param)
{
      ch_reg_channel_ID = target_channel_param & 0x1F;
      ch_reg_cmd_word0 = ((ch_reg_command_code & 0xF) << 12) + ((ch_reg_target_TIGER_ID & 0x7) << 8) + ((ch_reg_is_a_write & 0x1) << 7) + ((ch_reg_TO_ALL_enable & 0x1) << 6) + (ch_reg_channel_ID & 0x1F);
      ch_reg_command_words[9]=ch_reg_cmd_word0;
}

void ch_reg_set_to_ALL_param(int to_ALL_param)
{
      ch_reg_TO_ALL_enable = to_ALL_param & 0x1;
      ch_reg_cmd_word0 = ((ch_reg_command_code & 0xF) << 12) + ((ch_reg_target_TIGER_ID & 0x7) << 8) + ((ch_reg_is_a_write & 0x1) << 7) + ((ch_reg_TO_ALL_enable & 0x1) << 6) + (ch_reg_channel_ID & 0x1F);
      ch_reg_command_words[9]=ch_reg_cmd_word0;
}


void ch_reg_set_command_code(char* command_code_param)
{
      if (command_code_param == "WR"){
         ch_reg_command_code = 0x0;
         ch_reg_is_a_write = 1;
         ch_reg_cmd_word0 = ((ch_reg_command_code & 0xF) << 12) + ((ch_reg_target_TIGER_ID & 0x7) << 8) + ((ch_reg_is_a_write & 0x1) << 7) + ((ch_reg_TO_ALL_enable & 0x1) << 6) + (ch_reg_channel_ID & 0x1F);
         ch_reg_command_words[9]=ch_reg_cmd_word0;}
      else if (command_code_param == "RD"){
         ch_reg_command_code = 0x1;
         ch_reg_is_a_write = 0;
         ch_reg_cmd_word0 = ((ch_reg_command_code & 0xF) << 12) + ((ch_reg_target_TIGER_ID & 0x7) << 8) + ((ch_reg_is_a_write & 0x1) << 7) + ((ch_reg_TO_ALL_enable & 0x1) << 6) + (ch_reg_channel_ID & 0x1F);
         ch_reg_command_words[9]=ch_reg_cmd_word0;}
      else
         printf("ch_reg:bad command_code parameter passed");
}

 void ch_reg_update_command_words()
{
      if ( (ch_reg_command_code & 0xF) == 0x1 ){
         ch_reg_cmd_word1  = 0;
         ch_reg_cmd_word2  = 0;
         ch_reg_cmd_word3  = 0;
         ch_reg_cmd_word4  = 0;
         ch_reg_cmd_word5  = 0;
         ch_reg_cmd_word6  = 0;
         ch_reg_cmd_word7  = 0;
         ch_reg_cmd_word8  = 0;}
      else{
         ch_reg_cmd_word8 = ((ch_reg_DisableHyst & 0x1) << 24)   + ((ch_reg_T2Hyst & 0x7) << 16)        + ((ch_reg_T1Hyst & 0x7) << 8)         + ((ch_reg_Ch63ObufMSB & 0x1));
         ch_reg_cmd_word7 = ((ch_reg_TP_disable_FE & 0x1) << 24) + ((ch_reg_TDC_IB_E & 0xF) << 16)      + ((ch_reg_TDC_IB_T & 0xF) << 8)       + ((ch_reg_Integ & 0x1));
         ch_reg_cmd_word6 = ((ch_reg_PostAmpGain & 0x3) << 24)   + ((ch_reg_FE_delay & 0x1F) << 16)     + ((ch_reg_Vth_T2 & 0x3F) << 8)        + ((ch_reg_Vth_T1 & 0x3F));
         ch_reg_cmd_word5 = ((ch_reg_QTx2Enable & 0x1) << 24)    + ((ch_reg_MaxIntegTime & 0x7F) << 16) + ((ch_reg_MinIntegTime & 0x7F) << 8)  + ((ch_reg_TriggerBLatched & 0x1));
         ch_reg_cmd_word4 = ((ch_reg_QdcMode & 0x1) << 24)       + ((ch_reg_BranchEnableT & 0x1) << 16) + ((ch_reg_BranchEnableEQ & 0x1) << 8) + ((ch_reg_TriggerMode2B & 0x7));
         ch_reg_cmd_word3 = ((ch_reg_TriggerMode2Q & 0x3) << 24) + ((ch_reg_TriggerMode2E & 0x7) << 16) + ((ch_reg_TriggerMode2T & 0x3) << 8)  + ((ch_reg_TACMinAge & 0x1F));
         ch_reg_cmd_word2 = ((ch_reg_TACMaxAge & 0x1F) << 24)    + ((ch_reg_CounterMode & 0xF) << 16)   + ((ch_reg_DeadTime & 0x3F) << 8)      + ((ch_reg_SyncChainLen & 0x3));
         ch_reg_cmd_word1 = ((ch_reg_Ch_DebugMode & 0x3) << 24)  + ((ch_reg_TriggerMode & 0x3) << 16);}
         ch_reg_cmd_word0= ((ch_reg_command_code & 0xF) << command_code_shift) + ((ch_reg_target_TIGER_ID & 0x7) << target_TIGER_ID_shift) + ((ch_reg_TO_ALL_enable & 0x1) << 6) + (ch_reg_channel_ID & 0x1F);

ch_reg_command_words[9]=ch_reg_cmd_word0;
ch_reg_command_words[8]=ch_reg_cmd_word1;
ch_reg_command_words[7]=ch_reg_cmd_word2;
ch_reg_command_words[6]=ch_reg_cmd_word3;
ch_reg_command_words[5]=ch_reg_cmd_word4;
ch_reg_command_words[4]=ch_reg_cmd_word5;
ch_reg_command_words[3]=ch_reg_cmd_word6;
ch_reg_command_words[2]=ch_reg_cmd_word7;
ch_reg_command_words[1]=ch_reg_cmd_word8;
ch_reg_command_words[0]=ch_reg_cmd_header;
}



 g_reg_init(int TARGET_GEMROC_ID_param/*,char*cfg_filename_param*/)
{     int i;
      unsigned int header_tag = 0x8 << 28;
      unsigned int Global_cfg_cmd_ID = 0xF;
      unsigned int gcfg_cmd_tag = Global_cfg_cmd_ID << 24;
      unsigned int gcfg_cmd_word_count = 11;
      g_reg_TARGET_GEMROC_ID = TARGET_GEMROC_ID_param;
//      g_reg_cfg_filename = cfg_filename_param;
      g_reg_BufferBias = g_reg_parameter_array[0];
      g_reg_TDCVcasN = g_reg_parameter_array[1];
      g_reg_TDCVcasP = g_reg_parameter_array[2];
      g_reg_TDCVcasPHyst = g_reg_parameter_array[3];
      g_reg_DiscFE_Ibias = g_reg_parameter_array[4];
      g_reg_BiasFE_PpreN = g_reg_parameter_array[5];
      g_reg_AVcasp_global = g_reg_parameter_array[6];
      g_reg_TDCcompVcas = g_reg_parameter_array[7];
      g_reg_TDCIres = g_reg_parameter_array[8];
      g_reg_DiscVcasN = g_reg_parameter_array[9];
      g_reg_IntegVb1 = g_reg_parameter_array[10];
      g_reg_BiasFE_A1 = g_reg_parameter_array[11];
      g_reg_Vcasp_Vth = g_reg_parameter_array[12];
      g_reg_TAC_I_LSB = g_reg_parameter_array[13];
      g_reg_TDCcompVbias = g_reg_parameter_array[14];
      g_reg_Vref_Integ = g_reg_parameter_array[15]; 
      g_reg_IBiasTPcal = g_reg_parameter_array[16];
      g_reg_TP_Vcal = g_reg_parameter_array[17]; 
      g_reg_ShaperIbias = g_reg_parameter_array[18];
      g_reg_IPostamp = g_reg_parameter_array[19];
      g_reg_TP_Vcal_ref = g_reg_parameter_array[20];
      g_reg_Vref_integ_diff = g_reg_parameter_array[21];
      g_reg_Sig_pol = g_reg_parameter_array[22];
      g_reg_FE_TPEnable = g_reg_parameter_array[23];
      g_reg_CompactDataFormat = g_reg_parameter_array[24];
      g_reg_DataClkDiv = g_reg_parameter_array[25];
      g_reg_TACrefreshPeriod = g_reg_parameter_array[26];
      g_reg_TACrefreshEnable = g_reg_parameter_array[27];
      g_reg_CounterPeriod = g_reg_parameter_array[28];
      g_reg_CounterEnable = g_reg_parameter_array[29];
      g_reg_StopRampEnable = g_reg_parameter_array[30];
      g_reg_RClkEnable = g_reg_parameter_array[31];
      g_reg_TDCClkdiv = g_reg_parameter_array[32];
      g_reg_VetoMode = g_reg_parameter_array[33];
      g_reg_Ch_DebugMode = g_reg_parameter_array[34];
      g_reg_TxMode = g_reg_parameter_array[35];
      g_reg_TxDDR = g_reg_parameter_array[36];
      g_reg_TxLinks = g_reg_parameter_array[37];
      g_reg_command_words[0] = header_tag + (g_reg_TARGET_GEMROC_ID << 16) + gcfg_cmd_tag + gcfg_cmd_word_count;
      g_reg_command_words[1] = ((g_reg_BufferBias & 0x3) << 24) + ((g_reg_TDCVcasN & 0xF) << 16) + ((g_reg_TDCVcasP & 0x1F) << 8) + ((g_reg_TDCVcasPHyst & 0x3F));
      g_reg_command_words[2] = ((g_reg_DiscFE_Ibias & 0x3f) << 24) + ((g_reg_BiasFE_PpreN & 0x3F) << 16) + ((g_reg_AVcasp_global & 0x1F) << 8) + ((g_reg_TDCcompVcas & 0xF));
      g_reg_command_words[3] = ((g_reg_TDCIres & 0x1f) << 24) + ((g_reg_DiscVcasN & 0xF) << 16) + ((g_reg_IntegVb1 & 0x3F) << 8) + ((g_reg_BiasFE_A1 & 0xF));
      g_reg_command_words[4] = ((g_reg_Vcasp_Vth & 0x3f) << 24) + ((g_reg_TAC_I_LSB & 0x1F) << 16) + ((g_reg_TDCcompVbias & 0x1F) << 8) + ((g_reg_Vref_Integ & 0x3F));
      g_reg_command_words[5] = ((g_reg_IBiasTPcal & 0x1f) << 24) + ((g_reg_TP_Vcal & 0x1F) << 16) + ((g_reg_ShaperIbias & 0xF) << 8) + ((g_reg_IPostamp & 0x1F));
      g_reg_command_words[6] = ((g_reg_TP_Vcal_ref & 0x1f) << 24) + ((g_reg_Vref_integ_diff & 0x3F) << 16) + ((g_reg_Sig_pol & 0x1) << 8) + ((g_reg_FE_TPEnable & 0x1));
      g_reg_command_words[7] = ((g_reg_CompactDataFormat & 0x1) << 24) + ((g_reg_DataClkDiv & 0x3) << 16) + ((g_reg_TACrefreshPeriod & 0xf) << 8) + ((g_reg_TACrefreshEnable & 0x1));
      g_reg_command_words[8] = ((g_reg_CounterPeriod & 0x7) << 24) + ((g_reg_CounterEnable & 0x1) << 16) + ((g_reg_StopRampEnable & 0x3) << 8) + ((g_reg_RClkEnable & 0x1F));
      g_reg_command_words[9] = ((g_reg_TDCClkdiv & 0x1) << 24) + ((g_reg_VetoMode & 0x3F) << 16) + ((g_reg_Ch_DebugMode & 0x1) << 8) + ((g_reg_TxMode & 0x3));
      g_reg_command_words[10] = ((g_reg_TxDDR & 0x1) << 24) + ((g_reg_TxLinks & 0x3) << 16);
      g_reg_command_words[11] = ((g_reg_command_code & 0xF) << command_code_shift) + ((g_reg_target_TIGER_ID & 0x7) << target_TIGER_ID_shift);    }


int g_reg_cmd_words_array_size()
{
return sizeof(g_reg_command_words);
}

void g_reg_print_command_words()
{int i;
for(i=0;i<12;i++)
printf("%08X\n",g_reg_command_words[i]);
}

void g_reg_set_target_GEMROC(int GEMROC_ID_param)
{
g_reg_TARGET_GEMROC_ID = GEMROC_ID_param;
g_reg_command_words[0] = (g_reg_command_words[0] & 0xFF00FFFF) + (g_reg_TARGET_GEMROC_ID << 16);
}

void g_reg_set_target_TIGER(int target_TIGER_param)
{
g_reg_target_TIGER_ID = target_TIGER_param & 0x7;
g_reg_command_words[11] = ((g_reg_command_code & 0xF) << command_code_shift) + ((g_reg_target_TIGER_ID & 0x7) << target_TIGER_ID_shift);
}

void g_reg_set_command_code(char* command_code_param)
{
      if (command_code_param == "WR"){
         g_reg_command_code = 0x8;
         g_reg_is_a_write = 1;
    g_reg_command_words[11] = ((g_reg_command_code & 0xF) << command_code_shift) + ((g_reg_target_TIGER_ID & 0x7) << target_TIGER_ID_shift); }
      else if (command_code_param == "RD"){
         g_reg_command_code = 0x9;
         g_reg_is_a_write = 0;
       g_reg_command_words[11] = ((g_reg_command_code & 0xF) << command_code_shift) + ((g_reg_target_TIGER_ID & 0x7) << target_TIGER_ID_shift);}
      else
         printf("g_reg: bad command_code parameter passed\n");
}

void g_reg_update_command_words()
{
      if ((g_reg_command_code & 0xF) == 0x9 ){
         g_reg_command_words[1] = 0;
         g_reg_command_words[2]  = 0;
         g_reg_command_words[3]  = 0;
         g_reg_command_words[4]  = 0;
         g_reg_command_words[5]  = 0;
         g_reg_command_words[6]  = 0;
         g_reg_command_words[7]  = 0;
         g_reg_command_words[8]  = 0;
         g_reg_command_words[9]  = 0;
         g_reg_command_words[10]  = 0;  }
      else{
         g_reg_command_words[1] = ((g_reg_BufferBias & 0x3) << 24)+ ((g_reg_TDCVcasN & 0xF) << 16)+ ((g_reg_TDCVcasP & 0x1F) << 8)        + ((g_reg_TDCVcasPHyst & 0x3F));
         g_reg_command_words[2] =  ((g_reg_DiscFE_Ibias & 0x3f) << 24)     + ((g_reg_BiasFE_PpreN & 0x3F) << 16)    + ((g_reg_AVcasp_global & 0x1F) << 8)   + ((g_reg_TDCcompVcas & 0xF));
         g_reg_command_words[3] =  ((g_reg_TDCIres & 0x1f) << 24)          + ((g_reg_DiscVcasN & 0xF) << 16)        + ((g_reg_IntegVb1 & 0x3F) << 8)        + ((g_reg_BiasFE_A1 & 0xF));
         g_reg_command_words[4] =  ((g_reg_Vcasp_Vth & 0x3f) << 24)        + ((g_reg_TAC_I_LSB & 0x1F) << 16)       + ((g_reg_TDCcompVbias & 0x1F) << 8)    + ((g_reg_Vref_Integ & 0x3F));
         g_reg_command_words[5] =  ((g_reg_IBiasTPcal & 0x1f) << 24)       + ((g_reg_TP_Vcal & 0x1F) << 16)         + ((g_reg_ShaperIbias & 0xF) << 8)      + ((g_reg_IPostamp & 0x1F));
         g_reg_command_words[6] =  ((g_reg_TP_Vcal_ref & 0x1f) << 24)      + ((g_reg_Vref_integ_diff & 0x3F) << 16) + ((g_reg_Sig_pol & 0x1) << 8)          + ((g_reg_FE_TPEnable & 0x1));
         g_reg_command_words[7] =  ((g_reg_CompactDataFormat & 0x1) << 24) + ((g_reg_DataClkDiv & 0x3) << 16)       + ((g_reg_TACrefreshPeriod & 0xf) << 8) + ((g_reg_TACrefreshEnable & 0x1));
         g_reg_command_words[8] =  ((g_reg_CounterPeriod & 0x7) << 24)     + ((g_reg_CounterEnable & 0x1) << 16)    + ((g_reg_StopRampEnable & 0x3) << 8)   + ((g_reg_RClkEnable & 0x1F));
         g_reg_command_words[9] =  ((g_reg_TDCClkdiv & 0x1) << 24)         + ((g_reg_VetoMode & 0x3F) << 16)        + ((g_reg_Ch_DebugMode & 0x1) << 8)     + ((g_reg_TxMode & 0x3));
         g_reg_command_words[10] =  ((g_reg_TxDDR & 0x1) << 24)             + ((g_reg_TxLinks & 0x3) << 16);}
         g_reg_command_words[11]= ((g_reg_command_code & 0xF) << command_code_shift) + ((g_reg_target_TIGER_ID & 0x7) << target_TIGER_ID_shift);
      
}

void gemroc_init(int TARGET_GEMROC_ID_param,char* command_string_param,int TCAM_ID_param,int number_of_repetitions_param, 
                 int to_ALL_TCAM_enable_param)
{    int i;
     unsigned int header_tag;
     unsigned int gemroc_cmd_ID;
     unsigned int gemroc_cmd_tag;
     unsigned int gemroc_cmd_word_count;


      gemroc_TARGET_GEMROC_ID = TARGET_GEMROC_ID_param;
      gemroc_EN_TM_TCAM_pattern = gemroc_parameter_array[0];
      gemroc_AUTO_TP_EN_pattern     = gemroc_parameter_array[1];
      gemroc_AUTO_L1_EN_pattern     = gemroc_parameter_array[2];
      gemroc_Periodic_TP_EN_pattern = gemroc_parameter_array[3];
      gemroc_Periodic_L1_EN_pattern = gemroc_parameter_array[4];
      gemroc_TP_period = gemroc_parameter_array[5];
      gemroc_TP_width = gemroc_parameter_array[6];
      gemroc_L1_period = gemroc_parameter_array[7];
      gemroc_LowerDataScanWindowOffset = gemroc_parameter_array[8];
      gemroc_UpperDataScanWindowOffset = gemroc_parameter_array[9];
      gemroc_L1_latency = gemroc_parameter_array[10];
      gemroc_FEB_PWR_EN_pattern = gemroc_parameter_array[11];
      gemroc_FEB_OVC_EN_pattern = gemroc_parameter_array[12];
      gemroc_FEB_OVV_EN_pattern = gemroc_parameter_array[13];
      gemroc_FEB_OVT_EN_pattern = gemroc_parameter_array[14];
      gemroc_ROC_OVT_EN  = gemroc_parameter_array[15];
      gemroc_TL_nTM_ACQ  = gemroc_parameter_array[16];
      gemroc_ROC_OVT_LIMIT = gemroc_parameter_array[17];
      gemroc_OVC_LIMIT_FEB0 = gemroc_parameter_array[18];
      gemroc_OVV_LIMIT_FEB0 = gemroc_parameter_array[19];
      gemroc_OVT_LIMIT_FEB0 = gemroc_parameter_array[20];
      gemroc_OVC_LIMIT_FEB1 = gemroc_parameter_array[21];
      gemroc_OVV_LIMIT_FEB1 = gemroc_parameter_array[22];
      gemroc_OVT_LIMIT_FEB1 = gemroc_parameter_array[23];
      gemroc_OVC_LIMIT_FEB2 = gemroc_parameter_array[24];
      gemroc_OVV_LIMIT_FEB2 = gemroc_parameter_array[25];
      gemroc_OVT_LIMIT_FEB2 = gemroc_parameter_array[26];
      gemroc_OVC_LIMIT_FEB3 = gemroc_parameter_array[27];
      gemroc_OVV_LIMIT_FEB3 = gemroc_parameter_array[28];
      gemroc_OVT_LIMIT_FEB3 = gemroc_parameter_array[29];
      gemroc_TIMING_DLY_FEB0 = gemroc_parameter_array[30];
      gemroc_TIMING_DLY_FEB1 = gemroc_parameter_array[31];
      gemroc_TIMING_DLY_FEB2 = gemroc_parameter_array[32];
      gemroc_TIMING_DLY_FEB3 = gemroc_parameter_array[33];
      gemroc_TP_Pos_nNeg = gemroc_parameter_array[34];
      gemroc_command_string = command_string_param;
      gemroc_target_TCAM_ID = TCAM_ID_param;
      gemroc_to_ALL_TCAM_enable = to_ALL_TCAM_enable_param;
      gemroc_number_of_repetitions = number_of_repetitions_param;
      gemroc_is_a_write = 0x1;
   for(i=0;i<13;i++)
       if (strcmp(gemroc_command_string, gemroc_command_list[i]) == 0)  
           gemroc_gemroc_cmd_code = i;
      header_tag = 0x8 << 28;
      gemroc_cmd_ID = 0xD;
      gemroc_cmd_tag = gemroc_cmd_ID << 24;
      gemroc_cmd_word_count = 11;
      gemroc_command_words[0] = header_tag + (gemroc_TARGET_GEMROC_ID << 16) + gemroc_cmd_tag + gemroc_cmd_word_count;
      gemroc_command_words[1] = ((gemroc_TIMING_DLY_FEB3 & 0x3F) << 24) + ((gemroc_TIMING_DLY_FEB2 & 0x3F) << 16) + ((gemroc_TIMING_DLY_FEB1 & 0x3F) << 8) + ((gemroc_TIMING_DLY_FEB0 & 0x3F) << 0);
      gemroc_command_words[2] = ((gemroc_OVT_LIMIT_FEB3 & 0xFF) << 22) + ((gemroc_OVV_LIMIT_FEB3 & 0x1FF) << 13) + ((gemroc_OVC_LIMIT_FEB3 & 0x1FF) << 4);
      gemroc_command_words[3] = ((gemroc_OVT_LIMIT_FEB2 & 0xFF) << 22) + ((gemroc_OVV_LIMIT_FEB2 & 0x1FF) << 13) + ((gemroc_OVC_LIMIT_FEB2 & 0x1FF) << 4);
      gemroc_command_words[4] = ((gemroc_OVT_LIMIT_FEB1 & 0xFF) << 22) + ((gemroc_OVV_LIMIT_FEB1 & 0x1FF) << 13) + ((gemroc_OVC_LIMIT_FEB1 & 0x1FF) << 4);
      gemroc_command_words[5] = ((gemroc_OVT_LIMIT_FEB0 & 0xFF) << 22) + ((gemroc_OVV_LIMIT_FEB0 & 0x1FF) << 13) + ((gemroc_OVC_LIMIT_FEB0 & 0x1FF) << 4);
      gemroc_command_words[6] = ((gemroc_ROC_OVT_LIMIT & 0x3F) << 24) + ((gemroc_TP_Pos_nNeg & 0x1) << 18) + ((gemroc_TL_nTM_ACQ & 0x1) << 17) + ((gemroc_ROC_OVT_EN & 0x1) << 16) + ((gemroc_FEB_OVT_EN_pattern & 0xF) << 12) + ((gemroc_FEB_OVV_EN_pattern & 0xF) << 8) + ((gemroc_FEB_OVC_EN_pattern & 0xF) << 4) + (gemroc_FEB_PWR_EN_pattern & 0xF);
gemroc_command_words[7] = ((gemroc_L1_latency & 0x3FF) << 20) + ((gemroc_Periodic_TP_EN_pattern & 0xF) << 16) + (gemroc_UpperDataScanWindowOffset & 0xFFFF);
gemroc_command_words[8] = ((gemroc_L1_period  & 0x3FF) << 20) + ((gemroc_TP_width  & 0xF) << 16) + (gemroc_LowerDataScanWindowOffset & 0xFFFF);
gemroc_command_words[9] = ((gemroc_TP_period & 0x3FF) << 20) + ((gemroc_Periodic_L1_EN_pattern & 0xF) << 16) + ((gemroc_AUTO_L1_EN_pattern & 0xF) << 12) + ((gemroc_AUTO_TP_EN_pattern & 0xF) << 8) + (gemroc_EN_TM_TCAM_pattern & 0xFF);
gemroc_command_words[10] = ((gemroc_number_of_repetitions & 0x3FF) << 16) + ((gemroc_gemroc_cmd_code & 0xF) << 11) + ((gemroc_target_TCAM_ID & 0x3) << 8) + ((gemroc_to_ALL_TCAM_enable & 0x1) << 6);
}


int gemroc_cmd_words_array_size()
{
  return sizeof(gemroc_command_words); }


void gemroc_print_command_words()
{int i;
for(i=0;i<11;i++)
printf("%08X\n",gemroc_command_words[i]); }

void gemroc_set_target_GEMROC(int GEMROC_ID_param)
{  
 gemroc_TARGET_GEMROC_ID = GEMROC_ID_param & 0x1F;
 gemroc_command_words[0] = (gemroc_command_words[0] & 0xFF00FFFF) + (gemroc_TARGET_GEMROC_ID << 16);}

void gemroc_set_target_TCAM_ID(int target_TA_param, int to_ALL_TCAM_EN_param)
{
gemroc_target_TCAM_ID = target_TA_param & 0x3;
gemroc_to_ALL_TCAM_enable = to_ALL_TCAM_EN_param & 0x1;
}

void gemroc_set_FEB_PWR_EN_pattern(int target_FEB_PWR_EN_pattern)
{
gemroc_FEB_PWR_EN_pattern = target_FEB_PWR_EN_pattern & 0xF;
}

void gemroc_set_TP_width(int target_TP_width_param)
{
gemroc_TP_width = target_TP_width_param & 0xF;
}

void gemroc_set_TP_period(int TP_period_param)
{
gemroc_TP_period = TP_period_param & 0x3FF;
}

void gemroc_set_AUTO_TP_EN_pattern(int target_AUTO_TP_EN_param)
{
gemroc_AUTO_TP_EN_pattern = target_AUTO_TP_EN_param & 0xF;
}

void gemroc_set_Periodic_TP_EN_pattern(int Periodic_TP_EN_pattern_param)
{
gemroc_Periodic_TP_EN_pattern = Periodic_TP_EN_pattern_param & 0xF;
}

void gemroc_set_TL_nTM_ACQ(int TL_nTM_ACQ_param)
{
gemroc_TL_nTM_ACQ = TL_nTM_ACQ_param & 0x1;
}

void gemroc_set_EN_TM_TCAM_pattern(int EN_TM_TCAM_pattern_param)
{
gemroc_EN_TM_TCAM_pattern = EN_TM_TCAM_pattern_param & 0xFF;
}

void gemroc_set_TP_Pos_nNeg(int TP_Pos_nNeg_param)
{
gemroc_TP_Pos_nNeg = TP_Pos_nNeg_param & 0x1;
}

void gemroc_set_timing_toFEB_delay(int FEB3dly_param,int FEB2dly_param,int FEB1dly_param,int FEB0dly_param)
{
gemroc_TIMING_DLY_FEB3 = FEB3dly_param & 0x3F;
gemroc_TIMING_DLY_FEB2 = FEB2dly_param & 0x3F;
gemroc_TIMING_DLY_FEB1 = FEB1dly_param & 0x3F;
gemroc_TIMING_DLY_FEB0 = FEB0dly_param & 0x3F;
}

void gemroc_set_gemroc_cmd_code(char* command_string_param, int no_of_executions_param)
{int i;
gemroc_number_of_repetitions = no_of_executions_param;
gemroc_command_string = command_string_param;
 for(i=0;i<13;i++)
       if (strcmp(gemroc_command_string, gemroc_command_list[i]) == 0)
           gemroc_gemroc_cmd_code = i;
}

 void gemroc_update_command_words()
{ int i;    
 if ( ((gemroc_gemroc_cmd_code & 0xF) == 0x2)|| ((gemroc_gemroc_cmd_code & 0xF) == 0x3) || ((gemroc_gemroc_cmd_code & 0xF) == 0x4) )
{
         gemroc_command_words[1]  = 0;
         gemroc_command_words[2]  = 0;
         gemroc_command_words[3]  = 0;
         gemroc_command_words[4]  = 0;
         gemroc_command_words[5]  = 0;
         gemroc_command_words[6]  = 0;
         gemroc_command_words[7]  = 0;
         gemroc_command_words[8]  = 0;
         gemroc_command_words[9]  = 0; }
      else {
         gemroc_command_words[1] = ((gemroc_TIMING_DLY_FEB3 & 0x3F) << 24) + ((gemroc_TIMING_DLY_FEB2 & 0x3F) << 16) + ((gemroc_TIMING_DLY_FEB1 & 0x3F) << 8) + ((gemroc_TIMING_DLY_FEB0 & 0x3F) << 0);
         gemroc_command_words[2] = ((gemroc_OVT_LIMIT_FEB3 & 0xFF) << 22) + ((gemroc_OVV_LIMIT_FEB3 & 0x1FF) << 13) + ((gemroc_OVC_LIMIT_FEB3 & 0x1FF) << 4);
         gemroc_command_words[3] = ((gemroc_OVT_LIMIT_FEB2 & 0xFF) << 22) + ((gemroc_OVV_LIMIT_FEB2 & 0x1FF) << 13) + ((gemroc_OVC_LIMIT_FEB2 & 0x1FF) << 4);
         gemroc_command_words[4] = ((gemroc_OVT_LIMIT_FEB1 & 0xFF) << 22) + ((gemroc_OVV_LIMIT_FEB1 & 0x1FF) << 13) + ((gemroc_OVC_LIMIT_FEB1 & 0x1FF) << 4);
         gemroc_command_words[5] = ((gemroc_OVT_LIMIT_FEB0 & 0xFF) << 22) + ((gemroc_OVV_LIMIT_FEB0 & 0x1FF) << 13) + ((gemroc_OVC_LIMIT_FEB0 & 0x1FF) << 4);
         gemroc_command_words[6] = ((gemroc_ROC_OVT_LIMIT & 0x3F) << 24) + ((gemroc_TP_Pos_nNeg & 0x1) << 18) + ((gemroc_TL_nTM_ACQ & 0x1) << 17) + ((gemroc_ROC_OVT_EN & 0x1) << 16) + ((gemroc_FEB_OVT_EN_pattern & 0xF) << 12) + ((gemroc_FEB_OVV_EN_pattern & 0xF) << 8) + ((gemroc_FEB_OVC_EN_pattern & 0xF) << 4) + (gemroc_FEB_PWR_EN_pattern & 0xF);
         gemroc_command_words[7] = ((gemroc_L1_latency & 0x3FF) << 20) + ((gemroc_Periodic_TP_EN_pattern & 0xF) << 16) + (gemroc_UpperDataScanWindowOffset & 0xFFFF);
         gemroc_command_words[8] = ((gemroc_L1_period  & 0x3FF) << 20) + ((gemroc_TP_width  & 0xF) << 16) + (gemroc_LowerDataScanWindowOffset & 0xFFFF);
         gemroc_command_words[9] = ((gemroc_TP_period & 0x3FF) << 20) + ((gemroc_Periodic_L1_EN_pattern & 0xF) << 16) + ((gemroc_AUTO_L1_EN_pattern & 0xF) << 12) + ((gemroc_AUTO_TP_EN_pattern & 0xF) << 8) + (gemroc_EN_TM_TCAM_pattern & 0xFF);}
         gemroc_command_words[10] = ((gemroc_number_of_repetitions & 0x3FF) << 16) + ((gemroc_gemroc_cmd_code & 0xF) << 11) + ((gemroc_target_TCAM_ID & 0x3) << 8) + ((gemroc_to_ALL_TCAM_enable & 0x1) << 6);

int tmp=gemroc_command_words[1];
for(i=1;i<9;i++)
gemroc_command_words[i]=gemroc_command_words[i+1];
gemroc_command_words[9]=tmp;
for(i=0;i<11;i++)
printf("%08X\n",gemroc_command_words[i]);  }  


int main()
{
   FILE *fw = NULL;
   unsigned char buff[48],buff2[48],cmd_message[98];
//   int Total_Data=0;
//   fp = fopen("udptobin.dat", "wb");
   fw = fopen("gem_cfg_log.txt","w+");
//   unsigned char buff[1024];
   int i,j=0;
    int sin_len,len;
    int socket_descriptor,socket_descriptor1;
    struct sockaddr_in sin,send_addr,recv_addr;
    bzero(&sin,sizeof(sin));
    sin.sin_family=AF_INET;
    sin.sin_addr.s_addr=inet_addr(DEST_IP_ADDRESS);
    sin.sin_port=htons(DEST_PORT_NO);
    sin_len=sizeof(sin);
   
    bzero(&send_addr,sizeof(send_addr));
    send_addr.sin_family=AF_INET;
    send_addr.sin_addr.s_addr=inet_addr(HOST_IP);
    send_addr.sin_port=htons(HOST_PORT);
//    sin_len2=sizeof(sin2);
    socket_descriptor=socket(AF_INET,SOCK_DGRAM,0);
    bind(socket_descriptor,(struct sockaddr *)&send_addr,sizeof(send_addr));

    bzero(&recv_addr,sizeof(recv_addr));
    recv_addr.sin_family=AF_INET;
    recv_addr.sin_addr.s_addr=inet_addr(HOST_IP);
    recv_addr.sin_port=htons(HOST_PORT_RECEIVE);
//    len=sizeof(local_addr);
    socket_descriptor1=socket(AF_INET,SOCK_DGRAM,0);
    bind(socket_descriptor1,(struct sockaddr *)&recv_addr,sizeof(recv_addr));

    gemroc_init(2,"NONE",0,1,0);
    gemroc_set_TP_width(3);
    gemroc_set_FEB_PWR_EN_pattern(0x1);
    gemroc_set_EN_TM_TCAM_pattern (0x3);
    gemroc_set_TP_Pos_nNeg (0x1);
    gemroc_set_timing_toFEB_delay(0, 0, 0, 0);
    gemroc_set_gemroc_cmd_code("WR",1);
    gemroc_update_command_words();
    gemroc_print_command_words();
    memcpy(buff,gemroc_command_words,sizeof(gemroc_command_words)); 
for(i=0;i<44;i=i+4)
for(j=0;j<4;j++)
buff2[i+j]=buff[i+3-j];
sendto(socket_descriptor,buff2,44,0,(struct sockaddr *)&sin,sin_len);
sprintf(cmd_message,"CMD sent (GEMROC WR): \n"); // C4996
fwrite(cmd_message,sizeof(unsigned char),23,fw);
j=0;
for(i=0;i<44;i++)
j+=sprintf(cmd_message+j,"%02X",buff2[i]);
printf("sent code:\n");
for(i=0;i<44;i++)
printf("%02X",buff2[i]);
fwrite(cmd_message,sizeof(unsigned char),88,fw);
i=recvfrom(socket_descriptor1,buff,44,0,NULL,NULL);
sprintf(cmd_message,"\nCMD echo (GEMROC WR): \n");
fwrite(cmd_message,sizeof(unsigned char),24,fw);
j=0;
for(i=0;i<44;i++)
j+=sprintf(cmd_message+j,"%02X",buff[i]);
fwrite(cmd_message,sizeof(unsigned char),88,fw);
printf("\necho code:\n");
for(i=0;i<44;i++)
printf("%02X",buff[i]);
printf("\n");
sleep(1);
   
gemroc_set_gemroc_cmd_code("TIGER_SYNCH_RST_WITH_SPI",1);
gemroc_update_command_words();
memcpy(buff,gemroc_command_words,sizeof(gemroc_command_words));
for(i=0;i<44;i=i+4)
for(j=0;j<4;j++)
buff2[i+j]=buff[i+3-j];
sendto(socket_descriptor,buff2,44,0,(struct sockaddr *)&sin,sin_len);
sprintf(cmd_message,"\nCMD sent 'TIGER_SYNCH_RST_WITH_SPI': \n"); // C4996
fwrite(cmd_message,sizeof(unsigned char),39,fw);
j=0;
for(i=0;i<44;i++)
j+=sprintf(cmd_message+j,"%02X",buff2[i]);
fwrite(cmd_message,sizeof(unsigned char),88,fw);
i=recvfrom(socket_descriptor1,buff,44,0,NULL,NULL);
sprintf(cmd_message,"\nCMD echo 'TIGER_SYNCH_RST_WITH_SPI': \n");
fwrite(cmd_message,sizeof(unsigned char),39,fw);
j=0;
for(i=0;i<44;i++)
j+=sprintf(cmd_message+j,"%02X",buff[i]);
fwrite(cmd_message,sizeof(unsigned char),88,fw);
sleep(1);

    g_reg_init(0);
    g_reg_set_target_GEMROC(2);
    g_reg_set_target_TIGER(0);
    g_reg_set_command_code("WR");
    g_reg_update_command_words();
//    g_reg_set_gemroc_cmd_code("TIGER_SYNCH_RST_WITH_SPI",1);
//g_reg_update_command_words();
memcpy(buff,g_reg_command_words,sizeof(g_reg_command_words));
for(i=0;i<48;i=i+4)
for(j=0;j<4;j++)
buff2[i+j]=buff[i+3-j];
sendto(socket_descriptor,buff2,48,0,(struct sockaddr *)&sin,sin_len);
sprintf(cmd_message,"\nCMD sent (Global REG WR): \n"); // C4996
fwrite(cmd_message,sizeof(unsigned char),28,fw);
j=0;
for(i=0;i<48;i++)
j+=sprintf(cmd_message+j,"%02X",buff2[i]);
fwrite(cmd_message,sizeof(unsigned char),96,fw);
i=recvfrom(socket_descriptor1,buff,48,0,NULL,NULL);
sprintf(cmd_message,"\nCMD echo (Global REG WR): \n");
fwrite(cmd_message,sizeof(unsigned char),28,fw);
j=0;
for(i=0;i<48;i++)
j+=sprintf(cmd_message+j,"%02X",buff[i]);
fwrite(cmd_message,sizeof(unsigned char),96,fw);
sleep(1);

g_reg_set_command_code("RD");
g_reg_update_command_words();

memcpy(buff,g_reg_command_words,sizeof(g_reg_command_words));
for(i=0;i<48;i=i+4)
for(j=0;j<4;j++)
buff2[i+j]=buff[i+3-j];
sendto(socket_descriptor,buff2,48,0,(struct sockaddr *)&sin,sin_len);
sprintf(cmd_message,"\nCMD sent (Global REG RD): \n"); // C4996
fwrite(cmd_message,sizeof(unsigned char),28,fw);
j=0;
for(i=0;i<48;i++)
j+=sprintf(cmd_message+j,"%02X",buff2[i]);
fwrite(cmd_message,sizeof(unsigned char),96,fw);
i=recvfrom(socket_descriptor1,buff,48,0,NULL,NULL);
sprintf(cmd_message,"\nCMD echo (Global REG RD): \n");
fwrite(cmd_message,sizeof(unsigned char),28,fw);
j=0;
for(i=0;i<48;i++)
j+=sprintf(cmd_message+j,"%02X",buff[i]);
fwrite(cmd_message,sizeof(unsigned char),96,fw);
sleep(1);



    g_reg_set_target_TIGER(1);
    g_reg_set_command_code("WR");
    g_reg_update_command_words();
memcpy(buff,g_reg_command_words,sizeof(g_reg_command_words));
for(i=0;i<48;i=i+4)
for(j=0;j<4;j++)
buff2[i+j]=buff[i+3-j];
sendto(socket_descriptor,buff2,48,0,(struct sockaddr *)&sin,sin_len);
sprintf(cmd_message,"\nCMD sent (Global REG WR): \n"); // C4996
fwrite(cmd_message,sizeof(unsigned char),28,fw);
j=0;
for(i=0;i<48;i++)
j+=sprintf(cmd_message+j,"%02X",buff2[i]);
fwrite(cmd_message,sizeof(unsigned char),96,fw);
i=recvfrom(socket_descriptor1,buff,48,0,NULL,NULL);
sprintf(cmd_message,"\nCMD echo (Global REG WR): \n");
fwrite(cmd_message,sizeof(unsigned char),28,fw);
j=0;
for(i=0;i<48;i++)
j+=sprintf(cmd_message+j,"%02X",buff[i]);
fwrite(cmd_message,sizeof(unsigned char),96,fw);
sleep(1);

    g_reg_set_command_code("RD");
    g_reg_update_command_words();
memcpy(buff,g_reg_command_words,sizeof(g_reg_command_words));
for(i=0;i<48;i=i+4)
for(j=0;j<4;j++)
buff2[i+j]=buff[i+3-j];
sendto(socket_descriptor,buff2,48,0,(struct sockaddr *)&sin,sin_len);
sprintf(cmd_message,"\nCMD sent (Global REG RD): \n"); // C4996
fwrite(cmd_message,sizeof(unsigned char),28,fw);
j=0;
for(i=0;i<48;i++)
j+=sprintf(cmd_message+j,"%02X",buff2[i]);
fwrite(cmd_message,sizeof(unsigned char),96,fw);
i=recvfrom(socket_descriptor1,buff,48,0,NULL,NULL);
sprintf(cmd_message,"\nCMD echo (Global REG RD): \n");
fwrite(cmd_message,sizeof(unsigned char),28,fw);
j=0;
for(i=0;i<48;i++)
j+=sprintf(cmd_message+j,"%02X",buff[i]);
fwrite(cmd_message,sizeof(unsigned char),96,fw);
sleep(1);

ch_reg_init(0);
printf("ch_reg_cmd_word0:%08X\n",ch_reg_cmd_word0);
printf("ch_reg_command_words[9]:%08X\n",ch_reg_command_words[9]);
ch_reg_set_target_GEMROC(2);
printf("ch_reg_cmd_word0:%08X\n",ch_reg_cmd_word0);
printf("ch_reg_command_words[9]:%08X\n",ch_reg_command_words[9]);
ch_reg_set_target_TIGER(0);
printf("ch_reg_cmd_word0:%08X\n",ch_reg_cmd_word0);
printf("ch_reg_command_words[9]:%08X\n",ch_reg_command_words[9]);
ch_reg_set_command_code("WR");
printf("ch_reg_cmd_word0:%08X\n",ch_reg_cmd_word0);
printf("ch_reg_command_words[9]:%08X\n",ch_reg_command_words[9]);
ch_reg_set_to_ALL_param(1);
printf("ch_reg_cmd_word0:%08X\n",ch_reg_cmd_word0);
printf("ch_reg_command_words[9]:%08X\n",ch_reg_command_words[9]);
TriggerMode_default_TP_Disabled = 0;
ch_reg_TriggerMode = TriggerMode_default_TP_Disabled;
ch_reg_update_command_words();
printf("ch_reg_cmd_word0:%08X\n",ch_reg_cmd_word0);
printf("ch_reg_command_words[9]:%08X\n",ch_reg_command_words[9]);
ch_reg_print_command_words();
memcpy(buff,ch_reg_command_words,sizeof(ch_reg_command_words));
for(i=0;i<40;i=i+4)
for(j=0;j<4;j++)
buff2[i+j]=buff[i+3-j];
sendto(socket_descriptor,buff2,40,0,(struct sockaddr *)&sin,sin_len);
sprintf(cmd_message,"\nCMD sent (ch wr TOALL): \n"); // C4996
fwrite(cmd_message,sizeof(unsigned char),26,fw);
j=0;
for(i=0;i<40;i++)
j+=sprintf(cmd_message+j,"%02X",buff2[i]);
fwrite(cmd_message,sizeof(unsigned char),80,fw);
i=recvfrom(socket_descriptor1,buff,40,0,NULL,NULL);
sprintf(cmd_message,"\nCMD echo (ch wr TOALL): \n");
fwrite(cmd_message,sizeof(unsigned char),26,fw);
j=0;
for(i=0;i<40;i++)
j+=sprintf(cmd_message+j,"%02X",buff[i]);
fwrite(cmd_message,sizeof(unsigned char),80,fw);
sleep(1);


ch_reg_set_target_GEMROC(2);
ch_reg_set_target_TIGER(0);
ch_reg_set_command_code("WR");
ch_reg_set_to_ALL_param(0);
ch_reg_set_target_channel(3);
TriggerMode_default_TP_Enabled = 1;
ch_reg_TriggerMode = TriggerMode_default_TP_Enabled;
ch_reg_update_command_words();
ch_reg_print_command_words();
memcpy(buff,ch_reg_command_words,sizeof(ch_reg_command_words));
for(i=0;i<40;i=i+4)
for(j=0;j<4;j++)
buff2[i+j]=buff[i+3-j];
sendto(socket_descriptor,buff2,40,0,(struct sockaddr *)&sin,sin_len);
sprintf(cmd_message,"\nCMD sent (ch 3 wr): \n"); // C4996
fwrite(cmd_message,sizeof(unsigned char),22,fw);
j=0;
for(i=0;i<40;i++)
j+=sprintf(cmd_message+j,"%02X",buff2[i]);
fwrite(cmd_message,sizeof(unsigned char),80,fw);
i=recvfrom(socket_descriptor1,buff,40,0,NULL,NULL);
sprintf(cmd_message,"\nCMD echo (ch 3 wr): \n");
fwrite(cmd_message,sizeof(unsigned char),22,fw);
j=0;
for(i=0;i<40;i++)
j+=sprintf(cmd_message+j,"%02X",buff[i]);
fwrite(cmd_message,sizeof(unsigned char),80,fw);
sleep(1);

// TIGER 1
// sending standard channel configuration to all channels
ch_reg_set_target_GEMROC(2);
ch_reg_set_target_TIGER(1);
ch_reg_set_command_code("WR");
ch_reg_set_to_ALL_param(1);
TriggerMode_default_TP_Disabled = 0;
ch_reg_TriggerMode = TriggerMode_default_TP_Disabled;
ch_reg_update_command_words();
ch_reg_print_command_words();
memcpy(buff,ch_reg_command_words,sizeof(ch_reg_command_words));
for(i=0;i<40;i=i+4)
for(j=0;j<4;j++)
buff2[i+j]=buff[i+3-j];
sendto(socket_descriptor,buff2,40,0,(struct sockaddr *)&sin,sin_len);
sprintf(cmd_message,"\nCMD sent (ch wr TOALL): \n"); // C4996
fwrite(cmd_message,sizeof(unsigned char),26,fw);
j=0;
for(i=0;i<40;i++)
j+=sprintf(cmd_message+j,"%02X",buff2[i]);
fwrite(cmd_message,sizeof(unsigned char),80,fw);
i=recvfrom(socket_descriptor1,buff,40,0,NULL,NULL);
sprintf(cmd_message,"\nCMD echo (ch wr TOALL): \n");
fwrite(cmd_message,sizeof(unsigned char),26,fw);
j=0;
for(i=0;i<40;i++)
j+=sprintf(cmd_message+j,"%02X",buff[i]);
fwrite(cmd_message,sizeof(unsigned char),80,fw);
sleep(1);

ch_reg_set_target_GEMROC(2);
ch_reg_set_target_TIGER(1);
ch_reg_set_command_code("WR");
ch_reg_set_to_ALL_param(0);
ch_reg_set_target_channel(3);
TriggerMode_default_TP_Enabled = 1;
ch_reg_TriggerMode = TriggerMode_default_TP_Enabled;
ch_reg_update_command_words();
ch_reg_print_command_words();
memcpy(buff,ch_reg_command_words,sizeof(ch_reg_command_words));
for(i=0;i<40;i=i+4)
for(j=0;j<4;j++)
buff2[i+j]=buff[i+3-j];
sendto(socket_descriptor,buff2,40,0,(struct sockaddr *)&sin,sin_len);
sprintf(cmd_message,"\nCMD sent (ch 3 wr): \n"); // C4996
fwrite(cmd_message,sizeof(unsigned char),22,fw);
j=0;
for(i=0;i<40;i++)
j+=sprintf(cmd_message+j,"%02X",buff2[i]);
fwrite(cmd_message,sizeof(unsigned char),80,fw);
i=recvfrom(socket_descriptor1,buff,40,0,NULL,NULL);
sprintf(cmd_message,"\nCMD echo (ch 3 wr): \n");
fwrite(cmd_message,sizeof(unsigned char),22,fw);
j=0;
for(i=0;i<40;i++)
j+=sprintf(cmd_message+j,"%02X",buff[i]);
fwrite(cmd_message,sizeof(unsigned char),80,fw);
//sleep(1);
//    printf("before_ch_reg[9]_To:%08X\n",ch_reg_command_words[9]);
printf("ch_reg_cmd_word0:%08X\n",ch_reg_cmd_word0);
printf("ch_reg_command_words[9]:%08X\n",ch_reg_command_words[9]);
    ch_reg_set_target_GEMROC(2);
printf("ch_reg_cmd_word0:%08X\n",ch_reg_cmd_word0);
printf("ch_reg_command_words[9]:%08X\n",ch_reg_command_words[9]);
    ch_reg_set_target_TIGER(0);
printf("ch_reg_cmd_word0:%08X\n",ch_reg_cmd_word0);
printf("ch_reg_command_words[9]:%08X\n",ch_reg_command_words[9]);
    ch_reg_set_command_code("RD");
printf("ch_reg_cmd_word0:%08X\n",ch_reg_cmd_word0);
printf("ch_reg_command_words[9]:%08X\n",ch_reg_command_words[9]);
    ch_reg_set_to_ALL_param(0);
printf("ch_reg_cmd_word0:%08X\n",ch_reg_cmd_word0);
printf("ch_reg_command_words[9]:%08X\n",ch_reg_command_words[9]);
    ch_reg_set_target_channel(3);
printf("ch_reg_cmd_word0:%08X\n",ch_reg_cmd_word0);
printf("ch_reg_command_words[9]:%08X\n",ch_reg_command_words[9]);
    TriggerMode_default_TP_Enabled = 1;
    ch_reg_TriggerMode = TriggerMode_default_TP_Enabled;
    ch_reg_update_command_words();
printf("ch_reg_cmd_word0:%08X\n",ch_reg_cmd_word0);
printf("ch_reg_command_words[9]:%08X\n",ch_reg_command_words[9]);
//    printf("after_ch_reg[9]_To:%08X\n",ch_reg_command_words[9]);
    ch_reg_print_command_words();
memcpy(buff,ch_reg_command_words,sizeof(ch_reg_command_words));
for(i=0;i<40;i=i+4)
for(j=0;j<4;j++)
buff2[i+j]=buff[i+3-j];
sendto(socket_descriptor,buff2,40,0,(struct sockaddr *)&sin,sin_len);
sprintf(cmd_message,"\nCMD sent(T0 ch3 rd): \n"); // C4996
fwrite(cmd_message,sizeof(unsigned char),23,fw);
j=0;
for(i=0;i<40;i++)
j+=sprintf(cmd_message+j,"%02X",buff2[i]);
fwrite(cmd_message,sizeof(unsigned char),80,fw);
i=recvfrom(socket_descriptor1,buff,40,0,NULL,NULL);
sprintf(cmd_message,"\nCMD echo(T0 ch3 rd): \n");
fwrite(cmd_message,sizeof(unsigned char),23,fw);
j=0;
for(i=0;i<40;i++)
j+=sprintf(cmd_message+j,"%02X",buff[i]);
fwrite(cmd_message,sizeof(unsigned char),80,fw);

    ch_reg_set_target_GEMROC(2);
    ch_reg_set_target_TIGER(1);
    ch_reg_set_command_code("RD");
    ch_reg_set_to_ALL_param(0);
    ch_reg_set_target_channel(3);
    TriggerMode_default_TP_Enabled = 1;
    ch_reg_TriggerMode = TriggerMode_default_TP_Enabled;
    ch_reg_update_command_words();
    ch_reg_print_command_words();
memcpy(buff,ch_reg_command_words,sizeof(ch_reg_command_words));
for(i=0;i<40;i=i+4)
for(j=0;j<4;j++)
buff2[i+j]=buff[i+3-j];
sendto(socket_descriptor,buff2,40,0,(struct sockaddr *)&sin,sin_len);
sprintf(cmd_message,"\nCMD sent(T1 ch3 rd): \n"); // C4996
fwrite(cmd_message,sizeof(unsigned char),23,fw);
j=0;
for(i=0;i<40;i++)
j+=sprintf(cmd_message+j,"%02X",buff2[i]);
fwrite(cmd_message,sizeof(unsigned char),80,fw);
i=recvfrom(socket_descriptor1,buff,40,0,NULL,NULL);
sprintf(cmd_message,"\nCMD echo(T1 ch3 rd): \n");
fwrite(cmd_message,sizeof(unsigned char),23,fw);
j=0;
for(i=0;i<40;i++)
j+=sprintf(cmd_message+j,"%02X",buff[i]);
fwrite(cmd_message,sizeof(unsigned char),80,fw);


gemroc_set_gemroc_cmd_code("TIGER_SYNCH_RST",1);
gemroc_update_command_words();
gemroc_print_command_words();
memcpy(buff,gemroc_command_words,sizeof(gemroc_command_words));
for(i=0;i<44;i=i+4)
for(j=0;j<4;j++)
buff2[i+j]=buff[i+3-j];
sendto(socket_descriptor,buff2,44,0,(struct sockaddr *)&sin,sin_len);
sprintf(cmd_message,"\nCMD sent 'TIGER_SYNCH_RST': \n"); // C4996
fwrite(cmd_message,sizeof(unsigned char),30,fw);
j=0;
for(i=0;i<44;i++)
j+=sprintf(cmd_message+j,"%02X",buff2[i]);
fwrite(cmd_message,sizeof(unsigned char),88,fw);
i=recvfrom(socket_descriptor1,buff,44,0,NULL,NULL);
sprintf(cmd_message,"\nCMD echo 'TIGER_SYNCH_RST': \n");
fwrite(cmd_message,sizeof(unsigned char),30,fw);
j=0;
for(i=0;i<44;i++)
j+=sprintf(cmd_message+j,"%02X",buff[i]);
fwrite(cmd_message,sizeof(unsigned char),88,fw);
//sleep(1);
gemroc_set_gemroc_cmd_code("TCAM_SYNCH_RST",1);
gemroc_update_command_words();
gemroc_print_command_words();
memcpy(buff,gemroc_command_words,sizeof(gemroc_command_words));
for(i=0;i<44;i=i+4)
for(j=0;j<4;j++)
buff2[i+j]=buff[i+3-j];
sendto(socket_descriptor,buff2,44,0,(struct sockaddr *)&sin,sin_len);
sprintf(cmd_message,"\nCMD sent 'TCAM_SYNCH_RST': \n"); // C4996
fwrite(cmd_message,sizeof(unsigned char),29,fw);
j=0;
for(i=0;i<44;i++)
j+=sprintf(cmd_message+j,"%02X",buff2[i]);
fwrite(cmd_message,sizeof(unsigned char),88,fw);
i=recvfrom(socket_descriptor1,buff,44,0,NULL,NULL);
sprintf(cmd_message,"\nCMD echo 'TCAM_SYNCH_RST': \n");
fwrite(cmd_message,sizeof(unsigned char),29,fw);
j=0;
for(i=0;i<44;i++)
j+=sprintf(cmd_message+j,"%02X",buff[i]);
fwrite(cmd_message,sizeof(unsigned char),88,fw);

gemroc_set_TP_width(5);
gemroc_set_AUTO_TP_EN_pattern(0x0); 
gemroc_set_Periodic_TP_EN_pattern(0xF);
gemroc_set_TL_nTM_ACQ(1);
gemroc_set_TP_Pos_nNeg(1);
gemroc_set_TP_period(8);
gemroc_set_gemroc_cmd_code("WR",1);
gemroc_update_command_words();
gemroc_print_command_words();
memcpy(buff,gemroc_command_words,sizeof(gemroc_command_words));
for(i=0;i<44;i=i+4)
for(j=0;j<4;j++)
buff2[i+j]=buff[i+3-j];
sendto(socket_descriptor,buff2,44,0,(struct sockaddr *)&sin,sin_len);
sprintf(cmd_message,"\nCMD sent (GEMROC set_AUTO_TP_EN_pattern): \n"); // C4996
fwrite(cmd_message,sizeof(unsigned char),44,fw);
j=0;
for(i=0;i<44;i++)
j+=sprintf(cmd_message+j,"%02X",buff2[i]);
fwrite(cmd_message,sizeof(unsigned char),88,fw);
i=recvfrom(socket_descriptor1,buff,44,0,NULL,NULL);
sprintf(cmd_message,"\nCMD echo (GEMROC set_AUTO_TP_EN_pattern): \n");
fwrite(cmd_message,sizeof(unsigned char),44,fw);
j=0;
for(i=0;i<44;i++)
j+=sprintf(cmd_message+j,"%02X",buff[i]);
fwrite(cmd_message,sizeof(unsigned char),88,fw);
close(socket_descriptor);
close(socket_descriptor);
fclose(fw);

}
