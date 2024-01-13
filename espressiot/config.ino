#ifdef ENABLE_JSON

#include <ArduinoJson.h>
#include <StreamUtils.h>
#include "FS.h"

#define BUF_SIZE 1024
#define CONFIG_FILE_PATH "/config.json"

bool prepareFS() {
  if (!SPIFFS.begin()) {
    Serial.println("Failed to mount file system");
    return false;
  }
  return true;
}

bool loadConfig() {
  File configFile = SPIFFS.open(CONFIG_FILE_PATH, "r");
  if (!configFile) {
    Serial.println("Failed to open config file");
    return false;
  }

  DynamicJsonDocument jsonDocument(BUF_SIZE);
  ReadLoggingStream loggingStream(configFile, Serial);

  DeserializationError parsingError = deserializeJson(jsonDocument, loggingStream);
  if (parsingError) {
    Serial.println("Failed to deserialize json config file");
    Serial.println(parsingError.c_str());
    return false;
  }

  // Assign values from jsonDocument to variables

  configFile.close();
  return true;
}

bool saveConfig() {
  DynamicJsonDocument jsonDocument(BUF_SIZE);

  // Fill jsonDocument with variables

  File configFile = SPIFFS.open(CONFIG_FILE_PATH, "w");
  if (!configFile) {
    Serial.println("Failed to open config file for writing");
    return false;
  }

  WriteLoggingStream loggedFile(configFile, Serial);

  size_t writtenBytes = serializeJson(jsonDocument, loggedFile);

  if (writtenBytes == 0) {
    Serial.println(F("Failed to write to file"));
    return false;
  }
  Serial.println("Bytes written: " + String(writtenBytes));

  configFile.close();
  return true;
}

void resetConfig() {
  // Reset variables
}

#endif
