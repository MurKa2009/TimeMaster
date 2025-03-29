#pragma once
#include <ArduinoJson.h>
#include <LittleFS.h>

#define PROFILES_FILE "/profiles.json" // File where profiles are stored

// Load profiles from LittleFS
bool loadProfiles(JsonDocument &doc) {
  if (!LittleFS.exists(PROFILES_FILE)) {
    // If the file doesn't exist, initialize doc as an empty JSON array
    deserializeJson(doc, "[]");
    return true;
  }
  File file = LittleFS.open(PROFILES_FILE, "r"); // Open file for reading
  if (!file) return false; // Return false if file cannot be opened
  DeserializationError error = deserializeJson(doc, file); // Parse JSON from file
  file.close();
  if (error) return false; // Return false if parsing fails
  
  if (!doc.is<JsonArray>()) {
    // If the JSON is not an array, reset it to an empty array
    deserializeJson(doc, "[]");
  }
  return true;
}

// Save profiles to LittleFS
bool saveProfiles(const JsonDocument &doc) {
  File file = LittleFS.open(PROFILES_FILE, "w"); // Open file for writing
  if (!file) return false; // Return false if file cannot be opened
  bool ok = (serializeJson(doc, file) > 0); // Write JSON to file
  file.close();
  return ok; // Return true if successful
}

// Save a single profile to the profiles file
bool saveProfile(const JsonObject &profile) {
  if (!profile["id"].is<const char*>()) return false; // Check if profile has an "id"
  const char* profileId = profile["id"];
  
  JsonDocument doc;
  if (!loadProfiles(doc)) { // Load existing profiles
    deserializeJson(doc, "[]"); // Initialize as empty array if loading fails
  }
  
  JsonArray profiles = doc.as<JsonArray>();
  bool found = false;
  for (JsonObject obj : profiles) {
    if (obj["id"].is<const char*>() && strcmp(obj["id"], profileId) == 0) {
      // If profile with the same ID exists, update it
      obj.clear();
      for (JsonPair kv : profile) {
        obj[kv.key()] = kv.value();
      }
      found = true;
      break;
    }
  }
  if (!found) {
    // If profile doesn't exist, add it
    JsonObject newProfile = profiles.add<JsonObject>();
    for (JsonPair kv : profile) {
      newProfile[kv.key()] = kv.value();
    }
  }
  return saveProfiles(doc); // Save updated profiles
}

// Load a profile by ID
bool loadProfile(const char* profileId, JsonObject &profileOut) {
  JsonDocument doc;
  if (!loadProfiles(doc)) return false; // Load profiles
  JsonArray profiles = doc.as<JsonArray>();
  for (JsonObject obj : profiles) {
    if (obj["id"].is<const char*>() && strcmp(obj["id"], profileId) == 0) {
      profileOut = obj; // Return the profile if found
      return true;
    }
  }
  return false; // Return false if profile not found
}

// Delete a profile by ID
bool deleteProfile(const char* profileId) {
  JsonDocument doc;
  if (!loadProfiles(doc)) return false; // Load profiles
  JsonArray profiles = doc.as<JsonArray>();
  
  JsonDocument newDoc;
  JsonArray newProfiles = newDoc.add<JsonArray>(); // Create a new array for updated profiles
  bool found = false;
  for (JsonObject obj : profiles) {
    if (obj["id"].is<const char*>() && strcmp(obj["id"], profileId) == 0) {
      found = true; // Skip the profile to delete
      continue;
    }
    JsonObject newObj = newProfiles.add<JsonObject>(); // Copy other profiles
    for (JsonPair kv : obj) {
      newObj[kv.key()] = kv.value();
    }
  }
  if (!found) return false; // Return false if profile not found
  return saveProfiles(newDoc); // Save updated profiles
}