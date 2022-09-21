# USBLibrary for stm32

[![PlatformIO Registry](https://badges.registry.platformio.org/packages/levi--g/library/USBLibrarySTM32.svg)](https://registry.platformio.org/libraries/levi--g/USBLibrarySTM32)

This library adds a very basic implementation of the original AVR Arduino USB API to make existing libraries compatible with stm32 boards. Caution, this library is very beta and most likely not suited for production code unless you make sure to test it thoroughly!

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
- STM32F103 Blue pill
- STM32F103 Black pill (clone)
- CH32F103 Blue pill (chinese clone)

## Installation Instructions per project

- Include the library via [platformio](https://registry.platformio.org/libraries/levi--g/USBLibrarySTM32) or Arduino IDE
- Add `-D USBCON` and `-D HAL_PCD_MODULE_ENABLED` to your build flags
- Initialise Keyboard/Mouse/Serial
- Call `USB_Begin();` in your Setup
- Optionally also call `while (!USB_Running()){ delay(50); }` in your setup if you want to wait until the device actually has an usb connection, otherwise make sure to check `USB_Running()` before calling any usb methods!
- Initialise joystick library
- Do **NOT** include the builtin usb-composite Keyboard or mouse libraries, if you want to use mouse or keyboard include the [Arduino libraries](https://registry.platformio.org/libraries/arduino-libraries/Mouse) instead

## Installation in core one-time (mandatory)

Warning, this is needed to use this library but will change the core to not use the builtin usb library anymore, this means any existing projects cant use usbcomposite until you uninstall again

- Go to `%USERPROFILE%\.platformio\packages\framework-arduinoststm32\cores\arduino\stm32\usb`
- Delete all files in the folder
- Download [usbd_conf.h](https://github.com/Levi--G/USBLibrarySTM32/raw/main/include/usbd_conf.h), [usbd_desc.h](https://github.com/Levi--G/USBLibrarySTM32/raw/main/include/usbd_desc.h), [usbd_ep_conf.h](https://github.com/Levi--G/USBLibrarySTM32/raw/main/include/usbd_ep_conf.h) and [usbd_if.h](https://github.com/Levi--G/USBLibrarySTM32/raw/main/include/usbd_if.h)
- Place the 4 files in the usb directory

## Uninstallation from core

To revert the previous changes just delete the entire `%USERPROFILE%\.platformio\packages\framework-arduinoststm32` folder, platformio will download a new one and it will ensure everything is back to default.

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