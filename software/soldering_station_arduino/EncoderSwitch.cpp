#include "EncoderSwitch.h"

EncoderSwitch::EncoderSwitch(int pin1, int pin2)
{
  // Attach the input pins to the debouncer
  m_pin1.attach(pin1);
  m_pin1.interval(7);
  m_pin2.attach(pin2);
  m_pin2.interval(7);

  // Initialize the state
  state = ENCSW_STATE_UNKNOWN;
}

int EncoderSwitch::update()
{
  // Update the debouncers
  bool chg = m_pin1.update();
  chg |= m_pin2.update();
  int ret = 0;
  
  if (chg) {
    uint8_t inps = ((m_pin1.read() << 1) + m_pin2.read()) & 0x03;
    switch(state) {
    case ENCSW_STATE_UNKNOWN:
    case ENCSW_STATE_11:  
      if (inps == 0)
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
        ret = 1;
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
        ret = -1;
        state = ENCSW_STATE_11;
      }
      break;   
    }
  }
  
  return ret;
}
  

