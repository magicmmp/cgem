import socket 
## msg='01234567'*1024
## fo = open("FEB0_bin.dat", "r")
## msg=fo.read()
## print msg
address = ('127.0.0.1', 58914)
s = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)
while True:
##    msg = raw_input()
##    if not msg:
##	break
    s.sendto(msg, address)
s.close() 
