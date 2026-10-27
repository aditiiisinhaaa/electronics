#include <WiFi.h>
#include <ESPAsyncWebServer.h>
#include <ESP32Servo.h>

// ==================== CHANGE THESE ====================
const char* ssid     = "Students 1";
const char* password = "FutureisTech";
// =====================================================

// Servo objects
Servo frontFin;    // 10kg-cm
Servo rightFin;    // 20kg-cm
Servo backFin;     // 10kg-cm
Servo leftFin;     // 20kg-cm
Servo rightWing;   // 10kg-cm
Servo leftWing;    // 10kg-cm

// Pin assignments
const int pinFront  = 23;
const int pinRight  = 19;
const int pinBack   = 5;
const int pinLeft   = 18;
const int pinRWing  = 4;
const int pinLWing  = 2;

AsyncWebServer server(80);
AsyncWebSocket ws("/ws");

// === Safety timeout (returns to neutral if control lost) ===
unsigned long lastMessageTime = 0;
const unsigned long CONTROL_TIMEOUT = 2000;  // 2 seconds

void returnToNeutral() {
  Serial.println("CONTROL LOST → Returning all actuators to neutral (90°)");
  frontFin.write(90);
  rightFin.write(90);
  backFin.write(90);
  leftFin.write(90);
  rightWing.write(90);
  leftWing.write(90);
}

void onWsEvent(AsyncWebSocket *server, AsyncWebSocketClient *client, AwsEventType type, void *arg, uint8_t *data, size_t len) {
  if (type == WS_EVT_CONNECT) {
    Serial.printf("Client #%u connected\n", client->id());
    lastMessageTime = millis();
  } 
  else if (type == WS_EVT_DISCONNECT) {
    Serial.printf("Client #%u disconnected\n", client->id());
  } 
  else if (type == WS_EVT_DATA) {
    lastMessageTime = millis();  // Keep-alive

    AwsFrameInfo *info = (AwsFrameInfo*)arg;
    if (info->final && info->index == 0 && info->len == len) {
      data[len] = 0;
      String message = (char*)data;
      message.trim();
      message.toLowerCase();

      int colon = message.indexOf(':');
      if (colon == -1) return;

      String servo = message.substring(0, colon);
      servo.trim();
      int angle = message.substring(colon + 1).toInt();
      angle = constrain(angle, 0, 180);

      if (servo == "front" || servo == "frontfin")           frontFin.write(angle);
      else if (servo == "right" || servo == "rightfin")     rightFin.write(angle);
      else if (servo == "back" || servo == "backfin")       backFin.write(angle);
      else if (servo == "left" || servo == "leftfin")       leftFin.write(angle);
      else if (servo == "rwing" || servo == "rightwing")    rightWing.write(angle);
      else if (servo == "lwing" || servo == "leftwing")     leftWing.write(angle);

      Serial.printf("%s → %d°\n", servo.c_str(), angle);
    }
  }
}

void setup() {
  Serial.begin(115200);

  // Critical for stable multi-servo + WiFi operation
  ESP32PWM::allocateTimer(0);
  ESP32PWM::allocateTimer(1);
  ESP32PWM::allocateTimer(2);
  ESP32PWM::allocateTimer(3);

  // Attach servos with standard 500-2500µs pulse width
  frontFin.attach(pinFront,  500, 2500);
  rightFin.attach(pinRight,  500, 2500);
  backFin.attach(pinBack,    500, 2500);
  leftFin.attach(pinLeft,    500, 2500);
  rightWing.attach(pinRWing, 500, 2500);
  leftWing.attach(pinLWing,  500, 2500);

  // Start neutral
  returnToNeutral();
  delay(500);

  // Connect to WiFi
  WiFi.begin(ssid, password);
  Serial.print("Connecting to WiFi");
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("\nWiFi connected!");
  Serial.print("IP address: http://");
  Serial.println(WiFi.localIP());

  // === Web Interface ===
  server.on("/", HTTP_GET, [](AsyncWebServerRequest *request) {
    request->send(200, "text/html", R"rawliteral(
<!DOCTYPE html>
<html>
<head>
  <meta name="viewport" content="width=device-width, initial-scale=1">
  <title>Small Diameter Bomb Control</title>
  <style>
    body {font-family: Arial; background:#000; color:#0f9; text-align:center; padding:20px;}
    h1 {color:#f00; text-shadow: 0 0 10px #f00; font-size:2.5em;}
    .servo {margin:30px auto; max-width:500px; padding:20px; background:#200; border:2px solid #f00; border-radius:15px; box-shadow:0 0 20px #f005;}
    .name {font-size:1.8em; margin-bottom:10px; color:#f33;}
    input[type=range] {width:100%; height:50px; -webkit-appearance:none; background:#400; border-radius:25px;}
    input[type=range]::-webkit-slider-thumb {-webkit-appearance:none; width:60px; height:60px; background:#f00; border-radius:50%; box-shadow:0 0 15px #f00;}
    .value {font-size:2.5em; margin-top:15px; color:#f90;}
  </style>
</head>
<body>
  <h1>Small Diameter Bomb<br>Fin & Wing Control</h1>

  <div class="servo"><div class="name">Front Fin</div><input type="range" min="0" max="180" value="90" id="front"><div class="value"><span id="vfront">90</span>°</div></div>
  <div class="servo"><div class="name">Right Fin</div><input type="range" min="0" max="180" value="90" id="right"><div class="value"><span id="vright">90</span>°</div></div>
  <div class="servo"><div class="name">Back Fin</div><input type="range" min="0" max="180" value="90" id="back"><div class="value"><span id="vback">90</span>°</div></div>
  <div class="servo"><div class="name">Left Fin</div><input type="range" min="0" max="180" value="90" id="left"><div class="value"><span id="vleft">90</span>°</div></div>
  <div class="servo"><div class="name">Right Wing</div><input type="range" min="0" max="180" value="90" id="rwing"><div class="value"><span id="vrwing">90</span>°</div></div>
  <div class="servo"><div class="name">Left Wing</div><input type="range" min="0" max="180" value="90" id="lwing"><div class="value"><span id="vlwing">90</span>°</div></div>

<script>
  var ws = new WebSocket("ws://" + location.hostname + "/ws");

  const sliders = {
    front: document.getElementById("front"),
    right: document.getElementById("right"),
    back:  document.getElementById("back"),
    left:  document.getElementById("left"),
    rwing: document.getElementById("rwing"),
    lwing: document.getElementById("lwing")
  };

  Object.keys(sliders).forEach(key => {
    sliders[key].addEventListener("input", function() {
      let val = this.value;
      document.getElementById("v" + key).innerText = val;
      ws.send(key + ":" + val);
    });
  });

  ws.onopen = () => console.log("Control link active");
  ws.onclose = () => alert("CONNECTION LOST");
</script>
</body>
</html>
)rawliteral");
  });

  ws.onEvent(onWsEvent);
  server.addHandler(&ws);
  server.begin();
  Serial.println("Web server started");
}

void loop() {
  ws.cleanupClients();

  // Safety: return to neutral if no control signal for 2 seconds
  if (millis() - lastMessageTime > CONTROL_TIMEOUT) {
    static bool safeState = false;
    if (!safeState) {
      returnToNeutral();
      safeState = true;
    }
  } else {
    static bool safeState = false;
    if (safeState) safeState = false;  // Reset flag when control resumes
  }

  delay(10);
}