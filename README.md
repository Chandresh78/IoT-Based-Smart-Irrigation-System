# Building an IoT Based Smart Irrigation System

This project demonstrates how to build an IoT-based smart irrigation system that automates watering based on soil moisture levels. The system uses a NodeMCU, sensors, and a relay to control a water pump. Data is transmitted to ThingSpeak for monitoring.

## Objectives

1. Build an IoT-based irrigation system.
2. Read sensor data and select thresholds for actuating a water pump.
3. Interface a water pump to a smart farm system using a relay.
4. Automate soil watering when the moisture level is low.

## Components and Requirements

### Hardware

- NodeMCU board (1 unit)
- Micro USB cable A to B (1 unit)
- Breadboard (1 unit)
- Soil moisture sensor (1 unit)
- DHT11 temperature and humidity sensor (1 unit)
- 3–12V mini water pump (1 unit)
- 5V DC – 250V AC Relay module (1 unit)
- Jumper wires – male to female (9 units)
- A Cloud platform (1 unit)
- WiFi router connected to the internet (1 unit)

### Software

- Arduino IDE
- ThingSpeak account

## Getting Started

A system can be completely autonomous if it can interpret sensor data and act accordingly. In this project, the main actuator is a water pump, which is controlled based on soil moisture levels.

### Understanding Actuators

Actuators are output devices that perform actions based on sensor data to ensure the system operates within predefined thresholds. In this project, the water pump is the actuator, and its role is to maintain adequate soil moisture.

#### Current Requirements

The water pump requires significant current (~1A) to operate, which the NodeMCU's digital pins (~100mA) cannot supply directly. To address this, we use an external DC power source and control it via a relay.

### Relay Switches

A relay switch controls a high-power device (like a water pump) using a low-power signal from the NodeMCU. The relay isolates the low current NodeMCU circuit from the high current motor circuit.

#### Working of a Relay

When a small current flows through the relay circuit, an electromagnetic coil creates a magnetic field that closes a metallic contact, thus activating the second circuit (water pump circuit). This allows the water pump to operate using an external power supply.

## Circuit Connections

1. Connect the positive of the pump to the positive of a 9V battery.
2. Connect the negative of the pump to the COMMON terminal of the relay.
3. Connect the negative of the battery to the NO pin of the relay.

## Code Implementation

```cpp
#include <DHT.h>
#include <ESP8266WiFi.h>
#include <WiFiClient.h>
#include <ThingSpeak.h>

#define DHTPIN D5
#define DHTTYPE DHT11
DHT dht(DHTPIN, DHTTYPE);

const char* ssid = "Enter your Wifi name";
const char* password = "Enter your Wifi password";
WiFiClient client;
unsigned long myChannelNumber = 354734;
const char* myWriteAPIKey = "JBNXNMFKVWQJ1NNV";

uint8_t temperature, humidity;
float sensor_output = 0;
float percent_water = 0;
int state = 1;

void setup() {
    pinMode(1, OUTPUT); // Relay pin
    digitalWrite(1, HIGH); // Switch off the pump to start off
    Serial.begin(115200);
    dht.begin();
    delay(10);
    
    // Connect to WiFi network
    Serial.println();
    Serial.println();
    Serial.print("Connecting to ");
    Serial.println(ssid);
    WiFi.begin(ssid, password);
    
    while (WiFi.status() != WL_CONNECTED) {
        delay(500);
        Serial.print(".");
    }
    Serial.println("");
    Serial.println("WiFi connected");
    // Print the IP address
    Serial.println(WiFi.localIP());
    ThingSpeak.begin(client);
}

void loop() {
    temperature = dht.readTemperature();
    humidity = dht.readHumidity();

    Serial.print("Temperature Value is :");
    Serial.print(temperature);
    Serial.println("C");
    Serial.print("Humidity Value is :");
    Serial.print(humidity);
    Serial.println("%");

    sensor_output = analogRead(A0); // Read sensor data from A0 pin
    percent_water = map(sensor_output, 550, 10, 0, 100); // Map sensor value to percentage
    Serial.println("Reading value from sensor...");
    Serial.println("Percentage of water in the soil is:");
    Serial.print(percent_water);
    Serial.print("%");

    // Write to ThingSpeak
    if (state == 1) {
        ThingSpeak.writeField(myChannelNumber, 1, temperature, myWriteAPIKey);
        state++;
        Serial.println(state);
        Serial.println("Channel 1 transmitted");
    } else if (state == 2) {
        ThingSpeak.writeField(myChannelNumber, 2, humidity, myWriteAPIKey);
        state++;
        Serial.println(state);
        Serial.println("Channel 2 transmitted");
    } else {
        ThingSpeak.writeField(myChannelNumber, 3, percent_water, myWriteAPIKey);
        Serial.println(state);
        Serial.println("Channel 3 transmitted");
        state = 1;
    }
    Serial.println(state);
    delay(30000);

    // Actuate water pump if moisture content drops below a certain level
    if (percent_water < 50) {
        digitalWrite(1, LOW); // Activate relay and the pump
        delay(3000); // Turn pump on for 3 seconds
    } else {
        digitalWrite(1, HIGH); // Deactivate pump
    }
}
```

## How It Works

1. **Initialization**: The system initializes the DHT sensor, connects to WiFi, and sets up the ThingSpeak client.
2. **Sensor Readings**: The DHT11 sensor reads temperature and humidity, while the soil moisture sensor reads soil moisture levels.
3. **Data Transmission**: The system sends sensor data to ThingSpeak.
4. **Pump Control**: The water pump is activated if the soil moisture level drops below 50%. The pump runs for 3 seconds to water the soil.

## Conclusion

This IoT-based smart irrigation system ensures optimal soil moisture levels by automating watering using sensor data. The system transmits data to ThingSpeak for monitoring and control, making it an efficient solution for modern smart farming.
