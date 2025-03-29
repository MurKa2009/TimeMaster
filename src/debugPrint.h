#pragma once
#include <Arduino.h>
#include "handlers/settings.h"

// Function to print the debug header
void printDebugHeader() {
  Serial.println();
  Serial.println("  _____ _                __  __           _            ");
  Serial.println(" |_   _(_)_ __ ___   ___|  \\/  | __ _ ___| |_ ___ _ __ ");
  Serial.println("   | | | | '_ ` _ \\ / _ \\ |\\/| |/ _` / __| __/ _ \\ '__|");
  Serial.println("   | | | | | | | | |  __/ |  | | (_| \\__ \\ ||  __/ |   ");
  Serial.println("   |_| |_|_| |_| |_|\\___|_|  |_|\\__,_|___/\\__\\___|_|   ");
  Serial.println();
  Serial.println("========================================================");
  Serial.println("  Debug Mode: Enabled");
  Serial.println("  Use this console to monitor system activity and debug.");
  Serial.println("  All debug messages will be printed below.");
  Serial.println("========================================================");
  Serial.println();
}

// Print a message to Serial if debug mode is enabled
inline void DebugPrintln(const String &message) {
  if (deviceSettings.debugEnabled) {
    Serial.println(message);
  }
}

// Print a message to Serial without a newline if debug mode is enabled
inline void DebugPrint(const String &message) {
  if (deviceSettings.debugEnabled) {
    Serial.print(message);
  }
}