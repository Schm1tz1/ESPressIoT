//
// ESPressIoT Controller for Espresso Machines
// 2024 by Pourya Parsa
//
// Thermostat Switch Detection - Reading the state of a digital GPIO used to detect the state of a switch/button
// together with a pull-up/down resistor (here: 100k to GND)
//

#define SWITCH_GPIO D7
#define SWITCH_SMP_TIME 100

#ifdef ENABLE_SWITCH_DETECTION

// Function prototypes
void setupSwitch();
void readSwitchState();

unsigned long lastSwitchTime;

void setupSwitch() {
  pinMode(SWITCH_GPIO, INPUT);
}

void readSwitchState() {
  gButtonState = digitalRead(SWITCH_GPIO);
}

void loopSwitch() {
  if (abs(millis() - lastSwitchTime) >= SWITCH_SMP_TIME) {
    readSwitchState();
    lastSwitchTime = millis();
  }
}

#endif
