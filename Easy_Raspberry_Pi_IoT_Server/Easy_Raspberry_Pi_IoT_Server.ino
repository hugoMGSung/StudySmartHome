// 아두이노에서 MQTT로 서버 통신되는 소스!
#include <ArduinoJson.h>
#include <ArduinoJson.hpp>
#include <WiFiS3.h>
#include <PubSubClient.h>
#include <Wire.h>
// #include <DHT.h> // Replace "Adafruit_BME680.h" with "DHT.h"

// #define DHTPIN 8
// #define DHTTYPE DHT11
// DHT dht(DHTPIN, DHTTYPE);

long randNumber;

const char* ssid = "SK_WiFiGIGA3BA0_2.4G";
const char* password = "KAP23@0405";
const char* mqtt_server = "192.168.45.190"; //Ip of your raspberry pi

WiFiClient espClient;
PubSubClient client(espClient);
unsigned long lastMsg = 0;
#define MSG_BUFFER_SIZE (50)
char msg[MSG_BUFFER_SIZE];
int value = 0;

void setup_wifi() {
  delay(100);
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
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());
}

void callback(char* topic, byte* payload, unsigned int length) {
  // Handle incoming MQTT messages if needed
}

void reconnect() {
  while (!client.connected()) {
    Serial.print("Attempting MQTT connection...");
    String clientId = "ESP8266Client-";
    clientId += String(random(0xffff), HEX);
    if (client.connect(clientId.c_str())) {
      Serial.println("connected");
      client.publish("outTopic", "hello world");
      client.subscribe("inTopic");
    } else {
      Serial.print("failed, rc=");
      Serial.print(client.state());
      Serial.println(" try again in 5 seconds");
      delay(5000);
    }
  }
}

void setup() {
  Serial.begin(9600);
  while (!Serial);
  Serial.println(F("DHT11 test"));

  //dht.begin();

  setup_wifi();
  client.setServer(mqtt_server, 1883);
  client.setCallback(callback);
}

void loop() {
  if (!client.connected()) {
    reconnect();
  }
  client.loop();
  StaticJsonDocument<32> doc;
  char output[55];

  long now = millis();
  if (now - lastMsg > 5000) {
    lastMsg = now;

    //float temp = 23.0f; //dht.readTemperature();
    //float humidity = 60.0f; //dht.readHumidity();
    long temp = random(20, 30);
    doc["t"] = temp;
    long humidity = random(50, 80);
    doc["h"] = humidity;

    serializeJson(doc, output);
    client.publish("/home/sensors", output);
  }
}
