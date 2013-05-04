from hermes_client.client import Hermes_Client

client = Hermes_Client()
i = 0
while client.next_turn():
    i += 1
    message = client.get_stats()
    if not i % 60:
        print message
