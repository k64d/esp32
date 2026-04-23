#include <Arduino.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include "DHT.h"
#include <WiFi.h>  
#include <HTTPClient.h>

#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64
#define DHTPIN 23      // The ESP32 pin connected to the OUT/SDA pin of the DHT22
#define DHTTYPE DHT22  // Let the library know we are using the DHT22


//Initialising display
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, -1);
//Initialising DHT sensor
DHT dht(DHTPIN, DHTTYPE);

const char* ssid = "JioFiber-SSID";
const char* password = "PWD";
String host = "http://192.168.29.100:8182/api/message?message=";


void connectWifi(){
  WiFi.mode(WIFI_STA);
  Serial.print("Wi-Fi MAC Address: " + WiFi.macAddress());
  Serial.print("Connecting to Wi-Fi: " + String(ssid));
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500); 
    Serial.print(".");
  }
  Serial.println("\n\nSUCCESS! Connected to Wi-Fi. IP Address: " + WiFi.localIP().toString());
}

void ensureWiFiConnected(){
  if(WiFi.status() != WL_CONNECTED){
    Serial.println("WiFi disconnected. Reconnecting...");
    connectWifi();
  }
}

void pushToServer(String message){
    ensureWiFiConnected();
    
    // Retry HTTP up to 3 times
    int maxRetries = 3;
    int retryCount = 0;
    
    while(retryCount < maxRetries) {
      HTTPClient http;
      String tmpUrl = host + message;
      Serial.println("Pushing data to server (attempt " + String(retryCount + 1) + "/" + String(maxRetries) + "): " + tmpUrl);
      
      http.begin(tmpUrl);
      http.setConnectTimeout(10000);
      http.setTimeout(10000);
      http.setUserAgent("ESP32-DHT22");
      
      int httpResponseCode = http.GET();
      http.end();
      
      if (httpResponseCode == 200) {
        Serial.println("Successfully pushed data! Response code: " + String(httpResponseCode));
        return;
      }
      else {
        Serial.println("Push failed with code: " + String(httpResponseCode) + ". Retrying...");
        retryCount++;
        if(retryCount < maxRetries) {
          delay(2000);
        }
      }
    }
    Serial.println("Failed to push data after " + String(maxRetries) + " attempts");
}

void setup() {
  Serial.begin(115200);
  dht.begin();
  Serial.println(F("Setup started."));
  connectWifi();
  if(!display.begin(SSD1306_SWITCHCAPVCC, 0x3C)) {
    Serial.println(F("SSD1306 allocation failed"));
    for(;;);
  }
}


void clearAndPrintToDisplay(String text) {
  Serial.println("Updating display with text: " + text);
  display.clearDisplay();
  display.setTextColor(WHITE);
  display.setCursor(0, 30);
  display.println(text);
  display.display();  
}


void loop() {
  float humidity = dht.readHumidity();  
  float tempC = dht.readTemperature();
  
  if (isnan(humidity) || isnan(tempC)) {
    Serial.println("Failed to read from DHT sensor! Check your wiring.");
    return;
  }

  String text = String("Humidity: ") + String(humidity) + String("%\n") + String("Temperature: ") + String(tempC) + String("C");
  clearAndPrintToDisplay(text);
  Serial.println(text);
  pushToServer(String(tempC)+"C|"+String(humidity)+"%");
  delay(30*60*1000);
}
