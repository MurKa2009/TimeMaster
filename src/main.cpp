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
#include "GyverButton.h"
#include "handlers/settings.h"
#include "webServer.h"
#include "debugPrint.h"
#include "managers/wifiManager.h"
#include "managers/timeManager.h"
#include "managers/fileManager.h"
#include "managers/bellManager.h"
#include "pins.h"

GButton butt1(BUTTON_PIN);
ESP8266WebServer server(80); // Create a web server on port 80
const String colors[] = {"green", "red", "yellow", "blue"};
const int colorsCount = sizeof(colors) / sizeof(colors[0]);
int currentColorIndex = 0;

void setup()
{
    // Initialize pins
    pinMode(BELL_PIN, OUTPUT);
    digitalWrite(BELL_PIN, HIGH); // Ensure the bell is off

    pinMode(LED1_PIN, OUTPUT);
    pinMode(LED2_PIN, OUTPUT);
    pinMode(LED3_PIN, OUTPUT);
    pinMode(LED4_PIN, OUTPUT);
    pinMode(BUTTON_PIN, INPUT_PULLUP);

    Serial.begin(115200); // Start Serial communication
    delay(100);           // Wait for serial port to stabilize

    printDebugHeader(); // Print the fancy debug header

    DebugPrintln("System starting..."); // Debug message
    DebugPrintln("");
    if (!LittleFS.begin())
    { // Initialize LittleFS
        DebugPrintln("LittleFS initialization failed!");
        return;
    }
    DebugPrintln("LittleFS initialized successfully");

    listFiles();

    if (!loadSettings())
    { // Load settings from file
        DebugPrintln("Settings not found, using default values");
    }

    DebugPrintln("");
    setupWiFi(); // Configure WiFi (AP or Client mode)

    if (MDNS.begin(deviceSettings.localDomain))
    { // Start mDNS responder
        DebugPrintln("mDNS responder started: http://" + deviceSettings.localDomain + ".local");
    }
    else
    {
        DebugPrintln("mDNS failed to start");
    }

    DebugPrintln("");
    setupTime(); // Synchronize time with NTP or RTC

    setupRoutes();  // Set up web server routes
    server.begin(); // Start the web server
    DebugPrintln("HTTP server started on port 80");
}

void updateLEDs()
{
    if (deviceSettings.selectedColor == "red")
    {
        digitalWrite(LED1_PIN, HIGH);
        digitalWrite(LED2_PIN, LOW);
        digitalWrite(LED3_PIN, LOW);
        digitalWrite(LED4_PIN, LOW);
    }
    else if (deviceSettings.selectedColor == "green")
    {
        digitalWrite(LED1_PIN, LOW);
        digitalWrite(LED2_PIN, HIGH);
        digitalWrite(LED3_PIN, LOW);
        digitalWrite(LED4_PIN, LOW);
    }
    else if (deviceSettings.selectedColor == "blue")
    {
        digitalWrite(LED1_PIN, LOW);
        digitalWrite(LED2_PIN, LOW);
        digitalWrite(LED3_PIN, HIGH);
        digitalWrite(LED4_PIN, LOW);
    }
    else if (deviceSettings.selectedColor == "yellow")
    {
        digitalWrite(LED1_PIN, LOW);
        digitalWrite(LED2_PIN, LOW);
        digitalWrite(LED3_PIN, LOW);
        digitalWrite(LED4_PIN, HIGH);
    }
    else
    {
        digitalWrite(LED1_PIN, LOW);
        digitalWrite(LED2_PIN, LOW);
        digitalWrite(LED3_PIN, LOW);
        digitalWrite(LED4_PIN, LOW);
    }
}

void loop()
{
    MDNS.update();         // Handle mDNS requests
    server.handleClient(); // Handle web server requests

    //if (loadSettings()) Serial.println("Настройки успешно загружены.");
    //else Serial.println("Не удалось загрузить настройки.");

    int currentTime = getCurrentTime();                           // Get the current time in seconds since midnight
    checkBellSchedule(currentTime, deviceSettings.selectedColor); // Check if it's time to ring the bell

    butt1.tick();
    if (butt1.isClick()){
        currentColorIndex = (currentColorIndex + 1) % colorsCount; // Переключение на следующий цвет
        deviceSettings.selectedColor = colors[currentColorIndex];  // Обновление выбранного цвета
        saveSettings();                                            // Сохранение настроек
        Serial.print("Выбранный цвет: ");
        Serial.println(deviceSettings.selectedColor);
    }
    updateLEDs();
    // delay(500); // Wait for 1 second before the next iteration
    // Serial.print(deviceSettings.useAPMode);
}