#ifndef NTPMANAGER_H
#define NTPMANAGER_H

#include "WifiManager.h"
#include "OledManager.h"
#include <time.h>
#include <WiFi.h>



void initTimeIfConnectedToWifi();
void printTimeToSerialMonitor();
void resetMealFlagsAtMidnight();
#endif