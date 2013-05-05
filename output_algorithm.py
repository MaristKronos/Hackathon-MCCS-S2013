import math

#TODO: deal with weekend edgecase for history by setting weight to 0
#this provides a weighted average for the current demand and the predicted demand of our algorithms
def demand(config, hist, w1, demand, w2, roc, w3):

    print 'hist %s' % hist
    print 'roc %s' % roc

    print 'demand %s' % demand

    na_demand = (hist['na']*w1+demand['trades_NA']*w2+roc['NA_demand_predict']*w3)/(w1+w2+w3)
    eu_demand = (hist['eu']*w1+demand['trades_EU']*w2+roc['EU_demand_predict']*w3)/(w1+w2+w3)
    ap_demand = (hist['ap']*w1+demand['trades_AP']*w2+roc['AP_demand_predict']*w3)/(w1+w2+w3)

    return web_tier(
        config,
        {
            "na": na_demand,
            "eu": eu_demand,
            "ap": ap_demand
        })

def web_tier(config, demand):
    change_of_servers = {}

    change_of_servers['w_na'] = math.ceil( (demand['na'] - (config['web_NA_total'] * 180))/180 )
    change_of_servers['w_eu'] = math.ceil( (demand['eu'] - (config['web_EU_total'] * 180))/180 )
    change_of_servers['w_ap'] = math.ceil( (demand['ap'] - (config['web_AP_total'] * 180))/180 )


    return java_tier(config,change_of_servers,demand)

def java_tier(config, change_of_servers, demand):

    #we don't want EU to have any java servers
    #update demand to match new servers
    demand['na'] = demand['na'] + (demand['eu']*0.9)
    demand['eu'] = 0
    #this function calculates the demand per server and rounds up.  This will become the change for our servers
    change_of_servers['j_na'] = math.ceil( (demand['na'] - (config['java_NA_total']+change_of_servers['w_na']+change_of_servers['w_eu']) * 400)/400)
    #remove EU servers if they exist
    change_of_servers['j_eu'] = -config['java_EU_total']
    change_of_servers['j_ap'] = math.ceil( (demand['ap'] - (config['java_AP_total']+change_of_servers['w_ap']) * 400)/400)



    return database_tier(config, change_of_servers, demand)

def database_tier(config, change_of_servers, demand):
    print 'config %s' % config
    print 'change_of_servers %s' % change_of_servers
    print 'demand %s' % demand

    demand['na'] = demand['na'] + (demand['ap']*0.9)
    demand['ap'] = 0

    change_of_servers['d_na'] = math.ceil( (demand['na'] - (config['db_NA_total']+change_of_servers['j_na'] + change_of_servers['j_ap']) * 1000)/1000)
    change_of_servers['d_eu'] = int(-config['db_EU_total'])
    change_of_servers['d_ap'] = int(-config['db_AP_total'])

    return change_of_servers
