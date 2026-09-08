#pragma once
#include <Arduino.h>
#include "pins.h"
#include "handlers/profiles.h"
#include "timeManager.h"

unsigned long bellStopTime = 0;
bool bellRinging = false;
int lastBellDay = -1;
int lastBellTime = -1;

int getTimeInSeconds(const String &timeStr)
{
  int colonIndex = timeStr.indexOf(':');
  if (colonIndex <= 0)
    return -1;

  int hours = timeStr.substring(0, colonIndex).toInt();
  int minutes = timeStr.substring(colonIndex + 1).toInt();
  if (hours < 0 || hours > 23 || minutes < 0 || minutes > 59)
    return -1;

  return hours * 3600 + minutes * 60;
}

void stopBellIfNeeded()
{
  if (bellRinging && static_cast<long>(millis() - bellStopTime) >= 0)
  {
    digitalWrite(BELL_PIN, HIGH);
    bellStopTime = 0;
    bellRinging = false;
    DebugPrintln("Bell stopped.");
  }
}

void ringBell()
{
  int duration = max(0, deviceSettings.bell_duration);
  digitalWrite(BELL_PIN, LOW);
  bellRinging = true;
  bellStopTime = millis() + static_cast<unsigned long>(duration) * 1000UL;
  DebugPrintln("Bell ringing...");
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

bool lessonMatchesTime(JsonObject lesson, int currentTime)
{
  if (!lesson["start"].is<String>() || !lesson["end"].is<String>())
    return false;

  int startSeconds = getTimeInSeconds(lesson["start"].as<String>());
  int endSeconds = getTimeInSeconds(lesson["end"].as<String>());
  return currentTime == startSeconds || currentTime == endSeconds;
}

bool profileHasBellEvent(JsonObject profile, int currentTime)
{
  for (int lessonIndex = 1;; lessonIndex++)
  {
    String lessonKey = "lesson" + String(lessonIndex);
    if (!profile[lessonKey].is<JsonObject>())
      return false;

    if (lessonMatchesTime(profile[lessonKey].as<JsonObject>(), currentTime))
      return true;
  }
}

bool profileMatchesSchedule(JsonObject profile, const String &profileName,
                            int currentDayOfWeek, int workMode,
                            bool dayProfileFound)
{
  if (workMode == 0)
    return profile["name"].is<String>() && profile["name"].as<String>() == profileName;

  if (workMode == 1)
    return profile["day"].is<String>() &&
           currentDayOfWeek == getDayOfWeekIndex(profile["day"].as<String>());

  if (workMode == 2 && profile["day"].is<String>())
  {
    String day = profile["day"].as<String>();
    return currentDayOfWeek == getDayOfWeekIndex(day) ||
           (day == "none" && !dayProfileFound);
  }

  return false;
}

void checkBellSchedule(int currentTime, int currentDayOfWeek,
                       const String &profileName, int workMode)
{
  stopBellIfNeeded();
  if (currentTime < 0 || bellRinging)
    return;

  JsonDocument doc;
  if (!loadProfiles(doc))
  {
    DebugPrintln("Не удалось загрузить профили для расписания звонков.");
    return;
  }

  JsonArray profiles = doc.as<JsonArray>();
  bool dayProfileFound = false;
  if (workMode == 2)
  {
    for (JsonObject profile : profiles)
    {
      if (profile["day"].is<String>() &&
          currentDayOfWeek == getDayOfWeekIndex(profile["day"].as<String>()))
        dayProfileFound = true;
    }
  }

  for (JsonObject profile : profiles)
  {
    if (profileMatchesSchedule(profile, profileName, currentDayOfWeek,
                               workMode, dayProfileFound) &&
        profileHasBellEvent(profile, currentTime))
    {
      if (lastBellDay == currentDayOfWeek && lastBellTime == currentTime)
        return;
      lastBellDay = currentDayOfWeek;
      lastBellTime = currentTime;
      ringBell();
      return;
    }
  }
}
