#ifndef OLEDMANAGER_H
#define OLEDMANAGER_H



void initOLED();

void motorRunningMessage();

void successfulConnectionMessage();

void weightReachedMessage();

void wrongCredentialsMessage();

void foodWasGivenMessage();

void accessPointMessage();

void tryingToConnectMessage(const char* ssid);

#endif