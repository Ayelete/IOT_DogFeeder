
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
// Arrays to hold meal times and amounts
int mealHours[5]; // Assuming a maximum of 5 meals
int mealMinutes[5];
int mealAmounts[5];
int mealCount = 0;

String days[7] = {"Sun" ,"Mon", "Tue", "Wed" ,"Thu", "Fri", "Sat"};

// Global variable to track the index of the active meal
int activeMealIndex = -1;

// Array of flags to indicate if the function has been called for each meal
bool function_called_for_meal[5] = {false, false, false, false, false};

const char* projectId = "hellome-a9965";
const char* apiKey = "AIzaSyDQT20WMQz3lA-W4CJ4WLdKAYn_jy2Kj64";
const char* collectionPath = "Dogs/Billy/meals";

String CurrMealName = "";

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
  //xTaskCreatePinnedToCore(motorControlTask, "MotorControl", 10000, NULL, 2, NULL, 0); // Higher priority
  //xTaskCreatePinnedToCore(httpTask, "HTTP", 10000, NULL, 1, NULL, 1); // Lower priority
  }

void mySpecificFunction() {
  // Add your specific function logic here
  motor_speed = 500; // put minus to change direction
  Serial.println("Spinning Clockwise...");
  is_motor_running = true;

  motorRunningMessage();

}

  bool check = false;
void loop() {
  currentMillis = millis();
  initTimeIfConnectedToWifi();
  getMotorGoin();


  

  // If NTP is initialized, get and print current time
  if (ntp_initialized) {
    if (getLocalTime(&timeinfo)) {
    //printTimeToSerialMonitor();
    }
    else {
      Serial.println("Failed to obtain time");
    }
    
    
  }
  if (WiFi.status() == WL_CONNECTED){
    if(!check){
      check = true;
      //update_current_amount(2000);
      //update_weight_at_the_end_of_meal(2000);
      //update_happend_false();
      //update_total_day_amount(timeinfo.tm_wday);
      //update_to_zero_given_until_now();
      //update_start_amount(2000);
      //update_zero_weight_at_the_end_of_meal();
      //update_zero_last_meal_name();
      //update_given_untill_now(20);
      //update_true_happened();
      update_name_of_the_last_meal("jFnsAcSxezvkCn5HxwRk");
    }
  }


}

//green after each meal
void update_given_untill_now(float weight){
    String currentMeal = "";
    char* documentPath = "Dogs/Billy/Statistics/globalStats";
    String firestoreUrl = "https://firestore.googleapis.com/v1/projects/" + String(projectId) + "/databases/(default)/documents/" + String(documentPath) + "?key=" + String(apiKey);
    HTTPClient http;
    http.begin(firestoreUrl);
    //http.setTimeout(3000);
    int httpCode = http.GET();
    if (httpCode != HTTP_CODE_OK){
      Serial.printf("HTTP GET failed, error: %d\n", httpCode);
    }else{
        String payload = http.getString();
        // Process payload
        Serial.println("HTTP GET 1, processing data...");
        DynamicJsonDocument doc(1024);
        DeserializationError error = deserializeJson(doc, payload);
        if (!error) {
          DynamicJsonDocument updateDoc(1024);
          JsonObject fields = updateDoc.createNestedObject("fields");
          currentMeal = doc["fields"]["lastMealName"]["stringValue"].as<String>();
        }else{
            Serial.println("deserialize failed");
        }
    }
    http.end();

    if(currentMeal == "-"){
      return;
    }

    Serial.println(currentMeal);

    int amountOfFood = 0;
    char* documentPath2 = "Dogs/Billy/meals/";
    String firestoreUrl2 = "https://firestore.googleapis.com/v1/projects/" + String(projectId) + "/databases/(default)/documents/" + String(documentPath2)  + currentMeal + "?key=" + String(apiKey);
    HTTPClient http2;
    http2.begin(firestoreUrl2);
    //http2.setTimeout(3000);
    int httpCode2 = http2.GET();
    if (httpCode2 != HTTP_CODE_OK){
      Serial.printf("HTTP GET failed, error: %d\n", httpCode2);
    }else{
        String payload = http2.getString();
        // Process payload
        Serial.println("HTTP GET 2, processing data...");
        DynamicJsonDocument doc(1024);
        DeserializationError error = deserializeJson(doc, payload);
        if (!error) {
          Serial.println(doc["fields"]["amountOfFood"]["stringValue"].as<String>());
          amountOfFood = (doc["fields"]["amountOfFood"]["stringValue"].as<String>()).toInt();
          Serial.println(amountOfFood);
        }else{
            Serial.println("deserialize failed");
        }
    }
    http2.end();


    char* documentPath1 = "Dogs/Billy/Statistics/currentDay";
    String firestoreUrl1 = "https://firestore.googleapis.com/v1/projects/" + String(projectId) + "/databases/(default)/documents/" + String(documentPath1) + "?key=" + String(apiKey);
    HTTPClient http1;
    http1.begin(firestoreUrl1);
    //http1.setTimeout(3000);
    int httpCode1 = http1.GET();
     if (httpCode1 != HTTP_CODE_OK){

     }else{
        String payload = http1.getString();
        // Process payload
        Serial.println("HTTP GET curr, processing data...");
        DynamicJsonDocument doc(1024);
        DeserializationError error = deserializeJson(doc, payload);
        DynamicJsonDocument updateDoc(1024);
        JsonObject fields = updateDoc.createNestedObject("fields");
        fields["currentAmount"]["integerValue"] = (int)doc["fields"]["currentAmount"]["integerValue"];
        fields["givenUntilNow"]["integerValue"] = amountOfFood - ((int)weight);
        fields["startAmount"]["integerValue"] = (int)doc["fields"]["startAmount"]["integerValue"];

        String jsonPayload;
        serializeJson(updateDoc,jsonPayload);

        http1.addHeader("Content-Type", "application/json");
        int patchCode = http1.PATCH(jsonPayload);
        if(patchCode == HTTP_CODE_OK){
            Serial.println("Document updated successfully");
        }else{
            Serial.print("Faild to update document  ");
            Serial.println(patchCode);
        }
     }
     http1.end();
}
void update_true_happened(){
    String currentMeal = "";
    char* documentPath = "Dogs/Billy/Statistics/globalStats";
    String firestoreUrl = "https://firestore.googleapis.com/v1/projects/" + String(projectId) + "/databases/(default)/documents/" + String(documentPath) + "?key=" + String(apiKey);
    HTTPClient http;
    http.begin(firestoreUrl);
    //http.setTimeout(3000);
    int httpCode = http.GET();
    if (httpCode != HTTP_CODE_OK){
      Serial.printf("HTTP GET failed, error: %d\n", httpCode);
    }else{
        String payload = http.getString();
        // Process payload
        Serial.println("HTTP GET 1, processing data...");
        DynamicJsonDocument doc(1024);
        DeserializationError error = deserializeJson(doc, payload);
        if (!error) {
          DynamicJsonDocument updateDoc(1024);
          JsonObject fields = updateDoc.createNestedObject("fields");
          currentMeal = doc["fields"]["lastMealName"]["stringValue"].as<String>();
        }else{
            Serial.println("deserialize failed");
        }
    }
    http.end();

    if(currentMeal == "-"){
      return;
    }

    
    char* documentPath1 = "Dogs/Billy/meals/";
    String firestoreUrl1 = "https://firestore.googleapis.com/v1/projects/" + String(projectId) + "/databases/(default)/documents/" + String(documentPath1) + currentMeal + "?key=" + String(apiKey);
    HTTPClient http1;
    http1.begin(firestoreUrl1);
    //http1.setTimeout(3000);
    int httpCode1 = http1.GET();
     if (httpCode1 != HTTP_CODE_OK){

     }else{
        String payload = http1.getString();
        // Process payload
        Serial.println("HTTP GET curr, processing data...");
        DynamicJsonDocument doc(1024);
        DeserializationError error = deserializeJson(doc, payload);
        DynamicJsonDocument updateDoc(1024);
        JsonObject fields = updateDoc.createNestedObject("fields");
        fields["amountOfFood"]["stringValue"] = doc["fields"]["amountOfFood"]["stringValue"].as<String>();
        fields["happened"]["booleanValue"] = true;
        fields["weightAtTheEndOfTheMeal"]["integerValue"] = (int)doc["fields"]["weightAtTheEndOfTheMeal"]["integerValue"];
        fields["hourOfMeal"]["stringValue"] = doc["fields"]["hourOfMeal"]["stringValue"].as<String>();

        String jsonPayload;
        serializeJson(updateDoc,jsonPayload);

        http1.addHeader("Content-Type", "application/json");
        int patchCode = http1.PATCH(jsonPayload);
        if(patchCode == HTTP_CODE_OK){
            Serial.println("Document updated successfully");
        }else{
            Serial.print("Faild to update document  ");
            Serial.println(patchCode);
        }
     }
     http1.end();
}
void update_name_of_the_last_meal(String name){
  char* documentPath1 = "Dogs/Billy/Statistics/globalStats";
    String firestoreUrl1 = "https://firestore.googleapis.com/v1/projects/" + String(projectId) + "/databases/(default)/documents/" + String(documentPath1) + "?key=" + String(apiKey);
    HTTPClient http1;
    http1.begin(firestoreUrl1);
    int httpCode1 = http1.GET();
     if (httpCode1 != HTTP_CODE_OK){

     }else{
        String payload = http1.getString();
        // Process payload
        Serial.println("HTTP GET curr, processing data...");
        DynamicJsonDocument doc(1024);
        DeserializationError error = deserializeJson(doc, payload);
        DynamicJsonDocument updateDoc(1024);
        JsonObject fields = updateDoc.createNestedObject("fields");
        fields["lastMealName"]["stringValue"] = name;
        for(int i = 0; i < 7; i++){
          fields[days[i]]["integerValue"] = doc["fields"][days[i]]["integerValue"];
        }
        String jsonPayload;
        serializeJson(updateDoc,jsonPayload);

        http1.addHeader("Content-Type", "application/json");
        int patchCode = http1.PATCH(jsonPayload);
        if(patchCode == HTTP_CODE_OK){
            Serial.println("Document updated successfully");
        }else{
            Serial.print("Faild to update document  ");
            Serial.println(patchCode);
        }
     }
     http1.end();
}

//yellow 00:01
void update_start_amount(float weight){
      char* documentPath = "Dogs/Billy/Statistics/currentDay";
    String firestoreUrl = "https://firestore.googleapis.com/v1/projects/" + String(projectId) + "/databases/(default)/documents/" + String(documentPath) + "?key=" + String(apiKey);
    HTTPClient http;
    http.begin(firestoreUrl);

    int httpCode = http.GET();
      if (httpCode == HTTP_CODE_OK) {
        String payload = http.getString();
        // Process payload
        Serial.println("HTTP GET successful, processing data...");
        DynamicJsonDocument doc(1024);
        DeserializationError error = deserializeJson(doc, payload);
        if (!error) {

          DynamicJsonDocument updateDoc(1024);
          JsonObject fields = updateDoc.createNestedObject("fields");
          
          fields["currentAmount"]["integerValue"] = doc["fields"]["currentAmount"]["integerValue"];
          fields["startAmount"]["integerValue"] = (int)weight;
          fields["givenUntilNow"]["integerValue"] = doc["fields"]["givenUntilNow"]["integerValue"];

          String jsonPayload;
          serializeJson(updateDoc,jsonPayload);

          http.addHeader("Content-Type", "application/json");
          int patchCode = http.PATCH(jsonPayload);

          if(patchCode == HTTP_CODE_OK){
            Serial.println("Document updated successfully");
          }else{
            Serial.print("Faild to update document  ");
            Serial.println(patchCode);
          }
        }
 else {
          Serial.println("Failed to parse JSON");
        }
      } else {
        Serial.printf("HTTP GET failed, error: %d\n", httpCode);
      }
      http.end();
}

//purple 23:59
void update_zero_last_meal_name(){
    
    //String amount = 0; 

    char* documentPath1 = "Dogs/Billy/Statistics/globalStats";
    String firestoreUrl1 = "https://firestore.googleapis.com/v1/projects/" + String(projectId) + "/databases/(default)/documents/" + String(documentPath1) + "?key=" + String(apiKey);
    HTTPClient http1;
    http1.begin(firestoreUrl1);
    int httpCode1 = http1.GET();
     if (httpCode1 != HTTP_CODE_OK){

     }else{
        String payload = http1.getString();
        // Process payload
        Serial.println("HTTP GET curr, processing data...");
        DynamicJsonDocument doc(1024);
        DeserializationError error = deserializeJson(doc, payload);
        DynamicJsonDocument updateDoc(1024);
        JsonObject fields = updateDoc.createNestedObject("fields");
        fields["lastMealName"]["stringValue"] = "-";
        for(int i = 0; i < 7; i++){
          fields[days[i]]["integerValue"] = doc["fields"][days[i]]["integerValue"];
        }
        String jsonPayload;
        serializeJson(updateDoc,jsonPayload);

        http1.addHeader("Content-Type", "application/json");
        int patchCode = http1.PATCH(jsonPayload);
        if(patchCode == HTTP_CODE_OK){
            Serial.println("Document updated successfully");
        }else{
            Serial.print("Faild to update document  ");
            Serial.println(patchCode);
        }
     }
     http1.end();
}
void update_zero_weight_at_the_end_of_meal(){
    char* documentPath = "Dogs/Billy/Statistics/globalStats";
    String firestoreUrl = "https://firestore.googleapis.com/v1/projects/" + String(projectId) + "/databases/(default)/documents/" + String(documentPath) + "?key=" + String(apiKey);
    HTTPClient http;
    http.begin(firestoreUrl);

    int httpCode = http.GET();
      if (httpCode == HTTP_CODE_OK) {
        String payload = http.getString();
        // Process payload
        Serial.println("HTTP GET successful, processing data...");
        DynamicJsonDocument doc(1024);
        DeserializationError error = deserializeJson(doc, payload);
        if (!error) {
        String mealNumber = doc["fields"]["lastMealName"]["stringValue"];
        Serial.println(mealNumber);
        char* documentPath1 = "Dogs/Billy/meals/";
        String firestoreUrl1 = "https://firestore.googleapis.com/v1/projects/" + String(projectId) + "/databases/(default)/documents/" + String(documentPath1) + mealNumber + "/?key=" + String(apiKey);
        
        HTTPClient http1;
        http1.begin(firestoreUrl1);
        int httpCode1 = http1.GET();
        if (httpCode1 == HTTP_CODE_OK) {
            String payload1 = http1.getString();
            DynamicJsonDocument doc1(1024);
            DeserializationError error1 = deserializeJson(doc1, payload1);
            if(error1){
              Serial.println("Failed to parse JSON");
            }else{
                DynamicJsonDocument updateDoc(1024);
                JsonObject fields = updateDoc.createNestedObject("fields");
                fields["weightAtTheEndOfTheMeal"]["integerValue"] = (int)0;
                fields["hourOfMeal"]["stringValue"] = doc1["fields"]["hourOfMeal"]["stringValue"];
                fields["happened"]["booleanValue"] = doc1["fields"]["happened"]["booleanValue"];
                fields["amountOfFood"]["stringValue"] = doc1["fields"]["amountOfFood"]["stringValue"];

            String jsonPayload;
            serializeJson(updateDoc,jsonPayload);

            http.addHeader("Content-Type", "application/json");
            int patchCode = http1.PATCH(jsonPayload);

          if(patchCode == HTTP_CODE_OK){
            Serial.println("Document updated successfully");
          }else{
            Serial.print("Faild to update document  ");
            Serial.println(patchCode);
          }
                
            }
            http1.end();
        }
        
        else{
          Serial.printf("HTTP GET failed, error: %d\n", httpCode1);
        }


        }
 else {
          Serial.println("Failed to parse JSON");
        }
      } else {
        Serial.printf("HTTP GET failed, error: %d\n", httpCode);
      }
      http.end();
}
void update_to_zero_given_until_now(){
    char* documentPath = "Dogs/Billy/Statistics/currentDay";
    String firestoreUrl = "https://firestore.googleapis.com/v1/projects/" + String(projectId) + "/databases/(default)/documents/" + String(documentPath) + "?key=" + String(apiKey);
    HTTPClient http;
    http.begin(firestoreUrl);
    int httpCode = http.GET();
    if (httpCode != HTTP_CODE_OK){
      Serial.printf("HTTP GET failed, error: %d\n", httpCode);
    }
    else{
      String payload = http.getString();
        // Process payload
        Serial.println("HTTP GET successful, processing data...");
        DynamicJsonDocument doc(1024);
        DeserializationError error = deserializeJson(doc, payload);
        if(error){
          Serial.println("Failed to parse JSON");
        }else{

            DynamicJsonDocument updateDoc(1024);
            JsonObject fields = updateDoc.createNestedObject("fields");
            fields["currentAmount"]["integerValue"] = (int)doc["fields"]["currentAmount"]["integerValue"];
            fields["givenUntilNow"]["integerValue"] = (int)0;
            fields["startAmount"]["integerValue"] = (int)doc["fields"]["startAmount"]["integerValue"];

            String jsonPayload;
            serializeJson(updateDoc,jsonPayload);

            http.addHeader("Content-Type", "application/json");
            int patchCode = http.PATCH(jsonPayload);
            
            if(patchCode == HTTP_CODE_OK){
              Serial.println("Document updated successfully");
            }else{
              Serial.print("Faild to update document  ");
              Serial.println(patchCode);
            }
        }
    }
    http.end();
}
void update_total_day_amount(int day){
  int amount = 0;
  char* documentPath = "Dogs/Billy/Statistics/currentDay";
    String firestoreUrl = "https://firestore.googleapis.com/v1/projects/" + String(projectId) + "/databases/(default)/documents/" + String(documentPath) + "?key=" + String(apiKey);
    HTTPClient http;
    http.begin(firestoreUrl);
    int httpCode = http.GET();
    if (httpCode != HTTP_CODE_OK){
      Serial.printf("HTTP GET failed, error: %d\n", httpCode);
    }else{
        String payload = http.getString();
        // Process payload
        Serial.println("HTTP GET currentDay, processing data...");
        DynamicJsonDocument doc(1024);
        DeserializationError error = deserializeJson(doc, payload);
        if (!error) {
          DynamicJsonDocument updateDoc(1024);
          JsonObject fields = updateDoc.createNestedObject("fields");
          amount = doc["fields"]["startAmount"]["integerValue"].as<int>() + doc["fields"]["givenUntilNow"]["integerValue"].as<int>() - doc["fields"]["currentAmount"]["integerValue"].as<int>();
        }else{
            Serial.println("deserialize failed");
        }
    }
    http.end();

    Serial.println(amount);

    char* documentPath1 = "Dogs/Billy/Statistics/globalStats";
    String firestoreUrl1 = "https://firestore.googleapis.com/v1/projects/" + String(projectId) + "/databases/(default)/documents/" + String(documentPath1) + "?key=" + String(apiKey);
    HTTPClient http1;
    http1.begin(firestoreUrl1);
    int httpCode1 = http1.GET();
     if (httpCode1 != HTTP_CODE_OK){

     }else{
        String payload = http1.getString();
        // Process payload
        Serial.println("HTTP GET curr, processing data...");
        DynamicJsonDocument doc(1024);
        DeserializationError error = deserializeJson(doc, payload);
        DynamicJsonDocument updateDoc(1024);
        JsonObject fields = updateDoc.createNestedObject("fields");
        fields["lastMealName"]["stringValue"] = doc["fields"]["lastMealName"]["stringValue"];
        for(int i = 0; i < 7; i++){
          if(i == day){
            fields[days[i]]["integerValue"] = amount;
            continue;
          }
          fields[days[i]]["integerValue"] = doc["fields"][days[i]]["integerValue"];
        }
        String jsonPayload;
        serializeJson(updateDoc,jsonPayload);

        http1.addHeader("Content-Type", "application/json");
        int patchCode = http1.PATCH(jsonPayload);
        if(patchCode == HTTP_CODE_OK){
            Serial.println("Document updated successfully");
        }else{
            Serial.print("Faild to update document  ");
            Serial.println(patchCode);
        }
     }
     http1.end();

}
void update_happend_false(){
  char* documentPath = "Dogs/Billy/meals/";
    String firestoreUrl = "https://firestore.googleapis.com/v1/projects/" + String(projectId) + "/databases/(default)/documents/" + String(documentPath) + "?key=" + String(apiKey);
    HTTPClient http;
    http.begin(firestoreUrl);

    int httpCode = http.GET();
      if (httpCode == HTTP_CODE_OK) {
        String payload = http.getString();
        // Process payload
        Serial.println("HTTP GET happened, processing data...");
        DynamicJsonDocument doc(4096);
        DeserializationError error = deserializeJson(doc, payload);
        if (!error) {
           JsonArray documents = doc["documents"].as<JsonArray>();

           for(JsonVariant item : documents){
            JsonObject document = item.as<JsonObject>();

            String name = item["name"].as<String>();
            String firestoreUrl1 = "https://firestore.googleapis.com/v1/"+ name +"?key=" + String(apiKey);
            HTTPClient http1;
            http1.begin(firestoreUrl1);
            http1.setTimeout(3000);
            int httpCode1 = http1.GET();
            
            String payload1 = http1.getString();
            DynamicJsonDocument doc1(1024);
            DeserializationError error1 = deserializeJson(doc1, payload1);
            

            DynamicJsonDocument updateDoc(1024);
            JsonObject fields = updateDoc.createNestedObject("fields");
            fields["happened"]["booleanValue"] = false;
            fields["weightAtTheEndOfTheMeal"]["integerValue"] = (int)doc1["fields"]["weightAtTheEndOfTheMeal"]["integerValue"];
            fields["hourOfMeal"]["stringValue"] = doc1["fields"]["hourOfMeal"]["stringValue"];
            fields["amountOfFood"]["stringValue"] = doc1["fields"]["amountOfFood"]["stringValue"];


            String jsonPayload;
            serializeJson(updateDoc,jsonPayload);

            http1.addHeader("Content-Type", "application/json");
            int patchCode = http1.PATCH(jsonPayload);
            
            if(patchCode == HTTP_CODE_OK){
              Serial.println("Document updated successfully");
            }else{
              Serial.print("Faild to update document  ");
              Serial.println(patchCode);
            }
            http1.end();
          }




          
        }
 else {
          Serial.println("Failed to parse JSON");
        }
      } else {
        Serial.printf("HTTP GET failed, error: %d\n", httpCode);
      }
      http.end();
}

//blue always
void update_weight_at_the_end_of_meal(float weight){
    char* documentPath = "Dogs/Billy/Statistics/globalStats";
    String firestoreUrl = "https://firestore.googleapis.com/v1/projects/" + String(projectId) + "/databases/(default)/documents/" + String(documentPath) + "?key=" + String(apiKey);
    HTTPClient http;
    http.begin(firestoreUrl);

    int httpCode = http.GET();
      if (httpCode == HTTP_CODE_OK) {
        String payload = http.getString();
        // Process payload
        Serial.println("HTTP GET successful, processing data...");
        DynamicJsonDocument doc(1024);
        DeserializationError error = deserializeJson(doc, payload);
        if (!error) {
        String mealNumber = doc["fields"]["lastMealName"]["stringValue"];
        Serial.println(mealNumber);
        char* documentPath1 = "Dogs/Billy/meals/";
        String firestoreUrl1 = "https://firestore.googleapis.com/v1/projects/" + String(projectId) + "/databases/(default)/documents/" + String(documentPath1) + mealNumber + "/?key=" + String(apiKey);
        
        HTTPClient http1;
        http1.begin(firestoreUrl1);
        int httpCode1 = http1.GET();
        if (httpCode1 == HTTP_CODE_OK) {
            String payload1 = http1.getString();
            DynamicJsonDocument doc1(1024);
            DeserializationError error1 = deserializeJson(doc1, payload1);
            if(error1){
              Serial.println("Failed to parse JSON");
            }else{
                DynamicJsonDocument updateDoc(1024);
                JsonObject fields = updateDoc.createNestedObject("fields");
                fields["weightAtTheEndOfTheMeal"]["integerValue"] = (int)weight;
                fields["hourOfMeal"]["stringValue"] = doc1["fields"]["hourOfMeal"]["stringValue"];
                fields["happened"]["booleanValue"] = doc1["fields"]["happened"]["booleanValue"];
                fields["amountOfFood"]["stringValue"] = doc1["fields"]["amountOfFood"]["stringValue"];

            String jsonPayload;
            serializeJson(updateDoc,jsonPayload);

            http.addHeader("Content-Type", "application/json");
            int patchCode = http1.PATCH(jsonPayload);

          if(patchCode == HTTP_CODE_OK){
            Serial.println("Document updated successfully");
          }else{
            Serial.print("Faild to update document  ");
            Serial.println(patchCode);
          }
                
            }
            http1.end();
        }
        
        else{
          Serial.printf("HTTP GET failed, error: %d\n", httpCode1);
        }


        }
 else {
          Serial.println("Failed to parse JSON");
        }
      } else {
        Serial.printf("HTTP GET failed, error: %d\n", httpCode);
      }
      http.end();
}
void update_current_amount(float weight){
    char* documentPath = "Dogs/Billy/Statistics/currentDay";
    String firestoreUrl = "https://firestore.googleapis.com/v1/projects/" + String(projectId) + "/databases/(default)/documents/" + String(documentPath) + "?key=" + String(apiKey);
    HTTPClient http;
    http.begin(firestoreUrl);

    int httpCode = http.GET();
      if (httpCode == HTTP_CODE_OK) {
        String payload = http.getString();
        // Process payload
        Serial.println("HTTP GET successful, processing data...");
        DynamicJsonDocument doc(1024);
        DeserializationError error = deserializeJson(doc, payload);
        if (!error) {

          DynamicJsonDocument updateDoc(1024);
          JsonObject fields = updateDoc.createNestedObject("fields");
          
          fields["currentAmount"]["integerValue"] = (int)weight;
          fields["startAmount"]["integerValue"] = doc["fields"]["startAmount"]["integerValue"];
          fields["givenUntilNow"]["integerValue"] = doc["fields"]["givenUntilNow"]["integerValue"];

          String jsonPayload;
          serializeJson(updateDoc,jsonPayload);

          http.addHeader("Content-Type", "application/json");
          int patchCode = http.PATCH(jsonPayload);

          if(patchCode == HTTP_CODE_OK){
            Serial.println("Document updated successfully");
          }else{
            Serial.print("Faild to update document  ");
            Serial.println(patchCode);
          }
        }
 else {
          Serial.println("Failed to parse JSON");
        }
      } else {
        Serial.printf("HTTP GET failed, error: %d\n", httpCode);
      }
      http.end();
}



//(timeinfo.tm_hour == hour && timeinfo.tm_min == minute && !function_called_today) 
void motorControlTask(void *parameter) {
  
    while (true) {
        if (ntp_initialized && getLocalTime(&timeinfo)) {
            // Check if it's time to start the motor for any meal
            for (int i = 0; i < mealCount; i++) { // Loop through all stored meals
                if (timeinfo.tm_hour == mealHours[i] && timeinfo.tm_min == mealMinutes[i] && !function_called_for_meal[i]) {
                    motor_speed = 500; // Set the motor speed based on mealAmounts[i] if needed
                    is_motor_running = true;
                    activeMealIndex = i;  // Set the active meal index
                    Serial.println("Spinning Clockwise for meal " + String(i+1) + "...");
                    function_called_for_meal[i] = true; // Set the flag for this specific meal
                }
            }
            resetMealFlagsAtMidnight(); // Reset the flags at midnight
        }

        // Check if the motor should stop
        if (is_motor_running && activeMealIndex != -1) {
            if (currentMillis - previousMillis >= 1000) {
                previousMillis = currentMillis;
                float weight = getWeight();
                // Check if the current weight has reached the desired amount for the active meal
                if (weight >= mealAmounts[activeMealIndex]) {
                    stopMotor();
                    weightReachedMessage();
                    is_motor_running = false;  // Stop the motor
                    activeMealIndex = -1;  // Reset the active meal index
                }
            }
        }

        vTaskDelay(100 / portTICK_PERIOD_MS); // Non-blocking delay
    }
}
bool checkk = false;

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
          mealCount = 0; // Reset mealCount
          for (JsonVariant v : documents) {
            String time_of_meal_string = v["fields"]["hourOfMeal"]["stringValue"].as<String>();
            String amount_of_food_string = v["fields"]["amountOfFood"]["stringValue"].as<String>();
            //todo check if meal time equals current time? if true : CurrMealName = v["name"] && checkk = true (note that name is not the name *slicing needed*);

            mealHours[mealCount] = time_of_meal_string.substring(0, 2).toInt();  // Extract and store hour
            mealMinutes[mealCount] = time_of_meal_string.substring(3, 5).toInt();  // Extract and store minute
            mealAmounts[mealCount] = amount_of_food_string.toInt();  // Store amount of food

            mealCount++; // Increment mealCount
          }
        }
 else {
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





