#include <SPI.h>

#define LED 13
#define clockPin 0
#define dataPin 1
#define latchPin 12
  int x;            // x is going to be the value that we set
  int ledPin[] = {2,3,4,5,6,7,8,9,10,11};
  
  
void setup() {
  Serial.begin(9600);      // open the serial port at 9600 bps:    
  pinMode(clockPin, OUTPUT);    // configure the clock pin to be an output
  pinMode(dataPin, OUTPUT);    // configure the data pin to be an output
  pinMode(latchPin, OUTPUT);    // configure the latch pin to be an output
  digitalWrite(clockPin, LOW);  // Here we write the clock pin to be low
  digitalWrite(latchPin, LOW);  // Here we set the latch high to tell the device we dont need it
  for (int i =0;i<10;i++)  // here we turn all the 10 led pins to output
  {
    pinMode(ledPin[i], OUTPUT);
  }
  x == 0;            // inits x at 0
}


void displayBinary(int numToShow)
{
  for (int i =0;i<10;i++)
  {
    if (bitRead(numToShow, i)==1)
    {
      digitalWrite(ledPin[i], HIGH); 
      Serial.print ("1");
    }
    else
    {
      digitalWrite(ledPin[i], LOW); 
      Serial.print ("0");
    }
  }
  Serial.println();
}

void writeClockAndData (byte TopNumToSend, byte BottomNumToSend)
{
  Serial.print("Top = ");
  Serial.print(TopNumToSend);
  Serial.print(" | Bot =");
  Serial.println(BottomNumToSend);
  digitalWrite(latchPin, HIGH);
  digitalWrite(clockPin,LOW);  // we are interfacing with a device that is clocked by rising edges, so we must first set the clock pin low
  //shiftOut(dataPin, clockPin, MSBFIRST, (TopNumToSend >>8));  // here we send the data top end of the data
  shiftOut(dataPin, clockPin, LSBFIRST, BottomNumToSend);  // here we send the data bottom end of the data
  digitalWrite(latchPin, LOW); // driving the latch LOW updates the DAC
}

void loop () { // Main loop
  //allOFF();    // turn all the LEDs off
  //lightPin(); //setDigitalOutputPins(x);
  //binCountLightPin();
  displayBinary(x);
  writeClockAndData(highByte(x),lowByte(x));
  
  //Serial.print ("x = ");
  //Serial.println (x);
  
  delay(1);
  x = x + 1;
  if (x > 1023) {
    x = 0; // make x=0 if its getting to high
  }
}

