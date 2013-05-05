from hermes_client.client import Hermes_Client
import algorithm
from statsd_client.statsd_client import Statsd
import time

x = Statsd()

client = Hermes_Client()
i = 0
while client.next_turn():
    i += 1
    message = client.get_stats()
    x.update_value('demand.na', message[0]['trades_NA'], int(time.time()))
    x.update_value('demand.eu', message[0]['trades_EU'], int(time.time()))
    x.update_value('demand.ap', message[0]['trades_AP'], int(time.time()))

    x.update_value('servers.web.na', message[1]['NA_web'], int(time.time()))
    x.update_value('servers.web.eu', message[1]['EU_web'], int(time.time()))
    x.update_value('servers.web.ap', message[1]['AP_web'], int(time.time()))
    x.update_value('servers.java.na', message[1]['NA_java'], int(time.time()))
    x.update_value('servers.java.eu', message[1]['EU_java'], int(time.time()))
    x.update_value('servers.java.ap', message[1]['AP_java'], int(time.time()))
    x.update_value('servers.db.na', message[1]['NA_db'], int(time.time()))
    x.update_value('servers.db.eu', message[1]['EU_db'], int(time.time()))
    x.update_value('servers.db.ap', message[1]['AP_db'], int(time.time()))

    x.update_value('profit.total', message[2]['total_profit'], int(time.time()))
    x.update_value('profit.total_last_period', message[2]['profit_last_period'], int(time.time()))
    x.update_value('profit.total_percentage', message[2]['actual/max_profit_%_total'], int(time.time()))
    x.update_value('profit.percentage', message[2]['actual/max_profit_%_last period'], int(time.time()))
    x.update_value('profit.total', message[2]['total_profit'], int(time.time()))

    try:
        x.update_value('load.web.na',  message[1]['NA_web'] / client._config['web_NA_total'], int(time.time()))
    except ZeroDivisionError:
        pass
    try:
        x.update_value('load.web.eu', message[1]['EU_web'] / client._config['web_EU_total'], int(time.time()))
    except ZeroDivisionError:
        pass
    try:
        x.update_value('load.web.ap', message[1]['AP_web'] / client._config['web_AP_total'], int(time.time()))
    except ZeroDivisionError:
        pass
    try:
        x.update_value('load.java.na', message[1]['NA_java'] / client._config['java_NA_total'], int(time.time()))
    except ZeroDivisionError:
        pass
    try:
        x.update_value('load.java.eu', message[1]['EU_java'] / client._config['java_EU_total'], int(time.time()))
    except ZeroDivisionError:
        pass
    try:
        x.update_value('load.java.ap', message[1]['AP_java'] / client._config['java_AP_total'], int(time.time()))
    except ZeroDivisionError:
        pass
    try:
        x.update_value('load.db.na', message[1]['NA_db'] / client._config['db_NA_total'], int(time.time()))
    except ZeroDivisionError:
        pass
    try:
        x.update_value('load.db.eu', message[1]['EU_db'] / client._config['db_EU_total'], int(time.time()))
    except ZeroDivisionError:
        pass
    try:
        x.update_value('load.db.ap', message[1]['AP_db'] / client._config['db_AP_total'], int(time.time()))
    except ZeroDivisionError:
        pass

print '\n\n\n'
print '-----------------------------'
print '------TOTAL------------------'
print client._store_the_internet[-1]
print client._config
print '-----------------------------'
