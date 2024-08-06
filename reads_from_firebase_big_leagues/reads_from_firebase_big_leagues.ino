
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
#include "Arduino.h"

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
int hour = 0;
int minute = 1;

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

  // Create tasks for motor control and HTTP handling
  xTaskCreatePinnedToCore(motorControlTask, "MotorControl", 10000, NULL, 2, NULL, 0); // Higher priority
  xTaskCreatePinnedToCore(httpTask, "HTTP", 10000, NULL, 1, NULL, 1); // Lower priority
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
    }
    else {
      Serial.println("Failed to obtain time");
    }
  }


}

void motorControlTask(void *parameter) {
  while (true) {
    if(ntp_initialized && getLocalTime(&timeinfo)){
      // Check if the current time is 13:00 and if the function has not been called today
    if (timeinfo.tm_hour == hour && timeinfo.tm_min == minute && !function_called_today) {
      mySpecificFunction();
      function_called_today = true;
    }
    resetMealCountAtMidnight();
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

    // Using a non-blocking delay function
    vTaskDelay(100 / portTICK_PERIOD_MS); // Simulating work
  }
}

void httpTask(void *parameter) {
  while (true) {
    if (WiFi.status() == WL_CONNECTED && ntp_initialized) {
      HTTPClient http;
      String firestoreUrl = "https://firestore.googleapis.com/v1/projects/" + String(projectId) + "/databases/(default)/documents/" + String(collectionPath) + "?key=" + String(apiKey);
      http.begin(firestoreUrl);

      int httpCode = http.GET();
      if (httpCode == HTTP_CODE_OK) {
        String payload = http.getString();
        // Process payload
        Serial.println("HTTP GET successful, processing data...");
        DynamicJsonDocument doc(1024);
        DeserializationError error = deserializeJson(doc, payload);
        if (!error) {
          JsonArray documents = doc["documents"];
          for (JsonVariant v : documents) {
            String time_of_meal_string = v["fields"]["hourOfMeal"]["stringValue"].as<String>();
            Serial.println(time_of_meal_string);
            String amount_of_food_string = v["fields"]["amountOfFood"]["stringValue"].as<String>();
            Serial.println(amount_of_food_string);
            hour = time_of_meal_string.substring(0, 2).toInt();  // Extract the first two characters and convert to an integer
            minute = time_of_meal_string.substring(3, 5).toInt();  // Extract the next two characters after the colon and convert to an integer
          }
        } else {
          Serial.println("Failed to parse JSON");
        }
      } else {
        Serial.printf("HTTP GET failed, error: %d\n", httpCode);
      }
      http.end();
    } else {
      Serial.println("WiFi not connected");
    }
    vTaskDelay(5000 / portTICK_PERIOD_MS); // Run every 5 seconds
  }
}




























// void setup() {
//   Serial.begin(115200);
//   delay(5000);

//   WiFi.begin(ssid, password);
//   while (WiFi.status() != WL_CONNECTED) {
//     delay(1000);
//     Serial.println("Connecting to WiFi...");
//   }
//   Serial.println("Connected to WiFi");

//   // Create tasks for motor control and HTTP handling
//   xTaskCreatePinnedToCore(motorControlTask, "MotorControl", 10000, NULL, 2, NULL, 0); // Higher priority
//   xTaskCreatePinnedToCore(httpTask, "HTTP", 10000, NULL, 1, NULL, 1); // Lower priority
// }



// void motorControlTask(void *parameter) {
//   while (true) {
//     // Insert your motor control logic here
//     // This example just simulates motor control
//     Serial.println("Motor is running...");
//     // Using a non-blocking delay function
//     vTaskDelay(100 / portTICK_PERIOD_MS); // Simulating work
//   }
// }

// void httpTask(void *parameter) {
//   while (true) {
//     if (WiFi.status() == WL_CONNECTED) {
//       HTTPClient http;
//       String firestoreUrl = "https://firestore.googleapis.com/v1/projects/" + String(projectId) + "/databases/(default)/documents/" + String(collectionPath) + "?key=" + String(apiKey);
//       http.begin(firestoreUrl);

//       int httpCode = http.GET();
//       if (httpCode == HTTP_CODE_OK) {
//         String payload = http.getString();
//         // Process payload
//         Serial.println("HTTP GET successful, processing data...");
//         DynamicJsonDocument doc(1024);
//         DeserializationError error = deserializeJson(doc, payload);
//         if (!error) {
//           JsonArray documents = doc["documents"];
//           for (JsonVariant v : documents) {
//             Serial.println(v["fields"]["hourOfMeal"]["stringValue"].as<String>());
//             Serial.println(v["fields"]["amountOfFood"]["stringValue"].as<String>());
//           }
//         } else {
//           Serial.println("Failed to parse JSON");
//         }
//       } else {
//         Serial.printf("HTTP GET failed, error: %d\n", httpCode);
//       }
//       http.end();
//     } else {
//       Serial.println("WiFi not connected");
//     }
//     vTaskDelay(5000 / portTICK_PERIOD_MS); // Run every 5 seconds
//   }
// }





