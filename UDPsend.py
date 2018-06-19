import socket
import time
clientSock = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)
data= [0 for i in range(2)]
i=0
while 1:
    i=i+1
    data[0]=:
    buffer_to_send = struct.pack('>' + str(len(array_to_send_param)) + 'I', *array_to_send_param)
    clientSock.sendto(data, ('192.168.1.201', 58914))
    time.sleep(2)
clientSock.close()
