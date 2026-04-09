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


// Create display object
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, -1);
DHT dht(DHTPIN, DHTTYPE);

const char* ssid = "JioFiber-C1203-2.4Ghz";
const char* password = "8930679789";
String host = "http://192.168.2.29:8080/api/message?message=";


void connectWifi(){
  WiFi.mode(WIFI_STA); // Set Wi-Fi to "Station" mode (like a normal laptop/phone)
  Serial.print("Wi-Fi MAC Address: " + WiFi.macAddress());
  Serial.print("Connecting to Wi-Fi: " + String(ssid));
  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED) {
    delay(500); // Check every half second
    Serial.print("."); // Print dots to show it's working
  }
  Serial.println("\n\nSUCCESS! Connected to Wi-Fi. IP Address: " + WiFi.localIP().toString());
}

void pushToServer(String message){
      if(WiFi.status() == WL_CONNECTED){
      
      HTTPClient http;
      http.begin(host+message);
      // 4. Specify the GET request
      int httpResponseCode = http.GET();
      
      // 5. Check the response code (200 = OK, negative numbers = connection errors)
      if (httpResponseCode > 0) {

        Serial.print("HTTP Response code: ");
        Serial.println(httpResponseCode);
              }
      else {
        Serial.print("Error code: ");
        Serial.println(httpResponseCode);
      }
      
      http.end();
      
    } else {
      Serial.println("WiFi Disconnected");
    }

}

void setup() {
  Serial.begin(115200);
  dht.begin();
  Serial.println(F("Setup started."));

//  connectWifi();
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
  //clearAndPrintToDisplay("Reading DHT22...");
  // Wait 2 seconds between measurements!
  // The DHT22 is a slow sensor and will return errors if read too fast.
  delay(2000);
  //Serial.println("Looping");

  // Read humidity as a percentage
  float humidity = dht.readHumidity();
  
  // Read temperature as Celsius (the default)
  float tempC = dht.readTemperature();
  
  // Read temperature as Fahrenheit (isFahrenheit = true)

  // Check if any reads failed (NaN means "Not a Number")
  if (isnan(humidity) || isnan(tempC)) {
    Serial.println("Failed to read from DHT sensor! Check your wiring.");
    return; // Skip the rest of the loop and try again in 2 seconds
  }

  // Print the results to the Serial Monitor
  String text = String("Humidity: ") + String(humidity) + String("%\n") + String("Temperature: ") + String(tempC) + String("C");
  clearAndPrintToDisplay(text);
  Serial.println(text);
  //pushToServer(text);
  delay(2000);
}
