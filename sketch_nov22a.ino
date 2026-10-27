#include <WiFi.h>
#include <ESPAsyncWebServer.h>
#include <Wire.h>

// Replace with your network credentials
const char* ssid = "BlueMoon";
const char* password = "9820077720";

// MPU9250 I2C address
const uint8_t MPU9250_ADDRESS = 0x68;

// Create AsyncWebServer object on port 80
AsyncWebServer server(80);
AsyncWebSocket ws("/ws");

// Sensor readings
float accelX, accelY, accelZ;
float gyroX, gyroY, gyroZ;
float temperature;

void initMPU9250() {
  Wire.beginTransmission(MPU9250_ADDRESS);
  Wire.write(0x6B); // PWR_MGMT_1 register
  Wire.write(0);    // Wake up MPU9250
  Wire.endTransmission(true);
  delay(100);
  
  Serial.println("MPU9250 initialized successfully!");
}

void readMPU9250() {
  Wire.beginTransmission(MPU9250_ADDRESS);
  Wire.write(0x3B); // Starting register for accelerometer readings
  Wire.endTransmission(false);
  Wire.requestFrom(MPU9250_ADDRESS, (uint8_t)14);
  
  // Read accelerometer data
  int16_t rawAccelX = Wire.read() << 8 | Wire.read();
  int16_t rawAccelY = Wire.read() << 8 | Wire.read();
  int16_t rawAccelZ = Wire.read() << 8 | Wire.read();
  
  // Read temperature
  int16_t rawTemp = Wire.read() << 8 | Wire.read();
  
  // Read gyroscope data
  int16_t rawGyroX = Wire.read() << 8 | Wire.read();
  int16_t rawGyroY = Wire.read() << 8 | Wire.read();
  int16_t rawGyroZ = Wire.read() << 8 | Wire.read();
  
  // Convert to actual values
  accelX = rawAccelX / 16384.0;
  accelY = rawAccelY / 16384.0;
  accelZ = rawAccelZ / 16384.0;
  
  gyroX = rawGyroX / 131.0;
  gyroY = rawGyroY / 131.0;
  gyroZ = rawGyroZ / 131.0;
  
  temperature = (rawTemp / 333.87) + 21.0;
}

void onWsEvent(AsyncWebSocket *server, AsyncWebSocketClient *client, AwsEventType type, void *arg, uint8_t *data, size_t len) {
  if (type == WS_EVT_CONNECT) {
    Serial.println("WebSocket client connected");
  } else if (type == WS_EVT_DISCONNECT) {
    Serial.println("WebSocket client disconnected");
  }
}

void setup() {
  Serial.begin(115200);
  delay(1000);
  
  Serial.println("\n\n========================================");
  Serial.println("ESP32 MPU9250 Real-Time Monitor");
  Serial.println("========================================");
  
  // Initialize I2C
  Wire.begin(21, 22);
  Wire.setClock(400000);
  Serial.println("I2C initialized on pins SDA=21, SCL=22");
  
  // Initialize MPU9250
  initMPU9250();
  
  // Connect to Wi-Fi
  Serial.print("Connecting to WiFi: ");
  Serial.println(ssid);
  WiFi.begin(ssid, password);
  
  int attempts = 0;
  while (WiFi.status() != WL_CONNECTED && attempts < 30) {
    delay(500);
    Serial.print(".");
    attempts++;
  }
  Serial.println();
  
  if (WiFi.status() == WL_CONNECTED) {
    Serial.println("========================================");
    Serial.println("WiFi Connected Successfully!");
    Serial.print("IP Address: ");
    Serial.println(WiFi.localIP());
    Serial.println("========================================");
    Serial.print("\nOpen your browser and go to: http://");
    Serial.println(WiFi.localIP());
    Serial.println("\nMove the sensor to see real-time values!\n");
  } else {
    Serial.println("WiFi connection failed!");
    Serial.println("Check: 1) SSID  2) Password  3) 2.4GHz WiFi");
    return;
  }
  
  // WebSocket setup
  ws.onEvent(onWsEvent);
  server.addHandler(&ws);
  
  // Route for root / web page
  server.on("/", HTTP_GET, [](AsyncWebServerRequest *request) {
    request->send(200, "text/html", R"rawliteral(
<!DOCTYPE html>
<html>
<head>
  <meta name="viewport" content="width=device-width, initial-scale=1">
  <title>MPU9250 Live Data</title>
  <style>
    body {
      font-family: Arial, sans-serif;
      margin: 0;
      padding: 20px;
      background: linear-gradient(135deg, #1e3c72 0%, #2a5298 100%);
      color: white;
      min-height: 100vh;
    }
    .container {
      max-width: 1200px;
      margin: 0 auto;
    }
    h1 {
      text-align: center;
      font-size: 2.5em;
      margin-bottom: 30px;
      text-shadow: 2px 2px 4px rgba(0,0,0,0.5);
    }
    .status {
      text-align: center;
      padding: 15px;
      border-radius: 10px;
      margin-bottom: 30px;
      font-size: 1.2em;
      font-weight: bold;
      background: #f44336;
    }
    .status.connected {
      background: #4CAF50;
    }
    .grid {
      display: grid;
      grid-template-columns: repeat(auto-fit, minmax(300px, 1fr));
      gap: 20px;
      margin-bottom: 20px;
    }
    .card {
      background: rgba(255, 255, 255, 0.1);
      backdrop-filter: blur(10px);
      border-radius: 15px;
      padding: 25px;
      box-shadow: 0 8px 32px rgba(0,0,0,0.3);
      border: 1px solid rgba(255,255,255,0.2);
    }
    .card h2 {
      margin-top: 0;
      margin-bottom: 20px;
      font-size: 1.5em;
      border-bottom: 2px solid rgba(255,255,255,0.3);
      padding-bottom: 10px;
    }
    .data-item {
      display: flex;
      justify-content: space-between;
      align-items: center;
      margin: 15px 0;
      padding: 10px;
      background: rgba(255,255,255,0.05);
      border-radius: 8px;
    }
    .label {
      font-size: 1.1em;
      font-weight: 500;
    }
    .value {
      font-size: 1.4em;
      font-weight: bold;
      font-family: 'Courier New', monospace;
      background: linear-gradient(135deg, #667eea 0%, #764ba2 100%);
      padding: 8px 20px;
      border-radius: 8px;
      min-width: 100px;
      text-align: center;
      transition: transform 0.1s;
    }
    .value.updating {
      transform: scale(1.05);
    }
    .indicator {
      display: inline-block;
      width: 12px;
      height: 12px;
      border-radius: 50%;
      background: #4CAF50;
      animation: pulse 1s infinite;
      margin-left: 10px;
    }
    @keyframes pulse {
      0%, 100% { opacity: 1; transform: scale(1); }
      50% { opacity: 0.5; transform: scale(1.2); }
    }
    .info {
      text-align: center;
      margin-top: 30px;
      padding: 20px;
      background: rgba(255,255,255,0.1);
      border-radius: 10px;
      font-size: 1.1em;
    }
  </style>
</head>
<body>
  <div class="container">
    <h1>🚀 MPU9250 Live Motion Sensor <span class="indicator"></span></h1>
    <div class="status" id="status">Disconnected</div>
    
    <div class="grid">
      <div class="card">
        <h2>📊 Accelerometer (g)</h2>
        <div class="data-item">
          <span class="label">X:</span>
          <span class="value" id="accelX">0.000</span>
        </div>
        <div class="data-item">
          <span class="label">Y:</span>
          <span class="value" id="accelY">0.000</span>
        </div>
        <div class="data-item">
          <span class="label">Z:</span>
          <span class="value" id="accelZ">0.000</span>
        </div>
      </div>
      
      <div class="card">
        <h2>🔄 Gyroscope (°/s)</h2>
        <div class="data-item">
          <span class="label">X:</span>
          <span class="value" id="gyroX">0.00</span>
        </div>
        <div class="data-item">
          <span class="label">Y:</span>
          <span class="value" id="gyroY">0.00</span>
        </div>
        <div class="data-item">
          <span class="label">Z:</span>
          <span class="value" id="gyroZ">0.00</span>
        </div>
      </div>
      
      <div class="card">
        <h2>🌡️ Temperature</h2>
        <div class="data-item">
          <span class="label">Temp:</span>
          <span class="value" id="temp">0.0 °C</span>
        </div>
      </div>
    </div>
    
    <div class="info">
      💡 Move, tilt, or rotate the sensor to see values change in real-time!
    </div>
  </div>
  
  <script>
    var gateway = `ws://${window.location.hostname}/ws`;
    var websocket;
    
    function initWebSocket() {
      console.log('Connecting to WebSocket...');
      websocket = new WebSocket(gateway);
      
      websocket.onopen = function(event) {
        console.log('WebSocket connected!');
        document.getElementById('status').textContent = '✅ Connected - Live Data';
        document.getElementById('status').classList.add('connected');
      };
      
      websocket.onclose = function(event) {
        console.log('WebSocket disconnected. Reconnecting...');
        document.getElementById('status').textContent = '❌ Disconnected - Reconnecting...';
        document.getElementById('status').classList.remove('connected');
        setTimeout(initWebSocket, 2000);
      };
      
      websocket.onmessage = function(event) {
        var data = JSON.parse(event.data);
        updateValues(data);
      };
      
      websocket.onerror = function(error) {
        console.log('WebSocket error:', error);
      };
    }
    
    function updateValues(data) {
      updateElement('accelX', data.accelX.toFixed(3));
      updateElement('accelY', data.accelY.toFixed(3));
      updateElement('accelZ', data.accelZ.toFixed(3));
      updateElement('gyroX', data.gyroX.toFixed(2));
      updateElement('gyroY', data.gyroY.toFixed(2));
      updateElement('gyroZ', data.gyroZ.toFixed(2));
      updateElement('temp', data.temp.toFixed(1) + ' °C');
    }
    
    function updateElement(id, value) {
      var element = document.getElementById(id);
      if (element.textContent !== value) {
        element.classList.add('updating');
        element.textContent = value;
        setTimeout(() => element.classList.remove('updating'), 100);
      }
    }
    
    window.addEventListener('load', initWebSocket);
  </script>
</body>
</html>
)rawliteral");
  });
  
  // Start server
  server.begin();
  Serial.println("Web server started!");
}

void loop() {
  // Read sensor data
  readMPU9250();
  
  // Send data via WebSocket to all connected clients
  String jsonData = "{";
  jsonData += "\"accelX\":" + String(accelX, 3) + ",";
  jsonData += "\"accelY\":" + String(accelY, 3) + ",";
  jsonData += "\"accelZ\":" + String(accelZ, 3) + ",";
  jsonData += "\"gyroX\":" + String(gyroX, 2) + ",";
  jsonData += "\"gyroY\":" + String(gyroY, 2) + ",";
  jsonData += "\"gyroZ\":" + String(gyroZ, 2) + ",";
  jsonData += "\"temp\":" + String(temperature, 1);
  jsonData += "}";
  
  ws.textAll(jsonData);

}