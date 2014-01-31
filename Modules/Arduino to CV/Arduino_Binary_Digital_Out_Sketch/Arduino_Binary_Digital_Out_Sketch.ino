#include <SPI.h>

#define LED 13
#define clockPin 5
#define dataPin 6
#define latchPin 4
  int x;            // x is going to be the value that we set

  
  
void setup() {
  Serial.begin(9600);      // open the serial port at 9600 bps:    
  pinMode(clockPin, OUTPUT);    // configure the clock pin to be an output
  pinMode(dataPin, OUTPUT);    // configure the data pin to be an output
  pinMode(latchPin, OUTPUT);    // configure the latch pin to be an output
  digitalWrite(clockPin, LOW);  // Here we write the clock pin to be low
  digitalWrite(latchPin, LOW);  // Here we set the latch high to tell the device we dont need it
  x = 0;            // inits x at 0
}



void writeClockAndData (byte TopNumToSend, byte BottomNumToSend)
{
  Serial.print("Top = ");
  Serial.print(TopNumToSend);
  Serial.print(" | Bot =");
  Serial.println(BottomNumToSend);
  digitalWrite(latchPin, HIGH);
  digitalWrite(clockPin, LOW);  // we are interfacing with a device that is clocked by rising edges, so we must first set the clock pin low
  //delay(10);
  shiftOut(dataPin, clockPin, MSBFIRST, TopNumToSend);  // here we send the data top end of the data

   shiftOut(dataPin, clockPin, MSBFIRST, BottomNumToSend);  // here we send the data bottom end of the data
  
  //delay(10);
  digitalWrite(latchPin, LOW); // driving the latch high updates the DAC
}

void loop () { // Main loop
  //allOFF();    // turn all the LEDs off
  //lightPin(); //setDigitalOutputPins(x);
  //binCountLightPin();
  writeClockAndData(highByte(x),lowByte(x));
  //Serial.print ("x = ");
  //Serial.println (x);
  
   delay(1);
   x = x + 90;
  if (x > 4096) {
    x = 0; // make x=0 if its getting to high
  }
}

