"""Our classes that work with Hermes"""

# Our imports
import config
import seed
import algorithm
import output_algorithm
# Vendor imports
import socket
import re


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
    _config = {}
    _store_the_internet = []
    _week_demand_history = {}
    _turns = 0

    def __init__(self, conn=config.CONNECTION_TUPLES[0]):
        self._store_the_internet = []
        self._week_demand_history = seed.seed
        self._turns = 0

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
            raise DEMAND_STR_INCORRECT(demand_str)

        demand_dict = {
            'day': demand_list[1],
            'hour': int(demand_list[2]),
            'minute': int(demand_list[3]),
            'second': int(demand_list[4]),
            'trades_NA': int(demand_list[5]),
            'trades_EU': int(demand_list[6]),
            'trades_AP': int(demand_list[7]),
        }

        return demand_dict

    def parse_profit(self, profit_str):

        # TODO: This regex is used frequently, compile it when
        # there is time to look that up.
        profit_list = re.split('\s+', profit_str)
        if profit_list[0] != "PROFIT" and len(profit_str) != 5:
            raise PROFIT_STR_INCORRECT

        profit_dict = {
            'profit_last_period': int(profit_list[1]),
            'actual/max_profit_%_last period': int(profit_list[2]),
            'total_profit': int(profit_list[3]),
            'actual/max_profit_%_total': int(profit_list[4]),
        }

        return profit_dict

    def parse_config(self, config_str):

        config_list = re.split('\s+', config_str)
        if config_list[0] != "CONFIG" and len(config_str) != 10:
            raise CONFIG_STR_INCORRECT(config_str)

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
        #set the week demand turn granularity(wdtg)
        self.wdtg = 16
        eu_sum = 0
        na_sum = 0
        ap_sum = 0
        currTime = '%(day)s %(hour)s %(minute)s %(second)s'

        demand = self.parse_demand(self.send_receive(config.RECEIVE))
        dist = self.parse_dist(self.send_receive(config.RECEIVE))
        profit = self.parse_profit(self.send_receive(config.RECEIVE))
        
        print demand
        print profit
        print self._config

        self._store_the_internet.append({
            'demand': demand,
            'dist': dist,
            'profit': profit
        })

        # For every (wdtg) turns average the stats demand and save elsewhere
        # to predict for next time.
        # We multiply wdtg by 2 to convert to minutes
        # TODO: handle special case where we divide starting trades by wdtg
        # even thought there is only 1 item...
        if self._turns == 0:
            datapoint = self._store_the_internet[-1]
            eu_sum = datapoint['demand']['trades_EU']
            na_sum = datapoint['demand']['trades_NA']
            ap_sum = datapoint['demand']['trades_AP']
            self._week_demand_history[currTime % datapoint['demand']] = {
                'eu': eu_sum,
                'na': na_sum,
                'ap': ap_sum,
            }
        elif self._turns % self.wdtg == 0:
            eu_sum = 0
            na_sum = 0
            ap_sum = 0
        lst = self._store_the_internet[-self.wdtg:]
        for x in lst:
            eu_sum += x['demand']['trades_EU']
            na_sum += x['demand']['trades_NA']
            ap_sum += x['demand']['trades_AP']

        self._week_demand_history[currTime % lst[-1]['demand']] = {
            'eu': eu_sum/self.wdtg,
            'na': na_sum/self.wdtg,
            'ap': ap_sum/self.wdtg
        }
        return (demand, dist, profit)

    def next_turn(self):
        maybe_end = self.send_control(self.calc_serv_delta())
        if maybe_end == 'END':
            self.send_receive(config.STOP)
            return False
        else:
            #Increment the turn counter.
            self._turns += 1
            return True

    def send_control(self, all_servers):
        # TODO, first 1 should change based on day of the week [sat, sun]
        # Rate of change results
        if not len(self._store_the_internet) % 8:
            roc = algorithm.handle_data_input(self._store_the_internet)
            hist = self.hist_predict()
            result = output_algorithm.demand(self._config, hist, self._store_the_internet[-1]['demand'], roc)
            self._config = self.get_our_config(result)
            #before returning, check our gueses 
            output_algorithm.adjust(hist, self._store_the_internet[-1]['demand'], roc)
            return self.send_receive(config.CONTROL % result)
        return self.send_receive('CONTROL 0 0 0 0 0 0 0 0 0')

    def parse_control_to_config(self, control_data):

        config_format = {

            'web_NA_total': control_data['w_na'],
            'web_EU_total': control_data['w_eu'],
            'web_AP_total': control_data['w_ap'],
            'java_NA_total': control_data['j_na'],
            'java_EU_total': control_data['j_eu'],
            'java_AP_total': control_data['j_ap'],
            'db_NA_total': control_data['d_na'],
            'db_EU_total': control_data['d_eu'],
            'db_AP_total': control_data['d_ap'],

        }

        return config_format


    def determine_true_config(self, server_changes):

        true_config = {

            'web_NA_total': (self._config['web_NA_total'] + server_changes['web_NA_total']),
            'web_EU_total': (self._config['web_EU_total'] + server_changes['web_EU_total']),
            'web_AP_total': (self._config['web_AP_total'] + server_changes['web_AP_total']),
            'java_NA_total': (self._config['java_NA_total'] + server_changes['java_NA_total']),
            'java_EU_total': (self._config['java_EU_total'] + server_changes['java_EU_total']),
            'java_AP_total': (self._config['java_AP_total'] + server_changes['java_AP_total']),
            'db_NA_total': (self._config['db_NA_total'] + server_changes['db_NA_total']),
            'db_EU_total': (self._config['db_EU_total'] + server_changes['db_EU_total']),
            'db_AP_total': (self._config['db_AP_total'] + server_changes['db_AP_total']),

        }

        return true_config


    def get_our_config(self, control_dict):

        new_config = self.determine_true_config(self.parse_control_to_config(control_dict))
        
        return new_config


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

    def sub_day(self, myday):
        days = ['MON', 'TUE', 'WED', 'THU', 'FRI', 'SAT', 'SUN']
        day_index = days.index(myday) - 1
        if day_index < 0 or day_index >= len(days):
            day_index = 5

        return days[day_index]

    def add_day(self, myday):
        days = ['MON', 'TUE', 'WED', 'THU', 'FRI', 'SAT', 'SUN']
        day_index = days.index(myday) + 1
        if day_index < 0 or day_index >= len(days):
            day_index = 0

        return days[day_index]

    def hist_predict(self):
        # TODO: refine to ignore SAT and SUN, if SAT or SUN, instead
        # return another day like friday!
        returnval = None

        demand = self._store_the_internet[-1]['demand']
        if demand['day'] == 'SAT' or demand['day'] == 'SUN':
            return 0  # special case we will worry about later


        day = demand['day']
        hour = int(demand['hour'])
        minute = int(demand['minute'])
        second = int(demand['second'])


        if minute == 0:
            minute = 0
        elif minute < 7.5:
            minute = 7.5
        elif minute < 15:
            minute = 15
        elif minute < 22.5:
            minute = 22.5
        elif minute < 30:
            minute = 30
        elif minute < 37.5:
            minute = 37.5
        elif minute < 45:
            minute = 45
        elif minute < 52.5:
            minute = 52.5
        else:
            if hour == 23:
                day = self.add_day(day)
                hour = 0
            else:
                hour += 1

        try:
            returnval = self._week_demand_history['%s %s %s %s' % (day, hour, minute, second)]
        except:
            try:
                day = self.sub_day(day)
                returnval = self._week_demand_history['%s %s %s %s' % (day, hour, minute, second)]
            except:
                return {
                    'eu': demand['trades_EU'],
                    'na': demand['trades_NA'],
                    'ap': demand['trades_AP'],
                }

        return returnval
