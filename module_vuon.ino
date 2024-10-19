#include <WiFi.h>
#include <WiFiClient.h>
#include <WebServer.h>
#include <ESPmDNS.h>

const char *ssid = "ESP32";
const char *password = "0907923187a";

WebServer server(80);

// Chân analog đọc từ cảm biến độ ẩm đất
const int soilMoisturePin = 39;  // Sử dụng chân ADC

void handleRoot() {
  char msg[2000];
  snprintf(msg, 2000,
           "<html>\
  <head>\
    <meta http-equiv='refresh' content='4'/>\
    <meta name='viewport' content='width=device-width, initial-scale=1'>\
    <link rel='stylesheet' href='https://use.fontawesome.com/releases/v5.7.2/css/all.css'\
     integrity='sha384-fnmOCqbTlWIlj8LyTjo7mOUStjsKC4pOpQbqyi7RrhN7udi9RwhKkMHpvLbHG9Sr' crossorigin='anonymous'>\
    <title>Garden</title>\
    <style>\
    html { font-family: Arial; display: inline-block; margin: 0px auto; text-align: center;}\ 
    h2 { font-size: 3.0rem; }\
    p { font-size: 3.0rem; }\
    .units { font-size: 1.2rem; }\
    .moisture-labels{ font-size: 1.5rem; vertical-align:middle; padding-bottom: 15px;}\ 
    button {font-size: 1.5rem; padding: 10px; margin: 10px;}\ 
    </style>\
    <script>\
      function controlRelay(state) {\
        var xhr = new XMLHttpRequest();\
        xhr.open('GET', '/' + state, true);\
        xhr.send();\
      }\
    </script>\
  </head>\
  <body>\
      <h2>Garden!</h2>\
      <p>\
        <i class='fas fa-seedling' style='color:#32a852;'></i>\
        <span class='moisture-labels'>Soil Moisture</span>\
        <span>%.2f</span>\
        <sup class='units'>%%</sup>\
      </p>\
      <p><button onclick=\"controlRelay('relay/on')\">ON</button> Turn on the pump</p>\
      <p><button onclick=\"controlRelay('relay/off')\">OFF</button> Turn off the pump</p>\
  </body>\
</html>",
           readSoilMoisture()
          );
  server.send(200, "text/html", msg);
}

const int relayPin = 25;

void setup() {
  Serial.begin(9600);
  pinMode(relayPin, OUTPUT);
  digitalWrite(relayPin, LOW);

  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);
  Serial.println("");

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("");
  Serial.print("Connected to ");
  Serial.println(ssid);
  Serial.print("IP address: ");
  Serial.println(WiFi.localIP());

  if (MDNS.begin("esp32")) {
    Serial.println("MDNS responder started");
  }
  server.on("/", handleRoot);

  server.begin();
  Serial.println("HTTP server started");

  server.on("/relay/on", [](){
    digitalWrite(relayPin, HIGH);
    server.send(200, "text/plain", "Relay ON");
  });

  server.on("/relay/off", [](){
    digitalWrite(relayPin, LOW);
    server.send(200, "text/plain", "Relay OFF");
  });
}

void loop() {
  server.handleClient();
  delay(2);  // Đọc cảm biến mỗi 2 giây
}

float readSoilMoisture() {
  // Đọc giá trị từ cảm biến độ ẩm đất (0-4095)
  int sensorValue = analogRead(soilMoisturePin);
  
  // Chuyển đổi giá trị sang phần trăm độ ẩm (giả định khoảng 0-100%)
  float moisturePercent = map(sensorValue, 0, 4095, 0, 100);

  Serial.print("Soil moisture: ");
  Serial.print(moisturePercent);
  Serial.println(" %");

  return moisturePercent;
}
