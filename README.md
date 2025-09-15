# USBLibrary for stm32

[![PlatformIO Registry](https://badges.registry.platformio.org/packages/levi--g/library/USBLibrarySTM32.svg)](https://registry.platformio.org/libraries/levi--g/USBLibrarySTM32)

This library adds a complete implementation of the original AVR Arduino USB API to make existing libraries compatible with stm32 boards. This library is only tested on a limited set of boards and libraries, please create an issue on the project github if you encounter issues!

USBLibrary is currently stable on all supported targets and should be compatible with all board using the "USB" and "USB_OTG_FS" usb pheriperals. The goal is to eventually get all 5 usb implementations of STM32 working and to merge it back into the main STM32duino core.

## Features

At this moment the library has the following features:
- USBAPI implementation (identical to and extended from the AVR api)
- PluggableUSB implementation (with added runtime plugging and single root plug support)
- HID library implementation

What can you do with this?
- Use any AVR USB compatible library
- Use a (or multiple) Virtual serial port(s) over USB together with all the other connections => usbcomposite can't do this!
- Use the original Keyboard and Mouse libraries from Arduino
- Use a Joystick library like [ArduinoJoystickLibrary](https://github.com/MHeironimus/ArduinoJoystickLibrary) (my favorite)
- Use the MIDIUSB library to act as MIDI device
- Use any other HID/PUSB library if they are compatible

What can't i do with this/what is different?
- I havent had time to test all libraries except the ones mentioned above, if you want to see a library tested let me know.
- You need to call an extra method in Setup, or at some other point. e.g. `USB_Begin();` this will attach the usb and start communication, before calling this no communication can take place.
- "USB_OTG_HS" and other usb chips are not supported *yet*, due to me not owning any chips for testing, if anyone wants to send me one feel free to contact me.

## Testing/supported

I currently test these libraries:
- A custom pluggable USBCDC USBSerial library for stm32
- Mouse and Keyboard libraries made by Arduino
- [ArduinoJoystickLibrary](https://github.com/MHeironimus/ArduinoJoystickLibrary)
- [MIDIUSB](https://github.com/arduino-libraries/MIDIUSB) (needs extra build flag, might not be compatible with other libraries => look at the sample)

On these boards:
- STM32F103C8 Blue pill
- STM32F103C8 Black pill (clone)
- CH32F103 Blue pill (chinese clone)
- STM32F103RE generic
- STM32F401 (1.0.0+)
- STM32F411 (1.0.0+)

## Installation Instructions

Installation was moved to the wiki: [Getting started](https://github.com/Levi--G/USBLibrarySTM32/wiki/GettingStarted)

## Examples

Examples can also be copied from the [examples folder on github](https://github.com/Levi--G/USBLibrarySTM32/tree/develop/examples)

### Simple example

Example with [ArduinoJoystickLibrary](https://github.com/MHeironimus/ArduinoJoystickLibrary)

```C++
#include <Joystick.h>

// Create the Joystick
Joystick_ Joystick;

// Constant that maps the physical pin to the joystick button.
const int pinToButtonMap = 9;

void setup() {
	// Initialize Button Pins
	pinMode(pinToButtonMap, INPUT_PULLUP);

	//Initialize USB !IMPORTANT!
	USB_Begin();

	// Initialize Joystick Library
	Joystick.begin();
}

// Last state of the button
int lastButtonState = 0;

void loop() {

	// Read pin values
	int currentButtonState = !digitalRead(pinToButtonMap);
	if (currentButtonState != lastButtonState)
	{
		Joystick.setButton(0, currentButtonState);
		lastButtonState = currentButtonState;
	}

	delay(50);
}
```

## FAQ

The FAQ was moved to the wiki: [FAQ](https://github.com/Levi--G/USBLibrarySTM32/wiki/FAQ)