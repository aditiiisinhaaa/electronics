#include <WiFi.h>
#include <ESP32Servo.h>

// CHANGE YOUR WIFI
const char* ssid     = "Students 1";
const char* password = "FutureisTech";

WiFiServer server(80);
Servo myServo;
const int servoPin = 18;   // ←←← Your pin

void setup() {
  Serial.begin(115200);

  // Servo setup for ESP32
  ESP32PWM::allocateTimer(0);
  myServo.setPeriodHertz(50);
  myServo.attach(servoPin, 500, 2500);  // 500µs = 0°, 2500µs = 180°
  myServo.write(90);  // start centered

  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) delay(500);
  Serial.println("\nWiFi connected!");
  Serial.print("Open → http://");
  Serial.println(WiFi.localIP());

  server.begin();
}

void loop() {
  WiFiClient client = server.available();
  if (!client) return;

  String req = client.readStringUntil('\r');
  client.flush();

  // Only move while slider is being dragged
  if (req.indexOf("/move?a=") != -1) {
    int angle = req.substring(req.indexOf("a=") + 2).toInt();
    angle = constrain(angle, 0, 180);
    myServo.write(angle);           // ← moves instantly
    client.println("HTTP/1.1 200 OK\r\n");
    return;
  }

  // Webpage – sends command only on touchmove/mousemove
  client.println("HTTP/1.1 200 OK");
  client.println("Content-type:text/html");
  client.println();
  client.print(R"=====(
<!DOCTYPE html>
<html>
<head>
  <meta name="viewport" content="width=device-width,initial-scale=1">
  <title>ESP32 Pin 18 Servo</title>
  <style>
    body{background:#000;color:#0f0;font-family:Arial;text-align:center;padding:40px}
    h1{font-size:3.5em}
    input[type=range]{width:94%;height:120px;background:#222;border-radius:60px;outline:none}
    input[type=range]::-webkit-slider-thumb{width:140px;height:140px;background:#0f0;border-radius:50%;cursor:pointer}
    .val{font-size:6em;margin:40px}
  </style>
</head>
<body>
  <h1>ESP32 → Pin 18</h1>
  <input type="range" min="0" max="180" value="90"
   ontouchmove="move(this.value)" onmousemove="move(this.value)">
  <div class="val" id="val">90°</div>

<script>
function move(v){
  fetch("/move?a="+v);
  document.getElementById("val").innerText = v + "°";
}
</script>
</body>
</html>
)=====");

  client.stop();
}