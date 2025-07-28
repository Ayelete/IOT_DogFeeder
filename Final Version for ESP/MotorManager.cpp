#include "MotorManager.h"
#include <AccelStepper.h>
//some constants for motor
const int DIR_PIN = 12;
const int STEP_PIN = 14;
const int steps_per_rev = 200;

#define motorInterfaceType 1

// Flag to stop the motor if weight exceeds limit
volatile bool stop_motor = false;

unsigned long motorPreviousMillis = 0; // store last time motor was moved
const long motorInterval = 1000; // interval to control motor speed (microseconds)

int motor_speed = 0;
bool is_motor_running = false;

//initialize motor
  AccelStepper stepper(motorInterfaceType, STEP_PIN, DIR_PIN);

void initMotor(){
  // Initialize the stepper motor
  stepper.setMaxSpeed(2000);   // Set maximum speed
  stepper.setAcceleration(1000); // Set acceleration
}

void getMotorGoin(){
    stepper.setSpeed(motor_speed); // Set speed (positive for clockwise, negative for counterclockwise)
    stepper.runSpeed();
}

void stopMotor(){
    motor_speed = 0;
    is_motor_running = false;
    Serial.println("Motor stopped due to weight limit.");
}