#include "OledManager.h"

// OLED display object
Adafruit_SH1106G display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

void initOled() 
{
    if (!display.begin(i2c_Address, true)) {
        Serial.println(F("SH1106 allocation failed"));
        for (;;);
    }
    display.clearDisplay();
    display.setTextColor(SH110X_WHITE);
    display.setTextSize(1); // Increase text size
    display.setCursor(0, 20);
    display.print("connecting...");
    display.display();
}

void displayConnectingMessage() 
{
    display.clearDisplay();
    display.setTextColor(SH110X_WHITE);
    display.setTextSize(2); // Increase text size
    display.setCursor(0, 0);
    display.print("Successful");
    display.setCursor(0, 20); // Move to next line
    display.print("Connection");
    display.setCursor(0, 40); // Move to next line
    display.print(" :)");
    display.display();
}

void displayMotorRunning() 
{
    display.clearDisplay();
    display.setTextColor(SH110X_WHITE);
    display.setTextSize(1);
    display.setCursor(0, 20);
    display.print("motor is running");
    display.display();
}

void displayWeightLimit() 
{
    display.clearDisplay();
    display.setTextColor(SH110X_WHITE);
    display.setTextSize(1);
    display.setCursor(0, 0);
    display.print("Weight limit");
    display.setCursor(0, 20);
    display.print("reached,");
    display.setCursor(0, 40);
    display.print("motor stopped");
    display.display();
}

void clearDisplay() 
{
    display.clearDisplay();
}
