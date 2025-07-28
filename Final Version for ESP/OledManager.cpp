// #include <Adafruit_SH1106G.h>
#include <Adafruit_SH110X.h>
#include "WifiManager.h"


// OLED display width and height, adjust as needed
#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64
#define OLED_RESET -1

Adafruit_SH1106G display = Adafruit_SH1106G(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

void initOLED(){
    // Initialize the OLED display
  if(!display.begin(i2c_Address, true)) {
    Serial.println(F("SH1106 allocation failed"));
    for(;;);
  }
  display.clearDisplay();
  display.setTextColor(SH110X_WHITE);
  display.setTextSize(1); // Increase text size
  display.setCursor(0,20);
  display.print("Welcome");
  display.display();
}

void motorRunningMessage(){
    // print message to OLED
  display.clearDisplay();
  display.setTextColor(SH110X_WHITE);
  display.setTextSize(1);
  display.setCursor(0,20);
  display.print("motor is running");
  display.display(); 
}

void successfulConnectionMessage(){
    // Display message on OLED
    display.clearDisplay();
    display.setTextColor(SH110X_WHITE);
    display.setTextSize(2); // Increase text size
    display.setCursor(0,0);
    display.print("Successful");
    display.setCursor(0, 20); // Move to next line
    display.print("Connection");
    display.setCursor(0, 40); // Move to next line
    display.print(" :)");
    display.display();
}


void wrongCredentialsMessage(){
      // Display message on OLED
    display.clearDisplay();
    display.setTextColor(SH110X_WHITE);
    display.setTextSize(1); // Increase text size
    display.setCursor(0,0);
    display.print("Connection failed :(");
    display.setCursor(0, 20); // Move to next line
    // display.print("Failed");
    // display.setCursor(0, 40); // Move to next line
    // display.print(" :(");
    display.setCursor(0, 20);
    display.print("check credentials and try again");
    display.display();
}


void weightReachedMessage(){
    display.clearDisplay();
    display.setTextColor(SH110X_WHITE);
    display.setTextSize(2);
    display.setCursor(0,0);
    display.print("Bon");
    display.setCursor(0,20);
    display.print("Appetit");
    display.display();  
}

void foodWasGivenMessage(){
    display.clearDisplay();
    display.setTextColor(SH110X_WHITE);
    display.setTextSize(2);
    display.setCursor(0,0);
    display.print("Meal Is");
    display.setCursor(0,20);
    display.print("Served,");
    display.setCursor(0,40);
    display.print("Enjoy!");
    display.display();  
}


void accessPointMessage(){
  display.clearDisplay();
  display.setTextColor(SH110X_WHITE);
  display.setTextSize(1);
  display.setCursor(0,0);
  display.print("Access Point Started");
  display.setCursor(0,15);
  display.print("password: 123456789");
  display.setCursor(0,30);
  display.print("IP: 192.168.4.1");
  display.setCursor(0, 45);
  display.print("Enter IP in browser's url");
  display.display();  
}

void tryingToConnectMessage(const char* ssid){
  display.clearDisplay();
  display.setTextColor(SH110X_WHITE);
  display.setTextSize(1);
  display.setCursor(0,0);
  display.print("Connecting to");
  display.setCursor(0,20);
  display.print("WiFi:");
  display.print(ssid);
  display.display();  
}
