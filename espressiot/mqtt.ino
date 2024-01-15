// ESPressIoT Controller for Espresso Machines
// 2016-2021 by Roman Schmitz
//
// MQTT integration

#ifdef ENABLE_MQTT

#include <WiFiClient.h>
#include <PubSubClient.h>

WiFiClient espClient;
PubSubClient client(espClient);

const char* mqttConfigTopic = MQTT_TOPIC "/config/#";
const char* mqttStatusTopic = MQTT_TOPIC "/status";

void MQTT_reconnect() {
  // Retry connection to MQTT broker
  for (int i = 0; i < MAX_CONNECTION_RETRIES && !client.connected(); i++) {
    Serial.print("Attempting MQTT connection...");
    
    String clientId = "ESP8266Client";
    clientId += String(random(0xffff), HEX);
    
    if (client.connect(clientId.c_str(), MQTT_USER, MQTT_PASS)) {
      Serial.println("connected");
      client.subscribe(mqttConfigTopic, 1);
    } else {
      Serial.print("failed, rc=");
      Serial.print(client.state());
    }
  }
}

void MQTT_callback(char* topic, byte* payload, unsigned int length) {
  // Process incoming MQTT messages
  Serial.print("Message arrived [");
  Serial.print(topic);
  Serial.print("] '");

  String msg = "";
  for (int i = 0; i < length; i++) {
    Serial.print((char)payload[i]);
    msg += (char)payload[i];
  }
  Serial.println("'");

  double val = msg.toFloat();
  Serial.println(val);

  // Handle specific topics
  if (strstr(topic, "/config/tset")) {
    if (val > 1e-3) {
      gTargetTemp = val;
    }
  } else if (strstr(topic, "/config/toggle")) {
    poweroffMode = !poweroffMode;
  }
}

void setupMQTT() {
  // Initialize MQTT client and set callback function
  client.setServer(MQTT_HOST, MQTT_PORT);
  client.setCallback(MQTT_callback);
}

void loopMQTT() {
  // Ensure MQTT connection is maintained
  MQTT_reconnect();
  
  // Process MQTT messages
  client.loop();
  
  // Publish status to MQTT
  client.publish(mqttStatusTopic, gStatusAsJson.c_str());
}

#endif // ENABLE_MQTT
