
#ifndef _ONOFFCONTROLLER_H_
#define _ONOFFCONTROLLER_H_

#include <inttypes.h>

typedef enum {
  OOCTL_OFF,       // Controller is off, no heating
  OOCTL_UNDER,     // Heating up
  OOCTL_STABLE,    // Stable temperature reached
  OOCTL_OVER,      // Overtemperature
  OOCTL_FAULT      // Fault
} ControllerState;

const char* controllerStateToStr(ControllerState st);

//
//  --------------|----*------|--------*----|---- .... ---|
//                          setpoint
//                |---margin--|---margin----|
//                     * = switch on point = setpoint - margin + undershoot
//                                     * = switch off point = setpoint + margin - overshoot
class OnOffController {
public:
  // Constructs an OnOffController object,
  // initalizes it and puts it in OFF state
  OnOffController(
    unsigned int margin,        // The margin, around setpoint, considered "stable"
    unsigned int maxValidValue, // Maximal valid temperature. If the reading is above
	                            // this number, the controller will turn off immediately
	unsigned int timeout        // The maximal time, in milliseconds, in which the heater must
	                            // react to on/off command. If this time elapses, fault
								// condition is entered
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
  
  // Wrapper to the heaterControl method
  void internalHeaterControl(bool on);
  
  unsigned int m_lastMeasure;
  unsigned int m_prevMeasure;
  unsigned int m_setpoint;
  unsigned int m_stableLow;
  unsigned int m_stableHigh;
  unsigned int m_maxValidValue;  
  ControllerState m_currentState;
  bool m_heaterState;
  unsigned int m_overshoot;
  unsigned int m_undershoot;
  unsigned int m_tempAtSwitch;
};

#endif // _ONOFFCONTROLLER_H_
