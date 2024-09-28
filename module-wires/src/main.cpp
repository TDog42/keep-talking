#include <Arduino.h>
#include <chat.h>
#include <serial-number-flags.h>
#include "sensor_group.h"
#include "wire.h"

#define STATUS_GND 11
#define STATUS_RED 2
#define STATUS_GRN 3

bool disarmed = false;
bool detonated = false;
bool won = false;
bool last_digit_odd = false;

SensorGroup sensors;

Chat chat(ChatSource::Wires);
ChatMessage msg;


void setup()
{
  chat.begin();

  pinMode(STATUS_GND, OUTPUT);
  pinMode(STATUS_RED, OUTPUT);
  pinMode(STATUS_GRN, OUTPUT);
  
  digitalWrite(STATUS_GND, LOW);
  digitalWrite(STATUS_RED, HIGH);
  digitalWrite(STATUS_GRN, LOW);

  sensors.init(true);
}

void loop()
{
  if (chat.receive(&msg))
  {
    switch (msg.message)
    {
      case MessageType::Reset:
        sensors.init(msg.data & SN_LAST_DIGIT_ODD);
        detonated = false;
        disarmed = false;
        won = false;

        digitalWrite(STATUS_RED, HIGH);
        digitalWrite(STATUS_GRN, LOW);
        break;
      case MessageType::Detonate:
        detonated = true;
        break;
      case MessageType::Win:
        won = true;
        break;
    }
  }

  if (!disarmed && !detonated && !won)
  {
    SensorGroupEvent event = sensors.update();

    if (event == SensorGroupEvent::Disarmed)
    {
      disarmed = true;
      
      digitalWrite(STATUS_RED, LOW);
      digitalWrite(STATUS_GRN, HIGH);
      
      chat.send(MessageType::Disarm);
    }
    else if (event == SensorGroupEvent::Striken)
    {
      chat.send(MessageType::Strike);
    }
  }
}

