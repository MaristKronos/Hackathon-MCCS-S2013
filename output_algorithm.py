class demand Algorithm

	def na_demand(self, w, w2, h, roc,demand):
		self.weight1 = w
		self.weight2 = w2
		self.hist_dem_na = h
		self.roc_na = roc
		
		demand = ((hist_dem_na * weight1) + (roc_na * weight2))/(weight1+weight2)
		
		return demand
		
	def demand(self, hist, w1, demand, w2, roc, w3)

		self.na_demand = (hist['na']*w1+demand['na']*w2+roc['na']*w3
		self.eu_demand = (hist['eu']*w1+demand['eu']*w2+roc['eu']*w3
		self.ap_demand = (hist['ap']*w1+demand['ap']*w2+roc['ap']*w3
	
		return {
			"na":self.na_demand,
			"eu":self.eu_demand,
			"ap":self.ap_demand
			}
	
	def web_tier(self, config, c, d):
		
		self.current_config = config
		
		self.number_of_servers = {}
		self.change = c
		self.demand = d
		
		self.change_of_servers['w_na'] = demand['na'] - ((number_of_servers * 180)/180)
		self.change_of_servers['w_eu'] = demand['eu'] - ((number_of_servers * 180)/180)
		self.change_of_servers['w_ap'] = demand['ap'] - ((number_of_servers * 180)/180)
		
				
		return java_tier(config,self.change_of_servers)
	def java_tier(self, config, cos):
		self.current_config = config
		self.number_of_servers = {}
		
		self.change_of_servers['j_na'] = -((number_of_servers * 400)/400)
		self.change_of_servers['j_eu'] = -((number_of_servers * 400)/400)
		self.change_of_servers['j_ap'] = -((number_of_servers * 400)/400)
		
	    return database_tier(config,
	def database_tier(self, config, cos):
		self.current_config = config
		self.number_of_servers = {}
		
		self.change_of_servers['d_na'] = -((number_of_servers * 1000)/1000)
		self.change_of_servers['d_eu'] = -((number_of_servers * 1000)/1000)
		self.change_of_servers['d_ap'] = -((number_of_servers * 1000)/1000)
		
		return cos
				
					
			
	if web_efficiency > 100
				change = number_of_servers - 1
			elif web_efficiency < 80
				number_of_servers = number_of_servers + 1
			elif
			
				return current_config - change
	
		
	