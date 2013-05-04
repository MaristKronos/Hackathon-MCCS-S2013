import socket

sock = socket.create_connection(('hackathon.hopto.org', 31671))
sock.send('INIT Kronos')
print sock.recv(999)
sock.send('RECD')
print sock.recv(999)
sock.send('START')
print sock.recv(999)
sock.send('STOP')
print (sock.recv(999))
sock.shutdown(socket.SHUT_RDWR)
sock.close()
