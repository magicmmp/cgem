# file: GEMconf_module.py
# author: Angelo Cotta Ramusino INFN Ferrara
# date: 27 June 2017
# purpose: a collection of classes handling settings for the global, channel and GEMROC module functional registers
command_code_shift = 11 # acr 2017-08-29
target_TIGER_ID_shift = 8 # acr 2017-08-29

class g_reg_settings: # purpose: organize the Global Configuration Register Settings in an array format which can be sent over Ethernet or optical link
   def __init__(self,
                TARGET_GEMROC_ID_param = 0, # acr 2017-09-22
                cfg_filename_param = "default_g_cfg_mainz.txt" # acr 2017-07-11 
                ):
      self.TARGET_GEMROC_ID = TARGET_GEMROC_ID_param # acr 2017-09-22
      self.cfg_filename = cfg_filename_param
      ## acr 2017-07-12 BEGIN implementing a parameter array loaded from a configuration file
      self.parameter_array = [0 for i in range(38)]
      with open(self.cfg_filename, "r") as f:
         self.parameter_array = map(int, f)
##      0self.BufferBias = self.parameter_array [0] ## BufferBias_param; default 0
##      0self.TDCVcasN = self.parameter_array [1] ## TDCVcasN_param; default 0
##      0self.TDCVcasP = self.parameter_array [2] ## TDCVcasP_param; default 0
##      59self.TDCVcasPHyst = self.parameter_array [3] ## TDCVcasPHyst_param; default 55
##      19self.DiscFE_Ibias = self.parameter_array [4] ## DiscFE_Ibias_param; default 50
##      10self.BiasFE_PpreN = self.parameter_array [5] ## BiasFE_PpreN_param; default 10
##      5self.AVcasp_global = self.parameter_array [6] ## AVcasp_global_param; default 20
##      0self.TDCcompVcas = self.parameter_array [7] ## TDCcompVcas_param; default 0
##      5self.TDCIres = self.parameter_array [8] ## TDCIres_param; default 20
##      13self.DiscVcasN = self.parameter_array [9] ## DiscVcasN_param; default 7
##      54self.IntegVb1 = self.parameter_array [10] ## IntegVb1_param; default 27
##      8self.BiasFE_A1 = self.parameter_array [11] ## BiasFE_A1_param; default 8
##      59self.Vcasp_Vth = self.parameter_array [12] ## Vcasp_Vth_param; default 55
##      0self.TAC_I_LSB = self.parameter_array [13] ## TAC_I_LSB_param; default 0
##      0self.TDCcompVbias = self.parameter_array [14] ## TDCcompVbias_param; default 0
##      59self.Vref_Integ = self.parameter_array [15] ## Vref_Integ_param; default 55
##      19self.IBiasTPcal = self.parameter_array [16] ## IBiasTPcal_param; default 29
##      1self.TP_Vcal = self.parameter_array [17] ## TP_Vcal_param; default 8
##      0self.ShaperIbias = self.parameter_array [18] ## ShaperIbias_param; default 0
##      13self.IPostamp = self.parameter_array [19] ## IPostamp_param; default 26
##      29self.TP_Vcal_ref = self.parameter_array [20] ## TP_Vcal_ref_param; default 23
##      57self.Vref_integ_diff = self.parameter_array [21] ## Vref_integ_diff_param; default 39
##      1self.Sig_pol = self.parameter_array [22] ## Sig_pol_param; default p-type (1?)
##      1self.FE_TPEnable = self.parameter_array [23] ## FE_TPEnable_param; default X (1?)
##      0self.CompactDataFormat = self.parameter_array [24] ## CompactDataFormat_param; default EMPTY BOX (0?)
##      0self.DataClkDiv = self.parameter_array [25] ## DataClkDiv_param; default 0
##      9self.TACrefreshPeriod = self.parameter_array [26] ## TACrefreshPeriod_param; default 9
##      1self.TACrefreshEnable = self.parameter_array [27] ## TACrefreshEnable_param; default X (1?)
##      6self.CounterPeriod = self.parameter_array [28] ## CounterPeriod_param; default 6
##      0self.CounterEnable = self.parameter_array [29] ## CounterEnable_param; default EMPTYBOX (0?)
##      0self.StopRampEnable = self.parameter_array [30] ## StopRampEnable_param; default 0
##      7self.RClkEnable = self.parameter_array [31] ## RClkEnable_param; default 7
##      0self.TDCClkdiv = self.parameter_array [32] ## TDCClkdiv_param; default EMPTYBOX (0?)
##      0self.VetoMode = self.parameter_array [33] ## VetoMode_param; default 0
##      0self.Ch_DebugMode = self.parameter_array [34] ## Ch_DebugMode_param; default EMPTYBOX (0?)
##      2self.TxMode = self.parameter_array [35] ## TxMode_param; default 2
##      1self.TxDDR = self.parameter_array [36] ## TxDDR_param; default X (1?)
##      1self.TxLinks = self.parameter_array [37] ## TxLinks_param; default 2 links nel GUI di Torino; dovrebbe corrispondeere ad un codice 1 per 2 links
      self.BufferBias = self.parameter_array [0] ## BufferBias_param; default 0
      self.TDCVcasN = self.parameter_array [1] ## TDCVcasN_param; default 0
      self.TDCVcasP = self.parameter_array [2] ## TDCVcasP_param; default 0
      self.TDCVcasPHyst = self.parameter_array [3] ## TDCVcasPHyst_param; default 55
      self.DiscFE_Ibias = self.parameter_array [4] ## DiscFE_Ibias_param; default 50
      self.BiasFE_PpreN = self.parameter_array [5] ## BiasFE_PpreN_param; default 10
      self.AVcasp_global = self.parameter_array [6] ## AVcasp_global_param; default 20
      self.TDCcompVcas = self.parameter_array [7] ## TDCcompVcas_param; default 0
      self.TDCIres = self.parameter_array [8] ## TDCIres_param; default 20
      self.DiscVcasN = self.parameter_array [9] ## DiscVcasN_param; default 7
      self.IntegVb1 = self.parameter_array [10] ## IntegVb1_param; default 27
      self.BiasFE_A1 = self.parameter_array [11] ## BiasFE_A1_param; default 8
      self.Vcasp_Vth = self.parameter_array [12] ## Vcasp_Vth_param; default 55
      self.TAC_I_LSB = self.parameter_array [13] ## TAC_I_LSB_param; default 0
      self.TDCcompVbias = self.parameter_array [14] ## TDCcompVbias_param; default 0
      self.Vref_Integ = self.parameter_array [15] ## Vref_Integ_param; default 55
      self.IBiasTPcal = self.parameter_array [16] ## IBiasTPcal_param; default 29
      self.TP_Vcal = self.parameter_array [17] ## TP_Vcal_param; default 8
      self.ShaperIbias = self.parameter_array [18] ## ShaperIbias_param; default 0
      self.IPostamp = self.parameter_array [19] ## IPostamp_param; default 26
      self.TP_Vcal_ref = self.parameter_array [20] ## TP_Vcal_ref_param; default 23
      self.Vref_integ_diff = self.parameter_array [21] ## Vref_integ_diff_param; default 39
      self.Sig_pol = self.parameter_array [22] ## Sig_pol_param; default p-type (1?)
      self.FE_TPEnable = self.parameter_array [23] ## FE_TPEnable_param; default X (1?)
      self.CompactDataFormat = self.parameter_array [24] ## CompactDataFormat_param; default EMPTY BOX (0?)
      self.DataClkDiv = self.parameter_array [25] ## DataClkDiv_param; default 0
      self.TACrefreshPeriod = self.parameter_array [26] ## TACrefreshPeriod_param; default 9
      self.TACrefreshEnable = self.parameter_array [27] ## TACrefreshEnable_param; default X (1?)
      self.CounterPeriod = self.parameter_array [28] ## CounterPeriod_param; default 6
      self.CounterEnable = self.parameter_array [29] ## CounterEnable_param; default EMPTYBOX (0?)
      self.StopRampEnable = self.parameter_array [30] ## StopRampEnable_param; default 0
      self.RClkEnable = self.parameter_array [31] ## RClkEnable_param; default 7
      self.TDCClkdiv = self.parameter_array [32] ## TDCClkdiv_param; default EMPTYBOX (0?)
      self.VetoMode = self.parameter_array [33] ## VetoMode_param; default 0
      self.Ch_DebugMode = self.parameter_array [34] ## Ch_DebugMode_param; default EMPTYBOX (0?)
      self.TxMode = self.parameter_array [35] ## TxMode_param; default 2
      self.TxDDR = self.parameter_array [36] ## TxDDR_param; default X (1?)
      self.TxLinks = self.parameter_array [37] ## TxLinks_param; default 2 links nel GUI di Torino; dovrebbe corrispondeere ad un codice 1 per 2 links
      ## acr 2017-07-12 END   implementing a parameter array loaded from a configuration file
      self.is_a_write = 0x1
      self.target_TIGER_ID = 0x1
      self.command_code = 0x9
      
    ## command codes as defined by Ricardo Bugalho's configurator: CMD_WRCHCFG 0x0; CMD_RDCHCFG = 0x1; CMD_WRGCFG = 0x8; CMD_RDGCFG = 0x9;
      header_tag = 0x8 << 28
      ## command_code_shift = 11 # acr 2017-08-29
      ## target_TIGER_ID_shift = 8 # acr 2017-08-29
      ##trailer_tag = 0x9 << 28
      Global_cfg_cmd_ID = 0xF
      gcfg_cmd_tag = Global_cfg_cmd_ID << 24
      gcfg_cmd_word_count = 11
      self.cmd_header = header_tag + (self.TARGET_GEMROC_ID << 16) + gcfg_cmd_tag + gcfg_cmd_word_count
      ##cmd_trailer = trailer_tag + gcfg_cmd_tag
      self.cmd_word10 = ((self.BufferBias & 0x3) << 24) + ((self.TDCVcasN & 0xF) << 16) + ((self.TDCVcasP & 0x1F) << 8) + ((self.TDCVcasPHyst & 0x3F))  
      self.cmd_word9 = ((self.DiscFE_Ibias & 0x3f) << 24) + ((self.BiasFE_PpreN & 0x3F) << 16) + ((self.AVcasp_global & 0x1F) << 8) + ((self.TDCcompVcas & 0xF))  
      self.cmd_word8 = ((self.TDCIres & 0x1f) << 24) + ((self.DiscVcasN & 0xF) << 16) + ((self.IntegVb1 & 0x3F) << 8) + ((self.BiasFE_A1 & 0xF))  
      self.cmd_word7 = ((self.Vcasp_Vth & 0x3f) << 24) + ((self.TAC_I_LSB & 0x1F) << 16) + ((self.TDCcompVbias & 0x1F) << 8) + ((self.Vref_Integ & 0x3F))  
      self.cmd_word6 = ((self.IBiasTPcal & 0x1f) << 24) + ((self.TP_Vcal & 0x1F) << 16) + ((self.ShaperIbias & 0xF) << 8) + ((self.IPostamp & 0x1F))  
      self.cmd_word5 = ((self.TP_Vcal_ref & 0x1f) << 24) + ((self.Vref_integ_diff & 0x3F) << 16) + ((self.Sig_pol & 0x1) << 8) + ((self.FE_TPEnable & 0x1))  
      self.cmd_word4 = ((self.CompactDataFormat & 0x1) << 24) + ((self.DataClkDiv & 0x3) << 16) + ((self.TACrefreshPeriod & 0xf) << 8) + ((self.TACrefreshEnable & 0x1))  
      self.cmd_word3 = ((self.CounterPeriod & 0x7) << 24) + ((self.CounterEnable & 0x1) << 16) + ((self.StopRampEnable & 0x3) << 8) + ((self.RClkEnable & 0x1F))  
      self.cmd_word2 = ((self.TDCClkdiv & 0x1) << 24) + ((self.VetoMode & 0x3F) << 16) + ((self.Ch_DebugMode & 0x1) << 8) + ((self.TxMode & 0x3))  
      self.cmd_word1 = ((self.TxDDR & 0x1) << 24) + ((self.TxLinks & 0x3) << 16)

      ## acr 2017-08-22 self.cmd_word0 = ((self.command_code & 0xF) << 12) + ((self.target_TIGER_ID & 0x7) << 8) + ((self.is_a_write & 0x1) << 7)
      ## acr 2017-08-29 the WR_nRD bit is evaluated by NIOSII instead of being read from the fiels "is_a_write" self.cmd_word0 = ((self.command_code & 0xF) << 11) + ((self.target_TIGER_ID & 0x7) << 8) + ((self.is_a_write & 0x1) << 7)
      self.cmd_word0 = ((self.command_code & 0xF) << command_code_shift) + ((self.target_TIGER_ID & 0x7) << target_TIGER_ID_shift)
      ## self.cmd_word0 = ((self.command_code & 0xF) << 11) + ((self.target_TIGER_ID & 0x7) << 8) + ((self.is_a_write & 0x1) << 7)
      
      self.command_words = [ self.cmd_header,
                             self.cmd_word10,
                             self.cmd_word9,
                             self.cmd_word8,
                             self.cmd_word7,
                             self.cmd_word6,
                             self.cmd_word5,
                             self.cmd_word4,
                             self.cmd_word3,
                             self.cmd_word2,
                             self.cmd_word1,
                             self.cmd_word0
                              ]

   def __del__(self):
      class_name = self.__class__.__name__
      print class_name, "g_reg_settings destroyed"

   def cmd_words_array_size(self):
      return len(self.command_words)

   def print_command_words(self):
      for i in range (0, len(self.command_words)):
         print '%08X'% self.command_words[i]

##   def set_write_flag(self, write_flag_param):
##      self.is_a_write = write_flag_param & 0x1
##      self.cmd_word0 = ((self.command_code & 0xF) << 12) + ((self.target_TIGER_ID & 0x7) << 8) + ((self.is_a_write & 0x1) << 7)
##      self.command_words[11] = self.cmd_word0

   def set_target_GEMROC(self, GEMROC_ID_param): # acr 2017-09-22
      self.TARGET_GEMROC_ID = GEMROC_ID_param & 0x1f
      self.cmd_header = (self.cmd_header & 0xFF00FFFF) + (self.TARGET_GEMROC_ID << 16)

   def set_target_TIGER(self, target_TIGER_param):
      self.target_TIGER_ID = target_TIGER_param & 0x7
      ## acr 2017-08-29 self.cmd_word0 = ((self.command_code & 0xF) << 12) + ((self.target_TIGER_ID & 0x7) << 8) + ((self.is_a_write & 0x1) << 7)
      self.cmd_word0 = ((self.command_code & 0xF) << command_code_shift) + ((self.target_TIGER_ID & 0x7) << target_TIGER_ID_shift)
      self.command_words[11] = self.cmd_word0

   def set_command_code(self, command_code_param):
      if (command_code_param == 'WR'):
         self.command_code = 0x8
         self.is_a_write = 1
         ## acr 2017-08-29 self.cmd_word0 = ((self.command_code & 0xF) << 12) + ((self.target_TIGER_ID & 0x7) << 8) + ((self.is_a_write & 0x1) << 7)
         self.cmd_word0 = ((self.command_code & 0xF) << command_code_shift) + ((self.target_TIGER_ID & 0x7) << target_TIGER_ID_shift)
         self.command_words[11] = self.cmd_word0
      elif (command_code_param == 'RD'):
         self.command_code = 0x9
         self.is_a_write = 0
         ## acr 2017-08-29 self.cmd_word0 = ((self.command_code & 0xF) << 12) + ((self.target_TIGER_ID & 0x7) << 8) + ((self.is_a_write & 0x1) << 7)
         self.cmd_word0 = ((self.command_code & 0xF) << command_code_shift) + ((self.target_TIGER_ID & 0x7) << target_TIGER_ID_shift)
         self.command_words[11] = self.cmd_word0
      else :
         print class_name, "bad command_code parameter passed"

   def update_command_words(self):
      if ( (self.command_code & 0xF) == 0x9 ):
         self.cmd_word10 = 0
         self.cmd_word9  = 0
         self.cmd_word8  = 0
         self.cmd_word7  = 0
         self.cmd_word6  = 0
         self.cmd_word5  = 0
         self.cmd_word4  = 0
         self.cmd_word3  = 0
         self.cmd_word2  = 0
         self.cmd_word1  = 0
      else :
         self.cmd_word10 = ((self.BufferBias & 0x3) << 24)        + ((self.TDCVcasN & 0xF) << 16)         + ((self.TDCVcasP & 0x1F) << 8)        + ((self.TDCVcasPHyst & 0x3F))
         self.cmd_word9 =  ((self.DiscFE_Ibias & 0x3f) << 24)     + ((self.BiasFE_PpreN & 0x3F) << 16)    + ((self.AVcasp_global & 0x1F) << 8)   + ((self.TDCcompVcas & 0xF))  
         self.cmd_word8 =  ((self.TDCIres & 0x1f) << 24)          + ((self.DiscVcasN & 0xF) << 16)        + ((self.IntegVb1 & 0x3F) << 8)        + ((self.BiasFE_A1 & 0xF))  
         self.cmd_word7 =  ((self.Vcasp_Vth & 0x3f) << 24)        + ((self.TAC_I_LSB & 0x1F) << 16)       + ((self.TDCcompVbias & 0x1F) << 8)    + ((self.Vref_Integ & 0x3F))  
         self.cmd_word6 =  ((self.IBiasTPcal & 0x1f) << 24)       + ((self.TP_Vcal & 0x1F) << 16)         + ((self.ShaperIbias & 0xF) << 8)      + ((self.IPostamp & 0x1F))  
         self.cmd_word5 =  ((self.TP_Vcal_ref & 0x1f) << 24)      + ((self.Vref_integ_diff & 0x3F) << 16) + ((self.Sig_pol & 0x1) << 8)          + ((self.FE_TPEnable & 0x1))  
         self.cmd_word4 =  ((self.CompactDataFormat & 0x1) << 24) + ((self.DataClkDiv & 0x3) << 16)       + ((self.TACrefreshPeriod & 0xf) << 8) + ((self.TACrefreshEnable & 0x1))  
         self.cmd_word3 =  ((self.CounterPeriod & 0x7) << 24)     + ((self.CounterEnable & 0x1) << 16)    + ((self.StopRampEnable & 0x3) << 8)   + ((self.RClkEnable & 0x1F))  
         self.cmd_word2 =  ((self.TDCClkdiv & 0x1) << 24)         + ((self.VetoMode & 0x3F) << 16)        + ((self.Ch_DebugMode & 0x1) << 8)     + ((self.TxMode & 0x3))  
         self.cmd_word1 =  ((self.TxDDR & 0x1) << 24)             + ((self.TxLinks & 0x3) << 16)
      ## acr 2017-08-29 self.cmd_word0 =  ((self.command_code & 0xF) << 12)      + ((self.target_TIGER_ID & 0x7) << 8)   + ((self.is_a_write & 0x1) << 7)
      self.cmd_word0 = ((self.command_code & 0xF) << command_code_shift) + ((self.target_TIGER_ID & 0x7) << target_TIGER_ID_shift)                               
      self.command_words = [ self.cmd_header,
                             self.cmd_word10,
                             self.cmd_word9,
                             self.cmd_word8,
                             self.cmd_word7,
                             self.cmd_word6,
                             self.cmd_word5,
                             self.cmd_word4,
                             self.cmd_word3,
                             self.cmd_word2,
                             self.cmd_word1,
                             self.cmd_word0
                              ]
##      for i in range (0, len(self.command_words)):
##         print '%08X'% self.command_words[i]


class ch_reg_settings: # purpose: organize the Channel Configuration Register Settings in an array format which can be sent over Ethernet or optical link
   def __init__(self,
                TARGET_GEMROC_ID_param = 0, # acr 2017-09-22
                cfg_filename_param = "default_ch_cfg_mainz.txt" # acr 2017-11-16 
                ):
      self.TARGET_GEMROC_ID = TARGET_GEMROC_ID_param # acr 2017-09-22
      self.cfg_filename = cfg_filename_param
      ## acr 2017-07-12 BEGIN implementing a parameter array loaded from a configuration file
      self.parameter_array = [0 for i in range(30)]
      with open(self.cfg_filename, "r") as f:
         self.parameter_array = map(int, f)
      ##print self.parameter_array
##      1self.DisableHyst = self.parameter_array [0]  ## DisableHyst_param
##      2self.T2Hyst = self.parameter_array [1]  ## T2Hyst_param
##      2self.T1Hyst = self.parameter_array [2]  ## T1Hyst_param
##      0self.Ch63ObufMSB = self.parameter_array [3]  ## Ch63ObufMSB_param
##      0self.TP_disable_FE = self.parameter_array [4]  ## TP_disable_FE_param
##      0self.TDC_IB_E = self.parameter_array [5]  ## TDC_IB_E_param
##      0self.TDC_IB_T = self.parameter_array [6]  ## TDC_IB_T_param
##      0self.Integ = self.parameter_array [7]  ## Integ_param
##      0self.PostAmpGain = self.parameter_array [8]  ## PostAmpGain_param
##      1self.FE_delay = self.parameter_array [9]  ## FE_delay_param
##      0self.Vth_T2 = self.parameter_array [10]  ## Vth_T2_param
##      0self.Vth_T1 = self.parameter_array [11]  ## Vth_T1_param
##      1self.QTx2Enable = self.parameter_array [12]  ## QTx2Enable_param
##      6self.MaxIntegTime = self.parameter_array [13]  ## MaxIntegTime_param
##      6self.MinIntegTime = self.parameter_array [14]  ## MinIntegTime_param
##      1self.TriggerBLatched = self.parameter_array [15]  ## TriggerBLatched_param
##      0self.QdcMode = self.parameter_array [16]  ## QdcMode_param
##      0self.BranchEnableT = self.parameter_array [17]  ## BranchEnableT_param
##      0self.BranchEnableEQ = self.parameter_array [18]  ## BranchEnableEQ_param
##      0self.TriggerMode2B = self.parameter_array [19]  ## TriggerMode2B_param
##      0self.TriggerMode2Q = self.parameter_array [20]  ## TriggerMode2Q_param
##      0self.TriggerMode2E = self.parameter_array [21]  ## TriggerMode2E_param
##      0self.TriggerMode2T = self.parameter_array [22]  ## TriggerMode2T_param
##      10self.TACMinAge = self.parameter_array [23]  ## TACMinAge_param
##      31self.TACMaxAge = self.parameter_array [24]  ## TACMaxAge_param
##      0self.CounterMode = self.parameter_array [25]  ## CounterMode_param
##      0self.DeadTime = self.parameter_array [26]  ## DeadTime_param
##      0self.SyncChainLen = self.parameter_array [27]  ## SyncChainLen_param
##      0self.Ch_DebugMode = self.parameter_array [28]  ## Ch_DebugMode_param
##      0self.TriggerMode = self.parameter_array [29]  ## TriggerMode_param
      self.DisableHyst = self.parameter_array [0]  ## DisableHyst_param
      self.T2Hyst = self.parameter_array [1]  ## T2Hyst_param
      self.T1Hyst = self.parameter_array [2]  ## T1Hyst_param
      self.Ch63ObufMSB = self.parameter_array [3]  ## Ch63ObufMSB_param
      self.TP_disable_FE = self.parameter_array [4]  ## TP_disable_FE_param
      self.TDC_IB_E = self.parameter_array [5]  ## TDC_IB_E_param
      self.TDC_IB_T = self.parameter_array [6]  ## TDC_IB_T_param
      self.Integ = self.parameter_array [7]  ## Integ_param
      self.PostAmpGain = self.parameter_array [8]  ## PostAmpGain_param
      self.FE_delay = self.parameter_array [9]  ## FE_delay_param
      self.Vth_T2 = self.parameter_array [10]  ## Vth_T2_param
      self.Vth_T1 = self.parameter_array [11]  ## Vth_T1_param
      self.QTx2Enable = self.parameter_array [12]  ## QTx2Enable_param
      self.MaxIntegTime = self.parameter_array [13]  ## MaxIntegTime_param
      self.MinIntegTime = self.parameter_array [14]  ## MinIntegTime_param
      self.TriggerBLatched = self.parameter_array [15]  ## TriggerBLatched_param
      self.QdcMode = self.parameter_array [16]  ## QdcMode_param
      self.BranchEnableT = self.parameter_array [17]  ## BranchEnableT_param
      self.BranchEnableEQ = self.parameter_array [18]  ## BranchEnableEQ_param
      self.TriggerMode2B = self.parameter_array [19]  ## TriggerMode2B_param
      self.TriggerMode2Q = self.parameter_array [20]  ## TriggerMode2Q_param
      self.TriggerMode2E = self.parameter_array [21]  ## TriggerMode2E_param
      self.TriggerMode2T = self.parameter_array [22]  ## TriggerMode2T_param
      self.TACMinAge = self.parameter_array [23]  ## TACMinAge_param
      self.TACMaxAge = self.parameter_array [24]  ## TACMaxAge_param
      self.CounterMode = self.parameter_array [25]  ## CounterMode_param
      self.DeadTime = self.parameter_array [26]  ## DeadTime_param
      self.SyncChainLen = self.parameter_array [27]  ## SyncChainLen_param
      self.Ch_DebugMode = self.parameter_array [28]  ## Ch_DebugMode_param
      self.TriggerMode = self.parameter_array [29]  ## TriggerMode_param
      ## acr 2017-07-12 END   implementing a parameter array loaded from a configuration file

      self.is_a_write = 0x1
      self.target_TIGER_ID = 0x1
      self.command_code = 0x9
      self.TO_ALL_enable = 0x1
      self.channel_ID = 0x3
    ## command codes as defined by Ricardo Bugalho's configurator: CMD_WRCHCFG 0x0; CMD_RDCHCFG = 0x1; CMD_WRccfg = 0x8; CMD_RDccfg = 0x9;
      header_tag = 0x8 << 28
      ##trailer_tag = 0x9 << 28
      Channel_cfg_cmd_ID = 0xE #F for Global; E for channel
      ccfg_cmd_tag = Channel_cfg_cmd_ID << 24 
      ccfg_cmd_word_count = 9 #11 for Global cfg reg; 9 for channel cfg reg
      self.cmd_header = header_tag + (self.TARGET_GEMROC_ID << 16) + ccfg_cmd_tag + ccfg_cmd_word_count
      ##cmd_trailer = trailer_tag + ccfg_cmd_tag

      self.cmd_word8 = ((self.DisableHyst & 0x1) << 24) + ((self.T2Hyst & 0x7) << 16) + ((self.T1Hyst & 0x7) << 8) + ((self.Ch63ObufMSB & 0x1))  
      self.cmd_word7 = ((self.TP_disable_FE & 0x1) << 24) + ((self.TDC_IB_E & 0xF) << 16) + ((self.TDC_IB_T & 0xF) << 8) + ((self.Integ & 0x1))  
      self.cmd_word6 = ((self.PostAmpGain & 0x3) << 24) + ((self.FE_delay & 0x1F) << 16) + ((self.Vth_T2 & 0x3F) << 8) + ((self.Vth_T1 & 0x3F))  
      self.cmd_word5 = ((self.QTx2Enable & 0x1) << 24) + ((self.MaxIntegTime & 0x7F) << 16) + ((self.MinIntegTime & 0x7F) << 8) + ((self.TriggerBLatched & 0x1))  
      self.cmd_word4 = ((self.QdcMode & 0x1) << 24) + ((self.BranchEnableT & 0x1) << 16) + ((self.BranchEnableEQ & 0x1) << 8) + ((self.TriggerMode2B & 0x7))  
      self.cmd_word3 = ((self.TriggerMode2Q & 0x3) << 24) + ((self.TriggerMode2E & 0x7) << 16) + ((self.TriggerMode2T & 0x3) << 8) + ((self.TACMinAge & 0x1F))  
      self.cmd_word2 = ((self.TACMaxAge & 0x1F) << 24) + ((self.CounterMode & 0xF) << 16) + ((self.DeadTime & 0x3F) << 8) + ((self.SyncChainLen & 0x3))  
      self.cmd_word1 = ((self.Ch_DebugMode & 0x3) << 24) + ((self.TriggerMode & 0x3) << 16)
      ## acr 2017-08-22 self.cmd_word0 = ((self.command_code & 0xF) << 12) + ((self.target_TIGER_ID & 0x7) << 8) + ((self.is_a_write & 0x1) << 7) + ((self.TO_ALL_enable & 0x1) << 6) + (self.channel_ID & 0x1F)
      self.cmd_word0 = ((self.command_code & 0xF) << 11) + ((self.target_TIGER_ID & 0x7) << 8) + ((self.is_a_write & 0x1) << 7) + ((self.TO_ALL_enable & 0x1) << 6) + (self.channel_ID & 0x3F)
                                     
      self.command_words = [ self.cmd_header,
                             self.cmd_word8,
                             self.cmd_word7,
                             self.cmd_word6,
                             self.cmd_word5,
                             self.cmd_word4,
                             self.cmd_word3,
                             self.cmd_word2,
                             self.cmd_word1,
                             self.cmd_word0
                              ]

   def __del__(self):
      class_name = self.__class__.__name__
      print class_name, "ch_reg_settings destroyed"

   def cmd_words_array_size(self):
      return len(self.command_words)

   def print_command_words(self):
      for i in range (0, len(self.command_words)):
         print '%08X'% self.command_words[i]

##   def set_write_flag(self, write_flag_param):
##      self.is_a_write = write_flag_param & 0x1
##      self.cmd_word0 = ((self.command_code & 0xF) << 12) + ((self.target_TIGER_ID & 0x7) << 8) + ((self.is_a_write & 0x1) << 7)
##      self.command_words[9] = self.cmd_word0

   def set_target_GEMROC(self, GEMROC_ID_param): # acr 2017-09-22
      self.TARGET_GEMROC_ID = GEMROC_ID_param & 0x1f
      self.cmd_header = (self.cmd_header & 0xFF00FFFF) + (self.TARGET_GEMROC_ID << 16)

   def set_target_TIGER(self, target_TIGER_param):
      self.target_TIGER_ID = target_TIGER_param & 0x7
      self.cmd_word0 = ((self.command_code & 0xF) << 12) + ((self.target_TIGER_ID & 0x7) << 8) + ((self.is_a_write & 0x1) << 7) + ((self.TO_ALL_enable & 0x1) << 6) + (self.channel_ID & 0x1F)
      self.command_words[9] = self.cmd_word0

   def set_target_channel(self, target_channel_param):
      self.channel_ID = target_channel_param & 0x1F
      self.cmd_word0 = ((self.command_code & 0xF) << 12) + ((self.target_TIGER_ID & 0x7) << 8) + ((self.is_a_write & 0x1) << 7) + ((self.TO_ALL_enable & 0x1) << 6) + (self.channel_ID & 0x1F)
      self.command_words[9] = self.cmd_word0

   def set_to_ALL_param(self, to_ALL_param):
      self.TO_ALL_enable = to_ALL_param & 0x1
      self.cmd_word0 = ((self.command_code & 0xF) << 12) + ((self.target_TIGER_ID & 0x7) << 8) + ((self.is_a_write & 0x1) << 7) + ((self.TO_ALL_enable & 0x1) << 6) + (self.channel_ID & 0x1F)
      self.command_words[9] = self.cmd_word0

   def set_command_code(self, command_code_param):
      if (command_code_param == 'WR'):
         self.command_code = 0x0
         self.is_a_write = 1
         self.cmd_word0 = ((self.command_code & 0xF) << 12) + ((self.target_TIGER_ID & 0x7) << 8) + ((self.is_a_write & 0x1) << 7) + ((self.TO_ALL_enable & 0x1) << 6) + (self.channel_ID & 0x1F)
         self.command_words[9] = self.cmd_word0
      elif (command_code_param == 'RD'):
         self.command_code = 0x1
         self.is_a_write = 0
         self.cmd_word0 = ((self.command_code & 0xF) << 12) + ((self.target_TIGER_ID & 0x7) << 8) + ((self.is_a_write & 0x1) << 7) + ((self.TO_ALL_enable & 0x1) << 6) + (self.channel_ID & 0x1F)
         self.command_words[9] = self.cmd_word0
      else :
         print class_name, "bad command_code parameter passed"

##   def update_command_words(self):
##      self.cmd_word8 = ((self.DisableHyst & 0x1) << 24)   + ((self.T2Hyst & 0x7) << 16)        + ((self.T1Hyst & 0x7) << 8)         + ((self.Ch63ObufMSB & 0x1))  
##      self.cmd_word7 = ((self.TP_disable_FE & 0x1) << 24) + ((self.TDC_IB_E & 0xF) << 16)      + ((self.TDC_IB_T & 0xF) << 8)       + ((self.Integ & 0x1))  
##      self.cmd_word6 = ((self.PostAmpGain & 0x3) << 24)   + ((self.FE_delay & 0x1F) << 16)     + ((self.Vth_T2 & 0x3F) << 8)        + ((self.Vth_T1 & 0x3F))  
##      self.cmd_word5 = ((self.QTx2Enable & 0x1) << 24)    + ((self.MaxIntegTime & 0x7F) << 16) + ((self.MinIntegTime & 0x7F) << 8)  + ((self.TriggerBLatched & 0x1))  
##      self.cmd_word4 = ((self.QdcMode & 0x1) << 24)       + ((self.BranchEnableT & 0x1) << 16) + ((self.BranchEnableEQ & 0x1) << 8) + ((self.TriggerMode2B & 0x7))  
##      self.cmd_word3 = ((self.TriggerMode2Q & 0x3) << 24) + ((self.TriggerMode2E & 0x7) << 16) + ((self.TriggerMode2T & 0x3) << 8)  + ((self.TACMinAge & 0x1F))  
##      self.cmd_word2 = ((self.TACMaxAge & 0x1F) << 24)    + ((self.CounterMode & 0xF) << 16)   + ((self.DeadTime & 0x3F) << 8)      + ((self.SyncChainLen & 0x3))  
##      self.cmd_word1 = ((self.Ch_DebugMode & 0x3) << 24)  + ((self.TriggerMode & 0x3) << 16)
##      self.cmd_word0 = ((self.command_code & 0xF) << 12) + ((self.target_TIGER_ID & 0x7) << 8) + ((self.is_a_write & 0x1) << 7) + ((self.TO_ALL_enable & 0x1) << 6) + (self.channel_ID & 0x1F)
##                                     
##      self.command_words = [ self.cmd_header,
##                             self.cmd_word8,
##                             self.cmd_word7,
##                             self.cmd_word6,
##                             self.cmd_word5,
##                             self.cmd_word4,
##                             self.cmd_word3,
##                             self.cmd_word2,
##                             self.cmd_word1,
##                             self.cmd_word0
##                              ]
####      for i in range (0, len(self.command_words)):
####         print '%08X'% self.command_words[i]

   def update_command_words(self):  # acr 2017-09-01
      if ( (self.command_code & 0xF) == 0x1 ): # acr 2017-09-01
         self.cmd_word8  = 0
         self.cmd_word7  = 0
         self.cmd_word6  = 0
         self.cmd_word5  = 0
         self.cmd_word4  = 0
         self.cmd_word3  = 0
         self.cmd_word2  = 0
         self.cmd_word1  = 0
      else :
         self.cmd_word8 = ((self.DisableHyst & 0x1) << 24)   + ((self.T2Hyst & 0x7) << 16)        + ((self.T1Hyst & 0x7) << 8)         + ((self.Ch63ObufMSB & 0x1))  
         self.cmd_word7 = ((self.TP_disable_FE & 0x1) << 24) + ((self.TDC_IB_E & 0xF) << 16)      + ((self.TDC_IB_T & 0xF) << 8)       + ((self.Integ & 0x1))  
         self.cmd_word6 = ((self.PostAmpGain & 0x3) << 24)   + ((self.FE_delay & 0x1F) << 16)     + ((self.Vth_T2 & 0x3F) << 8)        + ((self.Vth_T1 & 0x3F))  
         self.cmd_word5 = ((self.QTx2Enable & 0x1) << 24)    + ((self.MaxIntegTime & 0x7F) << 16) + ((self.MinIntegTime & 0x7F) << 8)  + ((self.TriggerBLatched & 0x1))  
         self.cmd_word4 = ((self.QdcMode & 0x1) << 24)       + ((self.BranchEnableT & 0x1) << 16) + ((self.BranchEnableEQ & 0x1) << 8) + ((self.TriggerMode2B & 0x7))  
         self.cmd_word3 = ((self.TriggerMode2Q & 0x3) << 24) + ((self.TriggerMode2E & 0x7) << 16) + ((self.TriggerMode2T & 0x3) << 8)  + ((self.TACMinAge & 0x1F))  
         self.cmd_word2 = ((self.TACMaxAge & 0x1F) << 24)    + ((self.CounterMode & 0xF) << 16)   + ((self.DeadTime & 0x3F) << 8)      + ((self.SyncChainLen & 0x3))  
         self.cmd_word1 = ((self.Ch_DebugMode & 0x3) << 24)  + ((self.TriggerMode & 0x3) << 16)
      self.cmd_word0 = ((self.command_code & 0xF) << command_code_shift) + ((self.target_TIGER_ID & 0x7) << target_TIGER_ID_shift) + ((self.TO_ALL_enable & 0x1) << 6) + (self.channel_ID & 0x1F)

      self.command_words = [ self.cmd_header,
                             self.cmd_word8,
                             self.cmd_word7,
                             self.cmd_word6,
                             self.cmd_word5,
                             self.cmd_word4,
                             self.cmd_word3,
                             self.cmd_word2,
                             self.cmd_word1,
                             self.cmd_word0
                              ]
##      for i in range (0, len(self.command_words)):
##         print '%08X'% self.command_words[i]

class gemroc_cmd_settings(object): # purpose: organize the GEMROC Configuration Register Settings in an array format which can be sent over Ethernet or optical link
   def __init__(self,
                TARGET_GEMROC_ID_param = 0, # acr 2017-09-22              
                ## cfg_filename_param = "default_GEMROC_cfg.txt" # acr 2017-09-18 
                command_string_param = 'NONE',
                ## TIGER_ID_param = 1,              
                TCAM_ID_param = 1,              
                number_of_repetitions_param = 1,
                ##to_ALL_TIGER_enable_param = 0,
                to_ALL_TCAM_enable_param = 0
                ):    
      self.TARGET_GEMROC_ID = TARGET_GEMROC_ID_param # acr 2017-09-22
      ## self.cfg_filename = cfg_filename_param
      self.cfg_filename = 'GEMROC_%d_def_cfg_v20.txt'% self.TARGET_GEMROC_ID
      self.parameter_array = [0 for i in range(34)] # acr 2017-12-07 range extended from 30 to 34
      with open(self.cfg_filename, "r") as f:
         self.parameter_array = map(int, f)
##    >>>def square(x) :     
##       ...     return x ** 2
##       ... 
##    >>> map(square, [1,2,3,4,5])
##       [1, 4, 9, 16, 25]
      ##print self.parameter_array
      #Note: TCAM = Tiger Configuration/ Acquisition Module
      self.EN_TM_TCAM_pattern = self.parameter_array [0] # EN_TM_TCAM[7..0] Enable the target TCAM to generate Trigger Matched data packets 
      self.AUTO_TP_EN_pattern     = self.parameter_array [1] # Enable Test Pulse Generation for TCAM[3..0]
      self.AUTO_L1_EN_pattern     = self.parameter_array [2] # Enable simulated L1 Trigger Generation for TCAM[3..0]
      self.Periodic_TP_EN_pattern = self.parameter_array [3] # Enable Periodic Test Pulse Generation for TCAM[3..0]
      self.Periodic_L1_EN_pattern = self.parameter_array [4] # Enable simulated L1 Trigger Generation for TCAM[3..0]
      self.TP_period = self.parameter_array [5] # period (in units of BES-III clk cycles) of periodic Test Pulses; range: 0..1023
      self.TP_width = self.parameter_array [6] # acr 2017-09-28 width (in units of BES-III clk cycles) of periodic Test Pulses; range: 0..15
      self.L1_period = self.parameter_array [7] # period (in units of BES-III clk cycles) of periodic simulated L1 triggers; range: 0..1023
      self.LowerDataScanWindowOffset = self.parameter_array [8] # offset, w.r.t. to current write pointer, at which to start reading data from the TIGER data ring buffers; range: 16 bits
      self.UpperDataScanWindowOffset = self.parameter_array [9] # offset, w.r.t. to current write pointer, at which to stop reading data from the TIGER data ring buffers; range: 16 bits
      self.L1_latency = self.parameter_array [10] # latency with respect to the event of BES-III L1 trigger (in units of BES-III clk cycles); range: 0..1023
      self.FEB_PWR_EN_pattern = self.parameter_array [11] 
      self.FEB_OVC_EN_pattern = self.parameter_array [12] 
      self.FEB_OVV_EN_pattern = self.parameter_array [13] 
      self.FEB_OVT_EN_pattern = self.parameter_array [14] 
      self.ROC_OVT_EN  = self.parameter_array [15] # 1 bit
      self.TL_nTM_ACQ  = self.parameter_array [16] # 1 bit
      self.ROC_OVT_LIMIT = self.parameter_array [17] # Overtemperature limit for GEMROC module; range 0..255
      self.OVC_LIMIT_FEB0 = self.parameter_array [18] # Overcurrent limit for FEB0 connected to the target GEMROC module; range 0..511
      self.OVV_LIMIT_FEB0 = self.parameter_array [19] # Overvoltage limit for FEB0 connected to the target GEMROC module; range 0..511
      self.OVT_LIMIT_FEB0 = self.parameter_array [20] # Overtemperature limit for FEB0 connected to the target GEMROC module; range 0..255
      self.OVC_LIMIT_FEB1 = self.parameter_array [21] # Overcurrent limit for FEB1 connected to the target GEMROC module; range 0..511
      self.OVV_LIMIT_FEB1 = self.parameter_array [22] # Overvoltage limit for FEB1 connected to the target GEMROC module; range 0..511
      self.OVT_LIMIT_FEB1 = self.parameter_array [23] # Overtemperature limit for FEB1 connected to the target GEMROC module; range 0..255
      self.OVC_LIMIT_FEB2 = self.parameter_array [24] # Overcurrent limit for FEB2 connected to the target GEMROC module; range 0..511
      self.OVV_LIMIT_FEB2 = self.parameter_array [25] # Overvoltage limit for FEB2 connected to the target GEMROC module; range 0..511
      self.OVT_LIMIT_FEB2 = self.parameter_array [26] # Overtemperature limit for FEB2 connected to the target GEMROC module; range 0..255
      self.OVC_LIMIT_FEB3 = self.parameter_array [27] # Overcurrent limit for FEB3 connected to the target GEMROC module; range 0..511
      self.OVV_LIMIT_FEB3 = self.parameter_array [28] # Overvoltage limit for FEB3 connected to the target GEMROC module; range 0..511
      self.OVT_LIMIT_FEB3 = self.parameter_array [29] # Overtemperature limit for FEB3 connected to the target GEMROC module; range 0..255
      self.TIMING_DLY_FEB0 = self.parameter_array [30] # setting of timing signals to TIGER FEB0 in xns units
      self.TIMING_DLY_FEB1 = self.parameter_array [31] # setting of timing signals to TIGER FEB1 in xns units
      self.TIMING_DLY_FEB2 = self.parameter_array [32] # setting of timing signals to TIGER FEB2 in xns units
      self.TIMING_DLY_FEB3 = self.parameter_array [33] # setting of timing signals to TIGER FEB3 in xns units
      self.TP_Pos_nNeg = self.parameter_array [34] # select polarity of test pulse output to TIGER
      ## 
##      self.ROC_OVT_STATUS = 0
##      self.OVC_STATUS_FEB0 = 0
##      self.OVV_STATUS_FEB0 = 0
##      self.OVT_STATUS_FEB0 = 0
##      self.OVC_STATUS_FEB1 = 0
##      self.OVV_STATUS_FEB1 = 0
##      self.OVT_STATUS_FEB1 = 0
##      self.OVC_STATUS_FEB2 = 0
##      self.OVV_STATUS_FEB2 = 0
##      self.OVT_STATUS_FEB2 = 0
##      self.OVC_STATUS_FEB3 = 0
##      self.OVV_STATUS_FEB3 = 0
##      self.OVT_STATUS_FEB3 = 0
##      self.ROC_TEMP = 0
##      self.CURRENT_FEB0 = 0
##      self.VOLTAGE_FEB0 = 0
##      self.TEMP_FEB0 = 0
##      self.CURRENT_FEB1 = 0
##      self.VOLTAGE_FEB1 = 0
##      self.TEMP_FEB1 = 0
##      self.CURRENT_FEB2 = 0
##      self.VOLTAGE_FEB2 = 0
##      self.TEMP_FEB2 = 0
##      self.CURRENT_FEB3 = 0
##      self.VOLTAGE_FEB3 = 0
##      self.TEMP_FEB3 = 0
      ##
      self.command_string = command_string_param
      ## self.target_TIGER_ID = TIGER_ID_param
      self.target_TCAM_ID = TCAM_ID_param
      ## self.to_ALL_TIGER_enable = to_ALL_TIGER_enable_param
      self.to_ALL_TCAM_enable = to_ALL_TCAM_enable_param
      self.number_of_repetitions = number_of_repetitions_param
      ## acr 2017-11-09 added 'TIGER_SYNCH_RST_WITH_SPI' to the list
      self.command_list = [ 'NONE',
                            'WR',
                            'RD',
                            'TIGER_SYNCH_RST',
                            'TCAM_SYNCH_RST',
                            'TP_GEN',
                            'L1_GEN',
                            'XCVR_LOOPBACK_TEST_EN',
                            'ACK_FRAME_SEQ_ERR_FLAG',
##                            'EN_AUTO_TP_FROM_HEARTBEAT_WORD',
##                            'DIS_AUTO_TP_FROM_HEARTBEAT_WORD',
##                            'GEN_LONG_TIGER_SYNCH_RST',
##                            'SET_TARGET_FEB', ## for LV commands
##                            'FEB_PWR_ON',
##                            'FEB_PWR_OFF',
                            'IVT_UPDATE',
                            'IVT_READ',
                            'CMD_GEMROC_TIGER_CFG_RESET',
                            'FEB_TIMING_DELAYS_UPDATE'
                       ]
      self.is_a_write = 0x1
      for i in range (0, len(self.command_list)):
         if (self.command_string == self.command_list[i]):
            self.gemroc_cmd_code = i
      header_tag = 0x8 << 28
      gemroc_cmd_ID = 0xD #F for Global config register settings; E for channel config register settings; D for gemroc command settings
      gemroc_cmd_tag = gemroc_cmd_ID << 24 
      # acr 2017-10-02 gemroc_cmd_word_count = 9 # acr 2017-09-18
      # acr 2017-12-07 gemroc_cmd_word_count = 10 # acr 2017-10-02
      gemroc_cmd_word_count = 11 # acr 2017-12-07
      self.cmd_header = header_tag + (self.TARGET_GEMROC_ID << 16) + gemroc_cmd_tag + gemroc_cmd_word_count 
      self.cmd_word9 = ((self.TIMING_DLY_FEB3 & 0x3F) << 24) + ((self.TIMING_DLY_FEB2 & 0x3F) << 16) + ((self.TIMING_DLY_FEB1 & 0x3F) << 8) + ((self.TIMING_DLY_FEB0 & 0x3F) << 0) 
      self.cmd_word8 = ((self.OVT_LIMIT_FEB3 & 0xFF) << 22) + ((self.OVV_LIMIT_FEB3 & 0x1FF) << 13) + ((self.OVC_LIMIT_FEB3 & 0x1FF) << 4) 
      self.cmd_word7 = ((self.OVT_LIMIT_FEB2 & 0xFF) << 22) + ((self.OVV_LIMIT_FEB2 & 0x1FF) << 13) + ((self.OVC_LIMIT_FEB2 & 0x1FF) << 4) 
      self.cmd_word6 = ((self.OVT_LIMIT_FEB1 & 0xFF) << 22) + ((self.OVV_LIMIT_FEB1 & 0x1FF) << 13) + ((self.OVC_LIMIT_FEB1 & 0x1FF) << 4) 
      self.cmd_word5 = ((self.OVT_LIMIT_FEB0 & 0xFF) << 22) + ((self.OVV_LIMIT_FEB0 & 0x1FF) << 13) + ((self.OVC_LIMIT_FEB0 & 0x1FF) << 4) 
      self.cmd_word4 = ((self.ROC_OVT_LIMIT & 0x3F) << 24) + ((self.TP_Pos_nNeg & 0x1) << 18) + ((self.TL_nTM_ACQ & 0x1) << 17) + ((self.ROC_OVT_EN & 0x1) << 16) + ((self.FEB_OVT_EN_pattern & 0xF) << 12) + ((self.FEB_OVV_EN_pattern & 0xF) << 8) + ((self.FEB_OVC_EN_pattern & 0xF) << 4) + (self.FEB_PWR_EN_pattern & 0xF)
      #acr 2017-09-28 self.cmd_word3 = ((self.L1_latency & 0x3FF) << 20) + (self.UpperDataScanWindowOffset & 0xFFFF)
      self.cmd_word3 = ((self.L1_latency & 0x3FF) << 20) + ((self.Periodic_TP_EN_pattern & 0xF) << 16) + (self.UpperDataScanWindowOffset & 0xFFFF)
      # acr 2017-09-28 self.cmd_word2 = ((self.L1_period  & 0x3FF) << 20) + (self.LowerDataScanWindowOffset & 0xFFFF)
      self.cmd_word2 = ((self.L1_period  & 0x3FF) << 20) + ((self.TP_width  & 0xF) << 16) + (self.LowerDataScanWindowOffset & 0xFFFF)
      # acr 2017-09-18 self.cmd_word1 = self.number_of_executions
      #acr 2017-09-28 self.cmd_word1 = ((self.TP_period & 0x3FF) << 20) + ((self.Periodic_L1_EN_pattern & 0xF) << 16) + ((self.Periodic_TP_EN_pattern & 0xF) << 12) + ((self.AUTO_L1_EN_pattern & 0xF) << 8) + ((self.AUTO_TP_EN_pattern & 0xF) << 4) + (self.EN_TM_TCAM_pattern & 0xF)
      self.cmd_word1 = ((self.TP_period & 0x3FF) << 20) + ((self.Periodic_L1_EN_pattern & 0xF) << 16) + ((self.AUTO_L1_EN_pattern & 0xF) << 12) + ((self.AUTO_TP_EN_pattern & 0xF) << 8) + (self.EN_TM_TCAM_pattern & 0xFF)
      ## acr 2017-09-13 self.cmd_word0 = ((self.gemroc_cmd_code & 0xF) << 12) + ((self.target_TIGER_ID & 0x7) << 8) + \
      ##                 ((self.is_a_write & 0x1) << 7) + ((self.to_ALL_TIGER_enable & 0x1) << 6) + ((self.to_ALL_TCAM_enable & 0x1) << 5) + (self.target_TCAM_ID & 0x7)
      ## acr 2017-09-18 self.cmd_word0 = ((self.gemroc_cmd_code & 0xF) << 11) + ((self.target_TIGER_ID & 0x7) << 8) + \
      ##                 ((self.is_a_write & 0x1) << 7) + ((self.to_ALL_TIGER_enable & 0x1) << 6) + ((self.to_ALL_TCAM_enable & 0x1) << 5) + (self.target_TCAM_ID & 0x7)
      self.cmd_word0 = ((self.number_of_repetitions & 0x3FF) << 16) + ((self.gemroc_cmd_code & 0xF) << 11) + ((self.target_TCAM_ID & 0x3) << 8) + ((self.to_ALL_TCAM_enable & 0x1) << 6)                                 
      self.command_words = [ self.cmd_header,
                             self.cmd_word9,
                             self.cmd_word8,
                             self.cmd_word7,
                             self.cmd_word6,
                             self.cmd_word5,
                             self.cmd_word4,
                             self.cmd_word3,
                             self.cmd_word2,
                             self.cmd_word1,
                             self.cmd_word0
                              ]
      
   def __del__(self):
      class_name = self.__class__.__name__
      print class_name, "gemroc_cmd_settings destroyed"

   def cmd_words_array_size(self):
      return len(self.command_words)

   def print_command_words(self):
      for i in range (0, len(self.command_words)):
         print '%08X'% self.command_words[i]

##   def set_target_TIGER(self, target_TIGER_param):
##      self.target_TIGER_ID = target_TIGER_param & 0x7
##      self.cmd_word0 = ((self.gemroc_cmd_code & 0xF) << 12) + ((self.target_TIGER_ID & 0x7) << 8) + \
##                       ((self.is_a_write & 0x1) << 7) + ((self.to_ALL_TIGER_enable & 0x1) << 6) + ((self.to_ALL_TCAM_enable & 0x1) << 5) + (self.target_TCAM_ID & 0x7)
##      self.command_words[2] = self.cmd_word0

   def set_target_GEMROC (self, GEMROC_ID_param):
      self.TARGET_GEMROC_ID = GEMROC_ID_param & 0x1F
      self.cmd_header = (self.cmd_header & 0xFF00FFFF) + (self.TARGET_GEMROC_ID << 16)

   def set_target_TCAM_ID(self, target_TA_param, to_ALL_TCAM_EN_param):
      self.target_TCAM_ID = target_TA_param & 0x3
      self.to_ALL_TCAM_enable = to_ALL_TCAM_EN_param & 0x1
      ## self.cmd_word0 = ((self.number_of_repetitions & 0x3FF) << 16) + ((self.gemroc_cmd_code & 0xF) << 11) + ((self.target_TCAM_ID & 0x3) << 8) + ((self.to_ALL_TCAM_enable & 0x1) << 6)                                 

##   def set_to_ALL_TIGER_param(self, to_ALL_param):
##      self.to_ALL_TIGER_enable = to_ALL_param & 0x1
##      self.cmd_word0 = ((self.gemroc_cmd_code & 0xF) << 12) + ((self.target_TIGER_ID & 0x7) << 8) + \
##                       ((self.is_a_write & 0x1) << 7) + ((self.to_ALL_TIGER_enable & 0x1) << 6) + ((self.to_ALL_TCAM_enable & 0x1) << 5) + (self.target_TCAM_ID & 0x7)
##      self.command_words[2] = self.cmd_word0

##   def set_to_ALL_TCAM_param(self, to_ALL_param):
##      self.to_ALL_TCAM_enable = to_ALL_param & 0x1
##      self.cmd_word0 = ((self.gemroc_cmd_code & 0xF) << 12) + ((self.target_TIGER_ID & 0x7) << 8) + \
##                       ((self.is_a_write & 0x1) << 7) + ((self.to_ALL_TIGER_enable & 0x1) << 6) + ((self.to_ALL_TCAM_enable & 0x1) << 5) + (self.target_TCAM_ID & 0x7)
##      self.command_words[2] = self.cmd_word0

   def set_FEB_PWR_EN_pattern(self, target_FEB_PWR_EN_pattern): # acr 2017-10-03
      self.FEB_PWR_EN_pattern = target_FEB_PWR_EN_pattern & 0xF

   def set_TP_width(self, target_TP_width_param): # acr 2017-09-28 
      self.TP_width = target_TP_width_param & 0xF

   def set_TP_period(self, TP_period_param): # acr 2017-09-28 
      self.TP_period = TP_period_param & 0x3FF

   def set_AUTO_TP_EN_pattern(self, target_AUTO_TP_EN_param): # acr 2017-10-03
      self.AUTO_TP_EN_pattern = target_AUTO_TP_EN_param & 0xF

   def set_Periodic_TP_EN_pattern(self, Periodic_TP_EN_pattern_param): # acr 2017-10-03
      self.Periodic_TP_EN_pattern = Periodic_TP_EN_pattern_param & 0xF

   def set_TL_nTM_ACQ(self, TL_nTM_ACQ_param): # acr 2017-10-03
      self.TL_nTM_ACQ = TL_nTM_ACQ_param & 0x1

   def set_EN_TM_TCAM_pattern(self, EN_TM_TCAM_pattern_param): # acr 2017-11-15
      self.EN_TM_TCAM_pattern = EN_TM_TCAM_pattern_param & 0xFF

   def set_TP_Pos_nNeg(self, TP_Pos_nNeg_param): # acr 2017-10-03
      self.TP_Pos_nNeg = TP_Pos_nNeg_param & 0x1

   def set_timing_toFEB_delay(self, FEB3dly_param, FEB2dly_param, FEB1dly_param, FEB0dly_param): # acr 2017-12-07
      self.TIMING_DLY_FEB3 = FEB3dly_param & 0x3F
      self.TIMING_DLY_FEB2 = FEB2dly_param & 0x3F
      self.TIMING_DLY_FEB1 = FEB1dly_param & 0x3F
      self.TIMING_DLY_FEB0 = FEB0dly_param & 0x3F

   def set_gemroc_cmd_code(self, command_string_param, no_of_executions_param):
      self.number_of_repetitions = no_of_executions_param
      self.command_string = command_string_param
      for i in range (0, len(self.command_list)):
         if (self.command_string == self.command_list[i]):
            self.gemroc_cmd_code = i

## acr 2017-10-02 updated definition: zero all setting when it's a RD command (RD has a code = 2)
##      if ( (self.command_code & 0xF) == 0x1 ): # acr 2017-09-01
   def update_command_words(self):
      if ( ((self.gemroc_cmd_code & 0xF) == 0x2) or ((self.gemroc_cmd_code & 0xF) == 0x3) or ((self.gemroc_cmd_code & 0xF) == 0x4) ): 
         self.cmd_word9  = 0
         self.cmd_word8  = 0
         self.cmd_word7  = 0
         self.cmd_word6  = 0
         self.cmd_word5  = 0
         self.cmd_word4  = 0
         self.cmd_word3  = 0
         self.cmd_word2  = 0
         self.cmd_word1  = 0
      else :
         self.cmd_word9 = ((self.TIMING_DLY_FEB3 & 0x3F) << 24) + ((self.TIMING_DLY_FEB2 & 0x3F) << 16) + ((self.TIMING_DLY_FEB1 & 0x3F) << 8) + ((self.TIMING_DLY_FEB0 & 0x3F) << 0) 
         self.cmd_word8 = ((self.OVT_LIMIT_FEB3 & 0xFF) << 22) + ((self.OVV_LIMIT_FEB3 & 0x1FF) << 13) + ((self.OVC_LIMIT_FEB3 & 0x1FF) << 4) 
         self.cmd_word7 = ((self.OVT_LIMIT_FEB2 & 0xFF) << 22) + ((self.OVV_LIMIT_FEB2 & 0x1FF) << 13) + ((self.OVC_LIMIT_FEB2 & 0x1FF) << 4) 
         self.cmd_word6 = ((self.OVT_LIMIT_FEB1 & 0xFF) << 22) + ((self.OVV_LIMIT_FEB1 & 0x1FF) << 13) + ((self.OVC_LIMIT_FEB1 & 0x1FF) << 4) 
         self.cmd_word5 = ((self.OVT_LIMIT_FEB0 & 0xFF) << 22) + ((self.OVV_LIMIT_FEB0 & 0x1FF) << 13) + ((self.OVC_LIMIT_FEB0 & 0x1FF) << 4)
         self.cmd_word4 = ((self.ROC_OVT_LIMIT & 0x3F) << 24) + ((self.TP_Pos_nNeg & 0x1) << 18) + ((self.TL_nTM_ACQ & 0x1) << 17) + ((self.ROC_OVT_EN & 0x1) << 16) + ((self.FEB_OVT_EN_pattern & 0xF) << 12) + ((self.FEB_OVV_EN_pattern & 0xF) << 8) + ((self.FEB_OVC_EN_pattern & 0xF) << 4) + (self.FEB_PWR_EN_pattern & 0xF)
         #acr 2017-09-28 self.cmd_word3 = ((self.L1_latency & 0x3FF) << 20) + (self.UpperDataScanWindowOffset & 0xFFFF)
         self.cmd_word3 = ((self.L1_latency & 0x3FF) << 20) + ((self.Periodic_TP_EN_pattern & 0xF) << 16) + (self.UpperDataScanWindowOffset & 0xFFFF)
         # acr 2017-09-28 self.cmd_word2 = ((self.L1_period  & 0x3FF) << 20) + (self.LowerDataScanWindowOffset & 0xFFFF)
         self.cmd_word2 = ((self.L1_period  & 0x3FF) << 20) + ((self.TP_width  & 0xF) << 16) + (self.LowerDataScanWindowOffset & 0xFFFF)
         # acr 2017-09-18 self.cmd_word1 = self.number_of_executions
         #acr 2017-09-28 self.cmd_word1 = ((self.TP_period & 0x3FF) << 20) + ((self.Periodic_L1_EN_pattern & 0xF) << 16) + ((self.Periodic_TP_EN_pattern & 0xF) << 12) + ((self.AUTO_L1_EN_pattern & 0xF) << 8) + ((self.AUTO_TP_EN_pattern & 0xF) << 4) + (self.EN_TM_TCAM_pattern & 0xF)
         self.cmd_word1 = ((self.TP_period & 0x3FF) << 20) + ((self.Periodic_L1_EN_pattern & 0xF) << 16) + ((self.AUTO_L1_EN_pattern & 0xF) << 12) + ((self.AUTO_TP_EN_pattern & 0xF) << 8) + (self.EN_TM_TCAM_pattern & 0xFF)
         ## acr 2017-09-13 self.cmd_word0 = ((self.gemroc_cmd_code & 0xF) << 12) + ((self.target_TIGER_ID & 0x7) << 8) + \
         ##                 ((self.is_a_write & 0x1) << 7) + ((self.to_ALL_TIGER_enable & 0x1) << 6) + ((self.to_ALL_TCAM_enable & 0x1) << 5) + (self.target_TCAM_ID & 0x7)
         ## acr 2017-09-18 self.cmd_word0 = ((self.gemroc_cmd_code & 0xF) << 11) + ((self.target_TIGER_ID & 0x7) << 8) + \
         ##                 ((self.is_a_write & 0x1) << 7) + ((self.to_ALL_TIGER_enable & 0x1) << 6) + ((self.to_ALL_TCAM_enable & 0x1) << 5) + (self.target_TCAM_ID & 0x7)
      self.cmd_word0 = ((self.number_of_repetitions & 0x3FF) << 16) + ((self.gemroc_cmd_code & 0xF) << 11) + ((self.target_TCAM_ID & 0x3) << 8) + ((self.to_ALL_TCAM_enable & 0x1) << 6)                                 
      self.command_words = [ self.cmd_header,
                             self.cmd_word8,
                             self.cmd_word7,
                             self.cmd_word6,
                             self.cmd_word5,
                             self.cmd_word4,
                             self.cmd_word3,
                             self.cmd_word2,
                             self.cmd_word1,
                             self.cmd_word9,
                             self.cmd_word0
                              ]
      for i in range (0, len(self.command_words)):
         print '%08X'% self.command_words[i]

