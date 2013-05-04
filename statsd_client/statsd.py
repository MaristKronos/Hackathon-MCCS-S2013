"""Utility collection for connecting with StatsD"""

# Our imports
import config

# Vendor imports
import socket


class Statsd(object):
    _statsd = None
    _enc = ''

    def __init__(self, host=config.HOST, port=config.PORT, enc=config.ENC_KEY):
        self._statsd = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)
        self._.bind((host, port))
        self._enc = enc

    def __del__(self):
        self._stastd.shutdown(socket.SHUT_RDWR)
        self._stastd.close()
