  
  extern "C" {
  #include <stdlib.h>
  #include <string.h>
  #include <inttypes.h>
}
#include <Arduino.h>

#include "OnOffController.h"

const char* _controllerStateStr[] = {
  "OFF",
  "UNDER",
  "STABLE",
  "OVER",
  "FAULT"
};

const char* OnOffController::controllerStateToStr(ControllerState st)
{
  switch(st) {
  case OOCTL_OFF:
    return _controllerStateStr[0];

  case OOCTL_UNDER:
    return _controllerStateStr[1];

  case OOCTL_STABLE:
    return _controllerStateStr[2];

  case OOCTL_OVER:
    return _controllerStateStr[3];

  case OOCTL_FAULT:
    return _controllerStateStr[4];
  }
  
  return "";
}


OnOffController::OnOffController(unsigned int lowMargin, unsigned int maxValidValue, unsigned int timeout)
{
  // Initialize the controller state
  m_maxValidValue = maxValidValue;
  m_currentState = OOCTL_OFF;
  m_heaterState = false;
  m_overshoot = 0;
  m_undershoot = 0;
  m_tempAtSwitch = 0;
  m_enableXshoot = false;
  m_xshootTimeout = timeout;
  m_faultCode = FAULT_NONE;
  
  // Turn the heater off
  internalHeaterControl(false);
  
  stable(false);
}
  
void OnOffController::setSetpoint(int value)
{
  // Make sure the setpoint is valid
  if (value > m_maxValidValue)
    return;
    
  m_setpoint = value;
  int margin = 20;
  m_stableLow = value - margin;
  m_stableHigh = value + margin;
  m_overshoot = 0;
  m_undershoot = 0;
  
  // Temporarily disable over/undershoot measurment to prevent
  // false results due to the setpoint change
  m_enableXshoot = false;
  
  makeControlDecision();
}
  
void OnOffController::updateSamplingValue(int value)
{
  m_prevMeasure = m_lastMeasure;
  m_lastMeasure = value;
  
  unsigned int now = millis();
  
  // Calculate overshoot/undershoot
  if (m_enableXshoot && m_heaterState == true && (m_lastMeasure < m_prevMeasure) && (m_lastMeasure < m_tempAtSwitch)) {
    // When the heater is on, but the temperature is still
	// decliningg, wer'e in undershoot
	m_undershoot = m_tempAtSwitch - m_lastMeasure;

    // Check that wer'e not in undeshoot too much time
    if ((now - m_xshootStartTime) > m_xshootTimeout)
      switchToFault(FAULT_NOT_HEATING);
  }
  if (m_enableXshoot && m_heaterState == false && (m_lastMeasure > m_prevMeasure) && (m_lastMeasure > m_tempAtSwitch)) {
    // When the heater is off, but the temperature is still
	// rising, wer'e in overshoot
	m_overshoot = m_lastMeasure - m_tempAtSwitch;

      // Check that wer'e not in undeshoot too much time
      if ((now - m_xshootStartTime) > m_xshootTimeout)
        switchToFault(FAULT_NOT_COOLING);

  }
  
  makeControlDecision();

#ifdef DEBUG_CONTROLLER
  Serial.print(now);
  Serial.print('\t');
  Serial.print(value);
  Serial.print('\t');
  Serial.print(m_setpoint);
  Serial.print('\t');
  Serial.print(m_overshoot);
  Serial.print('\t');
  Serial.print(m_undershoot);
  Serial.print('\t');
  Serial.print(controllerStateToStr(m_currentState));
  Serial.print('\t');
  if (m_heaterState)
    Serial.println("on");
  else
    Serial.println("off"); 
#endif
}
  
bool OnOffController::isHeaterOn()
{
}
  
ControllerState OnOffController::getState()
{
  return m_currentState;
}
  
void OnOffController::setOnOffState(bool on)
{
  if (on && (m_currentState != OOCTL_FAULT)) {
    // Turn controller on
	m_currentState = OOCTL_UNDER;
        m_overshoot = 0;
        m_undershoot = 0;
      
	makeControlDecision();
  }
  else {
    // Turn controller off
	m_currentState = OOCTL_OFF;
        m_faultCode = FAULT_NONE;
	internalHeaterControl(false);
        stable(false);
  }
}

void OnOffController::makeControlDecision()
{

  if (m_lastMeasure > m_maxValidValue) {
	// Emergency stop - turn off the heater and the controller
        switchToFault(FAULT_OVERTEMP);
        return;
  }
  else if ((m_currentState != OOCTL_OFF) && (m_currentState != OOCTL_FAULT)) {
    // Decide whether to switch the heater on or off
	if (m_lastMeasure > (m_stableHigh - m_overshoot)) 
		internalHeaterControl(false);
    else if (m_lastMeasure < (m_stableLow + m_undershoot)) 
		internalHeaterControl(true);
		
    // Determine the state
    if (m_lastMeasure < m_stableLow) {
      m_currentState = OOCTL_UNDER;
      stable(false);
    }
    else if (m_lastMeasure < m_stableHigh) {
      m_currentState = OOCTL_STABLE;
      stable(true);
    }
    else {
      m_currentState = OOCTL_OVER;
      stable(false);
    }	
  }
}


void OnOffController::internalHeaterControl(bool on)
{
	if (on != m_heaterState) {
		// This is a switch
		m_tempAtSwitch = m_lastMeasure;
                m_enableXshoot = true;
                m_xshootStartTime = millis();
	}
	
	m_heaterState = on;
	
	// Call the actual working function
	heaterControl(on);
}

void OnOffController::switchToFault(int faultCode)
{
  /*
  // First of all, turn heater off (directly, without
  // going through the over/undershoot stuff)
  heaterControl(false);
  m_heaterState = false;
  
  // Set the current state to FAULT
  m_currentState = OOCTL_FAULT;
  
  // Set the fault code
  m_faultCode = faultCode;
  
  // Notify wer'e not in a stable state
  stable(false);
  */
}





