import socket

#create an INET, STREAMing socket
serversocket = socket.socket(
    socket.AF_INET, socket.SOCK_STREAM)
#bind the socket to a public host,
# and a well-known port
serversocket.bind((socket.gethostname(), 4242))
#become a server socket
serversocket.listen(5)
conn, addr = serversocket.accept()
print ("Connected by", addr)
while 1:
    data = conn.recv(8)
    if not data: break
    print (data)
conn.close()
