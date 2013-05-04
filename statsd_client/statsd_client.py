"""Utility collection for connecting with StatsD"""

# Our imports
import config

# Vendor imports
import statsd
import time


class TimerDoesNotExistError(Exception):
    pass


class Statsd(object):
    _statsd = None
    _enc = ''
    _objs = {}

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
        self._objs = {}

    def __del__(self):
        pass

    def start_timer(self, timer_name):
        if not timer_name in self._objs.keys():
            self._objs[timer_name] = self._statsd.timer(self._enc)
        self._objs[timer_name].start()

    def stop_timer(self, timer_name):
        if not timer_name in self._objs.keys():
            raise TimerDoesNotExistError
        self._objs[timer_name].stop()


x = Statsd()
x.start_timer('yo')
time.sleep(5)
x.stop_timer('yo')
