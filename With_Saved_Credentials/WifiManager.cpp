#include "WifiManager.h"
#include <Arduino.h>
#include <ESPAsyncWebServer.h>
#include <Preferences.h>
#include <WiFi.h>
// #include <WebServer.h>


//some constants for AP
const char* ssid_ap = "ESP32-Access-Point";
const char* password_ap = "123456789";
const char* ssid_key = "wifi_ssid";
const char* password_key = "wifi_password";


// Variables to store Wi-Fi credentials
// String ssid_key;
// String password_key;



AsyncWebServer server(80);
Preferences preferences;



// HTML form to get Wi-Fi credentials
const char* html_form = R"rawliteral(
<!DOCTYPE HTML><html>
<head>
  <title>ESP32 Wi-Fi Config</title>
  <style>
    body {
      font-family: Verdana, sans-serif;
      background-color: #f0e4fc;
      margin: 0;
      padding: 0;
      display: flex;
      justify-content: center;
      align-items: center;
      height: 100vh;
    }
    .container {
      background-color: #ffffff;
      padding: 20px;
      border-radius: 8px;
      box-shadow: 0 0 5px rgba(0, 0, 0, 0.2);
      max-width: 320px;
      width: 100%;
    }
    h2 {
      color: #333333;
      text-align: center;
      margin-bottom: 20px;
    }
    label {
      display: block;
      margin-bottom: 10px;
      color: #555555;
    }
    input[type="text"], input[type="password"] {
      width: 100%;
      padding: 8px;
      margin-bottom: 20px;
      border: 1px solid #cccccc;
      border-radius: 4px;
      box-sizing: border-box;
    }
    input[type="submit"] {
      width: 100%;
      padding: 10px;
      background-color: #983eef;
      color: white;
      border: none;
      border-radius: 4px;
      cursor: pointer;
    }
    input[type="submit"]:hover {
      background-color: #7410d3;
    }
  </style>
</head>
<body>
  <div class="container">
    <h2>Enter Wi-Fi Credentials</h2>
    <form action="/get">
      <label for="ssid">SSID:</label>
      <input type="text" id="ssid" name="ssid" required>
      <label for="password">Password:</label>
      <input type="password" id="password" name="password" required>
      <input type="submit" value="Submit">
    </form>
  </div>
</body>
</html>)rawliteral";




// delete saved credentials when 
volatile bool shouldClearCredentials = false;


void deleteCredentials(){
  if (shouldClearCredentials) {
    shouldClearCredentials = false;
    Serial.println("Clearing stored Wi-Fi credentials and restarting.");
    preferences.begin("wifi", false); // Open preferences with 'wifi' namespace
    preferences.clear(); // Clear all preferences in the 'wifi' namespace
    preferences.end();
    delay(1000); // Delay to ensure the message is sent over Serial
    ESP.restart(); // Restart the ESP32 to apply changes
  }
}

void initWifi(){
  // fetch credentials from file system
  preferences.begin("wifi", false);
  String saved_ssid = preferences.getString(ssid_key, "");
  String saved_password = preferences.getString(password_key, "");

  // check if credentials exist
  if (saved_ssid.length() > 0 && saved_password.length() > 0) {
    Serial.println("Using saved Wi-Fi credentials:");
    Serial.println("SSID: " + saved_ssid);
    Serial.println("Password: " + saved_password);
    WiFi.begin(saved_ssid.c_str(), saved_password.c_str());    

    int attempts = 0;
    while (WiFi.status() != WL_CONNECTED && attempts < 20) {
      delay(500);
      attempts++;
    }

    if (WiFi.status() == WL_CONNECTED) {
      Serial.println("Connected to the WiFi network");
      successfulConnectionMessage();
    } else {
      Serial.println("Failed to connect to saved Wi-Fi credentials, starting access point.");
      // Start the access point if Wi-Fi connection fails
      WiFi.softAP(ssid_ap, password_ap);
      Serial.println("Access Point started");
    }
  }
  else {
    Serial.println("No saved Wi-Fi credentials found, starting access point.");
    // Start the access point
    WiFi.softAP(ssid_ap, password_ap);
    Serial.println("Access Point started");
  }

  // Route for root / web page
  server.on("/", HTTP_GET, [](AsyncWebServerRequest *request){
    request->send(200, "text/html", html_form);
  });

  // Route to get the Wi-Fi credentials
  server.on("/get", HTTP_GET, [](AsyncWebServerRequest *request){
    if (request->hasParam("ssid") && request->hasParam("password")) {
      String wifi_ssid = request->getParam("ssid")->value();
      String wifi_password = request->getParam("password")->value();
      
      request->send(200, "text/html", "Credentials received, trying to connect...");

      // Disconnect from any current Wi-Fi connection
      WiFi.disconnect();
      preferences.begin("wifi", false); // Open preferences with 'wifi' namespace
      preferences.putString(ssid_key, wifi_ssid);
      preferences.putString(password_key, wifi_password);
      preferences.end();
      // Connect to the new Wi-Fi network
      WiFi.begin(wifi_ssid.c_str(), wifi_password.c_str());
      int attempts = 0;
      while (WiFi.status() != WL_CONNECTED && attempts < 20) {
      delay(500);
        attempts++;
      }

      if (WiFi.status() == WL_CONNECTED) {
        successfulConnectionMessage();
      }
      else{ // TODO: find out why it's not called when wrong credentials are entered
        Serial.println("Failed to connect to WiFi");
        wrongCredentialsMessage();
      }

    } else {
      request->send(200, "text/html", "Error: Missing SSID or Password");
    }
  });

  // Start server
  server.begin();

}

