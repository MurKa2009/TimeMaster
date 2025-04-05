#pragma once
#include <Arduino.h>
#include "pins.h"
#include "handlers/profiles.h"
#include "timeManager.h"

// Helper function to convert "HH:MM" to seconds since midnight
int getTimeInSeconds(String timeStr)
{
  int colonIndex = timeStr.indexOf(':');
  int hours = timeStr.substring(0, colonIndex).toInt();
  int minutes = timeStr.substring(colonIndex + 1).toInt();
  return hours * 3600 + minutes * 60;
}

// Function to ring the bell
void ringBell(int duration)
{
  digitalWrite(BELL_PIN, LOW); // Activate the bell
  DebugPrintln(String(getCurrentTime()));
  DebugPrintln("Bell ringing...");
  delay(duration * 1000);       // Ring for the specified duration
  digitalWrite(BELL_PIN, HIGH); // Deactivate the bell
  DebugPrintln("Bell stopped.");
}

int getDayOfWeekIndex(const String &day)
{
  if (day == "sunday")
    return 0;
  if (day == "monday")
    return 1;
  if (day == "tuesday")
    return 2;
  if (day == "wednesday")
    return 3;
  if (day == "thursday")
    return 4;
  if (day == "friday")
    return 5;
  if (day == "saturday")
    return 6;
  if (day == "none")
    return 7;
  return -1; // Invalid day
}

// Function to check if it's time to ring the bell
void checkBellSchedule(int currentTime, const String &color)
{
  JsonDocument doc;
  if (!loadProfiles(doc))
  {
    DebugPrintln("Не удалось загрузить профили для расписания звонков.");
    return;
  }

  int currentDayOfWeek = getCurrentDayOfWeek();
  bool dayProfileFound = false;

  JsonArray profiles = doc.as<JsonArray>();
  for (JsonObject profile : profiles)
  {
    if (profile.containsKey("color") && profile.containsKey("day"))
    {
      String profileColor = profile["color"].as<String>();
      String day = profile["day"].as<String>();

      // Check if the profile is for the current day of the week
      if (currentDayOfWeek == getDayOfWeekIndex(day))
      {
        dayProfileFound = true;
        for (int i = 1; i <= 3; i++)
        { // Check each lesson in the profile
          String lessonKey = "lesson" + String(i);
          if (profile.containsKey(lessonKey))
          {
            JsonObject lesson = profile[lessonKey];
            String startTime = lesson["start"].as<String>();
            String endTime = lesson["end"].as<String>();

            // Convert "HH:MM" to seconds since the start of the day
            int startSeconds = getTimeInSeconds(startTime);
            int endSeconds = getTimeInSeconds(endTime);

            // Check if the current time falls within the lesson time range
            if (currentTime == startSeconds || currentTime == endSeconds)
            {
              ringBell(4); // Bell rings for 4 seconds (change as needed)
              return;      // Stop checking other profiles
            }
          }
        }
      }
    }
  }

  // If no profile for the current day was found, check the "any" day profile
  if (!dayProfileFound)
  {
    for (JsonObject profile : profiles)
    {
      if (profile.containsKey("color") && profile.containsKey("day"))
      {
        String profileColor = profile["color"].as<String>();
        String day = profile["day"].as<String>();

        if (day == "none" && profileColor == color)
        {
          for (int i = 1; i <= 3; i++)
          { // Check each lesson in the profile
            String lessonKey = "lesson" + String(i);
            if (profile.containsKey(lessonKey))
            {
              JsonObject lesson = profile[lessonKey];
              String startTime = lesson["start"].as<String>();
              String endTime = lesson["end"].as<String>();

              // Convert "HH:MM" to seconds since the start of the day
              int startSeconds = getTimeInSeconds(startTime);
              int endSeconds = getTimeInSeconds(endTime);

              // Check if the current time falls within the lesson time range
              if (currentTime == startSeconds || currentTime == endSeconds)
              {
                ringBell(4); // Bell rings for 4 seconds (change as needed)
                return;      // Stop checking other profiles
              }
            }
          }
        }
      }
    }
  }
}