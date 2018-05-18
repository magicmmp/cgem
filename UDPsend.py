import socket
import time
clientSock = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)
data="data is from python."
i=0
while 1:
    i=i+1
    data="data is from python.times=%d"% i
    clientSock.sendto(data, ('192.168.1.201', 58914))
    time.sleep(2)
clientSock.close()
