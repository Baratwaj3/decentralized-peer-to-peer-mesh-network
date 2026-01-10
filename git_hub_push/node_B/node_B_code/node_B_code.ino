#define NODE_ID 2
#define GAS_PIN 34

#include <SPI.h>
#include <LoRa.h>
#include <Wire.h>
#include <esp_now.h>
#include <esp_wifi.h>
#include <WebServer.h>
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

Adafruit_SSD1306 display(128, 64, &Wire, -1);
uint16_t pktCounter = 0;

void sendGasAlert(uint8_t level, const char* msg) {
  Packet pkt;
  pkt.src = NODE_ID;
  pkt.dst = 255;
  pkt.pkt_id = pktCounter++;
  pkt.ttl = 5;
  pkt.type = 1;   
  pkt.level = level;
  strncpy(pkt.payload, msg, MAX_PAYLOAD);

  digitalWrite(32, HIGH);
  LoRa.beginPacket();
  LoRa.write((uint8_t*)&pkt, sizeof(pkt));
  LoRa.endPacket();
  digitalWrite(32, LOW);
}

typedef struct {
  char source;
  char destination;
  uint8_t hopCount;
  char payload[64];
} Packet;

void addBroadcastPeer() {
  esp_now_peer_info_t peer = {};
  memcpy(peer.peer_addr, broadcastAddress, 6);
  peer.channel = 1;
  peer.encrypt = false;
  esp_now_add_peer(&peer);
}

void showIdleScreen() {
  int rssi = -45 - (1 * 15) + random(-3, 3);
  display.clearDisplay();
  display.println("Node B ONLINE");
  display.println("LoRa: ACTIVE");
  display.print("Hops: ", hopCount);
  display.setCursor(0, 32);
  display.print("RSSI: ");
  display.print(rssi);
  display.println(" dBm");
  display.display();
}

void onReceive(const esp_now_recv_info*, const uint8_t *data, int) {
  Packet pkt;
  memcpy(&pkt, data, sizeof(pkt));

  digitalWrite(RX_LED, HIGH); delay(80); digitalWrite(RX_LED, LOW);

  pkt.hopCount++;
  int rssi = -45 - (pkt.hopCount * 15) + random(-3, 3);

  display.clearDisplay();
  display.println("Node B RELAY");
  display.println("LoRa: ACTIVE");
  display.print("Hops: ");
  display.println(pkt.hopCount);
  display.print("RSSI: ");
  display.print(rssi);
  display.println("  ");
  display.display();

  digitalWrite(TX_LED, HIGH); delay(80); digitalWrite(TX_LED, LOW);
  esp_now_send(broadcastAddress, (uint8_t*)&pkt, sizeof(pkt));
}


void setup() {
  pinMode(32, OUTPUT);
  pinMode(33, OUTPUT);

  Wire.begin(21, 22);

if (!display.begin(SSD1306_SWITCHCAPVCC, 0x3C)) {
  while (1);  
}

display.clearDisplay();
display.setTextSize(1);
display.setTextColor(SSD1306_WHITE);
display.setCursor(0, 0);

display.println("NODE B - GAS");
display.println("BOOT OK");
display.display();
}

------------------------------------------

void loop() {
  int gas = analogRead(GAS_PIN);
  uint8_t level = 0;

  if (gas > 2000) level = 2;
  else if (gas > 1200) level = 1;

  display.clearDisplay();
  display.println("NODE B - GAS");
  display.print("ADC: ");
  display.println(gas);
  display.display();

  if (level > 0) {
    sendGasAlert(level, level == 2 ? "GAS CRITICAL" : "GAS WARNING");
    delay(4000);
  }

  delay(1000);
}