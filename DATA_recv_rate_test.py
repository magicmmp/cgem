import socket
import string
import binascii
import time
HOST_IP = "192.168.1.200"
HOST_PORT = 58912+2
####FF_PORT => SLV(58880, 16), -- S.C. 2018-03-08: FF_PORT sends data to UDP port group 58880+0..58880+15 (base pattern 0xNNN[0] = 0xE600)
##HOST_PORT = 58880+2




#Total_data_size = 2**24
Total_data_size = 2**16
BUFSIZE = 4096
FEB_index = 0
data_file = 'FEB%d_data.txt'% FEB_index
bindata_file = 'FEB%d_bin.dat'% FEB_index
out_file = open(data_file, 'w')
binout_file = open(bindata_file, 'wb')

serverSock = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)
serverSock.settimeout(None)
serverSock.bind((HOST_IP, HOST_PORT))
print 'after bind....'
s=''
Total_Data=0
t0=time.time()
while Total_Data < Total_data_size :
    data,addr = serverSock.recvfrom(BUFSIZE)
    binout_file.write(data)
    hexdata = binascii.hexlify(data)
    Total_Data += len(data)
t1=time.time()
print 'time=%f '%(t1-t0)+'Total_Data bytes=%d '%Total_Data+'BW=%d\n'%(Total_Data/(t1-t0))
out_file.close()
binout_file.close()
print 'finished writing file'
serverSock.close()
