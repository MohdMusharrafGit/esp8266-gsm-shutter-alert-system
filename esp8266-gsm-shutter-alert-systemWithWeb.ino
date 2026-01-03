#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h>
#include <EEPROM.h>
#include <SoftwareSerial.h>

// ---------------- CONFIG ----------------
#define API_KEY "o6lCbSxiaWS1"
String CLOUD_URL = "https://www.dataloggersindia.org/sync";

// ---------------- PINS ------------------
// SIM800L
#define GSM_RX 12          // D6
#define GSM_TX 13          // D7

// Shutter pins (D1, D2, D5)
const int shutterPins[] = {5, 4, 10};
const int shutterCount = 3;

// Config button
#define CONFIG_BUTTON 16   // D0 (GPIO16)

// Status LED
#define STATUS_LED 2       // D4 (Active LOW)

// ---------------- OBJECTS ---------------
SoftwareSerial sim800(GSM_RX, GSM_TX);
ESP8266WebServer server(80);

// ---------------- GLOBALS ---------------
char mob1[16], mob2[16], mob3[16];

int shutterState[3];
int lastShutterState[3];

// ---------------- EEPROM ----------------
void readNumbers() {
  for (int i = 0; i < 15; i++) {
    mob1[i] = EEPROM.read(i);
    mob2[i] = EEPROM.read(20 + i);
    mob3[i] = EEPROM.read(40 + i);
  }
  mob1[15] = mob2[15] = mob3[15] = '\0';

  Serial.println("Saved Numbers:");
  Serial.println(mob1);
  Serial.println(mob2);
  Serial.println(mob3);
}

// ---------------- GSM HELPERS -----------
void sendAT(String cmd, int waitMs = 1000) {
  Serial.println(">> " + cmd);
  sim800.println(cmd);
  delay(waitMs);
  while (sim800.available()) {
    Serial.write(sim800.read());
  }
}

void gsmInit() {
  Serial.println("Initializing SIM800L...");
  delay(5000);

  sendAT("AT");
  sendAT("ATE0");
  sendAT("AT+CPIN?");
  sendAT("AT+CSQ");
  sendAT("AT+CREG?");
  sendAT("AT+CMGF=1");
  sendAT("AT+CSCS=\"GSM\"");

  Serial.println("GSM READY");
}

// ---------------- SMS -------------------
void sendSMS(const char *num, String msg) {
  if (strlen(num) < 10) return;

  Serial.println("SMS → " + String(num));
  sim800.print("AT+CMGS=\"");
  sim800.print(num);
  sim800.print("\"\r");
  delay(1500);

  sim800.print(msg);
  sim800.write(26);
  delay(6000);
}

void alertAll(String msg) {
  sendSMS(mob1, msg); delay(8000);
  sendSMS(mob2, msg); delay(8000);
  sendSMS(mob3, msg); delay(8000);
}

// ---------------- SHUTTER LOGIC ---------
void readShutters() {
  for (int i = 0; i < shutterCount; i++) {
    shutterState[i] = digitalRead(shutterPins[i]);

    if (shutterState[i] != lastShutterState[i]) {
      delay(50); // debounce
      shutterState[i] = digitalRead(shutterPins[i]);

      if (shutterState[i] != lastShutterState[i]) {
        lastShutterState[i] = shutterState[i];

        String state = (shutterState[i] == HIGH) ? "OPEN" : "CLOSED";
        String msg = "Shutter " + String(i + 1) + " " + state;

        Serial.println(msg);

        digitalWrite(STATUS_LED, LOW);
        alertAll(msg);
        digitalWrite(STATUS_LED, HIGH);
      }
    }
  }
}

// ---------------- WEB CONFIG ------------
void startConfigPortal() {
  WiFi.mode(WIFI_AP);
  WiFi.softAP("ESP_SHUTTER_CFG");

  server.on("/", []() {
    String page = R"rawliteral(
<!DOCTYPE html>
<html>
<head>
<meta name="viewport" content="width=device-width, initial-scale=1">
<title>Shutter Alert Config</title>

<style>
body {
  margin: 0;
  font-family: Arial, Helvetica, sans-serif;
  background: linear-gradient(135deg, #1d2671, #c33764);
  color: #fff;
}

.container {
  max-width: 360px;
  margin: 40px auto;
  background: #ffffff;
  color: #333;
  border-radius: 10px;
  padding: 20px;
  box-shadow: 0 10px 25px rgba(0,0,0,0.3);
}

h2 {
  text-align: center;
  margin-bottom: 20px;
  color: #1d2671;
}

label {
  font-weight: bold;
  margin-top: 10px;
  display: block;
}

input[type="text"] {
  width: 100%;
  padding: 10px;
  margin-top: 5px;
  border-radius: 5px;
  border: 1px solid #ccc;
  font-size: 16px;
}

button {
  width: 100%;
  margin-top: 20px;
  padding: 12px;
  font-size: 16px;
  background: linear-gradient(135deg, #1d2671, #c33764);
  color: white;
  border: none;
  border-radius: 5px;
  cursor: pointer;
}

button:hover {
  opacity: 0.9;
}

.footer {
  text-align: center;
  font-size: 12px;
  margin-top: 15px;
  color: #666;
}
</style>
</head>

<body>
  <div class="container">
    <h2>Shutter Alert Setup</h2>

    <form action="/save">
      <label>Mobile Number 1</label>
      <input type="text" name="m1" placeholder="+91XXXXXXXXXX">

      <label>Mobile Number 2</label>
      <input type="text" name="m2" placeholder="+91XXXXXXXXXX">

      <label>Mobile Number 3</label>
      <input type="text" name="m3" placeholder="+91XXXXXXXXXX">

      <button type="submit">Save & Restart</button>
    </form>

    <div class="footer">
      ESP8266 Shutter Monitoring System
    </div>
  </div>
</body>
</html>
)rawliteral";

    server.send(200, "text/html", page);
  });

  server.on("/save", []() {
    String m1 = server.arg("m1");
    String m2 = server.arg("m2");
    String m3 = server.arg("m3");

    for (int i = 0; i < 15; i++) {
      EEPROM.write(i, i < m1.length() ? m1[i] : 0);
      EEPROM.write(20 + i, i < m2.length() ? m2[i] : 0);
      EEPROM.write(40 + i, i < m3.length() ? m3[i] : 0);
    }
    EEPROM.commit();

    server.send(200, "text/html",
      "<h2 style='text-align:center;color:green;'>Saved Successfully!<br>Restarting...</h2>");
    delay(2000);
    ESP.restart();
  });

  server.begin();

  unsigned long t = millis();
  while (millis() - t < 120000) {
    server.handleClient();
    delay(10);
  }

  ESP.restart();
}

// ---------------- SETUP -----------------
void setup() {
  Serial.begin(115200);
  sim800.begin(9600);

  EEPROM.begin(512);

  pinMode(CONFIG_BUTTON, INPUT_PULLUP);
  pinMode(STATUS_LED, OUTPUT);
  digitalWrite(STATUS_LED, HIGH);

  for (int i = 0; i < shutterCount; i++) {
    pinMode(shutterPins[i], INPUT_PULLUP);
    lastShutterState[i] = digitalRead(shutterPins[i]);
  }

  readNumbers();
  gsmInit();
}

// ---------------- LOOP ------------------
void loop() {
  if (digitalRead(CONFIG_BUTTON) == LOW) {
    delay(300);
    startConfigPortal();
  }

  readShutters();
  delay(200);
}
