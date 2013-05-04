"""Utility collection for connecting with StatsD"""

# Our imports
import config

# Vendor imports
import statsd


class Statsd(object):
    _statsd = None
    _enc = ''

    def __init__( self, host=config.HOST, port=config.PORT, sample_rate=config.SAMPLE_RATE, prefix=config.ENC_KEY):
        self._statsd = statsd.StatsdClient(
            host=host,
            port=port,
            prefix=prefix,
            sample_rate=sample_rate
        )

    def __del__(self):
        pass

x = Statsd()
