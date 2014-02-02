#include <Arduino.h>
#include <TM1637Display.h>
#include <Bounce2.h>
#include "OnOffController.h"
#include "Average.h"
#include "EncoderSwitch.h"

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

class IronOnOffController: public OnOffController
{
public:
  IronOnOffController(): OnOffController(0, 700, 0) {}
};

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

Average readingAvg;
TM1637Display display(DISP_CLK, DISP_DIO);
OnOffController controller(0, 1000, 0);
Bounce encoderPin1;
Bounce encoderPin2;
TemperatureSetting temperatureSetpoint;

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
  
  controller.setSetpoint(850);
  
  // Fill the average window up
  int k;
  for(k=0; k < 10; k++) {
    int val = analogRead(A0);
    readingAvg.putValue(val);
    delay(50);
  }
  
  //
  controller.updateSamplingValue(readingAvg.getAverage());
  controller.setOnOffState(true);
  
  // Attach the debouncer objects to the encoder pins
  encoderPin1.attach(KNOB_1);
  encoderPin1.interval(5);
  encoderPin2.attach(KNOB_2);
  encoderPin2.interval(5);
    
}

void loop()
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
  
/*  
    int val = analogRead(A0);
    readingAvg.putValue(val);
  
    int temp = readingAvg.getAverage();
    display.showNumberDec(temp);
    controller.updateSamplingValue(temp);

    Serial.print(controllerStateToStr(controller.getState()));  
    Serial.print(" ");
    Serial.println(temp);
*/
  
//    delay(100);
  
}

