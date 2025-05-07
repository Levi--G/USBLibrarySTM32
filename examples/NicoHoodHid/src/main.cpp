#include <Arduino.h>
#include "HID-Project.h"

const int pinLed = LED_BUILTIN;
const int pinButton = PB9;

int analogvalue = 0;

void setup()
{
  pinMode(pinLed, OUTPUT);
  pinMode(pinButton, INPUT_PULLUP);
  BootKeyboard.begin();
  USB_Begin();
}

void loop()
{
  if (BootKeyboard.getLeds() & LED_CAPS_LOCK)
    digitalWrite(pinLed, HIGH);
  else
    digitalWrite(pinLed, LOW);

  // Trigger caps lock manually via button
  if (!digitalRead(pinButton))
  {
    BootKeyboard.write(KEY_CAPS_LOCK);

    // Simple debounce
    delay(300);
  }
}