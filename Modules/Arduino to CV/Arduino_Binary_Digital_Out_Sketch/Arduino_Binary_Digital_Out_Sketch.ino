#include <SPI.h>  // this library has the "ShiftOut" command in it
#include <MIDI.h>  // Add Midi Library

#define gatePin 13
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
int midiLookup[144]     // here we define the MIDI lookup table, this is 12x12 (12 octaves)
                        // and will define C-2 to C10 (NOTE: This table WILL need adjusting)
= {   12,   13,   14,   15,   16,   17,   18,   19,   20,   21,   22,      // Octave -2
      23,   24,   25,   26,   27,   28,   29,   30,   31,   32,   33,      // Octave -1
      34,   35,   36,   37,   38,   39,   40,   41,   42,   43,   44,      // Octave 0
      45,   46,   47,   48,   49,   50,   51,   52,   53,   54,   55,      // Octave 1
      56,   57,   58,   59,   60,   61,   62,   63,   64,   65,   66,      // Octave 2
      67,   68,   69,   70,   71,   72,   73,   74,   75,   76,   77,      // Octave 3
      78,   79,   80,   81,   82,   83,   84,   85,   86,   87,   88,      // Octave 4
      89,   90,   91,   92,   93,   94,   95,   96,   97,   98,   99,      // Octave 5
     100,  101,  102,  103,  104,  105,  106,  107,  108,  109,  110,      // Octave 6
     111,  112,  113,  114,  115,  116,  117,  118,  119,  120,  121,      // Octave 7
     122,  123,  124,  125,  126,  127,  128,  129,  130,  131,  132,      // Octave 8
     133,  134,  135,  136,  137,  138,  139,  140,  141,  142,  143,      // Octave 9
     144,  145,  146,  147,  148,  149,  150,  151,  152,  153,  154       // Octave 10
  };
    

// Below is a function that will be called by the Midi Library
// when a MIDI NOTE ON message is received.
// It will be passed bytes for Channel, Pitch, and Velocity
void HandleNoteOn(byte channel, byte pitch, byte velocity) {
  if (velocity == 0) {//A NOTE ON message with a velocity = Zero is actualy a NOTE OFF
    noteCount = noteCount - 1;  // we remove 1 from the 'note on' count
  }
  else
  {
    newNote = midiLookup[pitch]; // set the newnote to be the value defined by the lookup table
    noteCount = noteCount + 1;   // we add one to the 'note on' count
  }
  if (noteCount >> 0)
    {digitalWrite (gatePin, HIGH);}
  else
    {digitalWrite (gatePin, LOW);}
}



void setup() {
  pinMode(clockPin, OUTPUT);        // configure the clock pin to be an output
  pinMode(midiIn, INPUT);           // configure the midi in pin to be an input
  pinMode(gatePin, OUTPUT);         // configure the gate pin to be an output
  pinMode(dataPin, OUTPUT);         // configure the data pin to be an output
  pinMode(latchPin, OUTPUT);        // configure the latch pin to be an output
  pinMode(cvPot, INPUT);            // configure the pot to be an input
  pinMode(midiSwitchPin, INPUT);    // configure the midi switch to be an input
  pinMode(sweepSwitchPin, INPUT);   // configure the sweepSwitch io be an input
  digitalWrite(clockPin, LOW);      // Here we write the clock pin to be low
  digitalWrite(latchPin, LOW);      // Here we set the latch high to tell the device we dont need it
  oldNote = 0;                      // the last value we set at this point is invalid, so we set this to 0
  newNote = 1024;                   // we want to start off somewhere sensible so ballpark? 1024
  noteCount = 0;                    // assume that all notes are off when the unit powers on
  
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
  digitalWrite(gatePin, LOW);  // default position at the start is notes off
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
    noteCount = 0; // this is a soft reset incase we get MIDI notes stuck on, 
                   // we can flip in and out of MIDI mode to clear the problem
    digitalWrite(gatePin, LOW);  // this does a hard clear of the 'note on' count
    if (digitalRead(sweepSwitchPin) == HIGH) {
      delay(10);
      newNote = newNote + analogRead(cvPot);  // set the note to be incremented by the value of the analog pot
      if (newNote > 4096) {newNote = 0;}      // reset the sweep if it hits max.
    }
    else
    {
      delay(10);
      newNote = analogRead(cvPot);            // set the value of the note to be played to the value of the analog pot.
      newNote = newNote * 4;                  // scale the 0 to 1024 value from the pot to 0 to 4096 for the DAC
    }
  }



  if (oldNote != newNote) {                    // if the note value has changed from the last application scan...
    writeClockAndData(highByte(newNote),lowByte(newNote)); // here we set the data
  }
}


