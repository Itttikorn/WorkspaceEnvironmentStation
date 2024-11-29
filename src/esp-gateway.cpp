// Libraries
#include <Adafruit_Sensor.h>
#include <DHT.h>
#include <DHT_U.h>
#include <Wire.h>
#include <BH1750.h>
#include <WiFi.h>
#include <ArduinoJson.h>
#include <WiFiClientSecure.h>
#include <HTTPClient.h>
#include "time.h"

//Variables
const char *ssid = "Ittikorn-Hotspot";
const char *password = "itttikorn";
const char *node_ssid = "ESP32-Node";
const char *node_password = "12345678";
const char* test_root_ca= \
"-----BEGIN CERTIFICATE-----\n" \
"MIIE9DCCA9ygAwIBAgISBI3j+dtt+Tr0GQBojjIzGiiaMA0GCSqGSIb3DQEBCwUA\n" \
"MDMxCzAJBgNVBAYTAlVTMRYwFAYDVQQKEw1MZXQncyBFbmNyeXB0MQwwCgYDVQQD\n" \
"EwNSMTEwHhcNMjQxMDE3MDAwMjE0WhcNMjUwMTE1MDAwMjEzWjAXMRUwEwYDVQQD\n" \
"DAwqLnZlcmNlbC5hcHAwggEiMA0GCSqGSIb3DQEBAQUAA4IBDwAwggEKAoIBAQCq\n" \
"+6bTukipw3sJx74gd7Q6chT7Kj1Y3xcFMk1575ndJeX6wnXxN31UJ1BD9/Pe0U7H\n" \
"Z/KzUdXp7z1UufNXXZQG7THzoESmJn/wQHglKD21I1KRUHNzO2fcKmJzoBXksCHp\n" \
"NRTVCXszGUEal6rRbDrid6vmPZ13L1fp81U1mAI8xfnXvV+7JWD3x4HvTfekTfRv\n" \
"EgfaWH1LY9HxKbpLHr3h/WUQI61ssFqEOdKyFIf3TJCOiRstFPmnxXPoBZnzYBrl\n" \
"Sd+a/Z2RfiHvUojjtqRCdKcMTcPNba0ZRhZcwla+Y4leMQoeNhptqoBR/nCwfY97\n" \
"NOMRRluNw5ezj5KgBQkRAgMBAAGjggIcMIICGDAOBgNVHQ8BAf8EBAMCBaAwHQYD\n" \
"VR0lBBYwFAYIKwYBBQUHAwEGCCsGAQUFBwMCMAwGA1UdEwEB/wQCMAAwHQYDVR0O\n" \
"BBYEFKfp8G/Ct+EdBQH8QXkmr2b35amRMB8GA1UdIwQYMBaAFMXPRqTq9MPAemyV\n" \
"xC2wXpIvJuO5MFcGCCsGAQUFBwEBBEswSTAiBggrBgEFBQcwAYYWaHR0cDovL3Ix\n" \
"MS5vLmxlbmNyLm9yZzAjBggrBgEFBQcwAoYXaHR0cDovL3IxMS5pLmxlbmNyLm9y\n" \
"Zy8wIwYDVR0RBBwwGoIMKi52ZXJjZWwuYXBwggp2ZXJjZWwuYXBwMBMGA1UdIAQM\n" \
"MAowCAYGZ4EMAQIBMIIBBAYKKwYBBAHWeQIEAgSB9QSB8gDwAHcAzxFW7tUufK/z\n" \
"h1vZaS6b6RpxZ0qwF+ysAdJbd87MOwgAAAGSl/3LeAAABAMASDBGAiEApTeoXgJl\n" \
"wnLH2EDVaXxZJswU033J+/Gyj1idubiZshcCIQCFJ+I8MN/flaiMOZv5wTL+Nwz5\n" \
"SIg07Yh86gYrmz8JywB1ABNK3xq1mEIJeAxv70x6kaQWtyNJzlhXat+u2qfCq+Ai\n" \
"AAABkpf9zFwAAAQDAEYwRAIgTcsXCFfojF36Qqd7tPbmvhNsBhS2Up+GEGcXvgKe\n" \
"8k8CICM0+C8kY4TcNXq6bY4BdBjXZYftMy/7uKN1HFsdC1O2MA0GCSqGSIb3DQEB\n" \
"CwUAA4IBAQCuTGWL6RrpG6tZ7QVbq43qLJL54HQaupQu4tWbg/1Ernnxko9F7KNs\n" \
"n8wUcjlkCMmdrkuWVOsh+ToLwrReODO8GSMpllP5d2exMpCJ8LxsSS/5PW5ob/Ju\n" \
"nYWN4Jm9koF3f8u+EThPM09RJcF7+xlek8eIl4aHej7r7Rk73gJz4cihIEZQUEK8\n" \
"Qjw8uOe3QFREFSLeGDhofafRXee6AtFDimAxTOX45GTaFLW8eiXElF5gQkWdgWFh\n" \
"WKpXzSs39MvdnWfIWG8c7gifjBnEyVMu2XGQAcghgL2v3MhNio3e9Tb7hejmI+Xx\n" \
"i9z9V60YtFhzUiiiKn3FDJVuoNEprQWO\n" \
"-----END CERTIFICATE-----\n";
uint32_t delayMS;
BH1750 lightMeter;
const char* ntpServer = "pool.ntp.org";

//Pins
#define SMOKEPIN 34
#define REDPIN 2
#define GREENPIN 4
#define FANPIN 5

//WiFi settings
WiFiServer server(80);
IPAddress localIP(192, 168, 4, 1); // Fixed IP for ESP1
IPAddress gateway(192, 168, 4, 1);
IPAddress subnet(255, 255, 255, 0);

void setup()
{
  Serial.begin(9600);

  //Setup LED status pins
  pinMode(REDPIN, OUTPUT);
  pinMode(GREENPIN, OUTPUT);
  pinMode(SMOKEPIN, INPUT);
  pinMode(FANPIN, OUTPUT);

  //Setup WiFi
  WiFi.mode(WIFI_AP_STA);
  WiFi.begin(ssid, password);
  Wire.begin(21, 22);
  WiFi.softAPConfig(localIP, gateway, subnet);
  WiFi.softAP(node_ssid, node_password);
  WiFi.softAP(node_ssid, node_password, 6, 0, 4);
  server.begin();
  Serial.println("Server started");
  
  //Delay for sensor readings
  delayMS = 30000;

  //Initialize light sensor
  if (lightMeter.begin(BH1750::CONTINUOUS_HIGH_RES_MODE))
  {
    Serial.println(F("BH1750 configured successfully"));
  }
  else
  {
    Serial.println(F("Error configuring BH1750"));
  }
  Serial.println(F("BH1750 Test begin"));

  //Set ESP32 time
  configTime(0, 0, ntpServer);
}

void loop()
{
  //Initialize variables
  int sensorValue = 0;
  float lux = 0.0;
  bool smoke = false;
  float temperature = 0.0;
  float humidity = 0.0;
  int peopleCount = 0;

  //Check WiFi connection
  if (WiFi.status() != WL_CONNECTED)
  {
    digitalWrite(REDPIN, HIGH);
    digitalWrite(GREENPIN, LOW);
    Serial.println("WiFi not connected, attempting to reconnect...");
    WiFi.begin(ssid, password);

    // Keep trying to reconnect for 10 seconds
    while (WiFi.status() != WL_CONNECTED)
    {
      delay(500);
      Serial.print(".");
    }

    if (WiFi.status() == WL_CONNECTED)
    {
      Serial.println("Reconnected to WiFi");
    }
    else
    {
      Serial.println("Failed to reconnect to WiFi");
    }
  }

  //Enable LED status light
  digitalWrite(REDPIN, LOW);
  digitalWrite(GREENPIN, HIGH);

  //Check for client connection
  WiFiClient client = server.available();
  if (client)
  {
    // Send a request to the client
    client.println("REQUEST DATA");

    // Wait for a response from the client with a timeout
    unsigned long timeout = millis() + 5000; // 5 seconds timeout
    while (!client.available() && millis() < timeout)
    {
      delay(10);
    }

    if (client.available())
    {
      String response = client.readStringUntil('\n');
      // Parse the JSON response
      DynamicJsonDocument doc(1024);
      DeserializationError error = deserializeJson(doc, response);
      if (error)
      {
        Serial.print(F("deserializeJson() failed: "));
        Serial.println(error.f_str());
        return;
      }

      // Make a GET request to python server
      HTTPClient http;
      http.begin("http://192.168.4.5:5000/people_count");
      int httpResponseCode = http.GET();

      if (httpResponseCode > 0)
      {
        String response = http.getString();
        Serial.println(httpResponseCode);
        Serial.println(response);

        // Parse the JSON response
        DynamicJsonDocument peopleDoc(1024);
        DeserializationError error = deserializeJson(peopleDoc, response);
        if (error)
        {
          Serial.print(F("deserializeJson() failed: "));
          Serial.println(error.f_str());
        }
        else
        {
          // Extract the people count
          peopleCount = peopleDoc["count"];
          Serial.print("People count: ");
          Serial.println(peopleCount);
        }
      }
      else
      {
        Serial.print("Error on sending GET: ");
        Serial.println(httpResponseCode);
      }

      http.end();

      // Extract values
      temperature = doc["temperature"];
      humidity = doc["humidity"];
    }
    

    // Disconnect client
    client.stop();
  }

  // Read sensor values
  sensorValue = analogRead(SMOKEPIN);
  Serial.println(sensorValue);
  lux = lightMeter.readLightLevel();

  // Check for smoke
  if (sensorValue >= 1200)
  {
    smoke = true;
  }
  else
  {
    smoke = false;
  }

  // Check for people count
  if(peopleCount < 0){
    peopleCount = 0;
  }

  // Check for temperature and humidity to turn on fan
  if(temperature >= 27 || humidity >= 60){
    digitalWrite(FANPIN, HIGH);
  }else{
    digitalWrite(FANPIN, LOW);
  }
  
  // Prepare JSON payload
  DynamicJsonDocument jsonDoc(1024);
  jsonDoc["temperature"] = temperature;
  jsonDoc["humidity"] = humidity;
  jsonDoc["lux"] = lux;
  jsonDoc["smoke"] = smoke;
  jsonDoc["people"] = peopleCount;

  String jsonData;
  serializeJson(jsonDoc, jsonData);

  // POST data to REST API
  if (WiFi.status() == WL_CONNECTED)
  {
    WiFiClientSecure client;
    client.setCACert(test_root_ca);
    client.setInsecure();
    HTTPClient http;
    http.begin(client, "https://esp-backend-five.vercel.app/api/v1/logs/");

    int httpResponseCode = http.POST(jsonData);

    if (httpResponseCode > 0)
    {
      String response = http.getString();
      Serial.println(httpResponseCode);
      Serial.println(response);
    }
    else
    {
      Serial.print("Error on sending POST: ");
      Serial.println(httpResponseCode);
    }

    http.end();
  }
  else
  {
    Serial.println("Error in WiFi connection");
  }
  
  // Print the values
  Serial.print("Temperature: ");
  Serial.print(temperature);
  Serial.println(" °C");
  Serial.print("Humidity: ");
  Serial.print(humidity);
  Serial.println(" %");
  Serial.print("Light Sensor: ");
  Serial.print(lux);
  Serial.println(" lux");
  Serial.print("Smoke Sensor Value: ");
  Serial.println(smoke);
  
  // Delay between measurements.
  delay(delayMS);
}