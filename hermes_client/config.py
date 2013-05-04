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

MAX_PACKET_SIZE = 999
