#include <WiFi.h>
#include <WebServer.h>

const char* ssid = "ESP32-AP";
const char* password = "12345678";

WebServer server(80);

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

void handleRoot() {
  server.send(200, "text/html", html);
}

void handleSubmit() {
  String ssid = server.arg("ssid");
  String password = server.arg("password");

  String response = "Connecting to: " + ssid + "<br>";
  response += "With password: " + password + "<br>";

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

  // Start the access point
  WiFi.softAP(ssid, password);

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
}
