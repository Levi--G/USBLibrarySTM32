#include "USBLibrarySTM32.h"
#include <Arduino.h>

USBCDC USBSerial;

byte CreateConfig()
{
  byte config = 0;
  switch (USBSerial.numbits())
  {
  case 6:
    config |= 0x02;
    break;
  case 7:
    config |= 0x04;
    break;
  default: // 8
    config |= 0x06;
    break;
  }
  switch (USBSerial.paritytype())
  {
  case USBSerial.ODD_PARITY:
    config |= 0x30;
    break;
  case USBSerial.EVEN_PARITY:
    config |= 0x20;
    break;
  // not defined?
  //  case USBSerial.MARK_PARITY:
  // config |= 0x04;
  // break;
  // case USBSerial.SPACE_PARITY:
  // config |= 0x04;
  // break;
  default: // USBSerial.NO_PARITY
    break;
  }
  switch (USBSerial.stopbits())
  {
    // not defined?
  // case USBSerial.ONE_AND_HALF_STOP_BIT:
  //   config |= 0x04;
  //   break;
  case USBSerial.TWO_STOP_BITS:
    config |= 0x08;
    break;
  default: // USBSerial.ONE_STOP_BIT
    break;
  }
  return config;
}

uint8_t buffer[64];

void setup()
{
  pinMode(PC13, OUTPUT);
  USBSerial.begin();
  USB_Begin();
  while (!USB_Running())
  {
    // wait until usb connected
    delay(5);
  }
  while (!USBSerial)
  {
    // wait until Serial port is connected
    delay(5);
  }
  Serial.begin(USBSerial.baud(), CreateConfig());
}

int SerialAvailable = 0;
int CDCAvailable = 0;

void loop()
{
  if ((SerialAvailable = Serial.available()))
  {
    digitalWriteFast(PinName::PC_13, 0);
    if (USBSerial.write(buffer, Serial.readBytes(buffer, SerialAvailable)) != SerialAvailable)
    {
      // Can't keep up
    }
    digitalWriteFast(PinName::PC_13, 1);
  }
  if ((CDCAvailable = USBSerial.available()))
  {
    digitalWriteFast(PinName::PC_13, 0);
    if (Serial.write(buffer, USBSerial.readBytes(buffer, CDCAvailable)) != CDCAvailable)
    {
      // Can't keep up
    }
    digitalWriteFast(PinName::PC_13, 1);
  }
}