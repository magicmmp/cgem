# file: GEM_det_cfg_TIGER_GEMROC_FEB0_v25.py
# author: Angelo Cotta Ramusino INFN Ferrara
# date: 15 Jan 2018
# purpose: to debug TIGER configuration and data acquisition through the GEMROC prototype
# last modifications / additions
# acr 2018-01-15 modified "GEMconf_classes" to split LV parameter control from DAQ parameter control
# acr 2017-07-11 modified "GEMconf_classes" to read single TIGER global / channel configuration from files
import socket
import string
import time
import GEMconf_classes_2018v2 #acr 2018-02-19 import GEMconf_classes_2018
import struct
import binascii
import array
import classes_test_functions
import sys
import threading ## acr 2018-02-21
import Queue     ## acr 2018-02-21
import os        ## acr 2018-02-21
import datetime

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
    buffer_to_send = struct.pack('>' + str(len(array_to_send_param)) + 'L', *array_to_send_param)
    clientSock.sendto(buffer_to_send, (DEST_IP_ADDRESS_PARAM, DEST_PORT_NO_PARAM))
    cmd_message = "\nTarget GEMROC: %d; CMD sent (%s): \n" %(TARGET_GEMROC_ID_param, COMMAND_STRING_PARAM)
    log_file.write(cmd_message)
    log_file.write(binascii.b2a_hex(buffer_to_send))
    command_echo_f = receiveSock.recv(BUFSIZE)
    cmd_message = "\nTarget GEMROC: %d; \nCMD echo (%s): \n" %(TARGET_GEMROC_ID_param, COMMAND_STRING_PARAM)
    log_file.write(cmd_message)
    log_file.write(binascii.b2a_hex(command_echo_f))
    return command_echo_f

## acr 2018-02-19 BEGIN definining a function to call within the display loop
## acr 2018-02-23 def display_IVT(command_echo_param):
def display_and_log_IVT(command_echo_param, log_enable_param, log_filename_param): ## acr 2018-02-23 
    L_array = array.array('L') # L is an array of unsigned long
    L_array.fromstring(command_echo_param)
    L_array.byteswap()
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
    del L_array
    T_ref_PT1000 = 25.0
    V_ADC_at_25C = 247.2
    ADC_res_mV_1LSB = 0.305
    T_ADC_data_shift = 3
    shifted_T_ADC_res_mV_1LSB = 0.305 * (2 ** T_ADC_data_shift)
    V_ADC_data_shift = 4
    shifted_V_ADC_res_mV_1LSB = 0.305 * (2 ** V_ADC_data_shift)
    deltaT_over_deltaV_ratio = 1.28
    calibration_offset_mV_FEB3 = 4.0
    calibration_offset_mV_FEB2 = 4.0
    calibration_offset_mV_FEB1 = 4.0
    calibration_offset_mV_FEB0 = 4.0
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
    ## acr 2018-02-28 BEGIN prototype V2 have INA GAIN set at 200 instead of 50!!!###
    IADC_conv_fact_INA_GAIN_50 = 8.13
    IADC_conv_fact_INA_GAIN_200 = 8.13 / 4 ## mA per LSB
    if ( GEMROC_ID < 3):
        IADC_conv_fact_INA_GAIN = IADC_conv_fact_INA_GAIN_50
    else:
        IADC_conv_fact_INA_GAIN = IADC_conv_fact_INA_GAIN_200        
    ##FEB3_ID = (FEB3_ID_U * IADC_conv_fact_INA_GAIN_50) 
    ##FEB2_ID = (FEB2_ID_U * IADC_conv_fact_INA_GAIN_50) 
    ##FEB1_ID = (FEB1_ID_U * IADC_conv_fact_INA_GAIN_50) 
    ##FEB0_ID = (FEB0_ID_U * IADC_conv_fact_INA_GAIN_50) 
    ##FEB3_IA = (FEB3_IA_U * IADC_conv_fact_INA_GAIN_50) 
    ##FEB2_IA = (FEB2_IA_U * IADC_conv_fact_INA_GAIN_50) 
    ##FEB1_IA = (FEB1_IA_U * IADC_conv_fact_INA_GAIN_50) 
    ##FEB0_IA = (FEB0_IA_U * IADC_conv_fact_INA_GAIN_50)
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
    print'\n' + 'FEB3_T[degC]: ' + '%d; '%FEB3_T + 'FEB3_VD[mV]: ' + '%d; '%FEB3_VD + 'FEB3_ID[mA]: ' + '%d; '%FEB3_ID + 'FEB3_VA[mV]: ' + '%d; '%FEB3_VA + 'FEB3_IA[mA]: ' + '%d; '%FEB3_IA
    print'\n' + 'FEB2_T[degC]: ' + '%d; '%FEB2_T + 'FEB2_VD[mV]: ' + '%d; '%FEB2_VD + 'FEB2_ID[mA]: ' + '%d; '%FEB2_ID + 'FEB2_VA[mV]: ' + '%d; '%FEB2_VA + 'FEB2_IA[mA]: ' + '%d; '%FEB2_IA
    print'\n' + 'FEB1_T[degC]: ' + '%d; '%FEB1_T + 'FEB1_VD[mV]: ' + '%d; '%FEB1_VD + 'FEB1_ID[mA]: ' + '%d; '%FEB1_ID + 'FEB1_VA[mV]: ' + '%d; '%FEB1_VA + 'FEB1_IA[mA]: ' + '%d; '%FEB1_IA
    print'\n' + 'FEB0_T[degC]: ' + '%d; '%FEB0_T + 'FEB0_VD[mV]: ' + '%d; '%FEB0_VD + 'FEB0_ID[mA]: ' + '%d; '%FEB0_ID + 'FEB0_VA[mV]: ' + '%d; '%FEB0_VA + 'FEB0_IA[mA]: ' + '%d; '%FEB0_IA
    print'\n' + 'ROC_T: ' + '%d; '%ROC_T
    print'\n' + 'FEB3_OVT: ' + '%d; '%FEB3_OVT_FLAG + 'FEB3_DOVV: ' + '%d; '%FEB3_DOVV_FLAG + 'FEB3_DOVC: ' + '%d; '%FEB3_DOVC_FLAG + 'FEB3_AOVV: ' + '%d; '%FEB3_AOVV_FLAG + 'FEB3_AOVC: ' + '%d; '%FEB3_AOVC_FLAG
    print'\n' + 'FEB2_OVT: ' + '%d; '%FEB2_OVT_FLAG + 'FEB2_DOVV: ' + '%d; '%FEB2_DOVV_FLAG + 'FEB2_DOVC: ' + '%d; '%FEB2_DOVC_FLAG + 'FEB2_AOVV: ' + '%d; '%FEB2_AOVV_FLAG + 'FEB2_AOVC: ' + '%d; '%FEB2_AOVC_FLAG# acr 2018-10-19 END   test the functionality of the ADC read
    print'\n' + 'FEB1_OVT: ' + '%d; '%FEB1_OVT_FLAG + 'FEB1_DOVV: ' + '%d; '%FEB1_DOVV_FLAG + 'FEB1_DOVC: ' + '%d; '%FEB1_DOVC_FLAG + 'FEB1_AOVV: ' + '%d; '%FEB1_AOVV_FLAG + 'FEB1_AOVC: ' + '%d; '%FEB1_AOVC_FLAG
    print'\n' + 'FEB0_OVT: ' + '%d; '%FEB0_OVT_FLAG + 'FEB0_DOVV: ' + '%d; '%FEB0_DOVV_FLAG + 'FEB0_DOVC: ' + '%d; '%FEB0_DOVC_FLAG + 'FEB0_AOVV: ' + '%d; '%FEB0_AOVV_FLAG + 'FEB0_AOVC: ' + '%d; '%FEB0_AOVC_FLAG
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
    return
## acr 2018-02-19 END definining a function to call within the display loop

## ACR 2018-03-02 BEGIN DEPRECATED
##def display_TIGER_GCFG_Readback(command_echo_param, log_enable, log_fname_param):  #acr 2018-03-01
##    ##print (('\nTIGER%d  Gcfg RD echo          :' %t) + (binascii.b2a_hex(command_echo)))
##    L_array = array.array('L') # L is an array of unsigned long
##    L_array.fromstring(command_echo_param)
##    L_array.byteswap()
##    print'\nList of Global Config Register parameters read back from TIGER%d on RESPONDING GEMROC%d:'%( ((L_array[11] >> 8) & 0X7), ((L_array[0] >> 16) & 0X1f) ) # acr 2018-01-23
##    print ( "\nTIGER REPLY BYTE (LOW LEVEL SERIAL PROTOCOL ERROR FLAG):%02X;" %((L_array[11] >> 16) & 0XFF) )
##    print ( "\nBufferBias:%02X;\tTDCVcasN:%02X;\tTDCVcasP:%02X;\tTDCVcasPHyst:%02X;"                  %( ((L_array[1]>>24)&0x3), ((L_array[1]>>16)&0xF), ((L_array[1]>>8)&0x1F),((L_array[1]>>0)&0x3F) ) )
##    print ( "\nDiscFE_Ibias:%02X;\tBiasFE_PpreN:%02X;\tAVcasp_global:%02X;\tTDCcompVcas:%02X;"              %( ((L_array[2]>>24)&0x3F),((L_array[2]>>16)&0x3F),((L_array[2]>>8)&0x1F),((L_array[2]>>0)&0xF) ) )
##    print ( "\nTDCIres:%02X;\tDiscVcasN:%02X;\tIntegVb1:%02X;\tBiasFE_A1:%02X;"                       %( ((L_array[3]>>24)&0x1F),((L_array[3]>>16)&0xF), ((L_array[3]>>8)&0x3F),((L_array[3]>>0)&0xF) ) )
##    print ( "\nVcasp_Vth:%02X;\tTAC_I_LSB:%02X;\tTDCcompVbias:%02X;\tVref_Integ:%02X;"                  %( ((L_array[4]>>24)&0x3F),((L_array[4]>>16)&0x1F),((L_array[4]>>8)&0x1F),((L_array[4]>>0)&0x3F) ) )
##    print ( "\nIBiasTPcal:%02X;\tTP_Vcal:%02X;\tShaperIbias:%02X;\tIPostamp:%02X;"                    %( ((L_array[5]>>24)&0x1F),((L_array[5]>>16)&0x1F),((L_array[5]>>8)&0xF), ((L_array[5]>>0)&0x1F) ) )
##    print ( "\nTP_Vcal_ref:%02X;\tVref_integ_diff:%02X;\tSig_pol:%02X;\tFE_TPEnable:%02X;"              %( ((L_array[6]>>24)&0x1F),((L_array[6]>>16)&0x3F),((L_array[6]>>8)&0x1), ((L_array[6]>>0)&0x1) ) )
##    print ( "\nCompactDataFormat:%02X;\tDataClkDiv:%02X;\tTACrefreshPeriod:%02X;\tTACrefreshEnable:%02X;" %( ((L_array[7]>>24)&0x1), ((L_array[7]>>16)&0x3), ((L_array[7]>>8)&0xF), ((L_array[7]>>0)&0x1) ) )
##    print ( "\nCounterPeriod:%02X;\tCounterEnable:%02X;\tStopRampEnable:%02X;\tRClkEnable:%02X;"            %( ((L_array[8]>>24)&0x7), ((L_array[8]>>16)&0x1), ((L_array[8]>>8)&0x3), ((L_array[8]>>0)&0x1F) ) )
##    print ( "\nTDCClkdiv:%02X;\tVetoMode:%02X;\tCh_DebugMode:%02X;\tTxMode:%02X;"                       %( ((L_array[9]>>24)&0x1), ((L_array[9]>>16)&0x3F), ((L_array[9]>>8)&0x1 ), ((L_array[9]>>0)&0x3) ) )
##    print ( "\nTxDDR:%02X;\tTxLinks:%02X;"                                                                %( ((L_array[10]>>24)&0x1),((L_array[10]>>16)&0x3) ) )  
##    if (log_enable == 1):
##        log_fname_param.write ( '\nList of Global Config Register parameters read back from TIGER%d on RESPONDING GEMROC%d:'%( ((L_array[11] >> 8) & 0X7), ((L_array[0] >> 16) & 0X1f) ) )# acr 2018-03-02
##        log_fname_param.write ( "\nTIGER REPLY BYTE (LOW LEVEL SERIAL PROTOCOL ERROR FLAG):%02X;" %((L_array[11] >> 16) & 0XFF) )
##        log_fname_param.write ( "\nBufferBias:%02X;\t\tTDCVcasN:%02X;\t\tTDCVcasP:%02X;\t\tTDCVcasPHyst:%02X;"                  %( ((L_array[1]>>24)&0x3), ((L_array[1]>>16)&0xF), ((L_array[1]>>8)&0x1F),((L_array[1]>>0)&0x3F) ) )
##        log_fname_param.write ( "\nDiscFE_Ibias:%02X;\t\tBiasFE_PpreN:%02X;\t\tAVcasp_global:%02X;\t\tTDCcompVcas:%02X;"              %( ((L_array[2]>>24)&0x3F),((L_array[2]>>16)&0x3F),((L_array[2]>>8)&0x1F),((L_array[2]>>0)&0xF) ) )
##        log_fname_param.write ( "\nTDCIres:%02X;\t\tDiscVcasN:%02X;\t\tIntegVb1:%02X;\t\tBiasFE_A1:%02X;"                       %( ((L_array[3]>>24)&0x1F),((L_array[3]>>16)&0xF), ((L_array[3]>>8)&0x3F),((L_array[3]>>0)&0xF) ) )
##        log_fname_param.write ( "\nVcasp_Vth:%02X;\t\tTAC_I_LSB:%02X;\t\tTDCcompVbias:%02X;\t\tVref_Integ:%02X;"                  %( ((L_array[4]>>24)&0x3F),((L_array[4]>>16)&0x1F),((L_array[4]>>8)&0x1F),((L_array[4]>>0)&0x3F) ) )
##        log_fname_param.write ( "\nIBiasTPcal:%02X;\t\tTP_Vcal:%02X;\t\tShaperIbias:%02X;\t\tIPostamp:%02X;"                    %( ((L_array[5]>>24)&0x1F),((L_array[5]>>16)&0x1F),((L_array[5]>>8)&0xF), ((L_array[5]>>0)&0x1F) ) )
##        log_fname_param.write ( "\nTP_Vcal_ref:%02X;\t\tVref_integ_diff:%02X;\t\tSig_pol:%02X;\t\tFE_TPEnable:%02X;"              %( ((L_array[6]>>24)&0x1F),((L_array[6]>>16)&0x3F),((L_array[6]>>8)&0x1), ((L_array[6]>>0)&0x1) ) )
##        log_fname_param.write ( "\nCompactDataFormat:%02X;\t\tDataClkDiv:%02X;\t\tTACrefreshPeriod:%02X;\t\tTACrefreshEnable:%02X;" %( ((L_array[7]>>24)&0x1), ((L_array[7]>>16)&0x3), ((L_array[7]>>8)&0xF), ((L_array[7]>>0)&0x1) ) )
##        log_fname_param.write ( "\nCounterPeriod:%02X;\t\tCounterEnable:%02X;\t\tStopRampEnable:%02X;\t\tRClkEnable:%02X;"            %( ((L_array[8]>>24)&0x7), ((L_array[8]>>16)&0x1), ((L_array[8]>>8)&0x3), ((L_array[8]>>0)&0x1F) ) )
##        log_fname_param.write ( "\nTDCClkdiv:%02X;\t\tVetoMode:%02X;\t\tCh_DebugMode:%02X;\t\tTxMode:%02X;"                       %( ((L_array[9]>>24)&0x1), ((L_array[9]>>16)&0x3F), ((L_array[9]>>8)&0x1 ), ((L_array[9]>>0)&0x3) ) )
##        log_fname_param.write ( "\nTxDDR:%02X;\t\tTxLinks:%02X;"                                                                %( ((L_array[10]>>24)&0x1),((L_array[10]>>16)&0x3) ) )          
##    del L_array
##    return
## ACR 2018-03-02 END DEPRECATED

def display_log_GCfg_readback( command_echo_param, log_enable, log_fname_param): # acr 2018-03-02
    L_array = array.array('L') # L is an array of unsigned long
    L_array.fromstring(command_echo_param)
    L_array.byteswap()
    print('\nList of Global Config Register parameters read back from TIGER%d on RESPONDING GEMROC%d:'%( ((L_array[11] >> 8) & 0X7), ((L_array[0] >> 16) & 0X1f) ) ) # acr 2018-01-23
    print('\nTIGER REPLY BYTE (LOW LEVEL SERIAL PROTOCOL ERROR FLAG):%02X;' %(((L_array[11]>>16)&0xFF)) )
    print('\nBufferBias: %d' %(((L_array[1]>>24)&0x3)) )
    print('\nTDCVcasN: %d'  %(GEMconf_classes_2018v2.swap_order_N_bits(((L_array[1]>>16)&0xF),4)))
    print('\nTDCVcasP: %d'  %(GEMconf_classes_2018v2.swap_order_N_bits(((L_array[1]>>8)&0x1F),5)))
    print('\nTDCVcasPHyst: %d'  %(GEMconf_classes_2018v2.swap_order_N_bits(((L_array[1]>>0)&0x3F),6)))
    print('\nDiscFE_Ibias: %d'  %(GEMconf_classes_2018v2.swap_order_N_bits(((L_array[2]>>24)&0x3F),6)))
    print('\nBiasFE_PpreN: %d' %(((L_array[2]>>16)&0x3F)) )
    print('\nAVcasp_global: %d'  %(GEMconf_classes_2018v2.swap_order_N_bits(((L_array[2]>>8)&0x1F),5)))
    print('\nTDCcompVcas: %d'  %(GEMconf_classes_2018v2.swap_order_N_bits(((L_array[2]>>0)&0xF),4)))
    print('\nTDCIref_cs: %d'  %(GEMconf_classes_2018v2.swap_order_N_bits(((L_array[3]>>24)&0x1F),5)))
    print('\nDiscVcasN: %d'  %(GEMconf_classes_2018v2.swap_order_N_bits(((L_array[3]>>16)&0xF),4)))
    print('\nIntegVb1: %d'  %(GEMconf_classes_2018v2.swap_order_N_bits(((L_array[3]>>8)&0x3F),6)))
    print('\nBiasFE_A1: %d' %(((L_array[3]>>0)&0xF)) )
    print('\nVcasp_Vth: %d'  %(GEMconf_classes_2018v2.swap_order_N_bits(((L_array[4]>>24)&0x3F),6)))
    print('\nTAC_I_LSB: %d' %(((L_array[4]>>16)&0x1F)) )
    print('\nTDCcompVbias: %d' %(((L_array[4]>>8)&0x1F)) )
    print('\nVref_Integ: %d'  %(GEMconf_classes_2018v2.swap_order_N_bits(((L_array[4]>>0)&0x3F),6)))
    print('\nIBiasTPcal: %d'  %(GEMconf_classes_2018v2.swap_order_N_bits(((L_array[5]>>24)&0x1F),5)))
    print('\nTP_Vcal: %d'  %(GEMconf_classes_2018v2.swap_order_N_bits(((L_array[5]>>16)&0x1F),5)))
    print('\nShaperIbias: %d' %(((L_array[5]>>8)&0xF)) )
    print('\nIPostamp: %d'  %(GEMconf_classes_2018v2.swap_order_N_bits(((L_array[5]>>0)&0x1F),5)))
    print('\nTP_Vcal_ref: %d'  %(GEMconf_classes_2018v2.swap_order_N_bits(((L_array[6]>>24)&0x1F),5)))
    print('\nVref_integ_diff: %d'  %(GEMconf_classes_2018v2.swap_order_N_bits(((L_array[6]>>16)&0x3F),6)))
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
        log_fname_param.write('\nTDCVcasN: %d'  %(GEMconf_classes_2018v2.swap_order_N_bits(((L_array[1]>>16)&0xF),4)))
        log_fname_param.write('\nTDCVcasP: %d'  %(GEMconf_classes_2018v2.swap_order_N_bits(((L_array[1]>>8)&0x1F),5)))
        log_fname_param.write('\nTDCVcasPHyst: %d'  %(GEMconf_classes_2018v2.swap_order_N_bits(((L_array[1]>>0)&0x3F),6)))
        log_fname_param.write('\nDiscFE_Ibias: %d'  %(GEMconf_classes_2018v2.swap_order_N_bits(((L_array[2]>>24)&0x3F),6)))
        log_fname_param.write('\nBiasFE_PpreN: %d' %(((L_array[2]>>16)&0x3F)) )
        log_fname_param.write('\nAVcasp_global: %d'  %(GEMconf_classes_2018v2.swap_order_N_bits(((L_array[2]>>8)&0x1F),5)))
        log_fname_param.write('\nTDCcompVcas: %d'  %(GEMconf_classes_2018v2.swap_order_N_bits(((L_array[2]>>0)&0xF),4)))
        log_fname_param.write('\nTDCIref_cs: %d'  %(GEMconf_classes_2018v2.swap_order_N_bits(((L_array[3]>>24)&0x1F),5)))
        log_fname_param.write('\nDiscVcasN: %d'  %(GEMconf_classes_2018v2.swap_order_N_bits(((L_array[3]>>16)&0xF),4)))
        log_fname_param.write('\nIntegVb1: %d'  %(GEMconf_classes_2018v2.swap_order_N_bits(((L_array[3]>>8)&0x3F),6)))
        log_fname_param.write('\nBiasFE_A1: %d' %(((L_array[3]>>0)&0xF)) )
        log_fname_param.write('\nVcasp_Vth: %d'  %(GEMconf_classes_2018v2.swap_order_N_bits(((L_array[4]>>24)&0x3F),6)))
        log_fname_param.write('\nTAC_I_LSB: %d' %(((L_array[4]>>16)&0x1F)) )
        log_fname_param.write('\nTDCcompVbias: %d' %(((L_array[4]>>8)&0x1F)) )
        log_fname_param.write('\nVref_Integ: %d'  %(GEMconf_classes_2018v2.swap_order_N_bits(((L_array[4]>>0)&0x3F),6)))
        log_fname_param.write('\nIBiasTPcal: %d'  %(GEMconf_classes_2018v2.swap_order_N_bits(((L_array[5]>>24)&0x1F),5)))
        log_fname_param.write('\nTP_Vcal: %d'  %(GEMconf_classes_2018v2.swap_order_N_bits(((L_array[5]>>16)&0x1F),5)))
        log_fname_param.write('\nShaperIbias: %d' %(((L_array[5]>>8)&0xF)) )
        log_fname_param.write('\nIPostamp: %d'  %(GEMconf_classes_2018v2.swap_order_N_bits(((L_array[5]>>0)&0x1F),5)))
        log_fname_param.write('\nTP_Vcal_ref: %d'  %(GEMconf_classes_2018v2.swap_order_N_bits(((L_array[6]>>24)&0x1F),5)))
        log_fname_param.write('\nVref_integ_diff: %d'  %(GEMconf_classes_2018v2.swap_order_N_bits(((L_array[6]>>16)&0x3F),6)))
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
    L_array = array.array('L') # L is an array of unsigned long
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

def send_TIGER_GCFG_Reg_CMD_PKT( TIGER_ID_param, COMMAND_STRING_PARAM, array_to_send_param, DEST_IP_ADDRESS_PARAM, DEST_PORT_NO_PARAM): #acr 2018-02-20
    buffer_to_send = struct.pack('>' + str(len(array_to_send_param)) + 'L', *array_to_send_param)
    clientSock.sendto(buffer_to_send, (DEST_IP_ADDRESS_PARAM, DEST_PORT_NO_PARAM))
    cmd_message = '\nTIGER%d Global Cfg Reg CMD %s sent:\n' %(TIGER_ID_param, COMMAND_STRING_PARAM)
    log_file.write(cmd_message)
    log_file.write(binascii.b2a_hex(buffer_to_send))
    ##print (('\nTIGER%d  Gcfg WR echo          :' %t) + (binascii.b2a_hex(buffer_to_send))) # acr 2018-01-23
    ##cmd_message = "\nTIGER%d CMD echo (Global REG WR): \n" %t
    ##cmd_message = "\nCMD echo (%s): \n" %COMMAND_STRING_PARAM
    cmd_message = '\nTIGER%d Global Cfg Reg CMD %s command echo:\n' %(TIGER_ID_param, COMMAND_STRING_PARAM)
    log_file.write(cmd_message)
    command_echo_f = receiveSock.recv(BUFSIZE)
    log_file.write(binascii.b2a_hex(command_echo_f))
    ##return command_echo_f
    return command_echo_f

def send_TIGER_Ch_CFG_Reg_CMD_PKT( TIGER_ID_param, COMMAND_STRING_PARAM, array_to_send_param, DEST_IP_ADDRESS_PARAM, DEST_PORT_NO_PARAM): #acr 2018-02-20
    buffer_to_send = struct.pack('>' + str(len(array_to_send_param)) + 'L', *array_to_send_param)
    clientSock.sendto(buffer_to_send, (DEST_IP_ADDRESS_PARAM, DEST_PORT_NO_PARAM))
    ##print (('\nTIGER%d  Gcfg WR buffer to send:' %TIGER_ID_param) + (binascii.b2a_hex(buffer_to_send))) 
    ##cmd_message = "\nTIGER%d CMD sent (Global REG WR): \n" %TIGER_ID_param
    Target_Ch_ToALL_f = (array_to_send_param[(len(array_to_send_param)-1)] >> 6) & 0x1
    Target_Ch_ID_f    = (array_to_send_param[(len(array_to_send_param)-1)] >> 0) & 0x3F
    cmd_message = '\nTIGER% d; TOALL = % d; Channel% s Cfg Reg CMD %s sent\n' %(TIGER_ID_param, Target_Ch_ToALL_f, Target_Ch_ID_f, COMMAND_STRING_PARAM) 
    log_file.write(cmd_message)
    log_file.write(binascii.b2a_hex(buffer_to_send))
    ##print (('\nTIGER%d  Gcfg WR echo          :' %t) + (binascii.b2a_hex(buffer_to_send))) # acr 2018-01-23
    ##cmd_message = "\nTIGER%d CMD echo (Channel REG WR): \n" %t
    ##cmd_message = "\nCMD echo (%s): \n" %COMMAND_STRING_PARAM
    cmd_message = '\nTIGER% d; TOALL = % d; Channel% s Cfg Reg CMD %s echo\n' %(TIGER_ID_param, Target_Ch_ToALL_f, Target_Ch_ID_f, COMMAND_STRING_PARAM) 
    log_file.write(cmd_message)
    command_echo_f = receiveSock.recv(BUFSIZE)
    log_file.write(binascii.b2a_hex(command_echo_f))
    ##return command_echo_f
    return command_echo_f

def GEMROC_IVT_read_and_log(GEMROC_ID_param, log_enable_param, log_filename_param):
    gemroc_LV_XX.set_target_GEMROC(GEMROC_ID_param)
    COMMAND_STRING = 'CMD_GEMROC_LV_IVT_READ'
    gemroc_LV_XX.set_gemroc_cmd_code(COMMAND_STRING,1)
    gemroc_LV_XX.update_command_words()
    array_to_send = gemroc_LV_XX.command_words
    command_echo = send_GEMROC_CFG_CMD_PKT( GEMROC_ID_param, COMMAND_STRING, array_to_send, DEST_IP_ADDRESS, DEST_PORT_NO )
    print '\nGEMROC_ID: %d: CMD_GEMROC_LV_IVT_READ' %GEMROC_ID_param
    ##display_IVT(command_echo)
    display_and_log_IVT(command_echo, log_enable_param, log_filename_param)
    return

def send_GEMROC_LV_CMD (GEMROC_ID_param, COMMAND_STRING_PARAM):
    gemroc_LV_XX.set_target_GEMROC(GEMROC_ID_param)
    gemroc_LV_XX.set_gemroc_cmd_code(COMMAND_STRING_PARAM,1)
    gemroc_LV_XX.update_command_words()
    array_to_send = gemroc_LV_XX.command_words
    command_echo = send_GEMROC_CFG_CMD_PKT( GEMROC_ID_param, COMMAND_STRING_PARAM, array_to_send, DEST_IP_ADDRESS, DEST_PORT_NO )
    return command_echo

def FEBPwrEnPattern_set(GEMROC_ID_param, FEB_PWREN_pattern_param):    
    gemroc_LV_XX.set_FEB_PWR_EN_pattern(FEB_PWREN_pattern_param)    
    COMMAND_STRING = 'CMD_GEMROC_LV_CFG_WR'
    command_echo = send_GEMROC_LV_CMD (GEMROC_ID_param, COMMAND_STRING)
    return command_echo

##def send_GEMROC_DAQ_CMD (GEMROC_ID_param, COMMAND_STRING_PARAM):
##    gemroc_DAQ_XX.set_target_GEMROC(GEMROC_ID_param)
##    gemroc_DAQ_XX.set_gemroc_cmd_code(COMMAND_STRING_PARAM,1)
##    gemroc_DAQ_XX.update_command_words()
##    array_to_send = gemroc_DAQ_XX.command_words
##    command_echo = send_GEMROC_CFG_CMD_PKT(COMMAND_STRING_PARAM, array_to_send, DEST_IP_ADDRESS, DEST_PORT_NO )
##    return command_echo
##def ResetTgtGEMROC_ALL_TIGER_GCfgReg (GEMROC_ID_param, TIGER_ID_param):
##    COMMAND_STRING = 'CMD_GEMROC_DAQ_TIGER_GCFGREG_RESET'
##    command_echo = send_GEMROC_DAQ_CMD (GEMROC_ID_param, COMMAND_STRING_PARAM)
##    return command_echo
def send_GEMROC_DAQ_CMD (gemroc_DAQ_inst_param, GEMROC_ID_param, COMMAND_STRING_PARAM):
    gemroc_DAQ_inst_param.set_target_GEMROC(GEMROC_ID_param)
    gemroc_DAQ_inst_param.set_gemroc_cmd_code(COMMAND_STRING_PARAM,1)
    gemroc_DAQ_inst_param.update_command_words()
    array_to_send = gemroc_DAQ_inst_param.command_words
    command_echo = send_GEMROC_CFG_CMD_PKT( GEMROC_ID_param, COMMAND_STRING_PARAM, array_to_send, DEST_IP_ADDRESS, DEST_PORT_NO )
    return command_echo

def ResetTgtGEMROC_ALL_TIGER_GCfgReg (gemroc_DAQ_inst_param, GEMROC_ID_param):
    COMMAND_STRING = 'CMD_GEMROC_DAQ_TIGER_GCFGREG_RESET'
    command_echo = send_GEMROC_DAQ_CMD (gemroc_DAQ_inst_param, GEMROC_ID_param, COMMAND_STRING)
    ##return command_echo
    
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
    command_echo = send_TIGER_GCFG_Reg_CMD_PKT( TIGER_ID_param, COMMAND_STRING, array_to_send, DEST_IP_ADDRESS, DEST_PORT_NO )
    return command_echo
##def WriteTgtGEMROC_TIGER_GCfgReg_fromfile (GEMROC_ID_param, TIGER_ID_param, GCFGReg_def_fname_param):
##    g_inst.reload_gcfg_settings_from_file(GCFGReg_def_fname_param) ## acr 2018-02-23 new method to reaload from a default file
##    g_inst.TARGET_GEMROC_ID(GEMROC_ID_param)
##    g_inst.set_target_TIGER(TIGER_ID_param)
##    COMMAND_STRING = 'WR'
##    g_inst.set_command_code(COMMAND_STRING)
##    g_inst.update_command_words()
##    array_to_send = g_inst.command_words
##    command_echo = send_TIGER_GCFG_Reg_CMD_PKT( t, COMMAND_STRING, array_to_send, DEST_IP_ADDRESS, DEST_PORT_NO )
##    return command_echo

def ReadTgtGEMROC_TIGER_GCfgReg (GCFGReg_setting_inst, GEMROC_ID_param, TIGER_ID_param):
    GCFGReg_setting_inst.set_target_GEMROC(GEMROC_ID_param)
    GCFGReg_setting_inst.set_target_TIGER(TIGER_ID_param)
    COMMAND_STRING = 'RD'
    GCFGReg_setting_inst.set_command_code(COMMAND_STRING)
    GCFGReg_setting_inst.update_command_words()
    array_to_send = GCFGReg_setting_inst.command_words
    command_echo = send_TIGER_GCFG_Reg_CMD_PKT( TIGER_ID_param, COMMAND_STRING, array_to_send, DEST_IP_ADDRESS, DEST_PORT_NO )
    return command_echo

def WriteTgtGEMROC_TIGER_ChCfgReg_fromfile (ChCFGReg_setting_inst, GEMROC_ID_param, TIGER_ID_param, channel_ID_param, ChCFGReg_def_fname_param): 
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
        command_echo = send_TIGER_Ch_CFG_Reg_CMD_PKT(TIGER_ID_param, COMMAND_STRING, array_to_send, DEST_IP_ADDRESS, DEST_PORT_NO )
    else:
        for i in range (0, 64):
            ChCFGReg_setting_inst.set_target_channel(i)
            ChCFGReg_setting_inst.update_command_words()
            array_to_send = ChCFGReg_setting_inst.command_words
            command_echo = send_TIGER_Ch_CFG_Reg_CMD_PKT(TIGER_ID_param, COMMAND_STRING, array_to_send, DEST_IP_ADDRESS, DEST_PORT_NO )
    return command_echo

def ReadTgtGEMROC_TIGER_ChCfgReg (ChCFGReg_setting_inst, GEMROC_ID_param, TIGER_ID_param, channel_ID_param, verbose_mode):
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
        command_echo = send_TIGER_Ch_CFG_Reg_CMD_PKT(TIGER_ID_param, COMMAND_STRING, array_to_send, DEST_IP_ADDRESS, DEST_PORT_NO )
        if (verbose_mode == 1):
            display_log_ChCfg_readback(command_echo, 1, log_file)
    else:
        for i in range (0, 64):
            ChCFGReg_setting_inst.set_target_channel(i)
            ChCFGReg_setting_inst.update_command_words()
            array_to_send = ChCFGReg_setting_inst.command_words
            command_echo = send_TIGER_Ch_CFG_Reg_CMD_PKT(TIGER_ID_param, COMMAND_STRING, array_to_send, DEST_IP_ADDRESS, DEST_PORT_NO )
            if (verbose_mode == 1):
                display_log_ChCfg_readback(command_echo, 1, log_file)
    return command_echo

##(TP)EW_ch <GEMROC_ID>(1 thru 22) <TIGER_ID>(0 thru 7) <channel_ID>(0 thru 63) <On_Off_param> (0 or 1); if 64 then act on Test Pulse enable for ALL
def Set_GEMROC_TIGER_ch_TPEn (ChCFGReg_setting_inst, GEMROC_ID_param, TIGER_ID_param, Channel_ID_param, On_Off_param):
    ChCFGReg_setting_inst.set_target_GEMROC(GEMROC_ID_param)
    ChCFGReg_setting_inst.set_target_TIGER(TIGER_ID_param)
    ChCFGReg_setting_inst.set_to_ALL_param (0) ## let's do multiple configuration under script control rather than under GEMROC NIOS2 processor control
    COMMAND_STRING = 'WR'
    ChCFGReg_setting_inst.set_command_code(COMMAND_STRING)
    if (Channel_ID_param < 64):
        ChCFGReg_setting_inst.set_target_channel(Channel_ID_param)
        ChCFGReg_setting_inst.TriggerMode = On_Off_param
        ChCFGReg_setting_inst.update_command_words()
        array_to_send = ChCFGReg_setting_inst.command_words
        command_echo = send_TIGER_Ch_CFG_Reg_CMD_PKT(TIGER_ID_param, COMMAND_STRING, array_to_send, DEST_IP_ADDRESS, DEST_PORT_NO )
    else:
        for i in range (0, 64):
            ChCFGReg_setting_inst.set_target_channel(i)
            ChCFGReg_setting_inst.TriggerMode = On_Off_param
            ChCFGReg_setting_inst.update_command_words()
            array_to_send = ChCFGReg_setting_inst.command_words
            command_echo = send_TIGER_Ch_CFG_Reg_CMD_PKT(TIGER_ID_param, COMMAND_STRING, array_to_send, DEST_IP_ADDRESS, DEST_PORT_NO )
    last_command_echo = command_echo
    return last_command_echo

def SynchReset_to_TgtFEB(gemroc_DAQ_inst, GEMROC_ID_param, TargetFEB_param, To_ALL_param):
    gemroc_DAQ_inst.set_target_GEMROC(GEMROC_ID_param)
    gemroc_DAQ_inst.set_target_TCAM_ID(TargetFEB_param, To_ALL_param)
    COMMAND_STRING = 'CMD_GEMROC_DAQ_TIGER_SYNCH_RST'
    command_echo = send_GEMROC_DAQ_CMD (gemroc_DAQ_inst, GEMROC_ID_param, COMMAND_STRING)
    return command_echo

def SynchReset_to_TgtTCAM(gemroc_DAQ_inst, GEMROC_ID_param, TargetTCAM_param, To_ALL_param):
    gemroc_DAQ_inst.set_target_GEMROC(GEMROC_ID_param)
    gemroc_DAQ_inst.set_target_TCAM_ID(TargetTCAM_param, To_ALL_param)
    COMMAND_STRING = 'CMD_GEMROC_DAQ_TCAM_SYNCH_RST'
    command_echo = send_GEMROC_DAQ_CMD (gemroc_DAQ_inst, GEMROC_ID_param, COMMAND_STRING)
    return command_echo

def DAQ_set_and_TL_start(gemroc_DAQ_inst, GEMROC_ID_param, TCAM_Enable_pattern_param):
    gemroc_DAQ_inst.set_target_GEMROC(GEMROC_ID_param)
    gemroc_DAQ_inst.set_EN_TM_TCAM_pattern (0x01)
    gemroc_DAQ_inst.set_TP_width(5) 
    gemroc_DAQ_inst.set_AUTO_TP_EN_pattern(0x0)
    gemroc_DAQ_inst.set_Periodic_TP_EN_pattern(0xF)
    gemroc_DAQ_inst.set_TL_nTM_ACQ(1)
    gemroc_DAQ_inst.set_TP_Pos_nNeg(1)
    gemroc_DAQ_inst.set_TP_period(8)
    gemroc_DAQ_inst.set_gemroc_cmd_code('CMD_GEMROC_DAQ_CFG_WR',1)
    COMMAND_STRING = 'CMD_GEMROC_DAQ_CFG_WR'
    command_echo = send_GEMROC_DAQ_CMD (gemroc_DAQ_inst, GEMROC_ID_param, COMMAND_STRING)
    return command_echo

################8888888888888
##   def reload_gcfg_settings_from_file(self, GCFGReg_def_fname_param):
##      self.parameter_array = [0 for i in range(37)] # acr 2018-01-25 [0 for i in range(38)]
##      with open(GCFGReg_def_fname_param, "r") as f:
##         self.parameter_array = map(int, f)
##      self.BufferBias = self.parameter_array [0] ## BufferBias_param; default 0
##      self.TDCVcasN        = swap_order_N_bits(self.parameter_array [1],4)  # acr 2018-01-25 ## TDCVcasN_param; default 0
##      self.TDCVcasP        = swap_order_N_bits(self.parameter_array [2],5)  # acr 2018-01-25 ## TDCVcasP_param; default 0
##      self.TDCVcasPHyst    = swap_order_N_bits(self.parameter_array [3],6)  # acr 2018-01-25 ## TDCVcasPHyst_param; default 55
##      self.DiscFE_Ibias    = swap_order_N_bits(self.parameter_array [4],6)  # acr 2018-01-25 ## DiscFE_Ibias_param; default 50
##      self.BiasFE_PpreN    = self.parameter_array [5] ## BiasFE_PpreN_param; default 10
##      self.AVcasp_global   = swap_order_N_bits(self.parameter_array [6],5)  # acr 2018-01-25 ## AVcasp_global_param; default 20
##      self.TDCcompVcas     = swap_order_N_bits(self.parameter_array [7],4)  # acr 2018-01-25 ## TDCcompVcas_param; default 0
##      self.TDCIref_cs      = swap_order_N_bits(self.parameter_array [8],5)  # acr 2018-01-25 ## TDCIref_cs_param; default 20
##      self.DiscVcasN       = swap_order_N_bits(self.parameter_array [9],4)  # acr 2018-01-25 elf.parameter_array [9] ## DiscVcasN_param; default 7
##      self.IntegVb1        = swap_order_N_bits(self.parameter_array [10],6) # acr 2018-01-25 ## IntegVb1_param; default 27
##      self.BiasFE_A1 = self.parameter_array [11] ## BiasFE_A1_param; default 8
##      self.Vcasp_Vth       = swap_order_N_bits(self.parameter_array [12],6) # acr 2018-01-25 ## Vcasp_Vth_param; default 55
##      self.TAC_I_LSB = self.parameter_array [13] ## TAC_I_LSB_param; default 0
##      self.TDCcompVbias = self.parameter_array [14] ## TDCcompVbias_param; default 0
##      self.Vref_Integ      = swap_order_N_bits(self.parameter_array [15],6) # acr 2018-01-25 ## Vref_Integ_param; default 55
##      self.IBiasTPcal      = swap_order_N_bits(self.parameter_array [16],5) # acr 2018-01-25 ## IBiasTPcal_param; default 29
##      self.TP_Vcal         = swap_order_N_bits(self.parameter_array [17],5) # acr 2018-01-25 ## TP_Vcal_param; default 8
##      self.ShaperIbias = self.parameter_array [18] ## ShaperIbias_param; default 0
##      self.IPostamp        = swap_order_N_bits(self.parameter_array [19],5) # acr 2018-01-25 ## IPostamp_param; default 26
##      self.TP_Vcal_ref     = swap_order_N_bits(self.parameter_array [20],5) # acr 2018-01-25 ## TP_Vcal_ref_param; default 23
##      self.Vref_integ_diff = swap_order_N_bits(self.parameter_array [21],6) # acr 2018-01-25 ## Vref_integ_diff_param; default 39
##      self.Sig_pol = self.parameter_array [22] ## Sig_pol_param; default p-type (1?)
##      self.FE_TPEnable = self.parameter_array [23] ## FE_TPEnable_param; default X (1?)
##      # acr 2018-01-25 removing unused parameter self.CompactDataFormat = self.parameter_array [24] ## CompactDataFormat_param; default EMPTY BOX (0?)
##      self.DataClkDiv = self.parameter_array [24] # acr 2018-01-25 [25] ## DataClkDiv_param; default 0
##      self.TACrefreshPeriod = self.parameter_array [25] # acr 2018-01-25 [26] ## TACrefreshPeriod_param; default 9
##      self.TACrefreshEnable = self.parameter_array [26] # acr 2018-01-25 [27] ## TACrefreshEnable_param; default X (1?)
##      self.CounterPeriod = self.parameter_array [27] # acr 2018-01-25 [28] ## CounterPeriod_param; default 6
##      self.CounterEnable = self.parameter_array [28] # acr 2018-01-25 [29] ## CounterEnable_param; default EMPTYBOX (0?)
##      self.StopRampEnable = self.parameter_array [29] # acr 2018-01-25 [30] ## StopRampEnable_param; default 0
##      self.RClkEnable = self.parameter_array [30] # acr 2018-01-25 [31] ## RClkEnable_param; default 7
##      self.TDCClkdiv = self.parameter_array [31] # acr 2018-01-25 [32] ## TDCClkdiv_param; default EMPTYBOX (0?)
##      self.VetoMode = self.parameter_array [32] # acr 2018-01-25 [33] ## VetoMode_param; default 0
##      self.Ch_DebugMode = self.parameter_array [33] # acr 2018-01-25 [34] ## Ch_DebugMode_param; default EMPTYBOX (0?)
##      self.TxMode = self.parameter_array [34] # acr 2018-01-25 [35] ## TxMode_param; default 2
##      self.TxDDR = self.parameter_array [35] # acr 2018-01-25 [36] ## TxDDR_param; default X (1?)
##      self.TxLinks = self.parameter_array [36] # acr 2018-01-25 [37] ## TxLinks_param; default 2 links nel GUI di Torino; dovrebbe corrispondeere ad un codice 1 per 2 links
################8888888888888

##if (len(sys.argv) < 3):
##    print"\n GEMROC_TIGER_CFG argument list: <Target_GEMROC: 1 thru 22> <Target_FEB_PWR_EN_pattern(Hex):0x0 thru 0xF>"
####    exit()
##    default_arg_needed = 1
##if (default_arg_needed == 1):     
##    TARGET_GEMROC_ID_param = 3
##    TARGET_FEB_PWR_PATTERN_param = 0x1
##else:
##    TARGET_GEMROC_ID_param = int(sys.argv[1])
##    TARGET_FEB_PWR_PATTERN_param = int(sys.argv[2],16)
##    
##GEMROC_ID = TARGET_GEMROC_ID_param
##FEB_PWR_EN_pattern = TARGET_FEB_PWR_PATTERN_param

##print ("\n GEMROC_ID %d" %GEMROC_ID)
##print ("\n FEB_PWR_EN_pattern %X" %FEB_PWR_EN_pattern)

##log_fname = 'tiger%d_cfg_log.txt'% tiger_index
##log_file = open(log_fname, 'w')
log_fname = 'GEMROC%d_interactive_cfg_log.txt' %GEMROC_ID
log_file = open(log_fname, 'w')
IVT_log_fname = 'GEMROC%d_IVT_log.txt' %GEMROC_ID
IVT_log_file = open(IVT_log_fname, 'w')

# BEGIN uncomment for connection to the GEMROC module
HOST_IP = "192.168.1.200"
# END   uncomment for connection to the GEMROC module

# BEGIN uncomment for test only
#HOST_IP = "127.0.0.1"
# END   uncomment for test only

HOST_PORT = 54816+15
HOST_PORT_RECEIVE = 58912+15

# BEGIN uncomment for connection to the GEMROC module
DEST_IP_ADDRESS = "192.168.1.%d" %(GEMROC_ID+16)
DEST_PORT_NO = 58912+1
# END   uncomment for connection to the GEMROC module

# BEGIN uncomment for test only
#DEST_IP_ADDRESS = "127.0.0.1"
#DEST_PORT_NO = 58912+15
# END   uncomment for test only

tiger_index = 0
BUFSIZE = 4096 

clientSock = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)
receiveSock = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)
clientSock.bind((HOST_IP, HOST_PORT))
receiveSock.bind((HOST_IP, HOST_PORT_RECEIVE))

## creating an instance of the GEMROC LV configuration settings object
## parameter list:
##  TARGET_GEMROC_ID_param = 0, # acr 2017-09-22              
##  command_string_param = 'NONE',
##  number_of_repetitions_param = 1,
cfg_filename = 'GEMROC_%d_def_cfg_LV_2018.txt' %GEMROC_ID
gemroc_LV_XX = GEMconf_classes_2018v2.gemroc_cmd_LV_settings(GEMROC_ID,'NONE',1,cfg_filename)
gemroc_LV_XX.set_FEB_PWR_EN_pattern(FEB_PWR_EN_pattern)
COMMAND_STRING = 'CMD_GEMROC_LV_CFG_WR'
gemroc_LV_XX.set_gemroc_cmd_code(COMMAND_STRING,1)
gemroc_LV_XX.update_command_words()
##gemroc_LV_XX.print_command_words()
##gemroc_LV_XX.extract_parameters_from_UDP_packet()
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
cfg_filename = 'GEMROC_%d_def_cfg_DAQ_2018.txt' %GEMROC_ID
gemroc_DAQ_XX = GEMconf_classes_2018v2.gemroc_cmd_DAQ_settings(GEMROC_ID,'NONE',0,1,0,cfg_filename)

## creating an instance of the TIGER global configuration settings object
##    parameter list:
##    TARGET_GEMROC_ID_param = 0, 
##    cfg_filename_param = "default_g_cfg_2018_all_big_endian.txt"
default_g_inst_settigs_filename = "TIGER_def_g_cfg_2018.txt"
g_inst = GEMconf_classes_2018v2.g_reg_settings(GEMROC_ID,default_g_inst_settigs_filename)

#### creating an instance of the channel configuration settings object
##    parameter list:
##    TARGET_GEMROC_ID_param = 0, 
##    cfg_filename_param = "default_ch_cfg_mainz.txt" 
default_ch_inst_settigs_filename = "TIGER_def_ch_cfg_2018.txt"
c_inst = GEMconf_classes_2018v2.ch_reg_settings(GEMROC_ID,default_ch_inst_settigs_filename)

## acr 2018-02-21 BEGIN write the user interface
menu_string = '\
\nGEMROC_TIGER_CFG V1.0. A.C.R. 21 Feb 2018 INFN-FE \
\nMENU: \
\n(P)EW     <PWR_EN_PATTERN>(Hex, 0x0 thru 0xF) \
\n(I)VT                                                                             read and print IVT parameters of GEM\
\n GRST                                                                             RESETS ALL TIGER CONFIGURATION REGISTERS: must be executed before TIGER Global Register Write \
\n(GW)def   <TIGER_ID>(0 thru 7)                                                    (default file: "TIGER_def_g_cfg_2018.txt") \
\n(CW)def   <TIGER_ID>(0 thru 7) <channel_ID>(0 thru 64);                           if channel_ID =64 then WRITE ALL IN SEQUENCE (default file: "TIGER_def_ch_cfg_2018.txt"); \
\n(GR)d     <TIGER_ID>(0 thru 7)  \
\n(CR)d     <TIGER_ID>(0 thru 7) <channel_ID>(0 thru 64);                           if 64 then READ ALL IN SEQUENCE \
\n(TP)EW_ch <TIGER_ID>(0 thru 7) <channel_ID>(0 thru 64) <On_Off_param> (0 or 1);   if 64 then act on Test Pulse enable for ALL\
\n SRst     <SynchRst_TargetFEB>(0 to 4)                                            send a sycnh rst to the target FEB(0 to 3); if 4 is input then send synch rst to ALL FEBs  \
\n DRst     <SynchRst_TargetTCAM>(0 to 4)                                           send a sycnh rst to the TIGER Data Processing unit (TCAM)0 to 3; if 4 is input then send synch rst to ALL TCAMs    \
\n DSTART   <TCAM_En_pattern>(Hex, 0x0 thru 0xF)                                    set GEMROC to start sending triggerless data from enabled TCAM units  \
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
    fmt='%Y-%m-%d-%H-%M-%S'
    IVT_log_file.write('\n%s' %datetime.datetime.now().strftime(fmt) )
    GEMROC_IVT_read_and_log(GEMROC_ID, IVT_LOG_ENABLE, IVT_log_file)
    ##os.system('cls')
    sys.stdout.write(menu_string)
    #print "Enter your command: "

    while not DONE:
        if ((time.time()-last_update)>IVT_LOG_PERIOD_SECONDS) :
            os.system('cls')
            IVT_log_file.write('\n%s' %datetime.datetime.now().strftime(fmt) )
            GEMROC_IVT_read_and_log(GEMROC_ID, IVT_LOG_ENABLE, IVT_log_file)
            last_update = time.time()
##            time.sleep(2)
##            os.system('cls')
            sys.stdout.write(menu_string)
            #print "Enter your command: "
        else:
            if not input_queue.empty():
                input_array = (input_queue.get()).split()
                if ( (input_array[0] == 'quit') or (input_array[0] == 'q') or (input_array[0] == 'Quit') or (input_array[0] == 'Q') ):
                    DONE = True
                elif ( (input_array[0] == 'PEW') or (input_array[0] == 'P') ):
                    if (len(input_array) == 2):
                        FEBPwrEnPattern_set(GEMROC_ID, int(input_array[1],0) )
                    else:
                        print '\n bad command'
                        time.sleep(2)
                    os.system('cls')
                    sys.stdout.write(menu_string)
                elif ( (input_array[0] == 'IVT') or (input_array[0] == 'I') ) :
                    if (len(input_array) == 1):
                        GEMROC_IVT_read_and_log(GEMROC_ID, IVT_LOG_ENABLE, IVT_log_file)
                    else:
                        print '\n bad command'
                        time.sleep(2)
                    #os.system('cls')
                    sys.stdout.write(menu_string)
                elif (input_array[0] == 'GRST'):
                    if (len(input_array) == 1):
                        ResetTgtGEMROC_ALL_TIGER_GCfgReg(gemroc_DAQ_XX, GEMROC_ID)
                    else:
                        print '\n bad command'
                        time.sleep(2)
                    os.system('cls')
                    sys.stdout.write(menu_string)
                elif ( (input_array[0] == 'GWdef') or (input_array[0] == 'GW') ):
                    if (len(input_array) == 2):
                        default_filename = "TIGER_def_g_cfg_2018.txt"
                        command_reply = WriteTgtGEMROC_TIGER_GCfgReg_fromfile(g_inst, GEMROC_ID, int(input_array[1]), default_filename, log_file)
                        print '\nGWdef command_reply: %s' %binascii.b2a_hex(command_reply)
                        command_reply = ReadTgtGEMROC_TIGER_GCfgReg( g_inst, GEMROC_ID, int(input_array[1]) )
                        print '\nGRd   command_reply: %s' %binascii.b2a_hex(command_reply)
                        time.sleep(4)
                    else:
                        print('\n bad command')
                        time.sleep(2)
                    os.system('cls')
                    sys.stdout.write(menu_string)                
                elif ((input_array[0] == 'GRd') or (input_array[0] == 'GR')):
                    if (len(input_array) == 2):
                        command_reply = ReadTgtGEMROC_TIGER_GCfgReg( g_inst, GEMROC_ID, int(input_array[1]) )
                        print '\nGRd command_reply: %s' %binascii.b2a_hex(command_reply)
                        ## acr 2018-03-02 display_TIGER_GCFG_Readback(command_reply, 1, log_file)
                        display_log_GCfg_readback(command_reply, 1, log_file) ## acr 2018-03-02 
                        time.sleep(4)
                    else:
                        print('\n bad command')
                        time.sleep(2)
                    os.system('cls')
                    sys.stdout.write(menu_string)                
                elif ( (input_array[0] == 'CWdef') or (input_array[0] == 'CW') ):
                    if (len(input_array) == 3):
                        default_filename = "TIGER_def_ch_cfg_2018.txt"
                        command_reply = WriteTgtGEMROC_TIGER_ChCfgReg_fromfile(c_inst, GEMROC_ID, int(input_array[1]), int(input_array[2]), default_filename)
                        print '\nCWdef command_reply: %s' %binascii.b2a_hex(command_reply)
                        command_reply = ReadTgtGEMROC_TIGER_ChCfgReg (c_inst, GEMROC_ID, int(input_array[1]), int(input_array[2]), 0 ) 
                        print '\nCRd   command_reply: %s' %binascii.b2a_hex(command_reply)
                        time.sleep(4)
                    else:
                        print('\n bad command')
                        time.sleep(2)
                    os.system('cls')
                    sys.stdout.write(menu_string)                
                elif ((input_array[0] == 'CRd') or (input_array[0] == 'CR')):
                    if (len(input_array) == 3):
                        command_reply = ReadTgtGEMROC_TIGER_ChCfgReg (c_inst, GEMROC_ID, int(input_array[1]), int(input_array[2]), 1 ) 
                        print '\nCRd   command_reply: %s' %binascii.b2a_hex(command_reply)
                        ##display_log_ChCfg_readback(command_reply, 1, log_file)
                        time.sleep(4)
                    else:
                        print('\n bad command')
                        time.sleep(2)
                    os.system('cls')
                    sys.stdout.write(menu_string)                
                elif ( (input_array[0] == 'TPEW_ch') or (input_array[0] == 'TP') ):
                    if (len(input_array) == 4):
                        Set_GEMROC_TIGER_ch_TPEn (c_inst, GEMROC_ID, int(input_array[1]), int(input_array[2]), int(input_array[3]) )
                        print '\nTo TIGER %d on GEMROC %d: TP enable bit %d to channel: %d' %(int(input_array[1]), GEMROC_ID, int(input_array[2]), int(input_array[3]) )
                        time.sleep(2)
                    else:
                        print('\n bad command')
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
                    else:
                        print('\n bad command')
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
                    else:
                        print('\n bad command')
                        time.sleep(2)
                    os.system('cls')
                    sys.stdout.write(menu_string)                
                elif (input_array[0] == 'DSTART'):
                    if (len(input_array) == 2):
                        TCAM_Enable_pattern = int(input_array[1],0) & 0xFF
                        DAQ_set_and_TL_start(gemroc_DAQ_XX, GEMROC_ID, TCAM_Enable_pattern)
                        print '\nStart TL DAQ from enable TCAM pattern: %d on GEMROC %d' %( TCAM_Enable_pattern, GEMROC_ID )
                        time.sleep(2)
                    else:
                        print('\n bad command')
                        time.sleep(2)
                    os.system('cls')
                    sys.stdout.write(menu_string)                
                else:
                    input_queue.empty()

#### creating an instance of the GEMROC LV configuration settings object
#### parameter list:
####  TARGET_GEMROC_ID_param = 0, # acr 2017-09-22              
####  command_string_param = 'NONE',
####  number_of_repetitions_param = 1,
##cfg_filename = 'GEMROC_%d_def_cfg_LV_2018.txt' %GEMROC_ID
##gemroc_LV_XX = GEMconf_classes_2018v2.gemroc_cmd_LV_settings(GEMROC_ID,'NONE',1,cfg_filename)
##gemroc_LV_XX.set_FEB_PWR_EN_pattern(FEB_PWR_EN_pattern)
##COMMAND_STRING = 'CMD_GEMROC_LV_CFG_WR'
##gemroc_LV_XX.set_gemroc_cmd_code(COMMAND_STRING,1)
##gemroc_LV_XX.update_command_words()
####gemroc_LV_XX.print_command_words()
####gemroc_LV_XX.extract_parameters_from_UDP_packet()
##array_to_send = gemroc_LV_XX.command_words
##command_echo = send_GEMROC_CFG_CMD_PKT( GEMROC_ID, COMMAND_STRING, array_to_send, DEST_IP_ADDRESS, DEST_PORT_NO )
##print 'CMD_GEMROC_LV_CFG_WR'
##
#### creating an instance of the GEMROC DAQ configuration settings object
#### parameter list:
####  TARGET_GEMROC_ID_param = 0, # acr 2017-09-22              
####  command_string_param = 'NONE',
####  TCAM_ID_param = 1,              
####  number_of_repetitions_param = 1,
####  to_ALL_TCAM_enable_param = 0,
##cfg_filename = 'GEMROC_%d_def_cfg_DAQ_2018.txt' %GEMROC_ID
##gemroc_DAQ_XX = GEMconf_classes_2018v2.gemroc_cmd_DAQ_settings(GEMROC_ID,'NONE',0,1,0,cfg_filename)
##
#### creating an instance of the TIGER global configuration settings object
####    parameter list:
####    TARGET_GEMROC_ID_param = 0, 
####    cfg_filename_param = "default_g_cfg_2018_all_big_endian.txt"
##default_g_inst_settigs_filename = "TIGER_def_g_cfg_2018.txt"
##g_inst = GEMconf_classes_2018v2.g_reg_settings(GEMROC_ID,default_g_inst_settigs_filename)
##
###### creating an instance of the channel configuration settings object
####    parameter list:
####    TARGET_GEMROC_ID_param = 0, 
####    cfg_filename_param = "default_ch_cfg_mainz.txt" 
##default_ch_inst_settigs_filename = "TIGER_def_ch_cfg_2018.txt"
##c_inst = GEMconf_classes_2018v2.ch_reg_settings(GEMROC_ID,default_ch_inst_settigs_filename)

Menu_and_prompt()
print "\nExit debug. Bye!"
exit()
### acr 2018-02-26 END testing the loop


####***** acr 2018-01-15 BEGIN CREATE AN INSTANCE OF GEMROC LV CONFIG/STATUS REGISTER TO ALLOW POWER ON TO THE TIGER ******
##
## creating an instance of the GEMROC configuration settings object
## parameter list:
##  TARGET_GEMROC_ID_param = 0, # acr 2017-09-22              
##  command_string_param = 'NONE',
##  number_of_repetitions_param = 1,
##  cfg_filename_param = 'GEMROC_X_def_cfg_LV_2018.txt'# acr 2018-02-19
cfg_filename = 'GEMROC_%d_def_cfg_LV_2018.txt' %GEMROC_ID
gemroc_LV_XX = GEMconf_classes_2018v2.gemroc_cmd_LV_settings(GEMROC_ID,'NONE',1,cfg_filename)
gemroc_LV_XX.set_FEB_PWR_EN_pattern(FEB_PWR_EN_pattern)
gemroc_LV_XX.set_timing_toFEB_delay(20, 20, 20, 20) # acr 2017-12-07 NOTE this only writes the desidered setting into the GEMROC configuration register. A specific command is needed to
                                                    # actually program the delay chips
COMMAND_STRING = 'CMD_GEMROC_LV_CFG_WR'
gemroc_LV_XX.set_gemroc_cmd_code(COMMAND_STRING,1)
gemroc_LV_XX.update_command_words()
##gemroc_LV_XX.print_command_words()
##gemroc_LV_XX.extract_parameters_from_UDP_packet()
array_to_send = gemroc_LV_XX.command_words
command_echo = send_GEMROC_CFG_CMD_PKT( GEMROC_ID, COMMAND_STRING, array_to_send, DEST_IP_ADDRESS, DEST_PORT_NO )
print 'CMD_GEMROC_LV_CFG_WR'

COMMAND_STRING = 'CMD_GEMROC_LV_CFG_RD'
gemroc_LV_XX.set_gemroc_cmd_code(COMMAND_STRING,1)
gemroc_LV_XX.update_command_words()
array_to_send = gemroc_LV_XX.command_words
command_echo = send_GEMROC_CFG_CMD_PKT( GEMROC_ID, COMMAND_STRING, array_to_send, DEST_IP_ADDRESS, DEST_PORT_NO )
print 'CMD_GEMROC_LV_CFG_RD'


## creating an instance of the GEMROC configuration settings object
## parameter list:
##  TARGET_GEMROC_ID_param = 0, # acr 2017-09-22              
##  command_string_param = 'NONE',
##  TCAM_ID_param = 1,              
##  number_of_repetitions_param = 1,
##  to_ALL_TCAM_enable_param = 0,
##  cfg_filename_param = 'GEMROC_X_def_cfg_DAQ_2018.txt'# acr 2018-02-19
cfg_filename = 'GEMROC_%d_def_cfg_DAQ_2018.txt' %GEMROC_ID
gemroc_DAQ_XX = GEMconf_classes_2018v2.gemroc_cmd_DAQ_settings(GEMROC_ID,'NONE',0,1,0,cfg_filename)
gemroc_DAQ_XX.set_TP_width(3) ## acr 2017-10-03 override the default setting 
gemroc_DAQ_XX.set_EN_TM_TCAM_pattern (0x01)
gemroc_DAQ_XX.set_TP_Pos_nNeg (0x1)
COMMAND_STRING = 'CMD_GEMROC_DAQ_CFG_WR'
gemroc_DAQ_XX.set_gemroc_cmd_code(COMMAND_STRING,1)
gemroc_DAQ_XX.update_command_words()
array_to_send = gemroc_DAQ_XX.command_words
command_echo = send_GEMROC_CFG_CMD_PKT( GEMROC_ID, COMMAND_STRING, array_to_send, DEST_IP_ADDRESS, DEST_PORT_NO )
print 'CMD_GEMROC_DAQ_CFG_WR'

COMMAND_STRING = 'CMD_GEMROC_DAQ_CFG_RD'
gemroc_DAQ_XX.set_gemroc_cmd_code(COMMAND_STRING,1)
gemroc_DAQ_XX.update_command_words()
array_to_send = gemroc_DAQ_XX.command_words
command_echo = send_GEMROC_CFG_CMD_PKT( GEMROC_ID, COMMAND_STRING, array_to_send, DEST_IP_ADDRESS, DEST_PORT_NO )
print 'CMD_GEMROC_DAQ_CFG_RD'

##def GEMROC_IVT_read(GEMROC_ID_param):
##    gemroc_LV_XX.set_target_GEMROC(GEMROC_ID_param)
##    COMMAND_STRING = 'CMD_GEMROC_LV_IVT_READ'
##    gemroc_LV_XX.set_gemroc_cmd_code(COMMAND_STRING,1)
##    gemroc_LV_XX.update_command_words()
##    array_to_send = gemroc_LV_XX.command_words
##    command_echo = send_GEMROC_CFG_CMD_PKT( COMMAND_STRING, array_to_send, DEST_IP_ADDRESS, DEST_PORT_NO )
##    print 'CMD_GEMROC_LV_IVT_READ'
##    display_IVT(command_echo)
##    return

## acr 2018-01-22 BEGIN testing the TIGER ***************************************************************************************************************
COMMAND_STRING = 'CMD_GEMROC_DAQ_TIGER_CFGREG_RESET'
gemroc_DAQ_XX.set_gemroc_cmd_code(COMMAND_STRING,1)
gemroc_DAQ_XX.update_command_words()
array_to_send = gemroc_DAQ_XX.command_words
command_echo = send_GEMROC_CFG_CMD_PKT( GEMROC_ID, COMMAND_STRING, array_to_send, DEST_IP_ADDRESS, DEST_PORT_NO )
time.sleep(1)

# create an instance of the TIGER global configuration settings object
##    parameter list:
##    TARGET_GEMROC_ID_param = 0, 
##    cfg_filename_param = "default_g_cfg_mainz.txt"
# acr 2018-01-26 g_inst = GEMconf_classes_2018v2.g_reg_settings(GEMROC_ID,"default_g_cfg_mainz.txt") 
## acr 2018-02-19 g_inst = GEMconf_classes_2018v2.g_reg_settings(GEMROC_ID,"default_g_cfg_2018_all_big_endian.txt")
default_g_inst_settigs_filename = "TIGER_def_g_cfg_2018.txt"
g_inst = GEMconf_classes_2018v2.g_reg_settings(GEMROC_ID,default_g_inst_settigs_filename)

# acr 2018-02-05 !!! only TIGER 0 targeted for debugging!!! for t in range (0, 2):
for t in range (0, 2): 
    g_inst.set_target_TIGER(t)
    COMMAND_STRING = 'WR'
    g_inst.set_command_code(COMMAND_STRING)
    g_inst.update_command_words()
    ##print '\nTIGER%d Global Cfg Reg CMD %s sent' %(t, COMMAND_STRING)
    ##g_inst.extract_parameters_from_UDP_packet() # acr 2018-01-23
    ##print '\nTIGER%d G_reg_inst WR NIOS words reconstructed from UDP command words: \n' %t
    ##returned_array = classes_test_functions.get_GReg_GEMROC_words( g_inst )
    ##print '\nGCFG_168_137: ' + "%08X" %returned_array[0]
    ##print '\nGCFG_136_105: ' + "%08X" %returned_array[1]
    ##print '\nGCFG_104_73:  ' + "%08X" %returned_array[2]
    ##print '\nGCFG_72_41:   ' + "%08X" %returned_array[3]
    ##print '\nGCFG_40_9:    ' + "%08X" %returned_array[4]
    ##print '\nGCFG_8_m23:   ' + "%08X" %returned_array[5]    
    ##print '\nG_reg_inst WR bitstring_TO_format:'
    ##classes_test_functions.print_GReg_bitstring_TO_format(returned_array)
    array_to_send = g_inst.command_words
    command_echo = send_TIGER_GCFG_Reg_CMD_PKT( t, COMMAND_STRING, array_to_send, DEST_IP_ADDRESS, DEST_PORT_NO )
    time.sleep(1)
    COMMAND_STRING = 'RD'
    g_inst.set_command_code(COMMAND_STRING)
    g_inst.update_command_words()
    print '\nTIGER%d Global Cfg Reg CMD %s sent' %(t, COMMAND_STRING)
    array_to_send = g_inst.command_words
    command_echo = send_TIGER_GCFG_Reg_CMD_PKT( t, COMMAND_STRING, array_to_send, DEST_IP_ADDRESS, DEST_PORT_NO )
    ## ACR 2018-03-02 DEPRECATED display_TIGER_GCFG_Readback(command_reply, 1, log_file)
    time.sleep(1)

#### creating an instance of the channel configuration settings object
##    parameter list:
##    TARGET_GEMROC_ID_param = 0, 
##    cfg_filename_param = "default_g_cfg_mainz.txt" 
# acr 2018-01-26 c_inst = GEMconf_classes_2018v2.ch_reg_settings(GEMROC_ID,"default_ch_cfg_mainz.txt") 
## acr 2018-02-19 c_inst = GEMconf_classes_2018v2.ch_reg_settings(GEMROC_ID,"default_ch_cfg_2018_all_big_endian.txt") 
default_ch_inst_settigs_filename = "TIGER_def_ch_cfg_2018.txt"
c_inst = GEMconf_classes_2018v2.ch_reg_settings(GEMROC_ID,default_ch_inst_settigs_filename)
TriggerMode_default_TP_Disabled = 0
TriggerMode_default_TP_Enabled = 1
Target_Ch_ID = 0
Target_Ch_ToALL = 1
# acr 2018-02-05 !!! only TIGER 0 targeted for debugging!!! for t in range (0, 2):
for t in range (0, 2): # loop over N TIGERs
    c_inst.set_target_TIGER(t)
    c_inst.set_to_ALL_param(1)
    c_inst.TriggerMode = TriggerMode_default_TP_Disabled
    ## sending standard channel configuration to all channels
    ##c_inst.set_command_code('WR')
    ##c_inst.update_command_words()
    COMMAND_STRING = 'WR'
    c_inst.set_command_code(COMMAND_STRING)
    c_inst.update_command_words()
    array_to_send = c_inst.command_words
    Target_Ch_ToALL = (array_to_send[(len(array_to_send)-1)] >> 6) & 0x1
    Target_Ch_ID    = (array_to_send[(len(array_to_send)-1)] >> 0) & 0x3F
    ##
    #### acr 2018-02-06 BEGIN checking CHANNEL CONFIGURATION parameter string
    ##print '\nBEGIN checking CHANNEL CONFIGURATION parameter string'
    ##print '\nTIGER%d ch_reg_inst WR command words: \n' %t
    ##c_inst.print_command_words()
    ##print '\nTIGER%d G_reg_inst: UDP command words from UDP packet: \n' %t
    ##c_inst.extract_parameters_from_UDP_packet() # acr 2018-02-06
    ##print '\nTIGER%d G_reg_inst WR NIOS words reconstructed from UDP command words: \n' %t
    ##returned_array = classes_test_functions.get_Ch_CfgReg_GEMROC_words( c_inst )
    ####for i in range (0, len(returned_array)):
    ####    print "%08X" %returned_array[i]
    ##print '\nCHCFG_124_93: ' + "%08X" %returned_array[0]
    ##print '\nCHCFG_92_61:  ' + "%08X" %returned_array[1]
    ##print '\nCHCFG_60_29:  ' + "%08X" %returned_array[2]
    ##print '\nCHCFG_28_m3:  ' + "%08X" %returned_array[3]
    ##print '\nG_reg_inst WR bitstring_TO_format:'
    ##classes_test_functions.print_Ch_CfgReg_bitstring_TO_format(returned_array)
    ##print '\nEND checking CHANNEL CONFIGURATION parameter string'
    #### acr 2018-02-06 END checking  CHANNEL CONFIGURATION parameter string
    ##
    ##buffer_to_send = struct.pack('>' + str(len(array_to_send)) + 'L', *array_to_send)
    ##clientSock.sendto(buffer_to_send, (DEST_IP_ADDRESS, DEST_PORT_NO))
    ##cmd_message = "\nCMD sent (ch wr TOALL): \n" 
    ##log_file.write(cmd_message)
    ##log_file.write(binascii.b2a_hex(buffer_to_send))
    ##command_echo = receiveSock.recv(BUFSIZE)
    ##cmd_message = "\nCMD echo (ch wr TOALL): \n" 
    ##log_file.write(cmd_message)
    ##log_file.write(binascii.b2a_hex(command_echo))
    cmd_message = '\nTIGER%d; TOALL = %d; Channel%s Cfg Reg CMD %s sent' %(t, Target_Ch_ToALL, Target_Ch_ID, COMMAND_STRING) 
    print cmd_message 
    command_echo = send_TIGER_Ch_CFG_Reg_CMD_PKT( t, COMMAND_STRING, array_to_send, DEST_IP_ADDRESS, DEST_PORT_NO )
    time.sleep(1)

for t in range (0, 2): # loop over N TIGERs
    ## sending special configuration to a specific target channel
    c_inst.set_target_TIGER(t)
    COMMAND_STRING = 'WR'
    c_inst.set_command_code(COMMAND_STRING)
    c_inst.set_to_ALL_param(0)
    c_inst.set_target_channel(3)
    c_inst.TriggerMode = TriggerMode_default_TP_Enabled
    c_inst.update_command_words()
    array_to_send = c_inst.command_words
    ##buffer_to_send = struct.pack('>' + str(len(array_to_send)) + 'L', *array_to_send)
    ##clientSock.sendto(buffer_to_send, (DEST_IP_ADDRESS, DEST_PORT_NO))
    ##cmd_message = "\nCMD sent (ch 3 wr): \n" 
    ##log_file.write(cmd_message)
    ##log_file.write(binascii.b2a_hex(buffer_to_send))
    ##command_echo = receiveSock.recv(BUFSIZE)
    ##cmd_message = "\nCMD echo (ch 3 wr): \n" 
    ##log_file.write(cmd_message)
    ##log_file.write(binascii.b2a_hex(command_echo))
    command_echo = send_TIGER_Ch_CFG_Reg_CMD_PKT( t, COMMAND_STRING, array_to_send, DEST_IP_ADDRESS, DEST_PORT_NO )
    time.sleep(1)

for t in range (0, 2): # loop over N TIGERs
    ## reading configuration from a specific target channel
    c_inst.set_target_TIGER(t)
    c_inst.set_to_ALL_param(0)
    COMMAND_STRING = 'RD'
    c_inst.set_command_code(COMMAND_STRING)
    for i in range (0, 64): # loop over all channels
        c_inst.set_target_channel(i)
        c_inst.update_command_words()
        array_to_send = c_inst.command_words
        ##buffer_to_send = struct.pack('>' + str(len(array_to_send)) + 'L', *array_to_send)
        ##clientSock.sendto(buffer_to_send, (DEST_IP_ADDRESS, DEST_PORT_NO))
        ##cmd_message = "\nCMD sent(TIGER %d ch3 rd): \n" %t
        ##log_file.write(cmd_message)
        ##log_file.write(binascii.b2a_hex(buffer_to_send))
        ##command_echo = receiveSock.recv(BUFSIZE)
        ##cmd_message = "\nCMD echo(TIGER %d ch3 rd): \n" %t
        ##log_file.write(cmd_message)
        ##log_file.write(binascii.b2a_hex(command_echo))
        command_echo = send_TIGER_Ch_CFG_Reg_CMD_PKT( t, COMMAND_STRING, array_to_send, DEST_IP_ADDRESS, DEST_PORT_NO )
        display_TIGER_ChCFG_Readback(command_echo, 1, log_file)
        ##time.sleep(1)

## acr 2018-01-23 sending a 'TIGER_SYNCH_RST' command to the TARGET GEMROC board
gemroc_DAQ_XX.set_gemroc_cmd_code('CMD_GEMROC_DAQ_TIGER_SYNCH_RST',1)
gemroc_DAQ_XX.update_command_words()
array_to_send = gemroc_DAQ_XX.command_words
buffer_to_send = struct.pack('>' + str(len(array_to_send)) + 'L', *array_to_send)
clientSock.sendto(buffer_to_send, (DEST_IP_ADDRESS, DEST_PORT_NO))
cmd_message = "\nCMD sent 'CMD_GEMROC_DAQ_TIGER_SYNCH_RST': \n" 
log_file.write(cmd_message)
log_file.write(binascii.b2a_hex(buffer_to_send))
command_echo = receiveSock.recv(BUFSIZE)
cmd_message = "\nCMD echo 'CMD_GEMROC_DAQ_TIGER_SYNCH_RST': \n" 
log_file.write(cmd_message)
log_file.write(binascii.b2a_hex(command_echo))
time.sleep(1)

## acr 2018-01-23 sending a 'TCAM_SYNCH_RST' command to the TARGET GEMROC board
gemroc_DAQ_XX.set_gemroc_cmd_code('CMD_GEMROC_DAQ_TCAM_SYNCH_RST',1)
gemroc_DAQ_XX.update_command_words()
array_to_send = gemroc_DAQ_XX.command_words
buffer_to_send = struct.pack('>' + str(len(array_to_send)) + 'L', *array_to_send)
clientSock.sendto(buffer_to_send, (DEST_IP_ADDRESS, DEST_PORT_NO))
cmd_message = "\nCMD sent 'CMD_GEMROC_DAQ_TCAM_SYNCH_RST': \n" 
log_file.write(cmd_message)
log_file.write(binascii.b2a_hex(buffer_to_send))
command_echo = receiveSock.recv(BUFSIZE)
cmd_message = "\nCMD echo 'CMD_GEMROC_DAQ_TCAM_SYNCH_RST': \n" 
log_file.write(cmd_message)
log_file.write(binascii.b2a_hex(command_echo))
time.sleep(1)

## acr 2017-10-03 set_AUTO_TP_EN_pattern and ENABLE triggerless data acquisition
gemroc_DAQ_XX.set_TP_width(5) 
gemroc_DAQ_XX.set_AUTO_TP_EN_pattern(0x0)
gemroc_DAQ_XX.set_Periodic_TP_EN_pattern(0xF)
gemroc_DAQ_XX.set_TL_nTM_ACQ(1)
gemroc_DAQ_XX.set_TP_Pos_nNeg(1)
gemroc_DAQ_XX.set_TP_period(8)
gemroc_DAQ_XX.set_gemroc_cmd_code('CMD_GEMROC_DAQ_CFG_WR',1)
gemroc_DAQ_XX.update_command_words()
#gemroc_DAQ_XX.print_command_words()
gemroc_DAQ_XX.extract_parameters_from_UDP_packet()
array_to_send = gemroc_DAQ_XX.command_words
buffer_to_send = struct.pack('>' + str(len(array_to_send)) + 'L', *array_to_send)
clientSock.sendto(buffer_to_send, (DEST_IP_ADDRESS, DEST_PORT_NO))
cmd_message = "\nCMD sent (CMD_GEMROC_DAQ_CFG_WR): \n" 
log_file.write(cmd_message)
log_file.write(binascii.b2a_hex(buffer_to_send))
command_echo = receiveSock.recv(BUFSIZE)
cmd_message = "\nCMD echo (CMD_GEMROC_DAQ_CFG_WR): \n" 
log_file.write(cmd_message)
log_file.write(binascii.b2a_hex(command_echo))

##print ("\nwaiting 10s")
##time.sleep(20)

Menu_and_prompt()
print "\nExit debug. Bye!"
exit()

## acr 2018-02-21 END  write the user interface

log_file.close()
IVT_log_file.close()
