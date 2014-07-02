

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


// Controller Fault Conditions
#define FAULT_NONE         0
#define FAULT_NOT_HEATING  1
#define FAULT_NOT_COOLING  2
#define FAULT_OVERTEMP     4

// A simple implementation of an on/off temperature controller (aka Thermostat)
//
// An object of this class turns on and off a heating element trying to keep
// the measured temperature within a given margin from a setpoint. The heater
// is controlled via the heaterControl() method, which must be implemented to
// to drive the actual heating element.
// The algorithm detects three fault cases:
//
// FAULT_OVERTEMP - Overtemperature (beyond the given safety limit)
// FAULT_NOT_HEATING - Not heating as expected, might be caused by a faulty
//                     heating element or power system
// FAULT_NOT_COOLING - Not cooling as expected, suggests that the heating
//                     element heats uncontrollably. MIGHT BE DANGEROUS!
//
//  --------------|----*------|--------*----|---- .... ---|---> Temperature
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
  
  // Gets the setpoint value
  int getSetpoint() const;
  
  // Update the controller with a new sensor data value
  void updateSamplingValue(int value);
  
  // Check whether the heater is turned on
  bool isHeaterOn() const;
  
  // Get the current state of the controller
  ControllerState getState() const;
  
  // Turn the power on (controller workign) or off (heater always off)
  void setOnOffState(bool on);
  
  // Get the current fault code
  int getFaultCode() { return m_faultCode; }
  
  // Heater control callback, The function will be called when
  // the heater should be turned on or off. It must be overridden by
  // the user to imlement the physical operation.
  virtual void heaterControl(bool on) = 0;
  
  // Stability callback - called when the controller goes in/out
  // the stable state
  virtual void stable(bool stab) {}
  
  // Fault callback - will be called by the controller in case
  // of a fault condition
  virtual void fault(int faultCode) {}

  bool isStable() const 
    { return (getState() ==  OOCTL_STABLE); }
  
protected: 
  // Calculate the current state and decide
  // whether to turn the heater on or off
  void makeControlDecision();
  
  // Switch the controller into fault state
  void switchToFault(int faultCode);
  
  // Wrapper to the heaterControl method
  void internalHeaterControl(bool on);

  // Convert the state to string (for debugging)  
  const char* controllerStateToStr(ControllerState st);
  

  
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
  bool m_enableXshoot;
  unsigned int m_xshootStartTime;
  unsigned int m_xshootTimeout;
  int m_faultCode;
};

#endif // _ONOFFCONTROLLER_H_
