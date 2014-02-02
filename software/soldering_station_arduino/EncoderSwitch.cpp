#include "EncoderSwitch.h"

EncoderSwitch::EncoderSwitch()
{
  state = ENCSW_STATE_UNKNOWN;
}

void EncoderSwitch::update(int inp1, int inp2)
{
  uint8_t inps = ((inp1 << 1) + inp2) & 0x03;
  switch(state) {
  case ENCSW_STATE_UNKNOWN:
  case ENCSW_STATE_11:  
    if ((inp1 == 0) && (inp2 == 0))
      state = ENCSW_STATE_00;
    break;
    
  case ENCSW_STATE_00:
    switch (inps) {
    case 0b00:
      // No change
      break;
    case 0b01:
      state = ENCSW_STATE_01;
      break;
    case 0b10:
      state = ENCSW_STATE_10;
      break;
    default:
      state = ENCSW_STATE_UNKNOWN;
    }
    break;
    
  case ENCSW_STATE_01:
    switch(inps) {
    case 0b00:
      state = ENCSW_STATE_00;
      break;
    case 0b01:
      break;
    case 0b10:
      state = ENCSW_STATE_UNKNOWN;
    case 0b11:
      // Switch detected
      eventUp();
      state = ENCSW_STATE_11;
    }
    break;
    
  case ENCSW_STATE_10:
    switch(inps) {
    case 0b00:
      state = ENCSW_STATE_00;
      break;
    case 0b01:
      state = ENCSW_STATE_UNKNOWN;
      break;
    case 0b10:
      // No change
      break;
    case 0b11:
      // Switch detected
      eventDown();
      state = ENCSW_STATE_11;
    }
    break;   
  }
}
  

