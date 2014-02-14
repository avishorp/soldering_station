
#ifndef __ENCODERSWITCH_H__
#define __ENCODERSWITCH_H__

#include <inttypes.h>
#include <Bounce2.h>


typedef enum EncoderSwitchStates {
  ENCSW_STATE_00,
  ENCSW_STATE_01,
  ENCSW_STATE_10,
  ENCSW_STATE_11,
  ENCSW_STATE_UNKNOWN  
};

// Rotary Encoder Switch decoding class
// An object of this class accepts two digital input pins - pin1 and pin2,
// which are the two pins of the encoder. The update() method of the object
// must be called periodically. This method returns an indication corresponding
// encoder rotation direction (if any)
class EncoderSwitch
{
public:
  EncoderSwitch(int pin1, int pin2, bool invert);

  // Read the encoder pins and decode its rotation direction. Then
  // method returns +1 or -1 when a rotation step was detected 
  // (corresponding to the direction) and 0 if no motion was detected  
  int update();
  
protected:
  EncoderSwitchStates state;
  Bounce m_pin1;
  Bounce m_pin2;
  uint8_t m_mask;
};

#endif


