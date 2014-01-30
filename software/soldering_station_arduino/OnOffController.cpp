  
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

const char* controllerStateToStr(ControllerState st)
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


OnOffController::OnOffController(unsigned int lowMargin, unsigned int maxValidValue)
{
  // Initialize the controller state
  m_maxValidValue = maxValidValue;
  m_currentState = OOCTL_OFF;
  m_heaterState = false;
  m_overshoot = 0;
  m_undershoot = 0;
  m_tempAtSwitch = 0;
  
  // Turn the heater off
  
  internalHeaterControl(false);
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
  makeControlDecision();
}
  
void OnOffController::updateSamplingValue(int value)
{
  m_prevMeasure = m_lastMeasure;
  m_lastMeasure = value;
  
  // Calculate overshoot/undershoot
  if (m_heaterState == true && (m_lastMeasure < m_prevMeasure) && (m_lastMeasure < m_tempAtSwitch)) {
    // When the heater is on, but the temperature is still
	// lowering, wer'e in undershoot
	m_undershoot = m_tempAtSwitch - m_lastMeasure;
  }
  if (m_heaterState == false && (m_lastMeasure > m_prevMeasure) && (m_lastMeasure > m_tempAtSwitch)) {
    // When the heater is off, but the temperature is still
	// rising, wer'e in overshoot
	m_overshoot = m_lastMeasure - m_tempAtSwitch;
  }
  
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
	m_currentState = OOCTL_UNDER;
        m_overshoot = 0;
        m_undershoot = 0;
      
	makeControlDecision();
  }
  else {
    // Turn controller off
	m_currentState = OOCTL_OFF;
	internalHeaterControl(false);
  }
}

void OnOffController::makeControlDecision()
{

  if (m_lastMeasure > m_maxValidValue) {
	// Emergency stop - turn off the heater and the controller
	internalHeaterControl(false);
	m_currentState = OOCTL_OFF;
	return;
  }
  else if (m_currentState != OOCTL_OFF) {
    // Decide whether to switch the heater on or off
	if (m_lastMeasure > (m_stableHigh - m_overshoot)) 
		internalHeaterControl(false);
    else if (m_lastMeasure < (m_stableLow + m_undershoot)) 
		internalHeaterControl(true);
		
	// Determine the state
	if (m_lastMeasure < m_stableLow)
	  m_currentState = OOCTL_UNDER;
    else if (m_lastMeasure < m_stableHigh)
	  m_currentState = OOCTL_STABLE;
	else
      m_currentState = OOCTL_UNDER;
	
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

void OnOffController::internalHeaterControl(bool on)
{
	if (on != m_heaterState) {
		// This is a switch
		m_tempAtSwitch = m_lastMeasure;
	}
	
	m_heaterState = on;
	
	// Call the actual working function
	heaterControl(on);
}




