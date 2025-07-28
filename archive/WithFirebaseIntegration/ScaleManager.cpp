#include "ScaleManager.h"

// HX711 circuit wiring
const int LOADCELL_DOUT_PIN = 16;
const int LOADCELL_SCK_PIN = 4;
HX711 scale;

const float calibration_factor = 511.45;

void initScale() {
    scale.begin(LOADCELL_DOUT_PIN, LOADCELL_SCK_PIN);
    
    // Check if the scale is ready
    if (scale.is_ready()) {
        Serial.println(F("HX711 is ready."));
        scale.set_scale(calibration_factor); // Set the calibration factor
        scale.tare();  // Zero the scale
    } else {
        Serial.println(F("HX711 not found."));
    }
}

float readWeight() {
    if (scale.is_ready()) {
        return scale.get_units();
    } else {
        Serial.println(F("HX711 not found."));
        return 0.0;
    }
}
