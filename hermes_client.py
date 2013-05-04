"""Our classes that work with Hermes"""

# Our imports
import config

# Vendor imports
import socket


class Hermes_Client(object):
    """Our direct client that talks to Hermes"""

    _con = None

    def __init__(self, conn=config.CONNECTION_TUPLES[0]):
        self._con = socket.create_connection(conn)

    def send(self, msg):
        self._con.send(msg)

    def receive(self):
        return self._con.recv(config.MAX_PACKET_SIZE)

    def send_receive(self, msg):
        self.send(msg)
        return self.receive()
