#define NODE_ID 4

#include <SPI.h>
#include <LoRa.h>
#include <Wire.h>
#include <esp_now.h>
#include <esp_wifi.h>
#include <WebServer.h>
#include <MPU6050.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

#define MAX_PAYLOAD 48

struct Packet {
  uint8_t src;
  uint8_t dst;
  uint16_t pkt_id;
  uint8_t ttl;
  uint8_t type;    
  uint8_t level;   
  char payload[MAX_PAYLOAD];
};

TwoWire WireMPU = TwoWire(1);
MPU6050 mpu(MPU6050_DEFAULT_ADDRESS, &WireMPU);

Adafruit_SSD1306 display(128, 64, &Wire, -1);

typedef struct {
  char source;
  char destination;
  uint8_t hopCount;
  char payload[64];
} Packet;

String lastMsg = "None";

void addBroadcastPeer() {
  esp_now_peer_info_t peer = {};
  memcpy(peer.peer_addr, broadcastAddress, 6);
  peer.channel = 1;
  peer.encrypt = false;
  esp_now_add_peer(&peer);
}

void showIdleScreen() {
  int rssi = -45 - (3 * 15) + random(-3, 3);
  display.clearDisplay();
  display.println("Node D ONLINE");
  display.println("LoRa: ACTIVE");
  display.print("Hops: ", hopCount);
  display.setCursor(0, 32);
  display.print("RSSI: ");
  display.print(rssi);
  display.println(" ");
  display.display();
}

void onReceive(const esp_now_recv_info*, const uint8_t *data, int) {
  Packet pkt;
  memcpy(&pkt, data, sizeof(pkt));

  if (pkt.destination == 'D') {
    lastMsg = pkt.payload;
    int rssi = -45 - (pkt.hopCount * 15) + random(-3, 3);

    digitalWrite(RX_LED, HIGH); delay(80); digitalWrite(RX_LED, LOW);

    display.clearDisplay();
    display.println("Node D DEST");
    display.println("LoRa: ACTIVE");
    display.println(pkt.payload);
    display.print("RSSI: ");
    display.print(rssi);
    display.println(" ");
    display.display();
  }
}

void handleRoot() {
  server.send(200, "text/html",
    "<h3>Node D</h3><p>Received:</p><b>" + lastMsg + "</b>");
}

uint16_t packetCounter = 0;

void sendSeismic(uint8_t level, const char *msg) {
  Packet pkt;
  pkt.src = NODE_ID;
  pkt.dst = 255;
  pkt.pkt_id = packetCounter++;
  pkt.ttl = 6;
  pkt.type = 3;
  pkt.level = level;
  strncpy(pkt.payload, msg, MAX_PAYLOAD);

  digitalWrite(32, HIGH);
  LoRa.beginPacket();
  LoRa.write((uint8_t *)&pkt, sizeof(pkt));
  LoRa.endPacket();
  digitalWrite(32, LOW);
}

void setup() {
  pinMode(32, OUTPUT);
  pinMode(33, OUTPUT);
  digitalWrite(32, LOW);
  digitalWrite(33, LOW);

  delay(1500);

  Serial.begin(115200);
  Serial.println("NODE D BOOT");

  Wire.begin(21, 22);
  if (!display.begin(SSD1306_SWITCHCAPVCC, 0x3C)) {
    while (1);
  }

  display.clearDisplay();
  display.setTextSize(1);
  display.setTextColor(SSD1306_WHITE);
  display.setCursor(0, 0);
  display.println("NODE D - SEISMIC");
  display.println("OLED OK");
  display.display();

  WireMPU.begin(25, 27);
  mpu.initialize();
  if (!mpu.testConnection()) {
    display.clearDisplay();
    display.println("MPU FAIL");
    display.display();
    while (1);
  }

  LoRa.setPins(5, 14, 26);
  if (!LoRa.begin(433E6)) {
    display.clearDisplay();
    display.println("LoRa FAIL");
    display.display();
    while (1);
  }
  LoRa.setSpreadingFactor(9);
  LoRa.enableCrc();

  display.clearDisplay();
  display.println("NODE D - SEISMIC");
  display.println("READY");
  display.display();
}

------------------------------------------------------

void loop() {
  int16_t ax, ay, az;
  mpu.getAcceleration(&ax, &ay, &az);

  float g = sqrt(ax * ax + ay * ay + az * az) / 16384.0;

  uint8_t level = 0;
  if (g > 1.2) level = 2;
  else if (g > 0.5) level = 1;

  display.clearDisplay();
  display.setCursor(0, 0);
  display.println("NODE D - SEISMIC");
  display.print("G: ");
  display.println(g);

  if (level == 2) display.println("STATUS: CRITICAL");
  else if (level == 1) display.println("STATUS: WARNING");
  else display.println("STATUS: NORMAL");

  display.display();

  if (level > 0) {
    sendSeismic(level,
                level == 2 ? "SEISMIC CRITICAL" : "SEISMIC WARNING");
    delay(3000);
  }

  delay(1000);
}