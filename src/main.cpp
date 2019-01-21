#include <Arduino.h>
#include <PubSubClient.h>

#include "ESPBASE.h"

const byte mqttDebug = 1;
//const int ESP_BUILTIN_LED = 1;
#define buttonPin 0
bool buttonstate = false;
bool lightson = false;
String StatusTopic;
void toggle();

ESPBASE Esp;

void setup() {
  Serial.begin(115200);
  Esp.initialize();

  customWatchdog = millis();
  StatusTopic = String(DEVICE_TYPE) + "/" + config.DeviceName + "/status";  
  mqttSubscribe();
//  pinMode(ESP_BUILTIN_LED, OUTPUT);
  pinMode(buttonPin,INPUT);
  attachInterrupt(0,toggle,FALLING);
  Serial.println("Done with setup");
}

void loop() {
  static bool oldButtonState = false;
  Esp.loop();
  if(oldButtonState != buttonstate)
  {
    if(lightson)
    {
      Esp.mqttSend(config.ToggleTopic,"OFF","");
      Serial.println("off message sent");
      lightson = false;
    }
    else
    {
      Esp.mqttSend(config.ToggleTopic,"ON","");
      Serial.println("on message sent");
      lightson = true;
    }
    oldButtonState = buttonstate;
  }
}

void mqttSubscribe()
{
      if (Esp.mqttClient->connected()) {
        if (Esp.mqttClient->subscribe("SendStat")) {
          Serial.println("Subscribed to " + config.StatusTopic);
          Esp.mqttSend(StatusTopic,verstr,","+Esp.MyIP()+" start");
      }
    }
}

void mqttCallback(char* topic, byte* payload, unsigned int length) {
  char c_payload[length];
  memcpy(c_payload, payload, length);
  c_payload[length] = '\0';
  
  String s_topic = String(topic);
  String s_payload = String(c_payload);
  
  if (s_topic == StatusTopic) {
    Serial.println("Got Status: ");
    Serial.println(s_payload);
    Serial.println("This is for me");
//      mqttSend(config.StatusTopic,formatConfig());
    Esp.mqttSend(config.ToggleTopic,config.DeviceName+" ",formatConfig());
  }
  else {
    if (mqttDebug) { Serial.println(" [unknown message]"); }
  }
}

void toggle() 
{
  static unsigned long last_interrupt_time = 0;
  unsigned long interrupt_time = millis();
  if (interrupt_time - last_interrupt_time > 1000) 
  {
    buttonstate = !buttonstate;
    last_interrupt_time = interrupt_time;
  }
}

