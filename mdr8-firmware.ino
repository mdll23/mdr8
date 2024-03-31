/*
  Firmware for MDR8 Midi Controller
  -------------------------------
  Parts:
    - Pro Micro / Arduino Leonardo
    - CD74HC4067 Multiplexer
    - 8x 100k Potentiometer
  Wiring:
    CD74HC4067:
      SIG >> A0
      S0 >> 6
      S1 >> 7
      S2 >> 8
      S3 >> 9
      C0-C7 >> Potentiometer
  -------------------------------
*/

#include "MIDIUSB.h"
#include "ResponsiveAnalogRead.h"
#include "CD74HC4067.h"

const int N_POTIS = 8;
const int MIDI_Channel = 0;

// Mux control pins
const int s0 = 6;
const int s1 = 7;
const int s2 = 8;
const int s3 = 9;

// Mux "SIG" pin
const int SIG_pin = 0;

int controlPin[] = {s0, s1, s2, s3};
int muxChannel[16][4]={
  {0,0,0,0}, //channel 0
  {1,0,0,0}, //channel 1
  {0,1,0,0}, //channel 2
  {1,1,0,0}, //channel 3
  {0,0,1,0}, //channel 4
  {1,0,1,0}, //channel 5
  {0,1,1,0}, //channel 6
  {1,1,1,0}, //channel 7
  {0,0,0,1}, //channel 8
  {1,0,0,1}, //channel 9
  {0,1,0,1}, //channel 10
  {1,1,0,1}, //channel 11
  {0,0,1,1}, //channel 12
  {1,0,1,1}, //channel 13
  {0,1,1,1}, //channel 14
  {1,1,1,1}  //channel 15
};

int potPin[N_POTIS] = { 0, 1, 2, 3, 4, 5, 6, 7 };
int potCC[N_POTIS] = { 24, 25, 26, 27, 20, 21, 22, 23};

int potReading[N_POTIS] = { 0 };
int potPState[N_POTIS] = { 0 };
int potState[N_POTIS] = { 0 };

int midiState[N_POTIS] = { 0 };
int pmidiState[N_POTIS] = { 0 };

const byte potThreshold = 1;
const int POT_TIMEOUT = 500;
unsigned long ppotTime[N_POTIS] = {0};
unsigned long potTimer[N_POTIS] = {0};

float snapMultiplier = 0.01;
ResponsiveAnalogRead responsivePot[N_POTIS] = {};

void controlChange(byte channel, byte control, byte value) {
  midiEventPacket_t event = {0x0B, 0xB0 | channel, control, value};
  MidiUSB.sendMIDI(event);
}

void readPotis() {
  for (int i = 0; i < N_POTIS; i++) {
    potReading[i] = readMux(i);
    responsivePot[i].update(potReading[i]);
    potState[i] = responsivePot[i].getValue();
    midiState[i] = map(potState[i], 0, 1023, 0, 128);

    int potVar = abs(potPState[i] - potState[i]);
    if (potVar > potThreshold) {
      ppotTime[i] = millis();
    }
    potTimer[i] = millis() - ppotTime[i];
    int midiChange = abs(pmidiState[i] - midiState[i]);

    if (midiChange > 0) {
      controlChange(MIDI_Channel,potCC[i], midiState[i]);
      MidiUSB.flush();
    }
    potPState[i] = potState[i];
    pmidiState[i] = map(potPState[i], 0, 1023, 0, 128);
  }
}

float readMux(int channel){
  for(int i = 0; i < 4; i ++){
    digitalWrite(controlPin[i], muxChannel[channel][i]);
  }
  int val = analogRead(SIG_pin);
  return val;
}

void setup() {
  pinMode(s0, OUTPUT);
  pinMode(s1, OUTPUT);
  pinMode(s2, OUTPUT);
  pinMode(s3, OUTPUT);

  digitalWrite(s0, LOW);
  digitalWrite(s1, LOW);
  digitalWrite(s2, LOW);
  digitalWrite(s3, LOW);
  Serial.begin(9600);
  for (int i = 0; i < N_POTIS; i++) {
    responsivePot[i] = ResponsiveAnalogRead(0, true, snapMultiplier);
    responsivePot[i].setAnalogResolution(1023);
  }
}

void loop() {
  readPotis();
}
