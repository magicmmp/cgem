import socket  
address = ('127.0.0.1', 54831)
address2 = ('127.0.0.1', 58927)
s = socket.socket(socket.AF_INET, socket.SOCK_DGRAM) 
s.bind(address) 
s2 = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)
s2.bind(address2)
while True:
    data, addr = s.recvfrom(2048) 
    if not data:
        print "client has exist"
        break
    s2.sendto(data, addr)
    print "received:", data, "from", addr
s.close() 
