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
  display.print("connecting...");
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

void weightReachedMessage(){
    display.clearDisplay();
    display.setTextColor(SH110X_WHITE);
    display.setTextSize(1);
    display.setCursor(0,0);
    display.print("Weight limit");
    display.setCursor(0,20);
    display.print("reached,");
    display.setCursor(0,40);
    display.print("motor stopped");
    display.display();  
}
