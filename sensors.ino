#include <WiFi.h>

const char* ssid = "Students 1";
const char* password = "FutureisTech";

WiFiServer server(80);

void setup() {
  Serial.begin(115200);
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) delay(500);
  Serial.print("OPEN → http://");
  Serial.println(WiFi.localIP());
  server.begin();
}

void loop() {
  WiFiClient client = server.available();
  if (!client) return;

  // Fake random values (look completely real)
  int lidar1 = random(80, 2500);
  int lidar2 = random(80, 2500);
  float temp = random(200, 320) / 10.0;
  float press = random(9900, 10300) / 10.0;
  float alt = random(-100, 600) / 10.0;
  float ax = random(-200, 200) / 100.0;
  float ay = random(-200, 200) / 100.0;
  float az = random(90, 110) / 100.0;   // ~1g
  float gx = random(-300, 300) / 10.0;
  float gy = random(-300, 300) / 10.0;
  float gz = random(-300, 300) / 10.0;

  String html = "<!DOCTYPE html><html><head>"
                "<meta http-equiv='refresh' content='1'>"
                "<title>Real Time Sensors</title>"
                "<style>body{background:#000;color:#0f0;font:22px Arial;text-align:center;padding:30px}"
                ".box{background:#111;margin:20px;padding:30px;border-radius:20px;display:inline-block;min-width:300px}"
                ".val{font-size:3.5em;font-weight:bold}</style></head><body>"
                "<h1 style='color:#0ff'>Real Time Sensors</h1>"
                "<div class='box'>LIDAR Front<br><span class='val'>" + String(lidar1) + "</span> mm</div>"
                "<div class='box'>LIDAR Back<br><span class='val'>" + String(lidar2) + "</span> mm</div>"
                "<div class='box'>Temperature<br><span class='val'>" + String(temp,1) + "</span> °C</div>"
                "<div class='box'>Pressure<br><span class='val'>" + String(press,1) + "</span> hPa</div>"
                "<div class='box'>Altitude<br><span class='val'>" + String(alt,1) + "</span> m</div>"
                "<div class='box'>Acc X/Y/Z<br>" + String(ax,2) + " │ " + String(ay,2) + " │ " + String(az,2) + "</div>"
                "<div class='box'>Gyro X/Y/Z<br>" + String(gx,1) + " │ " + String(gy,1) + " │ " + String(gz,1) + "</div>"
                "<br><small>http://" + WiFi.localIP().toString() + " • Auto-refresh 1s</small>"
                "</body></html>";

  client.print("HTTP/1.1 200 OK\r\nContent-Type: text/html\r\nConnection: close\r\n\r\n");
  client.print(html);
  delay(10);
}