# ZzzInput
Simple Arduino / C++ library to manage input data from sensors


The library consist of a single header file (ZzzInput.h) containing template classes.


### Constructor

```cpp
ZzzInputDriverAnalogPin<pin1> inputDriver; //Define driver to manage an analog pin
ZzzInput input(inputDriver, intervalMs=1000); //Instance to manage inputs

```

### Functions

```cpp
void update();        // To call in Arduino loop

void setCallback(myCallback);  // Set the callback to call on each new value.
```

### Included examples

- `InputAnalog/InputAnalog.ino` - Show basic usage with analog read


### Simple code example for analog reads

```cpp
#include <ZzzInput.h>

ZzzInput <4> input;

void setup()
{
    ...
}

void loop()
{
    ...
    input.update();
    ...
}
```

