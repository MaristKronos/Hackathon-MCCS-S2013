"""Our classes that work with Hermes"""

# Our imports
import config

# Vendor imports
import socket

import re

import collections


class INCORRECT_STR(Exception):
    pass


class COSTS_STR_INCORRECT(INCORRECT_STR):
    pass


class DIST_STR_INCORRECT(INCORRECT_STR):
    pass


class DEMAND_STR_INCORRECT(INCORRECT_STR):
    pass


class PROFIT_STR_INCORRECT(INCORRECT_STR):
    pass


class CONFIG_STR_INCORRECT(INCORRECT_STR):
    pass


class CONNECTION_NOT_ACCEPTED(INCORRECT_STR):
    pass


class Hermes_Client(object):
    """Our direct client that talks to Hermes"""

    _store_the_internet = []

    def __init__(self, conn=config.CONNECTION_TUPLES[0]):
        self._store_the_internet = []

        self._con = socket.create_connection(conn)
        response = self.send_receive(config.BEGIN)
        if response != 'ACCEPT':
            raise CONNECTION_NOT_ACCEPTED(response)
        costs_str = self.send_receive(config.RECEIVE)
        self._costs = self.parse_costs(costs_str)

        _config = self.send_receive(config.START)
        self._config = self.parse_config(_config)

        self.get_stats()

    def __del__(self):
        """Once our instance ends, we want to ensure a good disconnect"""
        self._con.send(config.STOP)
        self._con.shutdown(socket.SHUT_RDWR)
        self._con.close()

    def send(self, msg):
        self._con.send(msg)

    def receive(self):
        return self._con.recv(config.MAX_PACKET_SIZE)

    def send_receive(self, msg):
        self.send(msg)
        return self.receive()

    def parse_costs(self, costs_str):

        # TODO: This regex is used frequently, compile it when
        # there is time to look that up.
        costs_list = re.split('\s+', costs_str)
        if costs_list[0] != "COSTS" and len(costs_list) != 5:
            raise COSTS_STR_INCORRECT

        costs_dict = {
            'transaction': int(costs_list[1]),
            'web': int(costs_list[2]),
            'java': int(costs_list[3]),
            'db': int(costs_list[4]),
        }

        return costs_dict

    def parse_dist(self, dist_str):

        # TODO: This regex is used frequently, compile it when
        # there is time to look that up.
        dist_list = re.split('\s+', dist_str)
        if dist_list[0] != "DIST" and len(dist_str) != 10:
            raise DIST_STR_INCORRECT

        dist_dict = {
            'NA_web': int(dist_list[1]),
            'EU_web': int(dist_list[2]),
            'AP_web': int(dist_list[3]),
            'NA_java': int(dist_list[4]),
            'EU_java': int(dist_list[5]),
            'AP_java': int(dist_list[6]),
            'NA_db': int(dist_list[7]),
            'EU_db': int(dist_list[8]),
            'AP_db': int(dist_list[9]),
        }

        return dist_dict

    def parse_demand(self, demand_str):

        # TODO: This regex is used frequently, compile it when
        # there is time to look that up.
        demand_list = re.split('\s+', demand_str)
        if demand_list[0] != "DEMAND" and len(demand_str) != 8:
            raise DEMAND_STR_INCORRECT

        demand_dict = {
            'day': demand_list[1],
            'hour': demand_list[2],
            'minute': demand_list[3],
            'second': demand_list[4],
            'trades_NA': demand_list[5],
            'trades_EU': demand_list[6],
            'trades_AP': demand_list[7],
        }

        return demand_dict

    def parse_profit(self, profit_str):

        # TODO: This regex is used frequently, compile it when
        # there is time to look that up.
        profit_list = re.split('\s+', profit_str)
        if profit_list[0] != "PROFIT" and len(profit_str) != 5:
            raise PROFIT_STR_INCORRECT

        profit_dict = {
            'profit_last_period': int profit_list[1],
            'actual/max_profit_%_last period': int profit_list[2],
            'total_profit': int profit_list[3],
            'actual/max_profit_%_total': int profit_list[4],
        }

        return profit_dict

    def parse_config(self, config_str):

        config_list = re.split('\s+', config_str)
        if config_list[0] != "CONFIG" and len(config_str) != 10:
            raise CONFIG_STR_INCORRECT

        config_dict = {
            'web_NA_total': int(config_list[1]),
            'web_EU_total': int(config_list[2]),
            'web_AP_total': int(config_list[3]),
            'java_NA_total': int(config_list[4]),
            'java_EU_total': int(config_list[5]),
            'java_AP_total': int(config_list[6]),
            'db_NA_total': int(config_list[7]),
            'db_EU_total': int(config_list[8]),
            'db_AP_total': int(config_list[9]),
        }

        return config_dict

    def get_stats(self):
        demand = self.parse_demand(self.send_receive(config.RECEIVE))
        dist = self.parse_dist(self.send_receive(config.RECEIVE))
        profit = self.parse_profit(self.send_receive(config.RECEIVE))

        self._store_the_internet.append({
            'demand': demand,
            'dist': dist,
            'profit': profit
        })

        return (demand, dist, profit)

    def next_turn(self):
        maybe_end = self.send_control(self.calc_serv_delta())
        if maybe_end == 'END':
            self.send_receive(config.STOP)
            return False
        else:
            self._config = self.parse_config(maybe_end)
            return True

    def send_control(self, all_servers):
        return self.send_receive(config.CONTROL % all_servers)

    def calc_serv_delta(self):
        return {
            "w_na": 0,
            "w_eu": 0,
            "w_ap": 0,
            "j_na": 0,
            "j_eu": 0,
            "j_ap": 0,
            "d_na": 0,
            "d_eu": 0,
            "d_ap": 0
        }
