import socket

#create an INET, STREAMing socket
serversocket = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
#bind the socket to a public host,
# and a well-known port
serversocket.bind((socket.gethostname(), 4242))
#become a server socket
serversocket.listen(1)
conn, addr = serversocket.accept()
print "Connected by", addr
while 1:
    data = conn.recv(4)
    if not data: break
    
    index = 0
    while index < len(data):
		print ord(data[index])
		index = index +1
conn.close()
print "FIN"
