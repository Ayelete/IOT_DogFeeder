#include <WiFi.h>
#include <ESPAsyncWebServer.h>
#include <time.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SH110X.h>
#include <AccelStepper.h>
#include <HX711.h>

//some constants for motor
const int DIR_PIN = 12;
const int STEP_PIN = 14;
const int steps_per_rev = 200;

// HX711 circuit wiring
const int LOADCELL_DOUT_PIN = 16;
const int LOADCELL_SCK_PIN = 4;
HX711 scale;

//some constants for AP
const char* ssid_ap = "ESP32-Access-Point";
const char* password_ap = "123456789";

AsyncWebServer server(80);
#define i2c_Address 0x3c

// OLED display width and height, adjust as needed
#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64
#define OLED_RESET -1
#define motorInterfaceType 1
Adafruit_SH1106G display = Adafruit_SH1106G(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

// HTML form to get Wi-Fi credentials

const char* html_form = R"rawliteral(
<!DOCTYPE HTML>
<html>
<head>
  <title>ESP32 Wi-Fi Config</title>
  <style>
    body {
      font-family: Arial, sans-serif;
      background-color: #f4f4f4;
      display: flex;
      justify-content: center;
      align-items: center;
      height: 100vh;
      margin: 0;
    }
    .container {
      background-color: #fff;
      padding: 20px;
      border-radius: 10px;
      box-shadow: 0 0 10px rgba(0, 0, 0, 0.1);
      width: 300px;
      text-align: center;
    }
    h2 {
      margin-bottom: 20px;
      color: #333;
    }
    input[type="text"],
    input[type="password"] {
      width: calc(100% - 20px);
      padding: 10px;
      margin-bottom: 20px;
      border: 1px solid #ccc;
      border-radius: 5px;
    }
    input[type="submit"] {
      background-color: #4CAF50;
      color: white;
      padding: 10px 20px;
      border: none;
      border-radius: 5px;
      cursor: pointer;
    }
    input[type="submit"]:hover {
      background-color: #45a049;
    }
  </style>
</head>
<body>
  <div class="container">
    <h2>Enter Wi-Fi Credentials</h2>
    <form action="/get">
      <input type="text" name="ssid" placeholder="SSID"><br>
      <input type="password" name="password" placeholder="Password"><br>
      <input type="submit" value="Submit">
    </form>
  </div>
</body>
</html>)rawliteral";


String wifi_ssid;
String wifi_password;
bool connectAttempt = false;
unsigned long connectStartTime = 0;
const unsigned long connectTimeout = 10000; // 10 seconds timeout


// Flag to indicate if NTP time is initialized
bool ntp_initialized = false;

// Flag to indicate if the function has been called for the current day
bool function_called_today = false;

// Flag to stop the motor if weight exceeds limit
volatile bool stop_motor = false;

unsigned long previousMillis = 0; // store last time HX711 was read
unsigned long previousMillis2 = 0; // store last time HX711 was read
const long interval = 1000; // interval to read HX711 (milliseconds)

unsigned long motorPreviousMillis = 0; // store last time motor was moved
const long motorInterval = 1000; // interval to control motor speed (microseconds)

const float calibration_factor = 406.91;

int motor_speed = 0;
bool motor_is_on = false;

//initialize motor
AccelStepper stepper(motorInterfaceType, STEP_PIN, DIR_PIN);

void setup() {
  Serial.begin(115200);
  delay(5000);

  
  // Initialize the stepper motor
  stepper.setMaxSpeed(2000);   // Set maximum speed
  stepper.setAcceleration(1000); // Set acceleration
  

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

  // Initialize the HX711 scale

  scale.begin(LOADCELL_DOUT_PIN, LOADCELL_SCK_PIN);
  scale.set_scale(calibration_factor); // Set the calibration factor
  scale.tare();  // Zero the scale


  // Setting up the ESP32 as an Access Point
  WiFi.softAP(ssid_ap, password_ap);
  Serial.println("Access Point started");

  // Route for root / web page
  server.on("/", HTTP_GET, [](AsyncWebServerRequest *request){
    request->send(200, "text/html", html_form);
  });

  // Route to get the Wi-Fi credentials
  server.on("/get", HTTP_GET, [](AsyncWebServerRequest *request){
    if (request->hasParam("ssid") && request->hasParam("password")) {
      wifi_ssid = request->getParam("ssid")->value();
      wifi_password = request->getParam("password")->value();

      request->send(200, "text/html", "Credentials received, trying to connect...");

      // Initiate connection attempt
      WiFi.disconnect();
      WiFi.begin(wifi_ssid.c_str(), wifi_password.c_str());
      connectAttempt = true;
      connectStartTime = millis();
    } else {
      request->send(200, "text/html", "Error: Missing SSID or Password");
    }
  });

  // Start server
  server.begin();
}

void mySpecificFunction() {
  // Add your specific function logic here
  motor_speed = -500; // put minus to change direction
  Serial.println("Spinning Clockwise...");
  motor_is_on = true;

  // print message to OLED
  display.clearDisplay();
  display.setTextColor(SH110X_WHITE);
  display.setTextSize(1);
  display.setCursor(0,20);
  display.print("motor is running");
  display.display(); 

}


void loop() {

  unsigned long currentMillis = millis();

  // Handle Wi-Fi connection attempts
  if (connectAttempt) {
    if (WiFi.status() == WL_CONNECTED) {
      connectAttempt = false;
      Serial.println("Connected to Wi-Fi successfully!");
      // Inform the user of success
      server.on("/status", HTTP_GET, [](AsyncWebServerRequest *request){
        request->send(200, "text/html", "Connected to Wi-Fi successfully!");
      });
    } else if (millis() - connectStartTime >= connectTimeout) {
      connectAttempt = false;
      Serial.println("Failed to connect to Wi-Fi. Please check your credentials and try again.");
      // Inform the user of failure
      server.on("/status", HTTP_GET, [](AsyncWebServerRequest *request){
        request->send(200, "text/html", "Failed to connect to Wi-Fi. Please check your credentials and try again.");
      });
    }
  }

  // Check if connected to Wi-Fi
  if (WiFi.status() == WL_CONNECTED && !ntp_initialized) {

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

    // Initialize NTP
    configTime(10800, 0, "pool.ntp.org", "time.nist.gov");
    
    // Set timezone for Israel (UTC+3)
    setenv("TZ", "IST-2IDT,M3.4.4/26,M10.5.0", 1);
    tzset();

    ntp_initialized = true;
    delay(15000);
  }

  stepper.setSpeed(motor_speed); // Set speed (positive for clockwise, negative for counterclockwise)
  stepper.runSpeed();

  // If NTP is initialized, get and print current time
  if (ntp_initialized) {
    time_t now;
    struct tm timeinfo;
    if (getLocalTime(&timeinfo)) {
      if(currentMillis - previousMillis2 >= interval){
        previousMillis2 = currentMillis;
        Serial.println(&timeinfo, "%A, %B %d %Y %H:%M:%S");
      }

      // Check if the current time is 13:00 and if the function has not been called today
      if (timeinfo.tm_hour == 13 && timeinfo.tm_min == 05 && !function_called_today) {
        mySpecificFunction();
        function_called_today = true;
      }

      // Reset the flag at midnight
      if (timeinfo.tm_hour == 0 && timeinfo.tm_min == 0) {
        function_called_today = false;
      }
    } else {
      Serial.println("Failed to obtain time");
    }
  }

  // Read the weight from HX711 every second
  if (currentMillis - previousMillis >= interval) {
    previousMillis = currentMillis;

    if (scale.is_ready()) {
      float weight = scale.get_units();
      Serial.print("Weight: ");
      Serial.print(weight);
      Serial.println(" grams");

      if (weight > 200 && motor_is_on) {
        motor_speed = 0;
        motor_is_on = false;
        Serial.println("Motor stopped due to weight limit.");
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
    } else {
      Serial.println("HX711 not found.");
    }
  }
}
