#pragma once
#include <ESP8266WebServer.h>
#include <LittleFS.h>
#include <ArduinoJson.h>
#include "handlers/settings.h"
#include "handlers/profiles.h"

extern ESP8266WebServer server; // Declare the web server instance

// Check if the user is authenticated
bool isAuthenticated() {
  if (!server.authenticate(deviceSettings.accountLogin.c_str(), deviceSettings.accountPassword.c_str())) {
    server.requestAuthentication(); // Request authentication if not logged in
    return false;
  }
  return true;
}

// Send an HTML template file to the client
void sendTemplate(const char* filename) {
  if (!LittleFS.exists(filename)) {
    server.send(404, "text/plain", "File not found"); // Return 404 if file doesn't exist
    return;
  }
  File file = LittleFS.open(filename, "r"); // Open file for reading
  if (!file) {
    server.send(500, "text/plain", "File open error"); // Return 500 if file cannot be opened
    return;
  }
  server.streamFile(file, "text/html"); // Stream the file to the client
  file.close();
}

// Handle requests to the root path
void handleRoot() {
  sendTemplate("/index.html"); // Send the index.html file
}

// Handle requests to the dashboard
void handleDashboard() {
  if (!isAuthenticated()) return; // Check authentication
  sendTemplate("/dashboard.html"); // Send the dashboard.html file
}

// Handle settings API requests
void handleSettingsAPI() {
  if (!isAuthenticated()) return; // Check authentication

  if (server.method() == HTTP_GET) {
    // Return current settings as JSON
    DynamicJsonDocument doc(1024);
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

    String output;
    serializeJson(doc, output);
    server.send(200, "application/json", output); // Send JSON response
  }
  else if (server.method() == HTTP_POST) {
    // Update settings from JSON
    DynamicJsonDocument doc(1024);
    DeserializationError error = deserializeJson(doc, server.arg("plain"));
    if (error) {
      server.send(400, "text/plain", "Invalid JSON format"); // Return 400 if JSON is invalid
      return;
    }

    Serial.println("Received JSON:");
    serializeJson(doc, Serial); // Print received JSON to Serial

    if (doc.containsKey("apSSID")) deviceSettings.apSSID = doc["apSSID"].as<String>();
    if (doc.containsKey("apPassword")) deviceSettings.apPassword = doc["apPassword"].as<String>();
    if (doc.containsKey("wifiSSID")) deviceSettings.wifiSSID = doc["wifiSSID"].as<String>();
    if (doc.containsKey("wifiPassword")) deviceSettings.wifiPassword = doc["wifiPassword"].as<String>();
    if (doc.containsKey("useAPMode")) {
      deviceSettings.useAPMode = doc["useAPMode"].as<bool>();
      Serial.print("useAPMode set to: ");
      Serial.println(deviceSettings.useAPMode);
    }
    if (doc.containsKey("debugEnabled")) {
      deviceSettings.debugEnabled = doc["debugEnabled"].as<bool>();
      Serial.print("debugEnabled set to: ");
      Serial.println(deviceSettings.debugEnabled);
    }
    if (doc.containsKey("accountLogin")) deviceSettings.accountLogin = doc["accountLogin"].as<String>();
    if (doc.containsKey("accountPassword")) deviceSettings.accountPassword = doc["accountPassword"].as<String>();
    if (doc.containsKey("localDomain")) deviceSettings.localDomain = doc["localDomain"].as<String>();
    if (doc.containsKey("ntpServer")) deviceSettings.ntpServer = doc["ntpServer"].as<String>();
    if (doc.containsKey("led1Color")) deviceSettings.led1Color = doc["led1Color"].as<String>();
    if (doc.containsKey("led2Color")) deviceSettings.led2Color = doc["led2Color"].as<String>();
    if (doc.containsKey("led3Color")) deviceSettings.led3Color = doc["led3Color"].as<String>();
    if (doc.containsKey("led4Color")) deviceSettings.led4Color = doc["led4Color"].as<String>();

    if (saveSettings()) { // Save updated settings
      DynamicJsonDocument resp(256);
      resp["success"] = true;
      String output;
      serializeJson(resp, output);
      server.send(200, "application/json", output); // Send success response
    } else {
      server.send(500, "text/plain", "Save error"); // Return 500 if save fails
    }
  } else {
    server.send(405, "text/plain", "Method Not Allowed"); // Return 405 for unsupported methods
  }
}

// Handle profiles API requests
void handleProfilesAPI() {
  if (!isAuthenticated()) return; // Check authentication

  if (server.method() == HTTP_GET) {
    // Return all profiles as JSON
    DynamicJsonDocument doc(1024);
    if (!loadProfiles(doc)) {
      server.send(500, "text/plain", "Load error"); // Return 500 if profiles cannot be loaded
      return;
    }
    String output;
    serializeJson(doc.as<JsonArray>(), output);
    server.send(200, "application/json", output); // Send JSON response
  }
  else if (server.method() == HTTP_POST) {
    // Add or update a profile
    DynamicJsonDocument doc(1024);
    DeserializationError error = deserializeJson(doc, server.arg("plain"));
    if (error) {
      server.send(400, "text/plain", "Invalid JSON format"); // Return 400 if JSON is invalid
      return;
    }
    JsonObject profile = doc.as<JsonObject>();
    if (!profile.containsKey("id") || !profile.containsKey("name")) {
      server.send(400, "text/plain", "Profile must contain id and name"); // Return 400 if required fields are missing
      return;
    }
    if (!profile.containsKey("color")) {
      profile["color"] = "#FFFFFF"; // Set default color if missing
    }

    if (saveProfile(profile)) { // Save the profile
      DynamicJsonDocument resp(256);
      resp["success"] = true;
      String output;
      serializeJson(resp, output);
      server.send(200, "application/json", output); // Send success response
    } else {
      server.send(500, "text/plain", "Save error"); // Return 500 if save fails
    }
  }
  else if (server.method() == HTTP_DELETE) {
    // Delete a profile
    if (!server.hasArg("id")) {
      server.send(400, "text/plain", "Missing id parameter"); // Return 400 if ID is missing
      return;
    }
    String profileId = server.arg("id");
    if (deleteProfile(profileId.c_str())) {
      server.send(200, "text/plain", "Profile deleted"); // Send success response
    } else {
      server.send(500, "text/plain", "Delete error"); // Return 500 if delete fails
    }
  } else {
    server.send(405, "text/plain", "Method Not Allowed"); // Return 405 for unsupported methods
  }
}

// Handle device time synchronization
void handleSyncDevice() {
  if (!isAuthenticated()) return; // Check authentication

  struct tm timeinfo;
  if (!getLocalTime(&timeinfo)) {
    DynamicJsonDocument doc(256);
    doc["success"] = false;
    doc["error"] = "Failed to get local time"; // Return 500 if time cannot be fetched
    String output;
    serializeJson(doc, output);
    server.send(500, "application/json", output);
    return;
  }

  char buffer[25]; // Buffer to store formatted time
  strftime(buffer, sizeof(buffer), "%Y-%m-%dT%H:%M:%S", &timeinfo); // Format time as ISO-8601

  DynamicJsonDocument doc(256);
  doc["success"] = true;
  doc["time"] = buffer;
  String output;
  serializeJson(doc, output);
  server.send(200, "application/json", output); // Send time in JSON response
}

// Handle NTP time synchronization
void handleSyncNTP() {
  if (!isAuthenticated()) return; // Check authentication

  // If device is in AP mode, NTP sync is not available
  if (deviceSettings.useAPMode) {
    DynamicJsonDocument doc(256);
    doc["success"] = false;
    doc["error"] = "NTP sync is not available in AP mode"; // Return 400 if in AP mode
    String output;
    serializeJson(doc, output);
    server.send(400, "application/json", output);
    return;
  }

  // Configure NTP and fetch time
  configTime(3 * 3600, 0, deviceSettings.ntpServer); // Set NTP server
  struct tm timeinfo;
  delay(2000); // Wait for time to sync

  if (!getLocalTime(&timeinfo)) {
    DynamicJsonDocument doc(256);
    doc["success"] = false;
    doc["error"] = "Failed to get time from NTP"; // Return 500 if NTP sync fails
    String output;
    serializeJson(doc, output);
    server.send(500, "application/json", output);
    return;
  }

  char buffer[25]; // Buffer to store formatted time
  strftime(buffer, sizeof(buffer), "%Y-%m-%dT%H:%M:%S", &timeinfo); // Format time as ISO-8601

  DynamicJsonDocument doc(256);
  doc["success"] = true;
  doc["time"] = buffer;
  String output;
  serializeJson(doc, output);
  server.send(200, "application/json", output); // Send time in JSON response
}

// Handle manual time synchronization
void handleSyncManual() {
  if (!isAuthenticated()) return; // Check authentication

  DynamicJsonDocument doc(256);
  DeserializationError error = deserializeJson(doc, server.arg("plain"));
  if (error || !doc.containsKey("time")) {
    doc.clear();
    doc["success"] = false;
    doc["error"] = "Invalid JSON format or missing 'time'"; // Return 400 if JSON is invalid or time is missing
    String output;
    serializeJson(doc, output);
    server.send(400, "application/json", output);
    return;
  }

  String timeStr = doc["time"].as<String>();
  struct tm t;
  if (sscanf(timeStr.c_str(), "%d-%d-%dT%d:%d:%d", &t.tm_year, &t.tm_mon, &t.tm_mday, &t.tm_hour, &t.tm_min, &t.tm_sec) == 6) {
    t.tm_year -= 1900;
    t.tm_mon -= 1;
    time_t newTime = mktime(&t);
    if (newTime == -1) {
      doc.clear();
      doc["success"] = false;
      doc["error"] = "Failed to convert time"; // Return 500 if time conversion fails
      String output;
      serializeJson(doc, output);
      server.send(500, "application/json", output);
      return;
    }

    struct timeval now = { .tv_sec = newTime };
    settimeofday(&now, NULL); // Set the system time

    DynamicJsonDocument resp(256);
    resp["success"] = true;
    resp["set_time"] = asctime(&t);
    String output;
    serializeJson(resp, output);
    server.send(200, "application/json", output); // Send success response
  } else {
    doc.clear();
    doc["success"] = false;
    doc["error"] = "Invalid time format, expected YYYY-MM-DDTHH:MM:SS"; // Return 400 if time format is invalid
    String output;
    serializeJson(doc, output);
    server.send(400, "application/json", output);
  }
}

// Set up all routes for the web server
void setupRoutes() {
  server.on("/", HTTP_GET, handleRoot); // Root path
  server.on("/dashboard", HTTP_GET, handleDashboard); // Dashboard path

  server.on("/api/settings", HTTP_GET, handleSettingsAPI); // Settings API (GET)
  server.on("/api/settings", HTTP_POST, handleSettingsAPI); // Settings API (POST)

  server.on("/api/profiles", HTTP_GET, handleProfilesAPI); // Profiles API (GET)
  server.on("/api/profiles", HTTP_POST, handleProfilesAPI); // Profiles API (POST)
  server.on("/api/profiles", HTTP_DELETE, handleProfilesAPI); // Profiles API (DELETE)

  server.on("/api/sync/device", HTTP_POST, handleSyncDevice); // Device time sync API
  server.on("/api/sync/ntp", HTTP_POST, handleSyncNTP); // NTP time sync API
  server.on("/api/sync/manual", HTTP_POST, handleSyncManual); // Manual time sync API

  server.serveStatic("/styles.min.css", LittleFS, "/styles.min.css"); // Serve static CSS file
  server.serveStatic("/scripts.js", LittleFS, "/scripts.js"); // Serve static JS file

  server.onNotFound([]() {
    server.send(404, "text/plain", "Page not found"); // Handle 404 errors
  });
}
