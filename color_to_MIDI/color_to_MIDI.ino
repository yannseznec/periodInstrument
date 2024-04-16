/* some code for turning RGB values into MIDI
September 2023
by Yann Seznec www.yannseznec.com
no copyright or anything, use however you want

*/

// #include <MIDI.h>
#include "Adafruit_APDS9960.h"
Adafruit_APDS9960 apds;

// the MIDI channel number to send messages
const int channel = 5;
//

int const numPins = 3; //  number of analog inputs for CC
int currentVal[numPins];
int newVal[numPins];
int analogPins[] = {  
  14,15,16   // which analog pins to use
};
int analogPinsCC[] = {  
  50,51,52   // which CC to use
};

// STRING CONTROLLER OR OTHER THING THAT NEEDS CALIBRATION ON STARTUP
int const numStringPins = 0; //  number of analog inputs 
int currentStringVal[numStringPins];
int newStringVal[numStringPins];
int newStringValCal[numStringPins];
int analogStringPins[] = {  
  15,18   // which analog pins to use 
};
int analogStringPinsCC[] = {  
  40,41   // which CC to use
};

int const numDigPins = 0; // number of digital pins to send note values
int currentDig[numDigPins];
int digitalpin[] = {
 2,9,10,11,12  // which digital pins to use for sending note values
};
int digitalpitch[] = {
  60,61,62,66,71,72 // which midi notes to send from the digitalpins pins
  }; 



int const numDigPinsCC = 0; // number of digital pins to send CC (0 or 127)
int currentDigCC[numDigPinsCC];
int digPinsCC[] = {
   2,3,4,5 // which digital pins to use for sending CC
};
int digitalPinsCC[] = {
  72,74,76,78
};



int const numTouchPins = 0; // number of pins to use as touchpins, sending note values
int touch[numTouchPins];
int touchon[numTouchPins];
int touchpin[] = {
  4,0,1}; // which digital pins to use as touch pins
  int touchpitch[] = {
70,71,73,74  }; // which midi notes to send from the touch pins
  int touchThreshold = 2000;
  int touchMax = 7000; 
  
int const numOutputs = 1; // number of pins to use as outputs
int outs[numOutputs];
int outPins[] = {
 23,14,4,5,6,7,8,9}; // which digital pins to use as out pins
  int outputpitch[] = {
 70,61,62,63,64,65,66,67,72}; // which midi notes to use for sending the outputs

int const numCCOutputs = 0; // number of pins to use as CC outputs (PWM)
int outsCC[numCCOutputs];
int outCCPins[] = {
 23,14,15,16}; // which digital pins to use as out pins
  int outputCC[] = {
 30,49,50,51 }; // which CC to use for sending the outputs





 // Created and binds the MIDI interface to the default hardware Serial port


void setup() {
  // MIDI.begin(4);
  usbMIDI.setHandleNoteOn(OnNoteOn);
  usbMIDI.setHandleNoteOff(OnNoteOff);
  usbMIDI.setHandleControlChange(OnControlChange);
  
  


  Serial.begin(115200);

   if(!apds.begin()){
    Serial.println("failed to initialize device! Please check your wiring.");
  }
  else Serial.println("Device initialized!");

  //enable color sensign mode
  apds.enableColor(true);

}

void loop() {

// color to midi conversion
uint16_t r, g, b, c;

  while(!apds.colorDataReady()){
  delay(5);
  }

  apds.getColorData(&r, &g, &b, &c);

 usbMIDI.sendControlChange(60, map(r, 0, 600,0,127), channel); 
 usbMIDI.sendControlChange(61, map(g,0,600,0,127), channel); 
 usbMIDI.sendControlChange(62, map(b, 0, 600,0,127), channel); 
 usbMIDI.sendControlChange(63, map(c, 0, 1100,0,127), channel); 
 float colorAvg = r/float((r+g+b));

 usbMIDI.sendControlChange(64, int(colorAvg*100), channel); 
 Serial.println(colorAvg);
Serial.print("R:\t"); Serial.print(int(r));
  Serial.print("\tG:\t"); Serial.print(int(g));
  Serial.print("\tB:\t"); Serial.print(int(b));
    Serial.print("\tC:\t"); Serial.print(int(c));
Serial.print("\n");
  Serial.println();

// analog pins
 for (int i = 0; i < numPins; i++) {

    newVal[i] = analogRead(analogPins[i]);

    if (abs(newVal[i] - currentVal[i])>3) {
//normal
      usbMIDI.sendControlChange(analogPinsCC[i], newVal[i]>>3, channel); 
  //    MIDI.sendControlChange(analogPinsCC[i], newVal[i]>>3, channel); 
 // use this if the wiring is backwards :\  
   //   usbMIDI.sendControlChange(analogPinsCC[i], map(newVal[i]>>3,0,127,127,0), channel); 
 //     MIDI.sendControlChange(analogPinsCC[i], map(newVal[i]>>3,0,127,127,0), channel); 
 
      currentVal[i] = newVal[i];
    }  
  }

  

  
  // i think if you remove these last two lines everything breaks and things are sad and people cry
  while (usbMIDI.read()); // read and discard any incoming MIDI messages
   delay(5); 
}

 void OnNoteOn(byte channel, byte note, byte velocity) {
   for (int i = 0; i < numOutputs; i++) {
     
     if (note == outputpitch[i]) {
      digitalWrite(outPins[i], HIGH);
     }
  }
     
}

void OnNoteOff(byte channel, byte note, byte velocity) {
   for (int i = 0; i < numOutputs; i++) {
      if (note == outputpitch[i]) {
      digitalWrite(outPins[i], LOW);
      }
  }
}

 void OnControlChange(byte channel, byte control, byte value) {

   for (int i = 0; i < numCCOutputs; i++) {
     
     if (control == outputCC[i]) {
      analogWrite(outCCPins[i], map(value,0,127,255,0));
    
     }
  }
     
}
