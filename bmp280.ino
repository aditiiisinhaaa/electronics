#include <WiFi.h>
#include <Wire.h>
#include <Adafruit_BMP280.h>

// Replace with your Wi-Fi credentials
const char* ssid = "Students 1";
const char* password = "FutureisTech";

Adafruit_BMP280 bmp;  // I2C interface

WiFiServer server(80);

void setup() {
  Serial.begin(115200);

  // Initialize BMP280
  if (!bmp.begin(0x76)) {  // Try 0x76 first; some modules use 0x77
    Serial.println("Could not find a valid BMP280 sensor, check wiring!");
    while (1)
      ;
  }
  // Optional settings (default is fine for most cases)
  bmp.setSampling(Adafruit_BMP280::MODE_NORMAL,
                  Adafruit_BMP280::SAMPLING_X2,   // Temperature oversampling
                  Adafruit_BMP280::SAMPLING_X16,  // Pressure oversampling
                  Adafruit_BMP280::FILTER_X16,
                  Adafruit_BMP280::STANDBY_MS_500);

  // Connect to Wi-Fi
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("\nWiFi connected!");
  Serial.print("IP address: ");
  Serial.println(WiFi.localIP());

  server.begin();
}

void loop() {
  WiFiClient client = server.available();
  if (client) {
    String request = client.readStringUntil('\r');
    client.flush();

    // Read sensor values
    float temperature = bmp.readTemperature();     // °C
    float pressure = bmp.readPressure() / 100.0F;  // hPa
    float altitude = bmp.readAltitude(1013.25);    // Approx. altitude (m); adjust sea-level pressure if needed

    // Send HTTP response with HTML page
    client.println("HTTP/1.1 200 OK");
    client.println("Content-type:text/html");
    client.println("Refresh:10");  // Auto-refresh every 10 seconds
    client.println();

    client.println("<!DOCTYPE html><html>");
    client.println("<head><meta name=\"viewport\" content=\"width=device-width, initial-scale=1\">");
    client.println("<title>ESP32 BMP280 Weather Station</title>");
    client.println("<style>body{font-family:Arial;text-align:center;background:#f0f0f0;}");
    client.println("h1{color:#333;} .reading{font-size:2em;margin:20px;}</style></head>");
    client.println("<body><h1>ESP32 + BMP280</h1>");
    client.printf("<div class=\"reading\">Temperature: %.2f &deg;C</div>", temperature);
    client.printf("<div class=\"reading\">Pressure: %.2f hPa</div>", pressure);
    client.printf("<div class=\"reading\">Approx. Altitude: %.2f m</div>", altitude);
    client.println("<p>Page refreshes every 10 seconds.</p></body></html>");

    client.stop();
  }
}