//##################################################################################################################
//##                                      ELET2415 DATA ACQUISITION SYSTEM CODE                                   ##
//## Patrick Marsden                                                                                                              ##
//##################################################################################################################

// LIBRARY IMPORTS
#include <rom/rtc.h>

#ifndef _WIFI_H 
#include <WiFi.h>
#endif

#ifndef STDLIB_H
#include <stdlib.h>
#endif

#ifndef STDIO_H
#include <stdio.h>
#endif

#ifndef ARDUINO_H
#include <Arduino.h>
#endif 
 
#ifndef ARDUINOJSON_H
#include <ArduinoJson.h>
#endif

// DEFINE VARIABLES
#define ARDUINOJSON_USE_DOUBLE      1 
// DEFINE THE PINS THAT WILL BE MAPPED TO THE 7 SEG DISPLAY BELOW, 'a' to 'g'
#define a   15
#define b   32
#define c   33
#define d   25
#define e   26
#define f   27
#define g   14
#define dp  12  // Decimal Point (Optional based on your specific display)



// DEFINE VARIABLES FOR TWO LEDs AND TWO BUTTONs. LED_A, LED_B, BTN_A , BTN_B
#define LED_A 4
#define LED_B 5   
#define BTN_A 18
/* Complete all others */



// MQTT CLIENT CONFIG  
static const char* pubtopic       = "620169874";                    // Add your ID number here
static const char* subtopic[]     = {"620169874_sub","/elet2415"};  // Array of Topics(Strings) to subscribe to
static const char* mqtt_server    = "192.168.0.6";                // Broker IP address or Domain name as a String 
static uint16_t mqtt_port         = 1883;

// WIFI CREDENTIALS
const char* ssid                  = "ARRIS-ED5E-5G"; // Add your Wi-Fi ssid
const char* password              = "70DFF79FED5E"; // Add your Wi-Fi password 




// TASK HANDLES 
TaskHandle_t xMQTT_Connect          = NULL; 
TaskHandle_t xNTPHandle             = NULL;  
TaskHandle_t xLOOPHandle            = NULL;  
TaskHandle_t xUpdateHandle          = NULL;
TaskHandle_t xButtonCheckeHandle    = NULL; 

// FUNCTION DECLARATION   
void checkHEAP(const char* Name);   // RETURN REMAINING HEAP SIZE FOR A TASK
void initMQTT(void);                // CONFIG AND INITIALIZE MQTT PROTOCOL
unsigned long getTimeStamp(void);   // GET 10 DIGIT TIMESTAMP FOR CURRENT TIME
void callback(char* topic, byte* payload, unsigned int length);
void initialize(void);
bool publish(const char *topic, const char *payload); // PUBLISH MQTT MESSAGE(PAYLOAD) TO A TOPIC
void vButtonCheck( void * pvParameters );
void vUpdate( void * pvParameters ); 
void GDP(void);   // GENERATE DISPLAY PUBLISH

/* Declare your functions below */
void Display(unsigned char number);
int8_t getLEDStatus(int8_t LED);
void setLEDState(int8_t LED, int8_t state);
void toggleLED(int8_t LED);
  

//############### IMPORT HEADER FILES ##################
#ifndef NTP_H
#include "NTP.h"
#endif

#ifndef MQTT_H
#include "mqtt.h"
#endif

// Temporary Variables
uint8_t number = 0;


void setup() {
  Serial.begin(115200);  // INIT SERIAL  

  // CONFIGURE THE ARDUINO PINS OF THE 7SEG AS OUTPUT
  pinMode(a,OUTPUT);
  pinMode(b, OUTPUT);
  pinMode(c, OUTPUT);
  pinMode(d, OUTPUT);
  pinMode(e, OUTPUT);
  pinMode(f, OUTPUT);
  pinMode(g, OUTPUT);
  pinMode(dp, OUTPUT);
  
  // Configure LEDs
  pinMode(LED_A, OUTPUT);
  pinMode(LED_B, OUTPUT);

  // Configure Button (Important: Use INPUT_PULLUP)
  pinMode(BTN_A, INPUT_PULLUP);

  // --- MISSING REQUIREMENT ADDED HERE ---
  Display(8); // Display 8 on start-up 
  // --------------------------------------

  initialize();
  
  xTaskCreate(vButtonCheck, "Button Check", 2048, NULL, 1, NULL);
}
  


void loop() {
    // put your main code here, to run repeatedly: 
    
}




  
//####################################################################
//#                          UTIL FUNCTIONS                          #
//####################################################################

// CHECK FOR AND ACTION BUTTON EVENTS (FreeRTOS Task)
void vButtonCheck(void * pvParameters) {
  int lastState = HIGH; // Input Pullup defaults HIGH (Not Pressed)
  int currentState;

  // Infinite loop required for FreeRTOS tasks
  for (;;) {
    currentState = digitalRead(BTN_A);

    // Check for falling edge (High -> Low transition = Press)
    if (lastState == HIGH && currentState == LOW) {
      Serial.println("Button Pressed");
      
      // Perform the main task: Generate, Display, Publish
      GDP(); 
      
      // Simple debounce delay (200ms)
      vTaskDelay(200 / portTICK_PERIOD_MS); 
    }
    
    lastState = currentState;
    
    // Yield to other tasks (essential for stability)
    vTaskDelay(50 / portTICK_PERIOD_MS); 
  }
}

// WRITE INTEGER TO 7-SEG DISPLAY
void Display(unsigned char number){
  // Reset all segments (Turn OFF) - Common Cathode means LOW is OFF
  digitalWrite(a, LOW); digitalWrite(b, LOW); digitalWrite(c, LOW);
  digitalWrite(d, LOW); digitalWrite(e, LOW); digitalWrite(f, LOW);
  digitalWrite(g, LOW);

  // Turn ON specific segments for numbers 0-9
  switch (number) {
    case 0: digitalWrite(a, HIGH); digitalWrite(b, HIGH); digitalWrite(c, HIGH); digitalWrite(d, HIGH); digitalWrite(e, HIGH); digitalWrite(f, HIGH); break;
    case 1: digitalWrite(b, HIGH); digitalWrite(c, HIGH); break;
    case 2: digitalWrite(a, HIGH); digitalWrite(b, HIGH); digitalWrite(d, HIGH); digitalWrite(e, HIGH); digitalWrite(g, HIGH); break;
    case 3: digitalWrite(a, HIGH); digitalWrite(b, HIGH); digitalWrite(c, HIGH); digitalWrite(d, HIGH); digitalWrite(g, HIGH); break;
    case 4: digitalWrite(b, HIGH); digitalWrite(c, HIGH); digitalWrite(f, HIGH); digitalWrite(g, HIGH); break;
    case 5: digitalWrite(a, HIGH); digitalWrite(c, HIGH); digitalWrite(d, HIGH); digitalWrite(f, HIGH); digitalWrite(g, HIGH); break;
    case 6: digitalWrite(a, HIGH); digitalWrite(c, HIGH); digitalWrite(d, HIGH); digitalWrite(e, HIGH); digitalWrite(f, HIGH); digitalWrite(g, HIGH); break;
    case 7: digitalWrite(a, HIGH); digitalWrite(b, HIGH); digitalWrite(c, HIGH); break;
    case 8: digitalWrite(a, HIGH); digitalWrite(b, HIGH); digitalWrite(c, HIGH); digitalWrite(d, HIGH); digitalWrite(e, HIGH); digitalWrite(f, HIGH); digitalWrite(g, HIGH); break;
    case 9: digitalWrite(a, HIGH); digitalWrite(b, HIGH); digitalWrite(c, HIGH); digitalWrite(d, HIGH); digitalWrite(f, HIGH); digitalWrite(g, HIGH); break;
  }
}

// RETURNS THE STATE OF A SPECIFIC LED. 0 = LOW, 1 = HIGH
int8_t getLEDStatus(int8_t LED) {
  return digitalRead(LED);
}

// SETS THE STATE OF A SPECIFIC LED
void setLEDState(int8_t LED, int8_t state){
  digitalWrite(LED, state);
}

// TOGGLES THE STATE OF SPECIFIC LED
void toggleLED(int8_t LED){
  int state = digitalRead(LED);
  digitalWrite(LED, !state); // Write the opposite of the current state
}

// GENERATE, DISPLAY THEN PUBLISH INTEGER
void GDP(void){
  // 1. Generate Random Number [0-9]
  // Fix: Use the global 'number' variable defined at the top of your file
  number = random(0, 10); 

  // 2. Display the number
  Display(number);

  // 3. Get Time
  // Use the helper function defined in your template
  unsigned long timestamp = getTimeStamp();
  if (timestamp == 0) {
      Serial.println("Failed to obtain time");
  }

  // 4. Construct JSON Payload
  // Schema: {"id": "student_id", "timestamp": 12345678, "number": 9, "ledA": 0, "ledB": 0}
  JsonDocument doc; // Uses ArduinoJson v7
  
  doc["id"] = "620169874";          // Student ID
  doc["timestamp"] = timestamp;
  doc["number"] = number;           // Use global 'number' variable
  doc["ledA"] = getLEDStatus(LED_A);
  doc["ledB"] = getLEDStatus(LED_B);

  // Serialize to string
  char buffer[256];
  serializeJson(doc, buffer);

  // 5. Publish to MQTT Topic
  // FIX: Use 'pubtopic' (defined at top of template) and 'publish' wrapper function
  publish(pubtopic, buffer); 
}

// Definition for initialize to ensure MQTT setup is called
void initialize(void) {
  initMQTT(); 
}

// ############## MQTT CALLBACK ######################################
void callback(char* topic, byte* payload, unsigned int length) {
  // RUNS WHENEVER A MESSAGE IS RECEIVED ON A TOPIC SUBSCRIBED TO
  
  Serial.printf("\nMessage received : ( topic: %s ) \n", topic);
  
  // Create a char array to hold the message
  char *received = new char[length + 1] {0};
  
  for (int i = 0; i < length; i++) {
    received[i] = (char)payload[i];
  }

  // PRINT RECEIVED MESSAGE
  Serial.printf("Payload : %s \n", received);

  // 1. CONVERT MESSAGE TO JSON
  JsonDocument doc; // Uses ArduinoJson v7
  DeserializationError error = deserializeJson(doc, received);

  if (error) {
    Serial.print(F("deserializeJson() failed: "));
    Serial.println(error.f_str());
    delete[] received; // Free memory before returning
    return;
  }

  // 2. PROCESS MESSAGE
  // Expected Schema: {"type": "toggle", "device": "LED A"}
  const char* type = doc["type"];
  const char* device = doc["device"];

  // Check if the message type is "toggle"
  if (strcmp(type, "toggle") == 0) {
    
    // Check which device to toggle
    if (strcmp(device, "LED A") == 0) {
      toggleLED(LED_A);
      Serial.println("Toggled LED A");
    } 
    else if (strcmp(device, "LED B") == 0) {
      toggleLED(LED_B);
      Serial.println("Toggled LED B");
    }
  }

  // Free memory allocated for the received string
  delete[] received;
}
