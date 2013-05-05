"""Utility collection for connecting with StatsD"""

# Our imports
import config

# Vendor imports
import socket


class Statsd(object):
    _statsd = None

    def __init__(self, host=config.host, port=config.port):
        pass

    def __del__(self):
        pass
