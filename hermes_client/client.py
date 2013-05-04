"""Our classes that work with Hermes"""

# Our imports
import config

# Vendor imports
import socket


class COSTS_STR_TOO_LONG(Exception):
    pass


class CONNECTION_NOT_ACCEPTED(Exception):
    pass


class Hermes_Client(object):
    """Our direct client that talks to Hermes"""

    _con = None
    _costs = None

    def __init__(self, conn=config.CONNECTION_TUPLES[0]):
        self._con = socket.create_connection(conn)

    def send(self, msg):
        self._con.send(msg)

    def receive(self):
        return self._con.recv(config.MAX_PACKET_SIZE)

    def send_receive(self, msg):
        self.send(msg)
        return self.receive()

    def parse_costs(self, cost_str):
        if len(cost_str) != 4:
            raise COSTS_STR_TOO_LONG

        costs_list = cost_str.split('')

        costs_dict = {
            'transaction': costs_list[0],
            'web': costs_list[1],
            'java': costs_list[2],
            'db': costs_list[2],
        }

        return costs_dict
