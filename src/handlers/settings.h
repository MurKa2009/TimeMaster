#pragma once
#include <ArduinoJson.h>
#include <LittleFS.h>

#define SETTINGS_FILE "/settings.json" // Файл, где хранятся настройки

// Структура для хранения настроек устройства
struct DeviceSettings {
  String apSSID;          // SSID точки доступа
  String apPassword;      // Пароль точки доступа
  String wifiSSID;        // SSID WiFi для режима клиента
  String wifiPassword;    // Пароль WiFi для режима клиента
  bool useAPMode;         // Использовать ли режим точки доступа
  bool debugEnabled;      // Включен ли режим отладки
  String accountLogin;    // Логин для веб-интерфейса
  String accountPassword; // Пароль для веб-интерфейса
  String localDomain;     // mDNS доменное имя
  String ntpServer;       // NTP сервер для синхронизации времени
  String led1Color;       // Цвет для LED 1
  String led2Color;       // Цвет для LED 2
  String led3Color;       // Цвет для LED 3
  String led4Color;       // Цвет для LED 4
  String selectedColor;   // Выбранный цвет (новая переменная)
};

// Настройки по умолчанию, если файл настроек не существует
DeviceSettings deviceSettings = {
  "SchoolBellAP", "12345678", "", "", true, true, "admin", "admin", "esp", "time.nist.gov", "", "", "", "", ""
};

// Загрузка настроек из LittleFS
bool loadSettings() {
  if (!LittleFS.exists(SETTINGS_FILE)) return false; // Проверка существования файла настроек
  File file = LittleFS.open(SETTINGS_FILE, "r"); // Открытие файла для чтения
  if (!file) return false; // Возврат false, если файл не может быть открыт
  JsonDocument doc;
  DeserializationError error = deserializeJson(doc, file); // Парсинг JSON из файла
  file.close();
  if (error) return false; // Возврат false, если парсинг не удался

  // Загрузка настроек из JSON, использование значений по умолчанию, если значения отсутствуют
  deviceSettings.apSSID          = doc["apSSID"]          | "SchoolBellAP";
  deviceSettings.apPassword      = doc["apPassword"]      | "12345678";
  deviceSettings.wifiSSID        = doc["wifiSSID"]        | "";
  deviceSettings.wifiPassword    = doc["wifiPassword"]    | "";
  deviceSettings.useAPMode       = doc["useAPMode"]       | true;
  deviceSettings.debugEnabled    = doc["debugEnabled"]    | true;
  deviceSettings.accountLogin    = doc["accountLogin"]    | "admin";
  deviceSettings.accountPassword = doc["accountPassword"] | "admin";
  deviceSettings.localDomain     = doc["localDomain"]     | "esp";
  deviceSettings.ntpServer       = doc["ntpServer"]       | "time.nist.gov";
  deviceSettings.led1Color       = doc["led1Color"]       | "";
  deviceSettings.led2Color       = doc["led2Color"]       | "";
  deviceSettings.led3Color       = doc["led3Color"]       | "";
  deviceSettings.led4Color       = doc["led4Color"]       | "";
  deviceSettings.selectedColor   = doc["selectedColor"]   | ""; // Загрузка выбранного цвета
  return true;
}

// Сохранение настроек в LittleFS
bool saveSettings() {
  File file = LittleFS.open(SETTINGS_FILE, "w"); // Открытие файла для записи
  if (!file) return false; // Возврат false, если файл не может быть открыт
  JsonDocument doc;
  // Сохранение настроек в JSON
  doc["apSSID"]          = deviceSettings.apSSID;
  doc["apPassword"]      = deviceSettings.apPassword;
  doc["wifiSSID"]        = deviceSettings.wifiSSID;
  doc["wifiPassword"]    = deviceSettings.wifiPassword;
  doc["useAPMode"]       = deviceSettings.useAPMode;
  doc["debugEnabled"]    = deviceSettings.debugEnabled;
  doc["accountLogin"]    = deviceSettings.accountLogin;
  doc["accountPassword"] = deviceSettings.accountPassword;
  doc["localDomain"]     = deviceSettings.localDomain;
  doc["ntpServer"]       = deviceSettings.ntpServer;
  doc["led1Color"]       = deviceSettings.led1Color;
  doc["led2Color"]       = deviceSettings.led2Color;
  doc["led3Color"]       = deviceSettings.led3Color;
  doc["led4Color"]       = deviceSettings.led4Color;
  doc["selectedColor"]   = deviceSettings.selectedColor; // Сохранение выбранного цвета
  bool ok = (serializeJson(doc, file) > 0); // Запись JSON в файл
  file.close();
  return ok; // Возврат true, если успешно
}
