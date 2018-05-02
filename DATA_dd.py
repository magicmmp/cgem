import socket
import string
import binascii

HOST_IP = "192.168.1.200"
HOST_PORT = 58912+2
####FF_PORT => SLV(58880, 16), -- S.C. 2018-03-08: FF_PORT sends data to UDP port group 58880+0..58880+15 (base pattern 0xNNN[0] = 0xE600)
##HOST_PORT = 58880+2




#Total_data_size = 2**24
Total_data_size = 2**16
BUFSIZE = 4096
FEB_index = 0
data_file = 'FEB%d_data.txt'% FEB_index
bindata_file = 'FFF_bin.dat20180502'
out_file = open(data_file, 'w')
binout_file = open(bindata_file, 'wb')

serverSock = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)
serverSock.settimeout(None)
serverSock.bind((HOST_IP, HOST_PORT))
print 'after bind....'
s=''
Total_Data=0
while Total_Data < Total_data_size :
    data,addr = serverSock.recvfrom(BUFSIZE)
    binout_file.write(data)
    Total_Data += len(data)
##-- received_tdata (45 downto 30); -- Tcoarse: 16 bits
##-- received_tdata (29 downto 20); -- Ecoarse: 10 bits
##-- received_tdata (19 downto 10); -- Tfine: 10 bits
##-- received_tdata ( 9 downto  0); -- Efine: 10 bits
## comment this block to avoid parsing        

out_file.close()
binout_file.close()
print 'finished writing file'
serverSock.close()
