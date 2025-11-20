// ===== ESP32: DHT11, Relay Active-Low, POST JSON ke Next.js =====
#include <WiFi.h>
#include <HTTPClient.h>
#include "DHT.h"

// --- WIFI ---
#define WIFI_SSID       "YOUR_SSID"
#define WIFI_PASS       "YOUR_PASSWORD"

// --- SENSOR & RELAY ---
#define DHTPIN          4
#define DHTTYPE         DHT11
#define RELAY_PIN       23
#define SUHU_MAKS       28.0

// --- SERVER ---
const char* host = "192.168.1.10"; // IP server Next.js
const int   port = 3000;
const char* device = "ESP32-RELAY-DHT";

DHT dht(DHTPIN, DHTTYPE);

// --- FUNGSI KONTROL RELAY ---
void kontrolRelay(float suhu) {
  if (suhu >= SUHU_MAKS) {
    digitalWrite(RELAY_PIN, LOW);   // ON (active-low)
    Serial.println(">>> SUHU >= BATAS! Relay ON");
  } else {
    digitalWrite(RELAY_PIN, HIGH);  // OFF
    Serial.println(">>> Suhu normal. Relay OFF");
  }
}

// --- POST JSON KE NEXT.JS ---
void kirimData(float tDHT, float h, int relayStatus) {
  if (WiFi.status() != WL_CONNECTED) return;

  String json = String("{\"tempDHT\":") + tDHT +
                ",\"humidity\":" + h +
                ",\"relayStatus\":" + relayStatus +
                "}";

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
  digitalWrite(RELAY_PIN, HIGH); // relay OFF saat awal

  dht.begin();

  WiFi.begin(WIFI_SSID, WIFI_PASS);
  while (WiFi.status() != WL_CONNECTED) {
    delay(400);
    Serial.print(".");
  }

  Serial.println("\nWiFi Connected!");
}

void loop() {
  float t = dht.readTemperature();
  float h = dht.readHumidity();

  if (isnan(t) || isnan(h)) {
    Serial.println("Sensor error!");
    delay(2000);
    return;
  }

  Serial.printf("Suhu: %.1f | Humidity: %.1f\n", t, h);

  // Kontrol relay
  kontrolRelay(t);

  int relayState = (digitalRead(RELAY_PIN) == LOW) ? 1 : 0;

  // Kirim ke server
  kirimData(t, h, relayState);

  delay(5000);
}
