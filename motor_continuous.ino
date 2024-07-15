void loop() {
  DateTime now = rtc.now();

  // Check if the current time matches the specified time
  if (now.hour() == onHour && now.minute() == onMinute && now.second() == onSecond) {
    // Turn on the stepper motor
    while (1) { // Keep the motor running
      digitalWrite(stepPin, HIGH);
      delayMicroseconds(200); // Adjust speed as needed
      digitalWrite(stepPin, LOW);
      delayMicroseconds(200);
    }
  }
}