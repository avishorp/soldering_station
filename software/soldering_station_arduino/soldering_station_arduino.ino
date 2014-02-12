#include <Arduino.h>
#include <TM1637Display.h>
#include <Bounce2.h>
#include "OnOffController.h"
#include "Average.h"
#include "EncoderSwitch.h"
#include "LinearEstimator.h"

// Pin Allocation
#define HEATER       6     // Heater control (on/off)
#define TSENSE       A0    // Temperature sense, analog input
#define DISP_CLK   2    // Display module clock
#define DISP_DIO   3    // Display module data
#define BTN_ONOFF 4   // On/Off Button
#define BTN_PRST1  A1   // Preset 1 Button
#define BTN_PRST2  A2   // Preset 2 Button
#define BTN_PRST3  A3   // Preset 3 Button
#define LED_HEAT   13  // Heater on indicator
#define LED_READY 8   // Ready LED
#define KNOB_1         9      // Setting knob input 1
#define KNOB_2         10      // Setting knob input 2

// General Settings
#define TEMPERATURE_MIN     150  // Min Setpoint Temperature
#define TEMPERATURE_MAX     400  // Max Setpoint Temperature
#define TEMPERATURE_STEP    5    // Adjustment step
#define TEMPERATURE_HOT     40   // The temperature below which the iron is
                                 // considered hot (and unsafe)
#define TEMPERATURE_FAULT   500  // The temperature above which a fault condition
                                 // is considered
#define UPDATE_INTERVAL     100  // Controller update interval (in mS)                              

//
// IronController
//
class IronController: public OnOffController
{
public:
  IronController(): OnOffController(0, 700, 0) {}
  
  virtual void heaterControl(bool on)
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
};

//
// TemperatureSetting
//
class TemperatureSetting: public EncoderSwitch
{
public:
  TemperatureSetting(): EncoderSwitch() {
    temperature = TEMPERATURE_MIN;
  }
  
  virtual void eventUp()
  {
    temperature += TEMPERATURE_STEP;
    if (temperature > TEMPERATURE_MAX)
      temperature = TEMPERATURE_MAX;
  }

  virtual void eventDown()
  {
    temperature -= TEMPERATURE_STEP;
    if (temperature < TEMPERATURE_MIN)
      temperature = TEMPERATURE_MIN;
  }
  
  unsigned int get()
  {
    return temperature;
  }
  
protected:
  unsigned int temperature;
  
};

//
// Constants
//
const uint8_t DISP_BLANK[] = {0, 0, 0, 0};       // Blank display
const uint8_t DISP_HOT[]  = {                    // hOt
  SEG_F | SEG_E | SEG_G | SEG_C,                 // h
  SEG_A | SEG_B | SEG_C | SEG_D | SEG_E | SEG_F, // O
  SEG_F | SEG_E | SEG_G | SEG_D };               // t
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
  bool btn1Changed;
  bool btn1Value;
  bool btn2Changed;
  bool btn2Value;
  bool btn3Changed;
  bool btn3Value;
} UpdateReport;

//
// Global Variables
//
SystemState systemState;
Average readingAvg;
TM1637Display display(DISP_CLK, DISP_DIO);
IronController controller;
Bounce encoderPin1;
Bounce encoderPin2;
Bounce buttonOnOff;
Bounce buttonPreset1;
Bounce buttonPreset2;
Bounce buttonPreset3;
TemperatureSetting temperatureSetpoint;
LinearEstimator analogToTempr;
unsigned int updateTime;
int subState;
UpdateReport updateReport;

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
  encoderPin1.attach(KNOB_1);
  encoderPin1.interval(5);
  encoderPin2.attach(KNOB_2);
  encoderPin2.interval(5);
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
  
  // Initial system state
  systemState = SYS_OFF;
  
  updateTime = 0;
    
}

void switchToOff()
{
  systemState = SYS_OFF;
  controller.setOnOffState(false);   
  subState = 0;
}

void switchToOn()
{
  systemState = SYS_ON;
  controller.setOnOffState(true);
  subState = 0;
  display.showNumberDec(temperatureSetpoint.get());
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
  // Update the encoder pin debouncers
  if (encoderPin1.update() ||  encoderPin2.update()) {
    uint8_t pin1 = (~encoderPin1.read()) & 0x01;
    uint8_t pin2 = (~encoderPin2.read()) & 0x01;    
    //digitalWrite(11, pin1);
    //digitalWrite(12, pin2);
    temperatureSetpoint.update(pin1, pin2);
    display.showNumberDec(temperatureSetpoint.get());
  }
  
  // Update and the the On/Off button state
  if (updateReport.btnOnOffChanged && updateReport.btnOnOffValue) {
      // On/Off button pressed, change to OFF state
      switchToOff();
      return;
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
  updateReport.btn1Changed = buttonPreset1.update();
  updateReport.btn1Value = !buttonPreset1.read();
  updateReport.btn2Changed = buttonPreset2.update();  
  updateReport.btn2Value = !buttonPreset1.read();
  updateReport.btn3Changed = buttonPreset3.update();  
  updateReport.btn3Value = !buttonPreset1.read();
  
  if (updateReport.btnOnOffChanged) {
    if (updateReport.btnOnOffValue)
      Serial.println("pushed");
    else
      Serial.println("released\n");
  }
  
  switch(systemState) {
  case SYS_ON:
    loopOn();
    break;
    
  case SYS_OFF:
    loopOff();
    break;
  };
}

