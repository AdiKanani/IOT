#include <Wire.h>
#include <Adafruit_Sensor.h>
#include <Adafruit_BMP085_U.h>
#include <LiquidCrystal.h>
#include <ESP8266WiFi.h>
#include <WiFiClient.h>

// Replace with your network credentials
const char* ssid = "YourWiFiSSID";
const char* password = "YourWiFiPassword";

const char* serverIP = "ServerIPAddress"; // Replace with your server's IP
const int serverPort = 80;

// Initialize sensors
Adafruit_BMP085_Unified bmp = Adafruit_BMP085_Unified(10085);
int glucosePin = A0; // Analog pin for glucose sensor
int heartRatePin = A1; // Analog pin for heart rate sensor

// Initialize LCD
LiquidCrystal lcd(12, 11, 5, 4, 3, 2);

void setup() {
  Serial.begin(9600);
  lcd.begin(16, 2);

  if (!bmp.begin()) {
    Serial.println("Could not find a valid BMP085 sensor, check wiring!");
    while (1);
  }

  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.println("Connecting to WiFi...");
  }
  Serial.println("Connected to WiFi");
}

void loop() {
  float temperature, pressure;
  int glucoseLevel, heartRate;

  sensors_event_t event;
  bmp.getEvent(&event);
  temperature = event.temperature;
  pressure = bmp.pressure / 100.0;

  glucoseLevel = analogRead(glucosePin); // Read glucose level from analog sensor
  heartRate = analogRead(heartRatePin); // Read heart rate from analog sensor

  // Display data on LCD
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Temp: ");
  lcd.print(temperature);
  lcd.print(" C");
  lcd.setCursor(0, 1);
  lcd.print("Pressure: ");
  lcd.print(pressure);
  lcd.print(" hPa");
  delay(2000);

  // Send data to server
  sendDataToServer(temperature, pressure, glucoseLevel, heartRate);

  delay(5000); // Delay before sending the next set of data
}

void sendDataToServer(float temp, float pressure, int glucose, int heartRate) {
  WiFiClient client;
  
  if (client.connect(serverIP, serverPort)) {
    String data = "temp=" + String(temp) +
                  "&pressure=" + String(pressure) +
                  "&glucose=" + String(glucose) +
                  "&heartRate=" + String(heartRate);
    
    client.println("POST /senddata HTTP/1.1");
    client.println("Host: " + String(serverIP));
    client.println("Content-Type: application/x-www-form-urlencoded");
    client.println("Connection: close");
    client.println("Content-Length: " + String(data.length()));
    client.println();
    client.println(data);
    client.println();
    
    while (client.available()) {
      String line = client.readStringUntil('\r');
      Serial.print(line);
    }
  }
  client.stop();
}
