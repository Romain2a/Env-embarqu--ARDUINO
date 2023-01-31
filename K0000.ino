#define SECRET_SSID "Livebox-8810"
#define SECRET_PASS "NrPMwZrG6Z27V9wqYF"
#include <ArduinoMqttClient.h>
#include <WiFiNINA.h>

char ssid[] = SECRET_SSID;        // your network SSID
char pass[] = SECRET_PASS;    // your network password

WiFiClient wifiClient;
MqttClient mqttClient(wifiClient);

const char broker[] = "test.mosquitto.org";
int        port     = 1883;
const char topic[]  = {"x"};
const char topic2[]  = {"y"};
const char topic3[]  = {"vitesse"};

int moteur_G = 4;
int moteur_D = 3;
int incomingData;

void setup() {
  pinMode(moteur_D, OUTPUT);
  pinMode(moteur_G, OUTPUT);
  Serial.begin(9600);

  while (!Serial) {
    ; // wait for serial port to connect. Needed for native USB port only
  }
  // attempt to connect to Wifi network:
  Serial.print("Attempting to connect to SSID: ");
  Serial.println(ssid);
  while (WiFi.begin(ssid, pass) != WL_CONNECTED) {
    // failed, retry
    Serial.print(".");
    delay(5000);
  }

  Serial.println("You're connected to the network");
  Serial.println();

  Serial.print("Attempting to connect to the MQTT broker: ");
  Serial.println(broker);

  if (!mqttClient.connect(broker, port)) {
    Serial.print("MQTT connection failed! Error code = ");
    Serial.println(mqttClient.connectError());

    while (1);
  }

  Serial.println("You're connected to the MQTT broker!");
  Serial.println();

  // set the message receive callback
  mqttClient.onMessage(onMqttMessage);

  Serial.print("Subscribing to topic: ");
  Serial.println(topic);
  Serial.println();

  // subscribe to a topic
  mqttClient.subscribe(topic);
  mqttClient.subscribe(topic2);
  mqttClient.subscribe(topic3);

  // topics can be unsubscribed using:
  // mqttClient.unsubscribe(topic);

  Serial.print("Topic: ");
  Serial.println(topic);
  Serial.print("Topic: ");
  Serial.println(topic2);
  Serial.print("Topic: ");
  Serial.println(topic3);

  Serial.println();
}

void stop() {
  digitalWrite(moteur_D,LOW);
  digitalWrite(moteur_G,LOW);
}
void start() {
  digitalWrite(moteur_D,HIGH);
  digitalWrite(moteur_G,HIGH);
}

void deplacement() {
  incomingData = mqttClient.available();
  while (mqttClient.available()) {
    Serial.println("data :" + incomingData);
    if (incomingData >= 600) {
      // avance
      digitalWrite(moteur_D,HIGH);
      digitalWrite(moteur_G,HIGH);  
    } else {
        stop();
    } 
    if (incomingData >= -1023 && incomingData <= -550) {
      // tourne à gauche
      digitalWrite(moteur_G, LOW);
      digitalWrite(moteur_D, HIGH);
    } else {
        stop();
    }
    if (incomingData >= -449 && incomingData <= -256) {
      // tourne à Droite
      digitalWrite(moteur_G, HIGH);
      digitalWrite(moteur_D, LOW);
    } else {
        stop();
    }
  } 
}

void loop() {
  // put your main code here, to run repeatedly:
  mqttClient.poll();
  deplacement();
  // start();
}

void onMqttMessage(int messageSize) {
  // we received a message, print out the topic and contents
  Serial.println("Received a message with topic '");
  Serial.print(mqttClient.messageTopic());
  Serial.print("', length ");
  Serial.print(messageSize);
  Serial.println(" bytes:");

  // use the Stream interface to print the contents
  while (mqttClient.available()) {
    // float x = mqttClient.read();
    // Serial.println(x);
    // delay(1000);
    // char x = mqttClient.read()
    Serial.print((char)mqttClient.read());
  }
  Serial.println();
  Serial.println();
}
