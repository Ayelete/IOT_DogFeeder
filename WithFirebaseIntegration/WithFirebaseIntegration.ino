#include <Adafruit_GFX.h>
#include <Adafruit_SH110X.h>

#include "WifiManager.h"
#include "OledManager.h"
#include "MotorManager.h"
#include "ScaleManager.h"
#include "NtpManager.h"
#include "FirebaseManager.h"

#include <Firebase_ESP_Client.h>
#include <ArduinoJson.h>

// Provide the token generation process info.
#include <addons/TokenHelper.h>

unsigned long previousMillis = 0; // store last time HX711 was read
unsigned long previousMillis2 = 0; // store last time HX711 was read
const long interval = 1000; // interval to read HX711 (milliseconds)
float weight = 0;
int motor_speed = 0;
bool is_motor_running = false;
std::map<String, bool> meal_called_today //Map to ensure that each meal will be called just one time a day

void checkIfItsFoodTimeAndIfYesEnjoy(const char* data, const String& mealName);
void runMotorUntilLimit( int amount);
void processMeals();

void setup() 
{
  Serial.begin(115200);
  delay(5000);

  initWifi();
  initOled();
  initMotor();
  initScale();
  initNtp();
  initFirebase();
}

void loop() 
{
  unsigned long currentMillis = millis();

  updateTime();
  setMotorSpeed(motor_speed); 
  runMotor();
  printCurrentTime(); 
 
  // If NTP is initialized, get and print current time
  if (ntp_initialized && currentMillis - previousMillis2 >= interval) 
  {
    previousMillis2 = currentMillis;
    processMeals();

    time_t now;
    struct tm timeinfo;
    if(getLocalTime(&timeinfo))
    {
      if(timeinfo.tm_hour == 0 && timeinfo.tm_min = 0)
      {
        meal_called_today.clear();
      }
    }
  }

  // Read the weight from HX711 every second
  if (currentMillis - previousMillis >= interval) 
  {
    previousMillis = currentMillis;
    float weight = readWeight();
    Serial.print("Weight: ");
    Serial.print(weight);
    Serial.println(" grams");

  }
}

void CheckIfItsFoodTimeAndIfYesEnjoy(const char *data, const String& mealName) *
{
    DynamicJsonDocument doc(2048);
    DeserializationError error = deserializeJson(doc, data);

    if (error) 
    {
        Serial.print("Failed to parse JSON: ");
        Serial.println(error.c_str());
        return;
    }

    struct tm timeinfo;
    if (!getLocalTime(&timeinfo)) 
    {
        Serial.println("Failed to obtain time");
        return;
    }

    char currentTime[6]; 
    strftime(currentTime, sizeof(currentTime), "%H:%M", &timeinfo);

    const char* hour = doc["fields"]["hour"]["stringValue"];
    int amount = doc["fields"]["amount"]["integerValue"];
    if (String(currentTime) == String(timeValue)) 
        {
            runMotorUntilLimit(amount);
            meal_called_today[mealName] = true;
        }
}

void runMotorUntilLimit(int amount) 
{
    motor_speed = 500; // Adjust as needed
    Serial.println("Spinning Clockwise...");
    is_motor_running = true;
    displayMotorRunning();

    while (is_motor_running) 
    {
        weight = readWeight();
        Serial.print("Weight: ");
        Serial.print(weight);
        Serial.println(" grams");

        if (weight > amount) 
        {
            stopMotor();
            is_motor_running = false;
            Serial.println("Motor stopped due to weight limit.");
            displayWeightLimit();                      
        }

        delay(500); 
    }
}

void processMeals()
{
  if(Firebase.getCollection(firebaseData, "Hours"))
      {
        String jsonData = FirebaseData.jsonString();
        DynamicJsonDocument = doc(2048);
        deserializeJson(doc, jsonData);
        JsonArray documents = doc["documents"].as<JsonArray>();

        for(JsonObject document : documents)
        {
          String mealName = document["name"].as<String>().substring(document["name"].as<String>().lastIndexOf('/')+1);
          Steing mealData = document.as<String>();
          CheckIfItsFoodTimeAndIfYesEnjoy(mealData.c_str(), mealName);
        }
      } else {
        Serial.print("Failed to get data from colelction: ");
        Seiral.println(firebaseData.errorReason());
      }
}
