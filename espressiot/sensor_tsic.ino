//
// ESPressIoT Controller for Espresso Machines
// 2016-2021 by Roman Schmitz
//
// TSIC-Sensors using Arduino-TSIC-Library
//

#ifndef SIMULATION_MODE

#include <TSIC.h>

// Pins for power and signal
#define TSIC_SIG D4 // D4 pin of NodeMCU just in the right position
#define TSIC_SMP_TIME 100
#define ACCURACY 0.1

TSIC TSens1(TSIC_SIG);

float lastT = 0.0;
float SumT = 0.0;
float lastErr = 0.0;
int CntT = 0;
uint16_t raw_temp = 0;
unsigned long lastSensTime;

// Function prototypes
void setupSensor();
void updateTempSensor();
float calculateAverageTemperature();
float getTemp();

void setupSensor() {
  lastSensTime = millis();
}

void updateTempSensor() {
  if (millis() - lastSensTime >= TSIC_SMP_TIME) {
    if (TSens1.getTemperature(&raw_temp)) {
      float curT = TSens1.calc_Celsius(&raw_temp);

      // Very simple selection of noise hits/invalid values
      if (abs(curT - lastT) < 1.0 || lastT < 1) {
        SumT += curT;
        lastT = curT;
        CntT++;
      }
      lastSensTime = millis();
    }
  }
}

float calculateAverageTemperature() {
  float avgTemp = gInputTemp;

  if (CntT >= 1) {
    avgTemp = SumT / CntT;
    SumT = 0.0;
    CntT = 0;
  }

  return avgTemp;
}

float getTemp() {
  return calculateAverageTemperature();
}

#endif
