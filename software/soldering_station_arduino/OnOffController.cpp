  
  extern "C" {
  #include <stdlib.h>
  #include <string.h>
  #include <inttypes.h>
}
#include <Arduino.h>

#include "OnOffController.h"

const char* _controllerStateStr[] = {
  "OFF",
  "HEATING",
  "STABLE",
  "OVER"
};

const char* controllerStateToStr(ControllerState st)
{
  switch(st) {
  case OOCTL_OFF:
    return _controllerStateStr[0];

  case OOCTL_HEATING:
    return _controllerStateStr[1];

  case OOCTL_STABLE:
    return _controllerStateStr[2];

  case OOCTL_OVER:
    return _controllerStateStr[3];   
  }
  
  return "";
}


OnOffController::OnOffController(unsigned int lowMargin, unsigned int maxValidValue)
{
  // Initialize the controller state
  m_maxValidValue = maxValidValue;
  m_currentState = OOCTL_OFF;
  m_heaterState = false;
  
  // Turn the heater off
  
  heaterControl(false);
}
  
void OnOffController::setSetpoint(int value)
{
  // Make sure the setpoint is valid
  if (value > m_maxValidValue)
    return;
    
  m_setpoint = value;
  m_stableLow = value - 10;
  m_stableHigh = value + 40;
  m_tripPoint = value - 20;
  makeControlDecision();
}
  
void OnOffController::updateSamplingValue(int value)
{
  m_lastMeasure = value;
  makeControlDecision();
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
  if (on) {
    // Turn controller on
	m_currentState = OOCTL_HEATING;
	makeControlDecision();
  }
  else {
    // Turn controller off
	m_currentState = OOCTL_OFF;
	heaterControl(false);
  }
}

void OnOffController::makeControlDecision()
{

  if (m_lastMeasure > m_maxValidValue) {
	// Emergency stop - turn off the heater and the controller
	heaterControl(false);
	m_currentState = OOCTL_OFF;
	return;
  }
  else if (m_currentState != OOCTL_OFF) {
	if (m_lastMeasure > m_stableHigh) {
	  // Overtemperature
	  if (m_currentState != OOCTL_OVER) {
		heaterControl(false);
		m_currentState = OOCTL_OVER;
	  }
    }
    else if (m_lastMeasure < m_tripPoint) {

	  // Temperature too low (may be stable or not)
	  heaterControl(true);
	
	  // Check if we are in the stable range
	  if (m_lastMeasure >= m_stableLow)
		m_currentState = OOCTL_STABLE;
	  else
		m_currentState = OOCTL_HEATING;
	}
  }
}

void OnOffController::heaterControl(bool on)
{
    if (on) {
      digitalWrite(13, HIGH);
      digitalWrite(6, HIGH);
    }
    else {
      digitalWrite(13, LOW);
      digitalWrite(6, LOW);
    }
}



