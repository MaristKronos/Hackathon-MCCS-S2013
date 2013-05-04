"""Utility collection for connecting with StatsD"""

# Our imports
import config

# Vendor imports
import statsd


class Statsd(object):
    _statsd = None

    def __init__(
        self,
        host=config.host,
        port=config.port,
        sample_rate=config.sample_rate,
        disabled=config.disabled
    ):
        self._statsd = statsd.Connection.set_defaults(
            host,
            port,
            sample_rate,
            disabled
        )

    def __del__(self):
        pass
