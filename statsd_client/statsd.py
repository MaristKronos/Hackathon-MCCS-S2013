"""Utility collection for connecting with StatsD"""

# Our imports
import config

# Vendor imports
import statsd-client


class Statsd(object):
    _statsd = None
    _enc = ''

    def __init__(
        self,
        host=config.HOST,
        port=config.PORT,
        sample_rate=config.SAMPLE_RATE,
        disabled=config.DISABLED
    ):
        self._statsd = statsd.Connection.set_defaults(
            host,
            port,
            sample_rate,
            disabled
        )

    def __del__(self):
        self._stastd.close()

x = Statsd()
