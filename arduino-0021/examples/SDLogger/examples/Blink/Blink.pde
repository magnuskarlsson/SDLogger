/*
Blink
Turns on the green LED on for one second, then turn on the red LED for one second, repeatedly.
This example code is in the public domain. */

// Pin 0 has a green LED connected:
#define greenLED 0

// Pin 1 has a red LED connected:
#define redLED 1

void setup() {
// initialize the digital pins as an outputs.
pinMode(greenLED, OUTPUT);
pinMode(redLED, OUTPUT);
}

void loop() {
digitalWrite(greenLED, HIGH); // set the green LED on
digitalWrite(redLED, LOW); // set the red LED off
delay(1000); // wait for a second
digitalWrite(redLED, HIGH); // set the red LED on
digitalWrite(greenLED, LOW); // set the green LED off
delay(1000); // wait for a second
}

