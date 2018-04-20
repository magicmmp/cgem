#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>
//#define HOST_IP "127.0.0.1"
#define HOST_IP "192.168.1.200"
#define BUFSIZE 4096 
unsigned int command_code_shift = 11; 
unsigned int target_TIGER_ID_shift = 8; 
      unsigned int default_arg_needed = 0;
      unsigned int TARGET_GEMROC_ID_param = 0;
      unsigned int TARGET_FEB_PWR_PATTERN_param = 0;
      unsigned int IVT_LOG_PERIOD_SECONDS = 20;
      unsigned int IVT_LOG_ENABLE = 1;
      unsigned int GEMROC_ID;
      unsigned int FEB_PWR_EN_pattern;

      unsigned int HOST_PORT;
      unsigned int HOST_PORT_RECEIVE;
      char DEST_IP_ADDRESS[14],cfg_filename[50];
      unsigned int DEST_PORT_NO ;
   FILE *fw = NULL;
   unsigned char buff[48],buff2[48],cmd_message[256];
    int sin_len,len;
    int socket_descriptor,socket_descriptor1;
    struct sockaddr_in sin_dest,send_addr,recv_addr;
#define GEMROC_CMD_LV_Num_of_params 31
#define GEMROC_CMD_LV_Num_Of_PktWords 12 
#define GEMROC_CMD_DAQ_Num_of_params 15
#define GEMROC_CMD_DAQ_Num_Of_PktWords 5
      unsigned int g_reg_TARGET_GEMROC_ID;
      char* g_reg_cfg_filename;
      unsigned int g_reg_parameter_array[37];
      unsigned int g_reg_BufferBias; 
      unsigned int g_reg_TDCVcasN;  
      unsigned int g_reg_TDCVcasP; 
      unsigned int g_reg_TDCVcasPHyst; 
      unsigned int g_reg_DiscFE_Ibias;
      unsigned int g_reg_BiasFE_PpreN; 
      unsigned int g_reg_AVcasp_global;
      unsigned int g_reg_TDCcompVcas; 
      unsigned int g_reg_TDCIref_cs; 
      unsigned int g_reg_DiscVcasN; 
      unsigned int g_reg_IntegVb1;
      unsigned int g_reg_BiasFE_A1; 
      unsigned int g_reg_Vcasp_Vth;
      unsigned int g_reg_TAC_I_LSB;
      unsigned int g_reg_TDCcompVbias;
      unsigned int g_reg_Vref_Integ;
      unsigned int g_reg_IBiasTPcal;
      unsigned int g_reg_TP_Vcal ;
      unsigned int g_reg_ShaperIbias;
      unsigned int g_reg_IPostamp;  
      unsigned int g_reg_TP_Vcal_ref; 
      unsigned int g_reg_Vref_integ_diff;
      unsigned int g_reg_Sig_pol;
      unsigned int g_reg_FE_TPEnable;
      unsigned int g_reg_DataClkDiv;
      unsigned int g_reg_TACrefreshPeriod;
      unsigned int g_reg_TACrefreshEnable;
      unsigned int g_reg_CounterPeriod;
      unsigned int g_reg_CounterEnable;
      unsigned int g_reg_StopRampEnable;
      unsigned int g_reg_RClkEnable;
      unsigned int g_reg_TDCClkdiv;
      unsigned int g_reg_VetoMode ;
      unsigned int g_reg_Ch_DebugMode; 
      unsigned int g_reg_TxMode; 
      unsigned int g_reg_TxDDR ;
      unsigned int g_reg_TxLinks;
      unsigned int g_reg_is_a_write;
      unsigned int g_reg_target_TIGER_ID;
      unsigned int g_reg_command_code;
      unsigned int g_reg_command_words[12];

      unsigned int ch_reg_TARGET_GEMROC_ID;
      char*        ch_reg_cfg_filename;
      unsigned int ch_reg_DisableHyst ;
      unsigned int ch_reg_T2Hyst ;
      unsigned int ch_reg_T1Hyst;
      unsigned int ch_reg_Ch63ObufMSB;
      unsigned int ch_reg_TP_disable_FE; 
      unsigned int ch_reg_TDC_IB_E ;
      unsigned int ch_reg_TDC_IB_T ;
      unsigned int ch_reg_Integ ;
      unsigned int ch_reg_PostAmpGain; 
      unsigned int ch_reg_FE_delay; 
      unsigned int ch_reg_Vth_T2; 
      unsigned int ch_reg_Vth_T1 ;
      unsigned int ch_reg_QTx2Enable; 
      unsigned int ch_reg_MaxIntegTime; 
      unsigned int ch_reg_MinIntegTime; 
      unsigned int ch_reg_TriggerBLatched; 
      unsigned int ch_reg_QdcMode ;
      unsigned int ch_reg_BranchEnableT; 
      unsigned int ch_reg_BranchEnableEQ; 
      unsigned int ch_reg_TriggerMode2B ;
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
      unsigned int ch_reg_is_a_write; 
      unsigned int ch_reg_target_TIGER_ID; 
      unsigned int ch_reg_command_code; 
      unsigned int ch_reg_TO_ALL_enable; 
      unsigned int ch_reg_channel_ID; 
      unsigned int ch_reg_command_words[10];
      unsigned int ch_reg_parameter_array[30];
      unsigned int gemroc_number_of_repetitions;
      unsigned int gemroc_TARGET_GEMROC_ID;
      char*        gemroc_cfg_filename;
      unsigned int gemroc_parameter_array[31];
      unsigned int gemroc_TIMING_DLY_FEB0;
      unsigned int gemroc_TIMING_DLY_FEB1;
      unsigned int gemroc_TIMING_DLY_FEB2; 
      unsigned int gemroc_TIMING_DLY_FEB3; 
      unsigned int gemroc_FEB_PWR_EN_pattern;
      unsigned int gemroc_FEB_OVC_EN_pattern;
      unsigned int gemroc_FEB_OVV_EN_pattern;
      unsigned int gemroc_FEB_OVT_EN_pattern;
      unsigned int gemroc_ROC_OVT_EN;  
      unsigned int gemroc_XCVR_LPBCK_TST_EN; 
      unsigned int gemroc_ROC_OVT_LIMIT; 
      unsigned int gemroc_A_OVC_LIMIT_FEB0; 
      unsigned int gemroc_A_OVV_LIMIT_FEB0;
      unsigned int gemroc_A_OVC_LIMIT_FEB1;
      unsigned int gemroc_A_OVV_LIMIT_FEB1;
      unsigned int gemroc_A_OVC_LIMIT_FEB2 ;
      unsigned int gemroc_A_OVV_LIMIT_FEB2;
      unsigned int gemroc_A_OVC_LIMIT_FEB3;
      unsigned int gemroc_A_OVV_LIMIT_FEB3;
      unsigned int gemroc_D_OVC_LIMIT_FEB0;
      unsigned int gemroc_D_OVV_LIMIT_FEB0;
      unsigned int gemroc_OVT_LIMIT_FEB0 ;
      unsigned int gemroc_D_OVC_LIMIT_FEB1;
      unsigned int gemroc_D_OVV_LIMIT_FEB1;
      unsigned int gemroc_OVT_LIMIT_FEB1; 
      unsigned int gemroc_D_OVC_LIMIT_FEB2;
      unsigned int gemroc_D_OVV_LIMIT_FEB2;
      unsigned int gemroc_OVT_LIMIT_FEB2; 
      unsigned int gemroc_D_OVC_LIMIT_FEB3; 
      unsigned int gemroc_D_OVV_LIMIT_FEB3; 
      unsigned int gemroc_OVT_LIMIT_FEB3; 
      char* gemroc_command_string; 
      char gemroc_command_list[6][32] = {"NONE",
                            "CMD_GEMROC_LV_CFG_WR",
                            "CMD_GEMROC_LV_CFG_RD",
                            "CMD_GEMROC_LV_IVT_UPDATE",
                            "CMD_GEMROC_LV_IVT_READ",
                            "CMD_GEMROC_TIMING_DELAYS_UPDATE"};
      unsigned int gemroc_is_a_write;
      unsigned int gemroc_gemroc_cmd_code;
      unsigned int gemroc_command_words[12];

      unsigned int DAQ_TARGET_GEMROC_ID;
      char*        DAQ_cfg_filename;
      unsigned int DAQ_parameter_array[15];
      unsigned int DAQ_EN_TM_TCAM_pattern;
      unsigned int DAQ_TP_Pos_nNeg;
      unsigned int DAQ_AUTO_TP_EN_pattern;
      unsigned int DAQ_AUTO_L1_EN_pattern;
      unsigned int DAQ_TL_nTM_ACQ;
      unsigned int DAQ_Periodic_TP_EN_pattern; 
      unsigned int DAQ_TP_period ;
      unsigned int DAQ_LowerDataScanWindowOffset;
      unsigned int DAQ_Periodic_L1_EN_pattern;
      unsigned int DAQ_L1_period;
      unsigned int DAQ_UpperDataScanWindowOffset;
      unsigned int DAQ_TP_width; 
      unsigned int DAQ_L1_latency;
      unsigned int DAQ_UDP_DATA_DESTINATION_IPPORT;
      unsigned int DAQ_UDP_DATA_DESTINATION_IPADDR;
      char* DAQ_command_string; 
      unsigned int DAQ_target_TCAM_ID;
      unsigned int DAQ_to_ALL_TCAM_enable;
      unsigned int DAQ_number_of_repetitions;
      char DAQ_command_list[10][35] ={"NONE",
                            "CMD_GEMROC_DAQ_CFG_WR",
                            "CMD_GEMROC_DAQ_CFG_RD",
                            "CMD_GEMROC_DAQ_TIGER_GCFGREG_RESET",
                            "CMD_GEMROC_DAQ_TIGER_SYNCH_RST",
                            "CMD_GEMROC_DAQ_TCAM_SYNCH_RST",
                            "CMD_GEMROC_DAQ_TP_GEN",
                            "CMD_GEMROC_DAQ_L1_GEN",
                            "CMD_GEMROC_DAQ_ACK_SEQ_ERROR",
                            "CMD_GEMROC_DAQ_XCVR_LPBCK_TEST"};
      unsigned int DAQ_is_a_write;
      unsigned int DAQ_gemroc_cmd_code;
      unsigned int DAQ_command_words[5];

      unsigned int num=0;//count send times

FILE *file_name = NULL;
FILE *fd_cmd=NULL;

//check_file = 'check_para_class.txt'
//file_name = open(check_file, 'w')

void write_array_to_txt(char*msg,int*array,int arraylen)
{   int i,j;
j=sprintf(cmd_message,"\n%s, len= %2d\n",msg,arraylen);
fwrite(cmd_message,sizeof(unsigned char),j,file_name);
j=0;
for(i=0;i<arraylen;i++)
j+=sprintf(cmd_message+j,"%d ",array[i]);
fwrite(cmd_message,sizeof(unsigned char),j,file_name);
}


void print_cmd(char*msg,int*array,int len)
{   int i,j,arraylen;
arraylen=len>>2;
j=sprintf(cmd_message,"\n%s, cmd len= %d \n",msg,arraylen);
fwrite(cmd_message,sizeof(unsigned char),j,fd_cmd);
j=0;
for(i=0;i<arraylen;i++)
j+=sprintf(cmd_message+j,"%08X ",array[i]);
fwrite(cmd_message,sizeof(unsigned char),j,fd_cmd);
}






void print_send_cmd(char*buff,int cmdlen)
{int i; 
for(i=0;i<cmdlen;i++)
{
if(i%4==0)
printf(" ");
printf("%02X",buff[i]);
}
printf("\n");  }


unsigned int swap_order_N_bits(unsigned int Hex_data,unsigned int N_bits)
{
  unsigned int temp = 0,i;
for(i=0;i<N_bits;i++)
    if ((Hex_data >> i) & 1)
       temp |= 1 << (N_bits - 1 - i);
    return temp;}

void readfromtxt(char* txt_file_name,unsigned int* buf,unsigned int Len)
{
   FILE *fp = NULL;
   int i;
   for( i=0;i<Len;i++)
     buf[i]=0;
   fp = fopen(txt_file_name, "r");
   if(fp==NULL)
   printf("open file : %s error\n",txt_file_name); 
   for( i=0;i<Len;i++)
      fscanf(fp,"%d",&buf[i]);
   fclose(fp);
}

void send_GEMROC_CFG_CMD_PKT( unsigned int TARGET_GEMROC_ID_param, char*info, 
                              void*array_to_send_param, int cmdlen,char*DEST_IP_ADDRESS_PARAM, unsigned int DEST_PORT_NO_PARAM)
{  int i,j;
 memcpy(buff,array_to_send_param,cmdlen);
for(i=0;i<cmdlen;i=i+4)
for(j=0;j<4;j++)
buff2[i+j]=buff[i+3-j];
sin_dest.sin_addr.s_addr=inet_addr(DEST_IP_ADDRESS_PARAM);
sin_dest.sin_port=htons(DEST_PORT_NO_PARAM);
sendto(socket_descriptor,buff2,cmdlen,0,(struct sockaddr *)&sin_dest,sin_len);
num++;
j=sprintf(cmd_message,"\nnum: %3d, %s\n",num,info); // C4996
fwrite(cmd_message,sizeof(unsigned char),j,fw);
j=0;
for(i=0;i<cmdlen;i++)
j+=sprintf(cmd_message+j,"%02x",buff2[i]);
fwrite(cmd_message,sizeof(unsigned char),cmdlen<<1,fw);

recvfrom(socket_descriptor1,buff,cmdlen,0,NULL,NULL);
}

void send_TIGER_GCFG_Reg_CMD_PKT(unsigned int TIGER_ID_param, char*info, void*array_to_send_param, int cmdlen,
                                 char*DEST_IP_ADDRESS_PARAM, unsigned int DEST_PORT_NO_PARAM, char*log_fname_param)
{
 int i,j;
 memcpy(buff,array_to_send_param,cmdlen);
for(i=0;i<cmdlen;i=i+4)
for(j=0;j<4;j++)
buff2[i+j]=buff[i+3-j];
sin_dest.sin_addr.s_addr=inet_addr(DEST_IP_ADDRESS_PARAM);
sin_dest.sin_port=htons(DEST_PORT_NO_PARAM);
sendto(socket_descriptor,buff2,cmdlen,0,(struct sockaddr *)&sin_dest,sin_len);
num++;
j=sprintf(cmd_message,"\nnum: %3d, %s\n",num,info);
fwrite(cmd_message,sizeof(unsigned char),j,fw);
j=0;
for(i=0;i<cmdlen;i++)
j+=sprintf(cmd_message+j,"%02x",buff2[i]);
fwrite(cmd_message,sizeof(unsigned char),cmdlen<<1,fw);

recvfrom(socket_descriptor1,buff,cmdlen,0,NULL,NULL);
}

void send_TIGER_Ch_CFG_Reg_CMD_PKT(unsigned int TIGER_ID_param, char*info,
       void*array_to_send_param,int cmdlen, char*DEST_IP_ADDRESS_PARAM, unsigned int DEST_PORT_NO_PARAM, char*log_fname_param)
{
int i,j;
 memcpy(buff,array_to_send_param,cmdlen);
for(i=0;i<cmdlen;i=i+4)
for(j=0;j<4;j++)
buff2[i+j]=buff[i+3-j];
sin_dest.sin_addr.s_addr=inet_addr(DEST_IP_ADDRESS_PARAM);
sin_dest.sin_port=htons(DEST_PORT_NO_PARAM);
sendto(socket_descriptor,buff2,cmdlen,0,(struct sockaddr *)&sin_dest,sin_len);
num++;
j=sprintf(cmd_message,"\nnum: %3d, %s\n",num,info);
fwrite(cmd_message,sizeof(unsigned char),j,fw);
j=0;
for(i=0;i<cmdlen;i++)
j+=sprintf(cmd_message+j,"%02x",buff2[i]);
fwrite(cmd_message,sizeof(unsigned char),cmdlen<<1,fw);

recvfrom(socket_descriptor1,buff,cmdlen,0,NULL,NULL);
}

void GEMROC_IVT_read_and_log(unsigned int GEMROC_ID_param, unsigned int display_enable_param, 
                             unsigned int log_enable_param, char*info)
{  
    gemroc_set_target_GEMROC(GEMROC_ID_param);
    gemroc_set_gemroc_cmd_code("CMD_GEMROC_LV_IVT_READ",1);
    gemroc_update_command_words();
    send_GEMROC_CFG_CMD_PKT( GEMROC_ID_param,info,gemroc_command_words,sizeof(gemroc_command_words), DEST_IP_ADDRESS, DEST_PORT_NO );
}

void send_GEMROC_LV_CMD (unsigned int GEMROC_ID_param, char*COMMAND_STRING_PARAM,char*info)
{
    gemroc_set_target_GEMROC(GEMROC_ID_param);
    gemroc_set_gemroc_cmd_code(COMMAND_STRING_PARAM,1);
    gemroc_update_command_words();
    send_GEMROC_CFG_CMD_PKT( GEMROC_ID_param, info,gemroc_command_words,sizeof(gemroc_command_words), DEST_IP_ADDRESS, DEST_PORT_NO );  }

void FEBPwrEnPattern_set(unsigned int GEMROC_ID_param, unsigned int FEB_PWREN_pattern_param,char*info)
{
    gemroc_set_target_GEMROC(GEMROC_ID_param);
    gemroc_set_FEB_PWR_EN_pattern(FEB_PWREN_pattern_param);
    send_GEMROC_LV_CMD (GEMROC_ID_param,"CMD_GEMROC_LV_CFG_WR",info);  }

void set_FEB_timing_delays(unsigned int GEMROC_ID_param, unsigned int FEB3_TDly, unsigned int FEB2_TDly,
                            unsigned int FEB1_TDly, unsigned int FEB0_TDly,char*info)
{
    gemroc_set_target_GEMROC(GEMROC_ID_param);
    gemroc_set_timing_toFEB_delay(FEB3_TDly, FEB2_TDly, FEB1_TDly, FEB0_TDly);
    send_GEMROC_LV_CMD(GEMROC_ID_param,"CMD_GEMROC_LV_CFG_WR",info);  }
void send_GEMROC_DAQ_CMD(unsigned int GEMROC_ID_param,char*info, char*COMMAND_STRING_PARAM)
{
    DAQ_set_target_GEMROC(GEMROC_ID_param);
    DAQ_set_gemroc_cmd_code(COMMAND_STRING_PARAM,1);
    DAQ_update_command_words();
    send_GEMROC_CFG_CMD_PKT( GEMROC_ID_param, info, DAQ_command_words,sizeof(DAQ_command_words), DEST_IP_ADDRESS, DEST_PORT_NO );}

void  ResetTgtGEMROC_ALL_TIGER_GCfgReg(unsigned int GEMROC_ID_param, char*info)
{
    send_GEMROC_DAQ_CMD(GEMROC_ID_param,info, "CMD_GEMROC_DAQ_TIGER_GCFGREG_RESET");
    printf("\n CMD_GEMROC_DAQ_TIGER_GCFGREG_RESET\n");
    sleep(5);
}

void WriteTgtGEMROC_TIGER_GCfgReg_fromfile(char*GCFGReg_setting_inst, unsigned int GEMROC_ID_param, unsigned int TIGER_ID_param,
                                            char* GCFGReg_def_fname_param, char*info)
{
    g_reg_reload_gcfg_settings_from_file(GCFGReg_def_fname_param);
    g_reg_set_target_GEMROC(GEMROC_ID_param);
    g_reg_set_target_TIGER(TIGER_ID_param);
    g_reg_set_command_code("WR");
    g_reg_update_command_words();
    send_TIGER_GCFG_Reg_CMD_PKT( TIGER_ID_param, info, g_reg_command_words,sizeof(g_reg_command_words),DEST_IP_ADDRESS, DEST_PORT_NO, info);
}

void set_FE_TPEnable(char*GCFGReg_setting_inst, unsigned int GEMROC_ID_param, unsigned int TIGER_ID_param,
                      unsigned int FE_TPEnable_param, char*info)
{
    g_reg_set_FE_TPEnable(FE_TPEnable_param);
    g_reg_set_target_GEMROC(GEMROC_ID_param);
    g_reg_set_target_TIGER(TIGER_ID_param);
    char*COMMAND_STRING = "WR";
    g_reg_set_command_code(COMMAND_STRING);
    g_reg_update_command_words();
    send_TIGER_GCFG_Reg_CMD_PKT( TIGER_ID_param,info,g_reg_command_words,sizeof(g_reg_command_words),DEST_IP_ADDRESS, DEST_PORT_NO, info);
}

void ReadTgtGEMROC_TIGER_GCfgReg (char*GCFGReg_setting_inst,unsigned int  GEMROC_ID_param, 
                                   unsigned int TIGER_ID_param, char*info)
{
    g_reg_set_target_GEMROC(GEMROC_ID_param);
    g_reg_set_target_TIGER(TIGER_ID_param);
    char*COMMAND_STRING = "RD";
    g_reg_set_command_code(COMMAND_STRING);
    g_reg_update_command_words();
    send_TIGER_GCFG_Reg_CMD_PKT( TIGER_ID_param,info, g_reg_command_words,sizeof(g_reg_command_words), DEST_IP_ADDRESS, DEST_PORT_NO,info);   }

void WriteTgtGEMROC_TIGER_ChCfgReg_fromfile (char*ChCFGReg_setting_inst,unsigned int  GEMROC_ID_param, 
          unsigned int TIGER_ID_param, unsigned int channel_ID_param, char*ChCFGReg_def_fname_param, char*info)
{   int i;
    ch_reg_reload_chcfg_settings_from_file(ChCFGReg_def_fname_param);
    print_cmd("1576,ch_reg_reload_chcfg_settings_from_file",ch_reg_command_words,sizeof(ch_reg_command_words));
    ch_reg_set_target_GEMROC(GEMROC_ID_param);
    print_cmd("1576,ch_reg_set_target_GEMROC",ch_reg_command_words,sizeof(ch_reg_command_words));
    ch_reg_set_target_TIGER(TIGER_ID_param);
    print_cmd("1576,ch_reg_set_target_TIGER",ch_reg_command_words,sizeof(ch_reg_command_words));
    ch_reg_set_to_ALL_param (0);
    print_cmd("1576,ch_reg_set_to_ALL_param",ch_reg_command_words,sizeof(ch_reg_command_words));
    char*COMMAND_STRING = "WR";
    ch_reg_set_command_code(COMMAND_STRING);
    print_cmd("1576,ch_reg_set_command_code",ch_reg_command_words,sizeof(ch_reg_command_words));
    if (channel_ID_param < 64)  {
        ch_reg_set_target_channel(channel_ID_param);
        ch_reg_update_command_words();
        send_TIGER_Ch_CFG_Reg_CMD_PKT(TIGER_ID_param,info,ch_reg_command_words,sizeof(ch_reg_command_words),DEST_IP_ADDRESS,DEST_PORT_NO,info) ; }
    else {
        for(i=0;i<64;i++)
       {
            ch_reg_set_target_channel(i);
            ch_reg_update_command_words();
            if(i==0)
               print_cmd("1576,ch_reg_update_command_words",ch_reg_command_words,sizeof(ch_reg_command_words));
            send_TIGER_Ch_CFG_Reg_CMD_PKT(TIGER_ID_param,info,ch_reg_command_words,sizeof(ch_reg_command_words),DEST_IP_ADDRESS,DEST_PORT_NO,info);  }  }  }


void ReadTgtGEMROC_TIGER_ChCfgReg (char*ChCFGReg_setting_inst, unsigned int GEMROC_ID_param, unsigned int TIGER_ID_param, 
                                    unsigned int channel_ID_param, unsigned int verbose_mode, char*info)
{   int i;
    ch_reg_set_target_GEMROC(GEMROC_ID_param);
    ch_reg_set_target_TIGER(TIGER_ID_param);
    ch_reg_set_target_channel(channel_ID_param);
    char*COMMAND_STRING = "RD";
    ch_reg_set_command_code(COMMAND_STRING);
    if (channel_ID_param < 64){
        ch_reg_set_target_channel(channel_ID_param);
        ch_reg_update_command_words();
        send_TIGER_Ch_CFG_Reg_CMD_PKT(TIGER_ID_param, info,ch_reg_command_words,sizeof(ch_reg_command_words), DEST_IP_ADDRESS, DEST_PORT_NO, info);
        if (verbose_mode == 1); }
    else
        for(i=0;i<64;i++){
            ch_reg_set_target_channel(i);
            ch_reg_update_command_words();
            send_TIGER_Ch_CFG_Reg_CMD_PKT(TIGER_ID_param,info,ch_reg_command_words,sizeof(ch_reg_command_words), DEST_IP_ADDRESS, DEST_PORT_NO,info );
            if (verbose_mode == 1);;}
}

void Set_GEMROC_TIGER_ch_TPEn (char*ChCFGReg_setting_inst, unsigned int GEMROC_ID_param, unsigned int TIGER_ID_param,
          unsigned int  Channel_ID_param, unsigned int TP_disable_FE_param, unsigned int TriggerMode_param, char*info)
{   int i;
    ch_reg_set_target_GEMROC(GEMROC_ID_param);
    ch_reg_set_target_TIGER(TIGER_ID_param);
    ch_reg_set_to_ALL_param (0);
    char*COMMAND_STRING = "WR";
    ch_reg_set_command_code(COMMAND_STRING);
    if (Channel_ID_param < 64){
        ch_reg_set_target_channel(Channel_ID_param);
        ch_reg_set_TP_disable_FE(TP_disable_FE_param);
        ch_reg_TriggerMode = TriggerMode_param;
        ch_reg_update_command_words();
        send_TIGER_Ch_CFG_Reg_CMD_PKT(TIGER_ID_param,info,ch_reg_command_words,sizeof(ch_reg_command_words), DEST_IP_ADDRESS, DEST_PORT_NO,info) ;}
    else
        for(i=0;i<64;i++){
            ch_reg_set_target_channel(i);
            ch_reg_set_TP_disable_FE(TP_disable_FE_param);
            ch_reg_TriggerMode = TriggerMode_param;
            ch_reg_update_command_words();
            send_TIGER_Ch_CFG_Reg_CMD_PKT(TIGER_ID_param, info,ch_reg_command_words,sizeof(ch_reg_command_words), DEST_IP_ADDRESS, DEST_PORT_NO,info );}
}


void Set_Vth_T1 (char*ChCFGReg_setting_inst, unsigned int GEMROC_ID_param, unsigned int TIGER_ID_param, 
            unsigned int Channel_ID_param, unsigned int VthT1_param, char*info)
{    int i;
    ch_reg_set_target_GEMROC(GEMROC_ID_param);
    ch_reg_set_target_TIGER(TIGER_ID_param);
    ch_reg_set_to_ALL_param (0);
    ch_reg_set_Vth_T1(VthT1_param);
    char*COMMAND_STRING = "WR";
    ch_reg_set_command_code(COMMAND_STRING);
    if (Channel_ID_param < 64){
        ch_reg_set_target_channel(Channel_ID_param);
        ch_reg_update_command_words();
        send_TIGER_Ch_CFG_Reg_CMD_PKT(TIGER_ID_param, info, ch_reg_command_words,sizeof(ch_reg_command_words), DEST_IP_ADDRESS, DEST_PORT_NO,info);}
    else
        for(i=0;i<64;i++) {
            ch_reg_set_target_channel(i);
            ch_reg_update_command_words();
            send_TIGER_Ch_CFG_Reg_CMD_PKT(TIGER_ID_param, info,ch_reg_command_words,sizeof(ch_reg_command_words), DEST_IP_ADDRESS, DEST_PORT_NO,info);  }    }

void set_AVcasp_global(char*GCFGReg_setting_inst, unsigned int GEMROC_ID_param, unsigned int TIGER_ID_param,
                        unsigned int AVcasp_global_param, char*info)
{
    g_reg_set_target_GEMROC(GEMROC_ID_param);
    g_reg_set_target_TIGER(TIGER_ID_param);
    g_reg_set_AVcasp_global(AVcasp_global_param);
    char*COMMAND_STRING = "WR";
    g_reg_set_command_code(COMMAND_STRING);
    g_reg_update_command_words();
    send_TIGER_GCFG_Reg_CMD_PKT( TIGER_ID_param,info, g_reg_command_words,sizeof(g_reg_command_words), DEST_IP_ADDRESS, DEST_PORT_NO, info);
}
void SynchReset_to_TgtFEB(char*info, unsigned int GEMROC_ID_param, unsigned int TargetFEB_param, unsigned int To_ALL_param)
{
    DAQ_set_target_GEMROC(GEMROC_ID_param);
    DAQ_set_target_TCAM_ID(TargetFEB_param, To_ALL_param);
    char*COMMAND_STRING = "CMD_GEMROC_DAQ_TIGER_SYNCH_RST";
    send_GEMROC_DAQ_CMD(GEMROC_ID_param, info, COMMAND_STRING);
}
void SynchReset_to_TgtTCAM(char*info, unsigned int GEMROC_ID_param, unsigned int TargetTCAM_param, unsigned int To_ALL_param)
{
    DAQ_set_target_GEMROC(GEMROC_ID_param);
    DAQ_set_target_TCAM_ID(TargetTCAM_param, To_ALL_param);
    char*COMMAND_STRING = "CMD_GEMROC_DAQ_TCAM_SYNCH_RST";
    send_GEMROC_DAQ_CMD(GEMROC_ID_param, info, COMMAND_STRING);
}

void DAQ_set_and_TL_start(char*info, unsigned int GEMROC_ID_param, 
                           unsigned int TCAM_Enable_pattern_param, unsigned int Per_FEB_TP_Enable_pattern_param)
{
    DAQ_set_target_GEMROC(GEMROC_ID_param);
    DAQ_set_EN_TM_TCAM_pattern (TCAM_Enable_pattern_param);
    DAQ_set_TP_width(5);
    DAQ_set_AUTO_TP_EN_pattern(0x0);
    DAQ_set_Periodic_TP_EN_pattern(Per_FEB_TP_Enable_pattern_param);
    DAQ_set_TL_nTM_ACQ(1);
    DAQ_set_TP_Pos_nNeg(1);
    DAQ_set_TP_period(8);
    char*COMMAND_STRING = "CMD_GEMROC_DAQ_CFG_WR";
    send_GEMROC_DAQ_CMD(GEMROC_ID_param, info, COMMAND_STRING);
}
void Set_OV_OC_OT_PWR_CUT_EN_FLAGS(char*gemroc_inst_param, unsigned int GEMROC_ID_param, unsigned int FEB_OVC_EN_pattern_param, 
       unsigned int FEB_OVV_EN_pattern_param, unsigned int FEB_OVT_EN_pattern_param,unsigned int ROC_OVT_EN_param, char*info)
{
    gemroc_FEB_OVC_EN_pattern = FEB_OVC_EN_pattern_param & 0xF;
    gemroc_FEB_OVV_EN_pattern = FEB_OVV_EN_pattern_param & 0xF;
    gemroc_FEB_OVT_EN_pattern = FEB_OVT_EN_pattern_param & 0xF;
    gemroc_ROC_OVT_EN = ROC_OVT_EN_param & 0x1;
    char*COMMAND_STRING = "CMD_GEMROC_LV_CFG_WR";
    send_GEMROC_LV_CMD (GEMROC_ID_param, COMMAND_STRING,info);
}

void Set_OVVA_LIMIT(char*gemroc_inst_param, unsigned int GEMROC_ID_param, unsigned int FEB3_OVVA_thr_param, unsigned int FEB2_OVVA_thr_param,
             unsigned int  FEB1_OVVA_thr_param, unsigned int FEB0_OVVA_thr_param, char*info)
{
    unsigned int FEB3_OVVA_thr_Unsigned8;
    unsigned int FEB2_OVVA_thr_Unsigned8;
    unsigned int FEB1_OVVA_thr_Unsigned8;
    unsigned int FEB0_OVVA_thr_Unsigned8;
    float  Vout_atten_factor = 0.5;
    unsigned int V_ADC_data_shift = 4;
    float shifted_V_ADC_res_mV_1LSB = 0.305 * 16;
    unsigned int FEB3_OVVA_thr_int = (FEB3_OVVA_thr_param * Vout_atten_factor)/shifted_V_ADC_res_mV_1LSB;
    if (FEB3_OVVA_thr_int <= 511)
        FEB3_OVVA_thr_Unsigned8 = FEB3_OVVA_thr_int;
    else
        FEB3_OVVA_thr_Unsigned8 = 511;
    unsigned int FEB2_OVVA_thr_int = ((FEB2_OVVA_thr_param * Vout_atten_factor)/shifted_V_ADC_res_mV_1LSB);
    if (FEB2_OVVA_thr_int <= 511)
        FEB2_OVVA_thr_Unsigned8 = FEB2_OVVA_thr_int;
    else
        FEB2_OVVA_thr_Unsigned8 = 511;
    unsigned int FEB1_OVVA_thr_int = (FEB1_OVVA_thr_param * Vout_atten_factor)/shifted_V_ADC_res_mV_1LSB;
    if (FEB1_OVVA_thr_int <= 511)
        FEB1_OVVA_thr_Unsigned8 = FEB1_OVVA_thr_int;
    else
        FEB1_OVVA_thr_Unsigned8 = 511;
    unsigned int FEB0_OVVA_thr_int = (FEB0_OVVA_thr_param * Vout_atten_factor)/shifted_V_ADC_res_mV_1LSB;
    if (FEB0_OVVA_thr_int <= 511)
        FEB0_OVVA_thr_Unsigned8 = FEB0_OVVA_thr_int;
    else
        FEB0_OVVA_thr_Unsigned8 = 511;
    gemroc_A_OVV_LIMIT_FEB3 = FEB3_OVVA_thr_Unsigned8;
    gemroc_A_OVV_LIMIT_FEB2 = FEB2_OVVA_thr_Unsigned8;
    gemroc_A_OVV_LIMIT_FEB1 = FEB1_OVVA_thr_Unsigned8;
    gemroc_A_OVV_LIMIT_FEB0 = FEB0_OVVA_thr_Unsigned8;
    char*COMMAND_STRING = "CMD_GEMROC_LV_CFG_WR";
    send_GEMROC_LV_CMD (GEMROC_ID_param, COMMAND_STRING,info);
}

void Set_OVVD_LIMIT(char*gemroc_inst_param, unsigned int GEMROC_ID_param, unsigned int FEB3_OVVD_thr_param, unsigned int FEB2_OVVD_thr_param,
                           unsigned int  FEB1_OVVD_thr_param, unsigned int FEB0_OVVD_thr_param, char*info)
{   
    unsigned int FEB3_OVVD_thr_Unsigned8;
    unsigned int FEB2_OVVD_thr_Unsigned8;
    unsigned int FEB1_OVVD_thr_Unsigned8;
    unsigned int FEB0_OVVD_thr_Unsigned8;
    float Vout_atten_factor = 0.5;
    unsigned int V_ADC_data_shift = 4;
    float shifted_V_ADC_res_mV_1LSB = 0.305 *16;
    unsigned int FEB3_OVVD_thr_int = (FEB3_OVVD_thr_param * Vout_atten_factor)/shifted_V_ADC_res_mV_1LSB;
    if (FEB3_OVVD_thr_int <= 511)
        FEB3_OVVD_thr_Unsigned8 = FEB3_OVVD_thr_int;
    else
        FEB3_OVVD_thr_Unsigned8 = 511;
    unsigned int FEB2_OVVD_thr_int =(FEB2_OVVD_thr_param * Vout_atten_factor)/shifted_V_ADC_res_mV_1LSB;
    if (FEB2_OVVD_thr_int <= 511)
        FEB2_OVVD_thr_Unsigned8 = FEB2_OVVD_thr_int;
    else
        FEB2_OVVD_thr_Unsigned8 = 511;
    unsigned int FEB1_OVVD_thr_int = (FEB1_OVVD_thr_param * Vout_atten_factor)/shifted_V_ADC_res_mV_1LSB;
    if (FEB1_OVVD_thr_int <= 511)
        FEB1_OVVD_thr_Unsigned8 = FEB1_OVVD_thr_int;
    else
        FEB1_OVVD_thr_Unsigned8 = 511;
    unsigned int FEB0_OVVD_thr_int = (FEB0_OVVD_thr_param * Vout_atten_factor)/shifted_V_ADC_res_mV_1LSB;
    if (FEB0_OVVD_thr_int <= 511)
        FEB0_OVVD_thr_Unsigned8 = FEB0_OVVD_thr_int;
    else
        FEB0_OVVD_thr_Unsigned8 = 511;
    gemroc_D_OVV_LIMIT_FEB3 = FEB3_OVVD_thr_Unsigned8;
    gemroc_D_OVV_LIMIT_FEB2 = FEB2_OVVD_thr_Unsigned8;
    gemroc_D_OVV_LIMIT_FEB1 = FEB1_OVVD_thr_Unsigned8;
    gemroc_D_OVV_LIMIT_FEB0 = FEB0_OVVD_thr_Unsigned8;
    char*COMMAND_STRING = "CMD_GEMROC_LV_CFG_WR";
    send_GEMROC_LV_CMD (GEMROC_ID_param, COMMAND_STRING,info);
}
void Set_OVCA_LIMIT(char*gemroc_inst_param,unsigned int GEMROC_ID_param, unsigned int FEB3_OVCA_thr_param, unsigned int FEB2_OVCA_thr_param,
                    unsigned int  FEB1_OVCA_thr_param, unsigned int FEB0_OVCA_thr_param, char*info)
{
    float IADC_conv_fact_INA_GAIN_50 = 8.13;
    float IADC_conv_fact_INA_GAIN_200 = 8.13 / 4 ;
    float IADC_conv_fact_INA_GAIN;
    unsigned int FEB3_OVCA_thr_U9;
    unsigned int FEB2_OVCA_thr_U9;
    unsigned int FEB1_OVCA_thr_U9;
    unsigned int FEB0_OVCA_thr_U9;
    if ( GEMROC_ID < 3)
        IADC_conv_fact_INA_GAIN = IADC_conv_fact_INA_GAIN_50;
    else
        IADC_conv_fact_INA_GAIN = IADC_conv_fact_INA_GAIN_200;
    unsigned int FEB3_OVCA_thr_int = FEB3_OVCA_thr_param / IADC_conv_fact_INA_GAIN;
    if (FEB3_OVCA_thr_int <= 511)
        FEB3_OVCA_thr_U9 = FEB3_OVCA_thr_int;
    else
        FEB3_OVCA_thr_U9 = 511;
    int FEB2_OVCA_thr_int = FEB2_OVCA_thr_param / IADC_conv_fact_INA_GAIN;
    if (FEB2_OVCA_thr_int <= 511)
        FEB2_OVCA_thr_U9 = FEB2_OVCA_thr_int;
    else
        FEB2_OVCA_thr_U9 = 511;
    int FEB1_OVCA_thr_int = FEB1_OVCA_thr_param / IADC_conv_fact_INA_GAIN;
    if (FEB1_OVCA_thr_int <= 511)
        FEB1_OVCA_thr_U9 = FEB1_OVCA_thr_int;
    else
        FEB1_OVCA_thr_U9 = 511;
    int FEB0_OVCA_thr_int = FEB0_OVCA_thr_param / IADC_conv_fact_INA_GAIN;
    if (FEB0_OVCA_thr_int <= 511)
        FEB0_OVCA_thr_U9 = FEB0_OVCA_thr_int;
    else
        FEB0_OVCA_thr_U9 = 511;
    gemroc_A_OVC_LIMIT_FEB3 = FEB3_OVCA_thr_U9;
    gemroc_A_OVC_LIMIT_FEB2 = FEB2_OVCA_thr_U9;
    gemroc_A_OVC_LIMIT_FEB1 = FEB1_OVCA_thr_U9;
    gemroc_A_OVC_LIMIT_FEB0 = FEB0_OVCA_thr_U9;
    char*COMMAND_STRING = "CMD_GEMROC_LV_CFG_WR";
    send_GEMROC_LV_CMD (GEMROC_ID_param, COMMAND_STRING,info);
}
void Set_OVCD_LIMIT(char*gemroc_inst_param, unsigned int GEMROC_ID_param, unsigned int FEB3_OVCD_thr_param, unsigned int FEB2_OVCD_thr_param,
                     unsigned int  FEB1_OVCD_thr_param,unsigned int  FEB0_OVCD_thr_param,char* info)
{
    float IADC_conv_fact_INA_GAIN_50 = 8.13;
    float IADC_conv_fact_INA_GAIN_200 = 8.13 / 4 ;
    float IADC_conv_fact_INA_GAIN;
    int FEB3_OVCD_thr_U9; 
    int FEB2_OVCD_thr_U9;
    int FEB1_OVCD_thr_U9;
    int FEB0_OVCD_thr_U9;
    if ( GEMROC_ID < 3)
        IADC_conv_fact_INA_GAIN = IADC_conv_fact_INA_GAIN_50;
    else
        IADC_conv_fact_INA_GAIN = IADC_conv_fact_INA_GAIN_200;
    int FEB3_OVCD_thr_int = FEB3_OVCD_thr_param / IADC_conv_fact_INA_GAIN;
    if (FEB3_OVCD_thr_int <= 511)
        FEB3_OVCD_thr_U9 = FEB3_OVCD_thr_int;
    else
        FEB3_OVCD_thr_U9 = 511;
    int FEB2_OVCD_thr_int = FEB2_OVCD_thr_param / IADC_conv_fact_INA_GAIN;
    if (FEB2_OVCD_thr_int <= 511)
        FEB2_OVCD_thr_U9 = FEB2_OVCD_thr_int;
    else
        FEB2_OVCD_thr_U9 = 511;
    int FEB1_OVCD_thr_int = FEB1_OVCD_thr_param / IADC_conv_fact_INA_GAIN;
    if (FEB1_OVCD_thr_int <= 511)
        FEB1_OVCD_thr_U9 = FEB1_OVCD_thr_int;
    else
        FEB1_OVCD_thr_U9 = 511;
    int FEB0_OVCD_thr_int = FEB0_OVCD_thr_param / IADC_conv_fact_INA_GAIN;
    if (FEB0_OVCD_thr_int <= 511)
        FEB0_OVCD_thr_U9 = FEB0_OVCD_thr_int;
    else
        FEB0_OVCD_thr_U9 = 511;
    gemroc_D_OVC_LIMIT_FEB3 = FEB3_OVCD_thr_U9;
    gemroc_D_OVC_LIMIT_FEB2 = FEB2_OVCD_thr_U9;
    gemroc_D_OVC_LIMIT_FEB1 = FEB1_OVCD_thr_U9;
    gemroc_D_OVC_LIMIT_FEB0 = FEB0_OVCD_thr_U9;
    char*COMMAND_STRING = "CMD_GEMROC_LV_CFG_WR";
    send_GEMROC_LV_CMD (GEMROC_ID_param, COMMAND_STRING,info);
}

void Set_OVTF_LIMIT(char*gemroc_inst_param, int GEMROC_ID_param, int FEB3_OVTF_thr_param,int FEB2_OVTF_thr_param, 
                     int FEB1_OVTF_thr_param, int FEB0_OVTF_thr_param, char*info)
{
    float T_ref_PT1000 = 25.0;
    float V_ADC_at_25C = 247.2;
    float ADC_res_mV_1LSB = 0.305;
    int T_ADC_data_shift = 2;
    float calibration_offset_mV_FEB3 = 1.0;
    float calibration_offset_mV_FEB2 = 1.0;
    float calibration_offset_mV_FEB1 = 1.0;
    float calibration_offset_mV_FEB0 = 1.0;
    float shifted_T_ADC_res_mV_1LSB = ADC_res_mV_1LSB * 4;
    float deltaT_over_deltaV_ratio = 1.283;
    int FEB3_OVTF_thr_Unsigned8;
    int FEB2_OVTF_thr_Unsigned8;
    int FEB1_OVTF_thr_Unsigned8;
    int FEB0_OVTF_thr_Unsigned8;
    int FEB3_OVTF_thr_int = (((FEB3_OVTF_thr_param - T_ref_PT1000)/deltaT_over_deltaV_ratio)+V_ADC_at_25C-calibration_offset_mV_FEB3)/shifted_T_ADC_res_mV_1LSB ;
    if (FEB3_OVTF_thr_int <= 255)
        FEB3_OVTF_thr_Unsigned8 = FEB3_OVTF_thr_int;
    else
        FEB3_OVTF_thr_Unsigned8 = 255;
    int FEB2_OVTF_thr_int = (((FEB2_OVTF_thr_param - T_ref_PT1000)/deltaT_over_deltaV_ratio)+V_ADC_at_25C-calibration_offset_mV_FEB2)/shifted_T_ADC_res_mV_1LSB ;
    if (FEB2_OVTF_thr_int <= 255)
        FEB2_OVTF_thr_Unsigned8 = FEB2_OVTF_thr_int;
    else
        FEB2_OVTF_thr_Unsigned8 = 255;
    int FEB1_OVTF_thr_int =(((FEB1_OVTF_thr_param - T_ref_PT1000)/deltaT_over_deltaV_ratio)+V_ADC_at_25C-calibration_offset_mV_FEB1)/shifted_T_ADC_res_mV_1LSB ;
    if (FEB1_OVTF_thr_int <= 255)
        FEB1_OVTF_thr_Unsigned8 = FEB1_OVTF_thr_int;
    else
        FEB1_OVTF_thr_Unsigned8 = 255;
    int FEB0_OVTF_thr_int = (((FEB0_OVTF_thr_param - T_ref_PT1000)/deltaT_over_deltaV_ratio)+V_ADC_at_25C-calibration_offset_mV_FEB0)/shifted_T_ADC_res_mV_1LSB ;
    if (FEB0_OVTF_thr_int <= 255)
        FEB0_OVTF_thr_Unsigned8 = FEB0_OVTF_thr_int;
    else
        FEB0_OVTF_thr_Unsigned8 = 255;
    gemroc_OVT_LIMIT_FEB3 = FEB3_OVTF_thr_Unsigned8;
    gemroc_OVT_LIMIT_FEB2 = FEB2_OVTF_thr_Unsigned8;
    gemroc_OVT_LIMIT_FEB1 = FEB1_OVTF_thr_Unsigned8;
    gemroc_OVT_LIMIT_FEB0 = FEB0_OVTF_thr_Unsigned8;
    char*COMMAND_STRING = "CMD_GEMROC_LV_CFG_WR";
    send_GEMROC_LV_CMD (GEMROC_ID_param, COMMAND_STRING,info);
}
void set_ROC_OVT_LIMIT(char*gemroc_inst_param, int GEMROC_ID_param, int ROC_OVT_thr_param, char*info)
{
    int ROC_OVT_thr_Unsigned6;
    if (ROC_OVT_thr_param <= 63)
        ROC_OVT_thr_Unsigned6 = ROC_OVT_thr_param;
    else
        ROC_OVT_thr_Unsigned6 = 63;
    gemroc_ROC_OVT_LIMIT = ROC_OVT_thr_Unsigned6;
    char*COMMAND_STRING = "CMD_GEMROC_LV_CFG_WR";
    send_GEMROC_LV_CMD (GEMROC_ID_param,COMMAND_STRING, info);
}

void Read_GEMROC_LV_CfgReg(char*gemroc_inst_param,int GEMROC_ID_param,char* info)
{
    char*COMMAND_STRING = "CMD_GEMROC_LV_CFG_RD";
    send_GEMROC_LV_CMD(GEMROC_ID_param, COMMAND_STRING,info);
}
void Read_GEMROC_DAQ_CfgReg(char*gemroc_inst_param, int GEMROC_ID_param, char*info)
{
    char*COMMAND_STRING = "CMD_GEMROC_DAQ_CFG_RD";
    send_GEMROC_DAQ_CMD (GEMROC_ID_param, info, COMMAND_STRING);
}







void DAQ_init(  unsigned int TARGET_GEMROC_ID_param,          
                char* command_string_param,
                unsigned int TCAM_ID_param,
                unsigned int number_of_repetitions_param,
                unsigned int to_ALL_TCAM_enable_param,
                char*cfg_filename_param)
{     int i;
      unsigned int header_tag;
      unsigned int gemroc_cmd_ID ;
      unsigned int gemroc_cmd_tag;
      unsigned int gemroc_cmd_word_count;
      DAQ_TARGET_GEMROC_ID = TARGET_GEMROC_ID_param ;
      DAQ_cfg_filename = cfg_filename_param;
      readfromtxt(DAQ_cfg_filename,DAQ_parameter_array,15);
      write_array_to_txt("DAQ_init",DAQ_parameter_array,15);
      DAQ_EN_TM_TCAM_pattern = DAQ_parameter_array [GEMROC_CMD_DAQ_Num_of_params-1];
      DAQ_TP_Pos_nNeg = DAQ_parameter_array [GEMROC_CMD_DAQ_Num_of_params-2];
      DAQ_AUTO_TP_EN_pattern = DAQ_parameter_array [GEMROC_CMD_DAQ_Num_of_params-3];
      DAQ_AUTO_L1_EN_pattern = DAQ_parameter_array [GEMROC_CMD_DAQ_Num_of_params-4];
      DAQ_TL_nTM_ACQ = DAQ_parameter_array [GEMROC_CMD_DAQ_Num_of_params-5];
      DAQ_Periodic_TP_EN_pattern = DAQ_parameter_array [GEMROC_CMD_DAQ_Num_of_params-6];
      DAQ_TP_period = DAQ_parameter_array [GEMROC_CMD_DAQ_Num_of_params-7] ;
      DAQ_LowerDataScanWindowOffset = DAQ_parameter_array [GEMROC_CMD_DAQ_Num_of_params-8];
      DAQ_Periodic_L1_EN_pattern = DAQ_parameter_array [GEMROC_CMD_DAQ_Num_of_params-9] ;
      DAQ_L1_period = DAQ_parameter_array [GEMROC_CMD_DAQ_Num_of_params-10] ;
      DAQ_UpperDataScanWindowOffset = DAQ_parameter_array [GEMROC_CMD_DAQ_Num_of_params-11];
      DAQ_TP_width = DAQ_parameter_array [GEMROC_CMD_DAQ_Num_of_params-12]; 
      DAQ_L1_latency = DAQ_parameter_array [GEMROC_CMD_DAQ_Num_of_params-13];
      DAQ_UDP_DATA_DESTINATION_IPPORT = DAQ_parameter_array [GEMROC_CMD_DAQ_Num_of_params-14];
      DAQ_UDP_DATA_DESTINATION_IPADDR = DAQ_parameter_array [GEMROC_CMD_DAQ_Num_of_params-15];
      DAQ_command_string = command_string_param;
      DAQ_target_TCAM_ID = TCAM_ID_param;
      DAQ_to_ALL_TCAM_enable = to_ALL_TCAM_enable_param;
      DAQ_number_of_repetitions = number_of_repetitions_param;
      DAQ_is_a_write = 0x1;

      for(i=0;i<10;i++)
        if(strcmp(DAQ_command_string,DAQ_command_list[i])==0)
          DAQ_gemroc_cmd_code = i;
      header_tag = 0x8 << 28;
      gemroc_cmd_ID = 0xD;
      gemroc_cmd_tag = gemroc_cmd_ID << 24;
      gemroc_cmd_word_count = GEMROC_CMD_DAQ_Num_Of_PktWords - 1;
      DAQ_command_words[0]= header_tag + (DAQ_TARGET_GEMROC_ID << 16) + gemroc_cmd_tag + ((DAQ_UDP_DATA_DESTINATION_IPADDR & 0xFF)<<8) + gemroc_cmd_word_count;
      DAQ_command_words[4] = ((DAQ_UDP_DATA_DESTINATION_IPPORT & 0xF)<<26) + ((DAQ_number_of_repetitions & 0x3FF) << 16) + ((DAQ_gemroc_cmd_code & 0xF) << 11) + ((DAQ_target_TCAM_ID & 0x3) << 8) + ((DAQ_to_ALL_TCAM_enable & 0x1) << 6);
      DAQ_command_words[1] = ((DAQ_L1_latency & 0x3FF) << 20) + ((DAQ_TP_width & 0xF) << 16) + (DAQ_UpperDataScanWindowOffset & 0xFFFF);
      DAQ_command_words[2] = ((DAQ_L1_period  & 0x3FF) << 20) + ((DAQ_Periodic_L1_EN_pattern  & 0xF) << 16) + (DAQ_LowerDataScanWindowOffset & 0xFFFF);
      DAQ_command_words[3] = ((DAQ_TP_period & 0x3FF) << 20) + ((DAQ_Periodic_TP_EN_pattern & 0xF) << 16) + ((DAQ_TL_nTM_ACQ & 0x1) << 11) + ((DAQ_AUTO_L1_EN_pattern & 0x1) << 10) + ((DAQ_AUTO_TP_EN_pattern & 0x1) << 9) + ((DAQ_TP_Pos_nNeg & 0x1) << 8)  + (DAQ_EN_TM_TCAM_pattern & 0xFF);   }


unsigned int DAQ_cmd_words_array_size()
{
      return 5;  }

void DAQ_print_command_words()
{    int i;
   for(i=0;i<5;i++)
     printf("%08X\n",DAQ_command_words[i]);  }

void DAQ_set_target_GEMROC (unsigned int GEMROC_ID_param)
{     
      DAQ_TARGET_GEMROC_ID = GEMROC_ID_param & 0x1F;
      DAQ_command_words[0] = (DAQ_command_words[0] & 0xFF00FFFF) + (DAQ_TARGET_GEMROC_ID << 16);  }

void DAQ_set_target_TCAM_ID(unsigned int target_TA_param,unsigned int to_ALL_TCAM_EN_param)
{
      DAQ_target_TCAM_ID = target_TA_param & 0x3;
      DAQ_to_ALL_TCAM_enable = to_ALL_TCAM_EN_param & 0x1;  }

void DAQ_set_TP_width(unsigned int target_TP_width_param)
{
      DAQ_TP_width = target_TP_width_param & 0xF;}

void DAQ_set_TP_period(unsigned int TP_period_param)
{
      DAQ_TP_period = TP_period_param & 0x3FF;}

void DAQ_set_AUTO_TP_EN_pattern(unsigned int target_AUTO_TP_EN_param)
{
      DAQ_AUTO_TP_EN_pattern = target_AUTO_TP_EN_param & 0xF;  }

void DAQ_set_Periodic_TP_EN_pattern(unsigned int Periodic_TP_EN_pattern_param)
{
      DAQ_Periodic_TP_EN_pattern = Periodic_TP_EN_pattern_param & 0xF;  }

void DAQ_set_TL_nTM_ACQ(unsigned int TL_nTM_ACQ_param)
{
      DAQ_TL_nTM_ACQ = TL_nTM_ACQ_param & 0x1;  }

void DAQ_set_EN_TM_TCAM_pattern(unsigned int EN_TM_TCAM_pattern_param)
{
      DAQ_EN_TM_TCAM_pattern = EN_TM_TCAM_pattern_param & 0xFF;  }

void DAQ_set_TP_Pos_nNeg(unsigned int TP_Pos_nNeg_param)
{
      DAQ_TP_Pos_nNeg = TP_Pos_nNeg_param & 0x1;}

void DAQ_set_gemroc_cmd_code( char* command_string_param,unsigned int no_of_executions_param)
{    int i;
      DAQ_number_of_repetitions = no_of_executions_param;
      DAQ_command_string = command_string_param;
      for(i=0;i<10;i++)
        if(strcmp(DAQ_command_string,DAQ_command_list[i])==0)
            DAQ_gemroc_cmd_code = i;   }

void DAQ_update_command_words()
{
       if ( ((DAQ_gemroc_cmd_code & 0xF) != 0x0)&&((DAQ_gemroc_cmd_code & 0xF) != 0x1) )
       {
           DAQ_command_words[0] &= ~(0xFF << 8);
           DAQ_command_words[1]= 0;
           DAQ_command_words[2]= 0;
           DAQ_command_words[3]= 0;
           DAQ_command_words[4]= ((DAQ_number_of_repetitions & 0x3FF) << 16) + ((DAQ_gemroc_cmd_code & 0xF) << 11) + ((DAQ_target_TCAM_ID & 0x3) << 8) + ((DAQ_to_ALL_TCAM_enable & 0x1) << 6);  }
       else{
           DAQ_command_words[0]&= ~(0xFF << 8);
           DAQ_command_words[0]+= ((DAQ_UDP_DATA_DESTINATION_IPADDR & 0xFF)<<8);
           DAQ_command_words[1]= ((DAQ_L1_latency & 0x3FF) << 20) + ((DAQ_TP_width & 0xF) << 16) + (DAQ_UpperDataScanWindowOffset & 0xFFFF);
           DAQ_command_words[2]= ((DAQ_L1_period  & 0x3FF) << 20) + ((DAQ_Periodic_L1_EN_pattern  & 0xF) << 16) + (DAQ_LowerDataScanWindowOffset & 0xFFFF);
           DAQ_command_words[3]= ((DAQ_TP_period & 0x3FF) << 20) + ((DAQ_Periodic_TP_EN_pattern & 0xF) << 16) + ((DAQ_TL_nTM_ACQ & 0x1) << 11) + ((DAQ_AUTO_L1_EN_pattern & 0x1) << 10) + ((DAQ_AUTO_TP_EN_pattern & 0x1) << 9) + ((DAQ_TP_Pos_nNeg & 0x1) << 8)  + (DAQ_EN_TM_TCAM_pattern & 0xFF);
           DAQ_command_words[4]= ((DAQ_UDP_DATA_DESTINATION_IPPORT & 0xF)<<26) + ((DAQ_number_of_repetitions & 0x3FF) << 16) + ((DAQ_gemroc_cmd_code & 0xF) << 11) + ((DAQ_target_TCAM_ID & 0x3) << 8) + ((DAQ_to_ALL_TCAM_enable & 0x1) << 6);}  }


void DAQ_extract_parameters_from_UDP_packet()
{
      printf( "\n DATA_DESTINATION_IPadr  = %X %d",((DAQ_command_words[0] >>  8) &  0xFF) , ((DAQ_command_words[0] >>  8) &  0xFF) );
      printf( "\n DATA_DESTINATION_IPport = %X %d",((DAQ_command_words[4] >> 26) &  0xF)   , ((DAQ_command_words[4] >> 26) &  0xF) );
      printf( "\n L1_latency = %X %d",((DAQ_command_words[1] >> 20) &  0x3FF) , ((DAQ_command_words[1] >> 20) &  0x3FF) );
      printf( "\n TP_width = %X %d",((DAQ_command_words[1] >> 16) &  0xF)   , ((DAQ_command_words[1] >> 16) &  0xF) );
      printf( "\n UpperDataScanWindowOffset = %X %d",((DAQ_command_words[1] >> 0)  &  0xFFFF), ((DAQ_command_words[1] >> 0)  &  0xFFFF) );
      printf( "\n L1_period = %X %d",((DAQ_command_words[2] >> 20) &  0x3FF) , ((DAQ_command_words[2] >> 20) &  0x3FF) );
      printf( "\n Periodic_L1_EN_pattern = %X %d",((DAQ_command_words[2] >> 16) &  0xF)   , ((DAQ_command_words[2] >> 16) &  0xF) );
      printf( "\n LowerDataScanWindowOffset = %X %d",((DAQ_command_words[2] >> 0)  &  0xFFFF), ((DAQ_command_words[2] >> 0)  &  0xFFFF) );
      printf( "\n TP_period = %X %d",((DAQ_command_words[3] >> 20)  &  0x3FF), ((DAQ_command_words[3] >> 20)  &  0x3FF) );
      printf( "\n Periodic_TP_EN_pattern = %X %d",((DAQ_command_words[3] >> 16)  &  0xF)  , ((DAQ_command_words[3] >> 16)  &  0xF) );
      printf( "\n TL_nTM_ACQ = %X %d",((DAQ_command_words[3] >> 11)  &  0x1)  , ((DAQ_command_words[3] >> 11)  &  0x1) );
      printf( "\n AUTO_L1_EN_pattern = %X %d",((DAQ_command_words[3] >> 10)  &  0x1)  , ((DAQ_command_words[3] >> 10)  &  0x1) );
      printf( "\n AUTO_TP_EN_pattern = %X %d",((DAQ_command_words[3] >>  9)  &  0x1)  , ((DAQ_command_words[3] >>  9)  &  0x1) );
      printf( "\n TP_Pos_nNeg = %X %d",((DAQ_command_words[3] >>  8)  &  0x1)  , ((DAQ_command_words[3] >>  8)  &  0x1) );
      printf( "\n EN_TM_TCAM_pattern = %X %d",((DAQ_command_words[3] >>  0)  &  0xFF) , ((DAQ_command_words[3] >>  0)  &  0xFF) );
      printf( "\n number_of_repetitions = %X %d",((DAQ_command_words[4] >> 16)  & 0x3FF)  , ((DAQ_command_words[4] >> 16)  & 0x3FF) );
      printf( "\n target_TCAM_ID = %X %d",((DAQ_command_words[4] >>  8)  &  0x3)  , ((DAQ_command_words[4] >>  8)  &  0x3) );
      printf( "\n to_ALL_TCAM_enable = %X %d",((DAQ_command_words[4] >>  6)  &  0x1)  , ((DAQ_command_words[4] >>  6)  &  0x1) );  }




void gemroc_init(unsigned int TARGET_GEMROC_ID_param,char*command_string_param,unsigned int number_of_repetitions_param,
                 char*cfg_filename_param)
{     unsigned int header_tag,i;
      unsigned int gemroc_cmd_ID;
      unsigned int gemroc_cmd_tag;
      unsigned int gemroc_cmd_word_count;
      gemroc_TARGET_GEMROC_ID = TARGET_GEMROC_ID_param;
      gemroc_cfg_filename = cfg_filename_param;
      readfromtxt(gemroc_cfg_filename,gemroc_parameter_array,31);
      write_array_to_txt("gemroc_init",gemroc_parameter_array,31);
      gemroc_TIMING_DLY_FEB0 = gemroc_parameter_array [GEMROC_CMD_LV_Num_of_params-1];
      gemroc_TIMING_DLY_FEB1 = gemroc_parameter_array [GEMROC_CMD_LV_Num_of_params-2];
      gemroc_TIMING_DLY_FEB2 = gemroc_parameter_array [GEMROC_CMD_LV_Num_of_params-3];
      gemroc_TIMING_DLY_FEB3 = gemroc_parameter_array [GEMROC_CMD_LV_Num_of_params-4];
      gemroc_FEB_PWR_EN_pattern = gemroc_parameter_array [GEMROC_CMD_LV_Num_of_params-5];
      gemroc_FEB_OVC_EN_pattern = gemroc_parameter_array [GEMROC_CMD_LV_Num_of_params-6];
      gemroc_FEB_OVV_EN_pattern = gemroc_parameter_array [GEMROC_CMD_LV_Num_of_params-7];
      gemroc_FEB_OVT_EN_pattern = gemroc_parameter_array [GEMROC_CMD_LV_Num_of_params-8];
      gemroc_ROC_OVT_EN  = gemroc_parameter_array [GEMROC_CMD_LV_Num_of_params-9];
      gemroc_XCVR_LPBCK_TST_EN  = gemroc_parameter_array [GEMROC_CMD_LV_Num_of_params-10]; 
      gemroc_ROC_OVT_LIMIT = gemroc_parameter_array [GEMROC_CMD_LV_Num_of_params-11];
      gemroc_A_OVC_LIMIT_FEB0 = gemroc_parameter_array [GEMROC_CMD_LV_Num_of_params-12];
      gemroc_A_OVV_LIMIT_FEB0 = gemroc_parameter_array [GEMROC_CMD_LV_Num_of_params-13];
      gemroc_A_OVC_LIMIT_FEB1 = gemroc_parameter_array [GEMROC_CMD_LV_Num_of_params-14];
      gemroc_A_OVV_LIMIT_FEB1 = gemroc_parameter_array [GEMROC_CMD_LV_Num_of_params-15];
      gemroc_A_OVC_LIMIT_FEB2 = gemroc_parameter_array [GEMROC_CMD_LV_Num_of_params-16];
      gemroc_A_OVV_LIMIT_FEB2 = gemroc_parameter_array [GEMROC_CMD_LV_Num_of_params-17];
      gemroc_A_OVC_LIMIT_FEB3 = gemroc_parameter_array [GEMROC_CMD_LV_Num_of_params-18];
      gemroc_A_OVV_LIMIT_FEB3 = gemroc_parameter_array [GEMROC_CMD_LV_Num_of_params-19];
      gemroc_D_OVC_LIMIT_FEB0 = gemroc_parameter_array [GEMROC_CMD_LV_Num_of_params-20];
      gemroc_D_OVV_LIMIT_FEB0 = gemroc_parameter_array [GEMROC_CMD_LV_Num_of_params-21];
      gemroc_OVT_LIMIT_FEB0 = gemroc_parameter_array [GEMROC_CMD_LV_Num_of_params-22] ;
      gemroc_D_OVC_LIMIT_FEB1 = gemroc_parameter_array [GEMROC_CMD_LV_Num_of_params-23];
      gemroc_D_OVV_LIMIT_FEB1 = gemroc_parameter_array [GEMROC_CMD_LV_Num_of_params-24];
      gemroc_OVT_LIMIT_FEB1 = gemroc_parameter_array [GEMROC_CMD_LV_Num_of_params-25];
      gemroc_D_OVC_LIMIT_FEB2 = gemroc_parameter_array [GEMROC_CMD_LV_Num_of_params-26];
      gemroc_D_OVV_LIMIT_FEB2 = gemroc_parameter_array [GEMROC_CMD_LV_Num_of_params-27];
      gemroc_OVT_LIMIT_FEB2 = gemroc_parameter_array [GEMROC_CMD_LV_Num_of_params-28];
      gemroc_D_OVC_LIMIT_FEB3 = gemroc_parameter_array [GEMROC_CMD_LV_Num_of_params-29];
      gemroc_D_OVV_LIMIT_FEB3 = gemroc_parameter_array [GEMROC_CMD_LV_Num_of_params-30];
      gemroc_OVT_LIMIT_FEB3 = gemroc_parameter_array [GEMROC_CMD_LV_Num_of_params-31];
      gemroc_command_string = command_string_param;
      gemroc_is_a_write = 0x1;
      for(i=0;i<6;i++)
       if(strcmp(gemroc_command_string,gemroc_command_list[i])==0)
          gemroc_gemroc_cmd_code = i;
      header_tag = 0x8 << 28;
      gemroc_cmd_ID = 0xC;
      gemroc_cmd_tag = gemroc_cmd_ID << 24;
      gemroc_cmd_word_count = GEMROC_CMD_LV_Num_Of_PktWords - 1; 
      gemroc_command_words[0] = header_tag + (gemroc_TARGET_GEMROC_ID << 16) + gemroc_cmd_tag + gemroc_cmd_word_count;
      gemroc_command_words[1] = ((gemroc_OVT_LIMIT_FEB3 & 0xFF) << 22) + ((gemroc_D_OVV_LIMIT_FEB3 & 0x1FF) << 13) + ((gemroc_D_OVC_LIMIT_FEB3 & 0x1FF) << 4);
      gemroc_command_words[2]  = ((gemroc_OVT_LIMIT_FEB2 & 0xFF) << 22) + ((gemroc_D_OVV_LIMIT_FEB2 & 0x1FF) << 13) + ((gemroc_D_OVC_LIMIT_FEB2 & 0x1FF) << 4);
      gemroc_command_words[3]  = ((gemroc_OVT_LIMIT_FEB1 & 0xFF) << 22) + ((gemroc_D_OVV_LIMIT_FEB1 & 0x1FF) << 13) + ((gemroc_D_OVC_LIMIT_FEB1 & 0x1FF) << 4);
      gemroc_command_words[4]  = ((gemroc_OVT_LIMIT_FEB0 & 0xFF) << 22) + ((gemroc_D_OVV_LIMIT_FEB0 & 0x1FF) << 13) + ((gemroc_D_OVC_LIMIT_FEB0 & 0x1FF) << 4);
      gemroc_command_words[5]  = ((gemroc_A_OVV_LIMIT_FEB3 & 0x1FF) << 13) + ((gemroc_A_OVC_LIMIT_FEB3 & 0x1FF) << 4);
      gemroc_command_words[6]  = ((gemroc_A_OVV_LIMIT_FEB2 & 0x1FF) << 13) + ((gemroc_A_OVC_LIMIT_FEB2 & 0x1FF) << 4);
      gemroc_command_words[7]  = ((gemroc_A_OVV_LIMIT_FEB1 & 0x1FF) << 13) + ((gemroc_A_OVC_LIMIT_FEB1 & 0x1FF) << 4);
      gemroc_command_words[8]  = ((gemroc_A_OVV_LIMIT_FEB0 & 0x1FF) << 13) + ((gemroc_A_OVC_LIMIT_FEB0 & 0x1FF) << 4);
      gemroc_command_words[9] = ((gemroc_ROC_OVT_LIMIT & 0x3F) << 24) + ((gemroc_XCVR_LPBCK_TST_EN & 0x1) << 18) +((gemroc_ROC_OVT_EN & 0x1) << 16) + ((gemroc_FEB_OVT_EN_pattern & 0xF) << 12) + ((gemroc_FEB_OVV_EN_pattern & 0xF) << 8) + ((gemroc_FEB_OVC_EN_pattern & 0xF) << 4)+(gemroc_FEB_PWR_EN_pattern & 0xF);
      gemroc_command_words[10] = ((gemroc_TIMING_DLY_FEB3 & 0x3F) << 24) + ((gemroc_TIMING_DLY_FEB2 & 0x3F) << 16) + ((gemroc_TIMING_DLY_FEB1 & 0x3F) << 8) + ((gemroc_TIMING_DLY_FEB0 & 0x3F) << 0);
      gemroc_command_words[11] = ((gemroc_gemroc_cmd_code & 0xF) << 11);   }

unsigned int gemroc_cmd_words_array_size()
{
      return 12;  }

void gemroc_print_command_words()
{     int i;
      for(i=0;i<12;i++)
         printf("%08X\n",gemroc_command_words[i]);   }

void gemroc_set_target_GEMROC (unsigned int GEMROC_ID_param)
{
      gemroc_TARGET_GEMROC_ID = GEMROC_ID_param & 0x1F;
      gemroc_command_words[0] = (gemroc_command_words[0] & 0xFF00FFFF) + (gemroc_TARGET_GEMROC_ID << 16);}

void gemroc_set_FEB_PWR_EN_pattern(unsigned int target_FEB_PWR_EN_pattern)
{
      gemroc_FEB_PWR_EN_pattern = target_FEB_PWR_EN_pattern & 0xF;  }

void gemroc_set_timing_toFEB_delay(unsigned int FEB3dly_param, unsigned int FEB2dly_param,
                                   unsigned int FEB1dly_param,unsigned int  FEB0dly_param)
{
      gemroc_TIMING_DLY_FEB3 = FEB3dly_param & 0x3F;
      gemroc_TIMING_DLY_FEB2 = FEB2dly_param & 0x3F;
      gemroc_TIMING_DLY_FEB1 = FEB1dly_param & 0x3F;
      gemroc_TIMING_DLY_FEB0 = FEB0dly_param & 0x3F;     }

void gemroc_set_gemroc_cmd_code(char*command_string_param, unsigned int no_of_executions_param)
{     int i;
      gemroc_number_of_repetitions = no_of_executions_param;
      gemroc_command_string = command_string_param;
      for(i=0;i<6;i++)
         if(strcmp(gemroc_command_string,gemroc_command_list[i])==0)
            gemroc_gemroc_cmd_code = i;    }

void gemroc_update_command_words()
{     int i;
      if ( ((gemroc_gemroc_cmd_code & 0xF) == 0x2)||((gemroc_gemroc_cmd_code & 0xF) == 0x3)||((gemroc_gemroc_cmd_code & 0xF) == 0x4) )
        for(i=1;i<11;i++)
           gemroc_command_words[i]=0;
      else {
         gemroc_command_words[1] = ((gemroc_OVT_LIMIT_FEB3 & 0xFF) << 22) + ((gemroc_D_OVV_LIMIT_FEB3 & 0x1FF) << 13) + ((gemroc_D_OVC_LIMIT_FEB3 & 0x1FF) << 4);
         gemroc_command_words[2]  = ((gemroc_OVT_LIMIT_FEB2 & 0xFF) << 22) + ((gemroc_D_OVV_LIMIT_FEB2 & 0x1FF) << 13) + ((gemroc_D_OVC_LIMIT_FEB2 & 0x1FF) << 4);
         gemroc_command_words[3]  = ((gemroc_OVT_LIMIT_FEB1 & 0xFF) << 22) + ((gemroc_D_OVV_LIMIT_FEB1 & 0x1FF) << 13) + ((gemroc_D_OVC_LIMIT_FEB1 & 0x1FF) << 4);
         gemroc_command_words[4]  = ((gemroc_OVT_LIMIT_FEB0 & 0xFF) << 22) + ((gemroc_D_OVV_LIMIT_FEB0 & 0x1FF) << 13) + ((gemroc_D_OVC_LIMIT_FEB0 & 0x1FF) << 4);
         gemroc_command_words[5]  = ((gemroc_A_OVV_LIMIT_FEB3 & 0x1FF) << 13) + ((gemroc_A_OVC_LIMIT_FEB3 & 0x1FF) << 4);
         gemroc_command_words[6]  = ((gemroc_A_OVV_LIMIT_FEB2 & 0x1FF) << 13) + ((gemroc_A_OVC_LIMIT_FEB2 & 0x1FF) << 4);
         gemroc_command_words[7]  = ((gemroc_A_OVV_LIMIT_FEB1 & 0x1FF) << 13) + ((gemroc_A_OVC_LIMIT_FEB1 & 0x1FF) << 4);
         gemroc_command_words[8]  = ((gemroc_A_OVV_LIMIT_FEB0 & 0x1FF) << 13) + ((gemroc_A_OVC_LIMIT_FEB0 & 0x1FF) << 4);
         gemroc_command_words[9] = ((gemroc_ROC_OVT_LIMIT & 0x3F) << 24) + ((gemroc_XCVR_LPBCK_TST_EN & 0x1) << 18) +((gemroc_ROC_OVT_EN & 0x1) << 16) + ((gemroc_FEB_OVT_EN_pattern & 0xF) << 12) + ((gemroc_FEB_OVV_EN_pattern & 0xF) << 8) + ((gemroc_FEB_OVC_EN_pattern & 0xF) << 4) + (gemroc_FEB_PWR_EN_pattern & 0xF);
         gemroc_command_words[10] = ((gemroc_TIMING_DLY_FEB3 & 0x3F) << 24) + ((gemroc_TIMING_DLY_FEB2 & 0x3F) << 16) + ((gemroc_TIMING_DLY_FEB1 & 0x3F) << 8) + ((gemroc_TIMING_DLY_FEB0 & 0x3F) << 0);   }       
         gemroc_command_words[11] = ((gemroc_gemroc_cmd_code & 0xF) << 11);   }

void gemroc_extract_parameters_from_UDP_packet()
{
      printf ( "\n   OVT_LIMIT_FEB3 = %04X %d",((gemroc_command_words[1] >> 22) &  0xFF), ((gemroc_command_words[1] >> 22) &  0xFF));
      printf ( "\n D_OVV_LIMIT_FEB3 = %04X %d ",((gemroc_command_words[1] >> 13) &  0x1FF), ((gemroc_command_words[1] >> 13) &  0x1FF) );
      printf ( "\n D_OVC_LIMIT_FEB3 = %04X %d",((gemroc_command_words[1] >> 4) &  0x1FF), ((gemroc_command_words[1] >> 4) &  0x1FF) );
      printf ( "\n   OVT_LIMIT_FEB2 = %04X %d",((gemroc_command_words[2] >> 22) &  0xFF), ((gemroc_command_words[2] >> 22) &  0xFF) );
      printf ( "\n D_OVV_LIMIT_FEB2 = %04X %d ",((gemroc_command_words[2] >> 13) &  0x1FF), ((gemroc_command_words[2] >> 13) &  0x1FF) );
      printf ( "\n D_OVC_LIMIT_FEB2 = %04X %d",((gemroc_command_words[2] >> 4) &  0x1FF), ((gemroc_command_words[2] >> 4) &  0x1FF) );
      printf ( "\n   OVT_LIMIT_FEB1 = %04X %d", ((gemroc_command_words[3] >> 22) &  0xFF), ((gemroc_command_words[3] >> 22) &  0xFF) );
      printf ( "\n D_OVV_LIMIT_FEB1 = %04X %d ", ((gemroc_command_words[3] >> 13) &  0x1FF), ((gemroc_command_words[3] >> 13) &  0x1FF) );
      printf ( "\n D_OVC_LIMIT_FEB1 = %04X %d", ((gemroc_command_words[3] >> 4) &  0x1FF), ((gemroc_command_words[3] >> 4) &  0x1FF) );
      printf ( "\n   OVT_LIMIT_FEB0 = %04X %d",((gemroc_command_words[4] >> 22) &  0xFF), ((gemroc_command_words[4] >> 22) &  0xFF) );
      printf ( "\n D_OVV_LIMIT_FEB0 = %04X %d ", ((gemroc_command_words[4] >> 13) &  0x1FF), ((gemroc_command_words[4] >> 13) &  0x1FF) );
      printf ( "\n D_OVC_LIMIT_FEB0 = %04X %d", ((gemroc_command_words[4] >> 4) &  0x1FF), ((gemroc_command_words[4] >> 4) &  0x1FF) );
      printf ( "\n A_OVV_LIMIT_FEB3 = %04X %d ", ((gemroc_command_words[5] >> 13) &  0x1FF), ((gemroc_command_words[5] >> 13) &  0x1FF) );
      printf ( "\n A_OVC_LIMIT_FEB3 = %04X %d",((gemroc_command_words[5] >> 4) &  0x1FF), ((gemroc_command_words[5] >> 4) &  0x1FF) );
      printf ( "\n A_OVV_LIMIT_FEB2 = %04X %d ", ((gemroc_command_words[6] >> 13) &  0x1FF), ((gemroc_command_words[6] >> 13) &  0x1FF) );
      printf ( "\n A_OVC_LIMIT_FEB2 = %04X %d",((gemroc_command_words[6] >> 4) &  0x1FF), ((gemroc_command_words[6] >> 4) &  0x1FF) );
      printf ( "\n A_OVV_LIMIT_FEB1 = %04X %d ", ((gemroc_command_words[7] >> 13) &  0x1FF), ((gemroc_command_words[7] >> 13) &  0x1FF) );
      printf ( "\n A_OVC_LIMIT_FEB1 = %04X %d",((gemroc_command_words[7] >> 4) &  0x1FF), ((gemroc_command_words[7] >> 4) &  0x1FF) );
      printf ( "\n A_OVV_LIMIT_FEB0 = %04X %d ", ((gemroc_command_words[8] >> 13) &  0x1FF), ((gemroc_command_words[8] >> 13) &  0x1FF) );
      printf ( "\n A_OVC_LIMIT_FEB0 = %04X %d",((gemroc_command_words[8] >> 4) &  0x1FF), ((gemroc_command_words[8] >> 4) &  0x1FF) );
      printf ( "\n ROC_OVT_LIMIT = %04X %d",((gemroc_command_words[9] >> 24) &  0x3F), ((gemroc_command_words[9] >> 24) &  0x3F) );
      printf ( "\n XCVR_LPBCK_TST_EN = %d",(gemroc_command_words[9] >> 18) &  0x1);
      printf ( "\n ROC_OVT_EN = %d",(gemroc_command_words[9] >> 16) &  0x1);
      printf ( "\n FEB_OVT_EN_pattern = %01X %d",((gemroc_command_words[9] >> 12) &  0xF), ((gemroc_command_words[9] >> 12) &  0xF) );
      printf ( "\n FEB_OVV_EN_pattern = %01X %d",((gemroc_command_words[9] >> 8) &  0xF), ((gemroc_command_words[9] >> 8) &  0xF) );
      printf ( "\n FEB_OVC_EN_pattern = %01X %d", ((gemroc_command_words[9] >> 4) &  0xF), ((gemroc_command_words[9] >> 4) &  0xF) );
      printf ( "\n FEB_PWR_EN_pattern = %01X %d", ((gemroc_command_words[9] >> 0) &  0xF), ((gemroc_command_words[9] >> 0) &  0xF) );
      printf ( "\n TIMING_DLY_FEB3 = %02X %d", ((gemroc_command_words[10] >> 24) &  0x3F), ((gemroc_command_words[10] >> 24) &  0x3F) );
      printf ( "\n TIMING_DLY_FEB2 = %02X %d",((gemroc_command_words[10] >> 16) &  0x3F), ((gemroc_command_words[10] >> 16) &  0x3F) );
      printf ( "\n TIMING_DLY_FEB1 = %02X %d", ((gemroc_command_words[10] >>  8) &  0x3F), ((gemroc_command_words[10] >>  8) &  0x3F) );
      printf ( "\n TIMING_DLY_FEB0 = %02X %d",((gemroc_command_words[10] >>  0) &  0x3F), ((gemroc_command_words[10] >>  0) &  0x3F) ) ;}




























void ch_reg_init(unsigned int TARGET_GEMROC_ID_param,char* cfg_filename_param)
{
      unsigned int header_tag; 
      unsigned int Channel_cfg_cmd_ID; 
      unsigned int ccfg_cmd_tag; 
      unsigned int ccfg_cmd_word_count; 
      ch_reg_TARGET_GEMROC_ID = TARGET_GEMROC_ID_param ;
      ch_reg_cfg_filename = cfg_filename_param;
      readfromtxt(ch_reg_cfg_filename,ch_reg_parameter_array,30);
      write_array_to_txt("ch_reg_init",ch_reg_parameter_array,30);
      ch_reg_DisableHyst = ch_reg_parameter_array [0]  ;
      ch_reg_T2Hyst = ch_reg_parameter_array [1]  ;
      ch_reg_T1Hyst = ch_reg_parameter_array [2]  ;
      ch_reg_Ch63ObufMSB = ch_reg_parameter_array [3]  ;
      ch_reg_TP_disable_FE = ch_reg_parameter_array [4]  ;
      ch_reg_TDC_IB_E = ch_reg_parameter_array [5]  ;
      ch_reg_TDC_IB_T = ch_reg_parameter_array [6]  ;
      ch_reg_Integ = ch_reg_parameter_array [7]  ;
      ch_reg_PostAmpGain = ch_reg_parameter_array [8]  ;
      ch_reg_FE_delay = ch_reg_parameter_array [9]  ;
      ch_reg_Vth_T2 = ch_reg_parameter_array [10]  ;
      ch_reg_Vth_T1 = ch_reg_parameter_array [11]  ;
      ch_reg_QTx2Enable = ch_reg_parameter_array [12]  ;
      ch_reg_MaxIntegTime = ch_reg_parameter_array [13]  ;
      ch_reg_MinIntegTime = ch_reg_parameter_array [14]  ;
      ch_reg_TriggerBLatched = ch_reg_parameter_array [15]  ;
      ch_reg_QdcMode = ch_reg_parameter_array [16]  ;
      ch_reg_BranchEnableT = ch_reg_parameter_array [17]  ;
      ch_reg_BranchEnableEQ = ch_reg_parameter_array [18]  ;
      ch_reg_TriggerMode2B = ch_reg_parameter_array [19]  ;
      ch_reg_TriggerMode2Q = ch_reg_parameter_array [20]  ;
      ch_reg_TriggerMode2E = ch_reg_parameter_array [21]  ;
      ch_reg_TriggerMode2T = ch_reg_parameter_array [22]  ;
      ch_reg_TACMinAge = ch_reg_parameter_array [23]  ;
      ch_reg_TACMaxAge = ch_reg_parameter_array [24]  ;
      ch_reg_CounterMode = ch_reg_parameter_array [25]  ;
      ch_reg_DeadTime = ch_reg_parameter_array [26]  ;
      ch_reg_SyncChainLen = ch_reg_parameter_array [27]  ;
      ch_reg_Ch_DebugMode = ch_reg_parameter_array [28]  ;
      ch_reg_TriggerMode = ch_reg_parameter_array [29];
      ch_reg_is_a_write = 0x1;
      ch_reg_target_TIGER_ID = 0x1;
      ch_reg_command_code = 0x9;
      ch_reg_TO_ALL_enable = 0x1;
      ch_reg_channel_ID = 0x3;
      header_tag = 0x8 << 28;
      Channel_cfg_cmd_ID = 0xE ;
      ccfg_cmd_tag = Channel_cfg_cmd_ID << 24;
      ccfg_cmd_word_count = 9 ;
      ch_reg_command_words[0] = header_tag + (ch_reg_TARGET_GEMROC_ID << 16) + ccfg_cmd_tag + ccfg_cmd_word_count;
      ch_reg_command_words[1] = ((ch_reg_DisableHyst & 0x1) << 24) + ((ch_reg_T2Hyst & 0x7) << 16) + ((ch_reg_T1Hyst & 0x7) << 8) + ((ch_reg_Ch63ObufMSB & 0x1));
      ch_reg_command_words[2] = ((ch_reg_TP_disable_FE & 0x1) << 24) + ((ch_reg_TDC_IB_E & 0xF) << 16) + ((ch_reg_TDC_IB_T & 0xF) << 8) + ((ch_reg_Integ & 0x1));
      ch_reg_command_words[3] = ((ch_reg_PostAmpGain & 0x3) << 24) + ((ch_reg_FE_delay & 0x1F) << 16) + ((ch_reg_Vth_T2 & 0x3F) << 8) + ((ch_reg_Vth_T1 & 0x3F));
      ch_reg_command_words[4] = ((ch_reg_QTx2Enable & 0x1) << 24) + ((ch_reg_MaxIntegTime & 0x7F) << 16) + ((ch_reg_MinIntegTime & 0x7F) << 8) + ((ch_reg_TriggerBLatched & 0x1));
      ch_reg_command_words[5] = ((ch_reg_QdcMode & 0x1) << 24) + ((ch_reg_BranchEnableT & 0x1) << 16) + ((ch_reg_BranchEnableEQ & 0x1) << 8) + ((ch_reg_TriggerMode2B & 0x7));
      ch_reg_command_words[6] = ((ch_reg_TriggerMode2Q & 0x3) << 24) + ((ch_reg_TriggerMode2E & 0x7) << 16) + ((ch_reg_TriggerMode2T & 0x3) << 8) + ((ch_reg_TACMinAge & 0x1F));
      ch_reg_command_words[7] = ((ch_reg_TACMaxAge & 0x1F) << 24) + ((ch_reg_CounterMode & 0xF) << 16) + ((ch_reg_DeadTime & 0x3F) << 8) + ((ch_reg_SyncChainLen & 0x3));
      ch_reg_command_words[8] = ((ch_reg_Ch_DebugMode & 0x3) << 24) + ((ch_reg_TriggerMode & 0x3) << 16);
      ch_reg_command_words[9] = ((ch_reg_command_code & 0xF) << 11) + ((ch_reg_target_TIGER_ID & 0x7) << 8) + ((ch_reg_is_a_write & 0x1) << 7) + ((ch_reg_TO_ALL_enable & 0x1) << 6) + (ch_reg_channel_ID & 0x3F);   }


void ch_reg_reload_chcfg_settings_from_file(char* ChCFGReg_def_fname_param)
{
      readfromtxt(ChCFGReg_def_fname_param,ch_reg_parameter_array,30);
      write_array_to_txt("ch_reg reload",ch_reg_parameter_array,30);
      ch_reg_DisableHyst = ch_reg_parameter_array [0]  ;
      ch_reg_T2Hyst = ch_reg_parameter_array [1]  ;
      ch_reg_T1Hyst = ch_reg_parameter_array [2]  ;
      ch_reg_Ch63ObufMSB = ch_reg_parameter_array [3]  ;
      ch_reg_TP_disable_FE = ch_reg_parameter_array [4]  ;
      ch_reg_TDC_IB_E = ch_reg_parameter_array [5]  ;
      printf("ch_reg_TDC_IB_E = ch_reg_parameter_array [5]=%d\n",ch_reg_TDC_IB_E);
      ch_reg_TDC_IB_T = ch_reg_parameter_array [6]  ;
      ch_reg_Integ = ch_reg_parameter_array [7]  ;
      ch_reg_PostAmpGain = ch_reg_parameter_array [8]  ;
      ch_reg_FE_delay = ch_reg_parameter_array [9]  ;
      ch_reg_Vth_T2 = ch_reg_parameter_array [10]  ;
      ch_reg_Vth_T1 = ch_reg_parameter_array [11]  ;
      ch_reg_QTx2Enable = ch_reg_parameter_array [12]  ;
      ch_reg_MaxIntegTime = ch_reg_parameter_array [13]  ;
      ch_reg_MinIntegTime = ch_reg_parameter_array [14]  ;
      ch_reg_TriggerBLatched = ch_reg_parameter_array [15]  ;
      ch_reg_QdcMode = ch_reg_parameter_array [16]  ;
      ch_reg_BranchEnableT = ch_reg_parameter_array [17]  ;
      ch_reg_BranchEnableEQ = ch_reg_parameter_array [18]  ;
      ch_reg_TriggerMode2B = ch_reg_parameter_array [19]  ;
      ch_reg_TriggerMode2Q = ch_reg_parameter_array [20]  ;
      ch_reg_TriggerMode2E = ch_reg_parameter_array [21]  ;
      ch_reg_TriggerMode2T = ch_reg_parameter_array [22]  ;
      ch_reg_TACMinAge = ch_reg_parameter_array [23]  ;
      ch_reg_TACMaxAge = ch_reg_parameter_array [24]  ;
      ch_reg_CounterMode = ch_reg_parameter_array [25]  ;
      ch_reg_DeadTime = ch_reg_parameter_array [26]  ;
      ch_reg_SyncChainLen = ch_reg_parameter_array [27]  ;
      ch_reg_Ch_DebugMode = ch_reg_parameter_array [28]  ;
      ch_reg_TriggerMode = ch_reg_parameter_array [29];
}

  
   unsigned int ch_reg_command_words_array_size()
{
      return 10;  }

void ch_reg_print_command_words()
{    int i;
     for(i=0;i<10;i++)
      printf("%08X\n", ch_reg_command_words[i]);  }

void ch_reg_set_target_GEMROC(unsigned int GEMROC_ID_param)
{
      ch_reg_TARGET_GEMROC_ID = GEMROC_ID_param & 0x1f;
      ch_reg_command_words[0] = (ch_reg_command_words[0] & 0xFF00FFFF) + (ch_reg_TARGET_GEMROC_ID << 16);  }


void ch_reg_set_target_TIGER(unsigned int target_TIGER_param)
{
      ch_reg_target_TIGER_ID = target_TIGER_param & 0x7;
      ch_reg_command_words[9] = ((ch_reg_command_code & 0xF) << 12) + ((ch_reg_target_TIGER_ID & 0x7) << 8) + ((ch_reg_is_a_write & 0x1) << 7) + ((ch_reg_TO_ALL_enable & 0x1) << 6) + (ch_reg_channel_ID & 0x1F);  }

void ch_reg_set_target_channel(unsigned int target_channel_param)
{
      ch_reg_channel_ID = target_channel_param & 0x3F;
      ch_reg_command_words[9] = ((ch_reg_command_code & 0xF) << 12) + ((ch_reg_target_TIGER_ID & 0x7) << 8) + ((ch_reg_is_a_write & 0x1) << 7) + ((ch_reg_TO_ALL_enable & 0x1) << 6) + (ch_reg_channel_ID & 0x3F);   }

void ch_reg_set_Vth_T1(unsigned int Vth_T1_param)
{
      ch_reg_Vth_T1 = Vth_T1_param & 0x3F;  }


void ch_reg_set_to_ALL_param(unsigned int to_ALL_param)
{
      ch_reg_TO_ALL_enable = to_ALL_param & 0x1;
      ch_reg_command_words[9] = ((ch_reg_command_code & 0xF) << 12) + ((ch_reg_target_TIGER_ID & 0x7) << 8) + ((ch_reg_is_a_write & 0x1) << 7) + ((ch_reg_TO_ALL_enable & 0x1) << 6) + (ch_reg_channel_ID & 0x1F);   }


  
void ch_reg_set_command_code(char*command_code_param)
{
      if (strcmp(command_code_param,"WR")==0){
         ch_reg_command_code = 0x0;
         ch_reg_is_a_write = 1;
         ch_reg_command_words[9] = ((ch_reg_command_code & 0xF) << 12) + ((ch_reg_target_TIGER_ID & 0x7) << 8) + ((ch_reg_is_a_write & 0x1) << 7) + ((ch_reg_TO_ALL_enable & 0x1) << 6) + (ch_reg_channel_ID & 0x1F); }
      else if (strcmp(command_code_param,"RD")==0){
         ch_reg_command_code = 0x1;
         ch_reg_is_a_write = 0;
         ch_reg_command_words[9] = ((ch_reg_command_code & 0xF) << 12) + ((ch_reg_target_TIGER_ID & 0x7) << 8) + ((ch_reg_is_a_write & 0x1) << 7) + ((ch_reg_TO_ALL_enable & 0x1) << 6) + (ch_reg_channel_ID & 0x1F);  }
      else 
         printf("ch_reg:bad command_code parameter passed");
   }


void ch_reg_set_TP_disable_FE(unsigned int TP_disable_FE_param)
{
      ch_reg_TP_disable_FE = TP_disable_FE_param & 0x1;}


void ch_reg_update_command_words()
{     int i;
      if ( (ch_reg_command_code & 0xF) == 0x1 )
       for(i=1;i<9;i++)
         ch_reg_command_words[i]=0;
      else{
         ch_reg_command_words[1] = ((ch_reg_DisableHyst & 0x1) << 24)   + ((ch_reg_T2Hyst & 0x7) << 16)        + ((ch_reg_T1Hyst & 0x7) << 8)         + ((ch_reg_Ch63ObufMSB & 0x1));
         ch_reg_command_words[2] = ((ch_reg_TP_disable_FE & 0x1) << 24) + ((ch_reg_TDC_IB_E & 0xF) << 16)      + ((ch_reg_TDC_IB_T & 0xF) << 8)       + ((ch_reg_Integ & 0x1));
         ch_reg_command_words[3] = ((ch_reg_PostAmpGain & 0x3) << 24)   + ((ch_reg_FE_delay & 0x1F) << 16)     + ((ch_reg_Vth_T2 & 0x3F) << 8)        + ((ch_reg_Vth_T1 & 0x3F));
         ch_reg_command_words[4] = ((ch_reg_QTx2Enable & 0x1) << 24)    + ((ch_reg_MaxIntegTime & 0x7F) << 16) + ((ch_reg_MinIntegTime & 0x7F) << 8)  + ((ch_reg_TriggerBLatched & 0x1));
         ch_reg_command_words[5] = ((ch_reg_QdcMode & 0x1) << 24)       + ((ch_reg_BranchEnableT & 0x1) << 16) + ((ch_reg_BranchEnableEQ & 0x1) << 8) + ((ch_reg_TriggerMode2B & 0x7));
         ch_reg_command_words[6] = ((ch_reg_TriggerMode2Q & 0x3) << 24) + ((ch_reg_TriggerMode2E & 0x7) << 16) + ((ch_reg_TriggerMode2T & 0x3) << 8)  + ((ch_reg_TACMinAge & 0x1F));
         ch_reg_command_words[7] = ((ch_reg_TACMaxAge & 0x1F) << 24)    + ((ch_reg_CounterMode & 0xF) << 16)   + ((ch_reg_DeadTime & 0x3F) << 8)      + ((ch_reg_SyncChainLen & 0x3));
         ch_reg_command_words[8] = ((ch_reg_Ch_DebugMode & 0x3) << 24)  + ((ch_reg_TriggerMode & 0x3) << 16);  }
      ch_reg_command_words[9] = ((ch_reg_command_code & 0xF) << command_code_shift) + ((ch_reg_target_TIGER_ID & 0x7) << target_TIGER_ID_shift) + ((ch_reg_TO_ALL_enable & 0x1) << 6) + (ch_reg_channel_ID & 0x3F);    }


void ch_reg_extract_parameters_from_UDP_packet()
{
      printf("\nList of parameters sent to TIGER%d:",ch_reg_target_TIGER_ID);
      printf("\ncmd_word8: DisableHyst:%02X;\t\tT2Hyst:%02X;\t\tT1Hyst:%02X;\t\tCh63ObufMSB:%02X;", ((ch_reg_command_words[1]>>24)&0x1), ((ch_reg_command_words[1]>>16)&0x7),  ((ch_reg_command_words[1]>>8)&0x7),  ((ch_reg_command_words[1]>>0)&0x1)  ) ;
      printf ("\ncmd_word7: TP_disable_FE:%02X;\t\tTDC_IB_E:%02X;\t\tTDC_IB_T:%02X;\t\tInteg:%02X;",((ch_reg_command_words[2]>>24)&0x1), ((ch_reg_command_words[2]>>16)&0xF),  ((ch_reg_command_words[2]>>8)&0xF),  ((ch_reg_command_words[2]>>0)&0x1)  ) ;
      printf ("\ncmd_word6: PostAmpGain:%02X;\t\tFE_delay:%02X;\t\tVth_T2:%02X;\t\tVth_T1:%02X;",((ch_reg_command_words[3]>>24)&0x3), ((ch_reg_command_words[3]>>16)&0x1F), ((ch_reg_command_words[3]>>8)&0x3F), ((ch_reg_command_words[3]>>0)&0x3F) ) ;
      printf("\ncmd_word5: QTx2Enable:%02X;\t\tMaxIntegTime:%02X;\t\tMinIntegTime:%02X;\t\tTriggerBLatched:%02X;",((ch_reg_command_words[4]>>24)&0x1), ((ch_reg_command_words[4]>>16)&0x7F), ((ch_reg_command_words[4]>>8)&0x7F), ((ch_reg_command_words[4]>>0)&0x1)  ) ;
      printf ("\ncmd_word4: QdcMode:%02X;\t\tBranchEnableT:%02X;\t\tBranchEnableEQ:%02X;\t\tTriggerMode2B:%02X;",((ch_reg_command_words[5]>>24)&0x1), ((ch_reg_command_words[5]>>16)&0x1),  ((ch_reg_command_words[5]>>8)&0x1),  ((ch_reg_command_words[5]>>0)&0x7)  );
      printf ("\ncmd_word3: TriggerMode2Q:%02X;\t\tTriggerMode2E:%02X;\t\tTriggerMode2T:%02X;\t\tTACMinAge:%02X;",((ch_reg_command_words[6]>>24)&0x3), ((ch_reg_command_words[6]>>16)&0x7),  ((ch_reg_command_words[6]>>8)&0x3),  ((ch_reg_command_words[6]>>0)&0x1F) );
      printf("\ncmd_word2: TACMaxAge:%02X;\t\tCounterMode:%02X;\t\tDeadTime:%02X;\t\tSyncChainLen:%02X;",((ch_reg_command_words[7]>>24)&0x1F),((ch_reg_command_words[7]>>16)&0xF),  ((ch_reg_command_words[7]>>8)&0x3F), ((ch_reg_command_words[7]>>0)&0x3)  );
      printf("\ncmd_word1: Ch_DebugMode:%02X;\t\tTriggerMode:%02X;",((ch_reg_command_words[8]>>24)&0x3), ((ch_reg_command_words[8]>>16)&0x3));
      printf("\ncmd_word0: command_code:%02X;\t\ttarget_TIGER_ID:%02X;\t\tTO_ALL_enable:%02X;\t\tchannel_ID:%02X;",((ch_reg_command_words[9]>>command_code_shift)&0xF), ((ch_reg_command_words[9]>>target_TIGER_ID_shift)&0x7),  ((ch_reg_command_words[9]>>6)&0x1), ((ch_reg_command_words[9]>>0)&0x3F)  );  }















void g_reg_init(int TARGET_GEMROC_ID_param, char*cfg_filename_param)
{
     unsigned int  header_tag;
     unsigned int  Global_cfg_cmd_ID;
     unsigned int  gcfg_cmd_tag;
     unsigned int  gcfg_cmd_word_count;
      g_reg_TARGET_GEMROC_ID = TARGET_GEMROC_ID_param;
      g_reg_cfg_filename = cfg_filename_param;
      readfromtxt(g_reg_cfg_filename,g_reg_parameter_array,37);
      write_array_to_txt("g_reg_init",g_reg_parameter_array,37);
      g_reg_BufferBias = g_reg_parameter_array [0];
      g_reg_TDCVcasN        = swap_order_N_bits(g_reg_parameter_array [1],4);
      g_reg_TDCVcasP        = swap_order_N_bits(g_reg_parameter_array [2],5);
      g_reg_TDCVcasPHyst    = swap_order_N_bits(g_reg_parameter_array [3],6);
      g_reg_DiscFE_Ibias    = swap_order_N_bits(g_reg_parameter_array [4],6);

      g_reg_BiasFE_PpreN = g_reg_parameter_array [5];
      g_reg_AVcasp_global   = swap_order_N_bits(g_reg_parameter_array [6],5);
      g_reg_TDCcompVcas     = swap_order_N_bits(g_reg_parameter_array [7],4);
      g_reg_TDCIref_cs      = swap_order_N_bits(g_reg_parameter_array [8],5);
      g_reg_DiscVcasN       = swap_order_N_bits(g_reg_parameter_array [9],4);
      g_reg_IntegVb1        = swap_order_N_bits(g_reg_parameter_array [10],6);
      g_reg_BiasFE_A1 = g_reg_parameter_array [11];
      g_reg_Vcasp_Vth       = swap_order_N_bits(g_reg_parameter_array [12],6);
      g_reg_TAC_I_LSB = g_reg_parameter_array [13];
      g_reg_TDCcompVbias = g_reg_parameter_array [14] ;

      g_reg_Vref_Integ      = swap_order_N_bits(g_reg_parameter_array [15],6) ;
      g_reg_IBiasTPcal      = swap_order_N_bits(g_reg_parameter_array [16],5) ;
      g_reg_TP_Vcal         = swap_order_N_bits(g_reg_parameter_array [17],5) ;
      g_reg_ShaperIbias = g_reg_parameter_array [18] ;
      g_reg_IPostamp        = swap_order_N_bits(g_reg_parameter_array [19],5) ;

      g_reg_TP_Vcal_ref     = swap_order_N_bits(g_reg_parameter_array [20],5) ;
      g_reg_Vref_integ_diff = swap_order_N_bits(g_reg_parameter_array [21],6) ;
      g_reg_Sig_pol = g_reg_parameter_array [22] ;
      g_reg_FE_TPEnable = g_reg_parameter_array [23];
      g_reg_DataClkDiv = g_reg_parameter_array [24] ;
      g_reg_TACrefreshPeriod = g_reg_parameter_array [25] ;
      g_reg_TACrefreshEnable = g_reg_parameter_array [26] ;
      g_reg_CounterPeriod = g_reg_parameter_array [27] ; 
      g_reg_CounterEnable = g_reg_parameter_array [28] ;

      g_reg_StopRampEnable = g_reg_parameter_array [29] ;
      g_reg_RClkEnable = g_reg_parameter_array [30] ;
      g_reg_TDCClkdiv = g_reg_parameter_array [31] ;
      g_reg_VetoMode = g_reg_parameter_array [32] ;
      g_reg_Ch_DebugMode = g_reg_parameter_array [33];
      g_reg_TxMode = g_reg_parameter_array [34] ;
      g_reg_TxDDR = g_reg_parameter_array [35] ; 
      g_reg_TxLinks = g_reg_parameter_array [36];
      g_reg_is_a_write = 0x1;
      g_reg_target_TIGER_ID = 0x1;
      g_reg_command_code = 0x9;
      header_tag = 0x8 << 28;
      Global_cfg_cmd_ID = 0xF;
      gcfg_cmd_tag = Global_cfg_cmd_ID << 24;
      gcfg_cmd_word_count = 11;
      g_reg_command_words[0] = header_tag + (g_reg_TARGET_GEMROC_ID << 16) + gcfg_cmd_tag + gcfg_cmd_word_count;
      g_reg_command_words[1] = ((g_reg_BufferBias & 0x3) << 24) + ((g_reg_TDCVcasN & 0xF) << 16) + ((g_reg_TDCVcasP & 0x1F) << 8) + ((g_reg_TDCVcasPHyst & 0x3F));
      g_reg_command_words[2] = ((g_reg_DiscFE_Ibias & 0x3f) << 24) + ((g_reg_BiasFE_PpreN & 0x3F) << 16) + ((g_reg_AVcasp_global & 0x1F) << 8) + ((g_reg_TDCcompVcas & 0xF));
      g_reg_command_words[3] = ((g_reg_TDCIref_cs & 0x1f) << 24) + ((g_reg_DiscVcasN & 0xF) << 16) + ((g_reg_IntegVb1 & 0x3F) << 8) + ((g_reg_BiasFE_A1 & 0xF));
      g_reg_command_words[4] = ((g_reg_Vcasp_Vth & 0x3f) << 24) + ((g_reg_TAC_I_LSB & 0x1F) << 16) + ((g_reg_TDCcompVbias & 0x1F) << 8) + ((g_reg_Vref_Integ & 0x3F));
      g_reg_command_words[5] = ((g_reg_IBiasTPcal & 0x1f) << 24) + ((g_reg_TP_Vcal & 0x1F) << 16) + ((g_reg_ShaperIbias & 0xF) << 8) + ((g_reg_IPostamp & 0x1F));
      g_reg_command_words[6] = ((g_reg_TP_Vcal_ref & 0x1f) << 24) + ((g_reg_Vref_integ_diff & 0x3F) << 16) + ((g_reg_Sig_pol & 0x1) << 8) + ((g_reg_FE_TPEnable & 0x1));
      g_reg_command_words[7] = ((g_reg_DataClkDiv & 0x3) << 16) + ((g_reg_TACrefreshPeriod & 0xf) << 8) + ((g_reg_TACrefreshEnable & 0x1));
      g_reg_command_words[8] = ((g_reg_CounterPeriod & 0x7) << 24) + ((g_reg_CounterEnable & 0x1) << 16) + ((g_reg_StopRampEnable & 0x3) << 8) + ((g_reg_RClkEnable & 0x1F));
      g_reg_command_words[9] = ((g_reg_TDCClkdiv & 0x1) << 24) + ((g_reg_VetoMode & 0x3F) << 16) + ((g_reg_Ch_DebugMode & 0x1) << 8) + ((g_reg_TxMode & 0x3)) ;
      g_reg_command_words[10] = ((g_reg_TxDDR & 0x1) << 24) + ((g_reg_TxLinks & 0x3) << 16);
      g_reg_command_words[11] = ((g_reg_command_code & 0xF) << command_code_shift) + ((g_reg_target_TIGER_ID & 0x7) << target_TIGER_ID_shift);
}

void g_reg_reload_gcfg_settings_from_file(char* GCFGReg_def_fname_param)
{
      readfromtxt(GCFGReg_def_fname_param,g_reg_parameter_array,37);
      write_array_to_txt("g_reg reload",g_reg_parameter_array,37);
      g_reg_BufferBias = g_reg_parameter_array [0] ;
      g_reg_TDCVcasN        = swap_order_N_bits(g_reg_parameter_array [1],4)  ;
      g_reg_TDCVcasP        = swap_order_N_bits(g_reg_parameter_array [2],5)  ;
      g_reg_TDCVcasPHyst    = swap_order_N_bits(g_reg_parameter_array [3],6)  ;
      g_reg_DiscFE_Ibias    = swap_order_N_bits(g_reg_parameter_array [4],6)  ;
      g_reg_BiasFE_PpreN    = g_reg_parameter_array [5] ;
      g_reg_AVcasp_global   = swap_order_N_bits(g_reg_parameter_array [6],5)  ;
      g_reg_TDCcompVcas     = swap_order_N_bits(g_reg_parameter_array [7],4)  ;
      g_reg_TDCIref_cs      = swap_order_N_bits(g_reg_parameter_array [8],5)  ;
      g_reg_DiscVcasN       = swap_order_N_bits(g_reg_parameter_array [9],4)  ;
      g_reg_IntegVb1        = swap_order_N_bits(g_reg_parameter_array [10],6) ;
      g_reg_BiasFE_A1 = g_reg_parameter_array [11] ;
      g_reg_Vcasp_Vth       = swap_order_N_bits(g_reg_parameter_array [12],6) ;
      g_reg_TAC_I_LSB = g_reg_parameter_array [13] ;
      g_reg_TDCcompVbias = g_reg_parameter_array [14] ;
      g_reg_Vref_Integ      = swap_order_N_bits(g_reg_parameter_array [15],6) ;
      g_reg_IBiasTPcal      = swap_order_N_bits(g_reg_parameter_array [16],5) ;
      g_reg_TP_Vcal         = swap_order_N_bits(g_reg_parameter_array [17],5) ;
      g_reg_ShaperIbias = g_reg_parameter_array [18] ;
      g_reg_IPostamp        = swap_order_N_bits(g_reg_parameter_array [19],5) ;
      g_reg_TP_Vcal_ref     = swap_order_N_bits(g_reg_parameter_array [20],5) ;
      g_reg_Vref_integ_diff = swap_order_N_bits(g_reg_parameter_array [21],6) ;
      g_reg_Sig_pol = g_reg_parameter_array [22] ;
      g_reg_FE_TPEnable = g_reg_parameter_array [23] ;
      g_reg_DataClkDiv = g_reg_parameter_array [24] ;
      g_reg_TACrefreshPeriod = g_reg_parameter_array [25] ;
      g_reg_TACrefreshEnable = g_reg_parameter_array [26] ;
      g_reg_CounterPeriod = g_reg_parameter_array [27] ;
      g_reg_CounterEnable = g_reg_parameter_array [28] ;
      g_reg_StopRampEnable = g_reg_parameter_array [29] ;
      g_reg_RClkEnable = g_reg_parameter_array [30] ;
      g_reg_TDCClkdiv = g_reg_parameter_array [31] ;
      g_reg_VetoMode = g_reg_parameter_array [32] ;
      g_reg_Ch_DebugMode = g_reg_parameter_array [33] ;
      g_reg_TxMode = g_reg_parameter_array [34] ;
      g_reg_TxDDR = g_reg_parameter_array [35] ;
      g_reg_TxLinks = g_reg_parameter_array [36];    }

unsigned int g_reg_command_words_array_size()
{
  return 12;  }

void g_reg_print_command_words()
{   unsigned int i;
    for(i=0; i<12;i++)
        printf("%08X\n",g_reg_command_words[i]);   }

void g_reg_set_target_GEMROC(unsigned int GEMROC_ID_param)
{
      g_reg_TARGET_GEMROC_ID = GEMROC_ID_param & 0x1f;
      g_reg_command_words[0] = (g_reg_command_words[0] & 0xFF00FFFF) + (g_reg_TARGET_GEMROC_ID << 16);   }

void g_reg_set_target_TIGER(unsigned int target_TIGER_param)
{
      g_reg_target_TIGER_ID = target_TIGER_param & 0x7;
      g_reg_command_words[11] = ((g_reg_command_code & 0xF) << command_code_shift) + ((g_reg_target_TIGER_ID & 0x7) << target_TIGER_ID_shift);
           }

void g_reg_set_FE_TPEnable(FE_TPEnable_param)
{
  g_reg_FE_TPEnable = FE_TPEnable_param & 0x1;  }


void g_reg_set_AVcasp_global(unsigned int AVcasp_global_param)
{
      g_reg_AVcasp_global = AVcasp_global_param & 0x1F;  }


void g_reg_set_command_code(char* command_code_param)
{
      if (strcmp(command_code_param,"WR")==0){
         g_reg_command_code = 0x8;
         g_reg_is_a_write = 1;
         g_reg_command_words[11] = ((g_reg_command_code & 0xF) << command_code_shift) + ((g_reg_target_TIGER_ID & 0x7) << target_TIGER_ID_shift);    }
      else if(strcmp(command_code_param,"RD")==0){
         g_reg_command_code = 0x9;
         g_reg_is_a_write = 0;
         g_reg_command_words[11] = ((g_reg_command_code & 0xF) << command_code_shift) + ((g_reg_target_TIGER_ID & 0x7) << target_TIGER_ID_shift);  }
      else
         printf("bad command_code parameter passed\n");   }


void g_reg_update_command_words()
{     int i;
      if ( (g_reg_command_code & 0xF) == 0x9 )
        for(i=1;i<11;i++)
          g_reg_command_words[i]=0;
      else {
         g_reg_command_words[1] = ((g_reg_BufferBias & 0x3) << 24)+ ((g_reg_TDCVcasN & 0xF) << 16) + ((g_reg_TDCVcasP & 0x1F) << 8) + ((g_reg_TDCVcasPHyst & 0x3F));
         g_reg_command_words[2] =((g_reg_DiscFE_Ibias & 0x3f) << 24) + ((g_reg_BiasFE_PpreN & 0x3F) << 16) + ((g_reg_AVcasp_global & 0x1F) << 8)   + ((g_reg_TDCcompVcas & 0xF));
         g_reg_command_words[3] =((g_reg_TDCIref_cs & 0x1f) << 24) + ((g_reg_DiscVcasN & 0xF) << 16) + ((g_reg_IntegVb1 & 0x3F) << 8)  + ((g_reg_BiasFE_A1 & 0xF));
         g_reg_command_words[4] =  ((g_reg_Vcasp_Vth & 0x3f) << 24) + ((g_reg_TAC_I_LSB & 0x1F) << 16) + ((g_reg_TDCcompVbias & 0x1F) << 8) + ((g_reg_Vref_Integ & 0x3F));
         g_reg_command_words[5] =  ((g_reg_IBiasTPcal & 0x1f) << 24) + ((g_reg_TP_Vcal & 0x1F) << 16) + ((g_reg_ShaperIbias & 0xF) << 8)      + ((g_reg_IPostamp & 0x1F));
         g_reg_command_words[6] =  ((g_reg_TP_Vcal_ref & 0x1f) << 24) + ((g_reg_Vref_integ_diff & 0x3F) << 16) + ((g_reg_Sig_pol & 0x1) << 8)          + ((g_reg_FE_TPEnable & 0x1));
         g_reg_command_words[7] = ((g_reg_DataClkDiv & 0x3) << 16) + ((g_reg_TACrefreshPeriod & 0xf) << 8) + ((g_reg_TACrefreshEnable & 0x1));
         g_reg_command_words[8] = ((g_reg_CounterPeriod & 0x7) << 24) + ((g_reg_CounterEnable & 0x1) << 16) + ((g_reg_StopRampEnable & 0x3) << 8)   + ((g_reg_RClkEnable & 0x1F));
         g_reg_command_words[9] =  ((g_reg_TDCClkdiv & 0x1) << 24) + ((g_reg_VetoMode & 0x3F) << 16) + ((g_reg_Ch_DebugMode & 0x1) << 8) + ((g_reg_TxMode & 0x3));
         g_reg_command_words[10] =  ((g_reg_TxDDR & 0x1) << 24) + ((g_reg_TxLinks & 0x3) << 16); }
      g_reg_command_words[11] = ((g_reg_command_code & 0xF) << command_code_shift) + ((g_reg_target_TIGER_ID & 0x7) << target_TIGER_ID_shift);     
}



void g_reg_extract_parameters_from_UDP_packet()
{
      printf("\nList of parameters sent to TIGER%d:",g_reg_target_TIGER_ID);
      printf("\nBufferBias:%02X;\t\tTDCVcasN:%02X;\t\tTDCVcasP:%02X;\t\tTDCVcasPHyst:%02X;",((g_reg_command_words[1]>>24)&0x3), swap_order_N_bits(((g_reg_command_words[1]>>16)&0xF),4), swap_order_N_bits(((g_reg_command_words[1]>>8)&0x1F),5), swap_order_N_bits(((g_reg_command_words[1]>>0)&0x3F),6) );
      printf("\nDiscFE_Ibias:%02X;\tBiasFE_PpreN:%02X;\tAVcasp_global:%02X;\tTDCcompVcas:%02X;",swap_order_N_bits(((g_reg_command_words[2]>>24)&0x3F),6), ((g_reg_command_words[2]>>16)&0x3F), swap_order_N_bits(((g_reg_command_words[2]>>8)&0x1F),5), swap_order_N_bits(((g_reg_command_words[2]>>0)&0xF),4) );
      printf( "\nTDCIref_cs:%02X;\t\tDiscVcasN:%02X;\t\tIntegVb1:%02X;\t\tBiasFE_A1:%02X;", swap_order_N_bits(((g_reg_command_words[3]>>24)&0x1F),5), swap_order_N_bits(((g_reg_command_words[3]>>16)&0xF),4), swap_order_N_bits(((g_reg_command_words[3]>>8)&0x3F),6), ((g_reg_command_words[3]>>0)&0xF) ); 
      printf( "\nVcasp_Vth:%02X;\t\tTAC_I_LSB:%02X;\t\tTDCcompVbias:%02X;\tVref_Integ:%02X;", swap_order_N_bits(((g_reg_command_words[4]>>24)&0x3F),6), ((g_reg_command_words[4]>>16)&0x1F), ((g_reg_command_words[4]>>8)&0x1F), swap_order_N_bits(((g_reg_command_words[4]>>0)&0x3F),6) ) ;
      printf ( "\nIBiasTPcal:%02X;\t\tTP_Vcal:%02X;\t\tShaperIbias:%02X;\t\tIPostamp:%02X;", swap_order_N_bits(((g_reg_command_words[5]>>24)&0x1F),5), swap_order_N_bits(((g_reg_command_words[5]>>16)&0x1F),5), ((g_reg_command_words[5]>>8)&0xF), swap_order_N_bits(((g_reg_command_words[5]>>0)&0x1F),5) ) ;
      printf( "\nTP_Vcal_ref:%02X;\t\tVref_integ_diff:%02X;\tSig_pol:%02X;\t\tFE_TPEnable:%02X;" , swap_order_N_bits(((g_reg_command_words[6]>>24)&0x1F),5), swap_order_N_bits(((g_reg_command_words[6]>>16)&0x3F),6), ((g_reg_command_words[6]>>8)&0x1), ((g_reg_command_words[6]>>0)&0x1) ) ;
      printf ( "\nDataClkDiv:%02X;\t\tTACrefreshPeriod:%02X;\tTACrefreshEnable:%02X;", ((g_reg_command_words[7]>>16)&0x3), ((g_reg_command_words[7]>>8)&0xF), ((g_reg_command_words[7]>>0)&0x1) ) ;
      printf ( "\nCounterPeriod:%02X;\tCounterEnable:%02X;\tStopRampEnable:%02X;\tRClkEnable:%02X;" , ((g_reg_command_words[8]>>24)&0x7), ((g_reg_command_words[8]>>16)&0x1), ((g_reg_command_words[8]>>8)&0x3), ((g_reg_command_words[8]>>0)&0x1F) );
      printf( "\nTDCClkdiv:%02X;\t\tVetoMode:%02X;\t\tCh_DebugMode:%02X;\tTxMode:%02X;" , ((g_reg_command_words[9]>>24)&0x1), ((g_reg_command_words[9]>>16)&0x3F), ((g_reg_command_words[9]>>8)&0x1 ), ((g_reg_command_words[9]>>0)&0x3) ) ;
      printf ( "\nTxDDR:%02X;\t\tTxLinks:%02X;" , ((g_reg_command_words[10]>>24)&0x1), ((g_reg_command_words[10]>>16)&0x3) ) ; }


void g_reg_extract_d_parameters_from_UDP_packet()
{
      printf("\nList of parameters sent to TIGER%d:",g_reg_target_TIGER_ID);
      printf( "\nBufferBias:%d;\t\tTDCVcasN:%d;\t\tTDCVcasP:%d;\t\tTDCVcasPHyst:%d;", ((g_reg_command_words[1]>>24)&0x3), swap_order_N_bits(((g_reg_command_words[1]>>16)&0xF),4), swap_order_N_bits(((g_reg_command_words[1]>>8)&0x1F),5), swap_order_N_bits(((g_reg_command_words[1]>>0)&0x3F),6) ) ;

     printf( "\nDiscFE_Ibias:%d;\tBiasFE_PpreN:%d;\tAVcasp_global:%d;\tTDCcompVcas:%d;", swap_order_N_bits(((g_reg_command_words[2]>>24)&0x3F),6), ((g_reg_command_words[2]>>16)&0x3F), swap_order_N_bits(((g_reg_command_words[2]>>8)&0x1F),5), swap_order_N_bits(((g_reg_command_words[2]>>0)&0xF),4) );
      printf( "\nTDCIref_cs:%d;\t\tDiscVcasN:%d;\t\tIntegVb1:%d;\t\tBiasFE_A1:%d;", swap_order_N_bits(((g_reg_command_words[3]>>24)&0x1F),5), swap_order_N_bits(((g_reg_command_words[3]>>16)&0xF),4), swap_order_N_bits(((g_reg_command_words[3]>>8)&0x3F),6), ((g_reg_command_words[3]>>0)&0xF) );
      printf ( "\nVcasp_Vth:%d;\t\tTAC_I_LSB:%d;\t\tTDCcompVbias:%d;\tVref_Integ:%d;", swap_order_N_bits(((g_reg_command_words[4]>>24)&0x3F),6), ((g_reg_command_words[4]>>16)&0x1F), ((g_reg_command_words[4]>>8)&0x1F), swap_order_N_bits(((g_reg_command_words[4]>>0)&0x3F),6) );
      printf ( "\nIBiasTPcal:%d;\t\tTP_Vcal:%d;\t\tShaperIbias:%d;\t\tIPostamp:%d;",swap_order_N_bits(((g_reg_command_words[5]>>24)&0x1F),5), swap_order_N_bits(((g_reg_command_words[5]>>16)&0x1F),5), ((g_reg_command_words[5]>>8)&0xF), swap_order_N_bits(((g_reg_command_words[5]>>0)&0x1F),5) );
      printf ( "\nTP_Vcal_ref:%d;\t\tVref_integ_diff:%d;\tSig_pol:%d;\t\tFE_TPEnable:%d;", swap_order_N_bits(((g_reg_command_words[6]>>24)&0x1F),5), swap_order_N_bits(((g_reg_command_words[6]>>16)&0x3F),6), ((g_reg_command_words[6]>>8)&0x1), ((g_reg_command_words[6]>>0)&0x1) ) ;
      printf ( "\nDataClkDiv:%d;\t\tTACrefreshPeriod:%d;\tTACrefreshEnable:%d;", ((g_reg_command_words[7]>>16)&0x3), ((g_reg_command_words[7]>>8)&0xF), ((g_reg_command_words[7]>>0)&0x1) ) ;
      printf ( "\nCounterPeriod:%d;\tCounterEnable:%d;\tStopRampEnable:%d;\tRClkEnable:%d;", ((g_reg_command_words[8]>>24)&0x7), ((g_reg_command_words[8]>>16)&0x1), ((g_reg_command_words[8]>>8)&0x3), ((g_reg_command_words[8]>>0)&0x1F) ) ;
      printf ( "\nTDCClkdiv:%d;\t\tVetoMode:%d;\t\tCh_DebugMode:%d;\tTxMode:%d;", ((g_reg_command_words[9]>>24)&0x1), ((g_reg_command_words[9]>>16)&0x3F), ((g_reg_command_words[9]>>8)&0x1 ), ((g_reg_command_words[9]>>0)&0x3) ) ;
      printf ( "\nTxDDR:%d;\t\tTxLinks:%d;", ((g_reg_command_words[10]>>24)&0x1), ((g_reg_command_words[10]>>16)&0x3) )  ;  } 

int main(int argc,char *argv[])
{  
if (argc < 3)
{    printf("\n GEMROC_TIGER_CFG argument list: <Target_GEMROC: 1 thru 22> <Target_FEB_PWR_EN_pattern(Hex):0x0 thru 0xF>\n"); 
    default_arg_needed = 1;}
if (default_arg_needed == 1)
{
    TARGET_GEMROC_ID_param = 3;
    TARGET_FEB_PWR_PATTERN_param = 0x1;}
else
{
    TARGET_GEMROC_ID_param =atoi(argv[1]);
    TARGET_FEB_PWR_PATTERN_param = atoi(argv[2]);}

GEMROC_ID = TARGET_GEMROC_ID_param;
FEB_PWR_EN_pattern = TARGET_FEB_PWR_PATTERN_param;


      HOST_PORT=54817+GEMROC_ID;
      HOST_PORT_RECEIVE=58913+GEMROC_ID;
      sprintf(DEST_IP_ADDRESS,"192.168.1.%d",(GEMROC_ID+16)) ;
//      sprintf(DEST_IP_ADDRESS,"127.0.0.%d",1) ;
      DEST_PORT_NO = 58913;
   fd_cmd = fopen("c_cmd_check.txt","w+");
   fw = fopen("my_c_log.txt","w+");
   file_name= fopen("check_c_para_class.txt","w+");
    bzero(&sin_dest,sizeof(sin_dest));
    sin_dest.sin_family=AF_INET;
    sin_dest.sin_addr.s_addr=inet_addr(DEST_IP_ADDRESS);
    sin_dest.sin_port=htons(DEST_PORT_NO);
    sin_len=sizeof(sin_dest);

    bzero(&send_addr,sizeof(send_addr));
    send_addr.sin_family=AF_INET;
    send_addr.sin_addr.s_addr=inet_addr(HOST_IP);
    send_addr.sin_port=htons(HOST_PORT);
    socket_descriptor=socket(AF_INET,SOCK_DGRAM,0);
    bind(socket_descriptor,(struct sockaddr *)&send_addr,sizeof(send_addr));

    bzero(&recv_addr,sizeof(recv_addr));
    recv_addr.sin_family=AF_INET;
    recv_addr.sin_addr.s_addr=inet_addr(HOST_IP);
    recv_addr.sin_port=htons(HOST_PORT_RECEIVE);
    socket_descriptor1=socket(AF_INET,SOCK_DGRAM,0);
    bind(socket_descriptor1,(struct sockaddr *)&recv_addr,sizeof(recv_addr));

    sprintf(cfg_filename,"GEMROC_%d_def_cfg_LV_2018.txt",GEMROC_ID) ;
    gemroc_init(GEMROC_ID,"NONE",1,cfg_filename);
    print_cmd("gemroc_init",gemroc_command_words,sizeof(gemroc_command_words));
    gemroc_set_FEB_PWR_EN_pattern(FEB_PWR_EN_pattern);
    print_cmd("gemroc_set_FEB_PWR_EN_pattern",gemroc_command_words,sizeof(gemroc_command_words));
    gemroc_set_gemroc_cmd_code("CMD_GEMROC_LV_CFG_WR",1);
     print_cmd("gemroc_set_gemroc_cmd_code",gemroc_command_words,sizeof(gemroc_command_words));
    gemroc_update_command_words();
    print_cmd("gemroc_update_command_words",gemroc_command_words,sizeof(gemroc_command_words));
    send_GEMROC_CFG_CMD_PKT(GEMROC_ID,"send_GEMROC_CFG_CMD_PKT in line 1560",gemroc_command_words,sizeof(gemroc_command_words),DEST_IP_ADDRESS,DEST_PORT_NO);
    printf("sent : CMD_GEMROC_LV_CFG_WR\n");
    
sprintf(cfg_filename,"GEMROC_%d_def_cfg_DAQ_2018v3.txt",GEMROC_ID) ;
DAQ_init(GEMROC_ID,"NONE",0,1,0,cfg_filename);
DAQ_extract_parameters_from_UDP_packet();
send_GEMROC_DAQ_CMD(GEMROC_ID, "send_GEMROC_DAQ_CMD,in line 1566","CMD_GEMROC_DAQ_CFG_WR");

g_reg_init(GEMROC_ID,"TIGER_def_g_cfg_2018.txt");
ch_reg_init(GEMROC_ID,"TIGER_def_ch_cfg_2018.txt");

GEMROC_IVT_read_and_log(GEMROC_ID,1, IVT_LOG_ENABLE, "GEMROC_IVT_read_and_log in line 1571");
FEBPwrEnPattern_set(GEMROC_ID,1,"FEBPwrEnPattern_set in line 1572");
ResetTgtGEMROC_ALL_TIGER_GCfgReg(GEMROC_ID, "ResetTgtGEMROC_ALL_TIGER_GCfgReg in line 1573");
WriteTgtGEMROC_TIGER_GCfgReg_fromfile("g_inst", GEMROC_ID, 0, "TIGER_def_g_cfg_2018.txt", "WriteTgtGEMROC_TIGER_GCfgReg_fromfile in line 1574");
ReadTgtGEMROC_TIGER_GCfgReg("g_inst", GEMROC_ID, 0, "ReadTgtGEMROC_TIGER_GCfgReg in line 1575" );
WriteTgtGEMROC_TIGER_ChCfgReg_fromfile("c_inst", GEMROC_ID,0, 64,"TIGER_def_ch_cfg_2018.txt", "WriteTgtGEMROC_TIGER_ChCfgReg_fromfile in line 1576");
ReadTgtGEMROC_TIGER_ChCfgReg ("c_inst", GEMROC_ID,0,64, 0, "ReadTgtGEMROC_TIGER_ChCfgReg in line 1577" );
set_FE_TPEnable("g_inst", GEMROC_ID, 0, 1, "set_FE_TPEnable in line 1578");
Set_GEMROC_TIGER_ch_TPEn ("c_inst", GEMROC_ID,0,0,0,1,"Set_GEMROC_TIGER_ch_TPEn in line 1579");
SynchReset_to_TgtFEB("SynchReset_to_TgtFEB in line 1580", GEMROC_ID, 0, 0 );
SynchReset_to_TgtTCAM("SynchReset_to_TgtTCAM in line 1581", GEMROC_ID,0,0);
DAQ_set_and_TL_start("DAQ_set_and_TL_start in line 1582", GEMROC_ID,1,1);

close(socket_descriptor);
close(socket_descriptor1);
fclose(fd_cmd);
fclose(fw);
fclose(file_name);













}
