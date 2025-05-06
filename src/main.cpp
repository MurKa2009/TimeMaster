#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h>
#include <LittleFS.h>
#include <ESP8266mDNS.h>
#include <WiFiClientSecure.h>
#include <GyverOLED.h>
#include "GyverEncoder.h"
#include "handlers/settings.h"
#include "webServer.h"
#include "debugPrint.h"
#include "managers/wifiManager.h"
#include "managers/timeManager.h"
#include "managers/fileManager.h"
#include "managers/bellManager.h"
#include "managers/oledManager.h"
#include "pins.h"
#include <vector> // Добавьте этот заголовок для использования std::vector

GyverOLED<SSD1306_128x64, OLED_NO_BUFFER> oled;
Encoder enc1(CLK, DT, SW);
ESP8266WebServer server(80);
WiFiClientSecure client;
unsigned long unixTimeOffset = 0;
unsigned long lastSyncTime = 0;
unsigned long previousMillis = 0;
const unsigned long interval = 1000;

String imy;
int ITEMS;
bool flag = true;
int currentTimee;

void setup() {
    pinMode(BELL_PIN, OUTPUT);
    digitalWrite(BELL_PIN, HIGH);
    Serial.begin(115200);
    Wire.setClock(400000L);
    oled.init();
    oled.setScale(2);
    oled.clear();
    oled.print(F("    TIME \n\r   MASTER"));
    oled.update();
    enc1.setType(TYPE2);
    delay(100);
    printDebugHeader();
    DebugPrintln("System starting...");
    if (!LittleFS.begin()) {
        DebugPrintln("LittleFS initialization failed!");
        return;
    }
    DebugPrintln("LittleFS initialized successfully");
    listFiles();
    if (!loadSettings()) {
        DebugPrintln("Settings not found, using default values");
    }
    setupWiFi();
    if (MDNS.begin(deviceSettings.localDomain)) {
        DebugPrintln("mDNS responder started: http://" + deviceSettings.localDomain + ".local");
    } else {
        DebugPrintln("mDNS failed to start");
    }
    setupTime();
    setupRoutes();
    server.begin();
    DebugPrintln("HTTP server started on port 80");
    while(unixTimeOffset<=1){
        unixTimeOffset = getCurrentTime();
    }
}

void loop() {
    unsigned long currentMillis = millis();
    MDNS.update();
    server.handleClient();
    if (deviceSettings.proksy_mod && WiFi.status() == WL_CONNECTED) {
        if (!client.connected()) {
            client.setInsecure();
            if (!client.connect(deviceSettings.proksy_ip.c_str(), deviceSettings.proksy_port)) {
                DebugPrintln("Failed to connect to proxy server");
            } else {
                DebugPrintln("Connected to proxy server");
            }
        }
    }
    if (currentMillis - lastSyncTime >= 8 * 60 * 60 * 1000UL) {
        lastSyncTime = currentMillis;
        unixTimeOffset = getCurrentTime();
    }
    if (currentMillis - previousMillis >= interval) {
        previousMillis = currentMillis;
        unsigned long unixTime = currentMillis / 1000;
        unixTimeOffset +=1;
        currentTimee = unixTimeOffset;
    }
    checkBellSchedule(currentTimee, deviceSettings.selectedProfile, deviceSettings.workMode);

    JsonDocument doc;
    if (!loadProfiles(doc)) {
        DebugPrintln("Failed to load profiles");
        return;
    }

    JsonArray profiles = doc.as<JsonArray>();
    std::vector<String> profileNames;
    for (JsonObject profile : profiles) {
        if (profile.containsKey("name")) {
            String profileName = profile["name"].as<String>();
            profileNames.push_back(profileName);
        }
    }
    ITEMS = profileNames.size() + 1;

    String* profileNamesArray = new String[ITEMS];
    for (size_t i = 0; i < profileNames.size(); i++) {
        profileNamesArray[i] = profileNames[i];
    }
    profileNamesArray[ITEMS - 1] = "режим работы";

    fullOled(profileNamesArray, ITEMS);
    delete[] profileNamesArray;
}
