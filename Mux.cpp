#include "Arduino.h"
#include "Mux.h"

uint8_t _CD74HC4067_Table[16][4] = {
    {0,  0,  0,  0},
    {1,  0,  0,  0},
    {0,  1,  0,  0},
    {1,  1,  0,  0},
    {0,  0,  1,  0},
    {1,  0,  1,  0},
    {0,  1,  1,  0},
    {1,  1,  1,  0},
    {0,  0,  0,  1},
    {1,  0,  0,  1},
    {0,  1,  0,  1},
    {1,  1,  0,  1},
    {0,  0,  1,  1},
    {1,  0,  1,  1},
    {0,  1,  1,  1},
    {1,  1,  1,  1}
};

Mux::Mux(uint8_t S0, uint8_t S1, uint8_t S2, uint8_t S3)
{
  pinMode(S0, OUTPUT);
  pinMode(S1, OUTPUT);
  pinMode(S2, OUTPUT);
  pinMode(S3, OUTPUT);
  _S0 = S0;
  _S1 = S1;
  _S2 = S2;
  _S3 = S3;
}

void Mux::channel(uint8_t channel)
{
  digitalWrite(_S0, _CD74HC4067_Table[channel][0]);
  digitalWrite(_S1, _CD74HC4067_Table[channel][1]);
  digitalWrite(_S2, _CD74HC4067_Table[channel][2]);
  digitalWrite(_S3, _CD74HC4067_Table[channel][3]);
}
