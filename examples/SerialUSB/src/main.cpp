#include <Arduino.h>
#include "SerialUSB.h"

SerialUSB_ SerialUSB;

void setup()
{
  SerialUSB.begin(115200);
  USB_Begin();
  while (!USB_Running()){
    //wait until usb connected
    delay(50);
  }
  while (!SerialUSB){
    //wait until Serial port is connected
    delay(50);
  }
}

void loop()
{
  if (SerialUSB.available()){
    SerialUSB.print("Echo: ");
    SerialUSB.println(SerialUSB.readString());
  }
}