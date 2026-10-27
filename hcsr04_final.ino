#include <WiFi.h>
#include <WebServer.h>
 
const char* ssid     = "Students 1"; // WiFi Credentials
const char* password = "FutureisTech";

 #define NUM_SENSORS 3 
const int TRIG[] = {33, 26, 13}; // Pins
const int ECHO[] = {32, 25, 12};
#define THRESHOLD_CM 10
#define BUZZER 27  // Change to your buzzer pin

WebServer server(80);
float distances[NUM_SENSORS];

float getDistance(int trigPin, int echoPin) {
  digitalWrite(trigPin, LOW);
  delayMicroseconds(2);
  digitalWrite(trigPin, HIGH);
  delayMicroseconds(10);
  digitalWrite(trigPin, LOW);

  long duration = pulseIn(echoPin, HIGH, 30000);
  if (duration == 0) return -1;
  return duration * 0.034 / 2;
}

void handleData() {
  bool anyAlert = false;
  for (int i = 0; i < NUM_SENSORS; i++) {
    if (distances[i] > 0 && distances[i] <= THRESHOLD_CM) anyAlert = true;
  }

  String json = "{";
  for (int i = 0; i < NUM_SENSORS; i++) {
    json += "\"sensor" + String(i + 1) + "\":" + String(distances[i], 1);
    if (i < NUM_SENSORS - 1) json += ",";
  }
  json += ",\"alert\":" + String(anyAlert ? "true" : "false");
  json += "}";
  server.send(200, "application/json", json);
}

void handleRoot() {
  String html = R"rawliteral(
<!DOCTYPE html>
<html>
<head>
  <meta charset="UTF-8">
  <title>Ultrasonic Sensors</title>
  <meta name="viewport" content="width=device-width, initial-scale=1">
  <style>
    * { box-sizing: border-box; margin: 0; padding: 0; }
    body {
      font-family: 'Segoe UI', sans-serif;
      background: #0f172a;
      color: #e2e8f0;
      min-height: 100vh;
      display: flex;
      flex-direction: column;
      align-items: center;
      padding: 30px 20px;
    }
    h1 {
      font-size: 1.8rem;
      margin-bottom: 30px;
      color: #7dd3fc;
      letter-spacing: 1px;
    }
    .cards {
      display: flex;
      flex-wrap: wrap;
      gap: 20px;
      justify-content: center;
      width: 100%;
      max-width: 900px;
    }
    .card {
      background: #1e293b;
      border-radius: 16px;
      padding: 30px 25px;
      flex: 1;
      min-width: 220px;
      text-align: center;
      border: 2px solid #334155;
      transition: border 0.3s;
    }
    .card.alert {
      border-color: #ef4444;
      animation: pulse 0.6s infinite alternate;
    }
    @keyframes pulse {
      from { box-shadow: 0 0 10px #ef444488; }
      to   { box-shadow: 0 0 25px #ef4444cc; }
    }
    .card h2 { font-size: 1rem; color: #94a3b8; margin-bottom: 15px; }
    .distance { font-size: 3rem; font-weight: bold; color: #38bdf8; }
    .card.alert .distance { color: #ef4444; }
    .unit { font-size: 1.1rem; color: #64748b; margin-top: 5px; }
    .status { margin-top: 12px; font-size: 0.85rem; font-weight: 600; color: #22c55e; }
    .card.alert .status { color: #ef4444; }
    .alert-banner {
      display: none;
      background: #ef4444;
      color: white;
      padding: 14px 30px;
      border-radius: 12px;
      font-size: 1rem;
      font-weight: bold;
      margin-bottom: 25px;
      letter-spacing: 0.5px;
      animation: pulse 0.6s infinite alternate;
    }
    .alert-banner.show { display: block; }
  </style>
</head>
<body>
  <h1>📡 Ultrasonic Sensor Dashboard</h1>
  <div class="alert-banner" id="banner">🚨 WARNING! Object detected within 10cm!</div>
  <div class="cards">
    <div class="card" id="card1">
      <h2>Sensor 1</h2>
      <div class="distance" id="d1">--</div>
      <div class="unit">cm</div>
      <div class="status" id="s1">Reading...</div>
    </div>
    <div class="card" id="card2">
      <h2>Sensor 2</h2>
      <div class="distance" id="d2">--</div>
      <div class="unit">cm</div>
      <div class="status" id="s2">Reading...</div>
    </div>
    <div class="card" id="card3">
      <h2>Sensor 3</h2>
      <div class="distance" id="d3">--</div>
      <div class="unit">cm</div>
      <div class="status" id="s3">Reading...</div>
    </div>
  </div>

  <script>
    function update() {
      fetch('/data')
        .then(r => r.json())
        .then(data => {
          let anyAlert = false;
          for (let i = 1; i <= 3; i++) {
            const dist = data['sensor' + i];
            const card = document.getElementById('card' + i);
            const dEl  = document.getElementById('d' + i);
            const sEl  = document.getElementById('s' + i);

            if (dist == -1) {
              dEl.textContent = '--';
              sEl.textContent = 'No reading';
              card.classList.remove('alert');
            } else {
              dEl.textContent = dist.toFixed(1);
              if (dist <= 10) {
                card.classList.add('alert');
                sEl.textContent = '⚠️ Too close!';
                anyAlert = true;
              } else {
                card.classList.remove('alert');
                sEl.textContent = '✅ Clear';
              }
            }
          }
          document.getElementById('banner').classList.toggle('show', anyAlert);
        });
    }
    setInterval(update, 500);
    update();
  </script>
</body>
</html>
  )rawliteral";
  server.send(200, "text/html", html);
}

void setup() {
  Serial.begin(115200);

  pinMode(BUZZER, OUTPUT);
  for (int i = 0; i < NUM_SENSORS; i++) {
    pinMode(TRIG[i], OUTPUT);
    pinMode(ECHO[i], INPUT);
  }

  WiFi.begin(ssid, password);
  Serial.print("Connecting to WiFi");
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("\nConnected! Open this IP in your browser:");
  Serial.println(WiFi.localIP());

  server.on("/", handleRoot);
  server.on("/data", handleData);
  server.begin();
}

void loop() {
  bool anyAlert = false;

  for (int i = 0; i < NUM_SENSORS; i++) {
    distances[i] = getDistance(TRIG[i], ECHO[i]);
    if (distances[i] > 0 && distances[i] <= THRESHOLD_CM) anyAlert = true;
  }

  if (anyAlert) {
    digitalWrite(BUZZER, HIGH);
  } else {
    digitalWrite(BUZZER, LOW);
  }

  server.handleClient();
}