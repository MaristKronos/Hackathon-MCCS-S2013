import config

import socket

sock = socket.create_connection(config.CONNECTION_TUPLES[0])
sock.send(config.BEGIN)
print sock.recv(config.MAX_PACKET_SIZE)
sock.send(config.RECEIVE)
print sock.recv(config.MAX_PACKET_SIZE)
sock.send(config.START)
print sock.recv(config.MAX_PACKET_SIZE)
sock.send(config.STOP)

sock.shutdown(socket.SHUT_RDWR)
sock.close()
