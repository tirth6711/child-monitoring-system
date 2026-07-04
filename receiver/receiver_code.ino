#include <WiFi.h>
#include <WebServer.h>
#include <SPI.h>
#include <LoRa.h>
#include <math.h>

#define VIBRATOR_PIN 27      // change if needed
#define SAFE_DISTANCE 50.0   // meters (your fixed range)


/* WiFi */
const char* ssid = "Vasu patel";
const char* password = "ABCD5678";

WebServer server(80);

/* LoRa Pins */
#define SS    5
#define RST   14
#define DIO0  26

double fatherLat = 0.0;
double fatherLon = 0.0;

double childLat = 0.0;
double childLon = 0.0;

bool fatherReady = false;
bool childReady  = false;

/* Send data to webpage */
void handleData()
{
  double distance = 0;

  if (fatherReady && childReady)
    distance = calculateDistance(fatherLat, fatherLon, childLat, childLon);

  String json = "{";
  json += "\"fatherLat\":" + String(fatherLat,6) + ",";
  json += "\"fatherLon\":" + String(fatherLon,6) + ",";
  json += "\"childLat\":" + String(childLat,6) + ",";
  json += "\"childLon\":" + String(childLon,6) + ",";
  json += "\"distance\":" + String(distance,2);
  json += "}";
  server.sendHeader("Access-Control-Allow-Origin", "*");
  server.send(200, "application/json", json);
}

/* Distance */
double calculateDistance(double lat1, double lon1, double lat2, double lon2)
{
  const double R = 6371000.0;

  double phi1 = lat1 * PI / 180.0;
  double phi2 = lat2 * PI / 180.0;

  double dPhi = (lat2 - lat1) * PI / 180.0;
  double dLambda = (lon2 - lon1) * PI / 180.0;

  double a = sin(dPhi/2.0) * sin(dPhi/2.0) +
             cos(phi1) * cos(phi2) *
             sin(dLambda/2.0) * sin(dLambda/2.0);

  double c = 2 * atan2(sqrt(a), sqrt(1-a));
  return R * c;
}

void printDistance()
{
  if (!fatherReady || !childReady) return;

  double d = calculateDistance(fatherLat, fatherLon, childLat, childLon);

  Serial.println("\n📏 DISTANCE UPDATE");
  Serial.print("Father: ");
  Serial.print(fatherLat,6);
  Serial.print(", ");
  Serial.println(fatherLon,6);

  Serial.print("Child : ");
  Serial.print(childLat,6);
  Serial.print(", ");
  Serial.println(childLon,6);

  Serial.print("Distance = ");
  Serial.print(d,2);
  Serial.println(" meters");

  if (d > SAFE_DISTANCE)
  {
    Serial.println("⚠ ALERT: Child outside safe range");
    digitalWrite(VIBRATOR_PIN, HIGH);   // Vibrate ON
  }
  else
  {
    digitalWrite(VIBRATOR_PIN, LOW);    // Stop vibration
    Serial.println("✅ Child inside safe range");
  }

  Serial.println("-----------------------");
}


/* Father mobile data */
void handleLocation()
{
  if (server.hasArg("lat") && server.hasArg("lon"))
  {
    fatherLat = server.arg("lat").toDouble();
    fatherLon = server.arg("lon").toDouble();
    fatherReady = true;

    Serial.println("\n👨 Father Location Updated");
    printDistance();
  }
  server.sendHeader("Access-Control-Allow-Origin", "*");
  server.send(200, "text/plain", "OK");
}

void setup()
{
  Serial.begin(115200);

  WiFi.begin(ssid, password);
  Serial.print("Connecting WiFi");

  pinMode(VIBRATOR_PIN, OUTPUT);
 digitalWrite(VIBRATOR_PIN, LOW);

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  Serial.println("\nWiFi Connected");
  Serial.println(WiFi.localIP());

  server.on("/location", handleLocation);
  server.on("/data", handleData);
  server.begin();

  SPI.begin(18, 19, 23, 5);
  LoRa.setPins(SS, RST, DIO0);

  if (!LoRa.begin(433E6)) {
    Serial.println("LoRa Failed");
    while (1);
  }

  /* MUST MATCH CHILD SETTINGS */
  LoRa.setSpreadingFactor(7);
  LoRa.setSignalBandwidth(125E3);
  LoRa.setCodingRate4(5);
  LoRa.enableCrc();
  LoRa.setSyncWord(0x12);

  Serial.println("System Ready");
}

void loop()
{
  server.handleClient();

  int packetSize = LoRa.parsePacket();

  if (packetSize)
  {
    String received = "";

    while (LoRa.available()) {
      received += (char)LoRa.read();
    }

    Serial.println("\n📡 Received from Child:");
    Serial.println(received);

    int latIndex = received.indexOf("Lat:");
    int lngIndex = received.indexOf("Lng:");

    if (latIndex != -1 && lngIndex != -1)
    {
      int commaIndex = received.indexOf(',');

      String latStr = received.substring(latIndex + 4, commaIndex);
      String lonStr = received.substring(lngIndex + 4);

      childLat = latStr.toDouble();
      childLon = lonStr.toDouble();
      childReady = true;

      Serial.println("🧒 Child Location Updated");
      printDistance();
    }
  }
}