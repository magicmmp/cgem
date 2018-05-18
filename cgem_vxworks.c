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

typedef struct {
      unsigned int TARGET_GEMROC_ID;
      char*        cfg_filename;
      unsigned int parameter_array[37];
      unsigned int BufferBias; 
      unsigned int TDCVcasN;  
      unsigned int TDCVcasP; 
      unsigned int TDCVcasPHyst; 
      unsigned int DiscFE_Ibias;
      unsigned int BiasFE_PpreN; 
      unsigned int AVcasp_global;
      unsigned int TDCcompVcas; 
      unsigned int TDCIref_cs; 
      unsigned int DiscVcasN; 
      unsigned int IntegVb1;
      unsigned int BiasFE_A1; 
      unsigned int Vcasp_Vth;
      unsigned int TAC_I_LSB;
      unsigned int TDCcompVbias;
      unsigned int Vref_Integ;
      unsigned int IBiasTPcal;
      unsigned int TP_Vcal ;
      unsigned int ShaperIbias;
      unsigned int IPostamp;  
      unsigned int TP_Vcal_ref; 
      unsigned int Vref_integ_diff;
      unsigned int Sig_pol;
      unsigned int FE_TPEnable;
      unsigned int DataClkDiv;
      unsigned int TACrefreshPeriod;
      unsigned int TACrefreshEnable;
      unsigned int CounterPeriod;
      unsigned int CounterEnable;
      unsigned int StopRampEnable;
      unsigned int RClkEnable;
      unsigned int TDCClkdiv;
      unsigned int VetoMode ;
      unsigned int Ch_DebugMode; 
      unsigned int TxMode; 
      unsigned int TxDDR ;
      unsigned int TxLinks;
      unsigned int is_a_write;
      unsigned int target_TIGER_ID;
      unsigned int command_code;
      unsigned int command_words[12];}g_reg_para;

typedef struct  {
      unsigned int TARGET_GEMROC_ID;
      char*        cfg_filename;
      unsigned int DisableHyst ;
      unsigned int T2Hyst ;
      unsigned int T1Hyst;
      unsigned int Ch63ObufMSB;
      unsigned int TP_disable_FE; 
      unsigned int TDC_IB_E ;
      unsigned int TDC_IB_T ;
      unsigned int Integ ;
      unsigned int PostAmpGain; 
      unsigned int FE_delay; 
      unsigned int Vth_T2; 
      unsigned int Vth_T1 ;
      unsigned int QTx2Enable; 
      unsigned int MaxIntegTime; 
      unsigned int MinIntegTime; 
      unsigned int TriggerBLatched; 
      unsigned int QdcMode ;
      unsigned int BranchEnableT; 
      unsigned int BranchEnableEQ; 
      unsigned int TriggerMode2B ;
      unsigned int TriggerMode2Q; 
      unsigned int TriggerMode2E; 
      unsigned int TriggerMode2T; 
      unsigned int TACMinAge; 
      unsigned int TACMaxAge; 
      unsigned int CounterMode; 
      unsigned int DeadTime; 
      unsigned int SyncChainLen; 
      unsigned int Ch_DebugMode; 
      unsigned int TriggerMode; 
      unsigned int is_a_write; 
      unsigned int target_TIGER_ID; 
      unsigned int command_code; 
      unsigned int TO_ALL_enable; 
      unsigned int channel_ID; 
      unsigned int command_words[10];
      unsigned int parameter_array[30];}ch_reg_para;

typedef struct {
      unsigned int number_of_repetitions;
      unsigned int TARGET_GEMROC_ID;
      char*        cfg_filename;
      unsigned int parameter_array[31];
      unsigned int TIMING_DLY_FEB0;
      unsigned int TIMING_DLY_FEB1;
      unsigned int TIMING_DLY_FEB2; 
      unsigned int TIMING_DLY_FEB3; 
      unsigned int FEB_PWR_EN_pattern;
      unsigned int FEB_OVC_EN_pattern;
      unsigned int FEB_OVV_EN_pattern;
      unsigned int FEB_OVT_EN_pattern;
      unsigned int ROC_OVT_EN;  
      unsigned int XCVR_LPBCK_TST_EN; 
      unsigned int ROC_OVT_LIMIT; 
      unsigned int A_OVC_LIMIT_FEB0; 
      unsigned int A_OVV_LIMIT_FEB0;
      unsigned int A_OVC_LIMIT_FEB1;
      unsigned int A_OVV_LIMIT_FEB1;
      unsigned int A_OVC_LIMIT_FEB2 ;
      unsigned int A_OVV_LIMIT_FEB2;
      unsigned int A_OVC_LIMIT_FEB3;
      unsigned int A_OVV_LIMIT_FEB3;
      unsigned int D_OVC_LIMIT_FEB0;
      unsigned int D_OVV_LIMIT_FEB0;
      unsigned int OVT_LIMIT_FEB0 ;
      unsigned int D_OVC_LIMIT_FEB1;
      unsigned int D_OVV_LIMIT_FEB1;
      unsigned int OVT_LIMIT_FEB1; 
      unsigned int D_OVC_LIMIT_FEB2;
      unsigned int D_OVV_LIMIT_FEB2;
      unsigned int OVT_LIMIT_FEB2; 
      unsigned int D_OVC_LIMIT_FEB3; 
      unsigned int D_OVV_LIMIT_FEB3; 
      unsigned int OVT_LIMIT_FEB3; 
      char*        command_string; 
      unsigned int is_a_write;
      unsigned int gemroc_cmd_code;
      unsigned int command_words[12]; }gemroc_para;
      char         gemroc_command_list[6][32] = {"NONE",
                            "CMD_GEMROC_LV_CFG_WR",
                            "CMD_GEMROC_LV_CFG_RD",
                            "CMD_GEMROC_LV_IVT_UPDATE",
                            "CMD_GEMROC_LV_IVT_READ",
                            "CMD_GEMROC_TIMING_DELAYS_UPDATE"};      

typedef struct {
      unsigned int TARGET_GEMROC_ID;
      char*        cfg_filename;
      unsigned int parameter_array[15];
      unsigned int EN_TM_TCAM_pattern;
      unsigned int TP_Pos_nNeg;
      unsigned int AUTO_TP_EN_pattern;
      unsigned int AUTO_L1_EN_pattern;
      unsigned int TL_nTM_ACQ;
      unsigned int Periodic_TP_EN_pattern; 
      unsigned int TP_period ;
      unsigned int LowerDataScanWindowOffset;
      unsigned int Periodic_L1_EN_pattern;
      unsigned int L1_period;
      unsigned int UpperDataScanWindowOffset;
      unsigned int TP_width; 
      unsigned int L1_latency;
      unsigned int UDP_DATA_DESTINATION_IPPORT;
      unsigned int UDP_DATA_DESTINATION_IPADDR;
      char*        command_string; 
      unsigned int target_TCAM_ID;
      unsigned int to_ALL_TCAM_enable;
      unsigned int number_of_repetitions;
     
      unsigned int is_a_write;
      unsigned int gemroc_cmd_code;
      unsigned int command_words[5];  } DAQ_para;
      char         DAQ_command_list[10][35] ={"NONE",
                            "CMD_GEMROC_DAQ_CFG_WR",
                            "CMD_GEMROC_DAQ_CFG_RD",
                            "CMD_GEMROC_DAQ_TIGER_GCFGREG_RESET",
                            "CMD_GEMROC_DAQ_TIGER_SYNCH_RST",
                            "CMD_GEMROC_DAQ_TCAM_SYNCH_RST",
                            "CMD_GEMROC_DAQ_TP_GEN",
                            "CMD_GEMROC_DAQ_L1_GEN",
                            "CMD_GEMROC_DAQ_ACK_SEQ_ERROR",
                            "CMD_GEMROC_DAQ_XCVR_LPBCK_TEST"};



      unsigned int num=0;//count send times
g_reg_para g_reg;
ch_reg_para ch_reg;
gemroc_para gemroc;
DAQ_para DAQ; 
FILE *file_name = NULL;
FILE *fd_cmd=NULL;
FILE *fd_read_para=NULL;
//check_file = 'check_para_class.txt'
//file_name = open(check_file, 'w')

int char_type(char c)
{
  if(c==' ')
    return 1;
  else if((c>='A'&&c<='Z')||(c>='a'&&c<='z'))
    return 2;
  else if(c>='0'&&c<='9')
    return 3;
  else
    return 0;
}

void extract_para_from_line(char*line,int line_strlen,char*cmd_string,unsigned int*cmd_para)
{
  int i,j,k,para_num, pre_char_type,para_info[10][3];
  unsigned int n;
  for(i=0;i<10;i++)
    for(j=0;j<3;j++)
      para_info[i][j]=0;
  pre_char_type=1; para_num=0;
  for(i=0;i<line_strlen-1;i++)
{
    if(pre_char_type==1&&char_type(line[i])==2)
   {  para_num=1;
      para_info[0][0]=1;
      para_info[0][1]=i;
      para_info[0][2]=i;}
    if(pre_char_type==2&&char_type(line[i])==2&&para_num==1)
      para_info[0][2]=i;
    if(pre_char_type==1&&char_type(line[i])==3&&para_num)
   {  para_num++;
      if(line[i]=='0'&&line[i+1]=='x'||line[i]=='0'&&line[i+1]=='X')
     {  para_info[para_num-1][0]=3;
        para_info[para_num-1][1]=i+2;
        para_info[para_num-1][2]=i+2; }
      else
     {  para_info[para_num-1][0]=2;
        para_info[para_num-1][1]=i;
        para_info[para_num-1][2]=i;}
    }
    if (para_num>1&&(line[i-1]>='0'&&line[i-1]<='9'||line[i-1]>='a'&&line[i-1]<='f'||line[i-1]>='A'&&line[i-1]<='F')&&(line[i]>='0'&&line[i]<='9'||line[i]>='a'&&line[i]<='f'||line[i]>='A'&&line[i]<='F'))
      para_info[para_num-1][2]=i;
    pre_char_type=char_type(line[i]);
 }
if(para_info[0][0]==1)
 {  j=0;
    for(i=para_info[0][1];i<=para_info[0][2];i++)
   {  cmd_string[j]=line[i];
      j++;}
    cmd_string[j]=0;}
  cmd_para[0]=para_num;
  for(i=1;i<cmd_para[0];i++)
  {
    cmd_para[i]=0;
    if(para_info[i][0]==2)
      for(k=para_info[i][2];k>=para_info[i][1];k--)
     {
        n=1;
        for(j=para_info[i][2]-k;j>0;j--)
          n=n*10;
        cmd_para[i]=cmd_para[i]+(line[k]-48)*n;
     }
     if(para_info[i][0]==3)
        for(k=para_info[i][2];k>=para_info[i][1];k--)
       {
          n=1;
          for(j=para_info[i][2]-k;j>0;j--)
            n=n*16;
          if(line[k]>='0'&&line[k]<='9')
            cmd_para[i]=cmd_para[i]+(line[k]-48)*n;
          else if(line[k]>='A'&&line[k]<='F')
            cmd_para[i]=cmd_para[i]+(line[k]-55)*n;
          else
            cmd_para[i]=cmd_para[i]+(line[k]-87)*n;
        }  }  }

void write_array_to_txt(char*msg,int*array,int arraylen)
 {  
    int i,j;
    j=sprintf(cmd_message,"\n%s, len= %2d\n",msg,arraylen);
    fwrite(cmd_message,sizeof(unsigned char),j,file_name);
    j=0;
    for(i=0;i<arraylen;i++)
      j+=sprintf(cmd_message+j,"%d ",array[i]);
    fwrite(cmd_message,sizeof(unsigned char),j,file_name);
 }


void print_cmd(char*msg,int*array,int len)
{ 
    int i,j,arraylen;
    arraylen=len>>2;
    j=sprintf(cmd_message,"\n%s, cmd len= %d \n",msg,arraylen);
    fwrite(cmd_message,sizeof(unsigned char),j,fd_cmd);
    j=0;
    for(i=0;i<arraylen;i++)
       j+=sprintf(cmd_message+j,"%08X ",array[i]);
    fwrite(cmd_message,sizeof(unsigned char),j,fd_cmd);
}


void print_send_cmd(char*buff,int cmdlen)
{   
    int i; 
    for(i=0;i<cmdlen;i++)
    {
     if(i%4==0)
       printf(" ");
     printf("%02X",buff[i]);
    }
    printf("\n");    }


unsigned int swap_order_N_bits(unsigned int Hex_data,unsigned int N_bits)
{
    unsigned int temp = 0,i;
    for(i=0;i<N_bits;i++)
      if ((Hex_data >> i) & 1)
        temp |= 1 << (N_bits - 1 - i);
    return temp;  }

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
    j=sprintf(cmd_message,"\nnum:%d, %s\n",num,info); // C4996
    fwrite(cmd_message,sizeof(unsigned char),j,fw);
    j=0;
    for(i=0;i<cmdlen;i++)
      j+=sprintf(cmd_message+j,"%02x",buff2[i]);
    fwrite(cmd_message,sizeof(unsigned char),cmdlen<<1,fw);
    recvfrom(socket_descriptor1,buff,cmdlen,0,NULL,NULL);
    j=sprintf(cmd_message,"\nnum:%d, %s\n",num,"echo:"); // C4996
    fwrite(cmd_message,sizeof(unsigned char),j,fw);
    j=0;
    for(i=0;i<cmdlen;i++)
      j+=sprintf(cmd_message+j,"%02x",buff[i]);
    fwrite(cmd_message,sizeof(unsigned char),cmdlen<<1,fw);
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
    j=sprintf(cmd_message,"\nnum:%d, %s\n",num,info);
    fwrite(cmd_message,sizeof(unsigned char),j,fw);
    j=0;
    for(i=0;i<cmdlen;i++)
      j+=sprintf(cmd_message+j,"%02x",buff2[i]);
    fwrite(cmd_message,sizeof(unsigned char),cmdlen<<1,fw);
    recvfrom(socket_descriptor1,buff,cmdlen,0,NULL,NULL);
    j=sprintf(cmd_message,"\nnum:%d, %s\n",num,"echo:"); // C4996
    fwrite(cmd_message,sizeof(unsigned char),j,fw);
    j=0;
    for(i=0;i<cmdlen;i++)
      j+=sprintf(cmd_message+j,"%02x",buff[i]);
    fwrite(cmd_message,sizeof(unsigned char),cmdlen<<1,fw);
}

void send_TIGER_Ch_CFG_Reg_CMD_PKT(unsigned int TIGER_ID_param, char*info,void*array_to_send_param,int cmdlen,
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
    j=sprintf(cmd_message,"\nnum:%d, %s\n",num,info);
    fwrite(cmd_message,sizeof(unsigned char),j,fw);
    j=0;
    for(i=0;i<cmdlen;i++)
      j+=sprintf(cmd_message+j,"%02x",buff2[i]);
    fwrite(cmd_message,sizeof(unsigned char),cmdlen<<1,fw);
    recvfrom(socket_descriptor1,buff,cmdlen,0,NULL,NULL);
    j=sprintf(cmd_message,"\nnum:%d, %s\n",num,"echo:"); // C4996
    fwrite(cmd_message,sizeof(unsigned char),j,fw);
    j=0;
    for(i=0;i<cmdlen;i++)
      j+=sprintf(cmd_message+j,"%02x",buff[i]);
    fwrite(cmd_message,sizeof(unsigned char),cmdlen<<1,fw);
}

void GEMROC_IVT_read_and_log(unsigned int GEMROC_ID_param, unsigned int display_enable_param, 
                             unsigned int log_enable_param, char*info)
{  
    gemroc_set_target_GEMROC(GEMROC_ID_param);
    gemroc_set_gemroc_cmd_code("CMD_GEMROC_LV_IVT_READ",1);
    gemroc_update_command_words();
    send_GEMROC_CFG_CMD_PKT( GEMROC_ID_param,info,gemroc.command_words,sizeof(gemroc.command_words), DEST_IP_ADDRESS, DEST_PORT_NO );
}

void send_GEMROC_LV_CMD (unsigned int GEMROC_ID_param, char*COMMAND_STRING_PARAM,char*info)
{
    gemroc_set_target_GEMROC(GEMROC_ID_param);
    gemroc_set_gemroc_cmd_code(COMMAND_STRING_PARAM,1);
    gemroc_update_command_words();
    send_GEMROC_CFG_CMD_PKT( GEMROC_ID_param, info,gemroc.command_words,sizeof(gemroc.command_words), DEST_IP_ADDRESS, DEST_PORT_NO );  }

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
    send_GEMROC_CFG_CMD_PKT( GEMROC_ID_param, info, DAQ.command_words,sizeof(DAQ.command_words), DEST_IP_ADDRESS, DEST_PORT_NO );}

void  ResetTgtGEMROC_ALL_TIGER_GCfgReg(unsigned int GEMROC_ID_param, char*info)
{
    send_GEMROC_DAQ_CMD(GEMROC_ID_param,info, "CMD_GEMROC_DAQ_TIGER_GCFGREG_RESET");
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
    send_TIGER_GCFG_Reg_CMD_PKT( TIGER_ID_param, info, g_reg.command_words,sizeof(g_reg.command_words),DEST_IP_ADDRESS, DEST_PORT_NO, info);
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
    send_TIGER_GCFG_Reg_CMD_PKT( TIGER_ID_param,info,g_reg.command_words,sizeof(g_reg.command_words),DEST_IP_ADDRESS, DEST_PORT_NO, info);
}

void ReadTgtGEMROC_TIGER_GCfgReg (char*GCFGReg_setting_inst,unsigned int  GEMROC_ID_param, 
                                   unsigned int TIGER_ID_param, char*info)
{
    g_reg_set_target_GEMROC(GEMROC_ID_param);
    g_reg_set_target_TIGER(TIGER_ID_param);
    char*COMMAND_STRING = "RD";
    g_reg_set_command_code(COMMAND_STRING);
    g_reg_update_command_words();
    send_TIGER_GCFG_Reg_CMD_PKT( TIGER_ID_param,info, g_reg.command_words,sizeof(g_reg.command_words), DEST_IP_ADDRESS, DEST_PORT_NO,info);   }
//4-16 17:09
void WriteTgtGEMROC_TIGER_ChCfgReg_fromfile (char*ChCFGReg_setting_inst,unsigned int  GEMROC_ID_param, 
          unsigned int TIGER_ID_param, unsigned int channel_ID_param, char*ChCFGReg_def_fname_param, char*info)
{   int i;
    ch_reg_reload_chcfg_settings_from_file(ChCFGReg_def_fname_param);
    print_cmd("1576,ch_reg_reload_chcfg_settings_from_file",ch_reg.command_words,sizeof(ch_reg.command_words));
    ch_reg_set_target_GEMROC(GEMROC_ID_param);
    print_cmd("1576,ch_reg_set_target_GEMROC",ch_reg.command_words,sizeof(ch_reg.command_words));
    ch_reg_set_target_TIGER(TIGER_ID_param);
    print_cmd("1576,ch_reg_set_target_TIGER",ch_reg.command_words,sizeof(ch_reg.command_words));
    ch_reg_set_to_ALL_param (0);
    print_cmd("1576,ch_reg_set_to_ALL_param",ch_reg.command_words,sizeof(ch_reg.command_words));
    char*COMMAND_STRING = "WR";
    ch_reg_set_command_code(COMMAND_STRING);
    print_cmd("1576,ch_reg_set_command_code",ch_reg.command_words,sizeof(ch_reg.command_words));
    if (channel_ID_param < 64)  {
        ch_reg_set_target_channel(channel_ID_param);
        ch_reg_update_command_words();
        send_TIGER_Ch_CFG_Reg_CMD_PKT(TIGER_ID_param,info,ch_reg.command_words,sizeof(ch_reg.command_words),DEST_IP_ADDRESS,DEST_PORT_NO,info) ; }
    else {
        for(i=0;i<64;i++)
       {
            ch_reg_set_target_channel(i);
            ch_reg_update_command_words();
            if(i==0)
               print_cmd("1576,ch_reg_update_command_words",ch_reg.command_words,sizeof(ch_reg.command_words));
            send_TIGER_Ch_CFG_Reg_CMD_PKT(TIGER_ID_param,info,ch_reg.command_words,sizeof(ch_reg.command_words),DEST_IP_ADDRESS,DEST_PORT_NO,info);  }  }  }

//4-16 17:15
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
        send_TIGER_Ch_CFG_Reg_CMD_PKT(TIGER_ID_param, info,ch_reg.command_words,sizeof(ch_reg.command_words), DEST_IP_ADDRESS, DEST_PORT_NO, info);
        if (verbose_mode == 1); }
    else
        for(i=0;i<64;i++){
            ch_reg_set_target_channel(i);
            ch_reg_update_command_words();
            send_TIGER_Ch_CFG_Reg_CMD_PKT(TIGER_ID_param,info,ch_reg.command_words,sizeof(ch_reg.command_words), DEST_IP_ADDRESS, DEST_PORT_NO,info );
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
        ch_reg.TriggerMode = TriggerMode_param;
        ch_reg_update_command_words();
        send_TIGER_Ch_CFG_Reg_CMD_PKT(TIGER_ID_param,info,ch_reg.command_words,sizeof(ch_reg.command_words), DEST_IP_ADDRESS, DEST_PORT_NO,info) ;}
    else
        for(i=0;i<64;i++){
            ch_reg_set_target_channel(i);
            ch_reg_set_TP_disable_FE(TP_disable_FE_param);
            ch_reg.TriggerMode = TriggerMode_param;
            ch_reg_update_command_words();
            send_TIGER_Ch_CFG_Reg_CMD_PKT(TIGER_ID_param, info,ch_reg.command_words,sizeof(ch_reg.command_words), DEST_IP_ADDRESS, DEST_PORT_NO,info );}
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
        send_TIGER_Ch_CFG_Reg_CMD_PKT(TIGER_ID_param, info, ch_reg.command_words,sizeof(ch_reg.command_words), DEST_IP_ADDRESS, DEST_PORT_NO,info);}
    else
        for(i=0;i<64;i++) {
            ch_reg_set_target_channel(i);
            ch_reg_update_command_words();
            send_TIGER_Ch_CFG_Reg_CMD_PKT(TIGER_ID_param, info,ch_reg.command_words,sizeof(ch_reg.command_words), DEST_IP_ADDRESS, DEST_PORT_NO,info);  }    }

void set_AVcasp_global(char*GCFGReg_setting_inst, unsigned int GEMROC_ID_param, unsigned int TIGER_ID_param,
                        unsigned int AVcasp_global_param, char*info)
{
    g_reg_set_target_GEMROC(GEMROC_ID_param);
    g_reg_set_target_TIGER(TIGER_ID_param);
    g_reg_set_AVcasp_global(AVcasp_global_param);
    char*COMMAND_STRING = "WR";
    g_reg_set_command_code(COMMAND_STRING);
    g_reg_update_command_words();
    send_TIGER_GCFG_Reg_CMD_PKT( TIGER_ID_param,info, g_reg.command_words,sizeof(g_reg.command_words), DEST_IP_ADDRESS, DEST_PORT_NO, info);
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
    gemroc.FEB_OVC_EN_pattern = FEB_OVC_EN_pattern_param & 0xF;
    gemroc.FEB_OVV_EN_pattern = FEB_OVV_EN_pattern_param & 0xF;
    gemroc.FEB_OVT_EN_pattern = FEB_OVT_EN_pattern_param & 0xF;
    gemroc.ROC_OVT_EN = ROC_OVT_EN_param & 0x1;
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
    gemroc.A_OVV_LIMIT_FEB3 = FEB3_OVVA_thr_Unsigned8;
    gemroc.A_OVV_LIMIT_FEB2 = FEB2_OVVA_thr_Unsigned8;
    gemroc.A_OVV_LIMIT_FEB1 = FEB1_OVVA_thr_Unsigned8;
    gemroc.A_OVV_LIMIT_FEB0 = FEB0_OVVA_thr_Unsigned8;
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
    gemroc.D_OVV_LIMIT_FEB3 = FEB3_OVVD_thr_Unsigned8;
    gemroc.D_OVV_LIMIT_FEB2 = FEB2_OVVD_thr_Unsigned8;
    gemroc.D_OVV_LIMIT_FEB1 = FEB1_OVVD_thr_Unsigned8;
    gemroc.D_OVV_LIMIT_FEB0 = FEB0_OVVD_thr_Unsigned8;
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
    gemroc.A_OVC_LIMIT_FEB3 = FEB3_OVCA_thr_U9;
    gemroc.A_OVC_LIMIT_FEB2 = FEB2_OVCA_thr_U9;
    gemroc.A_OVC_LIMIT_FEB1 = FEB1_OVCA_thr_U9;
    gemroc.A_OVC_LIMIT_FEB0 = FEB0_OVCA_thr_U9;
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
    gemroc.D_OVC_LIMIT_FEB3 = FEB3_OVCD_thr_U9;
    gemroc.D_OVC_LIMIT_FEB2 = FEB2_OVCD_thr_U9;
    gemroc.D_OVC_LIMIT_FEB1 = FEB1_OVCD_thr_U9;
    gemroc.D_OVC_LIMIT_FEB0 = FEB0_OVCD_thr_U9;
    char*COMMAND_STRING = "CMD_GEMROC_LV_CFG_WR";
    send_GEMROC_LV_CMD (GEMROC_ID_param, COMMAND_STRING,info);
}
//2018-4-19 11:20
void Set_OVTF_LIMIT(char*gemroc_inst_param, int GEMROC_ID_param, int FEB3_OVTF_thr_param,int FEB2_OVTF_thr_param, 
                     int FEB1_OVTF_thr_param, int FEB0_OVTF_thr_param, char*info)
{
    float T_ref_PT1000 = 25.0;
    float V_ADC_at_25C = 247.2;
    float ADC_res_mV_1LSB = 0.305;
    int   T_ADC_data_shift = 2;
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
    gemroc.OVT_LIMIT_FEB3 = FEB3_OVTF_thr_Unsigned8;
    gemroc.OVT_LIMIT_FEB2 = FEB2_OVTF_thr_Unsigned8;
    gemroc.OVT_LIMIT_FEB1 = FEB1_OVTF_thr_Unsigned8;
    gemroc.OVT_LIMIT_FEB0 = FEB0_OVTF_thr_Unsigned8;
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
    gemroc.ROC_OVT_LIMIT = ROC_OVT_thr_Unsigned6;
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
      DAQ.TARGET_GEMROC_ID = TARGET_GEMROC_ID_param ;
      DAQ.cfg_filename = cfg_filename_param;
      readfromtxt(DAQ.cfg_filename,DAQ.parameter_array,15);
      write_array_to_txt("DAQ_init",DAQ.parameter_array,15);
      DAQ.EN_TM_TCAM_pattern = DAQ.parameter_array [GEMROC_CMD_DAQ_Num_of_params-1];
      DAQ.TP_Pos_nNeg = DAQ.parameter_array [GEMROC_CMD_DAQ_Num_of_params-2];
      DAQ.AUTO_TP_EN_pattern = DAQ.parameter_array [GEMROC_CMD_DAQ_Num_of_params-3];
      DAQ.AUTO_L1_EN_pattern = DAQ.parameter_array [GEMROC_CMD_DAQ_Num_of_params-4];
      DAQ.TL_nTM_ACQ = DAQ.parameter_array [GEMROC_CMD_DAQ_Num_of_params-5];
      DAQ.Periodic_TP_EN_pattern = DAQ.parameter_array [GEMROC_CMD_DAQ_Num_of_params-6];
      DAQ.TP_period = DAQ.parameter_array [GEMROC_CMD_DAQ_Num_of_params-7] ;
      DAQ.LowerDataScanWindowOffset = DAQ.parameter_array [GEMROC_CMD_DAQ_Num_of_params-8];
      DAQ.Periodic_L1_EN_pattern = DAQ.parameter_array [GEMROC_CMD_DAQ_Num_of_params-9] ;
      DAQ.L1_period = DAQ.parameter_array [GEMROC_CMD_DAQ_Num_of_params-10] ;
      DAQ.UpperDataScanWindowOffset = DAQ.parameter_array [GEMROC_CMD_DAQ_Num_of_params-11];
      DAQ.TP_width = DAQ.parameter_array [GEMROC_CMD_DAQ_Num_of_params-12]; 
      DAQ.L1_latency = DAQ.parameter_array [GEMROC_CMD_DAQ_Num_of_params-13];
      DAQ.UDP_DATA_DESTINATION_IPPORT = DAQ.parameter_array [GEMROC_CMD_DAQ_Num_of_params-14];
      DAQ.UDP_DATA_DESTINATION_IPADDR = DAQ.parameter_array [GEMROC_CMD_DAQ_Num_of_params-15];
      DAQ.command_string = command_string_param;
      DAQ.target_TCAM_ID = TCAM_ID_param;
      DAQ.to_ALL_TCAM_enable = to_ALL_TCAM_enable_param;
      DAQ.number_of_repetitions = number_of_repetitions_param;
      DAQ.is_a_write = 0x1;

      for(i=0;i<10;i++)
        if(strcmp(DAQ.command_string,DAQ_command_list[i])==0)
          DAQ.gemroc_cmd_code = i;
      header_tag = 0x8 << 28;
      gemroc_cmd_ID = 0xD;
      gemroc_cmd_tag = gemroc_cmd_ID << 24;
      gemroc_cmd_word_count = GEMROC_CMD_DAQ_Num_Of_PktWords - 1;
      DAQ.command_words[0]= header_tag + (DAQ.TARGET_GEMROC_ID << 16) + gemroc_cmd_tag + ((DAQ.UDP_DATA_DESTINATION_IPADDR & 0xFF)<<8) + gemroc_cmd_word_count;
      DAQ.command_words[4] = ((DAQ.UDP_DATA_DESTINATION_IPPORT & 0xF)<<26) + ((DAQ.number_of_repetitions & 0x3FF) << 16) + ((DAQ.gemroc_cmd_code & 0xF) << 11) + ((DAQ.target_TCAM_ID & 0x3) << 8) + ((DAQ.to_ALL_TCAM_enable & 0x1) << 6);
      DAQ.command_words[1] = ((DAQ.L1_latency & 0x3FF) << 20) + ((DAQ.TP_width & 0xF) << 16) + (DAQ.UpperDataScanWindowOffset & 0xFFFF);
      DAQ.command_words[2] = ((DAQ.L1_period  & 0x3FF) << 20) + ((DAQ.Periodic_L1_EN_pattern  & 0xF) << 16) + (DAQ.LowerDataScanWindowOffset & 0xFFFF);    printf("\nPeriodic_L1_EN_pattern=%d\n",DAQ.Periodic_L1_EN_pattern&0xF);
      DAQ.command_words[3] = ((DAQ.TP_period & 0x3FF) << 20) + ((DAQ.Periodic_TP_EN_pattern & 0xF) << 16) + ((DAQ.TL_nTM_ACQ & 0x1) << 11) + ((DAQ.AUTO_L1_EN_pattern & 0x1) << 10) + ((DAQ.AUTO_TP_EN_pattern & 0x1) << 9) + ((DAQ.TP_Pos_nNeg & 0x1) << 8)  + (DAQ.EN_TM_TCAM_pattern & 0xFF);            printf("\nPeriodic_TP_EN_pattern=%d\n",DAQ.Periodic_TP_EN_pattern&0xF);
}


unsigned int DAQ_cmd_words_array_size()
{
      return 5;  }

void DAQ_print_command_words()
{    int i;
   for(i=0;i<5;i++)
     printf("%08X\n",DAQ.command_words[i]);  }

void DAQ_set_target_GEMROC (unsigned int GEMROC_ID_param)
{     
      DAQ.TARGET_GEMROC_ID = GEMROC_ID_param & 0x1F;
      DAQ.command_words[0] = (DAQ.command_words[0] & 0xFF00FFFF) + (DAQ.TARGET_GEMROC_ID << 16);  }

void DAQ_set_target_TCAM_ID(unsigned int target_TA_param,unsigned int to_ALL_TCAM_EN_param)
{
      DAQ.target_TCAM_ID = target_TA_param & 0x3;
      DAQ.to_ALL_TCAM_enable = to_ALL_TCAM_EN_param & 0x1;  }

void DAQ_set_TP_width(unsigned int target_TP_width_param)
{
      DAQ.TP_width = target_TP_width_param & 0xF;}

void DAQ_set_TP_period(unsigned int TP_period_param)
{
      DAQ.TP_period = TP_period_param & 0x3FF;}

void DAQ_set_AUTO_TP_EN_pattern(unsigned int target_AUTO_TP_EN_param)
{
      DAQ.AUTO_TP_EN_pattern = target_AUTO_TP_EN_param & 0xF;  }

void DAQ_set_Periodic_TP_EN_pattern(unsigned int Periodic_TP_EN_pattern_param)
{
      DAQ.Periodic_TP_EN_pattern = Periodic_TP_EN_pattern_param & 0xF;  }

void DAQ_set_TL_nTM_ACQ(unsigned int TL_nTM_ACQ_param)
{
      DAQ.TL_nTM_ACQ = TL_nTM_ACQ_param & 0x1;  }

void DAQ_set_EN_TM_TCAM_pattern(unsigned int EN_TM_TCAM_pattern_param)
{
      DAQ.EN_TM_TCAM_pattern = EN_TM_TCAM_pattern_param & 0xFF;  }

void DAQ_set_TP_Pos_nNeg(unsigned int TP_Pos_nNeg_param)
{
      DAQ.TP_Pos_nNeg = TP_Pos_nNeg_param & 0x1;}

void DAQ_set_gemroc_cmd_code( char* command_string_param,unsigned int no_of_executions_param)
{    int i;
      DAQ.number_of_repetitions = no_of_executions_param;
      DAQ.command_string = command_string_param;
      for(i=0;i<10;i++)
        if(strcmp(DAQ.command_string,DAQ_command_list[i])==0)
            DAQ.gemroc_cmd_code = i;   }

void DAQ_update_command_words()
{
       if ( ((DAQ.gemroc_cmd_code & 0xF) != 0x0)&&((DAQ.gemroc_cmd_code & 0xF) != 0x1) )
       {
           DAQ.command_words[0] &= ~(0xFF << 8);
           DAQ.command_words[1]= 0;
           DAQ.command_words[2]= 0;
           DAQ.command_words[3]= 0;
           DAQ.command_words[4]= ((DAQ.number_of_repetitions & 0x3FF) << 16) + ((DAQ.gemroc_cmd_code & 0xF) << 11) + ((DAQ.target_TCAM_ID & 0x3) << 8) + ((DAQ.to_ALL_TCAM_enable & 0x1) << 6);  }
       else{
           DAQ.command_words[0]&= ~(0xFF << 8);
           DAQ.command_words[0]+= ((DAQ.UDP_DATA_DESTINATION_IPADDR & 0xFF)<<8);
           DAQ.command_words[1]= ((DAQ.L1_latency & 0x3FF) << 20) + ((DAQ.TP_width & 0xF) << 16) + (DAQ.UpperDataScanWindowOffset & 0xFFFF);
           DAQ.command_words[2]= ((DAQ.L1_period  & 0x3FF) << 20) + ((DAQ.Periodic_L1_EN_pattern  & 0xF) << 16) + (DAQ.LowerDataScanWindowOffset & 0xFFFF);
           printf( "\n L1_period =  %d\n",(DAQ.command_words[2] >> 20) &  0x3FF) ;
           DAQ.command_words[3]= ((DAQ.TP_period & 0x3FF) << 20) + ((DAQ.Periodic_TP_EN_pattern & 0xF) << 16) + ((DAQ.TL_nTM_ACQ & 0x1) << 11) + ((DAQ.AUTO_L1_EN_pattern & 0x1) << 10) + ((DAQ.AUTO_TP_EN_pattern & 0x1) << 9) + ((DAQ.TP_Pos_nNeg & 0x1) << 8)  + (DAQ.EN_TM_TCAM_pattern & 0xFF);
           DAQ.command_words[4]= ((DAQ.UDP_DATA_DESTINATION_IPPORT & 0xF)<<26) + ((DAQ.number_of_repetitions & 0x3FF) << 16) + ((DAQ.gemroc_cmd_code & 0xF) << 11) + ((DAQ.target_TCAM_ID & 0x3) << 8) + ((DAQ.to_ALL_TCAM_enable & 0x1) << 6);}  }


void DAQ_extract_parameters_from_UDP_packet()
{
      printf( "\n DATA_DESTINATION_IPadr  = %X %d",((DAQ.command_words[0] >>  8) &  0xFF) , ((DAQ.command_words[0] >>  8) &  0xFF) );
      printf( "\n DATA_DESTINATION_IPport = %X %d",((DAQ.command_words[4] >> 26) &  0xF)   , ((DAQ.command_words[4] >> 26) &  0xF) );
      printf( "\n L1_latency = %X %d",((DAQ.command_words[1] >> 20) &  0x3FF) , ((DAQ.command_words[1] >> 20) &  0x3FF) );
      printf( "\n TP_width = %X %d",((DAQ.command_words[1] >> 16) &  0xF)   , ((DAQ.command_words[1] >> 16) &  0xF) );
      printf( "\n UpperDataScanWindowOffset = %X %d",((DAQ.command_words[1] >> 0)  &  0xFFFF), ((DAQ.command_words[1] >> 0)  &  0xFFFF) );
      printf( "\n L1_period = %X %d",((DAQ.command_words[2] >> 20) &  0x3FF) , ((DAQ.command_words[2] >> 20) &  0x3FF) );
      printf( "\n Periodic_L1_EN_pattern = %X %d",((DAQ.command_words[2] >> 16) &  0xF)   , ((DAQ.command_words[2] >> 16) &  0xF) );
      printf( "\n LowerDataScanWindowOffset = %X %d",((DAQ.command_words[2] >> 0)  &  0xFFFF), ((DAQ.command_words[2] >> 0)  &  0xFFFF) );
      printf( "\n TP_period = %X %d",((DAQ.command_words[3] >> 20)  &  0x3FF), ((DAQ.command_words[3] >> 20)  &  0x3FF) );
      printf( "\n Periodic_TP_EN_pattern = %X %d",((DAQ.command_words[3] >> 16)  &  0xF)  , ((DAQ.command_words[3] >> 16)  &  0xF) );
      printf( "\n TL_nTM_ACQ = %X %d",((DAQ.command_words[3] >> 11)  &  0x1)  , ((DAQ.command_words[3] >> 11)  &  0x1) );
      printf( "\n AUTO_L1_EN_pattern = %X %d",((DAQ.command_words[3] >> 10)  &  0x1)  , ((DAQ.command_words[3] >> 10)  &  0x1) );
      printf( "\n AUTO_TP_EN_pattern = %X %d",((DAQ.command_words[3] >>  9)  &  0x1)  , ((DAQ.command_words[3] >>  9)  &  0x1) );
      printf( "\n TP_Pos_nNeg = %X %d",((DAQ.command_words[3] >>  8)  &  0x1)  , ((DAQ.command_words[3] >>  8)  &  0x1) );
      printf( "\n EN_TM_TCAM_pattern = %X %d",((DAQ.command_words[3] >>  0)  &  0xFF) , ((DAQ.command_words[3] >>  0)  &  0xFF) );
      printf( "\n number_of_repetitions = %X %d",((DAQ.command_words[4] >> 16)  & 0x3FF)  , ((DAQ.command_words[4] >> 16)  & 0x3FF) );
      printf( "\n target_TCAM_ID = %X %d",((DAQ.command_words[4] >>  8)  &  0x3)  , ((DAQ.command_words[4] >>  8)  &  0x3) );
      printf( "\n to_ALL_TCAM_enable = %X %d\n",((DAQ.command_words[4] >>  6)  &  0x1)  , ((DAQ.command_words[4] >>  6)  &  0x1) );  }




void gemroc_init(unsigned int TARGET_GEMROC_ID_param,char*command_string_param,unsigned int number_of_repetitions_param,
                 char*cfg_filename_param)
{     unsigned int header_tag,i;
      unsigned int gemroc_cmd_ID;
      unsigned int gemroc_cmd_tag;
      unsigned int gemroc_cmd_word_count;
      gemroc.TARGET_GEMROC_ID = TARGET_GEMROC_ID_param;
      gemroc.cfg_filename = cfg_filename_param;
      readfromtxt(gemroc.cfg_filename,gemroc.parameter_array,31);
      write_array_to_txt("gemroc.init",gemroc.parameter_array,31);
      gemroc.TIMING_DLY_FEB0 = gemroc.parameter_array [GEMROC_CMD_LV_Num_of_params-1];
      gemroc.TIMING_DLY_FEB1 = gemroc.parameter_array [GEMROC_CMD_LV_Num_of_params-2];
      gemroc.TIMING_DLY_FEB2 = gemroc.parameter_array [GEMROC_CMD_LV_Num_of_params-3];
      gemroc.TIMING_DLY_FEB3 = gemroc.parameter_array [GEMROC_CMD_LV_Num_of_params-4];
      gemroc.FEB_PWR_EN_pattern = gemroc.parameter_array [GEMROC_CMD_LV_Num_of_params-5];
      gemroc.FEB_OVC_EN_pattern = gemroc.parameter_array [GEMROC_CMD_LV_Num_of_params-6];
      gemroc.FEB_OVV_EN_pattern = gemroc.parameter_array [GEMROC_CMD_LV_Num_of_params-7];
      gemroc.FEB_OVT_EN_pattern = gemroc.parameter_array [GEMROC_CMD_LV_Num_of_params-8];
      gemroc.ROC_OVT_EN  = gemroc.parameter_array [GEMROC_CMD_LV_Num_of_params-9];
      gemroc.XCVR_LPBCK_TST_EN  = gemroc.parameter_array [GEMROC_CMD_LV_Num_of_params-10]; 
      gemroc.ROC_OVT_LIMIT = gemroc.parameter_array [GEMROC_CMD_LV_Num_of_params-11];
      gemroc.A_OVC_LIMIT_FEB0 = gemroc.parameter_array [GEMROC_CMD_LV_Num_of_params-12];
      gemroc.A_OVV_LIMIT_FEB0 = gemroc.parameter_array [GEMROC_CMD_LV_Num_of_params-13];
      gemroc.A_OVC_LIMIT_FEB1 = gemroc.parameter_array [GEMROC_CMD_LV_Num_of_params-14];
      gemroc.A_OVV_LIMIT_FEB1 = gemroc.parameter_array [GEMROC_CMD_LV_Num_of_params-15];
      gemroc.A_OVC_LIMIT_FEB2 = gemroc.parameter_array [GEMROC_CMD_LV_Num_of_params-16];
      gemroc.A_OVV_LIMIT_FEB2 = gemroc.parameter_array [GEMROC_CMD_LV_Num_of_params-17];
      gemroc.A_OVC_LIMIT_FEB3 = gemroc.parameter_array [GEMROC_CMD_LV_Num_of_params-18];
      gemroc.A_OVV_LIMIT_FEB3 = gemroc.parameter_array [GEMROC_CMD_LV_Num_of_params-19];
      gemroc.D_OVC_LIMIT_FEB0 = gemroc.parameter_array [GEMROC_CMD_LV_Num_of_params-20];
      gemroc.D_OVV_LIMIT_FEB0 = gemroc.parameter_array [GEMROC_CMD_LV_Num_of_params-21];
      gemroc.OVT_LIMIT_FEB0 = gemroc.parameter_array [GEMROC_CMD_LV_Num_of_params-22] ;
      gemroc.D_OVC_LIMIT_FEB1 = gemroc.parameter_array [GEMROC_CMD_LV_Num_of_params-23];
      gemroc.D_OVV_LIMIT_FEB1 = gemroc.parameter_array [GEMROC_CMD_LV_Num_of_params-24];
      gemroc.OVT_LIMIT_FEB1 = gemroc.parameter_array [GEMROC_CMD_LV_Num_of_params-25];
      gemroc.D_OVC_LIMIT_FEB2 = gemroc.parameter_array [GEMROC_CMD_LV_Num_of_params-26];
      gemroc.D_OVV_LIMIT_FEB2 = gemroc.parameter_array [GEMROC_CMD_LV_Num_of_params-27];
      gemroc.OVT_LIMIT_FEB2 = gemroc.parameter_array [GEMROC_CMD_LV_Num_of_params-28];
      gemroc.D_OVC_LIMIT_FEB3 = gemroc.parameter_array [GEMROC_CMD_LV_Num_of_params-29];
      gemroc.D_OVV_LIMIT_FEB3 = gemroc.parameter_array [GEMROC_CMD_LV_Num_of_params-30];
      gemroc.OVT_LIMIT_FEB3 = gemroc.parameter_array [GEMROC_CMD_LV_Num_of_params-31];
      gemroc.command_string = command_string_param;
      gemroc.is_a_write = 0x1;
      for(i=0;i<6;i++)
       if(strcmp(gemroc.command_string,gemroc_command_list[i])==0)
          gemroc.gemroc_cmd_code = i;
      header_tag = 0x8 << 28;
      gemroc_cmd_ID = 0xC;
      gemroc_cmd_tag = gemroc_cmd_ID << 24;
      gemroc_cmd_word_count = GEMROC_CMD_LV_Num_Of_PktWords - 1; 
      gemroc.command_words[0] = header_tag + (gemroc.TARGET_GEMROC_ID << 16) + gemroc_cmd_tag + gemroc_cmd_word_count;
      gemroc.command_words[1] = ((gemroc.OVT_LIMIT_FEB3 & 0xFF) << 22) + ((gemroc.D_OVV_LIMIT_FEB3 & 0x1FF) << 13) + ((gemroc.D_OVC_LIMIT_FEB3 & 0x1FF) << 4);
      gemroc.command_words[2]  = ((gemroc.OVT_LIMIT_FEB2 & 0xFF) << 22) + ((gemroc.D_OVV_LIMIT_FEB2 & 0x1FF) << 13) + ((gemroc.D_OVC_LIMIT_FEB2 & 0x1FF) << 4);
      gemroc.command_words[3]  = ((gemroc.OVT_LIMIT_FEB1 & 0xFF) << 22) + ((gemroc.D_OVV_LIMIT_FEB1 & 0x1FF) << 13) + ((gemroc.D_OVC_LIMIT_FEB1 & 0x1FF) << 4);
      gemroc.command_words[4]  = ((gemroc.OVT_LIMIT_FEB0 & 0xFF) << 22) + ((gemroc.D_OVV_LIMIT_FEB0 & 0x1FF) << 13) + ((gemroc.D_OVC_LIMIT_FEB0 & 0x1FF) << 4);
      gemroc.command_words[5]  = ((gemroc.A_OVV_LIMIT_FEB3 & 0x1FF) << 13) + ((gemroc.A_OVC_LIMIT_FEB3 & 0x1FF) << 4);
      gemroc.command_words[6]  = ((gemroc.A_OVV_LIMIT_FEB2 & 0x1FF) << 13) + ((gemroc.A_OVC_LIMIT_FEB2 & 0x1FF) << 4);
      gemroc.command_words[7]  = ((gemroc.A_OVV_LIMIT_FEB1 & 0x1FF) << 13) + ((gemroc.A_OVC_LIMIT_FEB1 & 0x1FF) << 4);
      gemroc.command_words[8]  = ((gemroc.A_OVV_LIMIT_FEB0 & 0x1FF) << 13) + ((gemroc.A_OVC_LIMIT_FEB0 & 0x1FF) << 4);
      gemroc.command_words[9] = ((gemroc.ROC_OVT_LIMIT & 0x3F) << 24) + ((gemroc.XCVR_LPBCK_TST_EN & 0x1) << 18) +((gemroc.ROC_OVT_EN & 0x1) << 16) + ((gemroc.FEB_OVT_EN_pattern & 0xF) << 12) + ((gemroc.FEB_OVV_EN_pattern & 0xF) << 8) + ((gemroc.FEB_OVC_EN_pattern & 0xF) << 4)+(gemroc.FEB_PWR_EN_pattern & 0xF);
      gemroc.command_words[10] = ((gemroc.TIMING_DLY_FEB3 & 0x3F) << 24) + ((gemroc.TIMING_DLY_FEB2 & 0x3F) << 16) + ((gemroc.TIMING_DLY_FEB1 & 0x3F) << 8) + ((gemroc.TIMING_DLY_FEB0 & 0x3F) << 0);
      gemroc.command_words[11] = ((gemroc.gemroc_cmd_code & 0xF) << 11);   }

unsigned int gemroc_cmd_words_array_size()
{
      return 12;  }

void gemroc_print_command_words()
{     int i;
      for(i=0;i<12;i++)
         printf("%08X\n",gemroc.command_words[i]);   }

void gemroc_set_target_GEMROC (unsigned int GEMROC_ID_param)
{
      gemroc.TARGET_GEMROC_ID = GEMROC_ID_param & 0x1F;
      gemroc.command_words[0] = (gemroc.command_words[0] & 0xFF00FFFF) + (gemroc.TARGET_GEMROC_ID << 16);}

void gemroc_set_FEB_PWR_EN_pattern(unsigned int target_FEB_PWR_EN_pattern)
{
      gemroc.FEB_PWR_EN_pattern = target_FEB_PWR_EN_pattern & 0xF;  }

void gemroc_set_timing_toFEB_delay(unsigned int FEB3dly_param, unsigned int FEB2dly_param,
                                   unsigned int FEB1dly_param,unsigned int  FEB0dly_param)
{
      gemroc.TIMING_DLY_FEB3 = FEB3dly_param & 0x3F;
      gemroc.TIMING_DLY_FEB2 = FEB2dly_param & 0x3F;
      gemroc.TIMING_DLY_FEB1 = FEB1dly_param & 0x3F;
      gemroc.TIMING_DLY_FEB0 = FEB0dly_param & 0x3F;     }

void gemroc_set_gemroc_cmd_code(char*command_string_param, unsigned int no_of_executions_param)
{     int i;
      gemroc.number_of_repetitions = no_of_executions_param;
      gemroc.command_string = command_string_param;
      for(i=0;i<6;i++)
         if(strcmp(gemroc.command_string,gemroc_command_list[i])==0)
            gemroc.gemroc_cmd_code = i;    }

void gemroc_update_command_words()
{     int i;
      if ( ((gemroc.gemroc_cmd_code & 0xF) == 0x2)||((gemroc.gemroc_cmd_code & 0xF) == 0x3)||((gemroc.gemroc_cmd_code & 0xF) == 0x4) )
        for(i=1;i<11;i++)
           gemroc.command_words[i]=0;
      else {
         gemroc.command_words[1] = ((gemroc.OVT_LIMIT_FEB3 & 0xFF) << 22) + ((gemroc.D_OVV_LIMIT_FEB3 & 0x1FF) << 13) + ((gemroc.D_OVC_LIMIT_FEB3 & 0x1FF) << 4);
         gemroc.command_words[2]  = ((gemroc.OVT_LIMIT_FEB2 & 0xFF) << 22) + ((gemroc.D_OVV_LIMIT_FEB2 & 0x1FF) << 13) + ((gemroc.D_OVC_LIMIT_FEB2 & 0x1FF) << 4);
         gemroc.command_words[3]  = ((gemroc.OVT_LIMIT_FEB1 & 0xFF) << 22) + ((gemroc.D_OVV_LIMIT_FEB1 & 0x1FF) << 13) + ((gemroc.D_OVC_LIMIT_FEB1 & 0x1FF) << 4);
         gemroc.command_words[4]  = ((gemroc.OVT_LIMIT_FEB0 & 0xFF) << 22) + ((gemroc.D_OVV_LIMIT_FEB0 & 0x1FF) << 13) + ((gemroc.D_OVC_LIMIT_FEB0 & 0x1FF) << 4);
         gemroc.command_words[5]  = ((gemroc.A_OVV_LIMIT_FEB3 & 0x1FF) << 13) + ((gemroc.A_OVC_LIMIT_FEB3 & 0x1FF) << 4);
         gemroc.command_words[6]  = ((gemroc.A_OVV_LIMIT_FEB2 & 0x1FF) << 13) + ((gemroc.A_OVC_LIMIT_FEB2 & 0x1FF) << 4);
         gemroc.command_words[7]  = ((gemroc.A_OVV_LIMIT_FEB1 & 0x1FF) << 13) + ((gemroc.A_OVC_LIMIT_FEB1 & 0x1FF) << 4);
         gemroc.command_words[8]  = ((gemroc.A_OVV_LIMIT_FEB0 & 0x1FF) << 13) + ((gemroc.A_OVC_LIMIT_FEB0 & 0x1FF) << 4);
         gemroc.command_words[9] = ((gemroc.ROC_OVT_LIMIT & 0x3F) << 24) + ((gemroc.XCVR_LPBCK_TST_EN & 0x1) << 18) +((gemroc.ROC_OVT_EN & 0x1) << 16) + ((gemroc.FEB_OVT_EN_pattern & 0xF) << 12) + ((gemroc.FEB_OVV_EN_pattern & 0xF) << 8) + ((gemroc.FEB_OVC_EN_pattern & 0xF) << 4) + (gemroc.FEB_PWR_EN_pattern & 0xF);
         gemroc.command_words[10] = ((gemroc.TIMING_DLY_FEB3 & 0x3F) << 24) + ((gemroc.TIMING_DLY_FEB2 & 0x3F) << 16) + ((gemroc.TIMING_DLY_FEB1 & 0x3F) << 8) + ((gemroc.TIMING_DLY_FEB0 & 0x3F) << 0);   }       
         gemroc.command_words[11] = ((gemroc.gemroc_cmd_code & 0xF) << 11);   }

void gemroc_extract_parameters_from_UDP_packet()
{
      printf ( "\n   OVT_LIMIT_FEB3 = %04X %d",((gemroc.command_words[1] >> 22) &  0xFF), ((gemroc.command_words[1] >> 22) &  0xFF));
      printf ( "\n D_OVV_LIMIT_FEB3 = %04X %d ",((gemroc.command_words[1] >> 13) &  0x1FF), ((gemroc.command_words[1] >> 13) &  0x1FF) );
      printf ( "\n D_OVC_LIMIT_FEB3 = %04X %d",((gemroc.command_words[1] >> 4) &  0x1FF), ((gemroc.command_words[1] >> 4) &  0x1FF) );
      printf ( "\n   OVT_LIMIT_FEB2 = %04X %d",((gemroc.command_words[2] >> 22) &  0xFF), ((gemroc.command_words[2] >> 22) &  0xFF) );
      printf ( "\n D_OVV_LIMIT_FEB2 = %04X %d ",((gemroc.command_words[2] >> 13) &  0x1FF), ((gemroc.command_words[2] >> 13) &  0x1FF) );
      printf ( "\n D_OVC_LIMIT_FEB2 = %04X %d",((gemroc.command_words[2] >> 4) &  0x1FF), ((gemroc.command_words[2] >> 4) &  0x1FF) );
      printf ( "\n   OVT_LIMIT_FEB1 = %04X %d", ((gemroc.command_words[3] >> 22) &  0xFF), ((gemroc.command_words[3] >> 22) &  0xFF) );
      printf ( "\n D_OVV_LIMIT_FEB1 = %04X %d ", ((gemroc.command_words[3] >> 13) &  0x1FF), ((gemroc.command_words[3] >> 13) &  0x1FF) );
      printf ( "\n D_OVC_LIMIT_FEB1 = %04X %d", ((gemroc.command_words[3] >> 4) &  0x1FF), ((gemroc.command_words[3] >> 4) &  0x1FF) );
      printf ( "\n   OVT_LIMIT_FEB0 = %04X %d",((gemroc.command_words[4] >> 22) &  0xFF), ((gemroc.command_words[4] >> 22) &  0xFF) );
      printf ( "\n D_OVV_LIMIT_FEB0 = %04X %d ", ((gemroc.command_words[4] >> 13) &  0x1FF), ((gemroc.command_words[4] >> 13) &  0x1FF) );
      printf ( "\n D_OVC_LIMIT_FEB0 = %04X %d", ((gemroc.command_words[4] >> 4) &  0x1FF), ((gemroc.command_words[4] >> 4) &  0x1FF) );
      printf ( "\n A_OVV_LIMIT_FEB3 = %04X %d ", ((gemroc.command_words[5] >> 13) &  0x1FF), ((gemroc.command_words[5] >> 13) &  0x1FF) );
      printf ( "\n A_OVC_LIMIT_FEB3 = %04X %d",((gemroc.command_words[5] >> 4) &  0x1FF), ((gemroc.command_words[5] >> 4) &  0x1FF) );
      printf ( "\n A_OVV_LIMIT_FEB2 = %04X %d ", ((gemroc.command_words[6] >> 13) &  0x1FF), ((gemroc.command_words[6] >> 13) &  0x1FF) );
      printf ( "\n A_OVC_LIMIT_FEB2 = %04X %d",((gemroc.command_words[6] >> 4) &  0x1FF), ((gemroc.command_words[6] >> 4) &  0x1FF) );
      printf ( "\n A_OVV_LIMIT_FEB1 = %04X %d ", ((gemroc.command_words[7] >> 13) &  0x1FF), ((gemroc.command_words[7] >> 13) &  0x1FF) );
      printf ( "\n A_OVC_LIMIT_FEB1 = %04X %d",((gemroc.command_words[7] >> 4) &  0x1FF), ((gemroc.command_words[7] >> 4) &  0x1FF) );
      printf ( "\n A_OVV_LIMIT_FEB0 = %04X %d ", ((gemroc.command_words[8] >> 13) &  0x1FF), ((gemroc.command_words[8] >> 13) &  0x1FF) );
      printf ( "\n A_OVC_LIMIT_FEB0 = %04X %d",((gemroc.command_words[8] >> 4) &  0x1FF), ((gemroc.command_words[8] >> 4) &  0x1FF) );
      printf ( "\n ROC_OVT_LIMIT = %04X %d",((gemroc.command_words[9] >> 24) &  0x3F), ((gemroc.command_words[9] >> 24) &  0x3F) );
      printf ( "\n XCVR_LPBCK_TST_EN = %d",(gemroc.command_words[9] >> 18) &  0x1);
      printf ( "\n ROC_OVT_EN = %d",(gemroc.command_words[9] >> 16) &  0x1);
      printf ( "\n FEB_OVT_EN_pattern = %01X %d",((gemroc.command_words[9] >> 12) &  0xF), ((gemroc.command_words[9] >> 12) &  0xF) );
      printf ( "\n FEB_OVV_EN_pattern = %01X %d",((gemroc.command_words[9] >> 8) &  0xF), ((gemroc.command_words[9] >> 8) &  0xF) );
      printf ( "\n FEB_OVC_EN_pattern = %01X %d", ((gemroc.command_words[9] >> 4) &  0xF), ((gemroc.command_words[9] >> 4) &  0xF) );
      printf ( "\n FEB_PWR_EN_pattern = %01X %d", ((gemroc.command_words[9] >> 0) &  0xF), ((gemroc.command_words[9] >> 0) &  0xF) );
      printf ( "\n TIMING_DLY_FEB3 = %02X %d", ((gemroc.command_words[10] >> 24) &  0x3F), ((gemroc.command_words[10] >> 24) &  0x3F) );
      printf ( "\n TIMING_DLY_FEB2 = %02X %d",((gemroc.command_words[10] >> 16) &  0x3F), ((gemroc.command_words[10] >> 16) &  0x3F) );
      printf ( "\n TIMING_DLY_FEB1 = %02X %d", ((gemroc.command_words[10] >>  8) &  0x3F), ((gemroc.command_words[10] >>  8) &  0x3F) );
      printf ( "\n TIMING_DLY_FEB0 = %02X %d",((gemroc.command_words[10] >>  0) &  0x3F), ((gemroc.command_words[10] >>  0) &  0x3F) ) ;}




























void ch_reg_init(unsigned int TARGET_GEMROC_ID_param,char* cfg_filename_param)
{
      unsigned int header_tag; 
      unsigned int Channel_cfg_cmd_ID; 
      unsigned int ccfg_cmd_tag; 
      unsigned int ccfg_cmd_word_count; 
      ch_reg.TARGET_GEMROC_ID = TARGET_GEMROC_ID_param ;
      ch_reg.cfg_filename = cfg_filename_param;
      readfromtxt(ch_reg.cfg_filename,ch_reg.parameter_array,30);
      write_array_to_txt("ch_reg.init",ch_reg.parameter_array,30);
      ch_reg.DisableHyst = ch_reg.parameter_array [0]  ;
      ch_reg.T2Hyst = ch_reg.parameter_array [1]  ;
      ch_reg.T1Hyst = ch_reg.parameter_array [2]  ;
      ch_reg.Ch63ObufMSB = ch_reg.parameter_array [3]  ;
      ch_reg.TP_disable_FE = ch_reg.parameter_array [4]  ;
      ch_reg.TDC_IB_E = ch_reg.parameter_array [5]  ;
      ch_reg.TDC_IB_T = ch_reg.parameter_array [6]  ;
      ch_reg.Integ = ch_reg.parameter_array [7]  ;
      ch_reg.PostAmpGain = ch_reg.parameter_array [8]  ;
      ch_reg.FE_delay = ch_reg.parameter_array [9]  ;
      ch_reg.Vth_T2 = ch_reg.parameter_array [10]  ;
      ch_reg.Vth_T1 = ch_reg.parameter_array [11]  ;
      ch_reg.QTx2Enable = ch_reg.parameter_array [12]  ;
      ch_reg.MaxIntegTime = ch_reg.parameter_array [13]  ;
      ch_reg.MinIntegTime = ch_reg.parameter_array [14]  ;
      ch_reg.TriggerBLatched = ch_reg.parameter_array [15]  ;
      ch_reg.QdcMode = ch_reg.parameter_array [16]  ;
      ch_reg.BranchEnableT = ch_reg.parameter_array [17]  ;
      ch_reg.BranchEnableEQ = ch_reg.parameter_array [18]  ;
      ch_reg.TriggerMode2B = ch_reg.parameter_array [19]  ;
      ch_reg.TriggerMode2Q = ch_reg.parameter_array [20]  ;
      ch_reg.TriggerMode2E = ch_reg.parameter_array [21]  ;
      ch_reg.TriggerMode2T = ch_reg.parameter_array [22]  ;
      ch_reg.TACMinAge = ch_reg.parameter_array [23]  ;
      ch_reg.TACMaxAge = ch_reg.parameter_array [24]  ;
      ch_reg.CounterMode = ch_reg.parameter_array [25]  ;
      ch_reg.DeadTime = ch_reg.parameter_array [26]  ;
      ch_reg.SyncChainLen = ch_reg.parameter_array [27]  ;
      ch_reg.Ch_DebugMode = ch_reg.parameter_array [28]  ;
      ch_reg.TriggerMode = ch_reg.parameter_array [29];
      ch_reg.is_a_write = 0x1;
      ch_reg.target_TIGER_ID = 0x1;
      ch_reg.command_code = 0x9;
      ch_reg.TO_ALL_enable = 0x1;
      ch_reg.channel_ID = 0x3;
      header_tag = 0x8 << 28;
      Channel_cfg_cmd_ID = 0xE ;
      ccfg_cmd_tag = Channel_cfg_cmd_ID << 24;
      ccfg_cmd_word_count = 9 ;
      ch_reg.command_words[0] = header_tag + (ch_reg.TARGET_GEMROC_ID << 16) + ccfg_cmd_tag + ccfg_cmd_word_count;
      ch_reg.command_words[1] = ((ch_reg.DisableHyst & 0x1) << 24) + ((ch_reg.T2Hyst & 0x7) << 16) + ((ch_reg.T1Hyst & 0x7) << 8) + ((ch_reg.Ch63ObufMSB & 0x1));
      ch_reg.command_words[2] = ((ch_reg.TP_disable_FE & 0x1) << 24) + ((ch_reg.TDC_IB_E & 0xF) << 16) + ((ch_reg.TDC_IB_T & 0xF) << 8) + ((ch_reg.Integ & 0x1));
      ch_reg.command_words[3] = ((ch_reg.PostAmpGain & 0x3) << 24) + ((ch_reg.FE_delay & 0x1F) << 16) + ((ch_reg.Vth_T2 & 0x3F) << 8) + ((ch_reg.Vth_T1 & 0x3F));
      ch_reg.command_words[4] = ((ch_reg.QTx2Enable & 0x1) << 24) + ((ch_reg.MaxIntegTime & 0x7F) << 16) + ((ch_reg.MinIntegTime & 0x7F) << 8) + ((ch_reg.TriggerBLatched & 0x1));
      ch_reg.command_words[5] = ((ch_reg.QdcMode & 0x1) << 24) + ((ch_reg.BranchEnableT & 0x1) << 16) + ((ch_reg.BranchEnableEQ & 0x1) << 8) + ((ch_reg.TriggerMode2B & 0x7));
      ch_reg.command_words[6] = ((ch_reg.TriggerMode2Q & 0x3) << 24) + ((ch_reg.TriggerMode2E & 0x7) << 16) + ((ch_reg.TriggerMode2T & 0x3) << 8) + ((ch_reg.TACMinAge & 0x1F));
      ch_reg.command_words[7] = ((ch_reg.TACMaxAge & 0x1F) << 24) + ((ch_reg.CounterMode & 0xF) << 16) + ((ch_reg.DeadTime & 0x3F) << 8) + ((ch_reg.SyncChainLen & 0x3));
      ch_reg.command_words[8] = ((ch_reg.Ch_DebugMode & 0x3) << 24) + ((ch_reg.TriggerMode & 0x3) << 16);
      ch_reg.command_words[9] = ((ch_reg.command_code & 0xF) << 11) + ((ch_reg.target_TIGER_ID & 0x7) << 8) + ((ch_reg.is_a_write & 0x1) << 7) + ((ch_reg.TO_ALL_enable & 0x1) << 6) + (ch_reg.channel_ID & 0x3F);   }


void ch_reg_reload_chcfg_settings_from_file(char* ChCFGReg_def_fname_param)
{
      readfromtxt(ChCFGReg_def_fname_param,ch_reg.parameter_array,30);
      write_array_to_txt("ch_reg reload",ch_reg.parameter_array,30);
      ch_reg.DisableHyst = ch_reg.parameter_array [0]  ;
      ch_reg.T2Hyst = ch_reg.parameter_array [1]  ;
      ch_reg.T1Hyst = ch_reg.parameter_array [2]  ;
      ch_reg.Ch63ObufMSB = ch_reg.parameter_array [3]  ;
      ch_reg.TP_disable_FE = ch_reg.parameter_array [4]  ;
      ch_reg.TDC_IB_E = ch_reg.parameter_array [5]  ;
      ch_reg.TDC_IB_T = ch_reg.parameter_array [6]  ;
      ch_reg.Integ = ch_reg.parameter_array [7]  ;
      ch_reg.PostAmpGain = ch_reg.parameter_array [8]  ;
      ch_reg.FE_delay = ch_reg.parameter_array [9]  ;
      ch_reg.Vth_T2 = ch_reg.parameter_array [10]  ;
      ch_reg.Vth_T1 = ch_reg.parameter_array [11]  ;
      ch_reg.QTx2Enable = ch_reg.parameter_array [12]  ;
      ch_reg.MaxIntegTime = ch_reg.parameter_array [13]  ;
      ch_reg.MinIntegTime = ch_reg.parameter_array [14]  ;
      ch_reg.TriggerBLatched = ch_reg.parameter_array [15]  ;
      ch_reg.QdcMode = ch_reg.parameter_array [16]  ;
      ch_reg.BranchEnableT = ch_reg.parameter_array [17]  ;
      ch_reg.BranchEnableEQ = ch_reg.parameter_array [18]  ;
      ch_reg.TriggerMode2B = ch_reg.parameter_array [19]  ;
      ch_reg.TriggerMode2Q = ch_reg.parameter_array [20]  ;
      ch_reg.TriggerMode2E = ch_reg.parameter_array [21]  ;
      ch_reg.TriggerMode2T = ch_reg.parameter_array [22]  ;
      ch_reg.TACMinAge = ch_reg.parameter_array [23]  ;
      ch_reg.TACMaxAge = ch_reg.parameter_array [24]  ;
      ch_reg.CounterMode = ch_reg.parameter_array [25]  ;
      ch_reg.DeadTime = ch_reg.parameter_array [26]  ;
      ch_reg.SyncChainLen = ch_reg.parameter_array [27]  ;
      ch_reg.Ch_DebugMode = ch_reg.parameter_array [28]  ;
      ch_reg.TriggerMode = ch_reg.parameter_array [29];
}

  
   unsigned int ch_reg_command_words_array_size()
{
      return 10;  }

void ch_reg_print_command_words()
{    int i;
     for(i=0;i<10;i++)
      printf("%08X\n", ch_reg.command_words[i]);  }

void ch_reg_set_target_GEMROC(unsigned int GEMROC_ID_param)
{
      ch_reg.TARGET_GEMROC_ID = GEMROC_ID_param & 0x1f;
      ch_reg.command_words[0] = (ch_reg.command_words[0] & 0xFF00FFFF) + (ch_reg.TARGET_GEMROC_ID << 16);  }


void ch_reg_set_target_TIGER(unsigned int target_TIGER_param)
{
      ch_reg.target_TIGER_ID = target_TIGER_param & 0x7;
      ch_reg.command_words[9] = ((ch_reg.command_code & 0xF) << 12) + ((ch_reg.target_TIGER_ID & 0x7) << 8) + ((ch_reg.is_a_write & 0x1) << 7) + ((ch_reg.TO_ALL_enable & 0x1) << 6) + (ch_reg.channel_ID & 0x1F);  }

void ch_reg_set_target_channel(unsigned int target_channel_param)
{
      ch_reg.channel_ID = target_channel_param & 0x3F;
      ch_reg.command_words[9] = ((ch_reg.command_code & 0xF) << 12) + ((ch_reg.target_TIGER_ID & 0x7) << 8) + ((ch_reg.is_a_write & 0x1) << 7) + ((ch_reg.TO_ALL_enable & 0x1) << 6) + (ch_reg.channel_ID & 0x3F);   }

void ch_reg_set_Vth_T1(unsigned int Vth_T1_param)
{
      ch_reg.Vth_T1 = Vth_T1_param & 0x3F;  }


void ch_reg_set_to_ALL_param(unsigned int to_ALL_param)
{
      ch_reg.TO_ALL_enable = to_ALL_param & 0x1;
      ch_reg.command_words[9] = ((ch_reg.command_code & 0xF) << 12) + ((ch_reg.target_TIGER_ID & 0x7) << 8) + ((ch_reg.is_a_write & 0x1) << 7) + ((ch_reg.TO_ALL_enable & 0x1) << 6) + (ch_reg.channel_ID & 0x1F);   }


  
void ch_reg_set_command_code(char*command_code_param)
{
      if (strcmp(command_code_param,"WR")==0){
         ch_reg.command_code = 0x0;
         ch_reg.is_a_write = 1;
         ch_reg.command_words[9] = ((ch_reg.command_code & 0xF) << 12) + ((ch_reg.target_TIGER_ID & 0x7) << 8) + ((ch_reg.is_a_write & 0x1) << 7) + ((ch_reg.TO_ALL_enable & 0x1) << 6) + (ch_reg.channel_ID & 0x1F); }
      else if (strcmp(command_code_param,"RD")==0){
         ch_reg.command_code = 0x1;
         ch_reg.is_a_write = 0;
         ch_reg.command_words[9] = ((ch_reg.command_code & 0xF) << 12) + ((ch_reg.target_TIGER_ID & 0x7) << 8) + ((ch_reg.is_a_write & 0x1) << 7) + ((ch_reg.TO_ALL_enable & 0x1) << 6) + (ch_reg.channel_ID & 0x1F);  }
      else 
         printf("ch_reg:bad command_code parameter passed");
   }


void ch_reg_set_TP_disable_FE(unsigned int TP_disable_FE_param)
{
      ch_reg.TP_disable_FE = TP_disable_FE_param & 0x1;}


void ch_reg_update_command_words()
{     int i;
      if ( (ch_reg.command_code & 0xF) == 0x1 )
       for(i=1;i<9;i++)
         ch_reg.command_words[i]=0;
      else{
         ch_reg.command_words[1] = ((ch_reg.DisableHyst & 0x1) << 24)   + ((ch_reg.T2Hyst & 0x7) << 16)        + ((ch_reg.T1Hyst & 0x7) << 8)         + ((ch_reg.Ch63ObufMSB & 0x1));
         ch_reg.command_words[2] = ((ch_reg.TP_disable_FE & 0x1) << 24) + ((ch_reg.TDC_IB_E & 0xF) << 16)      + ((ch_reg.TDC_IB_T & 0xF) << 8)       + ((ch_reg.Integ & 0x1));
         ch_reg.command_words[3] = ((ch_reg.PostAmpGain & 0x3) << 24)   + ((ch_reg.FE_delay & 0x1F) << 16)     + ((ch_reg.Vth_T2 & 0x3F) << 8)        + ((ch_reg.Vth_T1 & 0x3F));
         ch_reg.command_words[4] = ((ch_reg.QTx2Enable & 0x1) << 24)    + ((ch_reg.MaxIntegTime & 0x7F) << 16) + ((ch_reg.MinIntegTime & 0x7F) << 8)  + ((ch_reg.TriggerBLatched & 0x1));
         ch_reg.command_words[5] = ((ch_reg.QdcMode & 0x1) << 24)       + ((ch_reg.BranchEnableT & 0x1) << 16) + ((ch_reg.BranchEnableEQ & 0x1) << 8) + ((ch_reg.TriggerMode2B & 0x7));
         ch_reg.command_words[6] = ((ch_reg.TriggerMode2Q & 0x3) << 24) + ((ch_reg.TriggerMode2E & 0x7) << 16) + ((ch_reg.TriggerMode2T & 0x3) << 8)  + ((ch_reg.TACMinAge & 0x1F));
         ch_reg.command_words[7] = ((ch_reg.TACMaxAge & 0x1F) << 24)    + ((ch_reg.CounterMode & 0xF) << 16)   + ((ch_reg.DeadTime & 0x3F) << 8)      + ((ch_reg.SyncChainLen & 0x3));
         ch_reg.command_words[8] = ((ch_reg.Ch_DebugMode & 0x3) << 24)  + ((ch_reg.TriggerMode & 0x3) << 16);  }
      ch_reg.command_words[9] = ((ch_reg.command_code & 0xF) << command_code_shift) + ((ch_reg.target_TIGER_ID & 0x7) << target_TIGER_ID_shift) + ((ch_reg.TO_ALL_enable & 0x1) << 6) + (ch_reg.channel_ID & 0x3F);    }


void ch_reg_extract_parameters_from_UDP_packet()
{
      printf("\nList of parameters sent to TIGER%d:",ch_reg.target_TIGER_ID);
      printf("\ncmd_word8: DisableHyst:%02X;\t\tT2Hyst:%02X;\t\tT1Hyst:%02X;\t\tCh63ObufMSB:%02X;", ((ch_reg.command_words[1]>>24)&0x1), ((ch_reg.command_words[1]>>16)&0x7),  ((ch_reg.command_words[1]>>8)&0x7),  ((ch_reg.command_words[1]>>0)&0x1)  ) ;
      printf ("\ncmd_word7: TP_disable_FE:%02X;\t\tTDC_IB_E:%02X;\t\tTDC_IB_T:%02X;\t\tInteg:%02X;",((ch_reg.command_words[2]>>24)&0x1), ((ch_reg.command_words[2]>>16)&0xF),  ((ch_reg.command_words[2]>>8)&0xF),  ((ch_reg.command_words[2]>>0)&0x1)  ) ;
      printf ("\ncmd_word6: PostAmpGain:%02X;\t\tFE_delay:%02X;\t\tVth_T2:%02X;\t\tVth_T1:%02X;",((ch_reg.command_words[3]>>24)&0x3), ((ch_reg.command_words[3]>>16)&0x1F), ((ch_reg.command_words[3]>>8)&0x3F), ((ch_reg.command_words[3]>>0)&0x3F) ) ;
      printf("\ncmd_word5: QTx2Enable:%02X;\t\tMaxIntegTime:%02X;\t\tMinIntegTime:%02X;\t\tTriggerBLatched:%02X;",((ch_reg.command_words[4]>>24)&0x1), ((ch_reg.command_words[4]>>16)&0x7F), ((ch_reg.command_words[4]>>8)&0x7F), ((ch_reg.command_words[4]>>0)&0x1)  ) ;
      printf ("\ncmd_word4: QdcMode:%02X;\t\tBranchEnableT:%02X;\t\tBranchEnableEQ:%02X;\t\tTriggerMode2B:%02X;",((ch_reg.command_words[5]>>24)&0x1), ((ch_reg.command_words[5]>>16)&0x1),  ((ch_reg.command_words[5]>>8)&0x1),  ((ch_reg.command_words[5]>>0)&0x7)  );
      printf ("\ncmd_word3: TriggerMode2Q:%02X;\t\tTriggerMode2E:%02X;\t\tTriggerMode2T:%02X;\t\tTACMinAge:%02X;",((ch_reg.command_words[6]>>24)&0x3), ((ch_reg.command_words[6]>>16)&0x7),  ((ch_reg.command_words[6]>>8)&0x3),  ((ch_reg.command_words[6]>>0)&0x1F) );
      printf("\ncmd_word2: TACMaxAge:%02X;\t\tCounterMode:%02X;\t\tDeadTime:%02X;\t\tSyncChainLen:%02X;",((ch_reg.command_words[7]>>24)&0x1F),((ch_reg.command_words[7]>>16)&0xF),  ((ch_reg.command_words[7]>>8)&0x3F), ((ch_reg.command_words[7]>>0)&0x3)  );
      printf("\ncmd_word1: Ch_DebugMode:%02X;\t\tTriggerMode:%02X;",((ch_reg.command_words[8]>>24)&0x3), ((ch_reg.command_words[8]>>16)&0x3));
      printf("\ncmd_word0: command_code:%02X;\t\ttarget_TIGER_ID:%02X;\t\tTO_ALL_enable:%02X;\t\tchannel_ID:%02X;",((ch_reg.command_words[9]>>command_code_shift)&0xF), ((ch_reg.command_words[9]>>target_TIGER_ID_shift)&0x7),  ((ch_reg.command_words[9]>>6)&0x1), ((ch_reg.command_words[9]>>0)&0x3F)  );  }















void g_reg_init(int TARGET_GEMROC_ID_param, char*cfg_filename_param)
{
     unsigned int  header_tag;
     unsigned int  Global_cfg_cmd_ID;
     unsigned int  gcfg_cmd_tag;
     unsigned int  gcfg_cmd_word_count;
      g_reg.TARGET_GEMROC_ID = TARGET_GEMROC_ID_param;
      g_reg.cfg_filename = cfg_filename_param;
      readfromtxt(g_reg.cfg_filename,g_reg.parameter_array,37);
      write_array_to_txt("g_reg.init",g_reg.parameter_array,37);
      g_reg.BufferBias = g_reg.parameter_array [0];
      g_reg.TDCVcasN        = swap_order_N_bits(g_reg.parameter_array [1],4);
      g_reg.TDCVcasP        = swap_order_N_bits(g_reg.parameter_array [2],5);
      g_reg.TDCVcasPHyst    = swap_order_N_bits(g_reg.parameter_array [3],6);
      g_reg.DiscFE_Ibias    = swap_order_N_bits(g_reg.parameter_array [4],6);

      g_reg.BiasFE_PpreN = g_reg.parameter_array [5];
      g_reg.AVcasp_global   = swap_order_N_bits(g_reg.parameter_array [6],5);
      g_reg.TDCcompVcas     = swap_order_N_bits(g_reg.parameter_array [7],4);
      g_reg.TDCIref_cs      = swap_order_N_bits(g_reg.parameter_array [8],5);
      g_reg.DiscVcasN       = swap_order_N_bits(g_reg.parameter_array [9],4);
      g_reg.IntegVb1        = swap_order_N_bits(g_reg.parameter_array [10],6);
      g_reg.BiasFE_A1 = g_reg.parameter_array [11];
      g_reg.Vcasp_Vth       = swap_order_N_bits(g_reg.parameter_array [12],6);
      g_reg.TAC_I_LSB = g_reg.parameter_array [13];
      g_reg.TDCcompVbias = g_reg.parameter_array [14] ;

      g_reg.Vref_Integ      = swap_order_N_bits(g_reg.parameter_array [15],6) ;
      g_reg.IBiasTPcal      = swap_order_N_bits(g_reg.parameter_array [16],5) ;
      g_reg.TP_Vcal         = swap_order_N_bits(g_reg.parameter_array [17],5) ;
      g_reg.ShaperIbias = g_reg.parameter_array [18] ;
      g_reg.IPostamp        = swap_order_N_bits(g_reg.parameter_array [19],5) ;

      g_reg.TP_Vcal_ref     = swap_order_N_bits(g_reg.parameter_array [20],5) ;
      g_reg.Vref_integ_diff = swap_order_N_bits(g_reg.parameter_array [21],6) ;
      g_reg.Sig_pol = g_reg.parameter_array [22] ;
      g_reg.FE_TPEnable = g_reg.parameter_array [23];
      g_reg.DataClkDiv = g_reg.parameter_array [24] ;
      g_reg.TACrefreshPeriod = g_reg.parameter_array [25] ;
      g_reg.TACrefreshEnable = g_reg.parameter_array [26] ;
      g_reg.CounterPeriod = g_reg.parameter_array [27] ; 
      g_reg.CounterEnable = g_reg.parameter_array [28] ;

      g_reg.StopRampEnable = g_reg.parameter_array [29] ;
      g_reg.RClkEnable = g_reg.parameter_array [30] ;
      g_reg.TDCClkdiv = g_reg.parameter_array [31] ;
      g_reg.VetoMode = g_reg.parameter_array [32] ;
      g_reg.Ch_DebugMode = g_reg.parameter_array [33];
      g_reg.TxMode = g_reg.parameter_array [34] ;
      g_reg.TxDDR = g_reg.parameter_array [35] ; 
      g_reg.TxLinks = g_reg.parameter_array [36];
      g_reg.is_a_write = 0x1;
      g_reg.target_TIGER_ID = 0x1;
      g_reg.command_code = 0x9;
      header_tag = 0x8 << 28;
      Global_cfg_cmd_ID = 0xF;
      gcfg_cmd_tag = Global_cfg_cmd_ID << 24;
      gcfg_cmd_word_count = 11;
      g_reg.command_words[0] = header_tag + (g_reg.TARGET_GEMROC_ID << 16) + gcfg_cmd_tag + gcfg_cmd_word_count;
      g_reg.command_words[1] = ((g_reg.BufferBias & 0x3) << 24) + ((g_reg.TDCVcasN & 0xF) << 16) + ((g_reg.TDCVcasP & 0x1F) << 8) + ((g_reg.TDCVcasPHyst & 0x3F));
      g_reg.command_words[2] = ((g_reg.DiscFE_Ibias & 0x3f) << 24) + ((g_reg.BiasFE_PpreN & 0x3F) << 16) + ((g_reg.AVcasp_global & 0x1F) << 8) + ((g_reg.TDCcompVcas & 0xF));
      g_reg.command_words[3] = ((g_reg.TDCIref_cs & 0x1f) << 24) + ((g_reg.DiscVcasN & 0xF) << 16) + ((g_reg.IntegVb1 & 0x3F) << 8) + ((g_reg.BiasFE_A1 & 0xF));
      g_reg.command_words[4] = ((g_reg.Vcasp_Vth & 0x3f) << 24) + ((g_reg.TAC_I_LSB & 0x1F) << 16) + ((g_reg.TDCcompVbias & 0x1F) << 8) + ((g_reg.Vref_Integ & 0x3F));
      g_reg.command_words[5] = ((g_reg.IBiasTPcal & 0x1f) << 24) + ((g_reg.TP_Vcal & 0x1F) << 16) + ((g_reg.ShaperIbias & 0xF) << 8) + ((g_reg.IPostamp & 0x1F));
      g_reg.command_words[6] = ((g_reg.TP_Vcal_ref & 0x1f) << 24) + ((g_reg.Vref_integ_diff & 0x3F) << 16) + ((g_reg.Sig_pol & 0x1) << 8) + ((g_reg.FE_TPEnable & 0x1));
      g_reg.command_words[7] = ((g_reg.DataClkDiv & 0x3) << 16) + ((g_reg.TACrefreshPeriod & 0xf) << 8) + ((g_reg.TACrefreshEnable & 0x1));
      g_reg.command_words[8] = ((g_reg.CounterPeriod & 0x7) << 24) + ((g_reg.CounterEnable & 0x1) << 16) + ((g_reg.StopRampEnable & 0x3) << 8) + ((g_reg.RClkEnable & 0x1F));
      g_reg.command_words[9] = ((g_reg.TDCClkdiv & 0x1) << 24) + ((g_reg.VetoMode & 0x3F) << 16) + ((g_reg.Ch_DebugMode & 0x1) << 8) + ((g_reg.TxMode & 0x3)) ;
      g_reg.command_words[10] = ((g_reg.TxDDR & 0x1) << 24) + ((g_reg.TxLinks & 0x3) << 16);
      g_reg.command_words[11] = ((g_reg.command_code & 0xF) << command_code_shift) + ((g_reg.target_TIGER_ID & 0x7) << target_TIGER_ID_shift);
}

void g_reg_reload_gcfg_settings_from_file(char* GCFGReg_def_fname_param)
{
      readfromtxt(GCFGReg_def_fname_param,g_reg.parameter_array,37);
      write_array_to_txt("g_reg reload",g_reg.parameter_array,37);
      g_reg.BufferBias = g_reg.parameter_array [0] ;
      g_reg.TDCVcasN        = swap_order_N_bits(g_reg.parameter_array [1],4)  ;
      g_reg.TDCVcasP        = swap_order_N_bits(g_reg.parameter_array [2],5)  ;
      g_reg.TDCVcasPHyst    = swap_order_N_bits(g_reg.parameter_array [3],6)  ;
      g_reg.DiscFE_Ibias    = swap_order_N_bits(g_reg.parameter_array [4],6)  ;
      g_reg.BiasFE_PpreN    = g_reg.parameter_array [5] ;
      g_reg.AVcasp_global   = swap_order_N_bits(g_reg.parameter_array [6],5)  ;
      g_reg.TDCcompVcas     = swap_order_N_bits(g_reg.parameter_array [7],4)  ;
      g_reg.TDCIref_cs      = swap_order_N_bits(g_reg.parameter_array [8],5)  ;
      g_reg.DiscVcasN       = swap_order_N_bits(g_reg.parameter_array [9],4)  ;
      g_reg.IntegVb1        = swap_order_N_bits(g_reg.parameter_array [10],6) ;
      g_reg.BiasFE_A1 = g_reg.parameter_array [11] ;
      g_reg.Vcasp_Vth       = swap_order_N_bits(g_reg.parameter_array [12],6) ;
      g_reg.TAC_I_LSB = g_reg.parameter_array [13] ;
      g_reg.TDCcompVbias = g_reg.parameter_array [14] ;
      g_reg.Vref_Integ      = swap_order_N_bits(g_reg.parameter_array [15],6) ;
      g_reg.IBiasTPcal      = swap_order_N_bits(g_reg.parameter_array [16],5) ;
      g_reg.TP_Vcal         = swap_order_N_bits(g_reg.parameter_array [17],5) ;
      g_reg.ShaperIbias = g_reg.parameter_array [18] ;
      g_reg.IPostamp        = swap_order_N_bits(g_reg.parameter_array [19],5) ;
      g_reg.TP_Vcal_ref     = swap_order_N_bits(g_reg.parameter_array [20],5) ;
      g_reg.Vref_integ_diff = swap_order_N_bits(g_reg.parameter_array [21],6) ;
      g_reg.Sig_pol = g_reg.parameter_array [22] ;
      g_reg.FE_TPEnable = g_reg.parameter_array [23] ;
      g_reg.DataClkDiv = g_reg.parameter_array [24] ;
      g_reg.TACrefreshPeriod = g_reg.parameter_array [25] ;
      g_reg.TACrefreshEnable = g_reg.parameter_array [26] ;
      g_reg.CounterPeriod = g_reg.parameter_array [27] ;
      g_reg.CounterEnable = g_reg.parameter_array [28] ;
      g_reg.StopRampEnable = g_reg.parameter_array [29] ;
      g_reg.RClkEnable = g_reg.parameter_array [30] ;
      g_reg.TDCClkdiv = g_reg.parameter_array [31] ;
      g_reg.VetoMode = g_reg.parameter_array [32] ;
      g_reg.Ch_DebugMode = g_reg.parameter_array [33] ;
      g_reg.TxMode = g_reg.parameter_array [34] ;
      g_reg.TxDDR = g_reg.parameter_array [35] ;
      g_reg.TxLinks = g_reg.parameter_array [36];    }

unsigned int g_reg_command_words_array_size()
{
  return 12;  }

void g_reg_print_command_words()
{   unsigned int i;
    for(i=0; i<12;i++)
        printf("%08X\n",g_reg.command_words[i]);   }

void g_reg_set_target_GEMROC(unsigned int GEMROC_ID_param)
{
      g_reg.TARGET_GEMROC_ID = GEMROC_ID_param & 0x1f;
      g_reg.command_words[0] = (g_reg.command_words[0] & 0xFF00FFFF) + (g_reg.TARGET_GEMROC_ID << 16);   }

void g_reg_set_target_TIGER(unsigned int target_TIGER_param)
{
      g_reg.target_TIGER_ID = target_TIGER_param & 0x7;
      g_reg.command_words[11] = ((g_reg.command_code & 0xF) << command_code_shift) + ((g_reg.target_TIGER_ID & 0x7) << target_TIGER_ID_shift);
           }

void g_reg_set_FE_TPEnable(FE_TPEnable_param)
{
  g_reg.FE_TPEnable = FE_TPEnable_param & 0x1;  }


void g_reg_set_AVcasp_global(unsigned int AVcasp_global_param)
{
      g_reg.AVcasp_global = AVcasp_global_param & 0x1F;  }


void g_reg_set_command_code(char* command_code_param)
{
      if (strcmp(command_code_param,"WR")==0){
         g_reg.command_code = 0x8;
         g_reg.is_a_write = 1;
         g_reg.command_words[11] = ((g_reg.command_code & 0xF) << command_code_shift) + ((g_reg.target_TIGER_ID & 0x7) << target_TIGER_ID_shift);    }
      else if(strcmp(command_code_param,"RD")==0){
         g_reg.command_code = 0x9;
         g_reg.is_a_write = 0;
         g_reg.command_words[11] = ((g_reg.command_code & 0xF) << command_code_shift) + ((g_reg.target_TIGER_ID & 0x7) << target_TIGER_ID_shift);  }
      else
         printf("bad command_code parameter passed\n");   }


void g_reg_update_command_words()
{     int i;
      if ( (g_reg.command_code & 0xF) == 0x9 )
        for(i=1;i<11;i++)
          g_reg.command_words[i]=0;
      else {
         g_reg.command_words[1] = ((g_reg.BufferBias & 0x3) << 24)+ ((g_reg.TDCVcasN & 0xF) << 16) + ((g_reg.TDCVcasP & 0x1F) << 8) + ((g_reg.TDCVcasPHyst & 0x3F));
         g_reg.command_words[2] =((g_reg.DiscFE_Ibias & 0x3f) << 24) + ((g_reg.BiasFE_PpreN & 0x3F) << 16) + ((g_reg.AVcasp_global & 0x1F) << 8)   + ((g_reg.TDCcompVcas & 0xF));
         g_reg.command_words[3] =((g_reg.TDCIref_cs & 0x1f) << 24) + ((g_reg.DiscVcasN & 0xF) << 16) + ((g_reg.IntegVb1 & 0x3F) << 8)  + ((g_reg.BiasFE_A1 & 0xF));
         g_reg.command_words[4] =  ((g_reg.Vcasp_Vth & 0x3f) << 24) + ((g_reg.TAC_I_LSB & 0x1F) << 16) + ((g_reg.TDCcompVbias & 0x1F) << 8) + ((g_reg.Vref_Integ & 0x3F));
         g_reg.command_words[5] =  ((g_reg.IBiasTPcal & 0x1f) << 24) + ((g_reg.TP_Vcal & 0x1F) << 16) + ((g_reg.ShaperIbias & 0xF) << 8)      + ((g_reg.IPostamp & 0x1F));
         g_reg.command_words[6] =  ((g_reg.TP_Vcal_ref & 0x1f) << 24) + ((g_reg.Vref_integ_diff & 0x3F) << 16) + ((g_reg.Sig_pol & 0x1) << 8)          + ((g_reg.FE_TPEnable & 0x1));
         g_reg.command_words[7] = ((g_reg.DataClkDiv & 0x3) << 16) + ((g_reg.TACrefreshPeriod & 0xf) << 8) + ((g_reg.TACrefreshEnable & 0x1));
         g_reg.command_words[8] = ((g_reg.CounterPeriod & 0x7) << 24) + ((g_reg.CounterEnable & 0x1) << 16) + ((g_reg.StopRampEnable & 0x3) << 8)   + ((g_reg.RClkEnable & 0x1F));
         g_reg.command_words[9] =  ((g_reg.TDCClkdiv & 0x1) << 24) + ((g_reg.VetoMode & 0x3F) << 16) + ((g_reg.Ch_DebugMode & 0x1) << 8) + ((g_reg.TxMode & 0x3));
         g_reg.command_words[10] =  ((g_reg.TxDDR & 0x1) << 24) + ((g_reg.TxLinks & 0x3) << 16); }
      g_reg.command_words[11] = ((g_reg.command_code & 0xF) << command_code_shift) + ((g_reg.target_TIGER_ID & 0x7) << target_TIGER_ID_shift);     
}



void g_reg_extract_parameters_from_UDP_packet()
{
      printf("\nList of parameters sent to TIGER%d:",g_reg.target_TIGER_ID);
      printf("\nBufferBias:%02X;\t\tTDCVcasN:%02X;\t\tTDCVcasP:%02X;\t\tTDCVcasPHyst:%02X;",((g_reg.command_words[1]>>24)&0x3), swap_order_N_bits(((g_reg.command_words[1]>>16)&0xF),4), swap_order_N_bits(((g_reg.command_words[1]>>8)&0x1F),5), swap_order_N_bits(((g_reg.command_words[1]>>0)&0x3F),6) );
      printf("\nDiscFE_Ibias:%02X;\tBiasFE_PpreN:%02X;\tAVcasp_global:%02X;\tTDCcompVcas:%02X;",swap_order_N_bits(((g_reg.command_words[2]>>24)&0x3F),6), ((g_reg.command_words[2]>>16)&0x3F), swap_order_N_bits(((g_reg.command_words[2]>>8)&0x1F),5), swap_order_N_bits(((g_reg.command_words[2]>>0)&0xF),4) );
      printf( "\nTDCIref_cs:%02X;\t\tDiscVcasN:%02X;\t\tIntegVb1:%02X;\t\tBiasFE_A1:%02X;", swap_order_N_bits(((g_reg.command_words[3]>>24)&0x1F),5), swap_order_N_bits(((g_reg.command_words[3]>>16)&0xF),4), swap_order_N_bits(((g_reg.command_words[3]>>8)&0x3F),6), ((g_reg.command_words[3]>>0)&0xF) ); 
      printf( "\nVcasp_Vth:%02X;\t\tTAC_I_LSB:%02X;\t\tTDCcompVbias:%02X;\tVref_Integ:%02X;", swap_order_N_bits(((g_reg.command_words[4]>>24)&0x3F),6), ((g_reg.command_words[4]>>16)&0x1F), ((g_reg.command_words[4]>>8)&0x1F), swap_order_N_bits(((g_reg.command_words[4]>>0)&0x3F),6) ) ;
      printf ( "\nIBiasTPcal:%02X;\t\tTP_Vcal:%02X;\t\tShaperIbias:%02X;\t\tIPostamp:%02X;", swap_order_N_bits(((g_reg.command_words[5]>>24)&0x1F),5), swap_order_N_bits(((g_reg.command_words[5]>>16)&0x1F),5), ((g_reg.command_words[5]>>8)&0xF), swap_order_N_bits(((g_reg.command_words[5]>>0)&0x1F),5) ) ;
      printf( "\nTP_Vcal_ref:%02X;\t\tVref_integ_diff:%02X;\tSig_pol:%02X;\t\tFE_TPEnable:%02X;" , swap_order_N_bits(((g_reg.command_words[6]>>24)&0x1F),5), swap_order_N_bits(((g_reg.command_words[6]>>16)&0x3F),6), ((g_reg.command_words[6]>>8)&0x1), ((g_reg.command_words[6]>>0)&0x1) ) ;
      printf ( "\nDataClkDiv:%02X;\t\tTACrefreshPeriod:%02X;\tTACrefreshEnable:%02X;", ((g_reg.command_words[7]>>16)&0x3), ((g_reg.command_words[7]>>8)&0xF), ((g_reg.command_words[7]>>0)&0x1) ) ;
      printf ( "\nCounterPeriod:%02X;\tCounterEnable:%02X;\tStopRampEnable:%02X;\tRClkEnable:%02X;" , ((g_reg.command_words[8]>>24)&0x7), ((g_reg.command_words[8]>>16)&0x1), ((g_reg.command_words[8]>>8)&0x3), ((g_reg.command_words[8]>>0)&0x1F) );
      printf( "\nTDCClkdiv:%02X;\t\tVetoMode:%02X;\t\tCh_DebugMode:%02X;\tTxMode:%02X;" , ((g_reg.command_words[9]>>24)&0x1), ((g_reg.command_words[9]>>16)&0x3F), ((g_reg.command_words[9]>>8)&0x1 ), ((g_reg.command_words[9]>>0)&0x3) ) ;
      printf ( "\nTxDDR:%02X;\t\tTxLinks:%02X;" , ((g_reg.command_words[10]>>24)&0x1), ((g_reg.command_words[10]>>16)&0x3) ) ; }


void g_reg_extract_d_parameters_from_UDP_packet()
{
      printf("\nList of parameters sent to TIGER%d:",g_reg.target_TIGER_ID);
      printf( "\nBufferBias:%d;\t\tTDCVcasN:%d;\t\tTDCVcasP:%d;\t\tTDCVcasPHyst:%d;", ((g_reg.command_words[1]>>24)&0x3), swap_order_N_bits(((g_reg.command_words[1]>>16)&0xF),4), swap_order_N_bits(((g_reg.command_words[1]>>8)&0x1F),5), swap_order_N_bits(((g_reg.command_words[1]>>0)&0x3F),6) ) ;

     printf( "\nDiscFE_Ibias:%d;\tBiasFE_PpreN:%d;\tAVcasp_global:%d;\tTDCcompVcas:%d;", swap_order_N_bits(((g_reg.command_words[2]>>24)&0x3F),6), ((g_reg.command_words[2]>>16)&0x3F), swap_order_N_bits(((g_reg.command_words[2]>>8)&0x1F),5), swap_order_N_bits(((g_reg.command_words[2]>>0)&0xF),4) );
      printf( "\nTDCIref_cs:%d;\t\tDiscVcasN:%d;\t\tIntegVb1:%d;\t\tBiasFE_A1:%d;", swap_order_N_bits(((g_reg.command_words[3]>>24)&0x1F),5), swap_order_N_bits(((g_reg.command_words[3]>>16)&0xF),4), swap_order_N_bits(((g_reg.command_words[3]>>8)&0x3F),6), ((g_reg.command_words[3]>>0)&0xF) );
      printf ( "\nVcasp_Vth:%d;\t\tTAC_I_LSB:%d;\t\tTDCcompVbias:%d;\tVref_Integ:%d;", swap_order_N_bits(((g_reg.command_words[4]>>24)&0x3F),6), ((g_reg.command_words[4]>>16)&0x1F), ((g_reg.command_words[4]>>8)&0x1F), swap_order_N_bits(((g_reg.command_words[4]>>0)&0x3F),6) );
      printf ( "\nIBiasTPcal:%d;\t\tTP_Vcal:%d;\t\tShaperIbias:%d;\t\tIPostamp:%d;",swap_order_N_bits(((g_reg.command_words[5]>>24)&0x1F),5), swap_order_N_bits(((g_reg.command_words[5]>>16)&0x1F),5), ((g_reg.command_words[5]>>8)&0xF), swap_order_N_bits(((g_reg.command_words[5]>>0)&0x1F),5) );
      printf ( "\nTP_Vcal_ref:%d;\t\tVref_integ_diff:%d;\tSig_pol:%d;\t\tFE_TPEnable:%d;", swap_order_N_bits(((g_reg.command_words[6]>>24)&0x1F),5), swap_order_N_bits(((g_reg.command_words[6]>>16)&0x3F),6), ((g_reg.command_words[6]>>8)&0x1), ((g_reg.command_words[6]>>0)&0x1) ) ;
      printf ( "\nDataClkDiv:%d;\t\tTACrefreshPeriod:%d;\tTACrefreshEnable:%d;", ((g_reg.command_words[7]>>16)&0x3), ((g_reg.command_words[7]>>8)&0xF), ((g_reg.command_words[7]>>0)&0x1) ) ;
      printf ( "\nCounterPeriod:%d;\tCounterEnable:%d;\tStopRampEnable:%d;\tRClkEnable:%d;", ((g_reg.command_words[8]>>24)&0x7), ((g_reg.command_words[8]>>16)&0x1), ((g_reg.command_words[8]>>8)&0x3), ((g_reg.command_words[8]>>0)&0x1F) ) ;
      printf ( "\nTDCClkdiv:%d;\t\tVetoMode:%d;\t\tCh_DebugMode:%d;\tTxMode:%d;", ((g_reg.command_words[9]>>24)&0x1), ((g_reg.command_words[9]>>16)&0x3F), ((g_reg.command_words[9]>>8)&0x1 ), ((g_reg.command_words[9]>>0)&0x3) ) ;
      printf ( "\nTxDDR:%d;\t\tTxLinks:%d;", ((g_reg.command_words[10]>>24)&0x1), ((g_reg.command_words[10]>>16)&0x3) )  ;  } 


int main(int argc,char *argv[])
{  
  char line[128],cmd_string[32];
  unsigned int cmd_para[10];
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
   fd_read_para=fopen("auto_input_para.txt","r");
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
    print_cmd("gemroc_init",gemroc.command_words,sizeof(gemroc.command_words));
    gemroc_set_FEB_PWR_EN_pattern(FEB_PWR_EN_pattern);
    print_cmd("gemroc_set_FEB_PWR_EN_pattern",gemroc.command_words,sizeof(gemroc.command_words));
    gemroc_set_gemroc_cmd_code("CMD_GEMROC_LV_CFG_WR",1);
     print_cmd("gemroc_set_gemroc_cmd_code",gemroc.command_words,sizeof(gemroc.command_words));
    gemroc_update_command_words();
    print_cmd("gemroc_update_command_words",gemroc.command_words,sizeof(gemroc.command_words));
    send_GEMROC_CFG_CMD_PKT(GEMROC_ID,"send_GEMROC_CFG_CMD_PKT in line 1560",gemroc.command_words,sizeof(gemroc.command_words),DEST_IP_ADDRESS,DEST_PORT_NO);
sprintf(cfg_filename,"GEMROC_%d_def_cfg_DAQ_2018v3.txt",GEMROC_ID) ;
DAQ_init(GEMROC_ID,"NONE",0,1,0,cfg_filename);
DAQ_extract_parameters_from_UDP_packet();
send_GEMROC_DAQ_CMD(GEMROC_ID, "send_GEMROC_DAQ_CMD,in line 1566","CMD_GEMROC_DAQ_CFG_WR");

g_reg_init(GEMROC_ID,"TIGER_def_g_cfg_2018.txt");
ch_reg_init(GEMROC_ID,"TIGER_def_ch_cfg_2018.txt");

GEMROC_IVT_read_and_log(GEMROC_ID,1, IVT_LOG_ENABLE, "GEMROC_IVT_read_and_log in line 1571");
  while(!feof(fd_read_para))
{  
  memset(cmd_string,0, sizeof(cmd_string));
  memset(line,0, sizeof(line));
  fgets(line, sizeof(line), fd_read_para);
  extract_para_from_line(line,strlen(line),cmd_string,cmd_para);
  if(cmd_para[0]>0)
  {
    if(strcmp(cmd_string,"quit")==0 ||strcmp(cmd_string ,"q")==0||strcmp(cmd_string,"Quit")==0||strcmp(cmd_string,"Q")==0)
    {  
       printf("%s\n",cmd_string);
       break;
    }
    if(strcmp(cmd_string,"PEW")==0 ||strcmp(cmd_string,"P")==0)
      if(cmd_para[0]==2)
       {
        FEBPwrEnPattern_set(GEMROC_ID,cmd_para[1],"FEBPwrEnPattern_set in line 1701");
        printf("%s %d\n",cmd_string,cmd_para[1]);
        }
    if(strcmp(cmd_string,"TDly")==0 ||strcmp(cmd_string,"TD")==0)
      if(cmd_para[0]==5)
       {
        set_FEB_timing_delays(GEMROC_ID,cmd_para[1],cmd_para[2],cmd_para[3],cmd_para[4],"set_FEB_timing_delays in line 1707");
        printf("%s %d %d %d %d\n",cmd_string,cmd_para[1],cmd_para[2],cmd_para[3],cmd_para[4]);
        }
    if(strcmp(cmd_string,"IVT")==0 ||strcmp(cmd_string,"I")==0)
      if(cmd_para[0]==1)
       {GEMROC_IVT_read_and_log(GEMROC_ID,1, IVT_LOG_ENABLE, "GEMROC_IVT_read_and_log in line 1710");
        printf("%s\n",cmd_string);
        sleep(1);  }
    if(strcmp(cmd_string,"OCVTEn")==0)
      if(cmd_para[0]==5)    
      {
        Set_OV_OC_OT_PWR_CUT_EN_FLAGS("OCVTEn",GEMROC_ID,cmd_para[1],cmd_para[2],cmd_para[3],cmd_para[4],"OCVTEn in line 1713");
        GEMROC_IVT_read_and_log(GEMROC_ID,1, IVT_LOG_ENABLE, "OCVTEn in line 1715");
        printf("%s %d %d %d %d\n",cmd_string,cmd_para[1],cmd_para[2],cmd_para[3],cmd_para[4]);
       }
    if(strcmp(cmd_string,"OVVA")==0)
      if(cmd_para[0]==5)
      {
        Set_OVVA_LIMIT("OVVA info",GEMROC_ID,cmd_para[1],cmd_para[2],cmd_para[3],cmd_para[4],"OVVA in line 1719");
        GEMROC_IVT_read_and_log(GEMROC_ID,1, IVT_LOG_ENABLE, "OVVA in line 1720");
        printf("%s %d %d %d %d\n",cmd_string,cmd_para[1],cmd_para[2],cmd_para[3],cmd_para[4]);
      }
    if(strcmp(cmd_string,"OVVD")==0)
      if(cmd_para[0]==5)
      {
        Set_OVVD_LIMIT("OVVD info",GEMROC_ID,cmd_para[1],cmd_para[2],cmd_para[3],cmd_para[4],"OVVD in line 1725");
        GEMROC_IVT_read_and_log(GEMROC_ID,1, IVT_LOG_ENABLE, "OVVD in line 1726");
        printf("%s %d %d %d %d\n",cmd_string,cmd_para[1],cmd_para[2],cmd_para[3],cmd_para[4]);
      }
    if(strcmp(cmd_string,"OVCA")==0)
      if(cmd_para[0]==5)
      {
        Set_OVCA_LIMIT("OVCA info",GEMROC_ID,cmd_para[1],cmd_para[2],cmd_para[3],cmd_para[4],"OVCA in 1731");
        GEMROC_IVT_read_and_log(GEMROC_ID,1, 0, "OVCA in line 1732");
        printf("%s %d %d %d %d\n",cmd_string,cmd_para[1],cmd_para[2],cmd_para[3],cmd_para[4]);
      }
    if(strcmp(cmd_string,"OVCD")==0)
      if(cmd_para[0]==5)
      {
        Set_OVCD_LIMIT("OVCD info",GEMROC_ID,cmd_para[1],cmd_para[2],cmd_para[3],cmd_para[4],"OVCD in 1737");
        GEMROC_IVT_read_and_log(GEMROC_ID,1, 0, "OVCD in line 1738");
        printf("%s %d %d %d %d\n",cmd_string,cmd_para[1],cmd_para[2],cmd_para[3],cmd_para[4]);
      }
    if(strcmp(cmd_string, "OVTF")==0)
      if(cmd_para[0]==5)
      {
        Set_OVTF_LIMIT("OVTF info",GEMROC_ID,cmd_para[1],cmd_para[2],cmd_para[3],cmd_para[4],"OVTF in 1743");
        GEMROC_IVT_read_and_log(GEMROC_ID,1, 0, "OVTF in line 1744");
        printf("%s %d %d %d %d\n",cmd_string,cmd_para[1],cmd_para[2],cmd_para[3],cmd_para[4]);
      }
    if(strcmp(cmd_string,"OVTR")==0)
      if(cmd_para[0]==2)
      {
        set_ROC_OVT_LIMIT("info",GEMROC_ID,cmd_para[1],"OVTR in 1749");
        GEMROC_IVT_read_and_log(GEMROC_ID,1, 0, "OVTR in line 1750");
        printf("%s %d\n",cmd_string,cmd_para[1]);
      }
    if(strcmp(cmd_string,"LVCR")==0)
      if(cmd_para[0]==1)
      {
        Read_GEMROC_LV_CfgReg("info",GEMROC_ID,"LVCR in 1755");
        printf("%s\n",cmd_string);
      }    
    if(strcmp(cmd_string,"DAQCR")==0)
      if(cmd_para[0]==1)
      {
        Read_GEMROC_DAQ_CfgReg("DAQCR",GEMROC_ID,"DAQCR in 1760");
        printf("%s\n",cmd_string);
      }
    if(strcmp(cmd_string,"GRST")==0)
      if(cmd_para[0]==1)
      {
        ResetTgtGEMROC_ALL_TIGER_GCfgReg(GEMROC_ID, "ResetTgtGEMROC_ALL_TIGER_GCfgReg in line 1765");
        printf("%s\n",cmd_string);
      }
    if(strcmp(cmd_string,"GWdef")==0||strcmp(cmd_string,"GW")==0)
      if(cmd_para[0]==2)
      {
      WriteTgtGEMROC_TIGER_GCfgReg_fromfile("GW", GEMROC_ID, cmd_para[1], "TIGER_def_g_cfg_2018.txt", "WriteTgtGEMROC_TIGER_GCfgReg_fromfile in line 1770");
      ReadTgtGEMROC_TIGER_GCfgReg("GW", GEMROC_ID, cmd_para[1], "ReadTgtGEMROC_TIGER_GCfgReg in line 1770" );
      printf("%s %d\n",cmd_string,cmd_para[1]);
      sleep(4);
      }
    if(strcmp(cmd_string,"GRd")==0||strcmp(cmd_string,"GR")==0)
      if(cmd_para[0]==2)
      {
      ReadTgtGEMROC_TIGER_GCfgReg("GRd", GEMROC_ID,cmd_para[1],"GRd in 1777" );
      printf("%s %d\n",cmd_string,cmd_para[1]);
      }
    if(strcmp(cmd_string,"CWdef")==0||strcmp(cmd_string,"CW")==0)
      if(cmd_para[0]==3)
      {
       WriteTgtGEMROC_TIGER_ChCfgReg_fromfile("c_inst", GEMROC_ID,cmd_para[1],cmd_para[2],"TIGER_def_ch_cfg_2018.txt", "WriteTgtGEMROC_TIGER_ChCfgReg_fromfile in line 1782");
       ReadTgtGEMROC_TIGER_ChCfgReg ("c_inst", GEMROC_ID,cmd_para[1],cmd_para[2],0, "ReadTgtGEMROC_TIGER_ChCfgReg in line 1784" ); 
       printf("%s %d %d\n",cmd_string,cmd_para[1],cmd_para[2]);
       sleep(4);
      }     
    if(strcmp(cmd_string,"CRd")==0||strcmp(cmd_string,"CR")==0)
      if(cmd_para[0]==3)
       {
        ReadTgtGEMROC_TIGER_ChCfgReg ("c_inst", GEMROC_ID,cmd_para[1],cmd_para[2],0, "CR in line 1789" );        
        printf("%s %d %d\n",cmd_string,cmd_para[1],cmd_para[2]);
        }
    if(strcmp(cmd_string,"TPEnG")==0)
      if(cmd_para[0]==3)
      {
        set_FE_TPEnable("g_inst", GEMROC_ID,cmd_para[1],cmd_para[2], "set_FE_TPEnable in line 1793");
        printf("%s %d %d\n",cmd_string,cmd_para[1],cmd_para[2]);
        sleep(2);        
      }
    if(strcmp(cmd_string,"TPEW_ch")==0||strcmp(cmd_string, "TP")==0)
      if(cmd_para[0]==5)
      {
        Set_GEMROC_TIGER_ch_TPEn ("c_inst", GEMROC_ID,cmd_para[1],cmd_para[2],cmd_para[3],cmd_para[4],"Set_GEMROC_TIGER_ch_TPEn in line 1799");
        printf("%s %d %d %d %d\n",cmd_string,cmd_para[1],cmd_para[2],cmd_para[3],cmd_para[4]);
        sleep(2);
      }
    if(strcmp(cmd_string,"VT1_ch")==0||strcmp(cmd_string,"VT1")==0)
      if(cmd_para[0]==4)
       {
        Set_Vth_T1 ("VT1",GEMROC_ID,cmd_para[1],cmd_para[2],cmd_para[3],"VT1 in line 1805");
        sleep(2);
        printf("%s %d %d %d\n",cmd_string,cmd_para[1],cmd_para[2],cmd_para[3]);
       }
    if(strcmp(cmd_string,"AVCaspGset")==0||strcmp(cmd_string,"AV")==0)
      if(cmd_para[0]==3)
      {
        set_AVcasp_global("AV",GEMROC_ID,cmd_para[1],cmd_para[2],"AV in 1811");
        printf("%s %d %d\n",cmd_string,cmd_para[1],cmd_para[2]);
        sleep(2);
      }    
    if(strcmp(cmd_string,"SRst")==0)
      if(cmd_para[0]==2)
      {
        if(cmd_para[1]<4)
          SynchReset_to_TgtFEB("SRst in line 1818", GEMROC_ID, cmd_para[1]&0x3, 0 );
        else
          SynchReset_to_TgtFEB("SRst in line 1818", GEMROC_ID, cmd_para[1]&0x3, 1);
        printf("%s %d\n",cmd_string,cmd_para[1]);
        sleep(2);
      }
    if(strcmp(cmd_string,"DRst")==0)
      if(cmd_para[0]==2)
      {
         if(cmd_para[1]<4)
           SynchReset_to_TgtTCAM("SDst in line 1827", GEMROC_ID,cmd_para[1]&0x3,0);
         else
           SynchReset_to_TgtTCAM("SDst in line 1829", GEMROC_ID,cmd_para[1]&0x3,1);
         printf("%s %d\n",cmd_string,cmd_para[1]);
         sleep(2);
      }
    if(strcmp(cmd_string,"DSTART")==0)
      if(cmd_para[0]==3)
        {
        DAQ_set_and_TL_start("DSTART in 1835", GEMROC_ID,cmd_para[1]&0xFF,cmd_para[2]&0xF);
        sleep(2);
        printf("%s %d %d\n",cmd_string,cmd_para[1],cmd_para[2]);
        }
  }   
}


close(socket_descriptor);
close(socket_descriptor1);
fclose(fd_cmd);
fclose(fd_read_para);
fclose(fw);
fclose(file_name);

}
