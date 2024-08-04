#include "ScaleManager.h"

extern unsigned long currentMillis;

// HX711 circuit wiring
const int LOADCELL_DOUT_PIN = 16;
const int LOADCELL_SCK_PIN = 4;
HX711 scale;


unsigned long previousMillis = 0; // store last time HX711 was read
unsigned long previousMillis2 = 0; // store last time HX711 was read
const long interval = 1000; // interval to read HX711 (milliseconds)
const float calibration_factor = 511.45;

// Initialize the HX711 scale
void initScale(){
scale.begin(LOADCELL_DOUT_PIN, LOADCELL_SCK_PIN);
  
  // Check if the scale is ready
  if (scale.is_ready()) {
    Serial.println("HX711 is ready.");
    scale.set_scale(calibration_factor); // Set the calibration factor
    scale.tare();  // Zero the scale
  } else {
    Serial.println("HX711 not found.");
  }
}

float getWeight(){
  float weight = -1000000000000000;
  if (scale.is_ready()) {
    weight = scale.get_units();
    Serial.print("Weight: ");
    Serial.print(weight);
    Serial.println(" grams");
  } 
  else {
    Serial.println("HX711 not found.");
  }

return weight;
}
  