# file: Cfg_GEMROC_TIGER.py
# author: Angelo Cotta Ramusino INFN Ferrara
# date: 15 Jan 2018
# last mod: 03 Mar 2018
# purpose: to debug TIGER configuration and data acquisition through the GEMROC prototype
# last modifications / additions
# acr 2018-01-15 modified "GEMconf_classes" to split LV parameter control from DAQ parameter control
# acr 2017-07-11 modified "GEMconf_classes" to read single TIGER global / channel configuration from files
import socket
import string
import time
import GEMconf_classes_2018v3 #acr 2018-02-19 import GEMconf_classes_2018
import struct
import binascii
import array
import classes_test_functions
import sys
import threading ## acr 2018-02-21
import Queue     ## acr 2018-02-21
import os        ## acr 2018-02-21
import datetime

global num
num=0
default_arg_needed = 0
TARGET_GEMROC_ID_param = 0
TARGET_FEB_PWR_PATTERN_param = 0
IVT_LOG_PERIOD_SECONDS = 20
IVT_LOG_ENABLE = 1
if (len(sys.argv) < 3):
    print"\n GEMROC_TIGER_CFG argument list: <Target_GEMROC: 1 thru 22> <Target_FEB_PWR_EN_pattern(Hex):0x0 thru 0xF>"
##    exit()
    default_arg_needed = 1
if (default_arg_needed == 1):     
    TARGET_GEMROC_ID_param = 3
    TARGET_FEB_PWR_PATTERN_param = 0x1
else:
    TARGET_GEMROC_ID_param = int(sys.argv[1])
    TARGET_FEB_PWR_PATTERN_param = int(sys.argv[2],16)
    
GEMROC_ID = TARGET_GEMROC_ID_param
FEB_PWR_EN_pattern = TARGET_FEB_PWR_PATTERN_param

def send_GEMROC_CFG_CMD_PKT( TARGET_GEMROC_ID_param, COMMAND_STRING_PARAM, array_to_send_param, DEST_IP_ADDRESS_PARAM, DEST_PORT_NO_PARAM):
    buffer_to_send = struct.pack('>' + str(len(array_to_send_param)) + 'I', *array_to_send_param)
    clientSock.sendto(buffer_to_send, (DEST_IP_ADDRESS_PARAM, DEST_PORT_NO_PARAM))
    cmd_message = "\nTarget GEMROC: %d; CMD sent (%s): \n" %(TARGET_GEMROC_ID_param, COMMAND_STRING_PARAM)
    log_file.write(cmd_message)
    log_file.write(binascii.b2a_hex(buffer_to_send))
    command_echo_f = receiveSock.recv(BUFSIZE)
    global num
    num=num+1
    cmd_message="\nnum:%02d,send_GEMROC_CFG_CMD_PKT,Target GEMROC:%d\n" %(num,TARGET_GEMROC_ID_param)
    my_log.write(cmd_message)
    my_log.write(binascii.b2a_hex(buffer_to_send))
##    (command_echo_f, source_IPADDR) = receiveSock.recvfrom(BUFSIZE) #S.C. 2018-03-08
##    cmd_message = "\nTarget GEMROC: %d; Target GEMROC IP_ADDR_LOWER_BYTE: %s; \nCMD echo (%s): \n" %(TARGET_GEMROC_ID_param, source_IPADDR, COMMAND_STRING_PARAM)
##    print (cmd_message) 
##    time.sleep(5)
    log_file.write(cmd_message)
    log_file.write(binascii.b2a_hex(command_echo_f))
#    my_log.write(cmd_message)
#    my_log.write(binascii.b2a_hex(command_echo_f))
    return command_echo_f

## acr 2018-02-19 BEGIN definining a function to call within the display loop
def display_and_log_IVT(command_echo_param, display_enable_param, log_enable_param, log_filename_param): ## acr 2018-02-23 
    L_array = array.array('I') # L is an array of unsigned long
    L_array.fromstring(command_echo_param)
    L_array.byteswap()
    #print L_array
    FEB3_T_U  = (L_array[1] >> 22) & 0Xff
    FEB2_T_U  = (L_array[2] >> 22) & 0Xff
    FEB1_T_U  = (L_array[3] >> 22) & 0Xff
    FEB0_T_U  = (L_array[4] >> 22) & 0Xff
    FEB3_VD_U = (L_array[1] >> 13) & 0X1ff
    FEB2_VD_U = (L_array[2] >> 13) & 0X1ff
    FEB1_VD_U = (L_array[3] >> 13) & 0X1ff
    FEB0_VD_U = (L_array[4] >> 13) & 0X1ff
    FEB3_ID_U = (L_array[1] >>  4) & 0X1ff
    FEB2_ID_U = (L_array[2] >>  4) & 0X1ff
    FEB1_ID_U = (L_array[3] >>  4) & 0X1ff
    FEB0_ID_U = (L_array[4] >>  4) & 0X1ff
    FEB3_VA_U = (L_array[5] >> 13) & 0X1ff
    FEB2_VA_U = (L_array[6] >> 13) & 0X1ff
    FEB1_VA_U = (L_array[7] >> 13) & 0X1ff
    FEB0_VA_U = (L_array[8] >> 13) & 0X1ff
    FEB3_IA_U = (L_array[5] >>  4) & 0X1ff
    FEB2_IA_U = (L_array[6] >>  4) & 0X1ff
    FEB1_IA_U = (L_array[7] >>  4) & 0X1ff
    FEB0_IA_U = (L_array[8] >>  4) & 0X1ff
    ROC_T_U   = (L_array[9] >> 24) & 0X3f
    FEB3_OVT_FLAG = (L_array[1] >> 3) & 0X1
    FEB3_DOVV_FLAG = (L_array[1] >> 2) & 0X1
    FEB3_DOVC_FLAG = (L_array[1] >> 1) & 0X1
    FEB3_AOVV_FLAG = (L_array[5] >> 2) & 0X1
    FEB3_AOVC_FLAG = (L_array[5] >> 1) & 0X1
    FEB2_OVT_FLAG = (L_array[2] >> 3) & 0X1
    FEB2_DOVV_FLAG = (L_array[2] >> 2) & 0X1
    FEB2_DOVC_FLAG = (L_array[2] >> 1) & 0X1
    FEB2_AOVV_FLAG = (L_array[6] >> 2) & 0X1
    FEB2_AOVC_FLAG = (L_array[6] >> 1) & 0X1
    FEB1_OVT_FLAG = (L_array[3] >> 3) & 0X1
    FEB1_DOVV_FLAG = (L_array[3] >> 2) & 0X1
    FEB1_DOVC_FLAG = (L_array[3] >> 1) & 0X1
    FEB1_AOVV_FLAG = (L_array[7] >> 2) & 0X1
    FEB1_AOVC_FLAG = (L_array[7] >> 1) & 0X1
    FEB0_OVT_FLAG = (L_array[4] >> 3) & 0X1
    FEB0_DOVV_FLAG = (L_array[4] >> 2) & 0X1
    FEB0_DOVC_FLAG = (L_array[4] >> 1) & 0X1
    FEB0_AOVV_FLAG = (L_array[8] >> 2) & 0X1
    FEB0_AOVC_FLAG = (L_array[8] >> 1) & 0X1
    ROC_OVT_FLAG   = (L_array[9] >> 17) & 0X1 #ACR 2018-03-15 
    del L_array
    T_ref_PT1000 = 25.0
    V_ADC_at_25C = 247.2
    ADC_res_mV_1LSB = 0.305
    # acr 2018-03-15 BEGIN increased the sensitivity of temperature measurement and adjusted some parameters
    #T_ADC_data_shift = 3
    T_ADC_data_shift = 2 # acr 2018-03-15 increased the sensitivity 
    # acr 2018-03-15 BEGIN increased the sensitivity T_ADC_data_shift = 3
    ##calibration_offset_mV_FEB3 = 4.0
    ##calibration_offset_mV_FEB2 = 4.0
    ##calibration_offset_mV_FEB1 = 4.0
    ##calibration_offset_mV_FEB0 = 4.0
    calibration_offset_mV_FEB3 = 1.0
    calibration_offset_mV_FEB2 = 1.0
    calibration_offset_mV_FEB1 = 1.0
    calibration_offset_mV_FEB0 = 1.0
    # acr 2018-03-15 END increased the sensitivity of temperature measurement and adjusted some parameters
    shifted_T_ADC_res_mV_1LSB = 0.305 * (2 ** T_ADC_data_shift)
    V_ADC_data_shift = 4
    shifted_V_ADC_res_mV_1LSB = 0.305 * (2 ** V_ADC_data_shift)
    deltaT_over_deltaV_ratio = 1.283
    FEB3_T = T_ref_PT1000 + (((FEB3_T_U * shifted_T_ADC_res_mV_1LSB) + calibration_offset_mV_FEB3 -V_ADC_at_25C)* deltaT_over_deltaV_ratio)
    FEB2_T = T_ref_PT1000 + (((FEB2_T_U * shifted_T_ADC_res_mV_1LSB) + calibration_offset_mV_FEB2 -V_ADC_at_25C)* deltaT_over_deltaV_ratio)
    FEB1_T = T_ref_PT1000 + (((FEB1_T_U * shifted_T_ADC_res_mV_1LSB) + calibration_offset_mV_FEB1 -V_ADC_at_25C)* deltaT_over_deltaV_ratio)
    FEB0_T = T_ref_PT1000 + (((FEB0_T_U * shifted_T_ADC_res_mV_1LSB) + calibration_offset_mV_FEB0 -V_ADC_at_25C)* deltaT_over_deltaV_ratio)
    Vout_atten_factor = 0.5
    FEB3_VD = (FEB3_VD_U * shifted_V_ADC_res_mV_1LSB) / Vout_atten_factor
    FEB2_VD = (FEB2_VD_U * shifted_V_ADC_res_mV_1LSB) / Vout_atten_factor
    FEB1_VD = (FEB1_VD_U * shifted_V_ADC_res_mV_1LSB) / Vout_atten_factor
    FEB0_VD = (FEB0_VD_U * shifted_V_ADC_res_mV_1LSB) / Vout_atten_factor
    FEB3_VA = (FEB3_VA_U * shifted_V_ADC_res_mV_1LSB) / Vout_atten_factor
    FEB2_VA = (FEB2_VA_U * shifted_V_ADC_res_mV_1LSB) / Vout_atten_factor
    FEB1_VA = (FEB1_VA_U * shifted_V_ADC_res_mV_1LSB) / Vout_atten_factor
    FEB0_VA = (FEB0_VA_U * shifted_V_ADC_res_mV_1LSB) / Vout_atten_factor
    ## acr 2018-02-28 BEGIN prototype V2 of GEMROC_IFC_CARD have INA GAIN set at 200 instead of 50!!!###
    IADC_conv_fact_INA_GAIN_50 = 8.13
    IADC_conv_fact_INA_GAIN_200 = 8.13 / 4 ## mA per LSB
    if ( GEMROC_ID < 3):
        IADC_conv_fact_INA_GAIN = IADC_conv_fact_INA_GAIN_50
    else:
        IADC_conv_fact_INA_GAIN = IADC_conv_fact_INA_GAIN_200        
    FEB3_IA = (FEB3_IA_U * IADC_conv_fact_INA_GAIN) 
    FEB2_IA = (FEB2_IA_U * IADC_conv_fact_INA_GAIN) 
    FEB1_IA = (FEB1_IA_U * IADC_conv_fact_INA_GAIN) 
    FEB0_IA = (FEB0_IA_U * IADC_conv_fact_INA_GAIN)
    FEB3_ID = (FEB3_ID_U * IADC_conv_fact_INA_GAIN) 
    FEB2_ID = (FEB2_ID_U * IADC_conv_fact_INA_GAIN) 
    FEB1_ID = (FEB1_ID_U * IADC_conv_fact_INA_GAIN) 
    FEB0_ID = (FEB0_ID_U * IADC_conv_fact_INA_GAIN) 
    ## acr 2018-02-28 END  prototype V2 have INA GAIN set at 200 instead of 50!!!###
    ROC_T_conv_fact_C_per_LSB = 1.0
    ROC_T = ROC_T_U * ROC_T_conv_fact_C_per_LSB
    if ( display_enable_param == 1):
        print'\n' + 'FEB3_T[degC]: ' + '%d; '%FEB3_T + 'FEB3_VD[mV]: ' + '%d; '%FEB3_VD + 'FEB3_ID[mA]: ' + '%d; '%FEB3_ID + 'FEB3_VA[mV]: ' + '%d; '%FEB3_VA + 'FEB3_IA[mA]: ' + '%d; '%FEB3_IA
        print'\n' + 'FEB2_T[degC]: ' + '%d; '%FEB2_T + 'FEB2_VD[mV]: ' + '%d; '%FEB2_VD + 'FEB2_ID[mA]: ' + '%d; '%FEB2_ID + 'FEB2_VA[mV]: ' + '%d; '%FEB2_VA + 'FEB2_IA[mA]: ' + '%d; '%FEB2_IA
        print'\n' + 'FEB1_T[degC]: ' + '%d; '%FEB1_T + 'FEB1_VD[mV]: ' + '%d; '%FEB1_VD + 'FEB1_ID[mA]: ' + '%d; '%FEB1_ID + 'FEB1_VA[mV]: ' + '%d; '%FEB1_VA + 'FEB1_IA[mA]: ' + '%d; '%FEB1_IA
        print'\n' + 'FEB0_T[degC]: ' + '%d; '%FEB0_T + 'FEB0_VD[mV]: ' + '%d; '%FEB0_VD + 'FEB0_ID[mA]: ' + '%d; '%FEB0_ID + 'FEB0_VA[mV]: ' + '%d; '%FEB0_VA + 'FEB0_IA[mA]: ' + '%d; '%FEB0_IA
        print'\n' + 'ROC_T: ' + '%d; '%ROC_T
        print'\n' + 'FEB3_OVT: ' + '%d; '%FEB3_OVT_FLAG + 'FEB3_DOVV: ' + '%d; '%FEB3_DOVV_FLAG + 'FEB3_DOVC: ' + '%d; '%FEB3_DOVC_FLAG + 'FEB3_AOVV: ' + '%d; '%FEB3_AOVV_FLAG + 'FEB3_AOVC: ' + '%d; '%FEB3_AOVC_FLAG
        print'\n' + 'FEB2_OVT: ' + '%d; '%FEB2_OVT_FLAG + 'FEB2_DOVV: ' + '%d; '%FEB2_DOVV_FLAG + 'FEB2_DOVC: ' + '%d; '%FEB2_DOVC_FLAG + 'FEB2_AOVV: ' + '%d; '%FEB2_AOVV_FLAG + 'FEB2_AOVC: ' + '%d; '%FEB2_AOVC_FLAG# acr 2018-10-19 END   test the functionality of the ADC read
        print'\n' + 'FEB1_OVT: ' + '%d; '%FEB1_OVT_FLAG + 'FEB1_DOVV: ' + '%d; '%FEB1_DOVV_FLAG + 'FEB1_DOVC: ' + '%d; '%FEB1_DOVC_FLAG + 'FEB1_AOVV: ' + '%d; '%FEB1_AOVV_FLAG + 'FEB1_AOVC: ' + '%d; '%FEB1_AOVC_FLAG
        print'\n' + 'FEB0_OVT: ' + '%d; '%FEB0_OVT_FLAG + 'FEB0_DOVV: ' + '%d; '%FEB0_DOVV_FLAG + 'FEB0_DOVC: ' + '%d; '%FEB0_DOVC_FLAG + 'FEB0_AOVV: ' + '%d; '%FEB0_AOVV_FLAG + 'FEB0_AOVC: ' + '%d; '%FEB0_AOVC_FLAG
        print'\n' + 'ROC_OVT: ' + '%d; '%ROC_OVT_FLAG
    if ( log_enable_param == 1):
        IVT_log_file.write('\n' + 'FEB3_T[degC]: ' + '%d; '%FEB3_T + 'FEB3_VD[mV]: ' + '%d; '%FEB3_VD + 'FEB3_ID[mA]: ' + '%d; '%FEB3_ID + 'FEB3_VA[mV]: ' + '%d; '%FEB3_VA + 'FEB3_IA[mA]: ' + '%d; '%FEB3_IA )
        IVT_log_file.write('\n' + 'FEB2_T[degC]: ' + '%d; '%FEB2_T + 'FEB2_VD[mV]: ' + '%d; '%FEB2_VD + 'FEB2_ID[mA]: ' + '%d; '%FEB2_ID + 'FEB2_VA[mV]: ' + '%d; '%FEB2_VA + 'FEB2_IA[mA]: ' + '%d; '%FEB2_IA )
        IVT_log_file.write('\n' + 'FEB1_T[degC]: ' + '%d; '%FEB1_T + 'FEB1_VD[mV]: ' + '%d; '%FEB1_VD + 'FEB1_ID[mA]: ' + '%d; '%FEB1_ID + 'FEB1_VA[mV]: ' + '%d; '%FEB1_VA + 'FEB1_IA[mA]: ' + '%d; '%FEB1_IA )
        IVT_log_file.write('\n' + 'FEB0_T[degC]: ' + '%d; '%FEB0_T + 'FEB0_VD[mV]: ' + '%d; '%FEB0_VD + 'FEB0_ID[mA]: ' + '%d; '%FEB0_ID + 'FEB0_VA[mV]: ' + '%d; '%FEB0_VA + 'FEB0_IA[mA]: ' + '%d; '%FEB0_IA )
        IVT_log_file.write('\n' + 'ROC_T: ' + '%d; '%ROC_T )
        IVT_log_file.write('\n' + 'FEB3_OVT: ' + '%d; '%FEB3_OVT_FLAG + 'FEB3_DOVV: ' + '%d; '%FEB3_DOVV_FLAG + 'FEB3_DOVC: ' + '%d; '%FEB3_DOVC_FLAG + 'FEB3_AOVV: ' + '%d; '%FEB3_AOVV_FLAG + 'FEB3_AOVC: ' + '%d; '%FEB3_AOVC_FLAG )
        IVT_log_file.write('\n' + 'FEB2_OVT: ' + '%d; '%FEB2_OVT_FLAG + 'FEB2_DOVV: ' + '%d; '%FEB2_DOVV_FLAG + 'FEB2_DOVC: ' + '%d; '%FEB2_DOVC_FLAG + 'FEB2_AOVV: ' + '%d; '%FEB2_AOVV_FLAG + 'FEB2_AOVC: ' + '%d; '%FEB2_AOVC_FLAG ) # acr 2018-10-19 END   test the functionality of the ADC read
        IVT_log_file.write('\n' + 'FEB1_OVT: ' + '%d; '%FEB1_OVT_FLAG + 'FEB1_DOVV: ' + '%d; '%FEB1_DOVV_FLAG + 'FEB1_DOVC: ' + '%d; '%FEB1_DOVC_FLAG + 'FEB1_AOVV: ' + '%d; '%FEB1_AOVV_FLAG + 'FEB1_AOVC: ' + '%d; '%FEB1_AOVC_FLAG )
        IVT_log_file.write('\n' + 'FEB0_OVT: ' + '%d; '%FEB0_OVT_FLAG + 'FEB0_DOVV: ' + '%d; '%FEB0_DOVV_FLAG + 'FEB0_DOVC: ' + '%d; '%FEB0_DOVC_FLAG + 'FEB0_AOVV: ' + '%d; '%FEB0_AOVV_FLAG + 'FEB0_AOVC: ' + '%d; '%FEB0_AOVC_FLAG )
        IVT_log_file.write('\n' + 'ROC_OVT : ' + '%d; '%ROC_OVT_FLAG)
    return
## acr 2018-02-19 END definining a function to call within the display loop

def display_log_GCfg_readback( command_echo_param, log_enable, log_fname_param): # acr 2018-03-02
    L_array = array.array('I') # L is an array of unsigned long
    L_array.fromstring(command_echo_param)
    L_array.byteswap()
    print('\nList of Global Config Register parameters read back from TIGER%d on RESPONDING GEMROC%d:'%( ((L_array[11] >> 8) & 0X7), ((L_array[0] >> 16) & 0X1f) ) ) # acr 2018-01-23
    print('\nTIGER REPLY BYTE (LOW LEVEL SERIAL PROTOCOL ERROR FLAG):%02X;' %(((L_array[11]>>16)&0xFF)) )
    print('\nBufferBias: %d' %(((L_array[1]>>24)&0x3)) )
    print('\nTDCVcasN: %d'  %(GEMconf_classes_2018v3.swap_order_N_bits(((L_array[1]>>16)&0xF),4)))
    print('\nTDCVcasP: %d'  %(GEMconf_classes_2018v3.swap_order_N_bits(((L_array[1]>>8)&0x1F),5)))
    print('\nTDCVcasPHyst: %d'  %(GEMconf_classes_2018v3.swap_order_N_bits(((L_array[1]>>0)&0x3F),6)))
    print('\nDiscFE_Ibias: %d'  %(GEMconf_classes_2018v3.swap_order_N_bits(((L_array[2]>>24)&0x3F),6)))
    print('\nBiasFE_PpreN: %d' %(((L_array[2]>>16)&0x3F)) )
    print('\nAVcasp_global: %d'  %(GEMconf_classes_2018v3.swap_order_N_bits(((L_array[2]>>8)&0x1F),5)))
    print('\nTDCcompVcas: %d'  %(GEMconf_classes_2018v3.swap_order_N_bits(((L_array[2]>>0)&0xF),4)))
    print('\nTDCIref_cs: %d'  %(GEMconf_classes_2018v3.swap_order_N_bits(((L_array[3]>>24)&0x1F),5)))
    print('\nDiscVcasN: %d'  %(GEMconf_classes_2018v3.swap_order_N_bits(((L_array[3]>>16)&0xF),4)))
    print('\nIntegVb1: %d'  %(GEMconf_classes_2018v3.swap_order_N_bits(((L_array[3]>>8)&0x3F),6)))
    print('\nBiasFE_A1: %d' %(((L_array[3]>>0)&0xF)) )
    print('\nVcasp_Vth: %d'  %(GEMconf_classes_2018v3.swap_order_N_bits(((L_array[4]>>24)&0x3F),6)))
    print('\nTAC_I_LSB: %d' %(((L_array[4]>>16)&0x1F)) )
    print('\nTDCcompVbias: %d' %(((L_array[4]>>8)&0x1F)) )
    print('\nVref_Integ: %d'  %(GEMconf_classes_2018v3.swap_order_N_bits(((L_array[4]>>0)&0x3F),6)))
    print('\nIBiasTPcal: %d'  %(GEMconf_classes_2018v3.swap_order_N_bits(((L_array[5]>>24)&0x1F),5)))
    print('\nTP_Vcal: %d'  %(GEMconf_classes_2018v3.swap_order_N_bits(((L_array[5]>>16)&0x1F),5)))
    print('\nShaperIbias: %d' %(((L_array[5]>>8)&0xF)) )
    print('\nIPostamp: %d'  %(GEMconf_classes_2018v3.swap_order_N_bits(((L_array[5]>>0)&0x1F),5)))
    print('\nTP_Vcal_ref: %d'  %(GEMconf_classes_2018v3.swap_order_N_bits(((L_array[6]>>24)&0x1F),5)))
    print('\nVref_integ_diff: %d'  %(GEMconf_classes_2018v3.swap_order_N_bits(((L_array[6]>>16)&0x3F),6)))
    print('\nSig_pol: %d' %(((L_array[6]>>8)&0x1)) )
    print('\nFE_TPEnable: %d' %(((L_array[6]>>0)&0x1)) )
    print('\nDataClkDiv: %d' %(((L_array[7]>>16)&0x3)) )
    print('\nTACrefreshPeriod: %d' %(((L_array[7]>>8)&0xF)) )
    print('\nTACrefreshEnable: %d' %(((L_array[7]>>0)&0x1)) )
    print('\nCounterPeriod: %d' %(((L_array[8]>>24)&0x7)) )
    print('\nCounterEnable: %d' %(((L_array[8]>>16)&0x1)) )
    print('\nStopRampEnable: %d' %(((L_array[8]>>8)&0x3)) )
    print('\nRClkEnable: %d' %(((L_array[8]>>0)&0x1F)) )
    print('\nTDCClkdiv: %d' %(((L_array[9]>>24)&0x1)) )
    print('\nVetoMode: %d' %(((L_array[9]>>16)&0x3F)) )
    print('\nCh_DebugMode: %d' %(((L_array[9]>>8)&0x1)) )
    print('\nTxMode: %d' %(((L_array[9]>>0)&0x3)) )
    print('\nTxDDR: %d' %(((L_array[10]>>24)&0x1)) )
    print('\nTxLinks: %d' %(((L_array[10]>>16)&0x3)) )
    if (log_enable == 1):
        log_fname_param.write('\nList of Global Config Register parameters read back from TIGER%d on RESPONDING GEMROC%d:'%( ((L_array[11] >> 8) & 0X7), ((L_array[0] >> 16) & 0X1f) ) )# acr 2018-01-23
        log_fname_param.write("\nTIGER REPLY BYTE (LOW LEVEL SERIAL PROTOCOL ERROR FLAG):%02X;" %(((L_array[11] >> 16) & 0XFF) ) )
        log_fname_param.write('\nBufferBias: %d' %(((L_array[1]>>24)&0x3)) )
        log_fname_param.write('\nTDCVcasN: %d'  %(GEMconf_classes_2018v3.swap_order_N_bits(((L_array[1]>>16)&0xF),4)))
        log_fname_param.write('\nTDCVcasP: %d'  %(GEMconf_classes_2018v3.swap_order_N_bits(((L_array[1]>>8)&0x1F),5)))
        log_fname_param.write('\nTDCVcasPHyst: %d'  %(GEMconf_classes_2018v3.swap_order_N_bits(((L_array[1]>>0)&0x3F),6)))
        log_fname_param.write('\nDiscFE_Ibias: %d'  %(GEMconf_classes_2018v3.swap_order_N_bits(((L_array[2]>>24)&0x3F),6)))
        log_fname_param.write('\nBiasFE_PpreN: %d' %(((L_array[2]>>16)&0x3F)) )
        log_fname_param.write('\nAVcasp_global: %d'  %(GEMconf_classes_2018v3.swap_order_N_bits(((L_array[2]>>8)&0x1F),5)))
        log_fname_param.write('\nTDCcompVcas: %d'  %(GEMconf_classes_2018v3.swap_order_N_bits(((L_array[2]>>0)&0xF),4)))
        log_fname_param.write('\nTDCIref_cs: %d'  %(GEMconf_classes_2018v3.swap_order_N_bits(((L_array[3]>>24)&0x1F),5)))
        log_fname_param.write('\nDiscVcasN: %d'  %(GEMconf_classes_2018v3.swap_order_N_bits(((L_array[3]>>16)&0xF),4)))
        log_fname_param.write('\nIntegVb1: %d'  %(GEMconf_classes_2018v3.swap_order_N_bits(((L_array[3]>>8)&0x3F),6)))
        log_fname_param.write('\nBiasFE_A1: %d' %(((L_array[3]>>0)&0xF)) )
        log_fname_param.write('\nVcasp_Vth: %d'  %(GEMconf_classes_2018v3.swap_order_N_bits(((L_array[4]>>24)&0x3F),6)))
        log_fname_param.write('\nTAC_I_LSB: %d' %(((L_array[4]>>16)&0x1F)) )
        log_fname_param.write('\nTDCcompVbias: %d' %(((L_array[4]>>8)&0x1F)) )
        log_fname_param.write('\nVref_Integ: %d'  %(GEMconf_classes_2018v3.swap_order_N_bits(((L_array[4]>>0)&0x3F),6)))
        log_fname_param.write('\nIBiasTPcal: %d'  %(GEMconf_classes_2018v3.swap_order_N_bits(((L_array[5]>>24)&0x1F),5)))
        log_fname_param.write('\nTP_Vcal: %d'  %(GEMconf_classes_2018v3.swap_order_N_bits(((L_array[5]>>16)&0x1F),5)))
        log_fname_param.write('\nShaperIbias: %d' %(((L_array[5]>>8)&0xF)) )
        log_fname_param.write('\nIPostamp: %d'  %(GEMconf_classes_2018v3.swap_order_N_bits(((L_array[5]>>0)&0x1F),5)))
        log_fname_param.write('\nTP_Vcal_ref: %d'  %(GEMconf_classes_2018v3.swap_order_N_bits(((L_array[6]>>24)&0x1F),5)))
        log_fname_param.write('\nVref_integ_diff: %d'  %(GEMconf_classes_2018v3.swap_order_N_bits(((L_array[6]>>16)&0x3F),6)))
        log_fname_param.write('\nSig_pol: %d' %(((L_array[6]>>8)&0x1)) )
        log_fname_param.write('\nFE_TPEnable: %d' %(((L_array[6]>>0)&0x1)) )
        log_fname_param.write('\nDataClkDiv: %d' %(((L_array[7]>>16)&0x3)) )
        log_fname_param.write('\nTACrefreshPeriod: %d' %(((L_array[7]>>8)&0xF)) )
        log_fname_param.write('\nTACrefreshEnable: %d' %(((L_array[7]>>0)&0x1)) )
        log_fname_param.write('\nCounterPeriod: %d' %(((L_array[8]>>24)&0x7)) )
        log_fname_param.write('\nCounterEnable: %d' %(((L_array[8]>>16)&0x1)) )
        log_fname_param.write('\nStopRampEnable: %d' %(((L_array[8]>>8)&0x3)) )
        log_fname_param.write('\nRClkEnable: %d' %(((L_array[8]>>0)&0x1F)) )
        log_fname_param.write('\nTDCClkdiv: %d' %(((L_array[9]>>24)&0x1)) )
        log_fname_param.write('\nVetoMode: %d' %(((L_array[9]>>16)&0x3F)) )
        log_fname_param.write('\nCh_DebugMode: %d' %(((L_array[9]>>8)&0x1)) )
        log_fname_param.write('\nTxMode: %d' %(((L_array[9]>>0)&0x3)) )
        log_fname_param.write('\nTxDDR: %d' %(((L_array[10]>>24)&0x1)) )
        log_fname_param.write('\nTxLinks: %d' %(((L_array[10]>>16)&0x3)) )

def display_log_ChCfg_readback( command_echo_param, log_enable, log_fname_param): # acr 2018-03-02
    L_array = array.array('I') # L is an array of unsigned long
    L_array.fromstring(command_echo_param)
    L_array.byteswap()
    print('\nList of Channel %d Config Register parameters read back from TIGER%d on RESPONDING GEMROC%d:'%( ((L_array[9] >> 0) & 0X3F), ((L_array[9] >> 8) & 0X7), ((L_array[0] >> 16) & 0X1f) ) )# acr 2018-01-23
    print("\nTIGER REPLY BYTE (LOW LEVEL SERIAL PROTOCOL ERROR FLAG):%02X;"                           %( ((L_array[9] >> 16)&0Xff) ) )
    print('\n DisableHyst: %d' %((L_array[1]>>24)&0x1) )
    print('\n T2Hyst: %d' %((L_array[1]>>16)&0x7) )
    print('\n T1Hyst: %d' %((L_array[1]>>8)&0x7) )
    print('\n Ch63ObufMSB: %d' %((L_array[1]>>0)&0x1) )
    print('\n TP_disable_FE: %d' %((L_array[2]>>24)&0x1) )
    print('\n TDC_IB_E: %d' %((L_array[2]>>16)&0xF) )
    print('\n TDC_IB_T: %d' %((L_array[2]>>8)&0xF) )
    print('\n Integ: %d' %((L_array[2]>>0)&0x1) )
    print('\n PostAmpGain: %d' %((L_array[3]>>24)&0x3) )
    print('\n FE_delay: %d' %((L_array[3]>>16)&0x1F) )
    print('\n Vth_T2: %d' %((L_array[3]>>8)&0x3F) )
    print('\n Vth_T1: %d' %((L_array[3]>>0)&0x3F) )
    print('\n QTx2Enable: %d' %((L_array[4]>>24)&0x1) )
    print('\n MaxIntegTime: %d' %((L_array[4]>>16)&0x7F) )
    print('\n MinIntegTime: %d' %((L_array[4]>>8)&0x7F) )
    print('\n TriggerBLatched: %d' %((L_array[4]>>0)&0x1) )
    print('\n QdcMode: %d' %((L_array[5]>>24)&0x1) )
    print('\n BranchEnableT: %d' %((L_array[5]>>16)&0x1) )
    print('\n BranchEnableEQ: %d' %((L_array[5]>>8)&0x1) )
    print('\n TriggerMode2B: %d' %((L_array[5]>>0)&0x7) )
    print('\n TriggerMode2Q: %d' %((L_array[6]>>24)&0x3) )
    print('\n TriggerMode2E: %d' %((L_array[6]>>16)&0x7) )
    print('\n TriggerMode2T: %d' %((L_array[6]>>8)&0x3) )
    print('\n TACMinAge: %d' %((L_array[6]>>0)&0x1F) )
    print('\n TACMaxAge: %d' %((L_array[7]>>24)&0x1F) )
    print('\n CounterMode: %d' %((L_array[7]>>16)&0xF) )
    print('\n DeadTime: %d' %((L_array[7]>>8)&0x3F) )
    print('\n SyncChainLen: %d' %((L_array[7]>>0)&0x3) )
    print('\n Ch_DebugMode: %d' %((L_array[8]>>24)&0x3) )
    print('\n TriggerMode: %d' %((L_array[8]>>16)&0x3) )
    if (log_enable == 1):
        log_fname_param.write('\nList of Channel %d Config Register parameters read back from TIGER%d on RESPONDING GEMROC%d:'%( ((L_array[9] >> 0) & 0X3F), ((L_array[9] >> 8) & 0X7), ((L_array[0] >> 16) & 0X1f) ) )# acr 2018-01-23
        log_fname_param.write("\nTIGER REPLY BYTE (LOW LEVEL SERIAL PROTOCOL ERROR FLAG):%02X;"                           %( ((L_array[9] >> 16)&0Xff) ) )
        log_fname_param.write('\n DisableHyst: %d' %((L_array[1]>>24)&0x1) )
        log_fname_param.write('\n T2Hyst: %d' %((L_array[1]>>16)&0x7) )
        log_fname_param.write('\n T1Hyst: %d' %((L_array[1]>>8)&0x7) )
        log_fname_param.write('\n Ch63ObufMSB: %d' %((L_array[1]>>0)&0x1) )
        log_fname_param.write('\n TP_disable_FE: %d' %((L_array[2]>>24)&0x1) )
        log_fname_param.write('\n TDC_IB_E: %d' %((L_array[2]>>16)&0xF) )
        log_fname_param.write('\n TDC_IB_T: %d' %((L_array[2]>>8)&0xF) )
        log_fname_param.write('\n Integ: %d' %((L_array[2]>>0)&0x1) )
        log_fname_param.write('\n PostAmpGain: %d' %((L_array[3]>>24)&0x3) )
        log_fname_param.write('\n FE_delay: %d' %((L_array[3]>>16)&0x1F) )
        log_fname_param.write('\n Vth_T2: %d' %((L_array[3]>>8)&0x3F) )
        log_fname_param.write('\n Vth_T1: %d' %((L_array[3]>>0)&0x3F) )
        log_fname_param.write('\n QTx2Enable: %d' %((L_array[4]>>24)&0x1) )
        log_fname_param.write('\n MaxIntegTime: %d' %((L_array[4]>>16)&0x7F) )
        log_fname_param.write('\n MinIntegTime: %d' %((L_array[4]>>8)&0x7F) )
        log_fname_param.write('\n TriggerBLatched: %d' %((L_array[4]>>0)&0x1) )
        log_fname_param.write('\n QdcMode: %d' %((L_array[5]>>24)&0x1) )
        log_fname_param.write('\n BranchEnableT: %d' %((L_array[5]>>16)&0x1) )
        log_fname_param.write('\n BranchEnableEQ: %d' %((L_array[5]>>8)&0x1) )
        log_fname_param.write('\n TriggerMode2B: %d' %((L_array[5]>>0)&0x7) )
        log_fname_param.write('\n TriggerMode2Q: %d' %((L_array[6]>>24)&0x3) )
        log_fname_param.write('\n TriggerMode2E: %d' %((L_array[6]>>16)&0x7) )
        log_fname_param.write('\n TriggerMode2T: %d' %((L_array[6]>>8)&0x3) )
        log_fname_param.write('\n TACMinAge: %d' %((L_array[6]>>0)&0x1F) )
        log_fname_param.write('\n TACMaxAge: %d' %((L_array[7]>>24)&0x1F) )
        log_fname_param.write('\n CounterMode: %d' %((L_array[7]>>16)&0xF) )
        log_fname_param.write('\n DeadTime: %d' %((L_array[7]>>8)&0x3F) )
        log_fname_param.write('\n SyncChainLen: %d' %((L_array[7]>>0)&0x3) )
        log_fname_param.write('\n Ch_DebugMode: %d' %((L_array[8]>>24)&0x3) )
        log_fname_param.write('\n TriggerMode: %d' %((L_array[8]>>16)&0x3) )

def send_TIGER_GCFG_Reg_CMD_PKT( TIGER_ID_param, COMMAND_STRING_PARAM, array_to_send_param, DEST_IP_ADDRESS_PARAM, DEST_PORT_NO_PARAM, log_fname_param): #acr 2018-03-05
    buffer_to_send = struct.pack('>' + str(len(array_to_send_param)) + 'I', *array_to_send_param)
    clientSock.sendto(buffer_to_send, (DEST_IP_ADDRESS_PARAM, DEST_PORT_NO_PARAM))
    cmd_message = '\nTIGER%d Global Cfg Reg CMD %s sent:\n' %(TIGER_ID_param, COMMAND_STRING_PARAM)
    log_fname_param.write(cmd_message)
    log_fname_param.write(binascii.b2a_hex(buffer_to_send))
    global num
    num=num+1
    cmd_message="\nnum:%02d,send_TIGER_GCFG_Reg_CMD_PKT,TIGER_ID:%d\n" % (num,TIGER_ID_param)
    my_log.write(cmd_message)
    my_log.write(binascii.b2a_hex(buffer_to_send))
    cmd_message = '\nTIGER%d Global Cfg Reg CMD %s command echo:\n' %(TIGER_ID_param, COMMAND_STRING_PARAM)
    log_fname_param.write(cmd_message)
#    my_log.write(cmd_message)
    command_echo_f = receiveSock.recv(BUFSIZE)
    log_fname_param.write(binascii.b2a_hex(command_echo_f))
#    my_log.write(binascii.b2a_hex(command_echo_f))
    return command_echo_f

def send_TIGER_Ch_CFG_Reg_CMD_PKT( TIGER_ID_param, COMMAND_STRING_PARAM, array_to_send_param, DEST_IP_ADDRESS_PARAM, DEST_PORT_NO_PARAM, log_fname_param): #acr 2018-03-04
    buffer_to_send = struct.pack('>' + str(len(array_to_send_param)) + 'I', *array_to_send_param)
    clientSock.sendto(buffer_to_send, (DEST_IP_ADDRESS_PARAM, DEST_PORT_NO_PARAM))
    Target_Ch_ToALL_f = (array_to_send_param[(len(array_to_send_param)-1)] >> 6) & 0x1
    Target_Ch_ID_f    = (array_to_send_param[(len(array_to_send_param)-1)] >> 0) & 0x3F
    cmd_message = '\nTIGER% d; TOALL = % d; Channel% s Cfg Reg CMD %s sent\n' %(TIGER_ID_param, Target_Ch_ToALL_f, Target_Ch_ID_f, COMMAND_STRING_PARAM) 
    log_fname_param.write(cmd_message)
    log_fname_param.write(binascii.b2a_hex(buffer_to_send))
    global num
    num=num+1
    cmd_message="\nnum:%02d,send_TIGER_Ch_CFG_Reg_CMD_PKT,TIGER_ch:%d\n" % (num,TIGER_ID_param)
    my_log.write(cmd_message)
    my_log.write(binascii.b2a_hex(buffer_to_send))
    cmd_message = '\nTIGER% d; TOALL = % d; Channel% s Cfg Reg CMD %s echo\n' %(TIGER_ID_param, Target_Ch_ToALL_f, Target_Ch_ID_f, COMMAND_STRING_PARAM) 
    log_fname_param.write(cmd_message)
#    my_log.write(cmd_message)
    command_echo_f = receiveSock.recv(BUFSIZE)
    log_fname_param.write(binascii.b2a_hex(command_echo_f))
#    my_log.write(binascii.b2a_hex(command_echo_f))
    return command_echo_f

def GEMROC_IVT_read_and_log(GEMROC_ID_param, display_enable_param, log_enable_param, log_filename_param):
    gemroc_LV_XX.set_target_GEMROC(GEMROC_ID_param)
    COMMAND_STRING = 'CMD_GEMROC_LV_IVT_READ'
    gemroc_LV_XX.set_gemroc_cmd_code(COMMAND_STRING,1)
    gemroc_LV_XX.update_command_words()
    array_to_send = gemroc_LV_XX.command_words
    command_echo = send_GEMROC_CFG_CMD_PKT( GEMROC_ID_param, COMMAND_STRING, array_to_send, DEST_IP_ADDRESS, DEST_PORT_NO )
    #print '\nGEMROC_ID: %d: CMD_GEMROC_LV_IVT_READ' %GEMROC_ID_param
    display_and_log_IVT(command_echo, display_enable_param, log_enable_param, log_filename_param)
    return

def send_GEMROC_LV_CMD (GEMROC_ID_param, COMMAND_STRING_PARAM):
    gemroc_LV_XX.set_target_GEMROC(GEMROC_ID_param)
    gemroc_LV_XX.set_gemroc_cmd_code(COMMAND_STRING_PARAM,1)
    gemroc_LV_XX.update_command_words()
    array_to_send = gemroc_LV_XX.command_words
    command_echo = send_GEMROC_CFG_CMD_PKT( GEMROC_ID_param, COMMAND_STRING_PARAM, array_to_send, DEST_IP_ADDRESS, DEST_PORT_NO )
    return command_echo

def FEBPwrEnPattern_set(GEMROC_ID_param, FEB_PWREN_pattern_param):    
    gemroc_LV_XX.set_target_GEMROC(GEMROC_ID_param)
    gemroc_LV_XX.set_FEB_PWR_EN_pattern(FEB_PWREN_pattern_param)    
    COMMAND_STRING = 'CMD_GEMROC_LV_CFG_WR'
    command_echo = send_GEMROC_LV_CMD (GEMROC_ID_param, COMMAND_STRING)
    return command_echo

def set_FEB_timing_delays(GEMROC_ID_param, FEB3_TDly, FEB2_TDly, FEB1_TDly, FEB0_TDly):
    gemroc_LV_XX.set_target_GEMROC(GEMROC_ID_param)
    gemroc_LV_XX.set_timing_toFEB_delay(FEB3_TDly, FEB2_TDly, FEB1_TDly, FEB0_TDly)    
    COMMAND_STRING = 'CMD_GEMROC_LV_CFG_WR'
    command_echo = send_GEMROC_LV_CMD (GEMROC_ID_param, COMMAND_STRING)
    return command_echo

# acr 2018-03-06 changed order of parameters of send_GEMROC_DAQ_CMD
def send_GEMROC_DAQ_CMD(GEMROC_ID_param, gemroc_DAQ_inst_param, COMMAND_STRING_PARAM): 
    gemroc_DAQ_inst_param.set_target_GEMROC(GEMROC_ID_param)
    gemroc_DAQ_inst_param.set_gemroc_cmd_code(COMMAND_STRING_PARAM,1)
    gemroc_DAQ_inst_param.update_command_words()
    array_to_send = gemroc_DAQ_inst_param.command_words
    command_echo = send_GEMROC_CFG_CMD_PKT( GEMROC_ID_param, COMMAND_STRING_PARAM, array_to_send, DEST_IP_ADDRESS, DEST_PORT_NO )
    return command_echo

def ResetTgtGEMROC_ALL_TIGER_GCfgReg (GEMROC_ID_param, gemroc_DAQ_inst_param):
    COMMAND_STRING = 'CMD_GEMROC_DAQ_TIGER_GCFGREG_RESET'
    command_echo = send_GEMROC_DAQ_CMD(GEMROC_ID_param, gemroc_DAQ_inst_param, COMMAND_STRING)
    print'\n CMD_GEMROC_DAQ_TIGER_GCFGREG_RESET'
    time.sleep(5)
    
def WriteTgtGEMROC_TIGER_GCfgReg_fromfile (GCFGReg_setting_inst, GEMROC_ID_param, TIGER_ID_param, GCFGReg_def_fname_param, log_fname_param):
    GCFGReg_setting_inst.reload_gcfg_settings_from_file(GCFGReg_def_fname_param) ## acr 2018-02-23 new method to reaload from a default file
    GCFGReg_setting_inst.set_target_GEMROC(GEMROC_ID_param)
    GCFGReg_setting_inst.set_target_TIGER(TIGER_ID_param)
    COMMAND_STRING = 'WR'
    GCFGReg_setting_inst.set_command_code(COMMAND_STRING)
    GCFGReg_setting_inst.update_command_words()
    log_fname_param.write('\nList of Global Config Register settings to TIGER%d of RESPONDING GEMROC%d:'%( TIGER_ID_param, GEMROC_ID_param) )
    classes_test_functions.print_log_GReg_members(GCFGReg_setting_inst, 1, log_fname_param)
    returned_array = classes_test_functions.get_GReg_GEMROC_words( GCFGReg_setting_inst )
    #keep print '\nGCFG_168_137: ' + "%08X" %returned_array[0]
    #keep print '\nGCFG_136_105: ' + "%08X" %returned_array[1]
    #keep print '\nGCFG_104_73:  ' + "%08X" %returned_array[2]
    #keep print '\nGCFG_72_41:   ' + "%08X" %returned_array[3]
    #keep print '\nGCFG_40_9:    ' + "%08X" %returned_array[4]
    #keep print '\nGCFG_8_m23:   ' + "%08X" %returned_array[5]    
    #keep print '\nG_reg_inst WR bitstring_TO_format:'
    print('\nList of Global Config Register settings to TIGER%d of RESPONDING GEMROC%d in Torino string format:'%( TIGER_ID_param, GEMROC_ID_param) )
    log_fname_param.write('\nList of Global Config Register settings to TIGER%d of RESPONDING GEMROC%d in Torino string format:'%( TIGER_ID_param, GEMROC_ID_param) )
    classes_test_functions.print_GReg_bitstring_TO_format(returned_array, 1, log_fname_param)    
    array_to_send = GCFGReg_setting_inst.command_words
    command_echo = send_TIGER_GCFG_Reg_CMD_PKT( TIGER_ID_param, COMMAND_STRING, array_to_send, DEST_IP_ADDRESS, DEST_PORT_NO, log_fname_param)
    return command_echo

def set_FE_TPEnable(GCFGReg_setting_inst, GEMROC_ID_param, TIGER_ID_param, FE_TPEnable_param, log_fname_param):    
    GCFGReg_setting_inst.set_FE_TPEnable(FE_TPEnable_param)    
    GCFGReg_setting_inst.set_target_GEMROC(GEMROC_ID_param)
    GCFGReg_setting_inst.set_target_TIGER(TIGER_ID_param)
    COMMAND_STRING = 'WR'
    GCFGReg_setting_inst.set_command_code(COMMAND_STRING)
    GCFGReg_setting_inst.update_command_words()
    array_to_send = GCFGReg_setting_inst.command_words
    command_echo = send_TIGER_GCFG_Reg_CMD_PKT( TIGER_ID_param, COMMAND_STRING, array_to_send, DEST_IP_ADDRESS, DEST_PORT_NO, log_fname_param)
    print('\nFE_TPEnable of Global CFG reg of TIGER%d of RESPONDING GEMROC%d: set to %d'%( TIGER_ID_param, GEMROC_ID_param, FE_TPEnable_param ) )
    log_fname_param.write('\nFE_TPEnable of Global CFG reg of TIGER%d of RESPONDING GEMROC%d: set to %d'%( TIGER_ID_param, GEMROC_ID_param, FE_TPEnable_param ) )
    return command_echo

def ReadTgtGEMROC_TIGER_GCfgReg (GCFGReg_setting_inst, GEMROC_ID_param, TIGER_ID_param, log_fname_param):
    GCFGReg_setting_inst.set_target_GEMROC(GEMROC_ID_param)
    GCFGReg_setting_inst.set_target_TIGER(TIGER_ID_param)
    COMMAND_STRING = 'RD'
    GCFGReg_setting_inst.set_command_code(COMMAND_STRING)
    GCFGReg_setting_inst.update_command_words()
    array_to_send = GCFGReg_setting_inst.command_words
    command_echo = send_TIGER_GCFG_Reg_CMD_PKT( TIGER_ID_param, COMMAND_STRING, array_to_send, DEST_IP_ADDRESS, DEST_PORT_NO, log_fname_param )
    return command_echo

def WriteTgtGEMROC_TIGER_ChCfgReg_fromfile (ChCFGReg_setting_inst, GEMROC_ID_param, TIGER_ID_param, channel_ID_param, ChCFGReg_def_fname_param, log_fname_param): 
    ChCFGReg_setting_inst.reload_chcfg_settings_from_file(ChCFGReg_def_fname_param) ## acr 2018-02-23 new method to reaload from a default file
    ChCFGReg_setting_inst.set_target_GEMROC(GEMROC_ID_param)
    ChCFGReg_setting_inst.set_target_TIGER(TIGER_ID_param)
    ChCFGReg_setting_inst.set_to_ALL_param (0) ## let's do multiple configuration under script control rather than under GEMROC NIOS2 processor control
    COMMAND_STRING = 'WR'
    ChCFGReg_setting_inst.set_command_code(COMMAND_STRING)
    if (channel_ID_param < 64):
        ChCFGReg_setting_inst.set_target_channel(channel_ID_param)
        ChCFGReg_setting_inst.update_command_words()
        array_to_send = ChCFGReg_setting_inst.command_words
        command_echo = send_TIGER_Ch_CFG_Reg_CMD_PKT(TIGER_ID_param, COMMAND_STRING, array_to_send, DEST_IP_ADDRESS, DEST_PORT_NO, log_fname_param )
    else:
        for i in range (0, 64):
            ChCFGReg_setting_inst.set_target_channel(i)
            ChCFGReg_setting_inst.update_command_words()
            array_to_send = ChCFGReg_setting_inst.command_words
            command_echo = send_TIGER_Ch_CFG_Reg_CMD_PKT(TIGER_ID_param, COMMAND_STRING, array_to_send, DEST_IP_ADDRESS, DEST_PORT_NO, log_fname_param )
    return command_echo

def ReadTgtGEMROC_TIGER_ChCfgReg (ChCFGReg_setting_inst, GEMROC_ID_param, TIGER_ID_param, channel_ID_param, verbose_mode, log_fname_param):
    ChCFGReg_setting_inst.set_target_GEMROC(GEMROC_ID_param)
    ChCFGReg_setting_inst.set_target_TIGER(TIGER_ID_param)
    ChCFGReg_setting_inst.set_target_channel(channel_ID_param)
    COMMAND_STRING = 'RD'
    ChCFGReg_setting_inst.set_command_code(COMMAND_STRING)
    array_to_send = ChCFGReg_setting_inst.command_words
    if (channel_ID_param < 64):
        ChCFGReg_setting_inst.set_target_channel(channel_ID_param)
        ChCFGReg_setting_inst.update_command_words()
        array_to_send = ChCFGReg_setting_inst.command_words
        command_echo = send_TIGER_Ch_CFG_Reg_CMD_PKT(TIGER_ID_param, COMMAND_STRING, array_to_send, DEST_IP_ADDRESS, DEST_PORT_NO, log_fname_param)
        if (verbose_mode == 1):
            display_log_ChCfg_readback(command_echo, 1, log_fname_param)
    else:
        for i in range (0, 64):
            ChCFGReg_setting_inst.set_target_channel(i)
            ChCFGReg_setting_inst.update_command_words()
            array_to_send = ChCFGReg_setting_inst.command_words
            command_echo = send_TIGER_Ch_CFG_Reg_CMD_PKT(TIGER_ID_param, COMMAND_STRING, array_to_send, DEST_IP_ADDRESS, DEST_PORT_NO, log_fname_param )
            if (verbose_mode == 1):
                display_log_ChCfg_readback(command_echo, 1, log_file)
    return command_echo

def Set_GEMROC_TIGER_ch_TPEn (ChCFGReg_setting_inst, GEMROC_ID_param, TIGER_ID_param, Channel_ID_param, TP_disable_FE_param, TriggerMode_param, log_fname_param):
    ChCFGReg_setting_inst.set_target_GEMROC(GEMROC_ID_param)
    ChCFGReg_setting_inst.set_target_TIGER(TIGER_ID_param)
    ChCFGReg_setting_inst.set_to_ALL_param (0) ## let's do multiple configuration under script control rather than under GEMROC NIOS2 processor control
    COMMAND_STRING = 'WR'
    ChCFGReg_setting_inst.set_command_code(COMMAND_STRING)
    if (Channel_ID_param < 64):
        ChCFGReg_setting_inst.set_target_channel(Channel_ID_param)
        ChCFGReg_setting_inst.set_TP_disable_FE(TP_disable_FE_param) # ACR 2018-03-04
        ChCFGReg_setting_inst.TriggerMode = TriggerMode_param     
        ChCFGReg_setting_inst.update_command_words()
        array_to_send = ChCFGReg_setting_inst.command_words
        command_echo = send_TIGER_Ch_CFG_Reg_CMD_PKT(TIGER_ID_param, COMMAND_STRING, array_to_send, DEST_IP_ADDRESS, DEST_PORT_NO, log_fname_param )
    else:
        for i in range (0, 64):
            ChCFGReg_setting_inst.set_target_channel(i)
            ChCFGReg_setting_inst.set_TP_disable_FE(TP_disable_FE_param)
            ChCFGReg_setting_inst.TriggerMode = TriggerMode_param
            ChCFGReg_setting_inst.update_command_words()
            array_to_send = ChCFGReg_setting_inst.command_words
            command_echo = send_TIGER_Ch_CFG_Reg_CMD_PKT(TIGER_ID_param, COMMAND_STRING, array_to_send, DEST_IP_ADDRESS, DEST_PORT_NO, log_fname_param )
    last_command_echo = command_echo
    return last_command_echo

def Set_Vth_T1 (ChCFGReg_setting_inst, GEMROC_ID_param, TIGER_ID_param, Channel_ID_param, VthT1_param, log_fname_param):
    ChCFGReg_setting_inst.set_target_GEMROC(GEMROC_ID_param)
    ChCFGReg_setting_inst.set_target_TIGER(TIGER_ID_param)
    ChCFGReg_setting_inst.set_to_ALL_param (0) ## let's do multiple configuration under script control rather than under GEMROC NIOS2 processor control
    ChCFGReg_setting_inst.set_Vth_T1(VthT1_param)
    COMMAND_STRING = 'WR'
    ChCFGReg_setting_inst.set_command_code(COMMAND_STRING)
    if (Channel_ID_param < 64):
        ChCFGReg_setting_inst.set_target_channel(Channel_ID_param)
        ChCFGReg_setting_inst.update_command_words()
        array_to_send = ChCFGReg_setting_inst.command_words
        command_echo = send_TIGER_Ch_CFG_Reg_CMD_PKT(TIGER_ID_param, COMMAND_STRING, array_to_send, DEST_IP_ADDRESS, DEST_PORT_NO, log_fname_param )
    else:
        for i in range (0, 64):
            ChCFGReg_setting_inst.set_target_channel(i)
            ChCFGReg_setting_inst.update_command_words()
            array_to_send = ChCFGReg_setting_inst.command_words
            command_echo = send_TIGER_Ch_CFG_Reg_CMD_PKT(TIGER_ID_param, COMMAND_STRING, array_to_send, DEST_IP_ADDRESS, DEST_PORT_NO, log_fname_param )
    last_command_echo = command_echo
    return last_command_echo

def set_AVcasp_global(GCFGReg_setting_inst, GEMROC_ID_param, TIGER_ID_param, AVcasp_global_param, log_fname_param):    
    GCFGReg_setting_inst.set_target_GEMROC(GEMROC_ID_param)
    GCFGReg_setting_inst.set_target_TIGER(TIGER_ID_param)
    GCFGReg_setting_inst.set_AVcasp_global(AVcasp_global_param)    
    COMMAND_STRING = 'WR'
    GCFGReg_setting_inst.set_command_code(COMMAND_STRING)
    GCFGReg_setting_inst.update_command_words()
    array_to_send = GCFGReg_setting_inst.command_words
    command_echo = send_TIGER_GCFG_Reg_CMD_PKT( TIGER_ID_param, COMMAND_STRING, array_to_send, DEST_IP_ADDRESS, DEST_PORT_NO, log_fname_param )
    print('\nAVcasp_global of Global CFG reg of TIGER%d of RESPONDING GEMROC%d: set to %d'%( TIGER_ID_param, GEMROC_ID_param, AVcasp_global_param ) )
    log_fname_param.write('\nAVcasp_global of Global CFG reg of TIGER%d of RESPONDING GEMROC%d: set to %d'%( TIGER_ID_param, GEMROC_ID_param, AVcasp_global_param ) )
    return command_echo

def SynchReset_to_TgtFEB(gemroc_DAQ_inst, GEMROC_ID_param, TargetFEB_param, To_ALL_param):
    gemroc_DAQ_inst.set_target_GEMROC(GEMROC_ID_param)
    gemroc_DAQ_inst.set_target_TCAM_ID(TargetFEB_param, To_ALL_param)
    COMMAND_STRING = 'CMD_GEMROC_DAQ_TIGER_SYNCH_RST'
    command_echo = send_GEMROC_DAQ_CMD(GEMROC_ID_param, gemroc_DAQ_inst, COMMAND_STRING)
    return command_echo

def SynchReset_to_TgtTCAM(gemroc_DAQ_inst, GEMROC_ID_param, TargetTCAM_param, To_ALL_param):
    gemroc_DAQ_inst.set_target_GEMROC(GEMROC_ID_param)
    gemroc_DAQ_inst.set_target_TCAM_ID(TargetTCAM_param, To_ALL_param)
    COMMAND_STRING = 'CMD_GEMROC_DAQ_TCAM_SYNCH_RST'
    command_echo = send_GEMROC_DAQ_CMD(GEMROC_ID_param, gemroc_DAQ_inst, COMMAND_STRING)
    return command_echo

def DAQ_set_and_TL_start(gemroc_DAQ_inst, GEMROC_ID_param, TCAM_Enable_pattern_param, Per_FEB_TP_Enable_pattern_param):
    gemroc_DAQ_inst.set_target_GEMROC(GEMROC_ID_param)
    gemroc_DAQ_inst.set_EN_TM_TCAM_pattern (TCAM_Enable_pattern_param)
    gemroc_DAQ_inst.set_TP_width(5) 
    gemroc_DAQ_inst.set_AUTO_TP_EN_pattern(0x0)
    gemroc_DAQ_inst.set_Periodic_TP_EN_pattern(Per_FEB_TP_Enable_pattern_param)
    gemroc_DAQ_inst.set_TL_nTM_ACQ(1)
    gemroc_DAQ_inst.set_TP_Pos_nNeg(1)
    gemroc_DAQ_inst.set_TP_period(8)
    COMMAND_STRING = 'CMD_GEMROC_DAQ_CFG_WR'
    command_echo = send_GEMROC_DAQ_CMD(GEMROC_ID_param, gemroc_DAQ_inst, COMMAND_STRING)
    return command_echo

def Set_OV_OC_OT_PWR_CUT_EN_FLAGS(gemroc_inst_param, GEMROC_ID_param, FEB_OVC_EN_pattern_param, FEB_OVV_EN_pattern_param, FEB_OVT_EN_pattern_param, ROC_OVT_EN_param, log_file_param):
    gemroc_inst_param.FEB_OVC_EN_pattern = FEB_OVC_EN_pattern_param & 0xF
    gemroc_inst_param.FEB_OVV_EN_pattern = FEB_OVV_EN_pattern_param & 0xF
    gemroc_inst_param.FEB_OVT_EN_pattern = FEB_OVT_EN_pattern_param & 0xF
    gemroc_inst_param.ROC_OVT_EN = ROC_OVT_EN_param & 0x1
    COMMAND_STRING = 'CMD_GEMROC_LV_CFG_WR'
    command_echo = send_GEMROC_LV_CMD (GEMROC_ID_param, COMMAND_STRING)
    return command_echo

def Set_OVVA_LIMIT(gemroc_inst_param, GEMROC_ID_param, FEB3_OVVA_thr_param, FEB2_OVVA_thr_param, FEB1_OVVA_thr_param, FEB0_OVVA_thr_param, log_file_param):
    Vout_atten_factor = 0.5
    V_ADC_data_shift = 4
    shifted_V_ADC_res_mV_1LSB = 0.305 * (2 ** V_ADC_data_shift)
    FEB3_OVVA_thr_int = int((FEB3_OVVA_thr_param * Vout_atten_factor)/shifted_V_ADC_res_mV_1LSB)
    if (FEB3_OVVA_thr_int <= 511):
        FEB3_OVVA_thr_Unsigned8 = FEB3_OVVA_thr_int
    else:
        FEB3_OVVA_thr_Unsigned8 = 511
    FEB2_OVVA_thr_int = int((FEB2_OVVA_thr_param * Vout_atten_factor)/shifted_V_ADC_res_mV_1LSB)
    if (FEB2_OVVA_thr_int <= 511):
        FEB2_OVVA_thr_Unsigned8 = FEB2_OVVA_thr_int
    else:
        FEB2_OVVA_thr_Unsigned8 = 511
    FEB1_OVVA_thr_int = int((FEB1_OVVA_thr_param * Vout_atten_factor)/shifted_V_ADC_res_mV_1LSB)
    if (FEB1_OVVA_thr_int <= 511):
        FEB1_OVVA_thr_Unsigned8 = FEB1_OVVA_thr_int
    else:
        FEB1_OVVA_thr_Unsigned8 = 511
    FEB0_OVVA_thr_int = int((FEB0_OVVA_thr_param * Vout_atten_factor)/shifted_V_ADC_res_mV_1LSB)
    if (FEB0_OVVA_thr_int <= 511):
        FEB0_OVVA_thr_Unsigned8 = FEB0_OVVA_thr_int
    else:
        FEB0_OVVA_thr_Unsigned8 = 511
    gemroc_inst_param.A_OVV_LIMIT_FEB3 = FEB3_OVVA_thr_Unsigned8
    gemroc_inst_param.A_OVV_LIMIT_FEB2 = FEB2_OVVA_thr_Unsigned8
    gemroc_inst_param.A_OVV_LIMIT_FEB1 = FEB1_OVVA_thr_Unsigned8
    gemroc_inst_param.A_OVV_LIMIT_FEB0 = FEB0_OVVA_thr_Unsigned8
    COMMAND_STRING = 'CMD_GEMROC_LV_CFG_WR'
    command_echo = send_GEMROC_LV_CMD (GEMROC_ID_param, COMMAND_STRING)
    return command_echo

def Set_OVVD_LIMIT(gemroc_inst_param, GEMROC_ID_param, FEB3_OVVD_thr_param, FEB2_OVVD_thr_param, FEB1_OVVD_thr_param, FEB0_OVVD_thr_param, log_file_param):
    Vout_atten_factor = 0.5
    V_ADC_data_shift = 4
    shifted_V_ADC_res_mV_1LSB = 0.305 * (2 ** V_ADC_data_shift)
    FEB3_OVVD_thr_int = int((FEB3_OVVD_thr_param * Vout_atten_factor)/shifted_V_ADC_res_mV_1LSB)
    if (FEB3_OVVD_thr_int <= 511):
        FEB3_OVVD_thr_Unsigned8 = FEB3_OVVD_thr_int
    else:
        FEB3_OVVD_thr_Unsigned8 = 511
    FEB2_OVVD_thr_int = int((FEB2_OVVD_thr_param * Vout_atten_factor)/shifted_V_ADC_res_mV_1LSB)
    if (FEB2_OVVD_thr_int <= 511):
        FEB2_OVVD_thr_Unsigned8 = FEB2_OVVD_thr_int
    else:
        FEB2_OVVD_thr_Unsigned8 = 511
    FEB1_OVVD_thr_int = int((FEB1_OVVD_thr_param * Vout_atten_factor)/shifted_V_ADC_res_mV_1LSB)
    if (FEB1_OVVD_thr_int <= 511):
        FEB1_OVVD_thr_Unsigned8 = FEB1_OVVD_thr_int
    else:
        FEB1_OVVD_thr_Unsigned8 = 511
    FEB0_OVVD_thr_int = int((FEB0_OVVD_thr_param * Vout_atten_factor)/shifted_V_ADC_res_mV_1LSB)
    if (FEB0_OVVD_thr_int <= 511):
        FEB0_OVVD_thr_Unsigned8 = FEB0_OVVD_thr_int
    else:
        FEB0_OVVD_thr_Unsigned8 = 511
    gemroc_inst_param.D_OVV_LIMIT_FEB3 = FEB3_OVVD_thr_Unsigned8
    gemroc_inst_param.D_OVV_LIMIT_FEB2 = FEB2_OVVD_thr_Unsigned8
    gemroc_inst_param.D_OVV_LIMIT_FEB1 = FEB1_OVVD_thr_Unsigned8
    gemroc_inst_param.D_OVV_LIMIT_FEB0 = FEB0_OVVD_thr_Unsigned8
    COMMAND_STRING = 'CMD_GEMROC_LV_CFG_WR'
    command_echo = send_GEMROC_LV_CMD (GEMROC_ID_param, COMMAND_STRING)
    return command_echo

def Set_OVCA_LIMIT(gemroc_inst_param, GEMROC_ID_param, FEB3_OVCA_thr_param, FEB2_OVCA_thr_param, FEB1_OVCA_thr_param, FEB0_OVCA_thr_param, log_file_param):
    IADC_conv_fact_INA_GAIN_50 = 8.13
    IADC_conv_fact_INA_GAIN_200 = 8.13 / 4 ## mA per LSB
    if ( GEMROC_ID < 3):
        IADC_conv_fact_INA_GAIN = IADC_conv_fact_INA_GAIN_50
    else:
        IADC_conv_fact_INA_GAIN = IADC_conv_fact_INA_GAIN_200        
    FEB3_OVCA_thr_int = int(FEB3_OVCA_thr_param / IADC_conv_fact_INA_GAIN)
    if (FEB3_OVCA_thr_int <= 511):
        FEB3_OVCA_thr_U9 = FEB3_OVCA_thr_int
    else:
        FEB3_OVCA_thr_U9 = 511
    FEB2_OVCA_thr_int = int(FEB2_OVCA_thr_param / IADC_conv_fact_INA_GAIN)
    if (FEB2_OVCA_thr_int <= 511):
        FEB2_OVCA_thr_U9 = FEB2_OVCA_thr_int
    else:
        FEB2_OVCA_thr_U9 = 511
    FEB1_OVCA_thr_int = int(FEB1_OVCA_thr_param / IADC_conv_fact_INA_GAIN)
    if (FEB1_OVCA_thr_int <= 511):
        FEB1_OVCA_thr_U9 = FEB1_OVCA_thr_int
    else:
        FEB1_OVCA_thr_U9 = 511
    FEB0_OVCA_thr_int = int(FEB0_OVCA_thr_param / IADC_conv_fact_INA_GAIN)
    if (FEB0_OVCA_thr_int <= 511):
        FEB0_OVCA_thr_U9 = FEB0_OVCA_thr_int
    else:
        FEB0_OVCA_thr_U9 = 511
    gemroc_inst_param.A_OVC_LIMIT_FEB3 = FEB3_OVCA_thr_U9
    gemroc_inst_param.A_OVC_LIMIT_FEB2 = FEB2_OVCA_thr_U9
    gemroc_inst_param.A_OVC_LIMIT_FEB1 = FEB1_OVCA_thr_U9
    gemroc_inst_param.A_OVC_LIMIT_FEB0 = FEB0_OVCA_thr_U9
    COMMAND_STRING = 'CMD_GEMROC_LV_CFG_WR'
    command_echo = send_GEMROC_LV_CMD (GEMROC_ID_param, COMMAND_STRING)
    return command_echo

def Set_OVCD_LIMIT(gemroc_inst_param, GEMROC_ID_param, FEB3_OVCD_thr_param, FEB2_OVCD_thr_param, FEB1_OVCD_thr_param, FEB0_OVCD_thr_param, log_file_param):
    IADC_conv_fact_INA_GAIN_50 = 8.13
    IADC_conv_fact_INA_GAIN_200 = 8.13 / 4 ## mA per LSB
    if ( GEMROC_ID < 3):
        IADC_conv_fact_INA_GAIN = IADC_conv_fact_INA_GAIN_50
    else:
        IADC_conv_fact_INA_GAIN = IADC_conv_fact_INA_GAIN_200        
    FEB3_OVCD_thr_int = int(FEB3_OVCD_thr_param / IADC_conv_fact_INA_GAIN)
    if (FEB3_OVCD_thr_int <= 511):
        FEB3_OVCD_thr_U9 = FEB3_OVCD_thr_int
    else:
        FEB3_OVCD_thr_U9 = 511
    FEB2_OVCD_thr_int = int(FEB2_OVCD_thr_param / IADC_conv_fact_INA_GAIN)
    if (FEB2_OVCD_thr_int <= 511):
        FEB2_OVCD_thr_U9 = FEB2_OVCD_thr_int
    else:
        FEB2_OVCD_thr_U9 = 511
    FEB1_OVCD_thr_int = int(FEB1_OVCD_thr_param / IADC_conv_fact_INA_GAIN)
    if (FEB1_OVCD_thr_int <= 511):
        FEB1_OVCD_thr_U9 = FEB1_OVCD_thr_int
    else:
        FEB1_OVCD_thr_U9 = 511
    FEB0_OVCD_thr_int = int(FEB0_OVCD_thr_param / IADC_conv_fact_INA_GAIN)
    if (FEB0_OVCD_thr_int <= 511):
        FEB0_OVCD_thr_U9 = FEB0_OVCD_thr_int
    else:
        FEB0_OVCD_thr_U9 = 511
    gemroc_inst_param.D_OVC_LIMIT_FEB3 = FEB3_OVCD_thr_U9
    gemroc_inst_param.D_OVC_LIMIT_FEB2 = FEB2_OVCD_thr_U9
    gemroc_inst_param.D_OVC_LIMIT_FEB1 = FEB1_OVCD_thr_U9
    gemroc_inst_param.D_OVC_LIMIT_FEB0 = FEB0_OVCD_thr_U9
    COMMAND_STRING = 'CMD_GEMROC_LV_CFG_WR'
    command_echo = send_GEMROC_LV_CMD (GEMROC_ID_param, COMMAND_STRING)
    return command_echo

def Set_OVTF_LIMIT(gemroc_inst_param, GEMROC_ID_param, FEB3_OVTF_thr_param, FEB2_OVTF_thr_param, FEB1_OVTF_thr_param, FEB0_OVTF_thr_param, log_file_param):
    T_ref_PT1000 = 25.0
    V_ADC_at_25C = 247.2
    ADC_res_mV_1LSB = 0.305
    # acr 2018-03-15 BEGIN increased the sensitivity of temperature measurement and adjusted some parameters
    #T_ADC_data_shift = 3
    T_ADC_data_shift = 2 # acr 2018-03-15 increased the sensitivity 
    # acr 2018-03-15 BEGIN increased the sensitivity T_ADC_data_shift = 3
    ##calibration_offset_mV_FEB3 = 4.0
    ##calibration_offset_mV_FEB2 = 4.0
    ##calibration_offset_mV_FEB1 = 4.0
    ##calibration_offset_mV_FEB0 = 4.0
    calibration_offset_mV_FEB3 = 1.0
    calibration_offset_mV_FEB2 = 1.0
    calibration_offset_mV_FEB1 = 1.0
    calibration_offset_mV_FEB0 = 1.0
    # acr 2018-03-15 END increased the sensitivity of temperature measurement and adjusted some parameters
    shifted_T_ADC_res_mV_1LSB = ADC_res_mV_1LSB * (2 ** T_ADC_data_shift)
    deltaT_over_deltaV_ratio = 1.283
    FEB3_OVTF_thr_int = int( (((FEB3_OVTF_thr_param - T_ref_PT1000)/deltaT_over_deltaV_ratio)+V_ADC_at_25C-calibration_offset_mV_FEB3)/shifted_T_ADC_res_mV_1LSB )
    if (FEB3_OVTF_thr_int <= 255):
        FEB3_OVTF_thr_Unsigned8 = FEB3_OVTF_thr_int
    else:
        FEB3_OVTF_thr_Unsigned8 = 255
    FEB2_OVTF_thr_int = int( (((FEB2_OVTF_thr_param - T_ref_PT1000)/deltaT_over_deltaV_ratio)+V_ADC_at_25C-calibration_offset_mV_FEB2)/shifted_T_ADC_res_mV_1LSB )
    if (FEB2_OVTF_thr_int <= 255):
        FEB2_OVTF_thr_Unsigned8 = FEB2_OVTF_thr_int
    else:
        FEB2_OVTF_thr_Unsigned8 = 255
    FEB1_OVTF_thr_int = int( (((FEB1_OVTF_thr_param - T_ref_PT1000)/deltaT_over_deltaV_ratio)+V_ADC_at_25C-calibration_offset_mV_FEB1)/shifted_T_ADC_res_mV_1LSB )
    if (FEB1_OVTF_thr_int <= 255):
        FEB1_OVTF_thr_Unsigned8 = FEB1_OVTF_thr_int
    else:
        FEB1_OVTF_thr_Unsigned8 = 255
    FEB0_OVTF_thr_int = int( (((FEB0_OVTF_thr_param - T_ref_PT1000)/deltaT_over_deltaV_ratio)+V_ADC_at_25C-calibration_offset_mV_FEB0)/shifted_T_ADC_res_mV_1LSB )
    if (FEB0_OVTF_thr_int <= 255):
        FEB0_OVTF_thr_Unsigned8 = FEB0_OVTF_thr_int
    else:
        FEB0_OVTF_thr_Unsigned8 = 255        
    gemroc_inst_param.OVT_LIMIT_FEB3 = FEB3_OVTF_thr_Unsigned8
    gemroc_inst_param.OVT_LIMIT_FEB2 = FEB2_OVTF_thr_Unsigned8
    gemroc_inst_param.OVT_LIMIT_FEB1 = FEB1_OVTF_thr_Unsigned8
    gemroc_inst_param.OVT_LIMIT_FEB0 = FEB0_OVTF_thr_Unsigned8
    COMMAND_STRING = 'CMD_GEMROC_LV_CFG_WR'
    command_echo = send_GEMROC_LV_CMD (GEMROC_ID_param, COMMAND_STRING)
    return command_echo

def set_ROC_OVT_LIMIT(gemroc_inst_param, GEMROC_ID_param, ROC_OVT_thr_param, log_file_param):
    ## ACR 2018-03-15 please note that for this measurement 1 LSB = 1 Centigrade (approximately)
    if (ROC_OVT_thr_param <= 63):
        ROC_OVT_thr_Unsigned6 = ROC_OVT_thr_param
    else:
        ROC_OVT_thr_Unsigned6 = 63
    gemroc_inst_param.ROC_OVT_LIMIT = ROC_OVT_thr_Unsigned6
    COMMAND_STRING = 'CMD_GEMROC_LV_CFG_WR'
    command_echo = send_GEMROC_LV_CMD (GEMROC_ID_param, COMMAND_STRING)
    return command_echo

def display_log_GEMROC_LV_CfgReg_readback( command_echo_param, display_enable_param, log_enable_param, log_fname_param): # acr 2018-03-16 at IHEP
    L_array = array.array('I') # L is an array of unsigned long
    L_array.fromstring(command_echo_param)
    L_array.byteswap()
    if ( display_enable_param == 1):
        print ('\nList of LV related GEMROC Config Register parameters read back RESPONDING GEMROC%d:'%( ((L_array[0] >> 16) & 0X1f) ) ) 
        print ('\n  OVT_LIMIT_FEB3 : %d'  %( (L_array[1]>>22)&0xFF  ) )
        print ('\nD_OVV_LIMIT_FEB3 : %d'  %( (L_array[1]>>13)&0x1FF ) )
        print ('\nD_OVC_LIMIT_FEB3 : %d'  %( (L_array[1]>> 4)&0x1FF ) )
        print ('\n  OVT_LIMIT_FEB2 : %d'  %( (L_array[2]>>22)&0xFF  ) )
        print ('\nD_OVV_LIMIT_FEB2 : %d'  %( (L_array[2]>>13)&0x1FF ) )
        print ('\nD_OVC_LIMIT_FEB2 : %d'  %( (L_array[2]>> 4)&0x1FF ) )
        print ('\n  OVT_LIMIT_FEB1 : %d'  %( (L_array[3]>>22)&0xFF  ) )
        print ('\nD_OVV_LIMIT_FEB1 : %d'  %( (L_array[3]>>13)&0x1FF ) )
        print ('\nD_OVC_LIMIT_FEB1 : %d'  %( (L_array[3]>> 4)&0x1FF ) )
        print ('\n  OVT_LIMIT_FEB0 : %d'  %( (L_array[4]>>22)&0xFF  ) )
        print ('\nD_OVV_LIMIT_FEB0 : %d'  %( (L_array[4]>>13)&0x1FF ) )
        print ('\nD_OVC_LIMIT_FEB0 : %d'  %( (L_array[4]>> 4)&0x1FF ) )
        print ('\nA_OVV_LIMIT_FEB3 : %d'  %( (L_array[5]>>13)&0x1FF ) )
        print ('\nA_OVC_LIMIT_FEB3 : %d'  %( (L_array[5]>> 4)&0x1FF ) ) 
        print ('\nA_OVV_LIMIT_FEB2 : %d'  %( (L_array[6]>>13)&0x1FF ) )
        print ('\nA_OVC_LIMIT_FEB2 : %d'  %( (L_array[6]>> 4)&0x1FF ) )
        print ('\nA_OVV_LIMIT_FEB1 : %d'  %( (L_array[7]>>13)&0x1FF ) )
        print ('\nA_OVC_LIMIT_FEB1 : %d'  %( (L_array[7]>> 4)&0x1FF ) )
        print ('\nA_OVV_LIMIT_FEB0 : %d'  %( (L_array[8]>>13)&0x1FF ) )
        print ('\nA_OVC_LIMIT_FEB0 : %d'  %( (L_array[8]>> 4)&0x1FF ) )
        print ('\nROC_OVT_LIMIT : %d'     %( (L_array[9]>>24)&0x3F ) )
        print ('\nXCVR_LPBCK_TST_EN = %d' %( (L_array[9]>>18)&0x1  ) )
        print ('\nROC_OVT_EN = %d'        %( (L_array[9]>>16)&0x1  ) )
        print ('\nFEB_OVT_EN_pattern : %d'%( (L_array[9]>>12)&0xF  ) )
        print ('\nFEB_OVV_EN_pattern : %d'%( (L_array[9]>>8 )&0xF  ) )
        print ('\nFEB_OVC_EN_pattern : %d'%( (L_array[9]>>4 )&0xF  ) )
        print ('\nFEB_PWR_EN_pattern : %d'%( (L_array[9]>>0 )&0xF  ) )
        print ('\nTIMING_DLY_FEB3 : %d'   %( (L_array[10]>>24 )&0x3F ) )
        print ('\nTIMING_DLY_FEB2 : %d'   %( (L_array[10]>>16 )&0x3F ) )
        print ('\nTIMING_DLY_FEB1 : %d'   %( (L_array[10]>>8  )&0x3F ) )
        print ('\nTIMING_DLY_FEB0 : %d'   %( (L_array[10]>>0  )&0x3F ) )
    if (log_enable_param == 1):
        log_fname_param.write('\nList of LV related GEMROC Config Register parameters read back RESPONDING GEMROC%d:'%( ((L_array[0] >> 16) & 0X1f) ) ) 
        log_fname_param.write('\n  OVT_LIMIT_FEB3 : %d'  %( (L_array[1]>>22)&0xFF  ) )
        log_fname_param.write('\nD_OVV_LIMIT_FEB3 : %d'  %( (L_array[1]>>13)&0x1FF ) )
        log_fname_param.write('\nD_OVC_LIMIT_FEB3 : %d'  %( (L_array[1]>> 4)&0x1FF ) )
        log_fname_param.write('\n  OVT_LIMIT_FEB2 : %d'  %( (L_array[2]>>22)&0xFF  ) )
        log_fname_param.write('\nD_OVV_LIMIT_FEB2 : %d'  %( (L_array[2]>>13)&0x1FF ) )
        log_fname_param.write('\nD_OVC_LIMIT_FEB2 : %d'  %( (L_array[2]>> 4)&0x1FF ) )
        log_fname_param.write('\n  OVT_LIMIT_FEB1 : %d'  %( (L_array[3]>>22)&0xFF  ) )
        log_fname_param.write('\nD_OVV_LIMIT_FEB1 : %d'  %( (L_array[3]>>13)&0x1FF ) )
        log_fname_param.write('\nD_OVC_LIMIT_FEB1 : %d'  %( (L_array[3]>> 4)&0x1FF ) )
        log_fname_param.write('\n  OVT_LIMIT_FEB0 : %d'  %( (L_array[4]>>22)&0xFF  ) )
        log_fname_param.write('\nD_OVV_LIMIT_FEB0 : %d'  %( (L_array[4]>>13)&0x1FF ) )
        log_fname_param.write('\nD_OVC_LIMIT_FEB0 : %d'  %( (L_array[4]>> 4)&0x1FF ) )
        log_fname_param.write('\nA_OVV_LIMIT_FEB3 : %d'  %( (L_array[5]>>13)&0x1FF ) )
        log_fname_param.write('\nA_OVC_LIMIT_FEB3 : %d'  %( (L_array[5]>> 4)&0x1FF ) )   
        log_fname_param.write('\nA_OVV_LIMIT_FEB2 : %d'  %( (L_array[6]>>13)&0x1FF ) )
        log_fname_param.write('\nA_OVC_LIMIT_FEB2 : %d'  %( (L_array[6]>> 4)&0x1FF ) )
        log_fname_param.write('\nA_OVV_LIMIT_FEB1 : %d'  %( (L_array[7]>>13)&0x1FF ) )
        log_fname_param.write('\nA_OVC_LIMIT_FEB1 : %d'  %( (L_array[7]>> 4)&0x1FF ) )
        log_fname_param.write('\nA_OVV_LIMIT_FEB0 : %d'  %( (L_array[8]>>13)&0x1FF ) )
        log_fname_param.write('\nA_OVC_LIMIT_FEB0 : %d'  %( (L_array[8]>> 4)&0x1FF ) )
        log_fname_param.write('\nROC_OVT_LIMIT : %d'     %( (L_array[9]>>24)&0x3F ) )
        log_fname_param.write('\nXCVR_LPBCK_TST_EN = %d' %( (L_array[9]>>18)&0x1  ) )
        log_fname_param.write('\nROC_OVT_EN = %d'        %( (L_array[9]>>16)&0x1  ) )
        log_fname_param.write('\nFEB_OVT_EN_pattern : %d'%( (L_array[9]>>12)&0xF  ) )
        log_fname_param.write('\nFEB_OVV_EN_pattern : %d'%( (L_array[9]>>8 )&0xF  ) )
        log_fname_param.write('\nFEB_OVC_EN_pattern : %d'%( (L_array[9]>>4 )&0xF  ) )
        log_fname_param.write('\nFEB_PWR_EN_pattern : %d'%( (L_array[9]>>0 )&0xF  ) )
        log_fname_param.write('\nTIMING_DLY_FEB3 : %d'   %( (L_array[10]>>24 )&0x3F ) )
        log_fname_param.write('\nTIMING_DLY_FEB2 : %d'   %( (L_array[10]>>16 )&0x3F ) )
        log_fname_param.write('\nTIMING_DLY_FEB1 : %d'   %( (L_array[10]>>8  )&0x3F ) )
        log_fname_param.write('\nTIMING_DLY_FEB0 : %d'   %( (L_array[10]>>0  )&0x3F ) )

def Read_GEMROC_LV_CfgReg(gemroc_inst_param, GEMROC_ID_param, log_file_param):
    COMMAND_STRING = 'CMD_GEMROC_LV_CFG_RD'
    command_echo = send_GEMROC_LV_CMD (GEMROC_ID_param, COMMAND_STRING)
    return command_echo

def display_log_GEMROC_DAQ_CfgReg_readback( command_echo_param, display_enable_param, log_enable_param, log_fname_param): # acr 2018-03-16 at IHEP
    L_array = array.array('I') # L is an array of unsigned long
    L_array.fromstring(command_echo_param)
    L_array.byteswap()
    if ( display_enable_param == 1):
        print ('\nList of DAQ related GEMROC Config Register parameters read back RESPONDING GEMROC%d:'%( ((L_array[0] >> 16) & 0X1f) ) ) 
        print ('\nUDP_DATA_DESTINATION_IPADDR: %d'  %( (L_array[0]>>8)&0xFF  ) )
        print ('\nUDP_DATA_DESTINATION_IPPORT: %d'  %( (L_array[4]>>26)&0xF ) )
        print ('\nL1_latency: %d'                   %( (L_array[1]>>20)&0x3FF ) )
        print ('\nTP_width: %d'                     %( (L_array[1]>>16)&0xF   ) )
        print ('\nUpperDataScanWindowOffset: %d'    %( (L_array[1]>>0 )&0xFFFF ) )
        print ('\nL1_period: %d'                    %( (L_array[2]>>20)&0x3FF ) )
        print ('\nPeriodic_L1_EN_pattern: %X'       %( (L_array[2]>>16)&0xF ) )
        print ('\nLowerDataScanWindowOffset: %d'    %( (L_array[2]>>0 )&0xFFFF ) )
        print ('\nTP_period: %d'                    %( (L_array[3]>>20)&0x3FF ) )
        print ('\nPeriodic_TP_EN_pattern: %X'       %( (L_array[3]>>16)&0xF ) )
        print ('\nTL_nTM_ACQ: %d'                   %( (L_array[3]>>11)&0x1 ) )
        print ('\nAUTO_L1_EN: %d'                   %( (L_array[3]>>10)&0x1 ) )
        print ('\nAUTO_TP_EN: %d'                   %( (L_array[3]>>9 )&0x1 ) )
        print ('\nTP_Pos_nNeg: %d'                  %( (L_array[3]>>8 )&0x1 ) )
        print ('\nEN_TM_TCAM_pattern: %X'           %( (L_array[3]>>0 )&0xFF ) )
        print ('\nnumber_of_repetitions: %d'        %( (L_array[4]>>16)&0x3FF ) )
        print ('\ntarget_TCAM_ID: %d'               %( (L_array[4]>>8)&0x3) )
        print ('\nto_ALL_TCAM_enable: %d'           %( (L_array[4]>>6)&0x1) )
    if (log_enable_param == 1):
        log_fname_param.write('\nList of DAQ related GEMROC Config Register parameters read back RESPONDING GEMROC%d:'%( ((L_array[0] >> 16) & 0X1f) ) ) 
        log_fname_param.write('\nUDP_DATA_DESTINATION_IPADDR: %d'  %( (L_array[0]>>8)&0xFF  ) )
        log_fname_param.write('\nUDP_DATA_DESTINATION_IPPORT: %d'  %( (L_array[4]>>26)&0xF ) )
        log_fname_param.write('\nL1_latency: %d'                   %( (L_array[1]>>20)&0x3FF ) )
        log_fname_param.write('\nTP_width: %d'                     %( (L_array[1]>>16)&0xF   ) )
        log_fname_param.write('\nUpperDataScanWindowOffset: %d'    %( (L_array[1]>>0 )&0xFFFF ) )
        log_fname_param.write('\nL1_period: %d'                    %( (L_array[2]>>20)&0x3FF ) )
        log_fname_param.write('\nPeriodic_L1_EN_pattern: %X'       %( (L_array[2]>>16)&0xF ) )
        log_fname_param.write('\nLowerDataScanWindowOffset: %d'    %( (L_array[2]>>0 )&0xFFFF ) )
        log_fname_param.write('\nTP_period: %d'                    %( (L_array[3]>>20)&0x3FF ) )
        log_fname_param.write('\nPeriodic_TP_EN_pattern: %X'       %( (L_array[3]>>16)&0xF ) )
        log_fname_param.write('\nTL_nTM_ACQ: %d'                   %( (L_array[3]>>11)&0x1 ) )
        log_fname_param.write('\nAUTO_L1_EN: %d'                   %( (L_array[3]>>10)&0x1 ) )
        log_fname_param.write('\nAUTO_TP_EN: %d'                   %( (L_array[3]>>9 )&0x1 ) )
        log_fname_param.write('\nTP_Pos_nNeg: %d'                  %( (L_array[3]>>8 )&0x1 ) )
        log_fname_param.write('\nEN_TM_TCAM_pattern: %X'           %( (L_array[3]>>0 )&0xFF ) )
        log_fname_param.write('\nnumber_of_repetitions: %d'        %( (L_array[4]>>16)&0x3FF ) )
        log_fname_param.write('\ntarget_TCAM_ID: %d'               %( (L_array[4]>>8)&0x3) )
        log_fname_param.write('\nto_ALL_TCAM_enable: %d'           %( (L_array[4]>>6)&0x1) )

def Read_GEMROC_DAQ_CfgReg(gemroc_inst_param, GEMROC_ID_param, log_file_param):
    COMMAND_STRING = 'CMD_GEMROC_DAQ_CFG_RD'
    command_echo = send_GEMROC_DAQ_CMD (GEMROC_ID_param, gemroc_inst_param, COMMAND_STRING)
    return command_echo

data_file = 'my_log.txt'
my_log = open(data_file, 'w')

log_fname = 'GEMROC%d_interactive_cfg_log.txt' %GEMROC_ID
log_file = open(log_fname, 'w')
IVT_log_fname = 'GEMROC%d_IVT_log.txt' %GEMROC_ID
IVT_log_file = open(IVT_log_fname, 'w')

#HOST_DYNAMIC_IP_ADDRESS = "192.168.1.%d" %(GEMROC_ID)
## NOTE: STEFANO CHIOZZI - 2018-03-08 A SECOND IP ADDRESS (E.G. 192.168.1.201) CAN BE ASSIGNED TO THE NIC
##                                    AND A SECOND SET OF INSTANCES OF THIS CODE (WITH DIFFERENT PORTS) CAN ACCESS THE SECOND SET OF GEMROC BOARDS (12 to 22) THROUGH IT
##HOST_IP = "192.168.1.200" # FOR  GEMROC 1 - 11
#HOST_IP = "192.168.1.200" # FOR  GEMROC 1 - 11
HOST_IP = "127.0.0.1" # uncomment for test only

HOST_PORT = 54816+1+GEMROC_ID
HOST_PORT_RECEIVE = 58912+1+GEMROC_ID

DEST_IP_ADDRESS = "127.0.0.1" 
#DEST_IP_ADDRESS = "192.168.1.%d" %(GEMROC_ID+16) # offset 16 is determined by Stefano's MAC
DEST_PORT_NO = 58912+1 # STEFANO CHIOZZI - 2018-03-08 offset 0 is reserved by the MAC for a custom protocol; offset 3 is also a special debug port

# BEGIN uncomment for test only
#DEST_IP_ADDRESS = "127.0.0.1"
#DEST_PORT_NO = 58912+15
# END   uncomment for test only

tiger_index = 0
BUFSIZE = 4096 

clientSock = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)
clientSock.bind((HOST_IP, HOST_PORT))
receiveSock = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)
receiveSock.setsockopt(socket.SOL_SOCKET, socket.SO_REUSEADDR, 1)
##receiveSock.setsockopt(socket.SOL_SOCKET, socket.SO_REUSEPORT, 1)
receiveSock.bind((HOST_IP, HOST_PORT_RECEIVE))
remote_IP_Address = '192.168.1.%d'%(GEMROC_ID+16)
##receiveSock.connect( (remote_IP_Address, 54817,) )
##print '\nnew socket handling'

## creating an instance of the GEMROC LV configuration settings object
## parameter list:
##  TARGET_GEMROC_ID_param = 0, # acr 2017-09-22              
##  command_string_param = 'NONE',
##  number_of_repetitions_param = 1,
cfg_filename = 'GEMROC_%d_def_cfg_LV_2018.txt' %GEMROC_ID
gemroc_LV_XX = GEMconf_classes_2018v3.gemroc_cmd_LV_settings(GEMROC_ID,'NONE',1,cfg_filename)
gemroc_LV_XX.set_FEB_PWR_EN_pattern(FEB_PWR_EN_pattern)
COMMAND_STRING = 'CMD_GEMROC_LV_CFG_WR'
gemroc_LV_XX.set_gemroc_cmd_code(COMMAND_STRING,1)
gemroc_LV_XX.update_command_words()
#keep gemroc_LV_XX.print_command_words()
#keep gemroc_LV_XX.extract_parameters_from_UDP_packet()
array_to_send = gemroc_LV_XX.command_words
command_echo = send_GEMROC_CFG_CMD_PKT( GEMROC_ID, COMMAND_STRING, array_to_send, DEST_IP_ADDRESS, DEST_PORT_NO )
print 'CMD_GEMROC_LV_CFG_WR'

## creating an instance of the GEMROC DAQ configuration settings object
## parameter list:
##  TARGET_GEMROC_ID_param = 0, # acr 2017-09-22              
##  command_string_param = 'NONE',
##  TCAM_ID_param = 1,              
##  number_of_repetitions_param = 1,
##  to_ALL_TCAM_enable_param = 0,
cfg_filename = 'GEMROC_%d_def_cfg_DAQ_2018v3.txt' %GEMROC_ID
gemroc_DAQ_XX = GEMconf_classes_2018v3.gemroc_cmd_DAQ_settings(GEMROC_ID,'NONE',0,1,0,cfg_filename)
gemroc_DAQ_XX.extract_parameters_from_UDP_packet()
COMMAND_STRING = 'CMD_GEMROC_DAQ_CFG_WR'
command_echo = send_GEMROC_DAQ_CMD(GEMROC_ID, gemroc_DAQ_XX, COMMAND_STRING)
## creating an instance of the TIGER global configuration settings object
##    parameter list:
##    TARGET_GEMROC_ID_param = 0, 
##    cfg_filename_param = "default_g_cfg_2018_all_big_endian.txt"
default_g_inst_settigs_filename = "TIGER_def_g_cfg_2018.txt"
g_inst = GEMconf_classes_2018v3.g_reg_settings(GEMROC_ID,default_g_inst_settigs_filename)

#### creating an instance of the channel configuration settings object
##    parameter list:
##    TARGET_GEMROC_ID_param = 0, 
##    cfg_filename_param = "default_ch_cfg_mainz.txt" 
default_ch_inst_settigs_filename = "TIGER_def_ch_cfg_2018.txt"
c_inst = GEMconf_classes_2018v3.ch_reg_settings(GEMROC_ID,default_ch_inst_settigs_filename)

## acr 2018-02-21 BEGIN write the user interface
menu_string = '\
\nGEMROC_TIGER_CFG V2.0; A.C.R. 14 Mar 2018 INFN-FE; last mod: 2018-03-14 \
\nMENU: \
\n(P)EW     <PWR_EN_PATTERN>(Hex, 0x0 thru 0xF) \
\n(TD)ly    <FEB3_timing_dly> <FEB2_timing_dly> <FEB1_timing_dly> <FEB0_timing_dly> range of parameters is 0 to 63 corresponding to 88ps of incremental delay per LSB \
\n(I)VT                                                                             read and print IVT parameters of GEM\
\n OCVTEn   <FEB_OVC_EN_pattern>(Hex, 0x0 thru 0xF) <FEB_OVV_EN_pattern>(Hex, 0x0 thru 0xF) <FEB_OVT_EN_pattern>(Hex, 0x0 thru 0xF) <ROC_OVT_EN>(0 or 1)\
\n OVVA     <FEB3_OVVA_thr_param> <FEB2_OVVA_thr_param> <FEB1_OVVA_thr_param> <FEB0_OVVA_thr_param>  range of parameters is 0 to 5000(dec) [mV]\
\n OVVD     <FEB3_OVVD_thr_param> <FEB2_OVVD_thr_param> <FEB1_OVVD_thr_param> <FEB0_OVVD_thr_param>  range of parameters is 0 to 5000(dec) [mV]\
\n OVCA     <FEB3_OVCA_thr_param> <FEB2_OVCA_thr_param> <FEB1_OVCA_thr_param> <FEB0_OVCA_thr_param>  range of parameters is 0 to 1200(dec) [mA]\
\n OVCD     <FEB3_OVCD_thr_param> <FEB2_OVCD_thr_param> <FEB1_OVCD_thr_param> <FEB0_OVCD_thr_param>  range of parameters is 0 to 1200(dec) [mA]\
\n OVTF     <FEB3_OVT_thr_param>  <FEB2_OVT_thr_param>  <FEB1_OVT_thr_param>  <FEB0_OVT_thr_param>   range of parameters is 0 to 100(dec)[degree Celsius]\
\n OVTR     <FEB3_OVT_thr_param>                                                                     range of parameters is 0 to 63(dec)[degree Celsius]\
\n LVCR                                                                             read and print setting of GEMROC LV configuration register  \
\n DAQCR                                                                            read and print setting of GEMROC DAQ configuration register  \
\n GRST                                                                             RESETS ALL TIGER CONFIGURATION REGISTERS: must be executed before TIGER Global Register Write \
\n(GW)def   <TIGER_ID>(0 thru 7)                                                    (default file: "TIGER_def_g_cfg_2018.txt") \
\n(AV)CaspGset <TIGER_ID>(0 thru 7) <AVcasp_global param> (0 thru 31);              AVcasp_global_set: 0 min power consumption; 31 max power consumption\
\n(CW)def   <TIGER_ID>(0 thru 7) <channel_ID>(0 thru 64);                           if channel_ID =64 then WRITE ALL IN SEQUENCE (default file: "TIGER_def_ch_cfg_2018.txt"); \
\n(GR)d     <TIGER_ID>(0 thru 7)  \
\n(CR)d     <TIGER_ID>(0 thru 7) <channel_ID>(0 thru 64);                           if channel_ID = 64 then READ ALL channels IN SEQUENCE \
\n TPEnG    <TIGER_ID>(0 thru 7) <On_Off_param> (0 or 1);                           set FE_TPEnable bit in the Global Configuration Register\
\n(TP)EW_ch <TIGER_ID>(0 thru 7) <channel_ID>(0 thru 64) <TP_disable_FE_param> (0 or 1) <TriggerMode_param> (0 or 1);   if channel_ID = 64 then act on all channels\
\n(VT1)_ch  <TIGER_ID>(0 thru 7) <channel_ID>(0 thru 64) <VthT1 param> (0 thru 63); if channel_ID = 64 then act on VthT1 for ALL channels\
\n SRst     <SynchRst_TargetFEB>(0 to 4)                                            send a sycnh rst to the target FEB(0 to 3); if 4 is input then send synch rst to ALL FEBs  \
\n DRst     <SynchRst_TargetTCAM>(0 to 4)                                           send a sycnh rst to the TIGER Data Processing unit (TCAM)0 to 3; if 4 is input then send synch rst to ALL TCAMs    \
\n DSTART   <TCAM_En_pattern>(0x0 thru 0xF) <FEB_TP_En_pattern>(0x0 thru 0xF)       set GEMROC to start sending triggerless data from enabled TCAM units; set FEB periodic TP Enable pattern \
\n(Q)uit: leave \
\nEnter your command: '
DONE = False
def add_input(input_queue):
    while True:
        #input_queue.put(sys.stdin.read(1))
        input_queue.put(sys.stdin.readline())

def Menu_and_prompt():
    input_queue = Queue.Queue()
    input_thread = threading.Thread(target=add_input, args=(input_queue,))
    input_thread.daemon = True
    input_thread.start()

    last_update = time.time()
    DONE = False
    IVT_DISPLAY_ENABLE = 1
    fmt='%Y-%m-%d-%H-%M-%S'
    IVT_log_file.write('\n%s' %datetime.datetime.now().strftime(fmt) )
    GEMROC_IVT_read_and_log(GEMROC_ID, IVT_DISPLAY_ENABLE, IVT_LOG_ENABLE, IVT_log_file)
    ##os.system('cls')
    sys.stdout.write(menu_string)
    #print "Enter your command: "

    while not DONE:
        if ((time.time()-last_update)>IVT_LOG_PERIOD_SECONDS) :
            #os.system('cls')
            IVT_DISPLAY_ENABLE = 0
            IVT_log_file.write('\n%s' %datetime.datetime.now().strftime(fmt) )
#            GEMROC_IVT_read_and_log(GEMROC_ID, IVT_DISPLAY_ENABLE, IVT_LOG_ENABLE, IVT_log_file)
            last_update = time.time()
            #sys.stdout.write(menu_string)
        else:
            if not input_queue.empty():
                input_array = (input_queue.get()).split()
                if (len(input_array) > 0):
                    if ( (input_array[0] == 'quit') or (input_array[0] == 'q') or (input_array[0] == 'Quit') or (input_array[0] == 'Q') ):
                        DONE = True
                    elif ( (input_array[0] == 'PEW') or (input_array[0] == 'P') ):
                        if (len(input_array) == 2):
                            FEBPwrEnPattern_set(GEMROC_ID, int(input_array[1],0) )
                            os.system('cls')
                            sys.stdout.write(menu_string)
                    elif ( (input_array[0] == 'TDly') or (input_array[0] == 'TD') ):
                        if (len(input_array) == 5):
                            FEB3_TIGER_timing_delay_setting = int(input_array[1]) # decimal format is expected
                            FEB2_TIGER_timing_delay_setting = int(input_array[2])
                            FEB1_TIGER_timing_delay_setting = int(input_array[3])
                            FEB0_TIGER_timing_delay_setting = int(input_array[4])
                            set_FEB_timing_delays(GEMROC_ID, FEB3_TIGER_timing_delay_setting, FEB2_TIGER_timing_delay_setting, FEB2_TIGER_timing_delay_setting, FEB1_TIGER_timing_delay_setting )
                            os.system('cls')
                            sys.stdout.write(menu_string)
                    elif ( (input_array[0] == 'IVT') or (input_array[0] == 'I') ) :
                        if (len(input_array) == 1):
                            IVT_DISPLAY_ENABLE = 1
                            IVT_log_file.write('\n%s' %datetime.datetime.now().strftime(fmt) )
                            GEMROC_IVT_read_and_log(GEMROC_ID, IVT_DISPLAY_ENABLE, IVT_LOG_ENABLE, IVT_log_file)
                            print '\npress RETURN to continue'
                            while input_queue.empty():
                                time.sleep(1)
                            os.system('cls')
                            sys.stdout.write(menu_string)
                    elif (input_array[0] == 'OCVTEn') :
                        if (len(input_array) == 5):
                            Set_OV_OC_OT_PWR_CUT_EN_FLAGS(gemroc_LV_XX, GEMROC_ID, int(input_array[1],16), int(input_array[2],16), int(input_array[3],16), int(input_array[4]), log_file)
                            GEMROC_IVT_read_and_log(GEMROC_ID, 1, 0, IVT_log_file) ## ACR 2018-03-15 PERFORM IVT READ TO UPDATE STATUS OF FEB POWER ENABLE BITS which may change as a consequence
                            ##time.sleep(2)
                            print '\npress RETURN to continue'
                            while input_queue.empty():
                                time.sleep(1)
                            os.system('cls')
                            sys.stdout.write(menu_string)
                    elif (input_array[0] == 'OVVA') :
                        if (len(input_array) == 5):
                            Set_OVVA_LIMIT(gemroc_LV_XX, GEMROC_ID, int(input_array[1]), int(input_array[2]), int(input_array[3]), int(input_array[4]), log_file)
                            GEMROC_IVT_read_and_log(GEMROC_ID, 1, 0, IVT_log_file) ## ACR 2018-03-15 PERFORM IVT READ TO UPDATE STATUS OF FEB POWER ENABLE BITS which may change as a consequence
                            ##time.sleep(2)
                            print '\npress RETURN to continue'
                            while input_queue.empty():
                                time.sleep(1)
                            os.system('cls')
                            sys.stdout.write(menu_string)
                    elif (input_array[0] == 'OVVD') :
                        if (len(input_array) == 5):
                            Set_OVVD_LIMIT(gemroc_LV_XX, GEMROC_ID, int(input_array[1]), int(input_array[2]), int(input_array[3]), int(input_array[4]), log_file)
                            GEMROC_IVT_read_and_log(GEMROC_ID, 1, 0, IVT_log_file) ## ACR 2018-03-15 PERFORM IVT READ TO UPDATE STATUS OF FEB POWER ENABLE BITS which may change as a consequence
                            ##time.sleep(2)
                            print '\npress RETURN to continue'
                            while input_queue.empty():
                                time.sleep(1)
                            os.system('cls')
                            sys.stdout.write(menu_string)
                    elif (input_array[0] == 'OVCA') :
                        if (len(input_array) == 5):
                            Set_OVCA_LIMIT(gemroc_LV_XX, GEMROC_ID, int(input_array[1]), int(input_array[2]), int(input_array[3]), int(input_array[4]), log_file)
                            GEMROC_IVT_read_and_log(GEMROC_ID, 1, 0, IVT_log_file) ## ACR 2018-03-15 PERFORM IVT READ TO UPDATE STATUS OF FEB POWER ENABLE BITS which may change as a consequence
                            ##time.sleep(2)
                            print '\npress RETURN to continue'
                            while input_queue.empty():
                                time.sleep(1)
                            os.system('cls')
                            sys.stdout.write(menu_string)
                    elif (input_array[0] == 'OVCD') :
                        if (len(input_array) == 5):
                            Set_OVCD_LIMIT(gemroc_LV_XX, GEMROC_ID, int(input_array[1]), int(input_array[2]), int(input_array[3]), int(input_array[4]), log_file)
                            GEMROC_IVT_read_and_log(GEMROC_ID, 1, 0, IVT_log_file) ## ACR 2018-03-15 PERFORM IVT READ TO UPDATE STATUS OF FEB POWER ENABLE BITS which may change as a consequence
                            ##time.sleep(2)
                            print '\npress RETURN to continue'
                            while input_queue.empty():
                                time.sleep(1)
                            os.system('cls')
                            sys.stdout.write(menu_string)
                    elif (input_array[0] == 'OVTF') :
                        if (len(input_array) == 5):
                            Set_OVTF_LIMIT(gemroc_LV_XX, GEMROC_ID, int(input_array[1]), int(input_array[2]), int(input_array[3]), int(input_array[4]), log_file)
                            GEMROC_IVT_read_and_log(GEMROC_ID, 1, 0, IVT_log_file) ## ACR 2018-03-15 PERFORM IVT READ TO UPDATE STATUS OF FEB POWER ENABLE BITS which may change as a consequence
                            ##time.sleep(2)
                            print '\npress RETURN to continue'
                            while input_queue.empty():
                                time.sleep(1)
                            os.system('cls')
                            sys.stdout.write(menu_string)
                    elif (input_array[0] == 'OVTR') :
                        if (len(input_array) == 2):
                            set_ROC_OVT_LIMIT(gemroc_LV_XX, GEMROC_ID, int(input_array[1]), log_file)
                            GEMROC_IVT_read_and_log(GEMROC_ID, 1, 0, IVT_log_file) ## ACR 2018-03-15 PERFORM IVT READ TO UPDATE STATUS OF FEB POWER ENABLE BITS which may change as a consequence
                            ##time.sleep(2)
                            print '\npress RETURN to continue'
                            while input_queue.empty():
                                time.sleep(1)
                            os.system('cls')
                            sys.stdout.write(menu_string)
                    elif (input_array[0] == 'LVCR'): # acr 2018-03-16 at IHEP
                        if (len(input_array) == 1):
                            command_reply = Read_GEMROC_LV_CfgReg(gemroc_LV_XX, GEMROC_ID, log_file)
                            print '\nLVCR command_reply: %s' %binascii.b2a_hex(command_reply)
                            display_log_GEMROC_LV_CfgReg_readback(command_reply, 1, 1, log_file) 
                            print '\npress RETURN to continue'
                            while input_queue.empty():
                                time.sleep(1)
                            os.system('cls')
                            sys.stdout.write(menu_string)
                    elif (input_array[0] == 'DAQCR'):
                        if (len(input_array) == 1):
                            command_reply = Read_GEMROC_DAQ_CfgReg(gemroc_DAQ_XX, GEMROC_ID, log_file)
                            print '\nDAQCR command_reply: %s' %binascii.b2a_hex(command_reply)
                            display_log_GEMROC_DAQ_CfgReg_readback(command_reply, 1, 1, log_file)
                            print '\npress RETURN to continue'
                            while input_queue.empty():
                                time.sleep(1)
                            os.system('cls')
                            sys.stdout.write(menu_string)
                    elif (input_array[0] == 'GRST'):
                        if (len(input_array) == 1):
                            ResetTgtGEMROC_ALL_TIGER_GCfgReg(GEMROC_ID, gemroc_DAQ_XX)
                            os.system('cls')
                            sys.stdout.write(menu_string)                
                    elif ( (input_array[0] == 'GWdef') or (input_array[0] == 'GW') ):
                        if (len(input_array) == 2):
                            default_filename = "TIGER_def_g_cfg_2018.txt"
                            command_reply = WriteTgtGEMROC_TIGER_GCfgReg_fromfile(g_inst, GEMROC_ID, int(input_array[1]), default_filename, log_file)
                            print '\nGWdef command_reply: %s' %binascii.b2a_hex(command_reply)
                            command_reply = ReadTgtGEMROC_TIGER_GCfgReg( g_inst, GEMROC_ID, int(input_array[1]), log_file )
                            print '\nGRd   command_reply: %s' %binascii.b2a_hex(command_reply)
                            time.sleep(4)
                            os.system('cls')
                            sys.stdout.write(menu_string)                
                    elif ((input_array[0] == 'GRd') or (input_array[0] == 'GR')):
                        if (len(input_array) == 2):
                            command_reply = ReadTgtGEMROC_TIGER_GCfgReg( g_inst, GEMROC_ID, int(input_array[1]), log_file )
                            print '\nGRd command_reply: %s' %binascii.b2a_hex(command_reply)
                            display_log_GCfg_readback(command_reply, 1, log_file) ## acr 2018-03-02 
                            print '\npress RETURN to continue'
                            while input_queue.empty():
                                time.sleep(1)
                            os.system('cls')
                            sys.stdout.write(menu_string)                
                    elif ( (input_array[0] == 'CWdef') or (input_array[0] == 'CW') ):
                        if (len(input_array) == 3):
                            default_filename = "TIGER_def_ch_cfg_2018.txt"
                            #command_reply = WriteTgtGEMROC_TIGER_ChCfgReg_fromfile(c_inst, GEMROC_ID, int(input_array[1]), int(input_array[2]), default_filename)
                            command_reply = WriteTgtGEMROC_TIGER_ChCfgReg_fromfile(c_inst, GEMROC_ID, int(input_array[1]), int(input_array[2]), default_filename, log_file)
                            print '\nCWdef command_reply: %s' %binascii.b2a_hex(command_reply)
                            #command_reply = ReadTgtGEMROC_TIGER_ChCfgReg (c_inst, GEMROC_ID, int(input_array[1]), int(input_array[2]), 0 ) 
                            command_reply = ReadTgtGEMROC_TIGER_ChCfgReg (c_inst, GEMROC_ID, int(input_array[1]), int(input_array[2]), 0, log_file ) 
                            print '\nCRd   command_reply: %s' %binascii.b2a_hex(command_reply)
                            time.sleep(4)
                            os.system('cls')
                            sys.stdout.write(menu_string)                
                    elif ((input_array[0] == 'CRd') or (input_array[0] == 'CR')):
                        if (len(input_array) == 3):
                            #command_reply = ReadTgtGEMROC_TIGER_ChCfgReg (c_inst, GEMROC_ID, int(input_array[1]), int(input_array[2]), 1 ) 
                            command_reply = ReadTgtGEMROC_TIGER_ChCfgReg (c_inst, GEMROC_ID, int(input_array[1]), int(input_array[2]), 1, log_file ) 
                            print '\nCRd   command_reply: %s' %binascii.b2a_hex(command_reply)
                            ##display_log_ChCfg_readback(command_reply, 1, log_file)
                            print '\npress RETURN to continue'
                            while input_queue.empty():
                                time.sleep(1)
                            os.system('cls')
                            sys.stdout.write(menu_string)                
                    elif (input_array[0] == 'TPEnG'):
                        if (len(input_array) == 3):
                            FE_TPEnable_PARAM = int(input_array[2])
                            set_FE_TPEnable(g_inst, GEMROC_ID, int(input_array[1]), FE_TPEnable_PARAM, log_file)  
                            print '\nTo TIGER %d on GEMROC %d: GCreg FE_TPEnable bit set to %d' %(int(input_array[1]), GEMROC_ID, int(input_array[2]) )
                            time.sleep(2)
                            os.system('cls')
                            sys.stdout.write(menu_string)                
                    elif ( (input_array[0] == 'TPEW_ch') or (input_array[0] == 'TP') ):
                        if (len(input_array) == 5):
                            TP_disable_FE_param = int(input_array[3])
                            TriggerMode_param = int(input_array[4])
                            Set_GEMROC_TIGER_ch_TPEn (c_inst, GEMROC_ID, int(input_array[1]), int(input_array[2]), TP_disable_FE_param, TriggerMode_param, log_file )
                            print '\nTo TIGER %d on GEMROC %d: TP_disable_FE bit set to %d and TriggerMode bit set to %d for channel %d' %(int(input_array[1]), GEMROC_ID, int(input_array[3]), int(input_array[4]),int(input_array[2]) )
                            time.sleep(2)
                            os.system('cls')
                            sys.stdout.write(menu_string)                
                    elif ( (input_array[0] == 'VT1_ch') or (input_array[0] == 'VT1') ):
                        if (len(input_array) == 4):
                            Set_Vth_T1(c_inst, GEMROC_ID, int(input_array[1]), int(input_array[2]), int(input_array[3]), log_file)
                            print '\nTo TIGER %d on GEMROC %d: Vth_T1 set to %d for channel %d' %(int(input_array[1]), GEMROC_ID, int(input_array[3]), int(input_array[2]) )
                            time.sleep(2)
                            os.system('cls')
                            sys.stdout.write(menu_string)
                    elif ( (input_array[0] == 'AVCaspGset') or (input_array[0] == 'AV') ):
                        if (len(input_array) == 3):
                            set_AVcasp_global(g_inst, GEMROC_ID, int(input_array[1]), int(input_array[2]), log_file)
                            time.sleep(2)
                            os.system('cls')
                            sys.stdout.write(menu_string)
                    elif (input_array[0] == 'SRst'):
                        if (len(input_array) == 2):
                            if (int(input_array[1]) < 4):
                                To_ALL_FEB_enable = 0
                            else:
                                To_ALL_FEB_enable = 1
                            TargetFEB = int(input_array[1]) & 0x3
                            SynchReset_to_TgtFEB(gemroc_DAQ_XX, GEMROC_ID, TargetFEB, To_ALL_FEB_enable )
                            print '\nTo FEB %d or ToALL_FEB: %d on GEMROC %d: sent synchronous reset' %( TargetFEB, To_ALL_FEB_enable, GEMROC_ID )
                            time.sleep(2)
                            os.system('cls')
                            sys.stdout.write(menu_string)                
                    elif (input_array[0] == 'DRst'):
                        if (len(input_array) == 2):
                            if (int(input_array[1]) < 4):
                                To_ALL_TCAM_enable = 0
                            else:
                                To_ALL_TCAM_enable = 1
                            TargetTCAM = int(input_array[1]) & 0x3
                            SynchReset_to_TgtTCAM(gemroc_DAQ_XX, GEMROC_ID, TargetTCAM, To_ALL_TCAM_enable )
                            print '\nTo TCAM %d or ToALL_TCAM: %d on GEMROC %d: sent synchronous reset' %( TargetTCAM, To_ALL_TCAM_enable, GEMROC_ID )
                            time.sleep(2)
                            os.system('cls')
                            sys.stdout.write(menu_string)                
                    elif (input_array[0] == 'DSTART'):
                        if (len(input_array) == 3):
                            TCAM_Enable_pattern = int(input_array[1],0) & 0xFF
                            Periodic_FEB_TP_Enable_pattern = int(input_array[2],0) & 0xF
                            DAQ_set_and_TL_start(gemroc_DAQ_XX, GEMROC_ID, TCAM_Enable_pattern, Periodic_FEB_TP_Enable_pattern)
                            print '\nStart TL DAQ from enable TCAM pattern: %d on GEMROC %d' %( TCAM_Enable_pattern, GEMROC_ID )
                            time.sleep(2)
                            os.system('cls')
                            sys.stdout.write(menu_string)                
                    else:
                        print('\n bad command')
                        time.sleep(2)
                        os.system('cls')
                        sys.stdout.write(menu_string)                
                else:
                    input_queue.empty()
                    del input_array

Menu_and_prompt()
print "\nExit debug. Bye!"
log_file.close()
my_log.close()
IVT_log_file.close()
exit()


##cfg_filename = 'GEMROC_%d_def_cfg_LV_2018.txt' %GEMROC_ID
##gemroc_LV_XX = GEMconf_classes_2018v3.gemroc_cmd_LV_settings(GEMROC_ID,'NONE',1,cfg_filename)
##gemroc_LV_XX.set_FEB_PWR_EN_pattern(FEB_PWR_EN_pattern)
##gemroc_LV_XX.set_timing_toFEB_delay(20, 20, 20, 20) # acr 2017-12-07 NOTE this only writes the desidered setting into the GEMROC configuration register. A specific command is needed to
##                                                    # actually program the delay chips
##COMMAND_STRING = 'CMD_GEMROC_LV_CFG_WR'
##gemroc_LV_XX.set_gemroc_cmd_code(COMMAND_STRING,1)
##gemroc_LV_XX.update_command_words()
####gemroc_LV_XX.print_command_words()
####gemroc_LV_XX.extract_parameters_from_UDP_packet()
##array_to_send = gemroc_LV_XX.command_words
##command_echo = send_GEMROC_CFG_CMD_PKT( GEMROC_ID, COMMAND_STRING, array_to_send, DEST_IP_ADDRESS, DEST_PORT_NO )
##COMMAND_STRING = 'CMD_GEMROC_LV_CFG_RD'
##gemroc_LV_XX.set_gemroc_cmd_code(COMMAND_STRING,1)
##gemroc_LV_XX.update_command_words()
##array_to_send = gemroc_LV_XX.command_words
##command_echo = send_GEMROC_CFG_CMD_PKT( GEMROC_ID, COMMAND_STRING, array_to_send, DEST_IP_ADDRESS, DEST_PORT_NO )


##cfg_filename = 'GEMROC_%d_def_cfg_DAQ_2018.txt' %GEMROC_ID
##gemroc_DAQ_XX = GEMconf_classes_2018v3.gemroc_cmd_DAQ_settings(GEMROC_ID,'NONE',0,1,0,cfg_filename)
##gemroc_DAQ_XX.set_TP_width(3) ## acr 2017-10-03 override the default setting 
##gemroc_DAQ_XX.set_EN_TM_TCAM_pattern (0x01)
##gemroc_DAQ_XX.set_TP_Pos_nNeg (0x1)
##COMMAND_STRING = 'CMD_GEMROC_DAQ_CFG_WR'
##gemroc_DAQ_XX.set_gemroc_cmd_code(COMMAND_STRING,1)
##gemroc_DAQ_XX.update_command_words()
##array_to_send = gemroc_DAQ_XX.command_words
##command_echo = send_GEMROC_CFG_CMD_PKT( GEMROC_ID, COMMAND_STRING, array_to_send, DEST_IP_ADDRESS, DEST_PORT_NO )
##COMMAND_STRING = 'CMD_GEMROC_DAQ_CFG_RD'
##gemroc_DAQ_XX.set_gemroc_cmd_code(COMMAND_STRING,1)
##gemroc_DAQ_XX.update_command_words()
##array_to_send = gemroc_DAQ_XX.command_words
##command_echo = send_GEMROC_CFG_CMD_PKT( GEMROC_ID, COMMAND_STRING, array_to_send, DEST_IP_ADDRESS, DEST_PORT_NO )

##COMMAND_STRING = 'CMD_GEMROC_DAQ_TIGER_CFGREG_RESET'
##gemroc_DAQ_XX.set_gemroc_cmd_code(COMMAND_STRING,1)
##gemroc_DAQ_XX.update_command_words()
##array_to_send = gemroc_DAQ_XX.command_words
##command_echo = send_GEMROC_CFG_CMD_PKT( GEMROC_ID, COMMAND_STRING, array_to_send, DEST_IP_ADDRESS, DEST_PORT_NO )
##time.sleep(1)


##default_g_inst_settigs_filename = "TIGER_def_g_cfg_2018.txt"
##g_inst = GEMconf_classes_2018v3.g_reg_settings(GEMROC_ID,default_g_inst_settigs_filename)
##for t in range (0, 2): 
##    g_inst.set_target_TIGER(t)
##    COMMAND_STRING = 'WR'
##    g_inst.set_command_code(COMMAND_STRING)
##    g_inst.update_command_words()
##    array_to_send = g_inst.command_words
##    command_echo = send_TIGER_GCFG_Reg_CMD_PKT( t, COMMAND_STRING, array_to_send, DEST_IP_ADDRESS, DEST_PORT_NO )
##    time.sleep(1)
##    COMMAND_STRING = 'RD'
##    g_inst.set_command_code(COMMAND_STRING)
##    g_inst.update_command_words()
##    print '\nTIGER%d Global Cfg Reg CMD %s sent' %(t, COMMAND_STRING)
##    array_to_send = g_inst.command_words
##    command_echo = send_TIGER_GCFG_Reg_CMD_PKT( t, COMMAND_STRING, array_to_send, DEST_IP_ADDRESS, DEST_PORT_NO )
##    time.sleep(1)

##default_ch_inst_settigs_filename = "TIGER_def_ch_cfg_2018.txt"
##c_inst = GEMconf_classes_2018v3.ch_reg_settings(GEMROC_ID,default_ch_inst_settigs_filename)
##TriggerMode_default_TP_Disabled = 0
##TriggerMode_default_TP_Enabled = 1
##Target_Ch_ID = 0
##Target_Ch_ToALL = 1
##for t in range (0, 2): # loop over N TIGERs
##    c_inst.set_target_TIGER(t)
##    c_inst.set_to_ALL_param(1)
##    c_inst.TriggerMode = TriggerMode_default_TP_Disabled
##    COMMAND_STRING = 'WR'
##    c_inst.set_command_code(COMMAND_STRING)
##    c_inst.update_command_words()
##    array_to_send = c_inst.command_words
##    Target_Ch_ToALL = (array_to_send[(len(array_to_send)-1)] >> 6) & 0x1
##    Target_Ch_ID    = (array_to_send[(len(array_to_send)-1)] >> 0) & 0x3F
##    cmd_message = '\nTIGER%d; TOALL = %d; Channel%s Cfg Reg CMD %s sent' %(t, Target_Ch_ToALL, Target_Ch_ID, COMMAND_STRING) 
##    print cmd_message 
##    command_echo = send_TIGER_Ch_CFG_Reg_CMD_PKT( t, COMMAND_STRING, array_to_send, DEST_IP_ADDRESS, DEST_PORT_NO )
##    time.sleep(1)

##for t in range (0, 2): # loop over N TIGERs
##    c_inst.set_target_TIGER(t)
##    COMMAND_STRING = 'WR'
##    c_inst.set_command_code(COMMAND_STRING)
##    c_inst.set_to_ALL_param(0)
##    c_inst.set_target_channel(3)
##    c_inst.TriggerMode = TriggerMode_default_TP_Enabled
##    c_inst.update_command_words()
##    array_to_send = c_inst.command_words
##    command_echo = send_TIGER_Ch_CFG_Reg_CMD_PKT( t, COMMAND_STRING, array_to_send, DEST_IP_ADDRESS, DEST_PORT_NO )
##    time.sleep(1)

##for t in range (0, 2): # loop over N TIGERs
##    c_inst.set_target_TIGER(t)
##    c_inst.set_to_ALL_param(0)
##    COMMAND_STRING = 'RD'
##    c_inst.set_command_code(COMMAND_STRING)
##    for i in range (0, 64): # loop over all channels
##        c_inst.set_target_channel(i)
##        c_inst.update_command_words()
##        array_to_send = c_inst.command_words
##        command_echo = send_TIGER_Ch_CFG_Reg_CMD_PKT( t, COMMAND_STRING, array_to_send, DEST_IP_ADDRESS, DEST_PORT_NO )
##        display_TIGER_ChCFG_Readback(command_echo, 1, log_file)

#### acr 2018-01-23 sending a 'TIGER_SYNCH_RST' command to the TARGET GEMROC board
##gemroc_DAQ_XX.set_gemroc_cmd_code('CMD_GEMROC_DAQ_TIGER_SYNCH_RST',1)
##gemroc_DAQ_XX.update_command_words()
##array_to_send = gemroc_DAQ_XX.command_words
##buffer_to_send = struct.pack('>' + str(len(array_to_send)) + 'L', *array_to_send)
##clientSock.sendto(buffer_to_send, (DEST_IP_ADDRESS, DEST_PORT_NO))
##cmd_message = "\nCMD sent 'CMD_GEMROC_DAQ_TIGER_SYNCH_RST': \n" 
##log_file.write(cmd_message)
##log_file.write(binascii.b2a_hex(buffer_to_send))
##command_echo = receiveSock.recv(BUFSIZE)
##cmd_message = "\nCMD echo 'CMD_GEMROC_DAQ_TIGER_SYNCH_RST': \n" 
##log_file.write(cmd_message)
##log_file.write(binascii.b2a_hex(command_echo))
##time.sleep(1)

#### acr 2018-01-23 sending a 'TCAM_SYNCH_RST' command to the TARGET GEMROC board
##gemroc_DAQ_XX.set_gemroc_cmd_code('CMD_GEMROC_DAQ_TCAM_SYNCH_RST',1)
##gemroc_DAQ_XX.update_command_words()
##array_to_send = gemroc_DAQ_XX.command_words
##buffer_to_send = struct.pack('>' + str(len(array_to_send)) + 'L', *array_to_send)
##clientSock.sendto(buffer_to_send, (DEST_IP_ADDRESS, DEST_PORT_NO))
##cmd_message = "\nCMD sent 'CMD_GEMROC_DAQ_TCAM_SYNCH_RST': \n" 
##log_file.write(cmd_message)
##log_file.write(binascii.b2a_hex(buffer_to_send))
##command_echo = receiveSock.recv(BUFSIZE)
##cmd_message = "\nCMD echo 'CMD_GEMROC_DAQ_TCAM_SYNCH_RST': \n" 
##log_file.write(cmd_message)
##log_file.write(binascii.b2a_hex(command_echo))
##time.sleep(1)

#### acr 2017-10-03 set_AUTO_TP_EN_pattern and ENABLE triggerless data acquisition
##gemroc_DAQ_XX.set_TP_width(5) 
##gemroc_DAQ_XX.set_AUTO_TP_EN_pattern(0x0)
##gemroc_DAQ_XX.set_Periodic_TP_EN_pattern(0xF)
##gemroc_DAQ_XX.set_TL_nTM_ACQ(1)
##gemroc_DAQ_XX.set_TP_Pos_nNeg(1)
##gemroc_DAQ_XX.set_TP_period(8)
##gemroc_DAQ_XX.set_gemroc_cmd_code('CMD_GEMROC_DAQ_CFG_WR',1)
##gemroc_DAQ_XX.update_command_words()
###gemroc_DAQ_XX.print_command_words()
##gemroc_DAQ_XX.extract_parameters_from_UDP_packet()
##array_to_send = gemroc_DAQ_XX.command_words
##buffer_to_send = struct.pack('>' + str(len(array_to_send)) + 'L', *array_to_send)
##clientSock.sendto(buffer_to_send, (DEST_IP_ADDRESS, DEST_PORT_NO))
##cmd_message = "\nCMD sent (CMD_GEMROC_DAQ_CFG_WR): \n" 
##log_file.write(cmd_message)
##log_file.write(binascii.b2a_hex(buffer_to_send))
##command_echo = receiveSock.recv(BUFSIZE)
##cmd_message = "\nCMD echo (CMD_GEMROC_DAQ_CFG_WR): \n" 
##log_file.write(cmd_message)
##log_file.write(binascii.b2a_hex(command_echo))
