
#ifndef __ENCODERSWITCH_H__
#define __ENCODERSWITCH_H__

#include <inttypes.h>

typedef enum EncoderSwitchStates {
  ENCSW_STATE_00,
  ENCSW_STATE_01,
  ENCSW_STATE_10,
  ENCSW_STATE_11,
  ENCSW_STATE_UNKNOWN  
};

// Rotary Encoder Switch decoding class
// An object of this class accepts two inputs - inp1 and inp2, which are
// the two pins of the encoder. The object should be updated with the
// update() method whenever a cheange in those pin occur (it may be called
// repeatadly with the same value even if there is no change). When a rotation
// step is detected, one of the methods eventUp() or eventDown() is called.
// These are abstract functions which must be implemented by the inheriting object
class EncoderSwitch
{
public:
  EncoderSwitch();
  
  void update(int inp1, int inp2);
  
  virtual void eventUp() = 0;
  
  virtual void eventDown() = 0;

protected:
  EncoderSwitchStates state;
};

#endif


