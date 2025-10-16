#pragma once
#include <Arduino.h>
#include "pins.h"
#include "handlers/profiles.h"
#include "timeManager.h"

unsigned long bellStartTime = 0;
bool bellRinging = false;

int getTimeInSeconds(String timeStr)
{
  int colonIndex = timeStr.indexOf(':');
  int hours = timeStr.substring(0, colonIndex).toInt();
  int minutes = timeStr.substring(colonIndex + 1).toInt();
  return hours * 3600 + minutes * 60;
}

void ringBell()
{
  digitalWrite(BELL_PIN, LOW);
  DebugPrintln(String(getCurrentTime()));
  DebugPrintln("Bell ringing...");
  delay(deviceSettings.bell_duration * 1000);
  digitalWrite(BELL_PIN, HIGH);
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
  return -1;
}

void checkBellSchedule(int currentTime, const String &profileName, int workMode)
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

  DebugPrintln("Current time: " + String(currentTime));
  DebugPrintln("Current day of week: " + String(currentDayOfWeek));
  DebugPrintln("Work mode: " + String(workMode));

  if (workMode == 0)
  {
    DebugPrintln("Work mode 0");
    for (JsonObject profile : profiles)
    {
      if (profile.containsKey("name") && profile["name"].as<String>() == profileName)
      {
        DebugPrintln("Profile found: " + profileName);
        int lessonIndex = 1;
        while (true)
        {
          String lessonKey = "lesson" + String(lessonIndex);
          if (profile.containsKey(lessonKey))
          {
            JsonObject lesson = profile[lessonKey];
            String startTime = lesson["start"].as<String>();
            String endTime = lesson["end"].as<String>();

            int startSeconds = getTimeInSeconds(startTime);
            int endSeconds = getTimeInSeconds(endTime);

            DebugPrintln("Lesson " + String(lessonIndex) + ": Start=" + String(startSeconds) + ", End=" + String(endSeconds));

            if (currentTime == startSeconds || currentTime == endSeconds)
            {
              DebugPrintln("Bell should ring now");
              ringBell();
              Serial.println(currentTime);
              return;
            }
            lessonIndex++;
          }
          else
          {
            break;
          }
        }
      }
    }
  }
  else if (workMode == 1)
  {
    DebugPrintln("Work mode 1");
    for (JsonObject profile : profiles)
    {
      if (profile.containsKey("name") && profile.containsKey("day"))
      {
        String profileNameKey = profile["name"].as<String>();
        String day = profile["day"].as<String>();

        if (currentDayOfWeek == getDayOfWeekIndex(day))
        {
          dayProfileFound = true;
          DebugPrintln("Day profile found: " + day);
          int lessonIndex = 1;
          while (true)
          {
            String lessonKey = "lesson" + String(lessonIndex);
            if (profile.containsKey(lessonKey))
            {
              JsonObject lesson = profile[lessonKey];
              String startTime = lesson["start"].as<String>();
              String endTime = lesson["end"].as<String>();

              int startSeconds = getTimeInSeconds(startTime);
              int endSeconds = getTimeInSeconds(endTime);

              DebugPrintln("Lesson " + String(lessonIndex) + ": Start=" + String(startSeconds) + ", End=" + String(endSeconds));

              if (currentTime == startSeconds || currentTime == endSeconds)
              {
                DebugPrintln("Bell should ring now");
                ringBell();
                Serial.println(currentTime);
                return;
              }
              lessonIndex++;
            }
            else
            {
              break;
            }
          }
        }
      }
    }
  }
  else if (workMode == 2)
  {
    DebugPrintln("Work mode 2");
    
    // Сначала проверяем профили с днями (приоритетные)
    for (JsonObject profile : profiles)
    {
      if (profile.containsKey("day"))
      {
        String day = profile["day"].as<String>();
        
        if (currentDayOfWeek == getDayOfWeekIndex(day))
        {
          dayProfileFound = true;
          DebugPrintln("Day profile found: " + day);
          int lessonIndex = 1;
          while (true)
          {
            String lessonKey = "lesson" + String(lessonIndex);
            if (profile.containsKey(lessonKey))
            {
              JsonObject lesson = profile[lessonKey];
              String startTime = lesson["start"].as<String>();
              String endTime = lesson["end"].as<String>();

              int startSeconds = getTimeInSeconds(startTime);
              int endSeconds = getTimeInSeconds(endTime);

              DebugPrintln("Lesson " + String(lessonIndex) + ": Start=" + String(startSeconds) + ", End=" + String(endSeconds));

              if (currentTime == startSeconds || currentTime == endSeconds)
              {
                DebugPrintln("Bell should ring now (from day profile)");
                ringBell();
                Serial.println(currentTime);
                return;
              }
              lessonIndex++;
            }
            else
            {
              break;
            }
          }
        }
      }
    }

    // Если профиль с днем не найден, используем профиль с day="none"
    if (!dayProfileFound)
    {
      DebugPrintln("Day profile not found, looking for profile with day='none'");
      for (JsonObject profile : profiles)
      {
        if (profile.containsKey("day") && profile["day"].as<String>() == "none")
        {
          DebugPrintln("Profile with day='none' found");
          int lessonIndex = 1;
          while (true)
          {
            String lessonKey = "lesson" + String(lessonIndex);
            if (profile.containsKey(lessonKey))
            {
              JsonObject lesson = profile[lessonKey];
              String startTime = lesson["start"].as<String>();
              String endTime = lesson["end"].as<String>();

              int startSeconds = getTimeInSeconds(startTime);
              int endSeconds = getTimeInSeconds(endTime);

              DebugPrintln("Lesson " + String(lessonIndex) + ": Start=" + String(startSeconds) + ", End=" + String(endSeconds));

              if (currentTime == startSeconds || currentTime == endSeconds)
              {
                DebugPrintln("Bell should ring now (from profile with day='none')");
                ringBell();
                Serial.println(currentTime);
                return;
              }
              lessonIndex++;
            }
            else
            {
              break;
            }
          }
        }
      }
    }
  }
}
