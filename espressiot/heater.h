#ifndef HEATER_H
#define HEATER_H

void checkIfSteaming(void);

void setupHeater(void);

void updateHeater(void);

void setHeatPowerPercentage(float);

float getHeatCycles(void);

void _turnHeatElementOnOff(bool);

#endif