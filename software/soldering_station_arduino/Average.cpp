  
  extern "C" {
  #include <stdlib.h>
  #include <string.h>
  #include <inttypes.h>
}

  #include "Average.h"
  
  Average::Average()
  {   
    m_ptr = 0;
  }
  
  void Average::init(int initValue)
  {
    uint8_t k;
    for(k = 0; k < 16; k++)
      m_data[k] = initValue;
  }
  
  void Average::putValue(int value)
  {
    m_data[m_ptr] = value;
    m_ptr = (m_ptr + 1) & 0x0f;
  }
  
  int Average::getAverage()
  {
    uint8_t k;
    int sum;
    
    for(k=0; k < 16; k++)
      sum += m_data[k];

   return (sum >> 4);
  }
  
  
  

