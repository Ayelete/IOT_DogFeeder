
#include <Adafruit_GFX.h>
#include <Adafruit_SH110X.h>

// OLED display width and height, adjust as needed
#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64
#define OLED_RESET -1
#define i2c_Address 0x3c
#define motorInterfaceType 1

// Function declarations
void initOled();
void displayConnectingMessage();
void displayMotorRunning();
void displayWeightLimit();
void clearDisplay();

extern Adafruit_SH1106G display;

