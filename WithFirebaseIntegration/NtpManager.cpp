#include <time.h>
#include <WiFi.h>
#include "NtpManager.h"
#include "OledManager.h"

const char* ntpServer1 = "pool.ntp.org";
const char* ntpServer2 = "ntp.Technion.ac.il";

bool ntp_initialized = false;

void initNtp() 
{
  if (WiFi.status() == WL_CONNECTED && !ntp_initialized) {
    Serial.println("Connected to the WiFi network");
    displayConnectingMessage();

    // Initialize NTP
    configTime(10800, 0, ntpServer1, ntpServer2);
    // Set timezone for Israel (UTC+3)
    setenv("TZ", "IST-2IDT,M3.4.4/26,M10.5.0", 1);
    tzset();

    ntp_initialized = true;
    delay(15000);
  }
}

void updateTime() 
{
  if (!ntp_initialized) {
    initNtp();
  }
}

void printCurrentTime() 
{
  if (ntp_initialized) {
    time_t now;
    struct tm timeinfo;
    if (getLocalTime(&timeinfo)) {
      Serial.println(&timeinfo, "%A, %B %d %Y %H:%M:%S");
    } else {
      Serial.println("Failed to obtain time");
    }
  }
}
