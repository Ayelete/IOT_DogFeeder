#include <WiFi.h>
#include <WebServer.h>
#include <Preferences.h>
// #include <esp_int_wdt.h>
// #include <esp_task_wdt.h>

const char* ap_ssid = "ESP32-AP";
const char* ap_password = "12345678";
const char* ssid_key = "wifi_ssid";
const char* password_key = "wifi_password";

WebServer server(80);
Preferences preferences;

const char* html = R"(
<!DOCTYPE html>
<html>
<body>
  <h2>Connect to WiFi</h2>
  <form action="/submit" method="post">
    SSID:<br>
    <input type="text" name="ssid"><br>
    Password:<br>
    <input type="password" name="password"><br><br>
    <input type="submit" value="Submit">
  </form>
</body>
</html>
)";

volatile bool shouldClearCredentials = false;

void IRAM_ATTR handleButtonPress() {
  shouldClearCredentials = true;
}

void handleRoot() {
  server.send(200, "text/html", html);
}

void handleSubmit() {
  String ssid = server.arg("ssid");
  String password = server.arg("password");

  String response = "Connecting to: " + ssid + "<br>";
  response += "With password: " + password + "<br>";

  // Save Wi-Fi credentials to Preferences
  preferences.begin("wifi", false); // Open preferences with 'wifi' namespace
  preferences.putString(ssid_key, ssid);
  preferences.putString(password_key, password);
  preferences.end();

  server.send(200, "text/html", response);

  // Connect to the new Wi-Fi network
  WiFi.begin(ssid.c_str(), password.c_str());
  
  int attempts = 0;
  while (WiFi.status() != WL_CONNECTED && attempts < 20) {
    delay(500);
    attempts++;
  }

  if (WiFi.status() == WL_CONNECTED) {
    response += "Connected successfully!";
  } else {
    response += "Failed to connect.";
  }

  server.send(200, "text/html", response);
}

void setup() {
  Serial.begin(115200);

  // Initialize Preferences
  preferences.begin("wifi", false); // Open preferences with 'wifi' namespace

  // Initialize button pin (GPIO 0)
  pinMode(0, INPUT_PULLUP); // GPIO 0 (boot button) with internal pull-up
  attachInterrupt(digitalPinToInterrupt(0), handleButtonPress, FALLING);

  // Read Wi-Fi credentials from preferences
  String saved_ssid = preferences.getString(ssid_key, "");
  String saved_password = preferences.getString(password_key, "");

  if (saved_ssid.length() > 0 && saved_password.length() > 0) {
    Serial.println("Using saved Wi-Fi credentials:");
    Serial.println("SSID: " + saved_ssid);
    Serial.println("Password: " + saved_password);
    
    // Connect to the saved Wi-Fi network
    WiFi.begin(saved_ssid.c_str(), saved_password.c_str());

    int attempts = 0;
    while (WiFi.status() != WL_CONNECTED && attempts < 20) {
      delay(500);
      attempts++;
    }

    if (WiFi.status() == WL_CONNECTED) {
      Serial.println("Connected to the WiFi network");
    } else {
      Serial.println("Failed to connect to saved Wi-Fi credentials, starting access point.");
      // Start the access point if Wi-Fi connection fails
      WiFi.softAP(ap_ssid, ap_password);
    }
  } else {
    Serial.println("No saved Wi-Fi credentials found, starting access point.");
    // Start the access point
    WiFi.softAP(ap_ssid, ap_password);
  }

  // Start the web server
  server.on("/", handleRoot);
  server.on("/submit", handleSubmit);
  server.begin();

  Serial.println("ESP32 Access Point Started");
  Serial.print("IP Address: ");
  Serial.println(WiFi.softAPIP());
}

void loop() {
  server.handleClient();

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
