#include <WiFi.h>
#include <HTTPClient.h>
#include <Update.h>
#include <Preferences.h>

// Include Wi-Fi credentials from a separate file
#include "secrets.h"

// Pin for the bulb (LED)
#define BULB_PIN 17

// GitHub raw link to the firmware file
const char* firmware_url = "https://raw.githubusercontent.com/Veolinan/OTA/main/OTA_Trial.ino.bootloader.bin";

// Preferences namespace for storing reboot count
Preferences preferences;
const char* rebootCountKey = "rebootCount";
const int maxReboots = 3;  // Maximum reboots before entering safe mode

void setup() {
  // Initialize Serial Monitor
  Serial.begin(115200);

  // Initialize Preferences
  preferences.begin("ota-safe", false);  // "ota-safe" is the namespace

  // Check reboot count
  int rebootCount = preferences.getInt(rebootCountKey, 0);
  if (rebootCount >= maxReboots) {
    Serial.println("Safe mode activated: Skipping OTA update.");
    preferences.putInt(rebootCountKey, 0);  // Reset the reboot count
    safeMode();
    return;  // Stop further setup logic
  } else {
    // Increment reboot count
    preferences.putInt(rebootCountKey, rebootCount + 1);
    Serial.printf("Reboot count: %d\n", rebootCount + 1);
  }

  // Initialize Bulb Pin
  pinMode(BULB_PIN, OUTPUT);

  // Connect to Wi-Fi
  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.println("Connecting to WiFi...");
  }
  Serial.println("Connected to WiFi");

  // Perform OTA Update
  performOTAUpdate();

  // Reset reboot count after successful OTA
  preferences.putInt(rebootCountKey, 0);
}

void loop() {
  // Toggle the bulb every second to show the device is running
  digitalWrite(BULB_PIN, HIGH);
  delay(1000);
  digitalWrite(BULB_PIN, LOW);
  delay(1000);
}

void performOTAUpdate() {
  Serial.println("Starting OTA Update Check...");

  HTTPClient http;
  http.begin(firmware_url);
  int httpCode = http.GET();

  if (httpCode == HTTP_CODE_OK) {
    int contentLength = http.getSize();
    WiFiClient* client = http.getStreamPtr();

    if (Update.begin(contentLength)) {
      size_t written = Update.writeStream(*client);

      if (written == contentLength) {
        Serial.println("OTA Update Completed Successfully!");
      } else {
        Serial.printf("OTA Update Failed: Written %d/%d Bytes\n", written, contentLength);
      }

      if (Update.end()) {
        Serial.println("Update Finished. Restarting Device...");
        ESP.restart();
      } else {
        Serial.printf("Update Failed: %s\n", Update.getError());
      }
    } else {
      Serial.println("Not Enough Space for OTA Update.");
    }
  } else {
    Serial.printf("HTTP GET Failed with Error Code %d. Reason: %s\n", httpCode, http.errorToString(httpCode).c_str());
  }

  http.end();
}

void safeMode() {
  Serial.println("Device is in Safe Mode.");
  // Keep LED blinking to indicate safe mode
  while (true) {
    digitalWrite(BULB_PIN, HIGH);
    delay(1000);
    digitalWrite(BULB_PIN, LOW);
    delay(1000);
  }
}
