"""
Flask Documentation:     https://flask.palletsprojects.com/
Jinja2 Documentation:    https://jinja.palletsprojects.com/
Werkzeug Documentation:  https://werkzeug.palletsprojects.com/
This file creates your application.
"""

# from crypt import methods
import site 

from app import app, Config,  mongo, Mqtt
from flask import escape, render_template, request, jsonify, send_file, redirect, make_response, send_from_directory 
from json import dumps, loads 
from werkzeug.utils import secure_filename
from datetime import datetime,timedelta, timezone
from os import getcwd
from os.path import join, exists
from time import time, ctime
from math import floor
 


#####################################
#   Routing for your application    #
#####################################

@app.route('/api/numberfrequency', methods=['GET'])
def get_numberFrequency():
    '''Returns list of frequency distribution for generated numbers'''
    if request.method == "GET":
        try:
            frequency = mongo.numberFrequency()
            if frequency:
                return jsonify({"status": "found", "data": frequency})
            return jsonify({"status": "failed", "data": []})
        except Exception as e:
            print(f"get_numberFrequency error: {str(e)}")
            return jsonify({"status": "failed", "data": []})

@app.route('/api/oncount', defaults={'ledName': None}, methods=['GET'])
@app.route('/api/oncount/<ledName>', methods=['GET'])
def get_onCount(ledName):
    '''
    HYBRID ROUTE: Handles both frontend query strings and direct path params.
    Matches: /api/oncount?LED_Name=ledA  OR  /api/oncount/ledA
    '''
    if request.method == "GET":
        try:
            # 1. Try to get name from the Query String (what your Vue app is sending)
            name = request.args.get('LED_Name')
            
            # 2. If not in query, use the path variable
            if not name:
                name = ledName
            
            if not name:
                return jsonify({"status": "failed", "message": "Missing LED name"}), 400

            # 3. Query Database
            LED_Name = escape(name)
            count = mongo.onCount(LED_Name)
            
            if count is not None:
                return jsonify({"status": "found", "data": count})
            return jsonify({"status": "failed", "data": 0})
        except Exception as e:
            print(f"get_onCount error: {str(e)}")
            return jsonify({"status": "failed", "data": 0})

@app.route('/api/mmnt/all', methods=['GET'])
def get_all_mmnt():
    '''Returns all raw documents from the MongoDB update collection'''
    if request.method == "GET":
        try:
            data = mongo.getAllUpdates() 
            return jsonify(data) if data else jsonify([])
        except Exception as e:
            print(f"get_all_mmnt error: {str(e)}")
            return jsonify([])

@app.route('/api/mmnt/control', methods=['POST'])
def control_device():
    '''Sends a command to the ESP32 via the MQTT Broker'''
    if request.method == "POST":
        try:
            data = request.get_json()
            payload = dumps(data)
            # Sends to your specific subscription topic
            Mqtt.client.publish("620169874_sub", payload)
            return jsonify({"status": "success", "message": "Command published"})
        except Exception as e:
            print(f"control_device error: {str(e)}")
            return jsonify({"status": "failed", "message": str(e)})

@app.route('/api/file/upload', methods=["POST"])  
def upload():
    '''Saves files to the uploads folder'''
    if request.method == "POST": 
        file = request.files['file']
        filename = secure_filename(file.filename)
        file.save(join(getcwd(), Config.UPLOADS_FOLDER, filename))
        return jsonify({"status": "File upload successful", "filename": filename})