#! /usr/bin/env python
from app import app, Config, Mqtt
from flask_cors import CORS 

# Apply CORS to the imported app from your project
CORS(app) 

# DELETE THIS LINE: app = Flask(__name__) 

if __name__ == "__main__":   

    # START MQTT CLIENT 
    Mqtt.client.loop_start()

    # RUN FLASK APP
    app.run(debug=Config.FLASK_DEBUG, host=Config.FLASK_RUN_HOST, port=Config.FLASK_RUN_PORT)