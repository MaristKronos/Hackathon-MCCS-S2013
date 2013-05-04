"""Utility collection for connecting with StatsD"""

# Our imports
import config

# Vendor imports
import statsd


class Statsd(object):
    _statsd = None
    _objs = {}

    def __init__(self, host=config.HOST, port=config.PORT, sample_rate=config.SAMPLE_RATE, prefix=config.ENC_KEY):
        self._statsd = statsd.StatsdClient( host=host, port=port, prefix=prefix, sample_rate=sample_rate)
        self._objs = {}
        self._prefix = prefix
        self._host = host
        self._port = port

    def __del__(self):
        pass

    def update_counter(self, counter_name, value):
        if not counter_name in self._objs.keys():
            self._objs[counter_name] = statsd.StatsdCounter(counter_name)
        self._objs[counter_name] = value

x = Statsd()
x.update_counter('w_eu', 2)
