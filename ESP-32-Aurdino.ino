
#include <WiFi.h>
#include <ThingSpeak.h>
#include <Arduino.h>
#include <stdio.h>

// ========== WIFI DETAILS ==========
const char* WIFI_SSID = "iot123456";
const char* WIFI_PASSWORD = "iot123456";

// ========== THINGSPEAK DETAILS ==========
unsigned long CHANNEL_ID = 3531111;  // Replace with your Channel ID
const char* WRITE_API_KEY = "IBFFUGC4AVH7HNA9";

// ========== STM32 UART CONNECTION ==========
// STM32 PA9 (USART1_TX) -> ESP32 GPIO16 (RX2)
// STM32 GND             -> ESP32 GND
#define STM32_RX_PIN 16
#define STM32_TX_PIN 17

HardwareSerial STM32Serial(2);
WiFiClient client;

// Sensor values
float temperature = 0;
float humidity = 0;
int x = 0;
int y = 0;
int z = 0;

bool newData = false;
unsigned long lastUpload = 0;

// ThingSpeak free channel: keep uploads at least 15 seconds apart
const unsigned long UPLOAD_INTERVAL = 20000;

void connectWiFi() {
  WiFi.mode(WIFI_STA);
  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);

  Serial.print("Connecting to Wi-Fi");

  int attempts = 0;

  while (WiFi.status() != WL_CONNECTED && attempts < 30) {
    delay(500);
    Serial.print(".");
    attempts++;
  }

  if (WiFi.status() == WL_CONNECTED) {
    Serial.println("\nWi-Fi connected!");
    Serial.print("IP address: ");
    Serial.println(WiFi.localIP());
  } else {
    Serial.println("\nWi-Fi connection failed!");
    Serial.println("Check Wi-Fi name, password and 2.4 GHz compatibility.");
  }
}

void setup() {
  Serial.begin(115200);
  delay(1000);

  STM32Serial.begin(
    115200,
    SERIAL_8N1,
    STM32_RX_PIN,
    STM32_TX_PIN
  );

  STM32Serial.setTimeout(100);

  Serial.println("\n==================================");
  Serial.println(" Industrial Machine Health Monitor");
  Serial.println(" STM32 + ESP32 + ThingSpeak");
  Serial.println("==================================");

  connectWiFi();
  ThingSpeak.begin(client);
}

void loop() {
  // Reconnect if Wi-Fi disconnects
  if (WiFi.status() != WL_CONNECTED) {
    static unsigned long lastReconnect = 0;

    if (millis() - lastReconnect >= 10000) {
      lastReconnect = millis();
      Serial.println("Wi-Fi disconnected. Reconnecting...");
      WiFi.reconnect();
    }
  }

  // Receive sensor data from STM32
  if (STM32Serial.available()) {
    String line = STM32Serial.readStringUntil('\n');
    line.trim();

    float t, h;
    int vx, vy, vz;

    // Expected STM32 format:
    // T=22,H=39,X=286,Y=3,Z=-43
    if (sscanf(line.c_str(),
               "T=%f,H=%f,X=%d,Y=%d,Z=%d",
               &t, &h, &vx, &vy, &vz) == 5) {

      temperature = t;
      humidity = h;
      x = vx;
      y = vy;
      z = vz;
      newData = true;

      Serial.println("\nSensor data received:");
      Serial.print("Temperature: ");
      Serial.print(temperature);
      Serial.println(" C");

      Serial.print("Humidity: ");
      Serial.print(humidity);
      Serial.println(" %");

      Serial.print("X: ");
      Serial.println(x);

      Serial.print("Y: ");
      Serial.println(y);

      Serial.print("Z: ");
      Serial.println(z);
    } else if (line.length() > 0) {
      Serial.print("Unrecognized STM32 data: ");
      Serial.println(line);
    }
  }

  // Upload to ThingSpeak every 20 seconds
  if (newData &&
      WiFi.status() == WL_CONNECTED &&
      millis() - lastUpload >= UPLOAD_INTERVAL) {

    Serial.println("\nUploading to ThingSpeak...");

    ThingSpeak.setField(1, temperature);
    ThingSpeak.setField(2, humidity);
    ThingSpeak.setField(3, x);
    ThingSpeak.setField(4, y);
    ThingSpeak.setField(5, z);

    int responseCode = ThingSpeak.writeFields(
      CHANNEL_ID,
      WRITE_API_KEY
    );

    if (responseCode == 200) {
      Serial.println("ThingSpeak upload successful!");
      Serial.println("Check your channel's Private View.");
    } else {
      Serial.print("ThingSpeak upload failed. HTTP code: ");
      Serial.println(responseCode);
    }

    lastUpload = millis();
  }
}
