#include <ESP8266WiFi.h>
#include <WiFiClient.h>

const char* ssid = "YourWiFiSSID";
const char* password = "YourWiFiPassword";
const char* serverIP = "ServerIPAddress"; // Replace with your server's IP

const int sensorPin = A0;  // Analog pin for ultrasonic sensor
const int ledPin = 13;     // LED pin for indicating parking availability

void setup() {
  Serial.begin(115200);
  pinMode(ledPin, OUTPUT);
  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.println("Connecting to WiFi...");
  }
  Serial.println("Connected to WiFi");
}

void loop() {
  long duration, distance;
  digitalWrite(ledPin, LOW);

  // Trigger ultrasonic pulse
  digitalWrite(sensorPin, LOW);
  delayMicroseconds(2);
  digitalWrite(sensorPin, HIGH);
  delayMicroseconds(10);
  digitalWrite(sensorPin, LOW);

  // Measure the time it takes for pulse to return
  duration = pulseIn(sensorPin, HIGH);

  // Calculate distance in centimeters
  distance = duration / 29 / 2;

  if (distance < 20) {  // Adjust this threshold based on your parking space
    digitalWrite(ledPin, HIGH); // Parking space occupied
    sendDataToServer(1); // Send status to server
  } else {
    digitalWrite(ledPin, LOW); // Parking space vacant
    sendDataToServer(0); // Send status to server
  }

  delay(1000); // Delay before next reading
}

void sendDataToServer(int status) {
  WiFiClient client;

  if (client.connect(serverIP, 80)) {
    Serial.println("Sending data to server...");
    client.print("GET /update?status=");
    client.print(status);
    client.println(" HTTP/1.1");
    client.println("Host: ServerIPAddress");
    client.println("Connection: close");
    client.println();
    delay(100); // Give some time for the server to respond
    Serial.println("Data sent!");
  }
  client.stop();
}
