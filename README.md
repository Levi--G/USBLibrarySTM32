# USBLibrary for stm32

Version 0.1.0

This library adds a very basic implementation of the original AVR Arduino USB API to make existing libraries compatible with stm32 boards. Caution, this library is very beta and most likely not suited for production code unless you make sure to test it thoroughly!

## Features

At this moment the library has the following features:
- USBAPI implementation (sending only)
- PluggableUSB implementation (with added runtime plugging)
- HID library implementation (with small fix on sending)

What can you do with this?
- Use the original Keyboard and Mouse libraries from Arduino
- Use a Joystick library like [ArduinoJoystickLibrary](https://github.com/MHeironimus/ArduinoJoystickLibrary) (my favorite)
- Use any other HID/PUSB library if they are compatible

What can't i do with this/what is different?
- I havent had time to test all libraries, nor implement receiving usb data, this will be done at a later time.
- You can't use Serial together with HID yet, this was already the case with the old composite library so nothing changed there, once i get a PluggableSerial working when i implemented receiving data you will be able to use Serial again.
- You need to call an extra method in Setup for now, until i find a proper solution. e.g. `USB_Begin();`

## Testing

I currently test these libraries:
- Mouse and Keyboard libraries made by Arduino
- [ArduinoJoystickLibrary](https://github.com/MHeironimus/ArduinoJoystickLibrary)

On these boards:
- STM32F103 Blue pill
- STM32F103 Black pill (clone)

## Installation Instructions

Todo ;)

## Examples

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

More examples can be found in the example folder.