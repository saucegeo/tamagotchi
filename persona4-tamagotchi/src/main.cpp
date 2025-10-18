#include <Arduino.h>
#include "Teddie.h"  // We'll add this file next

Teddie myTeddie;  // Create your virtual pet

void setup() {
  Serial.begin(9600);
  while (!Serial) {
    ; // Wait for serial port to connect (needed for native USB)
  }
  myTeddie.loadFromEEPROM();  // Load saved state
  Serial.println("Teddie Tamagotchi Started!");
  Serial.print("Initial Happiness: ");
  Serial.println(myTeddie.happiness);
}

void loop() {
  // Simulate time decay: Increase hunger every 5 seconds
  static unsigned long lastTimer = 0;
  if (millis() - lastTimer > 5000) {
    myTeddie.updateHunger(1);  // Hunger goes up
    myTeddie.saveToEEPROM();   // Persist changes
    Serial.print("Hunger now: ");
    Serial.println(myTeddie.hunger);
    lastTimer = millis();
  }
  delay(1000);
}
