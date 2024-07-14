#include <WiFi.h>
#include <ESPAsyncWebServer.h>
#include <time.h>

const char* ssid_ap = "ESP32-Access-Point";
const char* password_ap = "123456789";

AsyncWebServer server(80);

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

void setup() {
  Serial.begin(115200);

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

void loop() {
  // Check if connected to Wi-Fi
  if (WiFi.status() == WL_CONNECTED && !ntp_initialized) {
    Serial.println("Connected to the WiFi network");

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
    } else {
      Serial.println("Failed to obtain time");
    }
  }

  delay(1000); // Print time every 1 second
}
