// Libraries
#include <Adafruit_Sensor.h>
#include <DHT.h>
#include <DHT_U.h>
#include <Wire.h>
#include <BH1750.h>
#include <WiFi.h>

// Pins
#define DHTPIN 15 // Digital pin connected to the DHT sensor
#define DHTTYPE DHT11 // DHT 11
#define REDPIN 2
#define GREENPIN 4

// Variables
DHT_Unified dht(DHTPIN, DHTTYPE);
uint32_t delayMS;
const char *ssid = "ESP32-Node";
const char *password = "12345678";

// WiFi settings
IPAddress serverIP(192, 168, 4, 1); // Fixed IP of ESP1
const uint16_t serverPort = 80;
WiFiClient client;

void setup()
{
  Serial.begin(9600);
  pinMode(REDPIN,OUTPUT);
  pinMode(GREENPIN,OUTPUT);
  
  //Initialize pins
  digitalWrite(REDPIN, HIGH);
  digitalWrite(GREENPIN, LOW);
  
  // Initialize device.
  dht.begin();
  Serial.println(F("DHTxx Unified Sensor Example"));
  
  // Print temperature sensor details.
  sensor_t sensor;
  dht.temperature().getSensor(&sensor);
  Serial.println(F("------------------------------------"));
  Serial.println(F("Temperature Sensor"));
  Serial.print(F("Sensor Type: "));
  Serial.println(sensor.name);
  Serial.print(F("Driver Ver:  "));
  Serial.println(sensor.version);
  Serial.print(F("Unique ID:   "));
  Serial.println(sensor.sensor_id);
  Serial.print(F("Max Value:   "));
  Serial.print(sensor.max_value);
  Serial.println(F("°C"));
  Serial.print(F("Min Value:   "));
  Serial.print(sensor.min_value);
  Serial.println(F("°C"));
  Serial.print(F("Resolution:  "));
  Serial.print(sensor.resolution);
  Serial.println(F("°C"));
  Serial.println(F("------------------------------------"));
  
  // Print humidity sensor details.
  dht.humidity().getSensor(&sensor);
  Serial.println(F("Humidity Sensor"));
  Serial.print(F("Sensor Type: "));
  Serial.println(sensor.name);
  Serial.print(F("Driver Ver:  "));
  Serial.println(sensor.version);
  Serial.print(F("Unique ID:   "));
  Serial.println(sensor.sensor_id);
  Serial.print(F("Max Value:   "));
  Serial.print(sensor.max_value);
  Serial.println(F("%"));
  Serial.print(F("Min Value:   "));
  Serial.print(sensor.min_value);
  Serial.println(F("%"));
  Serial.print(F("Resolution:  "));
  Serial.print(sensor.resolution);
  Serial.println(F("%"));
  Serial.println(F("------------------------------------"));
  
  // Set delay between sensor readings based on sensor details.
  delayMS = sensor.min_delay / 1000;

  // Connect to WiFi network
  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);
  Serial.print("Connecting To WiFi Network .");
  while (WiFi.status() != WL_CONNECTED)
  {
    Serial.print(".");
    delay(100);
  }
  Serial.println("\nConnected To The WiFi Network");
  Serial.print("Local ESP32 IP: ");
  Serial.println(WiFi.localIP());
}

void loop()
{
    // Check if the WiFi connection is still active
  if (!client.connected())
  {
    if (!client.connect(serverIP, serverPort)){
      digitalWrite(REDPIN, HIGH);
      digitalWrite(GREENPIN, LOW);
      Serial.println("Connection to server failed");
      delay(500); // Wait before retrying
      return;
    }
  }else{
      digitalWrite(REDPIN, LOW);
      digitalWrite(GREENPIN, HIGH);
  }

  // Wait for a request from the server
  if (client.available())
  {
    String request = client.readStringUntil('\n');
    Serial.println("Received request: " + request);

    // Respond to the server's request
    request.trim();
    if (request == "REQUEST DATA")
    {
      // Get temperature event and print its value.
      sensors_event_t tempEvent;
      dht.temperature().getEvent(&tempEvent);
      sensors_event_t humidityEvent;
      dht.humidity().getEvent(&humidityEvent);
      if (isnan(tempEvent.temperature))
      {
        Serial.println(F("Error reading temperature!"));
      }
      
      // Get humidity event and print its value.
      if (isnan(humidityEvent.relative_humidity))
      {
        Serial.println(F("Error reading humidity!"));
      }
      
      // Send response to the server
      String jsonResponse = "{\"temperature\": " + String(tempEvent.temperature) + ", \"humidity\": " + String(humidityEvent.relative_humidity) + "}";
      client.println(jsonResponse);
      Serial.println("Sent response: " + jsonResponse);
      client.stop();
    }
  }
}