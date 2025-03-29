#pragma once
#include <Arduino.h>
#include "pins.h"
#include "handlers/profiles.h"
#include "timeManager.h"

// Function to ring the bell
void ringBell(int duration) {
  digitalWrite(BELL_PIN, LOW); // Activate the bell
  DebugPrintln("Bell ringing...");
  delay(duration * 1000); // Ring for the specified duration
  digitalWrite(BELL_PIN, HIGH); // Deactivate the bell
  DebugPrintln("Bell stopped.");
}

// Function to check if it's time to ring the bell
void checkBellSchedule(int currentTime) {
  JsonDocument doc;
  if (!loadProfiles(doc)) {
    DebugPrintln("Failed to load profiles for bell schedule.");
    return;
  }

  JsonArray profiles = doc.as<JsonArray>();
  for (JsonObject profile : profiles) {
    for (int i = 1; i <= 3; i++) { // Check each lesson in the profile
      String lessonKey = "lesson" + String(i);
      if (profile.containsKey(lessonKey)) {
        JsonObject lesson = profile[lessonKey];
        String startTime = lesson["start"].as<String>();
        String endTime = lesson["end"].as<String>();

        // Convert "HH:MM" to seconds since midnight
        int startSeconds = getTimeInSeconds(startTime);
        int endSeconds = getTimeInSeconds(endTime);

        // Check if the current time is within the lesson's time range
        if (currentTime >= startSeconds && currentTime <= endSeconds) {
          ringBell(10); // Ring the bell for 10 seconds (adjust as needed)
          break; // Stop checking other lessons in this profile
        }
      }
    }
  }
}

// Helper function to convert "HH:MM" to seconds since midnight
int getTimeInSeconds(String timeStr) {
  int colonIndex = timeStr.indexOf(':');
  int hours = timeStr.substring(0, colonIndex).toInt();
  int minutes = timeStr.substring(colonIndex + 1).toInt();
  return hours * 3600 + minutes * 60;
}