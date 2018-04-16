# file: TIGER_cfg_test.py
# author: Angelo Cotta Ramusino INFN Ferrara
# date: 27 June 2017
# purpose: to debug TIGER configuration and data acquisition through the GEMROC prototype
# last modifications / additions
# acr 2017-07-11 modified "GEMconf_classes" to read single TIGER global / channel configuration from files
import socket
import string
import time
import GEMconf_classes
import struct
import binascii

# BEGIN uncomment for connection to the GEMROC module
HOST_IP = "127.0.0.1"
# END   uncomment for connection to the GEMROC module

# BEGIN uncomment for test only
#HOST_IP = "127.0.0.1"
# END   uncomment for test only

HOST_PORT = 54831
HOST_PORT_RECEIVE = 58927

# BEGIN uncomment for connection to the GEMROC module
DEST_IP_ADDRESS = "127.0.0.1"
DEST_PORT_NO = 58913
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

log_fname = 'tiger%d_cfg_log.txt'% tiger_index
log_file = open(log_fname, 'w')



####***** acr 2017-10-30 BEGIN CREATE AN INSTANCE OF GEMROC CONFIG/STATUS REGISTER TO ALLOW POWER ON TO THE TIGER ******
##
## creating an instance of the GEMROC configuration settings object
## parameter list:
##  TARGET_GEMROC_ID_param = 0, # acr 2017-09-22              
##  command_string_param = 'NONE',
##  TCAM_ID_param = 1,              
##  number_of_repetitions_param = 1,
##  to_ALL_TCAM_enable_param = 0
gemroc_inst_0 = GEMconf_classes.gemroc_cmd_settings(2,'NONE',0,1,0)
## acr 2017-10-02 writing default configuration (with ome overrides) to the TARGET GEMROC board
gemroc_inst_0.set_TP_width(3) ## acr 2017-10-03 override the default setting 
gemroc_inst_0.set_FEB_PWR_EN_pattern(0x1)
gemroc_inst_0.set_EN_TM_TCAM_pattern (0x3)
gemroc_inst_0.set_TP_Pos_nNeg (0x1)
gemroc_inst_0.set_timing_toFEB_delay(0, 0, 0, 0) # acr 2017-12-07
gemroc_inst_0.set_gemroc_cmd_code('WR',1)
gemroc_inst_0.update_command_words()
gemroc_inst_0.print_command_words()
array_to_send = gemroc_inst_0.command_words
buffer_to_send = struct.pack('>' + str(len(array_to_send)) + 'L', *array_to_send)
## '>': little-endian ,str:change to string,'L':unsigned long.
clientSock.sendto(buffer_to_send, (DEST_IP_ADDRESS, DEST_PORT_NO))
cmd_message = "\nCMD sent (GEMROC WR): \n" 
log_file.write(cmd_message)
log_file.write(binascii.b2a_hex(buffer_to_send))
command_echo = receiveSock.recv(BUFSIZE)
cmd_message = "\nCMD echo (GEMROC WR): \n" 
log_file.write(cmd_message)
log_file.write(binascii.b2a_hex(command_echo))
##

#### acr 2017-10-02 sending a 'TIGER_SYNCH_RST' command to the TARGET GEMROC board
##for i in range (0,1):
##    gemroc_inst_0.set_timing_toFEB_delay(i, i, i, i) # acr 2017-12-07
##    gemroc_inst_0.set_gemroc_cmd_code('WR',1)
##    gemroc_inst_0.update_command_words()
##    gemroc_inst_0.print_command_words()
##    array_to_send = gemroc_inst_0.command_words
##    buffer_to_send = struct.pack('>' + str(len(array_to_send)) + 'L', *array_to_send)
##    clientSock.sendto(buffer_to_send, (DEST_IP_ADDRESS, DEST_PORT_NO))
##:wq

##    gemroc_inst_0.set_gemroc_cmd_code('FEB_TIMING_DELAYS_UPDATE',1)
##    gemroc_inst_0.update_command_words()
##    array_to_send = gemroc_inst_0.command_words
##    buffer_to_send = struct.pack('>' + str(len(array_to_send)) + 'L', *array_to_send)
##    clientSock.sendto(buffer_to_send, (DEST_IP_ADDRESS, DEST_PORT_NO))
##    cmd_message = "\nCMD sent 'FEB_TIMING_DELAYS_UPDATE': \n" 
##    log_file.write(cmd_message)
##    log_file.write(binascii.b2a_hex(buffer_to_send))
##    command_echo = receiveSock.recv(BUFSIZE)
##    cmd_message = "\nCMD echo 'FEB_TIMING_DELAYS_UPDATE': \n" 
##    log_file.write(cmd_message)
##    log_file.write(binascii.b2a_hex(command_echo))
##    time.sleep(1)
##
##for i in range (0,1):
##    gemroc_inst_0.set_gemroc_cmd_code('IVT_READ',1)
##    gemroc_inst_0.update_command_words()
##    array_to_send = gemroc_inst_0.command_words
##    buffer_to_send = struct.pack('>' + str(len(array_to_send)) + 'L', *array_to_send)
##    clientSock.sendto(buffer_to_send, (DEST_IP_ADDRESS, DEST_PORT_NO))
##    cmd_message = "\nCMD sent 'IVT_READ': \n" 
##    log_file.write(cmd_message)
##    log_file.write(binascii.b2a_hex(buffer_to_send))
##    command_echo = receiveSock.recv(BUFSIZE)
##    cmd_message = "\nCMD echo 'IVT_READ': \n" 
##    log_file.write(cmd_message)
##    log_file.write(binascii.b2a_hex(command_echo))

#### acr 2017-10-02 reading back configuration to the TARGET GEMROC board
##gemroc_inst_0.set_gemroc_cmd_code('RD',1)
##gemroc_inst_0.update_command_words()
##gemroc_inst_0.print_command_words()
##array_to_send = gemroc_inst_0.command_words
##buffer_to_send = struct.pack('>' + str(len(array_to_send)) + 'L', *array_to_send)
##clientSock.sendto(buffer_to_send, (DEST_IP_ADDRESS, DEST_PORT_NO))
##cmd_message = "\nCMD sent (GEMROC RD): \n" 
##log_file.write(cmd_message)
##log_file.write(binascii.b2a_hex(buffer_to_send))
##command_echo = receiveSock.recv(BUFSIZE)
##cmd_message = "\nCMD echo (GEMROC RD): \n" 
##log_file.write(cmd_message)
##log_file.write(binascii.b2a_hex(command_echo))
####
time.sleep(1)
##
####***** acr 2017-10-30 END CREATE AN INSTANCE OF GEMROC CONFIG/STATUS REGISTER TO ALLOW POWER ON TO THE TIGER ******

## acr 2017-11-09 BEGIN testing the TIGER 
for i in range (0, 1):
    gemroc_inst_0.set_gemroc_cmd_code('TIGER_SYNCH_RST_WITH_SPI',1)
    gemroc_inst_0.update_command_words()
    array_to_send = gemroc_inst_0.command_words
    buffer_to_send = struct.pack('>' + str(len(array_to_send)) + 'L', *array_to_send)
    clientSock.sendto(buffer_to_send, (DEST_IP_ADDRESS, DEST_PORT_NO))
    cmd_message = "\nCMD sent 'TIGER_SYNCH_RST_WITH_SPI': \n" 
    log_file.write(cmd_message)
    log_file.write(binascii.b2a_hex(buffer_to_send))
    command_echo = receiveSock.recv(BUFSIZE)
    cmd_message = "\nCMD echo 'TIGER_SYNCH_RST_WITH_SPI': \n" 
    log_file.write(cmd_message)
    log_file.write(binascii.b2a_hex(command_echo))
    time.sleep(1)
    


## acr 2017-10-30 creating an instance of the global configuration settings object; prepare it to turn ON the power
g_inst = GEMconf_classes.g_reg_settings() ## acr 2017-07-11 reading settings from "default_g_cfg.txt"

#### sending global configuration 
##g_inst.set_target_GEMROC(2)
##g_inst.set_target_TIGER(0)
##g_inst.set_command_code('WR')
##g_inst.update_command_words()
##array_to_send = g_inst.command_words
####for i in range (0, 11):
####    print '%08X'% g_inst.command_words[i]
####print "\n g_inst.print_command_words()\n"
####g_inst.print_command_words()
##
##buffer_to_send = struct.pack('>' + str(len(array_to_send)) + 'L', *array_to_send)
##for i in range (0, 1):
##    clientSock.sendto(buffer_to_send, (DEST_IP_ADDRESS, DEST_PORT_NO))
##    cmd_message = "\nCMD sent (Global REG WR): \n" 
##    log_file.write(cmd_message)
##    log_file.write(binascii.b2a_hex(buffer_to_send))
##    command_echo = receiveSock.recv(BUFSIZE)
##    cmd_message = "\nCMD echo (Global REG WR): \n" 
##    log_file.write(cmd_message)
##    log_file.write(binascii.b2a_hex(command_echo))
##    time.sleep(1)
##    
####time.sleep(3)
##
#### acr BEGIN 2017-08-30
#### READING BACK global configuration 
##g_inst.set_target_GEMROC(2)
##g_inst.set_target_TIGER(0)
####g_inst.update_command_words()
##g_inst.set_command_code('RD')
##g_inst.update_command_words()
##array_to_send = g_inst.command_words
###
##buffer_to_send = struct.pack('>' + str(len(array_to_send)) + 'L', *array_to_send)
##for i in range (0, 1):
##    clientSock.sendto(buffer_to_send, (DEST_IP_ADDRESS, DEST_PORT_NO))
##    cmd_message = "\nCMD sent (Global REG RD): \n" 
##    log_file.write(cmd_message)
##    log_file.write(binascii.b2a_hex(buffer_to_send))
##    command_echo = receiveSock.recv(BUFSIZE)
##    cmd_message = "\nCMD echo (Global REG RD): \n" 
##    log_file.write(cmd_message)
##    log_file.write(binascii.b2a_hex(command_echo))
##    time.sleep(1)
#### acr END 2017-08-30



###**** acr 2017-11-09 BEGIN sending/reading loop
## sending global configuration 
g_inst.set_target_GEMROC(2)
for i in range (0, 1):
    g_inst.set_target_TIGER(0)
    g_inst.set_command_code('WR')
    g_inst.update_command_words()
    array_to_send = g_inst.command_words
    buffer_to_send = struct.pack('>' + str(len(array_to_send)) + 'L', *array_to_send)
    clientSock.sendto(buffer_to_send, (DEST_IP_ADDRESS, DEST_PORT_NO))
    cmd_message = "\nCMD sent (Global REG WR): \n" 
    log_file.write(cmd_message)
    log_file.write(binascii.b2a_hex(buffer_to_send))
    command_echo = receiveSock.recv(BUFSIZE)
    cmd_message = "\nCMD echo (Global REG WR): \n" 
    log_file.write(cmd_message)
    log_file.write(binascii.b2a_hex(command_echo))
    time.sleep(1)
    g_inst.set_command_code('RD')
    g_inst.update_command_words()
    array_to_send = g_inst.command_words
    buffer_to_send = struct.pack('>' + str(len(array_to_send)) + 'L', *array_to_send)
    clientSock.sendto(buffer_to_send, (DEST_IP_ADDRESS, DEST_PORT_NO))
    cmd_message = "\nCMD sent (Global REG RD): \n" 
    log_file.write(cmd_message)
    log_file.write(binascii.b2a_hex(buffer_to_send))
    command_echo = receiveSock.recv(BUFSIZE)
    cmd_message = "\nCMD echo (Global REG RD): \n" 
    log_file.write(cmd_message)
    log_file.write(binascii.b2a_hex(command_echo))
    time.sleep(1)
    g_inst.set_target_TIGER(1)
    g_inst.set_command_code('WR')
    g_inst.update_command_words()
    array_to_send = g_inst.command_words
    buffer_to_send = struct.pack('>' + str(len(array_to_send)) + 'L', *array_to_send)
    clientSock.sendto(buffer_to_send, (DEST_IP_ADDRESS, DEST_PORT_NO))
    cmd_message = "\nCMD sent (Global REG WR): \n" 
    log_file.write(cmd_message)
    log_file.write(binascii.b2a_hex(buffer_to_send))
    command_echo = receiveSock.recv(BUFSIZE)
    cmd_message = "\nCMD echo (Global REG WR): \n" 
    log_file.write(cmd_message)
    log_file.write(binascii.b2a_hex(command_echo))
    time.sleep(1)
    g_inst.set_command_code('RD')
    g_inst.update_command_words()
    array_to_send = g_inst.command_words
    buffer_to_send = struct.pack('>' + str(len(array_to_send)) + 'L', *array_to_send)
    clientSock.sendto(buffer_to_send, (DEST_IP_ADDRESS, DEST_PORT_NO))
    cmd_message = "\nCMD sent (Global REG RD): \n" 
    log_file.write(cmd_message)
    log_file.write(binascii.b2a_hex(buffer_to_send))
    command_echo = receiveSock.recv(BUFSIZE)
    cmd_message = "\nCMD echo (Global REG RD): \n" 
    log_file.write(cmd_message)
    log_file.write(binascii.b2a_hex(command_echo))
##    gemroc_inst_0.set_gemroc_cmd_code('TIGER_SYNCH_RST',1)
##    gemroc_inst_0.update_command_words()
##    array_to_send = gemroc_inst_0.command_words
##    buffer_to_send = struct.pack('>' + str(len(array_to_send)) + 'L', *array_to_send)
##    clientSock.sendto(buffer_to_send, (DEST_IP_ADDRESS, DEST_PORT_NO))
##    cmd_message = "\nCMD sent 'TIGER_SYNCH_RST': \n" 
##    log_file.write(cmd_message)
##    log_file.write(binascii.b2a_hex(buffer_to_send))
##    command_echo = receiveSock.recv(BUFSIZE)
##    cmd_message = "\nCMD echo 'TIGER_SYNCH_RST': \n" 
##    log_file.write(cmd_message)
##    log_file.write(binascii.b2a_hex(command_echo))
    time.sleep(1)
## acr END 2017-08-30

###**** END sending/reading loop


    
##time.sleep(3)
##
##
##
##
##
#### creating an instance of the channel configuration settings object
c_inst = GEMconf_classes.ch_reg_settings() ## acr 2017-07-11 reading settings from "default_ch_cfg.txt"
##
#### TIGER 0
## sending standard channel configuration to all channels
c_inst.set_target_GEMROC(2)
c_inst.set_target_TIGER(0)
c_inst.set_command_code('WR')
c_inst.set_to_ALL_param(1)
TriggerMode_default_TP_Disabled = 0
c_inst.TriggerMode = TriggerMode_default_TP_Disabled
c_inst.update_command_words()
c_inst.print_command_words()
array_to_send = c_inst.command_words
buffer_to_send = struct.pack('>' + str(len(array_to_send)) + 'L', *array_to_send)
clientSock.sendto(buffer_to_send, (DEST_IP_ADDRESS, DEST_PORT_NO))
cmd_message = "\nCMD sent (ch wr TOALL): \n" 
log_file.write(cmd_message)
log_file.write(binascii.b2a_hex(buffer_to_send))
command_echo = receiveSock.recv(BUFSIZE)
cmd_message = "\nCMD echo (ch wr TOALL): \n" 
log_file.write(cmd_message)
log_file.write(binascii.b2a_hex(command_echo))

time.sleep(1)

## sending special configuration to a specific target channel
c_inst.set_target_GEMROC(2)
c_inst.set_target_TIGER(0)
c_inst.set_command_code('WR')
c_inst.set_to_ALL_param(0)
c_inst.set_target_channel(3)
TriggerMode_default_TP_Enabled = 1
c_inst.TriggerMode = TriggerMode_default_TP_Enabled
c_inst.update_command_words()
c_inst.print_command_words()
array_to_send = c_inst.command_words
buffer_to_send = struct.pack('>' + str(len(array_to_send)) + 'L', *array_to_send)
clientSock.sendto(buffer_to_send, (DEST_IP_ADDRESS, DEST_PORT_NO))
cmd_message = "\nCMD sent (ch 3 wr): \n" 
log_file.write(cmd_message)
log_file.write(binascii.b2a_hex(buffer_to_send))
command_echo = receiveSock.recv(BUFSIZE)
cmd_message = "\nCMD echo (ch 3 wr): \n" 
log_file.write(cmd_message)
log_file.write(binascii.b2a_hex(command_echo))


#### TIGER 1
## sending standard channel configuration to all channels
c_inst.set_target_GEMROC(2)
c_inst.set_target_TIGER(1)
c_inst.set_command_code('WR')
c_inst.set_to_ALL_param(1)
TriggerMode_default_TP_Disabled = 0
c_inst.TriggerMode = TriggerMode_default_TP_Disabled
c_inst.update_command_words()
c_inst.print_command_words()
array_to_send = c_inst.command_words
buffer_to_send = struct.pack('>' + str(len(array_to_send)) + 'L', *array_to_send)
clientSock.sendto(buffer_to_send, (DEST_IP_ADDRESS, DEST_PORT_NO))
cmd_message = "\nCMD sent (ch wr TOALL): \n" 
log_file.write(cmd_message)
log_file.write(binascii.b2a_hex(buffer_to_send))
command_echo = receiveSock.recv(BUFSIZE)
cmd_message = "\nCMD echo (ch wr TOALL): \n" 
log_file.write(cmd_message)
log_file.write(binascii.b2a_hex(command_echo))

time.sleep(1)

## sending special configuration to a specific target channel
c_inst.set_target_GEMROC(2)
c_inst.set_target_TIGER(1)
c_inst.set_command_code('WR')
c_inst.set_to_ALL_param(0)
c_inst.set_target_channel(3)
TriggerMode_default_TP_Enabled = 1
c_inst.TriggerMode = TriggerMode_default_TP_Enabled
c_inst.update_command_words()
c_inst.print_command_words()
array_to_send = c_inst.command_words
buffer_to_send = struct.pack('>' + str(len(array_to_send)) + 'L', *array_to_send)
clientSock.sendto(buffer_to_send, (DEST_IP_ADDRESS, DEST_PORT_NO))
cmd_message = "\nCMD sent (ch 3 wr): \n" 
log_file.write(cmd_message)
log_file.write(binascii.b2a_hex(buffer_to_send))
command_echo = receiveSock.recv(BUFSIZE)
cmd_message = "\nCMD echo (ch 3 wr): \n" 
log_file.write(cmd_message)
log_file.write(binascii.b2a_hex(command_echo))


#### reading configuration from a specific target channel (ch 0)
##c_inst.set_target_GEMROC(2)
##c_inst.set_target_TIGER(0)
##c_inst.set_command_code('RD')
##c_inst.set_to_ALL_param(0)
##c_inst.set_target_channel(0)
##TriggerMode_default_TP_Enabled = 1
##c_inst.TriggerMode = TriggerMode_default_TP_Enabled
##c_inst.update_command_words()
##c_inst.print_command_words()
##array_to_send = c_inst.command_words
##buffer_to_send = struct.pack('>' + str(len(array_to_send)) + 'L', *array_to_send)
##clientSock.sendto(buffer_to_send, (DEST_IP_ADDRESS, DEST_PORT_NO))
##cmd_message = "\nCMD sent(ch0 rd): \n" 
##log_file.write(cmd_message)
##log_file.write(binascii.b2a_hex(buffer_to_send))
##command_echo = receiveSock.recv(BUFSIZE)
##cmd_message = "\nCMD echo(ch0 rd): \n" 
##log_file.write(cmd_message)
##log_file.write(binascii.b2a_hex(command_echo))
####
## reading configuration from a specific target channel
for i in range (0, 1):
    c_inst.set_target_GEMROC(2)
    c_inst.set_target_TIGER(0)
    c_inst.set_command_code('RD')
    c_inst.set_to_ALL_param(0)
    c_inst.set_target_channel(3)
    TriggerMode_default_TP_Enabled = 1
    c_inst.TriggerMode = TriggerMode_default_TP_Enabled
    c_inst.update_command_words()
    c_inst.print_command_words()
    array_to_send = c_inst.command_words
    buffer_to_send = struct.pack('>' + str(len(array_to_send)) + 'L', *array_to_send)
    clientSock.sendto(buffer_to_send, (DEST_IP_ADDRESS, DEST_PORT_NO))
    cmd_message = "\nCMD sent(T0 ch3 rd): \n" 
    log_file.write(cmd_message)
    log_file.write(binascii.b2a_hex(buffer_to_send))
    command_echo = receiveSock.recv(BUFSIZE)
    cmd_message = "\nCMD echo(T0 ch3 rd): \n" 
    log_file.write(cmd_message)
    log_file.write(binascii.b2a_hex(command_echo))
for i in range (0, 1):
    c_inst.set_target_GEMROC(2)
    c_inst.set_target_TIGER(1)
    c_inst.set_command_code('RD')
    c_inst.set_to_ALL_param(0)
    c_inst.set_target_channel(3)
    TriggerMode_default_TP_Enabled = 1
    c_inst.TriggerMode = TriggerMode_default_TP_Enabled
    c_inst.update_command_words()
    c_inst.print_command_words()
    array_to_send = c_inst.command_words
    buffer_to_send = struct.pack('>' + str(len(array_to_send)) + 'L', *array_to_send)
    clientSock.sendto(buffer_to_send, (DEST_IP_ADDRESS, DEST_PORT_NO))
    cmd_message = "\nCMD sent(T1 ch3 rd): \n" 
    log_file.write(cmd_message)
    log_file.write(binascii.b2a_hex(buffer_to_send))
    command_echo = receiveSock.recv(BUFSIZE)
    cmd_message = "\nCMD echo(T1 ch3 rd): \n" 
    log_file.write(cmd_message)
    log_file.write(binascii.b2a_hex(command_echo))
##
##time.sleep(3)
##
##
##
##
##
##
## creating an instance of the GEMROC configuration settings object
## parameter list:
##  TARGET_GEMROC_ID_param = 0, # acr 2017-09-22              
##  command_string_param = 'NONE',
##  TCAM_ID_param = 1,              
##  number_of_repetitions_param = 1,
##  to_ALL_TCAM_enable_param = 0
####***** acr 2017-10-30 BEGIN COMMENT OUT; it is already done *****
## gemroc_inst_0 = GEMconf_classes.gemroc_cmd_settings(2,'NONE',0,1,0)
####***** acr 2017-10-30 END COMMENT OUT; it is already done *****## acr 2017-10-02 writing default configuration (with ome overrides) to the TARGET GEMROC board
##

    
## acr 2017-11-16 commented in Maincz to save time
##gemroc_inst_0.set_TP_width(3) ## acr 2017-10-03 override the default setting 
#### gemroc_inst_0.set_AUTO_TP_EN_pattern(0xF) ## acr 2017-10-03 override the default setting 
##gemroc_inst_0.set_gemroc_cmd_code('WR',1)
##gemroc_inst_0.update_command_words()
##gemroc_inst_0.print_command_words()
##array_to_send = gemroc_inst_0.command_words
##buffer_to_send = struct.pack('>' + str(len(array_to_send)) + 'L', *array_to_send)
##clientSock.sendto(buffer_to_send, (DEST_IP_ADDRESS, DEST_PORT_NO))
##cmd_message = "\nCMD sent (GEMROC WR): \n" 
##log_file.write(cmd_message)
##log_file.write(binascii.b2a_hex(buffer_to_send))
##command_echo = receiveSock.recv(BUFSIZE)
##cmd_message = "\nCMD echo (GEMROC WR): \n" 
##log_file.write(cmd_message)
##log_file.write(binascii.b2a_hex(command_echo))
##
#### acr 2017-10-02 reading back configuration to the TARGET GEMROC board
##gemroc_inst_0.set_gemroc_cmd_code('RD',1)
##gemroc_inst_0.update_command_words()
##gemroc_inst_0.print_command_words()
##array_to_send = gemroc_inst_0.command_words
##buffer_to_send = struct.pack('>' + str(len(array_to_send)) + 'L', *array_to_send)
##clientSock.sendto(buffer_to_send, (DEST_IP_ADDRESS, DEST_PORT_NO))
##cmd_message = "\nCMD sent (GEMROC RD): \n" 
##log_file.write(cmd_message)
##log_file.write(binascii.b2a_hex(buffer_to_send))
##command_echo = receiveSock.recv(BUFSIZE)
##cmd_message = "\nCMD echo (GEMROC RD): \n" 
##log_file.write(cmd_message)
##log_file.write(binascii.b2a_hex(command_echo))


## acr 2017-10-02 sending a 'TIGER_SYNCH_RST' command to the TARGET GEMROC board
gemroc_inst_0.set_gemroc_cmd_code('TIGER_SYNCH_RST',1)
gemroc_inst_0.update_command_words()
gemroc_inst_0.print_command_words()
array_to_send = gemroc_inst_0.command_words
buffer_to_send = struct.pack('>' + str(len(array_to_send)) + 'L', *array_to_send)
clientSock.sendto(buffer_to_send, (DEST_IP_ADDRESS, DEST_PORT_NO))
cmd_message = "\nCMD sent 'TIGER_SYNCH_RST': \n" 
log_file.write(cmd_message)
log_file.write(binascii.b2a_hex(buffer_to_send))
command_echo = receiveSock.recv(BUFSIZE)
cmd_message = "\nCMD echo 'TIGER_SYNCH_RST': \n" 
log_file.write(cmd_message)
log_file.write(binascii.b2a_hex(command_echo))

## acr 2017-10-02 sending a 'TCAM_SYNCH_RST' command to the TARGET GEMROC board
gemroc_inst_0.set_gemroc_cmd_code('TCAM_SYNCH_RST',1)
gemroc_inst_0.update_command_words()
gemroc_inst_0.print_command_words()
array_to_send = gemroc_inst_0.command_words
buffer_to_send = struct.pack('>' + str(len(array_to_send)) + 'L', *array_to_send)
clientSock.sendto(buffer_to_send, (DEST_IP_ADDRESS, DEST_PORT_NO))
cmd_message = "\nCMD sent 'TCAM_SYNCH_RST': \n" 
log_file.write(cmd_message)
log_file.write(binascii.b2a_hex(buffer_to_send))
command_echo = receiveSock.recv(BUFSIZE)
cmd_message = "\nCMD echo 'TCAM_SYNCH_RST': \n" 
log_file.write(cmd_message)
log_file.write(binascii.b2a_hex(command_echo))

## acr 2017-10-03 set_AUTO_TP_EN_pattern and ENABLE triggerless data acquisition
gemroc_inst_0.set_TP_width(5) ## acr 2017-10-03 override the default setting 
gemroc_inst_0.set_AUTO_TP_EN_pattern(0x0) ## acr 2017-10-03 override the default setting
gemroc_inst_0.set_Periodic_TP_EN_pattern(0xF) ## acr 2017-10-03 override the default setting
gemroc_inst_0.set_TL_nTM_ACQ(1)
# acr 2017-11-15 gemroc_inst_0.set_TP_Pos_nNeg(0) ## acr 2017-10-04
gemroc_inst_0.set_TP_Pos_nNeg(1) ## acr 2017-10-04
gemroc_inst_0.set_TP_period(8) ## acr 2017-10-04
##gemroc_inst_0.set_gemroc_cmd_code('WR',1)
gemroc_inst_0.set_gemroc_cmd_code('WR',1)
gemroc_inst_0.update_command_words()
gemroc_inst_0.print_command_words()
array_to_send = gemroc_inst_0.command_words
buffer_to_send = struct.pack('>' + str(len(array_to_send)) + 'L', *array_to_send)
clientSock.sendto(buffer_to_send, (DEST_IP_ADDRESS, DEST_PORT_NO))
cmd_message = "\nCMD sent (GEMROC set_AUTO_TP_EN_pattern): \n" 
log_file.write(cmd_message)
log_file.write(binascii.b2a_hex(buffer_to_send))
command_echo = receiveSock.recv(BUFSIZE)
cmd_message = "\nCMD echo (GEMROC set_AUTO_TP_EN_pattern): \n" 
log_file.write(cmd_message)
log_file.write(binascii.b2a_hex(command_echo))

##time.sleep(3)
##
#### acr 2017-10-02 writing default configuration (with ome overrides) to the TARGET GEMROC board
##gemroc_inst_0.set_TP_width(5) ## acr 2017-10-03 override the default setting 
##gemroc_inst_0.set_AUTO_TP_EN_pattern(0x0) ## acr 2017-10-03 override the default setting 
##gemroc_inst_0.set_TL_nTM_ACQ(0)
##gemroc_inst_0.set_gemroc_cmd_code('WR',1)
##gemroc_inst_0.update_command_words()
##gemroc_inst_0.print_command_words()
##array_to_send = gemroc_inst_0.command_words
##buffer_to_send = struct.pack('>' + str(len(array_to_send)) + 'L', *array_to_send)
##clientSock.sendto(buffer_to_send, (DEST_IP_ADDRESS, DEST_PORT_NO))
##cmd_message = "\nCMD sent (GEMROC WR): \n" 
##log_file.write(cmd_message)
##log_file.write(binascii.b2a_hex(buffer_to_send))
##command_echo = receiveSock.recv(BUFSIZE)
##cmd_message = "\nCMD echo (GEMROC WR): \n" 
##log_file.write(cmd_message)
##log_file.write(binascii.b2a_hex(command_echo))
##
##time.sleep (3)



## acr 2017-11-16 commented in Mainz to save time
##gemroc_inst_0.set_FEB_PWR_EN_pattern(0xF)
##gemroc_inst_0.set_gemroc_cmd_code('WR',1)
##gemroc_inst_0.update_command_words()
###gemroc_inst_0.print_command_words()
##array_to_send = gemroc_inst_0.command_words
##buffer_to_send = struct.pack('>' + str(len(array_to_send)) + 'L', *array_to_send)
##clientSock.sendto(buffer_to_send, (DEST_IP_ADDRESS, DEST_PORT_NO))
##cmd_message = "\nCMD sent (GEMROC WR): \n" 
##log_file.write(cmd_message)
##log_file.write(binascii.b2a_hex(buffer_to_send))
##command_echo = receiveSock.recv(BUFSIZE)
##cmd_message = "\nCMD echo (GEMROC WR): \n" 
##log_file.write(cmd_message)
##log_file.write(binascii.b2a_hex(command_echo))
##
##time.sleep (3)
##
##gemroc_inst_0.set_FEB_PWR_EN_pattern(0xA)
##gemroc_inst_0.set_gemroc_cmd_code('WR',1)
##gemroc_inst_0.update_command_words()
###gemroc_inst_0.print_command_words()
##array_to_send = gemroc_inst_0.command_words
##buffer_to_send = struct.pack('>' + str(len(array_to_send)) + 'L', *array_to_send)
##
##for i in range (0, 3):
##    clientSock.sendto(buffer_to_send, (DEST_IP_ADDRESS, DEST_PORT_NO))
##    cmd_message = "\nCMD sent (GEMROC WR): \n" 
##    log_file.write(cmd_message)
##    log_file.write(binascii.b2a_hex(buffer_to_send))
##    command_echo = receiveSock.recv(BUFSIZE)
##    cmd_message = "\nCMD echo (GEMROC WR): \n" 
##    log_file.write(cmd_message)
##    log_file.write(binascii.b2a_hex(command_echo))
##
##time.sleep (3)
##
##gemroc_inst_0.set_FEB_PWR_EN_pattern(0x5)
##gemroc_inst_0.set_gemroc_cmd_code('WR',1)
##gemroc_inst_0.update_command_words()
###gemroc_inst_0.print_command_words()
##array_to_send = gemroc_inst_0.command_words
##buffer_to_send = struct.pack('>' + str(len(array_to_send)) + 'L', *array_to_send)
##clientSock.sendto(buffer_to_send, (DEST_IP_ADDRESS, DEST_PORT_NO))
##cmd_message = "\nCMD sent (GEMROC WR): \n" 
##log_file.write(cmd_message)
##log_file.write(binascii.b2a_hex(buffer_to_send))
##command_echo = receiveSock.recv(BUFSIZE)
##cmd_message = "\nCMD echo (GEMROC WR): \n" 
##log_file.write(cmd_message)
##log_file.write(binascii.b2a_hex(command_echo))
##
##time.sleep (3)
##
##gemroc_inst_0.set_FEB_PWR_EN_pattern(0x0)
##gemroc_inst_0.set_gemroc_cmd_code('WR',1)
##gemroc_inst_0.update_command_words()
###gemroc_inst_0.print_command_words()
##array_to_send = gemroc_inst_0.command_words
##buffer_to_send = struct.pack('>' + str(len(array_to_send)) + 'L', *array_to_send)
##clientSock.sendto(buffer_to_send, (DEST_IP_ADDRESS, DEST_PORT_NO))
##cmd_message = "\nCMD sent (GEMROC WR): \n" 
##log_file.write(cmd_message)
##log_file.write(binascii.b2a_hex(buffer_to_send))
##command_echo = receiveSock.recv(BUFSIZE)
##cmd_message = "\nCMD echo (GEMROC WR): \n" 
##log_file.write(cmd_message)
##log_file.write(binascii.b2a_hex(command_echo))




##
##gemroc_inst.set_target_TIGER(0)
##gemroc_inst.set_gemroc_cmd_code('GEN_TA_SYNCH_RST',1)
##gemroc_inst.update_command_words()
##array_to_send = gemroc_inst.command_words
##buffer_to_send = struct.pack('>' + str(len(array_to_send)) + 'L', *array_to_send)
##clientSock.sendto(buffer_to_send, (DEST_IP_ADDRESS, DEST_PORT_NO))
##cmd_message = "\nCMD sent: \n" 
##log_file.write(cmd_message)
##log_file.write(binascii.b2a_hex(buffer_to_send))
##command_echo = receiveSock.recv(BUFSIZE)
##cmd_message = "\nCMD echo: \n" 
##log_file.write(cmd_message)
##log_file.write(binascii.b2a_hex(command_echo))
##
##time.sleep(3)
##
##gemroc_inst.set_target_TIGER(0)
##gemroc_inst.set_gemroc_cmd_code('EN_TP_FROM_HEARTBEAT_WORD',1)
##gemroc_inst.update_command_words()
##array_to_send = gemroc_inst.command_words
##buffer_to_send = struct.pack('>' + str(len(array_to_send)) + 'L', *array_to_send)
##clientSock.sendto(buffer_to_send, (DEST_IP_ADDRESS, DEST_PORT_NO))
##cmd_message = "\nCMD sent: \n" 
##log_file.write(cmd_message)
##log_file.write(binascii.b2a_hex(buffer_to_send))
##command_echo = receiveSock.recv(BUFSIZE)
##cmd_message = "\nCMD echo: \n" 
##log_file.write(cmd_message)
##log_file.write(binascii.b2a_hex(command_echo))
##
##time.sleep(3)
##
##gemroc_inst.set_target_TIGER(0)
##gemroc_inst.set_gemroc_cmd_code('DIS_TP_FROM_HEARTBEAT_WORD',1)
##gemroc_inst.update_command_words()
##array_to_send = gemroc_inst.command_words
##buffer_to_send = struct.pack('>' + str(len(array_to_send)) + 'L', *array_to_send)
##clientSock.sendto(buffer_to_send, (DEST_IP_ADDRESS, DEST_PORT_NO))
##cmd_message = "\nCMD sent: \n" 
##log_file.write(cmd_message)
##log_file.write(binascii.b2a_hex(buffer_to_send))
##command_echo = receiveSock.recv(BUFSIZE)
##cmd_message = "\nCMD echo: \n" 
##log_file.write(cmd_message)
##log_file.write(binascii.b2a_hex(command_echo))

log_file.close()

