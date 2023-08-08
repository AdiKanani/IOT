#define GAS_SENSOR_PIN A0 // Analog pin for gas sensor

void setup() {
  Serial.begin(9600);
}

void loop() {
  float sensorVoltage = analogRead(GAS_SENSOR_PIN) * (5.0 / 1023.0); // Convert analog reading to voltage
  float airQuality = getAirQuality(sensorVoltage); // Calculate air quality index

  Serial.print("Sensor Voltage: ");
  Serial.print(sensorVoltage);
  Serial.print("V, Air Quality Index: ");
  Serial.println(airQuality);

  delay(5000); // Delay before next reading
}

float getAirQuality(float sensorVoltage) {
  // Calibration values for MQ-135 sensor
  float Ro = 10.0; // Sensor resistance in clean air
  float Rs = (5.0 - sensorVoltage) / sensorVoltage; // Sensor resistance in current air

  // Calculate air quality index
  float ratio = Rs / Ro;
  float ppm = pow(10, (1.7 * ratio - 1.8));

  return ppm;
}
