#include <Arduino.h>
#include "SerialUSB.h"

SerialUSB_ SerialUSB;
int available = 0;
size_t bytes = 0;
byte buffer[64];

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
    // wait until Serial port is connected
    delay(5);
  }
}

void loop()
{
  available = SerialUSB.available();
  if (available)
  {
    bytes = SerialUSB.readBytes((char *)buffer, available);
    SerialUSB.write(buffer, bytes);
  }
}