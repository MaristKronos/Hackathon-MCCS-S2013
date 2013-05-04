from hermes_client.client import Hermes_Client

client = Hermes_Client()
i = 0
while True:
    i += 1
    if i == 60:
        print client.next_turn()
        i = 0
    else:
        client.next_turn()
