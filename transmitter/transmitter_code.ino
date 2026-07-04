#include <TinyGPS++.h>
#include <SPI.h>
#include <LoRa.h>

/* -------- LoRa Pins -------- */
#define SS    5
#define RST   14
#define DIO0  26

/* -------- GPS Pins (Changed) -------- */
#define GPS_RX 4   // ESP32 RX
#define GPS_TX 2   // ESP32 TX

TinyGPSPlus gps;
HardwareSerial gpsSerial(1);

unsigned long lastSendTime = 0;

void setup() {
  Serial.begin(115200);
  delay(2000);

  Serial.println("🚀 ESP32 GPS + LoRa TRANSMITTER");

  /* -------- GPS Init -------- */
  gpsSerial.begin(9600, SERIAL_8N1, GPS_RX, GPS_TX);
  Serial.println("📍 GPS Initialized");

  /* -------- LoRa Init -------- */
  SPI.begin(18, 19, 23, SS);
  LoRa.setPins(SS, RST, DIO0);

  if (!LoRa.begin(433E6)) {
    Serial.println("❌ LoRa init failed!");
    while (1);
  }

  // Matching settings with RX
  LoRa.setSpreadingFactor(7);
  LoRa.setSignalBandwidth(125E3);
  LoRa.setCodingRate4(5);
  LoRa.enableCrc();
  LoRa.setSyncWord(0x12);

  Serial.println("✅ LoRa Initialized Successfully");
  Serial.println("--------------------------------");
}

void loop() {

  // Read GPS data continuously
  while (gpsSerial.available()) {
    gps.encode(gpsSerial.read());
  }

  // If new GPS location available
  if (gps.location.isUpdated()) {

    double latitude  = gps.location.lat();
    double longitude = gps.location.lng();

    Serial.print("Latitude  : ");
    Serial.println(latitude, 6);

    Serial.print("Longitude : ");
    Serial.println(longitude, 6);

    // Send every 2 seconds
    if (millis() - lastSendTime > 2000) {

      String data = String(latitude, 6) + "," + String(longitude, 6);

      LoRa.beginPacket();
      LoRa.print(data);
      LoRa.endPacket();

      Serial.println("📤 GPS Sent via LoRa");
      Serial.println("--------------------------------");

      lastSendTime = millis();
    }
  }
}