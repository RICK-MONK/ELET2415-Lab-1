import paho.mqtt.client as mqtt
from random import randint
from json import dumps, loads
from time import sleep
from .config import Config

class MQTT:
    ID = f"IOT_B_{randint(1,1000000)}"
    sub_topics = [] 

    def __init__(self, mongo):
        self.randint = randint
        self.loads = loads
        self.dumps = dumps
        self.sleep = sleep
        self.mongo = mongo
        self.client = mqtt.Client(client_id=self.ID, clean_session=True, reconnect_on_failure=True)
        self.client.on_connect = self.on_connect
        self.client.on_message = self.on_message
        self.client.on_disconnect = self.on_disconnect
        self.client.on_subscribe = self.on_subscribe

        # DYNAMICALLY CREATE TOPICS
        self.topic = Config.MQTT_TOPIC
        self.sub_topics = [(f"{self.topic}_pub", 0), (self.topic, 0), (f"{self.topic}_sub", 0)]

        # REGISTER CALLBACK FUNCTION FOR EACH TOPIC
        self.client.message_callback_add(self.topic, self.gdp)
        self.client.message_callback_add(f"{self.topic}_sub", self.toggle)

        # FIX 1: CONNECT TO THE ACTUAL SERVER, NOT LOCALHOST
        self.client.connect_async("www.yanacreations.com", 1883, 60)

    def connack_string(self, rc):
        connection = {0: "Connection successful", 1: "Connection refused - incorrect protocol version", 2: "Connection refused - invalid client identifier", 3: "Connection refused - server unavailable", 4: "Connection refused - bad username or password", 5: "Connection refused - not authorised" }
        return connection[rc]

    def on_connect(self, client, userdata, flags, rc):
        print("\n\nMQTT: "+ self.connack_string(rc)," ID: ", client._client_id.decode('utf-8'))
        client.subscribe(self.sub_topics) 

    def on_subscribe(self, client, userdata, mid, granted_qos): 
        print("MQTT: Subscribed to", [topic[0] for topic in self.sub_topics])

    def on_message(self, client, userdata, msg):
        # This is a fallback; specific topics are handled by gdp and toggle
        pass

    def gdp(self, client, userdata, msg):
        '''Handles data from ESP32 and saves to MongoDB'''
        try:
            payload = msg.payload.decode("utf-8")
            print(f"MQTT Received Data: {payload}") # Important for debugging
            
            update = loads(payload) 
            # FIX 2: Use self.mongo to access the database class
            self.mongo.addUpdate(update) 
            print("MQTT: Successfully saved to MongoDB")

        except Exception as e:
            print(f"MQTT: GDP Error (Save failed): {str(e)}")

    def toggle(self, client, userdata, msg):    
        try:
            payload = msg.payload.decode("utf-8")
            print(f"MQTT Received Control: {payload}")
        except Exception as e:
            print(f"MQTT: toggle Error - {str(e)}")

    def on_disconnect(self, client, userdata, rc):
        if rc != 0:
            print("MQTT: Unexpected Disconnection.")