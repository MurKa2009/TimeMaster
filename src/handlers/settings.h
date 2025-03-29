#pragma once
#include <ArduinoJson.h>
#include <LittleFS.h>

#define SETTINGS_FILE "/settings.json" // File where settings are stored

// Structure to hold device settings
struct DeviceSettings {
  String apSSID;          // Access Point SSID
  String apPassword;      // Access Point password
  String wifiSSID;        // WiFi SSID for client mode
  String wifiPassword;    // WiFi password for client mode
  bool useAPMode;         // Whether to use Access Point mode
  bool debugEnabled;      // Whether debug mode is enabled
  String accountLogin;    // Login for web interface
  String accountPassword; // Password for web interface
  String localDomain;     // mDNS domain name
  String ntpServer;       // NTP server for time synchronization
  String led1Color;       // Color for LED 1
  String led2Color;       // Color for LED 2
  String led3Color;       // Color for LED 3
  String led4Color;       // Color for LED 4
};

// Default settings if no settings file exists
DeviceSettings deviceSettings = {
  "SchoolBellAP", "12345678", "", "", true, true, "admin", "admin", "esp", "time.nist.gov", "", "", "", ""
};

// Load settings from LittleFS
bool loadSettings() {
  if (!LittleFS.exists(SETTINGS_FILE)) return false; // Check if settings file exists
  File file = LittleFS.open(SETTINGS_FILE, "r"); // Open file for reading
  if (!file) return false; // Return false if file cannot be opened
  JsonDocument doc;
  DeserializationError error = deserializeJson(doc, file); // Parse JSON from file
  file.close();
  if (error) return false; // Return false if parsing fails
  
  // Load settings from JSON, using defaults if values are missing
  deviceSettings.apSSID          = doc["apSSID"]          | "SchoolBellAP";
  deviceSettings.apPassword      = doc["apPassword"]      | "12345678";
  deviceSettings.wifiSSID        = doc["wifiSSID"]        | "";
  deviceSettings.wifiPassword    = doc["wifiPassword"]    | "";
  deviceSettings.useAPMode       = doc["useAPMode"]       | true;
  deviceSettings.debugEnabled    = doc["debugEnabled"]    | true;
  deviceSettings.accountLogin    = doc["accountLogin"]    | "admin";
  deviceSettings.accountPassword = doc["accountPassword"] | "admin";
  deviceSettings.localDomain     = doc["localDomain"]     | "esp";
  deviceSettings.ntpServer       = doc["ntpServer"]       | "time.nist.gov";
  deviceSettings.led1Color       = doc["led1Color"]       | "";
  deviceSettings.led2Color       = doc["led2Color"]       | "";
  deviceSettings.led3Color       = doc["led3Color"]       | "";
  deviceSettings.led4Color       = doc["led4Color"]       | "";
  return true;
}

// Save settings to LittleFS
bool saveSettings() {
  File file = LittleFS.open(SETTINGS_FILE, "w"); // Open file for writing
  if (!file) return false; // Return false if file cannot be opened
  JsonDocument doc;
  // Store settings in JSON
  doc["apSSID"]          = deviceSettings.apSSID;
  doc["apPassword"]      = deviceSettings.apPassword;
  doc["wifiSSID"]        = deviceSettings.wifiSSID;
  doc["wifiPassword"]    = deviceSettings.wifiPassword;
  doc["useAPMode"]       = deviceSettings.useAPMode;
  doc["debugEnabled"]    = deviceSettings.debugEnabled;
  doc["accountLogin"]    = deviceSettings.accountLogin;
  doc["accountPassword"] = deviceSettings.accountPassword;
  doc["localDomain"]     = deviceSettings.localDomain;
  doc["ntpServer"]       = deviceSettings.ntpServer;
  doc["led1Color"]       = deviceSettings.led1Color;
  doc["led2Color"]       = deviceSettings.led2Color;
  doc["led3Color"]       = deviceSettings.led3Color;
  doc["led4Color"]       = deviceSettings.led4Color;
  bool ok = (serializeJson(doc, file) > 0); // Write JSON to file
  file.close();
  return ok; // Return true if successful
}