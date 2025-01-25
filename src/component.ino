#include <WiFi.h>
#include <HTTPClient.h>
#include <ESPAsyncWebServer.h>
#include <config.h>

// Define GPIO pins
#define LED_PIN 18     
#define ULTRA_SENSOR_E 4
#define ULTRA_SENSOR_T 2
#define TRIGGER_pulse 1

int DURATION;
long DISTANCE;

// Server endpoint URL
const char* serverUrl = "https://prod-84.westeurope.logic.azure.com:443/workflows/1e7fc604d38a4dde8784b71a035a7893/triggers/manual/paths/invoke?api-version=2016-06-01&sp=%2Ftriggers%2Fmanual%2Frun&sv=1.0&sig=nRE-jBhuwUOuzBj0WHn-GlnQwcXt9m3KZM630ozcmYY";  // Replace with your endpoint

// Create Async Web Server running on port 80
AsyncWebServer server(80);

void setup() {
  // Start serial communication
  Serial.begin(115200);

  // Set the LED pin as output
  pinMode(LED_PIN, OUTPUT);
  pinMode(ULTRA_SENSOR_E, INPUT);
  pinMode(ULTRA_SENSOR_T, OUTPUT);

  // Connect to Wi-Fi
  WiFi.begin(ssid, password);
  Serial.print("Connecting to Wi-Fi");
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("\nConnected to Wi-Fi");
  Serial.println(WiFi.localIP());

  // Handle HTTP POST request at "/trigger"
  server.on("/trigger", HTTP_POST, [](AsyncWebServerRequest *request){
    digitalWrite(LED_PIN, HIGH); // Turn on LED to indicate process
    sendDuration();
    digitalWrite(LED_PIN, LOW);  // Turn off LED
    request->send(200, "text/plain", "Request received, duration sent");
  });

  // Start the server
  server.begin();
}

void sendDuration() {
  if (WiFi.status() == WL_CONNECTED) {
    HTTPClient http;

    http.begin(serverUrl);  // Specify the request URL
    http.addHeader("Content-Type", "application/json");  // Set content type

    sense();
    String payload = "{\"duration\":" + String(DURATION) + "}";  // JSON payload

    int httpResponseCode = http.POST(payload);  // Send the POST request

    if (httpResponseCode > 0) {
      Serial.print("Server response: ");
      Serial.println(http.getString());
    } else {
      Serial.print("Error on sending POST: ");
      Serial.println(httpResponseCode);
    }

    http.end();  // Free resources
  } else {
    Serial.println("Wi-Fi not connected");
  }
}

void sense() {
  long sum = 0;
  const int numMeasurements = 5;
  int measurements[numMeasurements];

  for (int i = 0; i < numMeasurements; i++) {
    digitalWrite(ULTRA_SENSOR_T, HIGH);
    delay(TRIGGER_pulse);
    digitalWrite(ULTRA_SENSOR_T, LOW);
    
    measurements[i] = pulseIn(ULTRA_SENSOR_E, HIGH);
    sum += measurements[i];

    Serial.print("Measurement ");
    Serial.print(i + 1);
    Serial.print(": ");
    Serial.println(measurements[i]);
    
    delay(50);  // Small delay between measurements to avoid sensor errors
  }

  DURATION = sum / numMeasurements;  // Calculate the average duration
  DISTANCE = (DURATION * 0.034 / 2) + 1;

  Serial.print("Average Duration: ");
  Serial.println(DURATION);
  Serial.print("Calculated Distance: ");
  Serial.println(DISTANCE);
}

void loop() {
  // No need to poll continuously, handled via web server
}
