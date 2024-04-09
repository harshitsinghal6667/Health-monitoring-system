// Connect to WiFi
#include <ESP8266WiFi.h>

// WiFi settings
char ssidCharArray[] ="realme 11 Pro+ 5G";
char passwordCharArray[] ="123456789";

// ThingSpeak settings
const char* server = "api.thingspeak.com";
const char* apiKey = "I9N3ZCBRPCUL52P2";
const char* field = "field1";

// AD8232 ECG sensor pin
const int ecgPin = A0;

void setup() {
  Serial.begin(115200);

  // Connect to WiFi
  connectToWiFi();

  // Wait for a stable connection
  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.println("Connecting to WiFi...");
  }
  Serial.println("Connected to WiFi!");
}

void loop() {
  // Read ECG sensor data
  int ecgValue = analogRead(ecgPin);

  // Upload data to ThingSpeak
  if (uploadToThingSpeak(ecgValue)) {
    Serial.println("Data sent to ThingSpeak successfully!");
  } else {
    Serial.println("Failed to send data to ThingSpeak.");
  }

  delay(10000); // Wait 10 seconds before sending next data
}

void connectToWiFi() {
  Serial.println("Connecting to WiFi...");
  WiFi.begin(ssidCharArray, passwordCharArray);

  int attempts = 0;
  while (WiFi.status() != WL_CONNECTED && attempts < 30) {
    delay(1000);
    Serial.print(".");
    attempts++;
  }

  if (WiFi.status() != WL_CONNECTED) {
    Serial.println("Failed to connect to WiFi. Please check your credentials.");
    while (true) {
      delay(1000);
    }
  }
}

bool uploadToThingSpeak(int value) {
  // Create an HTTP client
  WiFiClient client;

  // Create the API URL
  String url = "/update?api_key=";
  url += apiKey;
  url += "&";
  url += field;
  url += "=";
  url += String(value);

  // Connect to ThingSpeak
  if (client.connect(server, 80)) {
    // Make the HTTP request
    client.print(String("GET ") + url + " HTTP/1.1\r\n" +
                 "Host: " + server + "\r\n" +
                 "Connection: close\r\n\r\n");

    // Wait for the server to respond
    delay(1000);

    // Read and print the server response
    while (client.available()) {
      char c = client.read();
      Serial.print(c);
    }

    // Check if the upload was successful (look for "200 OK" in the response)
    return (String(client.readStringUntil('\r\n')).indexOf("200 OK") != -1);
  } else {
    Serial.println("Failed to connect to ThingSpeak.");
    return false;
  }
}
