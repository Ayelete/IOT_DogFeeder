#ifndef FIREBASEMANAGER_H
#define FIREBASEMANAGER_H

#include <Firebase_ESP_Client.h>
#include <ArduinoJson.h>

// Provide the token generation process info.
#include <addons/TokenHelper.h>

// Firebase credentials and configuration
#define API_KEY "AIzaSyDQT20WMQz3lA-W4CJ4WLdKAYn_jy2Kj64"
#define FIREBASE_PROJECT_ID "hellome-a9965"
#define USER_EMAIL "hillahhassan@gmail.com"
#define USER_PASSWORD "hp7rocks"

extern FirebaseData fbdo;
extern FirebaseAuth auth;
extern FirebaseConfig config;

void initFirebase();

#endif // FIREBASEMANAGER_H
