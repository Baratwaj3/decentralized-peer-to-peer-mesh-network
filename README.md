# 📡 Offline LoRa-Based Disaster Communication Network

## Project Overview

This project implements a *completely offline, decentralized wireless communication system* designed for disaster and infrastructure-failure scenarios.
The system operates *without internet, cellular networks, or centralized servers, using **LoRa (Long Range Radio)* to enable reliable communication between geographically separated nodes.

Each node represents a different real-world environment (city, mining site, ocean site, earthquake zone) and can *generate and broadcast alerts independently*.
A central monitoring node aggregates and displays these alerts in real time.

---

## Key Features

* Fully offline communication (no Wi-Fi, no internet, no cloud)
* Long-range, low-power LoRa communication
* Decentralized broadcast architecture
* Multiple independent alert-generating nodes
* Real-time alert display at a command node
* Modular and extensible design

---

## Network Architecture Summary

| Node   | Role                      | Environment Simulated      |
| ------ | ------------------------- | -------------------------- |
| Node A | Command & Monitoring Node | City / Commercial Area     |
| Node B | Alert Source Node         | Mining Site (Gas Hazard)   |
| Node C | Alert Source Node         | Ocean / Weather Monitoring |
| Node D | Alert Source Node         | Earthquake-Prone Zone      |

All nodes communicate using a *common packet format* and *shared LoRa configuration*.

---

# 📦 Common Hardware & Configuration (All Nodes)

## Microcontroller

* ESP32 (Dev Module)

## Radio Module

* LoRa RA-02 (SX1278)
* Frequency: *433 MHz*
* Spreading Factor: *9*
* CRC: Enabled

## Common LoRa Wiring

| RA-02 Pin | ESP32 GPIO |
| --------- | ---------- |
| VCC       | 3.3 V      |
| GND       | GND        |
| SCK       | GPIO 18    |
| MISO      | GPIO 19    |
| MOSI      | GPIO 23    |
| NSS / CS  | GPIO 5     |
| RESET     | GPIO 14    |
| DIO0      | GPIO 26    |

⚠️ *Important:*

* Antenna must be connected before powering
* RA-02 must never be powered with 5 V
* SPI wiring quality is critical (short wires, good solder joints)

---

## Common Packet Format

All nodes use the same data structure for seamless interoperability:

cpp
#define MAX_PAYLOAD 32

struct Packet {
  uint8_t node_id;      // Sender node ID
  uint8_t event_type;   // 1=Gas, 2=Weather, 3=Seismic
  uint8_t level;        // 0=Normal, 1=Warning, 2=Critical
  char message[MAX_PAYLOAD];
};


---

# 🟢 Node A — Command & Monitoring Node

## Purpose

Node A acts as the *central monitoring station*.
It listens to all LoRa transmissions and displays the *latest alert received* from any node.

This node represents a *city or command center* that aggregates disaster information from remote locations.

---

## Responsibilities

* Initialize and maintain LoRa reception
* Decode incoming packets
* Display node ID, event type, severity level, and message
* Operate continuously without freezing or flickering

---

## Additional Hardware

### OLED Display (SSD1306)

| OLED Pin | ESP32 GPIO |
| -------- | ---------- |
| SDA      | GPIO 21    |
| SCL      | GPIO 22    |
| VCC      | 3.3 V      |
| GND      | GND        |

---

## Behavior

* Displays “Listening…” on startup
* Updates display whenever a new packet is received
* Never transmits packets
* Acts purely as a monitoring endpoint

---

## Expected Output

Example display:


NODE A - COMMAND
FROM NODE: 3
EVENT: 2
LEVEL: 1
HUMIDITY RISE


---

# 🟡 Node B — Mining Site / Gas Alert Node

## Purpose

Node B simulates a *mining or industrial site*, where hazardous gas buildup is a major risk.

In the current implementation, gas events are *simulated* to validate network behavior before integrating real sensors.

---

## Responsibilities

* Periodically generate gas-related alerts
* Broadcast alerts using LoRa
* Operate independently without external triggers

---

## Event Types

| Level | Meaning      |
| ----- | ------------ |
| 0     | Gas Normal   |
| 1     | Gas Warning  |
| 2     | Gas Critical |

---

## Behavior

* Cycles through NORMAL → WARNING → CRITICAL
* Transmits one packet every 5 seconds
* Does not receive or display packets

---

## Example Messages

* GAS NORMAL
* GAS WARNING
* GAS CRITICAL

---

# 🔵 Node C — Ocean / Weather Monitoring Node

## Purpose

Node C represents an *ocean-side or coastal monitoring station*, where weather conditions can escalate rapidly.

Weather alerts are currently *simulated* to validate communication stability.

---

## Responsibilities

* Generate weather-related alerts
* Broadcast status changes via LoRa
* Operate continuously and autonomously

---

## Event Types

| Level | Meaning                   |
| ----- | ------------------------- |
| 0     | Weather Normal            |
| 1     | Rising Humidity / Warning |
| 2     | Storm Alert               |

---

## Behavior

* Cycles through weather conditions
* Sends packets every 6 seconds
* Stateless, no dependency on other nodes

---

## Example Messages

* WEATHER NORMAL
* HUMIDITY RISE
* STORM ALERT

---

# 🔴 Node D — Earthquake / Seismic Alert Node

## Purpose

Node D simulates a *seismic monitoring station* deployed in an earthquake-prone region.

For system validation, seismic events are *software-simulated*.

---

## Responsibilities

* Generate seismic alert messages
* Broadcast alerts at regular intervals
* Represent sudden and high-priority disaster events

---

## Event Types

| Level | Meaning       |
| ----- | ------------- |
| 0     | Ground Stable |
| 1     | Minor Tremor  |
| 2     | Seismic Alert |

---

## Behavior

* Cycles through seismic states
* Transmits every 7 seconds
* Designed to be extended with real IMU sensors later

---

## Example Messages

* GROUND STABLE
* MINOR TREMOR
* SEISMIC ALERT

---

# 🧪 Testing & Validation Strategy

1. Test each LoRa module independently using minimal SPI test code
2. Verify Node A receives packets from each node individually
3. Power all nodes simultaneously
4. Confirm stable, continuous updates on Node A
5. Observe packet timing and display consistency

---

# 🚀 Future Enhancements

* Integration of real sensors (MQ-9, DHT, MPU6050)
* Heartbeat packets for node health monitoring
* TTL / hop-count based forwarding
* Mesh routing algorithms
* Laptop-based monitoring dashboard
* Lightweight packet authentication

---

## Final Note

This project focuses on *resilience, decentralization, and failure-tolerant communication*, rather than high data rates or cloud connectivity.

It demonstrates how *critical information can still flow when conventional networks collapse*.
