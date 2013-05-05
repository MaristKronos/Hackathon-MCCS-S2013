# File containing the beginning stages of our algorithm code-5 minute blocks
import config
import socket
import re
import collections

class Algorithm(object):

    def take_from_list(self, data_list, amount):
        # Function to take variable number of elements of a list
        out_list = data_list[amount:]

        return out_list

    def calc_avgs(self, data_list):
        # Calculates averages of demand over 5 minute period by region
        five_min_avg_NA = (sum([x['demand']['trades_NA'] for x in [data_list]]) / len(data_list))
        five_min_avg_EU = (sum([x['demand']['trades_EU'] for x in [data_list]]) / len(data_list))
        five_min_avg_AP = (sum([x['demand']['trades_AP'] for x in [data_list]]) / len(data_list))

        current_demand_avgs = {
            'NA_avg': five_min_avg_NA,
            'EU_avg': five_min_avg_EU,
            'AP_avg': five_min_avg_AP,
        }

        return current_demand_avgs

    def calc_delta(self, avg_list):
        # Calculates averages changes of demand over 5 minute period by region
        delta_list = []
        for i in range (0, (len(avg_list)) - 1): # May have to change to -2 to avoid outOfBounds error
            curr = avg_list[i]
            fol = avg_list[i+1]

            if (fol == null):
                return

            else:
                delta_diffs = {

                   'NA_delta': (fol['NA_avg'] - curr['NA_avg']),
                   'EU_delta': (fol['EU_avg'] - curr['EU_avg']),
                   'AP_delta': (fol['AP_avg'] - curr['AP_avg']),

                }

            delta_list = delta_list.append(delta_diffs)

        return delta_list

    def calc_deltapr(self, change_list):
        # Calculates averages of change of change of demand over 5 minute period by region

        deltapr_list = []
        for j in range(0, (len(change_list)) - 1): # May have to change to -2 to avoid outOfBounds error
            curr = change_list[1]
            fol = change_list[i+1]

            if (fol == null):
                return

            else:
                deltapr_diffs = {

                    'NA_deltapr': (fol['NA_delta'] - curr['NA_delta']),
                    'EU_deltapr': (fol['EU_delta'] - curr['EU_delta']),
                    'AP_deltapr': (fol['AP_delta'] - curr['AP_delta']),

                }

            deltapr_list = deltapr_list.append(deltapr_diffs)

        return deltapr_list

    def predict_demand(self, dem_list, d_list, dpr_list):
        # Predicts demand future totals for each region

        turn_interval = 10

        NA_predict = (((dpr_list[0])['NA_deltapr']) * time_interval * time_interval) + (((d_list[0])['NA_delta']) * time_interval) + ((dem_list[0])['NA_avg'])
        EU_predict = (((dpr_list[0])['EU_deltapr']) * time_interval * time_interval) + (((d_list[0])['EU_delta']) * time_interval) + ((dem_list[0])['EU_avg'])
        AP_predict = (((dpr_list[0])['AP_deltapr']) * time_interval * time_interval) + (((d_list[0])['AP_delta']) * time_interval) + ((dem_list[0])['AP_avg'])

        prediction_dict = {

            'NA_demand_predict': NA_predict,
            'EU_demand_predict': EU_predict,
            'AP_demand_predict': AP_predict,

        }

        return prediction_dict
