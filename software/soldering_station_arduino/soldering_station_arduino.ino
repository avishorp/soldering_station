#include <Arduino.h>
#include <TM1637Display.h>
#include <Bounce2.h>
#include "OnOffController.h"
#include "Average.h"
#include "EncoderSwitch.h"
#include "LinearEstimator.h"

// Pin Allocation
#define HEATER         6  // Heater control (on/off)
#define TSENSE         A0 // Temperature sense, analog input
#define DISP_CLK       2  // Display module clockw
#define DISP_DIO       3  // Display module data
#define BTN_ONOFF      9  // On/Off Button
#define BTN_PRST1      10 // Preset 1 Button
#define BTN_PRST2      7  // Preset 2 Button
#define BTN_PRST3      8  // Preset 3 Button
#define LED_HEAT       11 // Heater on indicator
#define LED_READY      12 // Ready LED
#define KNOB_1         5  // Setting knob input 1
#define KNOB_2         4  // Setting knob input 2

// General Settings
#define TEMPERATURE_MIN     150  // Min Setpoint Temperature
#define TEMPERATURE_MAX     400  // Max Setpoint Temperature
#define TEMPERATURE_STEP    5    // Adjustment step
#define TEMPERATURE_HOT     40   // The temperature below which the iron is
                                 // considered hot (and unsafe)
#define TEMPERATURE_FAULT   500  // The temperature above which a fault condition
                                 // is considered
#define UPDATE_INTERVAL     100  // Controller update interval (in mS)    
#define PRESET_TIME        4000  // The button push time (in mS) required to set 
                                 // a preset

//
// IronController
//
class IronController: public OnOffController
{
public:
  IronController(): OnOffController(0, 1000, 0) {}
  
  virtual void heaterControl(bool on)
  {
    if (on) {
      digitalWrite(LED_HEAT, HIGH);
      digitalWrite(HEATER, HIGH);
    }
    else {
      digitalWrite(LED_HEAT, LOW);
      digitalWrite(HEATER, LOW);
    }
  }
  
  virtual void stable(bool stab)
  {
    if (stab)
      digitalWrite(LED_READY, HIGH);
    else
      digitalWrite(LED_READY, LOW);
  }
};

//
// Constants
//
const uint8_t DISP_BLANK[] = {0, 0, 0, 0};       // Blank display
const uint8_t DISP_HOT[]  = {                    // hOt
  SEG_F | SEG_E | SEG_G | SEG_C,                 // h
  SEG_A | SEG_B | SEG_C | SEG_D | SEG_E | SEG_F, // O
  SEG_F | SEG_E | SEG_G | SEG_D };               // t
const uint8_t DISP_PRESET[] = {
  SEG_A | SEG_B | SEG_E | SEG_F | SEG_G,
  0, 0, 0 };
const uint8_t DISP_ALL_ON[] = {0xff, 0xff, 0xff, 0xff};

//
// Types
//
typedef enum {
  SYS_OFF,
  SYS_ON,
  SYS_CAL
} SystemState;

typedef struct {
  int temperature;
  bool temperatureUpdate;
  bool btnOnOffChanged;
  bool btnOnOffValue;
  bool btnChanged[3];
  bool btnValue[3];
  int  knobValue;
} UpdateReport;

//
// Global Variables
//
SystemState systemState;
Average readingAvg;
TM1637Display display(DISP_CLK, DISP_DIO);
IronController controller;
Bounce buttonOnOff;
Bounce buttonPreset1;
Bounce buttonPreset2;
Bounce buttonPreset3;
EncoderSwitch knobEncoder(KNOB_1, KNOB_2, true);
LinearEstimator analogToTempr;
unsigned int updateTime;
int subState;
UpdateReport updateReport;
int temperatureSetpoint;
int presetTemperature[3];
int btnPushIndex;
unsigned int btnPushTime;
bool btnPushPresetElapsed;

void self_test()
{
  display.setSegments(DISP_ALL_ON);
  digitalWrite(LED_READY, HIGH);
  digitalWrite(LED_HEAT, HIGH);
  delay(2000);
}

void setup()
{
  Serial.begin(9600);

  // Set up the I/O pin direction
  pinMode(HEATER, OUTPUT);
  pinMode(BTN_ONOFF, INPUT_PULLUP);
  pinMode(BTN_PRST1, INPUT_PULLUP);
  pinMode(BTN_PRST2, INPUT_PULLUP);
  pinMode(BTN_PRST3, INPUT_PULLUP);  
  pinMode(KNOB_1, INPUT_PULLUP);
  pinMode(KNOB_2, INPUT_PULLUP);
  pinMode(11, OUTPUT);
  pinMode(12, OUTPUT);
  pinMode(13, OUTPUT);
  
  readingAvg.init(analogRead(A0));
  
  display.setBrightness(15);
  
  controller.setSetpoint(550);
  
  // Fill the average window up
  int k;
  for(k=0; k < 10; k++) {
    int val = analogRead(A0);
    readingAvg.putValue(val);
    delay(50);
  }
  
  //
  controller.updateSamplingValue(readingAvg.getAverage());
  
  // Attach the debouncer objects to the encoder and button pins
  buttonOnOff.attach(BTN_ONOFF);
  buttonOnOff.interval(25);
  buttonPreset1.attach(BTN_PRST1);
  buttonPreset1.interval(5);
  buttonPreset2.attach(BTN_PRST2);
  buttonPreset2.interval(5);
  buttonPreset3.attach(BTN_PRST3);
  buttonPreset3.interval(5);  
  
  int xx[] = {550, 650, 750, 850};
  int yy[] = {167, 232, 295, 360};
  analogToTempr.linest(4, xx, yy);
  
  temperatureSetpoint = 250; // TODO: Read from EEPROM
  presetTemperature[0] = 280;
  presetTemperature[1] = 320;
  presetTemperature[2] = 355;
  
  // Initial system state
  systemState = SYS_OFF;
  
  updateTime = 0;
  
  digitalWrite(LED_READY, HIGH);
  
  self_test();
  
  // Initially, the unit is off
  switchToOff();
    
}

void switchToOff()
{
  systemState = SYS_OFF;
  controller.setOnOffState(false);   
  subState = 0;
  btnPushIndex = -1;
  btnPushPresetElapsed = false;
}

void switchToOn()
{
  systemState = SYS_ON;
  controller.setOnOffState(true);
  subState = 0;
  display.showNumberDec(temperatureSetpoint);
}

void loopOff()
{
  if (updateReport.btnOnOffChanged && updateReport.btnOnOffValue) {
    switchToOn();
    return;
  }

  if (updateReport.temperatureUpdate && (subState == 0)) {  
    // Check if the iron handle is too hot, and display "HOT" if
    // so. Otherwise display nothing
    if ((updateReport.temperature > TEMPERATURE_HOT) && (updateReport.temperature < TEMPERATURE_FAULT))
      display.setSegments(DISP_HOT);
    else {
      display.setSegments(DISP_BLANK);
      subState = 1;
    }
  }
  
}

void loopOn()
{
  bool tempChanged = false;
  int i;
  
  // Update and the the On/Off button state
  if (updateReport.btnOnOffChanged && updateReport.btnOnOffValue) {
      // On/Off button pressed, change to OFF state
      switchToOff();
      return;
  }  
  
  // Process knob changes
  if (updateReport.knobValue != 0) {
    if ((updateReport.knobValue == 1) && (temperatureSetpoint < TEMPERATURE_MAX))
      temperatureSetpoint += TEMPERATURE_STEP;
    else if ((updateReport.knobValue == -1) && (temperatureSetpoint > TEMPERATURE_MIN))
      temperatureSetpoint -= TEMPERATURE_STEP;
      
    tempChanged = true;
  }
  
  // Preset buttons
  if ((btnPushIndex >= 0) && (millis() > btnPushTime)) {
    // Preset button was pushed long enough to set the preset
    btnPushPresetElapsed = true;
    display.setSegments(DISP_PRESET);
  }
        
  for(i=0; i < 3; i++) {
    if (updateReport.btnChanged[i]) {
      // Button i has changed
     
      if (updateReport.btnValue[i]) {
        // Button pushed - record this fact and set the timer
        btnPushIndex = i;
        btnPushTime = millis() + PRESET_TIME;
      }
      else {
        // Button released
        if (btnPushPresetElapsed) {
          // Released after a long push - set the preset temperature
          presetTemperature[btnPushIndex] = temperatureSetpoint;
          display.showNumberDec(temperatureSetpoint);
          btnPushPresetElapsed = false;
          btnPushIndex = -1;
        }
        else {
          // Released after short push - set the setpoint to the preset value
          temperatureSetpoint = presetTemperature[i];
          tempChanged = true;
        }
      }
    }
  }
  
  // Update temperature if necessary
  if (tempChanged) {
    display.showNumberDec(temperatureSetpoint);
    int sp = analogToTempr.estimateX(temperatureSetpoint);
    controller.setSetpoint(sp); 
    btnPushIndex = -1;
  }
      
        
 
}

void loopCal(int temperature, bool update)
{
}

void loop()
{
  // Regular task - update the temperature reading every UPDATE_INTERVAL
  updateReport.temperatureUpdate = true;
  unsigned int now = millis();
  if (now > updateTime) {
    updateTime += UPDATE_INTERVAL;
  
    int val = analogRead(A0);
    readingAvg.putValue(val);
    int meas = readingAvg.getAverage();
    int temperature = analogToTempr.estimateY(meas);
    updateReport.temperature = temperature;
    updateReport.temperatureUpdate = true;
    controller.updateSamplingValue(meas);
  } 
  
  // Update the various debouncers
  updateReport.btnOnOffChanged = buttonOnOff.update();
  updateReport.btnOnOffValue = !buttonOnOff.read();
  updateReport.btnChanged[0] = buttonPreset1.update();
  updateReport.btnValue[0] = !buttonPreset1.read();
  updateReport.btnChanged[1] = buttonPreset2.update();  
  updateReport.btnValue[1] = !buttonPreset2.read();
  updateReport.btnChanged[2] = buttonPreset3.update();  
  updateReport.btnValue[2] = !buttonPreset3.read();
  updateReport.knobValue = knobEncoder.update();
   
  switch(systemState) {
  case SYS_ON:
    loopOn();
    break;
    
  case SYS_OFF:
    loopOff();
    break;
  };
}

