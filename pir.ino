// ===== ESP32: PIR SENSOR + RELAY ACTIVE-LOW =====
#include <WiFi.h>
#include <HTTPClient.h>

// --- WIFI ---
#define WIFI_SSID       "YOUR_SSID"
#define WIFI_PASS       "YOUR_PASSWORD"

// --- SENSOR & RELAY ---
#define PIR_PIN         4          // Pin PIR
#define RELAY_PIN       23         // Relay Active-Low

// --- SERVER ---
const char* host = "192.168.1.10"; // IP server Next.js
const int   port = 3000;
const char* device = "ESP32-PIR-RELAY";

// ----------------- KIRIM DATA KE NEXT.JS -----------------
void kirimData(int pirValue, int relayStatus) {
  if (WiFi.status() != WL_CONNECTED) return;

  String json = String("{\"pir\":") + pirValue +
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

  pinMode(PIR_PIN, INPUT);
  pinMode(RELAY_PIN, OUTPUT);
  digitalWrite(RELAY_PIN, HIGH); // relay OFF awal (active-low)

  // WiFi
  WiFi.begin(WIFI_SSID, WIFI_PASS);
  Serial.print("Connecting");
  while (WiFi.status() != WL_CONNECTED) {
    Serial.print(".");
    delay(300);
  }
  Serial.println("\nWiFi connected!");
}

void loop() {
  int pirValue = digitalRead(PIR_PIN);  // HIGH = gerakan terdeteksi

  Serial.printf("PIR: %d\n", pirValue);

  // ---- KONTROL RELAY ----
  if (pirValue == HIGH) {
    digitalWrite(RELAY_PIN, LOW);   // Relay ON
    Serial.println("Gerakan terdeteksi → Relay ON");
  } else {
    digitalWrite(RELAY_PIN, HIGH);  // Relay OFF
    Serial.println("Tidak ada gerakan → Relay OFF");
  }

  int relayStatus = (digitalRead(RELAY_PIN) == LOW) ? 1 : 0;

  // Kirim data ke server
  kirimData(pirValue, relayStatus);

  delay(2000);
}
