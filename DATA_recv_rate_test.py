import socket
import string
import binascii
import time
HOST_IP = "127.0.0.1"
HOST_PORT = 58880
####FF_PORT => SLV(58880, 16), -- S.C. 2018-03-08: FF_PORT sends data to UDP port group 58880+0..58880+15 (base pattern 0xNNN[0] = 0xE600)
##HOST_PORT = 58880+2




#Total_data_size = 2**24
Total_data_size = 2**20
BUFSIZE = 4096

serverSock = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)
serverSock.settimeout(None)
serverSock.bind((HOST_IP, HOST_PORT))
print '  data flow test:'
for i in range (0,5):
  Total_Data=0
  t0=time.time()
  while Total_Data < Total_data_size :
    data,addr = serverSock.recvfrom(BUFSIZE)
    Total_Data += len(data)
#    print len(data)
  t1=time.time()
  print '    time=%f sec '%(t1-t0)+' bytes=%d '%Total_Data+' BW=%d bytes/sec\n'%(Total_Data/(t1-t0))
print '  finish.'
serverSock.close()
