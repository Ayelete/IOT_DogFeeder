#include <Stepper.h>

// Define the number of steps per revolution for your motor
#define STEPS_PER_REV 200

// Create an instance of the Stepper class
Stepper stepper(STEPS_PER_REV, 8, 9, 10, 11); // Adjust pins as needed

void setup() {
  Serial.begin(9600); // Start serial communication at 9600 baud
  stepper.setSpeed(60); // Set the speed of the motor (in RPM)
}

void loop() {
  if (Serial.available() > 0) {
    String command = Serial.readStringUntil('\n'); // Read the incoming message

    if (command == "TURN_ON_MOTOR") {
      // Turn on the stepper motor
      while (1) { // Keep the motor running
        stepper.step(1); // Move one step at a time
        delay(1); // Adjust speed as needed
      }
    }
  }
}
