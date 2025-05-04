# USBLibrary for stm32

[![PlatformIO Registry](https://badges.registry.platformio.org/packages/levi--g/library/USBLibrarySTM32.svg)](https://registry.platformio.org/libraries/levi--g/USBLibrarySTM32)

This library adds a very basic implementation of the original AVR Arduino USB API to make existing libraries compatible with stm32 boards. Caution, this library is only tested on a limited number of boards and libraries, please create an issue on the project github if you encounter issues!

STM32F103 is currently stable. STM32F4xx support is being worked on and TX(IN) data is already possible, this means while serial might not work mouse, keyboard, joystick and standard HID reports do work. The goal is to eventually get all 5 usb implementations of STM32 working and to merge it back into the main STM32duino core.

## Features

At this moment the library has the following features:
- USBAPI implementation
- PluggableUSB implementation (with added runtime plugging)
- HID library implementation (with small fix on sending)

What can you do with this?
- Use a (or multiple) Virtual serial port(s) over USB together with all the other connections => usbcomposite can't do this!
- Use the original Keyboard and Mouse libraries from Arduino
- Use a Joystick library like [ArduinoJoystickLibrary](https://github.com/MHeironimus/ArduinoJoystickLibrary) (my favorite)
- Use the MIDIUSB library to act as MIDI device
- Use any other HID/PUSB library if they are compatible

What can't i do with this/what is different?
- I havent had time to test all libraries except the ones mentioned above, if you want to see a library tested let me know.
- You need to call an extra method in Setup for now, until i find a proper solution. e.g. `USB_Begin();`
- Non STM32F103 chips are not supported *yet*, due to issues with the underlying stm code of stm32duino, this will be the focus of the next revision

## Testing/supported

I currently test these libraries:
- A custom pluggable SerialUSB library for stm32
- Mouse and Keyboard libraries made by Arduino
- [ArduinoJoystickLibrary](https://github.com/MHeironimus/ArduinoJoystickLibrary)
- [MIDIUSB](https://github.com/arduino-libraries/MIDIUSB) (needs extra build flag, might not be compatible with other libraries => look at the sample)

On these boards:
- STM32F103C8 Blue pill
- STM32F103C8 Black pill (clone)
- CH32F103 Blue pill (chinese clone)
- STM32F103RE generic
- STM32F401 TX only
- STM32F411 TX only

## Installation Instructions per project

- Include the library via [platformio](https://registry.platformio.org/libraries/levi--g/USBLibrarySTM32) or on Arduino IDE via add library zip
- (only on Arduino IDE) Add `-D USBCON` and `-D HAL_PCD_MODULE_ENABLED` to your build flags
- Add Keyboard/Mouse/Serial/joystick... instances
- Call `USB_Begin();` in your Setup
- Optionally also call `while (!USB_Running()){ delay(50); }` in your setup if you want to wait until the device actually has an usb connection, otherwise make sure to check `USB_Running()` before calling any usb methods!
- Call begin() on your instances after being connected
- Do **NOT** include the builtin usb-composite Keyboard or mouse libraries, if you want to use mouse or keyboard include the [Arduino libraries](https://registry.platformio.org/libraries/arduino-libraries/Mouse) instead

## Examples

Examples can also be copied from the examples folder on github

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

	//Initialise USB !IMPORTANT!
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

## Why is this not in the core yet?

This is still code in testing, and while it works well on the tested libraries i want to improve the resource usage some more before this could be a valid alternative for the existing functionality, if you want to help me do that, help is very much welcome.

## Why does my USB_Begin() return false and i have no usb connection

USB_Begin will only return false if an error occured, such as for instance if not enough ram was available to initialise the usb, due to differences in the stm and arduino usb api we need to use buffers to buffer the data. With default settings and 2 endpoints in use for serial for instance this can be as much as 1536 bytes for the buffers alone. If you need larger/smaller buffers you can easily do that with compiler flags. Check out the USBOptions.h file for more info.

## Can i use x on board y?

STM32 uses 5 different usb types in their products, depending on series and number, you can check those out here: https://www.st.com/resource/en/application_note/dm00296349-usb-hardware-and-pcb-guidelines-using-stm32-mcus-stmicroelectronics.pdf

In theory all stm32 chips using a "USB" type are supported and stable, but only the ones mentioned above are actually tested. The OTG_HS type is now being added and can send data. since i don't have chips with OTG_FS type i can not test these. If you encounter problems feel free to create an issue to find out what goes wrong.