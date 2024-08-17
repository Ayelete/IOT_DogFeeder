#include "ntpManager.h"


extern unsigned long currentMillis;
extern unsigned long previousMillis2;
extern bool function_called_today;
extern int mealCount;
extern bool function_called_for_meal[5];

time_t now;
struct tm timeinfo;

//some constants for ntp request
const char* ntpServer1 = "pool.ntp.org";
const char* ntpServer2 = "ntp.Technion.ac.il";
//const char* ntpServer2 = "time.nist.gov";

// Flag to indicate if NTP time is initialized
bool ntp_initialized = false;

void initTimeIfConnectedToWifi(){
      // Check if connected to Wi-Fi
  if (WiFi.status() == WL_CONNECTED && !ntp_initialized) {
    Serial.println("Connected to the WiFi network");

    // successfulConnectionMessage();

    // Initialize NTP
    configTime(10800, 0, ntpServer1, ntpServer2);
    
    // Set timezone for Israel (UTC+3)
    setenv("TZ", "IST-2IDT,M3.4.4/26,M10.5.0", 1);
    tzset();

    ntp_initialized = true;
    delay(15000);
  }
}

void printTimeToSerialMonitor(){
    // time_t now;
    // struct tm timeinfo;
    
      if(currentMillis - previousMillis2 >= 1000){
        previousMillis2 = currentMillis;
        Serial.println(&timeinfo, "%A, %B %d %Y %H:%M:%S");
      }

}

void resetMealFlagsAtMidnight() {
    // Reset all the flags at midnight to allow the function to trigger the next day
    if (timeinfo.tm_hour == 0 && timeinfo.tm_min == 0) {
        for (int i = 0; i < mealCount; i++) {
            function_called_for_meal[i] = false;
        }
    }
}