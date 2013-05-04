CONNECTION_TUPLES = [
    ('hackathon.hopto.org', 31671),
    ('hackathon.hopto.org', 31672)
]

TEAM = 'Kronos'

INIT = 'INIT'
RECEIVE = 'RECD'
START = 'START'
STOP = 'STOP'
BEGIN = '%(init)s %(team)s' % {'init': INIT, 'team': TEAM}
CONTROL = '%(w_na)s %(w_eu)s %(w_ap)s %(j_na)s %(j_eu)s %(j_ap)s %(d_na)s %(d_eu)s %(d_ap)s' 

MAX_PACKET_SIZE = 999
