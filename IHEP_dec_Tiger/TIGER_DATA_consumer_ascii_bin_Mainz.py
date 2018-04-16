import socket
import string
import binascii

HOST_IP = "192.168.1.200"
HOST_PORT = 58912+2
#Total_data_size = 2**24
Total_data_size = 2**10
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
while Total_Data < Total_data_size :
    data,addr = serverSock.recvfrom(BUFSIZE)
    binout_file.write(data)
    hexdata = binascii.hexlify(data)
    Total_Data += len(data)
    for x in range (0, len(hexdata)-1, 16):
        int_x = 0
        for b in range (7, 0, -1):
            ##hex_to_int = (int(hexdata[x+(b-1)*2],16))*16 + int(hexdata[x+(b-1)*2+1],16)
            hex_to_int = (int(hexdata[x+b*2],16))*16 + int(hexdata[x+b*2+1],16)
            int_x = (int_x + hex_to_int) << 8
# acr 2017-11-17 results in the least significant byte being a copy of the next most significant byte        hex_to_int = (int(hexdata[x+b*2],16))*16 + int(hexdata[x+b*2+1],16)
        hex_to_int = (int(hexdata[x],16))*16 + int(hexdata[x+1],16) # acr 2017-11-17 this should fix the problem
        int_x = (int_x + hex_to_int)
##acr 2017-11-16        s = '%016X\n'%int_x
        s = '%016X '%int_x
        out_file.write(s)

## comment this block to avoid parsing        
##acr 2017-11-16        if (((int_x & 0xFF00000000000000)>>56) == 0x20):
        if (((int_x & 0xFF00000000000000)>>59) == 0x04):
            s = 'TIGER ' + '%01X: '%((int_x >> 56)& 0x7) + 'HB: '+'Framecount: %08X '%((int_x >> 15)& 0xFFFF)+'SEUcount: %08X\n'%(int_x & 0x7FFF)
#acr 2017-11-16        if (((int_x & 0xFF00000000000000)>>56) == 0x40):
        if (((int_x & 0xFF00000000000000)>>59) == 0x08):
            s = 'TIGER ' + '%01X: '%((int_x >> 56)& 0x7) + 'CW: '+'ChID: %02X '%((int_x >> 24)& 0x3F)+' CounterWord: %016X\n'%(int_x & 0x00FFFFFF)
#acr 2017-11-16        if (((int_x & 0xFF00000000000000)>>56) == 0x00):
        if (((int_x & 0xFF00000000000000)>>59) == 0x00):
            s = 'TIGER ' + '%01X: '%((int_x >> 56)& 0x7) + 'EW: '+'ChID: %02X '%((int_x >> 48)& 0x3F)+'tacID: %01X '%((int_x >> 46)& 0x3)+'Tcoarse: %04X '%((int_x >> 30)& 0xFFFF)+'Ecoarse: %03X '%((int_x >> 20)& 0x3FF)+'Tfine: %03X '%((int_x >> 10)& 0x3FF)+'Efine: %03X \n'%(int_x & 0x3FF)
        out_file.write(s)        
##-- field size in bits (56 total):   2      6        2       16         10       10     10
##-- received_tdata                 "10" & ch_ID & tac_ID & tcoarse & ecoarse & tfine & efine
##-- tcoarse(15) should match bit 0 of framecount in the HB FrameWord
##-- received_tdata (53 downto 48); -- channel_id: 6 bits
##-- received_tdata (47 downto 46); -- tac_id: 2 bits
##-- received_tdata (45 downto 30); -- Tcoarse: 16 bits
##-- received_tdata (29 downto 20); -- Ecoarse: 10 bits
##-- received_tdata (19 downto 10); -- Tfine: 10 bits
##-- received_tdata ( 9 downto  0); -- Efine: 10 bits
## comment this block to avoid parsing        

out_file.close()
binout_file.close()
print 'finished writing file'
serverSock.close()
