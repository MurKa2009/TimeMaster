// //========------------------------------------------========//
// //   _____ _                __  __           _              //
// //  |_   _(_)_ __ ___   ___|  \/  | __ _ ___| |_ ___ _ __   //
// //    | | | | '_ ` _ \ / _ \ |\/| |/ _` / __| __/ _ \ '__|  //
// //    | | | | | | | | |  __/ |  | | (_| \__ \ ||  __/ |     //
// //    |_| |_|_| |_| |_|\___|_|  |_|\__,_|___/\__\___|_|     //
// //                                                          //
// //========------------------------------------------========//

// #include <ESP8266WiFi.h>
// #include <ESP8266WebServer.h>
// #include <LittleFS.h>
// #include <ESP8266mDNS.h>
// #include "handlers/settings.h"
// #include "webServer.h"
// #include "debugPrint.h"
// #include "managers/wifiManager.h"
// #include "managers/timeManager.h"
// #include "managers/fileManager.h"

// ESP8266WebServer server(80); // Create a web server on port 80

// void setup() {
//     Serial.begin(115200); // Start Serial communication
//     delay(100); // Wait for serial port to stabilize

//     printDebugHeader(); // Print the fancy debug header

//     DebugPrintln("System starting..."); // Debug message
//     DebugPrintln("");
//     if (!LittleFS.begin()) { // Initialize LittleFS
//         DebugPrintln("LittleFS initialization failed!");
//         return;
//     }
//     DebugPrintln("LittleFS initialized successfully");

//     listFiles();

//     if (!loadSettings()) { // Load settings from file
//         DebugPrintln("Settings not found, using default values");
//     }

//     DebugPrintln("");
//     setupWiFi(); // Configure WiFi (AP or Client mode)

//     if (MDNS.begin(deviceSettings.localDomain)) { // Start mDNS responder
//         DebugPrintln("mDNS responder started: http://" + deviceSettings.localDomain + ".local");
//     } else {
//         DebugPrintln("mDNS failed to start");
//     }

//     DebugPrintln("");
//     setupTime(); // Synchronize time with NTP

//     setupRoutes(); // Set up web server routes
//     server.begin(); // Start the web server
//     DebugPrintln("HTTP server started on port 80");
// }

// void loop() {
//     MDNS.update(); // Handle mDNS requests
//     server.handleClient(); // Handle web server requests
// }

#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h>
#include <LittleFS.h>
#include <ESP8266mDNS.h>
#include "handlers/settings.h"
#include "webServer.h"
#include "debugPrint.h"
#include "managers/wifiManager.h"
#include "managers/timeManager.h"
#include "managers/fileManager.h"
#include "managers/bellManager.h"
#include "pins.h"

ESP8266WebServer server(80); // Create a web server on port 80

void setup() {
    // Initialize pins
    pinMode(BELL_PIN, OUTPUT);
    digitalWrite(BELL_PIN, HIGH); // Ensure the bell is off

    pinMode(LED1_PIN, OUTPUT);
    pinMode(LED2_PIN, OUTPUT);
    pinMode(LED3_PIN, OUTPUT);
    pinMode(LED4_PIN, OUTPUT);
    pinMode(BUTTON_PIN, INPUT_PULLUP);

    Serial.begin(115200); // Start Serial communication
    delay(100); // Wait for serial port to stabilize

    printDebugHeader(); // Print the fancy debug header

    DebugPrintln("System starting..."); // Debug message
    DebugPrintln("");
    if (!LittleFS.begin()) { // Initialize LittleFS
        DebugPrintln("LittleFS initialization failed!");
        return;
    }
    DebugPrintln("LittleFS initialized successfully");

    listFiles();

    if (!loadSettings()) { // Load settings from file
        DebugPrintln("Settings not found, using default values");
    }

    DebugPrintln("");
    setupWiFi(); // Configure WiFi (AP or Client mode)

    if (MDNS.begin(deviceSettings.localDomain)) { // Start mDNS responder
        DebugPrintln("mDNS responder started: http://" + deviceSettings.localDomain + ".local");
    } else {
        DebugPrintln("mDNS failed to start");
    }

    DebugPrintln("");
    setupTime(); // Synchronize time with NTP or RTC

    setupRoutes(); // Set up web server routes
    server.begin(); // Start the web server
    DebugPrintln("HTTP server started on port 80");
}

void loop() {
    MDNS.update(); // Handle mDNS requests
    server.handleClient(); // Handle web server requests

    int currentTime = getCurrentTime(); // Get the current time in seconds since midnight
    checkBellSchedule(currentTime); // Check if it's time to ring the bell

    // Handle button press (if needed)
    if (digitalRead(BUTTON_PIN)) {
        // Add button logic here
    }

    delay(1000); // Wait for 1 second before the next iteration
}