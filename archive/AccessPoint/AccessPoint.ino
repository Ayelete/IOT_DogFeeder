#include <WiFi.h>
#include <ESPAsyncWebServer.h>
#include <time.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SH110X.h>

const char* ssid_ap = "ESP32-Access-Point";
const char* password_ap = "123456789";

AsyncWebServer server(80);
#define i2c_Address 0x3c

// OLED display width and height, adjust as needed
#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64
#define OLED_RESET -1
Adafruit_SH1106G display = Adafruit_SH1106G(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

// HTML form to get Wi-Fi credentials
const char* html_form = R"rawliteral(
<!DOCTYPE HTML><html>
<head>
  <title>ESP32 Wi-Fi Config</title>
</head>
<body>
  <h2>Enter Wi-Fi Credentials</h2>
  <form action="/get">
    SSID: <input type="text" name="ssid"><br>
    Password: <input type="password" name="password"><br>
    <input type="submit" value="Submit">
  </form>
</body>
</html>)rawliteral";

// Variables to store Wi-Fi credentials
String wifi_ssid;
String wifi_password;

// Flag to indicate if NTP time is initialized
bool ntp_initialized = false;

// Flag to indicate if the function has been called for the current day
bool function_called_today = false;

void setup() {
  Serial.begin(115200);

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
      
      // Disconnect from any current Wi-Fi connection
      WiFi.disconnect();
      
      // Connect to the new Wi-Fi network
      WiFi.begin(wifi_ssid.c_str(), wifi_password.c_str());
    } else {
      request->send(200, "text/html", "Error: Missing SSID or Password");
    }
  });

  // Start server
  server.begin();
}

void mySpecificFunction() {
  Serial.println("The specific function has been called!");
  // Add your specific function logic here
}

void loop() {
  // Check if connected to Wi-Fi
  if (WiFi.status() == WL_CONNECTED && !ntp_initialized) {
    Serial.println("Connected to the WiFi network");

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
  }

  // If NTP is initialized, get and print current time
  if (ntp_initialized) {
    time_t now;
    struct tm timeinfo;
    if (getLocalTime(&timeinfo)) {
      Serial.println(&timeinfo, "%A, %B %d %Y %H:%M:%S");

      // Check if the current time is 13:00 and if the function has not been called today
      if (timeinfo.tm_hour == 13 && timeinfo.tm_min == 0 && !function_called_today) {
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

  delay(1000); // Print time every 1 second
}
