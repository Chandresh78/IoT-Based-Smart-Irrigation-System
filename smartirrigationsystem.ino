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
