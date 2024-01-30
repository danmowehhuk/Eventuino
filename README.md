# Eventuino

Eventuino is a C++ library for Arduino that makes it way easier for your code
to handle input devices like buttons, switches, potentiometers, and more.

If you're using a single button to turn an LED on and off, then Eventuino is
probably overkill. But as you add buttons and switches to your project, you 
will quickly find that nesting `if`/`else if`/`else` statements in your
`loop()` function gets messy and unmaintainable. This is especially true when
some buttons change the behavior of other buttons (think the "shift" and
"control" keys on your keyboard). And don't forget about debouncing/denoising!

Eventuino takes care of all the polling and debouncing, and lets you define 
your behaviors as function callbacks for _events_ such as `onFlip(uint8_t)`, 
`onPressed(uint8_t)`, `onLongPressed(uint8_t)`, `onReleased(uint8_t)`, etc. 
You can even change the function callbacks on the fly!

Eventuino can also be extended to support your other digital or analog input
devices; say, a thermocoupler connected to the base of a transistor forming
a "temperature switch."

Written by Dan Mowehhuk (danmowehhuk@gmail.com)\
BSD license, check license.txt for more information\
All text above must be included in any redistribution

## Quick Start

Try the following sketch by connecting a button between pin 5 and GND on an
Arduino, then open the Serial Monitor to see the results.

```cpp
#include <Eventuino.h>
#include <eventuino/Button.h>

using namespace eventuino;

#define BUTTON_PIN 5
#define BUTTON_VALUE 1

Button button(BUTTON_PIN, BUTTON_VALUE); 
Eventuino evt;
EventSource* eventSources[] = { &button };

void buttonPressed(uint8_t value) {
  Serial.print("Button pressed with value=");
  Serial.println(value);
}

void setup() {
  Serial.begin(9600);
  while (!Serial);

  button.onPressed=buttonPressed;

  evt.setEventSources(eventSources, 1);
  evt.begin();
}

void loop() {
  evt.poll();
}

```

Notice the `using namespace eventuino;` statement after the `#include`s. Class 
names like `Button` and `Toggle` are fairly common across Arduino libraries, 
so Eventuino uses a namespace to avoid naming conflicts. However, this means 
you need to choose a different name for your `Eventuino` instance (e.g. `evt`
in the example above).

First, instantiate your event sources (`Button`s, `Toggle`s, etc.) passing in
their pin numbers and a value of your choosing. This value will be passed to
your event callback function. You can use the same callback function for
multiple event sources, so you could use the `value` to identify which button
was pressed, for example.

Next, instantiate `Eventuino` and an `EventSource` array containing references
to all the event sources you created (note the `&` before each array value).

In your `setup()` function, call `setEventSources` and pass in the event source
array and the number of elements in it. Then, call Eventuino's `begin()` 
method. Eventuino will take care of initializing all your pins, so no need to 
worry about `pinMode`s here.

Lastly, place a call to Eventuino's `poll()` method in the `loop()` function.

In this example, the `onPressed` callback is set in the `setup()` function, 
but this can be done anywhere in your code, and even changed on the fly.
Setting a new callback function (or unsetting one) takes effect immediately.

### Handling Events

Create a function (not a class method) that returns `void` and takes a 
single `uint8_t` parameter. For example:
```c
void myHandler(uint8_t value) { // Do something... }
```

Assign your function to one of the available callbacks. For example, a button
has an `onPressed` callback:
```c
myButton.onPressed = myHandler;
```

You can unset a handler, too:
```
myButton.onPressed = 0;
```

The event sources provided with Eventuino have the following callbacks:

| Event Source | Callback | Event Trigger |
| ------------ | -------- | ------- |
| [Button](src/eventuino/Button.h) | onPressed | When the button switches from unpressed to pressed |
| [Button](src/eventuino/Button.h) | onReleased | When the button switches from pressed to unpressed |
| [Button](src/eventuino/Button.h) | onLongPressed | When the button has remained pressed for more than some delay. May optionally repeat (disabled by default, see `enableRepeat`) |
| [Toggle](src/eventuino/Toggle.h) | onFlip | When the switch changes state in either direction |
| [Toggle](src/eventuino/Toggle.h) | onActivate | When the switch changes to the activated state |
| [Toggle](src/eventuino/Toggle.h) | onDeactivate | When the switch changes to the deactivated state |

### Debounce, Long Hold and Repeat delays

Eventuino has default delays for debouncing (75ms), long holds (1s) and repeats (200ms), but these can be changed.
Note, however, that these changes apply to all the event sources. The `DigitalPinSource` class provides
the following static methods:
- `DigitalPinSource::setDebounceDelayMs(uint8_t)`
- `DigitalPinSource::setLongHoldDelayMs(uint16_t)`
- `DigitalPinSource::setRepeatMs(uint8_t)`

The 75ms debounce delay balances effectiveness with responsiveness. Depending on your
hardware, you may be able to reduce this delay.

### Analog Inputs

_Coming soon..._


## Using the Callback Constructors

If you are using a port expander such as the MCP23017, you will need to provide 
callback functions to certain constructors telling the instance how to perform a
'pinMode' and 'digitalRead'.

See the [callback example](examples/button_with_callbacks/button_with_callbacks.ino)
for more details.


# Extending Eventuino

Say you have a thing that connects to a digital pin as an input. You could poll it
from the `loop()` function, but if you want to take the event handler approach, you
can create your own Eventuino event source!

Just extend the `eventuino/DigitalPinSource` class! Your class must implement its 
own `onChange(uint8_t)` method, and may optionally implement `onLongHold(uint8_t)`
if needed. You can define your own callback function fields with names that make
sense for your hardware; e.g. `onLight`, `onDark`, `onHot`, `onCold`...

See the [Button.cpp](src/eventuino/Button.cpp) and [Toggle.cpp](src/eventuino/Toggle.cpp)
classes for ideas.
