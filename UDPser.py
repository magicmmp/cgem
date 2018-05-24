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
receiveSock.bind((HOST_IP, HOST_PORT_RECEIVE))
while 1:
    data,addr = receiveSock.recvfrom(BUFSIZE)
    if n==1:
      print 'receive data from ' + str(addr) + '\n'
      n=0
    clientSock.sendto(data, ('127.0.0.1', 58916))
##print 'send:',buffer_to_sen
clientSock.close()
receiveSock.close()
