from hermes_client.client import Hermes_Client

client = Hermes_Client()
while True:
    message = client.next_turn()

print message
