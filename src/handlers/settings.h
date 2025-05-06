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
  bool proksy_mod;        // Включение работы с прокси
  int bell_duration;      // Длительность звонка в секундах
  String proksy_ip;       // IP прокси
  int proksy_port;     // Порт прокси сервера
  String accountLogin;    // Логин для веб-интерфейса
  String accountPassword; // Пароль для веб-интерфейса
  String localDomain;     // mDNS доменное имя
  String ntpServer;       // NTP сервер для синхронизации времени
  String selectedProfile; // Выбранный профиль
  int selectedpoint;
  int workMode;           // Режим работы (число)
  int currentPage;
};

// Настройки по умолчанию, если файл настроек не существует
DeviceSettings deviceSettings = {
  "SchoolBellAP", "12345678", "HOME-111", "06061986", false, true, false, 2, "", 0, "admin", "admin", "esp", "time.nist.gov", "", 0, 2, 0
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
  deviceSettings.wifiSSID        = doc["wifiSSID"]        | "HOME-111";
  deviceSettings.wifiPassword    = doc["wifiPassword"]    | "06061986";
  deviceSettings.useAPMode       = doc["useAPMode"]       | true;
  deviceSettings.debugEnabled    = doc["debugEnabled"]    | true;
  deviceSettings.proksy_mod      = doc["proksy_mod"]      | true;
  deviceSettings.bell_duration   = doc["bell_duration"]   | 2;
  deviceSettings.proksy_ip       = doc["proksy_ip"]       | "";
  deviceSettings.proksy_port     = doc["proksy_port"]     | 0;
  deviceSettings.accountLogin    = doc["accountLogin"]    | "admin";
  deviceSettings.accountPassword = doc["accountPassword"] | "admin";
  deviceSettings.localDomain     = doc["localDomain"]     | "esp";
  deviceSettings.ntpServer       = doc["ntpServer"]       | "time.nist.gov"; 
  deviceSettings.selectedProfile = doc["selectedProfile"] | ""; 
  deviceSettings.selectedpoint   = doc["selectedpoint"]   | 0;
  deviceSettings.workMode        = doc["workMode"]        | 2;
  deviceSettings.currentPage     = doc["currentPage"]     | 0;
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
  doc["proksy_mod"]      = deviceSettings.proksy_mod;
  doc["bell_duration"]   = deviceSettings.bell_duration;
  doc["proksy_ip"]       = deviceSettings.proksy_ip;
  doc["proksy_port"]     = deviceSettings.proksy_port;
  doc["accountLogin"]    = deviceSettings.accountLogin;
  doc["accountPassword"] = deviceSettings.accountPassword;
  doc["localDomain"]     = deviceSettings.localDomain;
  doc["ntpServer"]       = deviceSettings.ntpServer;
  doc["selectedProfile"] = deviceSettings.selectedProfile; 
  doc["selectedpoint"]   = deviceSettings.selectedpoint;
  doc["workMode"]        = deviceSettings.workMode;
  doc["currentPage"]     = deviceSettings.currentPage;
  bool ok = (serializeJson(doc, file) > 0); // Запись JSON в файл
  file.close();
  return ok; // Возврат true, если успешно
}
