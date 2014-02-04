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
= {    28,   57,   85,  114,  142,  171,  199,  228,  256,  284,  313,  341, //-2
      370,  398,  427,  455,  483,  512,  540,  569,  597,  626,  654,  682, //-1
      711,  739,  768,  796,  825,  853,  882,  910,  938,  967,  995, 1024, //0
     1052, 1081, 1109, 1138, 1166, 1194, 1223, 1251, 1280, 1308, 1337, 1365, //1
     1393, 1422, 1450, 1479, 1507, 1536, 1564, 1592, 1621, 1649, 1678, 1706, //2
     1735, 1763, 1792, 1820, 1848, 1877, 1905, 1934, 1962, 1991, 2019, 2048, //3
     2076, 2104, 2133, 2161, 2190, 2218, 2247, 2275, 2303, 2332, 2360, 2389, //4
     2417, 2446, 2474, 2502, 2531, 2559, 2588, 2616, 2645, 2673, 2702, 2730, //5
     2758, 2787, 2815, 2844, 2872, 2901, 2929, 2958, 2986, 3014, 3043, 3071, //6
     3100, 3128, 3157, 3185, 3213, 3242, 3270, 3299, 3327, 3356, 3384, 3412, //7
     3441, 3469, 3498, 3526, 3555, 3583, 3612, 3640, 3668, 3697, 3725, 3754, //8
     3782, 3811, 3839, 3868, 3896, 3924, 3953, 3981, 4010, 4038, 4067, 4095  //9
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


