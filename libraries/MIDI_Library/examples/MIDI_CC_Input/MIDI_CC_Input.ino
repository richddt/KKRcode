/*
    MIDI CC Light Show
    by Notes and Volts
    www.notesandvolts.com
*/

#include <MIDI.h>  // Add Midi Library

// Define labels for the 6 PWM pins
#define LEDPWM1 3
#define LEDPWM2 5
#define LEDPWM3 6
#define LEDPWM4 9
#define LEDPWM5 10
#define LEDPWM6 11

//Create an instance of the library with default name, serial port and settings
MIDI_CREATE_DEFAULT_INSTANCE();

void setup() {
  //pinMode (LED, OUTPUT); // Set Arduino board pin 13 to output
  MIDI.begin(MIDI_CHANNEL_OMNI); // Initialize the Midi Library.
  // OMNI sets it to listen to all channels.. MIDI.begin(2) would set it
  // to respond to notes on channel 2 only.
  MIDI.setHandleControlChange(MyCCFunction); // This command tells the MIDI Library
  // the function you want to call when a Continuous Controller command
  // is received. In this case it's "MyCCFunction".
}

void loop() { // Main loop
  MIDI.read(); // Continuously check if Midi data has been received.
}

// MyCCFunction is the function that will be called by the Midi Library
// when a Continuous Controller message is received.
// It will be passed bytes for Channel, Controller Number, and Value
// It checks if the controller number is within the 22 to 27 range
// If it is, light up the corresponding LED with the PWM brightness equal to the Value byte
void MyCCFunction(byte channel, byte number, byte value) {
  switch (number) {
    case 22:
      analogWrite(LEDPWM1, value * 2);
      break;
    case 23:
      analogWrite(LEDPWM2, value * 2);
      break;
    case 24:
      analogWrite(LEDPWM3, value * 2);
      break;
    case 25:
      analogWrite(LEDPWM4, value * 2);
      break;
    case 26:
      analogWrite(LEDPWM5, value * 2);
      break;
    case 27:
      analogWrite(LEDPWM6, value * 2);
      break;
  }
}
