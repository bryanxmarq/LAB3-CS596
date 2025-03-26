#include <Wire.h>
#include <Arduino.h>
#include "DHT20.h"
#include <WiFi.h>
#include <HTTPClient.h>

// Wi-Fi credentials
const char* ssid = "NETGEAR86";
const char* password = "zanyflower374";

// Create DHT20 object
DHT20 dht20;

void setup() {
  Serial.begin(115200);
  WiFi.begin(ssid, password);  // Connect to Wi-Fi
  
  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.println("Connecting to WiFi...");
  }
  Serial.println("Connected to WiFi!");

  Wire.begin(21, 22);  // Initialize I2C on GPIO 21 (SDA) and GPIO 22 (SCL)

  if (!dht20.begin()) {
    Serial.println("Failed to initialize DHT20 sensor!");
    while (1);  // Stop if the sensor fails to initialize
  }
}

void loop() {
  // Trigger a sensor read
  int result = dht20.read();  // This will block for about 45 milliseconds

  // Check if the read was successful
  if (result != DHT20_OK) {
    Serial.println("Error reading from DHT20 sensor!");
    delay(2000);  // Wait before trying again
    return;
  }

  // Retrieve the humidity and temperature values
  float temperature = dht20.getTemperature();  // Get the last temperature reading
  float humidity = dht20.getHumidity();  // Get the last humidity reading

  // Check if the readings are valid
  if (isnan(temperature) || isnan(humidity)) {
    Serial.println("Error reading valid data from DHT20 sensor!");
  } else {
    // Print data to the serial monitor
    Serial.print("Temperature: ");
    Serial.print(temperature);
    Serial.println("°C");

    Serial.print("Humidity: ");
    Serial.print(humidity);
    Serial.println("%");

    // Send data to Flask server (GET request)
    HTTPClient http;
    String serverPath = "http://3.131.82.246:5000/?temperature=" + String(temperature) + "&humidity=" + String(humidity);
    http.begin(serverPath);  // Specify the URL
    int httpResponseCode = http.GET();  // Send GET request

    if (httpResponseCode > 0) {
      Serial.print("HTTP Response code: ");
      Serial.println(httpResponseCode);
    } else {
      Serial.println("Error sending request");
    }

    http.end();  // Close the connection
  }

  delay(2000);  // Wait 2 seconds before the next reading
}