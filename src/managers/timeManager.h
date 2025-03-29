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
    configTime(3 * 3600, 0, deviceSettings.ntpServer); // Configure NTP
    DebugPrintln("Fetching NTP time...");
    struct tm timeinfo;
    if (!getLocalTime(&timeinfo)) {
        DebugPrintln("Failed to obtain time from NTP. Falling back to RTC.");
    } else {
        DebugPrintln("Time synchronized: " + String(asctime(&timeinfo)));
        return; // Use NTP time
    }

    // Fallback to RTC if NTP fails or WiFi is not connected
    setupRTC();
    DebugPrintln("Using RTC time.");
}

// Get the current time in seconds since midnight
int getCurrentTime() {
  struct tm timeinfo;
  if (getLocalTime(&timeinfo)) { // Try to get time from NTP or RTC
    return timeinfo.tm_hour * 3600 + timeinfo.tm_min * 60 + timeinfo.tm_sec;
  }
  return 0; // Default fallback
}