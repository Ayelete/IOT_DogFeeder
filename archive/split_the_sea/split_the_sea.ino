
//some constants for motor
const int DIR_PIN = 12;
const int STEP_PIN = 14;
const int steps_per_rev = 200;

#define motorInterfaceType 1

// Flag to stop the motor if weight exceeds limit

#include <ESPAsyncWebServer.h>
#include <time.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SH110X.h>
#include <AccelStepper.h>
#include <HX711.h>

#include "WifiManager.h"
#include "ScaleManager.h"
#include "OledManager.h"
#include "ntpManager.h"
#include "MotorManager.h"

//external variables
extern unsigned long previousMillis;
extern unsigned long previousMillis2;

extern HX711 scale;
extern time_t now;
extern struct tm timeinfo;
extern bool ntp_initialized;

extern int motor_speed;
extern bool is_motor_running;
/////////////////////////////////////////////

//GLOBAL VARS
unsigned long currentMillis = 0;

// Flag to indicate if the function has been called for the current day
bool function_called_today = false;



void setup() {
  Serial.begin(115200);
  delay(5000);

  initMotor();

  initOLED();

  initScale();
  
  initWifi();
  }

void mySpecificFunction() {
  // Add your specific function logic here
  motor_speed = 500; // put minus to change direction
  Serial.println("Spinning Clockwise...");
  is_motor_running = true;

  motorRunningMessage();

}


void loop() {

  currentMillis = millis();

  initTimeIfConnectedToWifi();

  getMotorGoin();

  // If NTP is initialized, get and print current time
  if (ntp_initialized) {
    if (getLocalTime(&timeinfo)) {

    printTimeToSerialMonitor();
    
    // Check if the current time is 13:00 and if the function has not been called today
    if (timeinfo.tm_hour == 21 && timeinfo.tm_min == 5 && !function_called_today) {
      mySpecificFunction();
      function_called_today = true;
    }

    resetMealCountAtMidnight();
    
    } else {
      Serial.println("Failed to obtain time");
    }
  }

// Read the weight from HX711 every second
if (currentMillis - previousMillis >= 1000){
  previousMillis = currentMillis;

  //Serial.println("HERE///////////////////////////////////////////////////////////1");
  float weight = getWeight();
  //Serial.println("HERE///////////////////////////////////////////////////////////2");
  if (weight > 200 && is_motor_running) {
      stopMotor();
      weightReachedMessage();                    
    }
}
}
