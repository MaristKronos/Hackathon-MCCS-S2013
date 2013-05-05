class demand Algorithm

    import math
        
    #this provides a weighted average for the current demand and the predicted demand of our algorithms
    def demand(self, hist, w1, demand, w2, roc, w3)

        self.na_demand = (hist['na']*w1+demand['na']*w2+roc['na']*w3
        self.eu_demand = (hist['eu']*w1+demand['eu']*w2+roc['eu']*w3
        self.ap_demand = (hist['ap']*w1+demand['ap']*w2+roc['ap']*w3
    
        return {
            "na":self.na_demand,
            "eu":self.eu_demand,
            "ap":self.ap_demand
            }
    
    def web_tier(self, config, demand):
        
        change_of_servers['w_na'] = math.ceil( (demand['na'] - (config['web_NA_total'] * 180))/180 )
        change_of_servers['w_eu'] = math.ceil( (demand['eu'] - (config['web_EU_total'] * 180))/180 )
        change_of_servers['w_ap'] = math.ceil( (demand['ap'] - (config['web_AP_total'] * 180))/180 )
        
                
        return java_tier(config,change_of_servers,demand)
            
    def java_tier(self, config, change_of_servers, demand):
    
        #we don't want EU to have any java servers
        #update demand to match new servers
        demand['na'] = demand['na'] + (demand['eu']*0.9)
        demand['eu'] = 0
        #this function calculates the demand per server and rounds up.  This will become the change for our servers
        change_of_servers['j_na'] = math.ceil( (demand['na']-( (config['java_NA_total']+change_of_servers['w_na']) * 400) ) /400 )
        #remove EU servers if they exist
        change_of_servers['j_eu'] = -config['java_EU_total']
        change_of_servers['j_ap'] = math.ceil( (demand['ap']-( (config['java_AP_total']+change_of_servers['w_ap']) * 400) ) /400 )
        
        
        
        return database_tier(config, change_of_servers, demand)
        
    def database_tier(self, config, change_of_servers, demand):
        
        demand['na'] = demand['na'] + (demand['ap']*0.9)
        demand['ap'] = 0
        
        change_of_servers['d_na'] = math.ceil( (demand[na]-(config['db_NA_total']+change_of_servers['j_na'] * 1000))/1000 )
        change_of_servers['d_eu'] = -config['db_EU_total']
        change_of_servers['d_ap'] = -config['db_AP_total']
        
        return change_of_servers

    
        
    