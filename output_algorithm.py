import math

_hist_weight = 0.1
_demand_weight = 1
_roc_weight = 0


#TODO: deal with weekend edgecase for history by setting weight to 0
#this provides a weighted average for the current demand and the predicted demand of our algorithms
def demand(config, hist, demand, roc):

    na_demand = (hist['na']*_hist_weight+demand['trades_NA']*_demand_weight+roc['NA_demand_predict']*_roc_weight)/(_hist_weight+_demand_weight+_roc_weight)
    eu_demand = (hist['eu']*_hist_weight+demand['trades_EU']*_demand_weight+roc['EU_demand_predict']*_roc_weight)/(_hist_weight+_demand_weight+_roc_weight)
    ap_demand = (hist['ap']*_hist_weight+demand['trades_AP']*_demand_weight+roc['AP_demand_predict']*_roc_weight)/(_hist_weight+_demand_weight+_roc_weight)

    return web_tier(
        config,
        {
            "na": na_demand,
            "eu": eu_demand,
            "ap": ap_demand
        })

def adjust(hist,demand,roc):
    #check how close we are to the last guess
    hist_sum  = hist['na']+hist['eu']+hist['ap']
    demand_sum = demand['trades_NA']+demand['trades_EU']+demand['trades_AP']
    roc_sum = roc['NA_demand_predict']+roc['EU_demand_predict']+roc['AP_demand_predict']

    hist_err = 1 - ((hist_sum - demand_sum) / demand_sum)
    roc_err = 1 - ((roc_sum - demand_sum) / demand_sum)
    print "errors rates"
    print hist_err
    print roc_err

    

def web_tier(config, demand):
    change_of_servers = {}

    change_of_servers['w_na'] = max( math.ceil( (demand['na'] - (config['web_NA_total'] * 180))/180 ), -config['web_NA_total'] )
    change_of_servers['w_eu'] = max( math.ceil( (demand['eu'] - (config['web_EU_total'] * 180))/180 ), -config['web_EU_total'] )
    change_of_servers['w_ap'] = max( math.ceil( (demand['ap'] - (config['web_AP_total'] * 180))/180 ), -config['web_AP_total'] )


    return java_tier(config,change_of_servers,demand)

def java_tier(config, change_of_servers, demand):

    #we don't want EU to have any java servers
    #update demand to match new servers
    demand['na'] = demand['na'] + (demand['eu']*0.9)
    demand['eu'] = 0
    #this function calculates the demand per server and rounds up.  This will become the change for our servers
    change_of_servers['j_na'] = max( math.ceil( (demand['na'] - (config['java_NA_total']+change_of_servers['w_na']+change_of_servers['w_eu']) * 400)/400), -config['java_NA_total'] )
    #remove EU servers if they exist
    change_of_servers['j_eu'] = -config['java_EU_total']
    change_of_servers['j_ap'] = max(  math.ceil( (demand['ap'] - (config['java_AP_total']+change_of_servers['w_ap']) * 400)/400), -config['java_AP_total'] )



    return database_tier(config, change_of_servers, demand)

def database_tier(config, change_of_servers, demand):
    demand['na'] = demand['na'] + (demand['ap']*0.9)
    demand['ap'] = 0

    change_of_servers['d_na'] = max(math.ceil( (demand['na'] - (config['db_NA_total']+change_of_servers['j_na'] + change_of_servers['j_ap']) * 1000)/1000), ((-config['db_NA_total'])+1) )
    change_of_servers['d_eu'] = int(-config['db_EU_total'])
    change_of_servers['d_ap'] = int(-config['db_AP_total'])

    return change_of_servers
