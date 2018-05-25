##import socket  
##address = ('127.0.0.1', 54831)
##address2 = ('127.0.0.1', 58927)
##s = socket.socket(socket.AF_INET, socket.SOCK_DGRAM) 
##s.bind(address) 
##s2 = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)
##s2.bind(address2)
##while True:
##    data, addr = s.recvfrom(2048) 
##    if not data:
##        print "client has exist"
##        break
##    s2.sendto(data, addr)
##    print "received:", data, "from", addr
##s.close() 
import socket
import binascii
HOST_IP="127.0.0.1"
#HOST_PORT = 54800
HOST_PORT_RECEIVE = 58913
##DEST_IP_ADDRESS = "127.0.0.1"
##DEST_PORT_NO = 58927
BUFSIZE = 4096
clientSock = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)
receiveSock = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)
##clientSock.bind((HOST_IP, HOST_PORT))
n=1
num=1
out_file = open("my_UDPser_log.txt", 'w')
out_file.close()
receiveSock.bind((HOST_IP, HOST_PORT_RECEIVE))
while 1:
    data,addr = receiveSock.recvfrom(BUFSIZE)
    if n==1:
      print 'receive data from ' + str(addr) + '\n'
      n=0
    clientSock.sendto(data, ('127.0.0.1', 58916))
    out_file = open("my_UDPser_log.txt", 'a')
    out_file.write("\nnum=%d,my_UDPser_log,send:\n"%num)
    out_file.write(binascii.b2a_hex(data))
    out_file.write("\nnum=%d,my_UDPser_log,echo:\n"%num)
    out_file.write(binascii.b2a_hex(data))
    num=num+1
    out_file.close()
##print 'send:',buffer_to_sen
clientSock.close()
receiveSock.close()
