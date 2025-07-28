#include "MotorManager.h"


const int DIR_PIN = 12;
const int STEP_PIN = 14;
const int steps_per_rev = 200;


const int motorInterfaceType = 1;
AccelStepper stepper(motorInterfaceType, STEP_PIN, DIR_PIN);

void initMotor() {
    stepper.setMaxSpeed(2000);   
    stepper.setAcceleration(1000); 
}

void setMotorSpeed(int speed) {
    stepper.setSpeed(speed); 
}

void runMotor() {
    stepper.runSpeed(); 
}

void stopMotor() {
    stepper.setSpeed(0); 
}
