// #pragma once
// #include <time.h>
// #include "handlers/settings.h"
// #include "debugPrint.h"

// // Synchronize time with NTP server
// void setupTime() {
//     configTime(3 * 3600, 0, deviceSettings.ntpServer); // Configure NTP
//     DebugPrintln("Fetching NTP time...");
//     struct tm timeinfo;
//     if (!getLocalTime(&timeinfo)) {
//         DebugPrintln("Failed to obtain time");
//         DebugPrintln("");
//     } else {
//         DebugPrintln("Time synchronized: " + String(asctime(&timeinfo)));
//     }
//     return;
// }

#pragma once
#include <time.h>
#include <Wire.h>
#include "handlers/settings.h"
#include "debugPrint.h"

// RTC (DS3231) setup
void setupRTC() {
  Wire.begin(); // Initialize I2C for RTC
  DebugPrintln("RTC initialized.");
}

// Get the current time from the RTC in seconds since midnight
int getRTCTime() {
  struct tm timeinfo;
  if (!getLocalTime(&timeinfo)) { // Try to get time from RTC
    DebugPrintln("Failed to get time from RTC");
    return 0;
  }
  return timeinfo.tm_hour * 3600 + timeinfo.tm_min * 60 + timeinfo.tm_sec;
}

// Synchronize time with NTP server
void setupTime() {
  if (WiFi.status() != WL_CONNECTED) {
    DebugPrintln("WiFi not connected. Cannot sync with NTP.");
    setupRTC();
    DebugPrintln("Using RTC time.");
    return;
  }

  configTime(3 * 3600, 0, deviceSettings.ntpServer); // Configure NTP
  DebugPrintln("Fetching NTP time...");
  struct tm timeinfo;
  delay(2000); // Wait for time to sync

  if (!getLocalTime(&timeinfo)) {
    DebugPrintln("Failed to obtain time from NTP. Falling back to RTC.");
    setupRTC();
    DebugPrintln("Using RTC time.");
  } else {
    DebugPrintln("Time synchronized: " + String(asctime(&timeinfo)));
  }
}

// Get the current time in seconds since midnight
int getCurrentTime() {
  struct tm timeinfo;
  if (getLocalTime(&timeinfo)) { // Try to get time from NTP or RTC
    return timeinfo.tm_hour * 3600 + timeinfo.tm_min * 60 + timeinfo.tm_sec;
  }
  return 0; // Default fallback
}

int getCurrentDayOfWeek() {
  struct tm timeinfo;
  if (getLocalTime(&timeinfo)) { // Try to get time from NTP or RTC
    return timeinfo.tm_wday;
  }
  return -1; // Default fallback if time cannot be obtained
}