
#ifndef _ONOFFCONTROLLER_H_
#define _ONOFFCONTROLLER_H_

#include <inttypes.h>

typedef enum {
  OOCTL_OFF,             // Controller is off, no heating
  OOCTL_HEATING,   // Heating up
  OOCTL_STABLE,     // Stable temperature reached
  OOCTL_OVER           // Overtemperature
} ControllerState;

const char* controllerStateToStr(ControllerState st);

class OnOffController {
public:
  // Constructs an OnOffController object,
  // initalizes it and puts it in OFF state
  OnOffController(
    unsigned int lowMargin,
    unsigned int maxValidValue
    );
  
  // Set the desired temperature setpoint
  void setSetpoint(int value);
  
  // Update the controller with a new sensor data value
  void updateSamplingValue(int value);
  
  // Check whether the heater is turned on
  bool isHeaterOn();
  
  // Get the current state of the controller
  ControllerState getState();
  
  // Turn the power on (controller workign) or off (heater always off)
  void setOnOffState(bool on);
  
  // Heater control callback, The function will be called when
  // the heater should be turned on or off. It must be overridden by
  // the user to imlement the physical operation.
  void heaterControl(bool on);
  
protected: 
  // Calculate the current state and decide
  // whether to turn the heater on or off
  void makeControlDecision();
  
  unsigned int m_lastMeasure;
  unsigned int m_setpoint;
  unsigned int m_stableLow;
  unsigned int m_stableHigh;
  unsigned int m_tripPoint;
  unsigned int m_maxValidValue;  
  ControllerState m_currentState;
  bool m_heaterState;
};

#endif // _ONOFFCONTROLLER_H_
