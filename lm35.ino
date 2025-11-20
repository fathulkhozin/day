#include <WiFi.h>
#include <HTTPClient.h>

// WIFI
#define WIFI_SSID       "YOUR_SSID"
#define WIFI_PASS       "YOUR_PASSWORD"

// SENSOR & RELAY
#define LM35_PIN        34        // Pin analog input
#define RELAY_PIN       23        // Relay active-low
#define SUHU_BATAS      28.0

// SERVER
const char* host = "192.168.1.10";
const int   port = 3000;
const char* device = "ESP32-LM35-RELAY";

float bacaLM35() {
  int adcValue = analogRead(LM35_PIN);
  float voltage = adcValue * (3.3 / 4095.0);
  float tempC = voltage * 100.0;  // LM35: 10mV per degree
  return tempC;
}

void kirimData(float suhu, int relayStatus) {
  if (WiFi.status() != WL_CONNECTED) return;

  String json = String("{\"tempLM35\":") + suhu +
                ",\"relayStatus\":" + relayStatus + "}";

  String url = String("http://") + host + ":" + port + "/api/ingest?device=" + device;

  HTTPClient http;
  http.begin(url);
  http.addHeader("Content-Type", "application/json");

  int code = http.POST(json);
  Serial.printf("POST => %d\n", code);
  if (code > 0) Serial.println(http.getString());

  http.end();
}

void setup() {
  Serial.begin(115200);

  pinMode(RELAY_PIN, OUTPUT);
  digitalWrite(RELAY_PIN, HIGH); // relay OFF awal

  WiFi.begin(WIFI_SSID, WIFI_PASS);
  Serial.print("Connecting");
  while (WiFi.status() != WL_CONNECTED) {
    Serial.print(".");
    delay(300);
  }
  Serial.println("\nWiFi connected");
}

void loop() {
  float suhu = bacaLM35();
  Serial.printf("Suhu LM35: %.2f C\n", suhu);

  if (suhu >= SUHU_BATAS) {
    digitalWrite(RELAY_PIN, LOW);   // Relay ON (active-low)
    Serial.println("Suhu tinggi, relay ON");
  } else {
    digitalWrite(RELAY_PIN, HIGH);  // Relay OFF
    Serial.println("Suhu normal, relay OFF");
  }

  int relayStatus = (digitalRead(RELAY_PIN) == LOW) ? 1 : 0;

  kirimData(suhu, relayStatus);

  delay(2000);
}
