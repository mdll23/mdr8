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

#include <MIDIUSB.h>
#include <ResponsiveAnalogRead.h>
#include "Mux.h"

const int N_POTIS = 8;
const byte MIDI_Channel = 1;

// Mux "SIG" pin
const int SIG_pin = 0;

int potPin[N_POTIS] = { 0, 1, 2, 3, 4, 5, 6, 7 };
uint8_t potCC[N_POTIS] = { 24, 25, 26, 27, 20, 21, 22, 23};

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

Mux mux(6, 7, 8, -1);

void readPotis(int num) {
  potReading[num] = analogRead(SIG_pin);
  responsivePot[num].update(potReading[num]);
  potState[num] = responsivePot[num].getValue();
  midiState[num] = map(potState[num], 0, 1023, 0, 128);

  int potVar = abs(potPState[num] - potState[num]);
  if (potVar > potThreshold) {
    ppotTime[num] = millis();
  }
  potTimer[num] = millis() - ppotTime[num];
  int midiChange = abs(pmidiState[num] - midiState[num]);

  if (midiChange > 0) {
    controlChange(MIDI_Channel,potCC[num], midiState[num]);
    MidiUSB.flush();
  }
  potPState[num] = potState[num];
  pmidiState[num] = map(potPState[num], 0, 1023, 0, 128);

}


void setup() {
  Serial.begin(9600);
  pinMode(9, OUTPUT);
  digitalWrite(9, 0);
  for (int i = 0; i < N_POTIS; i++) {
    responsivePot[i] = ResponsiveAnalogRead(0, true, snapMultiplier);
    responsivePot[i].setAnalogResolution(1023);
  }
}

void loop() {
  for (int i = 0; i < N_POTIS; i++) {
    mux.channel(i);
    readPotis(i);
  }
}
