#include <SPI.h>  // this library has the "ShiftOut" command in it
#include <MIDI.h>  // Add Midi Library

#define cvPin 13
#define midiIn 0
#define clockPin 5
#define dataPin 6
#define latchPin 4
#define midiSwitchPin 2
#define cvPot A0
#define sweepSwitchPin 3
int oldNote;            // this is the last value that we did set
int newNote;            // this is the value we want to set
int noteCount;          // here we will count how many notes we have pressed at any one time



// Below is my function that will be called by the Midi Library
// when a MIDI NOTE ON message is received.
// It will be passed bytes for Channel, Pitch, and Velocity
void HandleNoteOn(byte channel, byte pitch, byte velocity) {

  if (velocity == 0) {//A NOTE ON message with a velocity = Zero is actualy a NOTE OFF
    noteCount = noteCount - 1;
  }
  else
  {
    newNote = pitch; // set the new note to be the pitch of the midi in
    noteCount = noteCount + 1;
  }
  if (noteCount >> 0)
    {digitalWrite (cvPin, HIGH);}
  else
    {digitalWrite (cvPin, LOW);}
}



void setup() {
  pinMode(clockPin, OUTPUT);    // configure the clock pin to be an output
  pinMode(midiIn, INPUT);
  pinMode(cvPin, OUTPUT);
  pinMode(dataPin, OUTPUT);    // configure the data pin to be an output
  pinMode(latchPin, OUTPUT);    // configure the latch pin to be an output
  pinMode(cvPot, INPUT);    // configure the pot to be an input
  pinMode(midiSwitchPin, INPUT);    // configure the midi switch to be an input
  pinMode(sweepSwitchPin, INPUT);  // configure the sweepSwitch io be an input
  digitalWrite(clockPin, LOW);  // Here we write the clock pin to be low
  digitalWrite(latchPin, LOW);  // Here we set the latch high to tell the device we dont need it
  oldNote = 0;            // the last value we set at this point is invalid, so we set this to 0
  newNote = 1024;            // we want to start off somewhere sensible so ballpark? 1024
  noteCount = 0;
  
  MIDI.begin(MIDI_CHANNEL_OMNI); // Initialize the Midi Library.
// OMNI sets it to listen to all channels.. MIDI.begin(2) would set it
// to respond to channel 2 notes only.
  
    MIDI.setHandleNoteOn(HandleNoteOn); // This is important!! This command
  // tells the Midi Library which function I want called when a Note ON command
  // is received. in this case it's "HandleNoteOn".
}



void writeClockAndData (byte TopNumToSend, byte BottomNumToSend)
{
  digitalWrite(latchPin, HIGH);
  digitalWrite(cvPin, LOW);  // default position at the start is notes off
  digitalWrite(clockPin, LOW);  // we are interfacing with a device that is clocked by rising edges, so we must first set the clock pin low
  shiftOut(dataPin, clockPin, MSBFIRST, TopNumToSend);  // here we send the data top end of the data
  shiftOut(dataPin, clockPin, MSBFIRST, BottomNumToSend);  // here we send the data bottom end of the data
  digitalWrite(latchPin, LOW); // driving the latch high updates the DAC
}


void loop () { // Main loop
  oldNote = newNote; //transfere the old value into oldNote before updating newNote
  if (digitalRead(midiSwitchPin) == HIGH) {
     MIDI.read(); // Continually check what Midi Commands have been received.
  }
  else
  {
    if (digitalRead(sweepSwitchPin) == HIGH) {
      delay(10);
      newNote = newNote + analogRead(cvPot);
      if (newNote > 4096) {newNote = 0;}
    }
    else
    {
      delay(10);
      newNote = analogRead(cvPot);
      newNote = newNote * 4; // scale the 0 to 1024 value from the pot to 0 to 4096 for the DAC
    }
  }



  if (oldNote != newNote) {
    writeClockAndData(highByte(newNote),lowByte(newNote)); // here we set the data
  }
}


