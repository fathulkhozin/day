#include <WiFi.h>
#include <HTTPClient.h>

// WIFI
#define WIFI_SSID       "YOUR_SSID"
#define WIFI_PASS       "YOUR_PASSWORD"

// SENSOR & RELAY
#define LDR_PIN         34        // Pin analog
#define RELAY_PIN       23        // Relay active-low
#define BATAS_GELAP     1500      // Jika ADC di bawah ini dianggap gelap

// SERVER
const char* host = "192.168.1.10";
const int   port = 3000;
const char* device = "ESP32-LDR-RELAY";

void kirimData(int ldrValue, int relayStatus) {
  if (WiFi.status() != WL_CONNECTED) return;

  String json = String("{\"ldr\":") + ldrValue +
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
  int ldrValue = analogRead(LDR_PIN);
  Serial.printf("Nilai LDR: %d\n", ldrValue);

  // Logika: jika gelap (nilai kecil), relay ON
  if (ldrValue < BATAS_GELAP) {
    digitalWrite(RELAY_PIN, LOW);   // Relay ON
    Serial.println("Gelap, relay ON");
  } else {
    digitalWrite(RELAY_PIN, HIGH);  // Relay OFF
    Serial.println("Terang, relay OFF");
  }

  int relayStatus = (digitalRead(RELAY_PIN) == LOW) ? 1 : 0;

  kirimData(ldrValue, relayStatus);

  delay(2000);
}
