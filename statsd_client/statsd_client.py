"""Utility collection for connecting with StatsD"""

# Our imports
import config

# Vendor imports
import socket


class Statsd(object):
    _statsd = None

    def __init__(self, host=config.HOST, port=config.PORT, sample_rate=config.SAMPLE_RATE, prefix=config.ENC_KEY):
        self._statsd = socket.socket()
        self._statsd.connect((host, port))

    def __del__(self):
        self._statsd.close()

    def update_value(self, key, value, the_time):
        lines = []
        lines.append("hackathon3.%s %s %d" % (key, value, the_time))
        message = '\n'.join(lines) + '\n'
        self._statsd.sendall(message)
