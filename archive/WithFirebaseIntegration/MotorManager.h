#ifndef MOTORMANAGER_H
#define MOTORMANAGER_H

#include <AccelStepper.h>

// Motor control constants
extern const int DIR_PIN;
extern const int STEP_PIN;
extern const int steps_per_rev;

// Function declarations
void initMotor();
void setMotorSpeed(int speed);
void runMotor();
void stopMotor(); 

#endif