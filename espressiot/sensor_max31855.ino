//
// ESPressIoT Controller for Espresso Machines
// 2016 by Roman Schmitz
//
// TSIC-Sensors using Arduino-TSIC-Library
//

#ifndef SIMULATION_MODE

#include "Adafruit_MAX31855.h"

#define ACCURACY 0.1
#define SMP_TIME 100

#define MAXDO   D6
#define MAXCS   D7
#define MAXCLK  D5

// Initialize the Thermocouple
Adafruit_MAX31855 thermocouple(MAXCLK, MAXCS, MAXDO);

float lastT = 0.0;
float SumT = 0.0;
float lastErr = 0.0;
int CntT = 0;
uint16_t raw_temp = 0;
unsigned long lastSensTime;

void setupSensor() {  
  lastSensTime=millis();
}

void updateTempSensor() {
  if (abs(millis() - lastSensTime) >= SMP_TIME) {
    if(thermocouple.readInternal()) {
      double curT = thermocouple.readCelsius();

      // very simple selection of noise hits/invalid values 
      if(abs(curT-lastT)<1.0 || lastT<1) {
        SumT+=curT;
        CntT++;        
      }     
      lastSensTime=millis();
    }
  }  
}

float getTemp() {
  float retVal=gInputTemp;

  if(CntT>=1) {
    retVal=(SumT/CntT);
    SumT=0.;
    CntT=0;
  }  

  return retVal;
}

#endif
