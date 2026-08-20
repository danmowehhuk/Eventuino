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

Eventuino provides `Button`, `Toggle`, a generic `DigitalPinSource` and various
`Timer`s out of the box, but Eventuino can also be extended to support your 
other digital or analog input devices; say, a thermocoupler connected to the 
base of a transistor forming a "temperature switch."

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

void buttonPressed(uint8_t value) {
  Serial.print("Button pressed with value=");
  Serial.println(value);
}

void setup() {
  Serial.begin(9600);
  while (!Serial);

  button.onPressed=buttonPressed;

  evt.addEventSource(&button);
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
was pressed, for example. Next, instantiate `Eventuino`.

In your `setup()` function, call `addEventSource` for each of your event
sources. Then, call Eventuino's `begin()` method. Eventuino will take care of
initializing all your pins, so no need to worry about `pinMode`s here.

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

Eventuino provides several common event sources out of the box. Below are the
provided event sources and their callback functions:

| Event Source | Callback | Event Trigger |
| ------------ | -------- | ------- |
| [DigitalPinSource](src/eventuino/DigitalPinSource.h) | onChangeState | When the pin switches from HIGH to LOW or vice versa |
| [Button](src/eventuino/Button.h) | onPressed | When the button switches from unpressed to pressed |
| [Button](src/eventuino/Button.h) | onReleased | When the button switches from pressed to unpressed |
| [Button](src/eventuino/Button.h) | onLongPressed | When the button has remained pressed for more than some delay. May optionally repeat (disabled by default, see `enableRepeat`) |
| [Toggle](src/eventuino/Toggle.h) | onFlip | When the switch changes state in either direction |
| [Toggle](src/eventuino/Toggle.h) | onActivate | When the switch changes to the activated state |
| [Toggle](src/eventuino/Toggle.h) | onDeactivate | When the switch changes to the deactivated state |
| [Timer14Bit](src/eventuino/Timer.h) | onExpire | When *at least* `duration`ms have passed |
| [Timer30Bit](src/eventuino/Timer.h) | onExpire | When *at least* `duration`ms have passed |
| [IntervalTimer14Bit](src/eventuino/Timer.h) | onExpire | Every time *at least* N*`duration`ms have passed |
| [IntervalTimer30Bit](src/eventuino/Timer.h) | onExpire | Every time *at least* N*`duration`ms have passed |

### Debounce, Long Hold and Repeat delays

Eventuino has default delays for debouncing (75ms), long holds (1s) and repeats (200ms), but these can be changed.
Note, however, that these changes apply to all the event sources. The `DigitalPinSource` class is the base class for all digital event sources, and provides the following static methods:
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


## Building without Arduino

Everything above assumes you're building through `arduino-cli`/the Arduino
IDE. Eventuino also builds outside Arduino entirely - useful if your
project doesn't use the Arduino core at all (a bare-metal `main()`, a
different build system, etc.). AVR is the platform this works on today;
`examples/button_basic_avr/` and `test/test-suite-avr/` below are worked
examples for it, not the only non-Arduino target this is meant to support.

For AVR specifically, this build is selected with two flags passed
together: `-DNO_ARDUINO -DHAL_AVR`. It requires
[BareMetalHAL](https://github.com/danmowehhuk/BareMetalHAL) - a sibling
library providing the GPIO, UART, timing and dynamic-memory primitives
that Arduino normally supplies.

**The callback signature is different on this build.** The ["Handling
Events"](#handling-events) section above documents callbacks as taking a
single `uint8_t value` parameter, and that's accurate for Arduino - but
it only compiles there because `arduino-cli` bakes in `-fpermissive`,
which quietly tolerates a function pointer with a shorter parameter list
than expected. Bare `avr-g++` has no such leniency, so on
`-DNO_ARDUINO -DHAL_AVR` every callback needs the full two-argument
signature:

```c
void myHandler(uint8_t value, void* state) { // Do something... }
```

Assigning it works exactly the same as before (`myButton.onPressed =
myHandler;`). If you don't need the `state` parameter, just ignore it -
see the worked example below for the idiom.

See [examples/button_basic_avr/](examples/button_basic_avr/) for a
complete bare-metal sketch, and
[test/test-suite-avr/](test/test-suite-avr/) for the bare-metal port of
Eventuino's own test suite. Both build with their own `build.sh`, which
looks for BareMetalHAL (and, for the test suite, `TestTool`) as sibling
libraries under `~/Arduino/libraries/` by default - set the
`BAREMETALHAL_SRC` and/or `TESTTOOL_SRC` environment variables to point
at their `src/` directories if yours live somewhere else.


# Extending Eventuino

Create custom event sources!

Say you have a thing that connects to a digital pin as an input. You could poll it
from the `loop()` function, but if you want to take the event handler approach, you
can create your own Eventuino event source!

Just extend the `eventuino/DigitalPinSource` class! Your class would override the 
`onChange(uint8_t)` method, and may optionally override `onLongHold(uint8_t)`
if needed. You can define your own callback function fields with names that make
sense for your hardware; e.g. `onLight`, `onDark`, `onHot`, `onCold`...

See the [Button.cpp](src/eventuino/Button.cpp) and [Toggle.cpp](src/eventuino/Toggle.cpp)
classes for ideas.
