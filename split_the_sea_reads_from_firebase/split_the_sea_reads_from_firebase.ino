
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

#include "ArduinoJson.h"
#include "HTTPClient.h"

bool data_printed = false;

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

const char* projectId = "hellome-a9965";
const char* apiKey = "AIzaSyDQT20WMQz3lA-W4CJ4WLdKAYn_jy2Kj64";
const char* collectionPath = "Dogs/Billy/meals";

JsonDocument JsonMeals;
JsonArray newArray = JsonMeals.to<JsonArray>();


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

    HTTPClient http;

    String firestoreUrl = "https://firestore.googleapis.com/v1/projects/" + String(projectId) + "/databases/(default)/documents/" + String(collectionPath) + "?key=" + String(apiKey);
    if (currentMillis - previousMillis >= 100000){
        http.begin(firestoreUrl);

    int httpCode = http.GET();
    if(httpCode == HTTP_CODE_OK){
      String payload = http.getString();

      JsonDocument filter;
      filter["documents"][0]["fields"] = true;

      JsonDocument filtereddoc;
      DeserializationError error =  deserializeJson(filtereddoc, payload, DeserializationOption::Filter(filter), DeserializationOption::NestingLimit(13));

      if(error){
        Serial.print("deserializeJson() failed: ");
        Serial.println(error.c_str());
        //todo stop
      }

      if(filtereddoc.containsKey("documents")){
        JsonArray data = filtereddoc["documents"];

        //for(JSonVariant item = data[0];item != data[n];item[i++]);
        for(JsonVariant item : data){
          JsonObject obj = newArray.createNestedObject();
          obj["time"] = item["fields"]["hourOfMeal"]["stringValue"];
          obj["amount"] = item["fields"]["amountOfFood"]["stringValue"];
        }
      }else{
        Serial.println("No documents found in the collection.");
      }

    } else {
      Serial.println("Failed to read data from Firestore");
      Serial.println(httpCode);
    }
    http.end();
    // if(data_printed == false){
    //   for(JsonVariant item : newArray){
    //   const char* t = item["time"];
    //   const char* a = item["amount"];
    //       Serial.print(t);
    //       Serial.print("  ");
    //       Serial.println(a);
    //     }
    // data_printed = true;
    // }
    
    }
    
    // Check if the current time is 13:00 and if the function has not been called today
    if (timeinfo.tm_hour == 14 && timeinfo.tm_min == 51 && !function_called_today) {
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
