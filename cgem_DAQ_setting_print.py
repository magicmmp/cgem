
GEMROC_ID=3
cfg_filename = 'GEMROC_%d_def_cfg_DAQ_2018v3.txt' %GEMROC_ID
GEMROC_CMD_DAQ_Num_of_params=15
parameter_array = [0 for i in range(GEMROC_CMD_DAQ_Num_of_params-1)]
with open(cfg_filename, "r") as f:
	parameter_array = map(int, f)
UDP_DATA_DESTINATION_IPPORT = parameter_array [GEMROC_CMD_DAQ_Num_of_params-14]
UDP_DATA_DESTINATION_IPADDR = parameter_array [GEMROC_CMD_DAQ_Num_of_params-15]
print "DAQ setting file is : %s" % cfg_filename 
print "cgem data recv UDP IP   = %d\n" %UDP_DATA_DESTINATION_IPADDR
print "cgem data recv UDP port = %d\n"%UDP_DATA_DESTINATION_IPPORT











