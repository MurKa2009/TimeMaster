#pragma once
#include <ESP8266WiFi.h>
#include "handlers/settings.h"
#include "debugPrint.h"

// Configure WiFi (AP or Client mode)
void setupWiFi() {
    if (deviceSettings.useAPMode) {
        WiFi.mode(WIFI_AP); // Set WiFi to Access Point mode
        IPAddress localIP(172, 8, 254, 1);
        IPAddress gateway(172, 8, 254, 1);
        IPAddress subnet(255, 255, 255, 0);
        WiFi.softAPConfig(localIP, gateway, subnet); // Configure AP
        WiFi.softAP(deviceSettings.apSSID.c_str(), deviceSettings.apPassword.c_str()); // Start AP
        DebugPrintln("Running in Access Point mode");
        DebugPrintln("AP IP: " + WiFi.softAPIP().toString());
    } else {
        WiFi.mode(WIFI_STA); // Set WiFi to Client mode
        WiFi.begin(deviceSettings.wifiSSID.c_str(), deviceSettings.wifiPassword.c_str()); // Connect to WiFi
        DebugPrint("Starting WiFi Client mode");
        
        unsigned long startTime = millis();
        while (WiFi.status() != WL_CONNECTED && millis() - startTime < 15000) { // Wait for 15 seconds
            delay(500);
            DebugPrint(".");
        }
        DebugPrintln("");
        if (WiFi.status() == WL_CONNECTED) {
            DebugPrintln("WiFi connected, IP: " + WiFi.localIP().toString());
        } else {
            DebugPrintln("WiFi connection failed! Switching to AP mode...");
            WiFi.mode(WIFI_AP); // Fallback to AP mode
            IPAddress localIP(172, 8, 254, 1);
            IPAddress gateway(172, 8, 254, 1);
            IPAddress subnet(255, 255, 255, 0);
            WiFi.softAPConfig(localIP, gateway, subnet);
            WiFi.softAP(deviceSettings.apSSID.c_str(), deviceSettings.apPassword.c_str());
            DebugPrintln("Running in Access Point mode");
            DebugPrintln("AP IP: " + WiFi.softAPIP().toString());
        }
    }
}