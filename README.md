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

## Installation Instructions per project

- Include the library via [platformio](https://registry.platformio.org/libraries/levi--g/USBLibrarySTM32) or on Arduino IDE via "add library zip" (broken, waiting for a fix from arduino, go upvote this: https://github.com/arduino/arduino-cli/issues/2908)
- Add `-D USBCON` and `-D HAL_PCD_MODULE_ENABLED` to your build flags in platformio.ini (check the samples)
- Add Keyboard/Mouse/Serial/joystick... instances
- Call `USB_Begin();` in your Setup
- Optionally also call `while (!USB_Running()){ delay(5); }` in your setup if you want to wait until the device actually has an usb connection, caution: if you don't connect usb, this will never exit the loop.
- Call begin() on your library instances (mouse, joystick) after being connected
- Do **NOT** include the builtin stm32 usb-composite Keyboard or mouse libraries, if you want to use mouse or keyboard include the [Arduino libraries](https://registry.platformio.org/libraries/arduino-libraries/Mouse) instead

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

## Why is this not in the core yet?

While the library works well on the tested libraries i want to improve the resource usage and compatibility some more before this could be a valid alternative for the existing functionality, if you want to help me do that, help is very much welcome. This also ensures updates can be done more often if needed.

## Why does my USB_Begin() return false and i have no usb connection

USB_Begin will only return false if an error occured, such as for instance if not enough ram was available to initialize the usb or your device does not have enough endpoints for the used libraries. Due to differences in the stm and arduino usb api we need to use buffers to buffer the data and allocate endpoints at runtime. With default settings and 2 endpoints in use for serial for instance this can be as much as 1536 bytes for the buffers alone. If you need larger/smaller buffers you can easily do that with compiler flags. Check out the USBOptions.h file for more info.

## Can i use x on board y?

STM32 uses 5 different usb types in their products, depending on series and number, you can check those out here: https://www.st.com/resource/en/application_note/dm00296349-usb-hardware-and-pcb-guidelines-using-stm32-mcus-stmicroelectronics.pdf

In theory all stm32 chips using a "USB" or "USB_OTG_FS" type are supported and stable, but only the ones mentioned above are actually tested. Since i don't have chips with USB_OTG_HS type i can not test these. If you encounter problems feel free to create an issue to find out what goes wrong.

## What performance can i expect?

While the builtin libraries are more perfomance optimised due to the api difference, you can easily expect 500kb/s over USBCDC which is 1/3rd of the maximum speed of the usb connection and more than 4x the speed of a traditional arduino uno R3 at 115200 baud. You can use custom hid and endpoint sizes to optimise this further.

## ADVANCED: How do i use less RAM? / Why do you use so much RAM?

We use ram buffers due to the stm implementation doing everything in interrupts which is incompatible with the AVR API and to provide better compatibility with all libraries.

How to reduce ram:
1) Important! Get you sketch working with the default settings (otherwise you can't properly test the different changes)
2) Check what the maximum EP size is your library needs, for instance Mouse needs only 5, Joystick depends on the axis and buttons, if this is lower than 64 you can set `-D USB_EP_SIZE=5` or some other value to reduce size a lot.
3) Try adding `-D PACKETBUFFER_USE_TX_BUFFERS=false` and seeing if you sketch still works, if it doesn't or it seems slower, revert the change
4) If you still need more ram you can set `-D PACKETBUFFER_COUNT=2` this might reduce performance or your controller might skip data. `-D PACKETBUFFER_ALLOW_OVERWRITE=false` might help with skipped data, but might also just cause no data to be transferred.

Compiling does not include the RAM used by USB, this is allocated dynamically upon connecting!