#include "Battery.h"

int rawLevel = 0;
double batteryLevel = -1.0;

static unsigned long lastBatteryTime = 0;

void updateBattery() {

    unsigned long currentBatteryTime = millis();

    // Only calculate battery status every 3 seconds
    if (currentBatteryTime - lastBatteryTime >= 3000) {
        // Reset the timer
        lastBatteryTime = currentBatteryTime;


    rawLevel = M5.Power.getBatteryLevel();

    if (batteryLevel < 0) {
        batteryLevel = rawLevel; // First run
    } else {
        // Smoothed battery
        // Old battery level has a stronger weight than the new battery data being read
        batteryLevel = (batteryLevel * 0.97) + (rawLevel * 0.03);
    }
}

}