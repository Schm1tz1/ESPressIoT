//
// ESPressIoT Controller for Espresso Machines
// 2016-2021 by Roman Schmitz
//
// Simplified Heater PWM - just connect SSR to HEAT_RELAY_PIN

#define HEAT_RELAY_PIN D5 // D5 + GND are close on NodeMCU

float heatcycles = 0.0; // the number of millis out of 1000 for the current heat amount (percent * 10)
boolean heaterState = false;
unsigned long heatCurrentTime = 0, heatLastTime = 0;

#ifndef SIMULATION_MODE

void setupHeater() {
  pinMode(HEAT_RELAY_PIN, OUTPUT);
}

void updateHeater() {
  heatCurrentTime = millis();

  if (heatCurrentTime - heatLastTime >= HEATER_INTERVAL || heatLastTime > heatCurrentTime) {
    // begin cycle
    _turnHeatElementOnOff(true);
    heatLastTime = heatCurrentTime;
  }

  if (heatCurrentTime - heatLastTime >= heatcycles) {
    _turnHeatElementOnOff(false);
  }
}

#endif

void setHeatPowerPercentage(float power) {
  if (power < 0.0) {
    power = 0.0;
  }
  if (power > 1000.0) {
    power = 1000.0;
  }
  heatcycles = power;
}

float getHeatCycles() {
  return heatcycles;
}

void _turnHeatElementOnOff(boolean on) {
  digitalWrite(HEAT_RELAY_PIN, on);
  heaterState = on;
}

// End Heater Control
