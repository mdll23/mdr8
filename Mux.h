#ifndef Mux_h
#define Mux_h

#include "Arduino.h"
#include <inttypes.h>

extern uint8_t _CD74HC467_Table[16][4];

class Mux
{
  public:
    Mux(uint8_t S0, uint8_t S1, uint8_t S2, uint8_t S3);
    void channel(uint8_t channel);
  private:
    uint8_t _S0;
    uint8_t _S1;
    uint8_t _S2;
    uint8_t _S3;
};

#endif
