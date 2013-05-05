from hermes_client.client import Hermes_Client
import algorithm

client = Hermes_Client()
i = 0
while client.next_turn():
    i += 1
    message = client.get_stats()
    if int(message[2]['actual/max_profit_%_last period']) < -10:
        print '-----------------------------'
        print 'Lost %s percent profit this period' % (message[2]['actual/max_profit_%_last period'])
        print client._store_the_internet[-1]
        print client._config
        print 'mike: %s' % algorithm.handle_data_input(client._store_the_internet)
        print 't3kk: %s' % client.hist_predict()
        print '-----------------------------'

print '\n\n\n'
print '-----------------------------'
print '------TOTAL------------------'
print client._store_the_internet[-1]
print client._config
print '-----------------------------'
