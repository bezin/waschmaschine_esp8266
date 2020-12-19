#include <Arduino.h>;
#include <ESP8266WiFi.h>;
#include <ESP8266HTTPClient.h>;
#include <WiFiClient.h>;
#include <time.h>

#define WIFI_SSID "Helsinki"
#define WIFI_PASSPHRASE "dasisteintollespasswort123"
#define WIFI_HOSTNAME "waschmaschine"

#define ENDPOINT "http://192.168.178.20:8080/api.php"

// Structures

struct DataPoint {
  int tstamp;
  int x;
  int y;
  int z;
};

// Functions

void flashLED(int flashCount = 3);
int calculateDelta(int val, int lastVal);
int storeValue(DataPoint payload);
String serializePayload(DataPoint payload);

// Variables

int xPin = A0;
int x = 0;
int xLast = 0;

DataPoint data;
WiFiClient client;
HTTPClient http;

void setup() {
  // Connect D0 to RST to wake up
  pinMode(D0, WAKEUP_PULLUP);
  pinMode(LED_BUILTIN, OUTPUT);

  pinMode(xPin, INPUT);

  Serial.begin(115200);
  WiFi.hostname(WIFI_HOSTNAME);
  WiFi.begin(WIFI_SSID, WIFI_PASSPHRASE);

  Serial.print("Connecting…");

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.println(".");
    flashLED(1);
  }

  Serial.println("Connected!");
  Serial.println(WiFi.localIP());
  flashLED(2);
}

void loop() {
  delay(5000);
  flashLED(1);

  x = analogRead(xPin);
  int xDelta = calculateDelta(x, xLast);
  Serial.println("======");
  Serial.println(x);

  data.tstamp = time(NULL);
  data.x = xDelta;
  data.y = 0;
  data.z = 0;

  storeValue(data);
}

int calculateDelta(int val, int lastVal) {
  int valDelta = (val - lastVal);
  lastVal = val;
  return valDelta;
}

int storeValue(DataPoint payload) {
  int code;
  bool success;

  success = http.begin(client, ENDPOINT);
  http.addHeader("Content-Type", "application/x-www-form-urlencoded");

  if (success) {
    Serial.println(serializePayload(payload));
    code = http.POST(serializePayload(payload));
    Serial.println(String(code));
  } else {
    Serial.println("Could not connect to Endpoint");
  }
}

String serializePayload(DataPoint payload) {
  String serializedPayload;
  return serializedPayload
    + "value[timestamp]=" + payload.tstamp
    + "&value[x]="        + payload.x
    + "&value[y]="        + payload.y
    + "&value[z]="        + payload.z;
}

void flashLED(int flashCount) {
  for(int i = 0; i < flashCount; i++) {
    digitalWrite(LED_BUILTIN, LOW);
    delay(200);
    digitalWrite(LED_BUILTIN, HIGH);
    delay(200);
  }
}