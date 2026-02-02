import os
from dotenv import load_dotenv

# Try to load the .env file
load_dotenv()

class Config:
    # 1. Database Credentials
    DB_USERNAME = os.environ.get('DB_USERNAME', 'root')
    DB_PASSWORD = os.environ.get('DB_PASSWORD', 'pass')
    
    # 2. Database Connection
    DB_SERVER   = os.environ.get('DB_HOST', 'localhost') 
    DB_PORT     = os.environ.get('DB_PORT', '27017')
    DB_NAME     = os.environ.get('DB_NAME', 'ELET2415')
    
    # 3. MQTT Configuration
    BROKER_URL  = os.environ.get('BROKER_URL', 'localhost')
    BROKER_PORT = int(os.environ.get('BROKER_PORT', 1883))
    
    # 4. Flask Server Configuration (New!)
    FLASK_DEBUG    = True
    FLASK_RUN_HOST = "0.0.0.0"
    FLASK_RUN_PORT = 8080

    # Debug: Confirm it is working
    print(f"Config Loaded - Username: {DB_USERNAME}")
    
    #other settings
    MQTT_BROKER_URL     = "localhost"
    MQTT_BROKER_PORT    = 1883
    # Ensure this matches your ID
    MQTT_TOPIC          = "620169874"

    