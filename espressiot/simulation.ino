//
// ESPressIoT Controller for Espresso Machines
// 2016-2021 by Roman Schmitz
//

#ifdef SIMULATION_MODE

#define SIM_TIME 100
#define SIM_T_START 20
#define SIM_T_LOSS 2.9e-2
#define SIM_T_HEAT 0.7787

float heaterSavedState = 0.0;
float curTemp = 0.0;
unsigned long lastSensTime = 0;

// Function prototypes
void setupSensor();
void setupHeater();
void updateTempSensor();
void updateHeater();
float generateRandomTemperatureOffset();
float getTemp();

void setupSensor() {
  curTemp = SIM_T_START;
}

void setupHeater() {
  heatcycles = 0;
}

void updateTempSensor() {
  if (abs(time_now - lastSensTime) >= SIM_TIME) {
    lastSensTime = time_now;
    curTemp = max(SIM_T_START, curTemp + (heaterSavedState * SIM_T_HEAT * 1e-3) - SIM_T_LOSS);
  }
}

void updateHeater() {
  heatCurrentTime = time_now;
  if (heatCurrentTime - heatLastTime >= 1000 or heatLastTime > heatCurrentTime) {
    heaterSavedState = getHeatCycles();
    heatLastTime = heatCurrentTime;
  }
}

float generateRandomTemperatureOffset() {
  return static_cast<float>(random(-10, 10)) / 100;
}

float getTemp() {
  return curTemp + generateRandomTemperatureOffset();
}

#endif
