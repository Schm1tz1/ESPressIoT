//
// ESPressIoT Controller for Espresso Machines
// 2016 by Roman Schmitz
//
// Uses PID library
//

#include <PID_v1.h>
#include <ESP8266WiFi.h>
#include "arduino_secrets.h"
#include "config.h"
#include "heater.h"
#include "sensor_max31855.h"
#include "tuning.h"
#include "web.h"

// WIFI


#define MAX_CONNECTION_RETRIES 20
#define BUTTON_PIN D1
#define HEAT_RELAY_PIN D3

// options for special modules
#define ENABLE_JSON
#define ENABLE_HTTP
//#define ENABLE_MQTT

// use simulation or real heater and sensors
//#define SIMULATION_MODE

//
// STANDARD reset values for Rancilio Silvia
//
#define S_P 91.0 //normal
#define S_I .26
#define S_D 7950
#define S_aP 100.0 //adaptive
#define S_aI 0.0
#define S_aD 0.0
#define S_TSET 100.0 //target temp
#define S_STEAM 120.0
#define S_TBAND 1.5

//
// Intervals for I/O
//
#define HEATER_INTERVAL 1000
#define DISPLAY_INTERVAL 1000
#define PID_INTERVAL 200

//
// global variables
//
double gTargetTemp=S_TSET;
double gSteamTemp=S_STEAM;
double gOvershoot=S_TBAND;
double gInputTemp=20.0;
double gOutputPwr=0.0;
double gP = S_P, gI = S_I, gD = S_D;
double gaP = S_aP, gaI = S_aI, gaD = S_aD;
//const unsigned int  HEAT_RELAY_PIN = 14;

unsigned long time_now=0;
unsigned long time_last=0;

boolean tuning = false;
boolean steaming = false;
boolean osmode = false;
boolean poweroffMode = false;
boolean fahrenheitOn = true;

//
// gloabl classes
//
PID ESPPID(&gInputTemp, &gOutputPwr, &gTargetTemp, gP, gI, gD, DIRECT);

void setup()
{
  gOutputPwr=0;
  pinMode(BUTTON_PIN, INPUT);
  Serial.begin(115200);

  Serial.println("Mounting SPIFFS...");
  if(!prepareFS()) {
    Serial.println("Failed to mount SPIFFS !");
  } else {
    Serial.println("Mounted.");
  }
 
  Serial.println("Loading config...");
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
   
  Serial.println("Settin up PID...");

  WiFi.begin(SECRET_SSID, SECRET_WIFI_PASS);
  Serial.println("");
  Serial.print("MAC address: ");
  Serial.println(WiFi.macAddress());

  Serial.print("Connecting to Wifi AP");
  for (int i=0; i<MAX_CONNECTION_RETRIES && WiFi.status()!=WL_CONNECTED; i++) {
    delay(500);
    Serial.print(".");
  }
  
  Serial.println("");
  Serial.println("WiFi connected.");
  Serial.print("IP address: ");
  Serial.println(WiFi.localIP());

  #ifdef ENABLE_HTTP
  setupWebSrv();
  #endif
  
  #ifdef ENABLE_MQTT
  setupMQTT();
  #endif

  // setup components
  setupHeater();
  setupSensor();

  // start PID
  ESPPID.SetTunings(gP, gI, gD);
  ESPPID.SetSampleTime(PID_INTERVAL);
  ESPPID.SetOutputLimits(0, 1000);
  ESPPID.SetMode(AUTOMATIC);
 
  time_now=millis();
  time_last=time_now;
    
}

void serialStatus() {
  Serial.print(gInputTemp, 2); Serial.print(" ");
  Serial.print(gTargetTemp, 2); Serial.print(" ");
  Serial.print(gOutputPwr, 2); Serial.print(" ");
  Serial.print(gP, 2); Serial.print(" ");
  Serial.print(gI, 2); Serial.print(" ");
  Serial.print(gD, 2); Serial.print(" ");
  Serial.print(ESPPID.GetKp(), 2); Serial.print(" ");
  Serial.print(ESPPID.GetKi(), 2); Serial.print(" ");
  Serial.print(ESPPID.GetKd(), 2);
  Serial.println("");
}

void loop() {
  time_now=millis();

  updateTempSensor(); 
  gInputTemp=getTemp();
  checkIfSteaming();


  if(abs(time_now-time_last)>=PID_INTERVAL or time_last > time_now) {
    if(poweroffMode==true) {
      gOutputPwr=0;
      setHeatPowerPercentage(0);
    }    
    else if(tuning==true)
    {
      tuning_loop();
    }
    else  {
      if( !osmode && abs(gTargetTemp-gInputTemp)>=gOvershoot ) {        
        ESPPID.SetTunings(gaP, gaI, gaD);
        osmode=true;
      }
      else if( osmode && abs(gTargetTemp-gInputTemp)<gOvershoot ) {
        ESPPID.SetTunings(gP,gI,gD);
        osmode=false;
      }
      if(ESPPID.Compute()==true) {   
        setHeatPowerPercentage(gOutputPwr);
      }
    }        
    
    #ifdef ENABLE_MQTT
    loopMQTT();
    #endif
    
    serialStatus();
    time_last=time_now;
  }

  updateHeater();
  
  #ifdef ENABLE_HTTP
  loopWebSrv();
  #endif 
  
}
