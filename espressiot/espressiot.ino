#include <ArduinoJson.h>
#include <PID_v1.h>
#include <ESP8266WiFi.h>
#include "WiFiSecrets.h"

#define MAX_CONNECTION_RETRIES 20
#define ENABLE_JSON
#define ENABLE_HTTP
#define ENABLE_TELNET
#define ENABLE_MQTT
#define ENABLE_SWITCH_DETECTION
#define SIMULATION_MODE

#define S_P 115.0
#define S_I 4.0
#define S_D 850.0
#define S_aP 100.0
#define S_aI 0.0
#define S_aD 0.0
#define S_TSET 96.5
#define S_TBAND 1.5

#define HEATER_INTERVAL 1000
#define DISPLAY_INTERVAL 1000
#define PID_INTERVAL 200

double gTargetTemp = S_TSET;
double gOvershoot = S_TBAND;
double gInputTemp = 20.0;
double gOutputPwr = 0.0;
double gP = S_P, gI = S_I, gD = S_D;
double gaP = S_aP, gaI = S_aI, gaD = S_aD;

unsigned long time_now = 0;
unsigned long time_last = 0;

int gButtonState = 0;
uint8_t mac[6];

boolean tuning = false;
boolean osmode = false;
boolean poweroffMode = false;
boolean externalControlMode = false;

String gStatusAsJson;

PID ESPPID(&gInputTemp, &gOutputPwr, &gTargetTemp, gP, gI, gD, DIRECT);

void setup() {
  gOutputPwr = 0;
  Serial.begin(115200);

  if (!prepareFS()) {
    Serial.println("Failed to mount SPIFFS !");
  }

  if (!loadConfig()) {
    Serial.println("Failed to load config. Using default values and creating config...");
    if (!saveConfig()) {
      Serial.println("Failed to save config");
    } else {
      Serial.println("Config saved");
    }
  } else {
    Serial.println("Config loaded");
  }

  Serial.println("Setting up PID...");

  connectToWiFi();

#ifdef ENABLE_TELNET
  setupTelnet();
#endif

#ifdef ENABLE_HTTP
  setupWebSrv();
#endif

#ifdef ENABLE_MQTT
  setupMQTT();
#endif

#ifdef ENABLE_SWITCH_DETECTION
  setupSwitch();
#endif

  setupComponents();

  ESPPID.SetTunings(gP, gI, gD);
  ESPPID.SetSampleTime(PID_INTERVAL);
  ESPPID.SetOutputLimits(0, 1000);
  ESPPID.SetMode(AUTOMATIC);

  time_now = millis();
  time_last = time_now;
}

void loop() {
  time_now = millis();

  updateTempSensor();
  gInputTemp = getTemp();

#ifdef ENABLE_SWITCH_DETECTION
  loopSwitch();
#endif

  if (shouldRunPID()) {
    runPIDControl();
    updateStatus();
    time_last = time_now;
  }

  updateHeater();

#ifdef ENABLE_HTTP
  loopWebSrv();
#endif
}

