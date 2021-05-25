#include <ZzzData.h>
#include <ZzzInput.h>

//Update to an analog pin to read
#define ANALOG_PIN 33

ZzzInputDriverAnalogPin<ANALOG_PIN, ZzzDataMinMax <int>> inputDriver; //Define driver to read an analog pin
ZzzInput input(inputDriver, 5000); //Instance to manage inputs, will read input every 5000ms = 5seconds

/** Callback invoked by input instance */
void inputReady() {
	Serial.print("Analog read:");
	Serial.print(inputDriver.data.get());
	Serial.print(" min:");
	Serial.print(inputDriver.data.min());
	Serial.print(" max:");
	Serial.print(inputDriver.data.max());
	Serial.println();
}

void setup() {
	Serial.begin(115200);
	delay(250); //to ensure correctly initialized

	input.setCallback(inputReady);
}

void loop() {
	//call periodically to process inputs
	input.update();
}

