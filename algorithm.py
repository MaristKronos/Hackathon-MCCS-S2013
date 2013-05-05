# File containing the beginning stages of our algorithm code-5 minute blocks
import config
import socket
import re
import collections

accum_list = []

def take_from_list(data_list, amount):
    # Function to take variable number of elements of a list
    out_list = data_list[:amount]

    return out_list

def handle_data_input(full_data_list):

     accum_list = []

     avgs = calc_avgs(take_from_list(full_data_list, 10))

     if len(avgs) < 2:
        avgs.append(avgs[0])

     deltas = calc_delta(avgs)
     if len(deltas) < 2:
        deltas.append(deltas[0])

     deltaprs = calc_deltapr(deltas)

     return predict_demand(avgs, deltas, deltaprs)

def calc_avgs(data_list):
    # Calculates averages of demand over 5 minute period by region
    five_min_avg_NA = (sum([x['demand']['trades_NA'] for x in data_list])) / len(data_list)
    five_min_avg_EU = (sum([x['demand']['trades_EU'] for x in data_list])) / len(data_list)
    five_min_avg_AP = (sum([x['demand']['trades_AP'] for x in data_list])) / len(data_list)

    current_demand_avgs = {
        'NA_avg': five_min_avg_NA,
        'EU_avg': five_min_avg_EU,
        'AP_avg': five_min_avg_AP,
    }

    accum_list.append(current_demand_avgs)

    return accum_list

def calc_delta(avg_list):
    # Calculates averages changes of demand over 5 minute period by region
    delta_list = []
    for i in range(len(avg_list) - 1): # May have to change to -2 to avoid outOfBounds error
        curr = avg_list[i]
        fol = avg_list[i+1]

        if (not fol):
            return

        else:
            delta_diffs = {

               'NA_delta': (fol['NA_avg'] - curr['NA_avg']),
               'EU_delta': (fol['EU_avg'] - curr['EU_avg']),
               'AP_delta': (fol['AP_avg'] - curr['AP_avg']),

            }

        delta_list.append(delta_diffs)

    return delta_list

def calc_deltapr(change_list):
    # Calculates averages of change of change of demand over 5 minute period by region

    deltapr_list = []
    for i in range(0, (len(change_list)) - 1): # May have to change to -2 to avoid outOfBounds error
        curr = change_list[1]
        fol = change_list[i+1]

        if (not fol):
            return

        else:
            deltapr_diffs = {

                'NA_deltapr': (fol['NA_delta'] - curr['NA_delta']),
                'EU_deltapr': (fol['EU_delta'] - curr['EU_delta']),
                'AP_deltapr': (fol['AP_delta'] - curr['AP_delta']),

             }

        deltapr_list.append(deltapr_diffs)

    return deltapr_list

def predict_demand(dem_list, d_list, dpr_list):
    # Predicts demand future totals for each region

    time_interval = 10

    NA_predict = ((sum([x['NA_deltapr'] for x in dpr_list]) / len(dpr_list)) * time_interval * time_interval) + ((sum([x['NA_delta'] for x in dp_list]) / len(dp_list)) * time_interval) + ((dem_list[0])['NA_avg'])

    EU_predict = ((sum([x['EU_deltapr'] for x in dpr_list]) / len(dpr_list)) * time_interval * time_interval) + ((sum([x['EU_delta'] for x in dp_list]) / len(dp_list)) * time_interval) + ((dem_list[0])['EU_avg'])

    AP_predict = ((sum([x['AP_deltapr'] for x in dpr_list]) / len(dpr_list)) * time_interval * time_interval) + ((sum([x['AP_delta'] for x in dp_list]) / len(dp_list)) * time_interval) + ((dem_list[0])['AP_avg'])


    prediction_dict = {

        'NA_demand_predict': NA_predict,
        'EU_demand_predict': EU_predict,
        'AP_demand_predict': AP_predict,

    }

    return prediction_dict
