
#ifndef WIFIMANAGER_H
#define WIFIMANAGER_H

#include <WiFi.h>
#include <ESPAsyncWebServer.h>

// Constants for Access Point
extern const char* ssid_ap;
extern const char* password_ap;

// Variables to store Wi-Fi credentials
extern String wifi_ssid;
extern String wifi_password;

// HTML form to get Wi-Fi credentials
extern const char* html_form;

// AsyncWebServer instance
extern AsyncWebServer server;

void initWifi();

#endif