# File containing the beginning stages of our algorithm code-5 minute blocks
import config
import socket
import re
import collections

class Algorithm(object):
    
    def take_from_list(self, amount):
        # Function to take variable number of elements off of an ordered dict
        # Returns list of entries with size amount
        out_list = []
        for x in range(1, amount):
            out_list = out_list.append(next(self))

        return out_list

    def calc_avgs(self, data_list):
        # Calculates averages of demand over 5 minute period by region
        5_min_avg_NA = (sum([x['DEMAND']['trades_NA'] for x in [data_list]]) / len(data_list))
        5_min_avg_EU = (sum([x['DEMAND']['trades_EU'] for x in [data_list]]) / len(data_list))
        5_min_avg_AP = (sum([x['DEMAND']['trades_AP'] for x in [data_list]]) / len(data_list))

        current_demand_avgs = {
            'NA_avg': 5_min_avg_NA
            'EU_avg': 5_min_avg_EU
            'AP_avg': 5_min_avg_AP
        }

        return current_demand_avgs

    def calc_delta(self, avg_list):
        # Calculates averages changes of demand over 5 minute period by region
        delta_list = []
        for i in range (0, (len(avg_list)) - 1): # May have to change to -2 to avoid outOfBounds error
            curr = avg_list[i]
            fol = avg_list[i+1]

            if (fol == null)
                return

            else
                delta_diffs = {

                   'NA_delta': (fol['NA_avg'] - curr['NA_avg'])
                   'EU_delta': (fol['EU_avg'] - curr['EU_avg'])
                   'AP_delta': (fol['AP_avg'] - curr['AP_avg'])

                }

            delta_list = delta_list.append(delta_diffs)

        return delta_list

    def calc_deltasq(self, change_list):
        # Calculate averages of change of change of demand over 5 minute period by region
        deltasq_list = []
        for j in range(0, (len(change_list)) - 1): # May have to change to -2 to avoid outOfBounds error
            curr = change_list[1]
            fol = change_list[i+1]

            if (fol == null)
                return

            else
                deltasq_diffs = {

                    'NA_deltasq': (fol['NA_delta'] - curr['NA_delta'])
                    'EU_deltasq': (fol['EU_delta'] - curr['EU_delta'])
                    'AP_deltasq': (fol['AP_delta'] - curr['AP_delta'])

                }

            deltasq_list = deltasq_list.append(deltasq_diffs)

        return deltasq_list
