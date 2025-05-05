#include <Arduino.h>
#include "SerialUSB.h"

SerialUSB_ SerialUSB;

void setup()
{
  SerialUSB.begin(115200);
  USB_Begin();
  while (!USB_Running())
  {
    // wait until usb connected
    delay(5);
  }
  while (!SerialUSB)
  {
    //(optional)wait until Serial port is connected
    delay(5);
  }
}

void loop()
{
  if (SerialUSB.available())
  {
    SerialUSB.print("Echo: ");
    SerialUSB.println((char)SerialUSB.read());
  }
}