#include <WiFi.h>
#include <HTTPClient.h>

// WIFI
#define WIFI_SSID       "YOUR_SSID"
#define WIFI_PASS       "YOUR_PASSWORD"

// SENSOR & RELAY
#define IR_PIN          4         // Digital output IR sensor
#define RELAY_PIN       23        // Relay active-low

// SERVER
const char* host = "192.168.1.10";
const int   port = 3000;
const char* device = "ESP32-IR-RELAY";

void kirimData(int irValue, int relayStatus) {
  if (WiFi.status() != WL_CONNECTED) return;

  String json = String("{\"ir\":") + irValue +
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

  pinMode(IR_PIN, INPUT);
  pinMode(RELAY_PIN, OUTPUT);
  digitalWrite(RELAY_PIN, HIGH); // Relay OFF (active-low)

  WiFi.begin(WIFI_SSID, WIFI_PASS);
  Serial.print("Connecting");
  while (WiFi.status() != WL_CONNECTED) {
    Serial.print(".");
    delay(300);
  }
  Serial.println("\nWiFi connected");
}

void loop() {
  int irValue = digitalRead(IR_PIN);  
  // IR sensor: LOW = benda terdeteksi, HIGH = tidak ada benda

  Serial.printf("Nilai IR: %d\n", irValue);

  if (irValue == LOW) {
    digitalWrite(RELAY_PIN, LOW);   // Relay ON
    Serial.println("Benda terdeteksi, relay ON");
  } else {
    digitalWrite(RELAY_PIN, HIGH);  // Relay OFF
    Serial.println("Tidak ada benda, relay OFF");
  }

  int relayStatus = (digitalRead(RELAY_PIN) == LOW) ? 1 : 0;

  kirimData(irValue, relayStatus);

  delay(1000);
}
