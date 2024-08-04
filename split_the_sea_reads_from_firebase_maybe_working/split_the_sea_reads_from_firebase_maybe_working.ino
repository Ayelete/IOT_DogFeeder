#include <ESPAsyncWebServer.h>
#include <AsyncHTTPRequest_Generic.h>
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

bool data_printed = false;

// External variables
extern unsigned long previousMillis;
extern unsigned long previousMillis2;

extern HX711 scale;
extern time_t now;
extern struct tm timeinfo;
extern bool ntp_initialized;

extern int motor_speed;
extern bool is_motor_running;

unsigned long currentMillis = 0;
bool function_called_today = false;

const char* projectId = "hellome-a9965";
const char* apiKey = "AIzaSyDQT20WMQz3lA-W4CJ4WLdKAYn_jy2Kj64";
const char* collectionPath = "Dogs/Billy/meals";

StaticJsonDocument<1024> JsonMeals;
JsonArray newArray = JsonMeals.to<JsonArray>();

AsyncHTTPRequest request;

void setup() {
  Serial.begin(115200);
  delay(5000);

  initMotor();
  initOLED();
  initScale();
  initWifi();
}

void handleHTTPResponse(void* optParm, AsyncHTTPRequest* request, int readyState) {
  if (readyState == readyStateDone) {
    if (request->responseHTTPcode() == 200) {
      String payload = request->responseText();
      StaticJsonDocument<1024> filter;
      filter["documents"][0]["fields"] = true;

      StaticJsonDocument<1024> filtereddoc;
      DeserializationError error = deserializeJson(filtereddoc, payload, DeserializationOption::Filter(filter), DeserializationOption::NestingLimit(13));

      if (error) {
        Serial.print("deserializeJson() failed: ");
        Serial.println(error.c_str());
      } else if (filtereddoc.containsKey("documents")) {
        JsonArray data = filtereddoc["documents"];
        for (JsonVariant item : data) {
          JsonObject obj = newArray.createNestedObject();
          obj["time"] = item["fields"]["hourOfMeal"]["stringValue"];
          obj["amount"] = item["fields"]["amountOfFood"]["stringValue"];
        }
      } else {
        Serial.println("No documents found in the collection.");
      }
    } else {
      Serial.println("Failed to read data from Firestore");
      Serial.println(request->responseHTTPcode());
    }
  }
}

void mySpecificFunction() {
  motor_speed = 500; // put minus to change direction
  Serial.println("Spinning Clockwise...");
  is_motor_running = true;
  motorRunningMessage();
}

void loop() {
  currentMillis = millis();

  initTimeIfConnectedToWifi();

  getMotorGoin();

  if (ntp_initialized) {
    if (getLocalTime(&timeinfo)) {
      printTimeToSerialMonitor();

      if (currentMillis - previousMillis >= 100000) {
        String firestoreUrl = "https://firestore.googleapis.com/v1/projects/" + String(projectId) + "/databases/(default)/documents/" + String(collectionPath) + "?key=" + String(apiKey);
        request.onReadyStateChange(handleHTTPResponse);
        request.open("GET", firestoreUrl.c_str());
        request.send();
        previousMillis = currentMillis;
      }

      if (timeinfo.tm_hour == 14 && timeinfo.tm_min == 51 && !function_called_today) {
        mySpecificFunction();
        function_called_today = true;
      }

      resetMealCountAtMidnight();
    } else {
      Serial.println("Failed to obtain time");
    }
  }

  if (currentMillis - previousMillis2 >= 1000) {
    previousMillis2 = currentMillis;
    float weight = getWeight();
    if (weight > 200 && is_motor_running) {
      stopMotor();
      weightReachedMessage();
    }
  }
}
