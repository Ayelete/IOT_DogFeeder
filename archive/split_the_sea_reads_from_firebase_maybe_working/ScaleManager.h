#ifndef SCALEMANAGER_H
#define SCALEMANAGER_H

#include <HX711.h>

void initScale();
// Read the weight from HX711 every second
float getWeight();

#endif