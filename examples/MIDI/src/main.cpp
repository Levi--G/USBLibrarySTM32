#include <Arduino.h>
#include "USBAPI.h"
#include "PluggableUSB.h"
#include "MIDIUSB.h"

int ppqn = 0;

void setup()
{
  MidiUSB.available();
  USB_Begin();
  while (!USB_Running()){
    //wait until usb connected
    delay(50);
  }
}

void noteOn(byte channel, byte pitch, byte velocity) {
  midiEventPacket_t noteOn = {0x09, 0x90 | channel, pitch, velocity};
  MidiUSB.sendMIDI(noteOn);
}

void noteOff(byte channel, byte pitch, byte velocity) {
  midiEventPacket_t noteOff = {0x08, 0x80 | channel, pitch, velocity};
  MidiUSB.sendMIDI(noteOff);
}

void loop()
{
  midiEventPacket_t rx;
  
  do {
    rx = MidiUSB.read();

    //Count pulses and send note 
    if(rx.byte1 == 0xF8){
       ++ppqn;
       
       if(ppqn == 24){
          noteOn(1,50,127);
          MidiUSB.flush();      
          ppqn = 0;
       };
    }
    //Clock start byte
    else if(rx.byte1 == 0xFA){
      noteOn(1,50,127);
      MidiUSB.flush();
      ppqn = 0;
    }
    //Clock stop byte
    else if(rx.byte1 == 0xFC){
      noteOff(1,50,0);
      MidiUSB.flush();
      ppqn = 0;
    };
    
  } while (rx.header != 0);
  
}