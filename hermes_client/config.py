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
CONTROL = 'CONTROL %(w_na)i %(w_eu)i %(w_ap)i %(j_na)i %(j_eu)i %(j_ap)i %(d_na)i %(d_eu)i %(d_ap)i'

MAX_PACKET_SIZE = 999
