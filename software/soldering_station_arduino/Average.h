
#ifndef __AVERAGE_H__
#define __AVERAGE_H__

#include <inttypes.h>

// Implement moving window average algorithm.
// For efficiency reason, the window size is fixed at 8

class Average {
public:
  Average();

  void init(int initValue);
  
  void putValue(int value);
  
  int getAverage();
  
protected:
  uint8_t m_ptr;
  int m_data[8];
};

#endif

