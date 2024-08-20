#include "ScaleManager.h"
#include <Ewma.h>


extern unsigned long currentMillis;

// HX711 circuit wiring
const int LOADCELL_DOUT_PIN = 16;
const int LOADCELL_SCK_PIN = 4;
HX711 scale;

Ewma adcFilter(0.5);
Ewma adcFilter2(0.5);


unsigned long previousMillis = 0; // store last time HX711 was read
unsigned long previousMillis2 = 0; // store last time HX711 was read
const long interval = 1000; // interval to read HX711 (milliseconds)
const float calibration_factor = 435.47;
bool scale_initialized = false;
float start_weight = 0;


// Initialize the HX711 scale
void initScale(){
scale.begin(LOADCELL_DOUT_PIN, LOADCELL_SCK_PIN);
  
  // Check if the scale is ready
  int scale_attempts = 0;
  while(!scale.is_ready() && scale_attempts < 10){
      delay(500);
      scale_attempts++;
  }

  if (scale.is_ready()) {

    Serial.println("HX711 is ready.");
    scale.set_scale(calibration_factor); // Set the calibration factor
    scale.tare(); 
    // delay(1000);
    
    start_weight = scale.get_units(10);
    scale_initialized = true;
    Serial.print("START WEIGHT: ");
    Serial.println(start_weight);
  } else {
    Serial.println("HX711 not found.");
  }
}

float getWeight(){

  float weight = 0;
  float weight2 = 0;
  if (scale.is_ready()) {
    int eng = scale.get_units();
    float filtered = adcFilter2.filter(eng);

    weight = filtered;
    Serial.print("Weight: ");
    Serial.print(weight);
    Serial.println(" grams");

    // weight2 is less sensitive to noise, if it doesn't work in the tests uncomment this section and change the return value to weight2
    // weight2 = scale.get_units(10);
    // Serial.print("Weight2: ");
    // Serial.print(weight2);
    // Serial.println(" grams");
  } 
  else {
    Serial.println("HX711 not found.");
  }

return weight;
}
  