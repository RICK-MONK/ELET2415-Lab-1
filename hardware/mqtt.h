#ifndef MQTT_H
#define MQTT_H

#include <WiFi.h>
#include <PubSubClient.h>

// --------------------------------------------------------------------------
// EXTERNAL VARIABLES (Brings in variables defined in hardware.ino)
// --------------------------------------------------------------------------
extern const char* ssid;
extern const char* password;
extern const char* mqtt_server;
extern uint16_t mqtt_port;
extern const char* pubtopic;
extern const char* subtopic[]; 
extern TaskHandle_t xMQTT_Connect; // Task handle from hardware.ino

// Function Prototype for the callback in hardware.ino
extern void callback(char* topic, byte* payload, unsigned int length);

// --------------------------------------------------------------------------
// OBJECTS
// --------------------------------------------------------------------------
WiFiClient espClient;
PubSubClient client(espClient);

// --------------------------------------------------------------------------
// HELPER FUNCTIONS
// --------------------------------------------------------------------------

// 1. Connect to Wi-Fi
void setup_wifi() {
  delay(10);
  Serial.println();
  Serial.print("Connecting to ");
  Serial.println(ssid);

  WiFi.mode(WIFI_STA); // Station Mode
  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  Serial.println("");
  Serial.println("WiFi connected");
  Serial.print("IP address: ");
  Serial.println(WiFi.localIP());
}

// 2. Reconnect to MQTT Broker
void reconnect() {
  // Loop until we're reconnected
  while (!client.connected()) {
    Serial.print("Attempting MQTT connection...");
    
    // Create a random client ID
    String clientId = "ESP32Client-";
    clientId += String(random(0xffff), HEX);
    
    // Attempt to connect
    if (client.connect(clientId.c_str())) {
      Serial.println("connected");
      
      // Once connected, subscribe to ALL topics in the subtopic array
      // Note: We assume the array has 2 elements based on your hardware.ino
      // {"620169874_sub", "/elet2415"}
      client.subscribe(subtopic[0]);
      client.subscribe(subtopic[1]);
      
      Serial.print("Subscribed to: ");
      Serial.print(subtopic[0]);
      Serial.print(" and ");
      Serial.println(subtopic[1]);
      
    } else {
      Serial.print("failed, rc=");
      Serial.print(client.state());
      Serial.println(" try again in 5 seconds");
      // Wait 5 seconds before retrying
      delay(5000);
    }
  }
}

// --------------------------------------------------------------------------
// MQTT TASK (Handles the Loop)
// --------------------------------------------------------------------------
void vMQTT(void * pvParameters) {
  // Ensure WiFi is connected before starting
  setup_wifi();
  
  // Set Server and Callback
  client.setServer(mqtt_server, mqtt_port);
  client.setCallback(callback);

  // Infinite Task Loop
  for (;;) {
    if (!client.connected()) {
      reconnect();
    }
    client.loop(); // CRITICAL: This checks for incoming messages
    
    // Small delay to prevent watchdog timer triggers and allow other tasks to run
    vTaskDelay(10 / portTICK_PERIOD_MS); 
  }
}

// --------------------------------------------------------------------------
// MAIN FUNCTIONS (Called from hardware.ino)
// --------------------------------------------------------------------------

void initMQTT(void) {
  // Create the FreeRTOS task to handle MQTT
  // This prevents the main loop from being blocked
  xTaskCreatePinnedToCore(
    vMQTT,              // Task function
    "MQTT_Task",        // Name
    4096,               // Stack size (increased for safety)
    NULL,               // Parameters
    1,                  // Priority
    &xMQTT_Connect,     // Handle
    1                   // Core
  );
}

// Wrapper to publish messages easily
bool publish(const char *topic, const char *payload) {
  if (client.connected()) {
    return client.publish(topic, payload);
  }
  return false;
}

#endif