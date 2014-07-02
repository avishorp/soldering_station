#include <Arduino.h>
#include <TM1637Display.h>
#include <Bounce2.h>
#include <EEPROM.h>
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
#define TEMPERATURE_MAX     420  // Max Setpoint Temperature
#define TEMPERATURE_STEP    5    // Adjustment step
#define TEMPERATURE_HOT     40   // The temperature below which the iron is
                                 // considered hot (and unsafe)
#define TEMPERATURE_FAULT   500  // The temperature above which a fault condition
                                 // is considered
#define UPDATE_INTERVAL     100  // Controller update interval (in mS)    
#define PRESET_TIME        4000  // The button push time (in mS) required to set 
                                 // a preset
#define CAL_TIME           20000 // The time the "CAL" display is shown (counter cycles,
                                 // no specific units)
#define BRIGHTNESS_ON      15    // Display brightness (ON state)
#define BRIGHTNESS_OFF     8     // Display brightness (OFF state)

// Fault codes
#define FAULT_CALIBRATION    1   // No calibtartion data

// Calibration points
const uint16_t CAL_POINTS[] = { 650, 750, 850, 950 };
const uint8_t N_CAL_POINTS = sizeof(CAL_POINTS)/sizeof(uint16_t);

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
  SEG_F | SEG_E | SEG_G | SEG_D,
  0 };               // t
const uint8_t DISP_PRESET[] = {
  SEG_A | SEG_B | SEG_E | SEG_F | SEG_G,
  0, 0, 0 };
const uint8_t DISP_CAL[] = {
   SEG_A | SEG_D | SEG_E | SEG_F,                 // C
   SEG_A | SEG_B | SEG_C | SEG_E | SEG_F | SEG_G, // A
   SEG_D | SEG_E | SEG_F,                         // L
   0};
const uint8_t DISP_DONE[] = {
   SEG_B | SEG_C | SEG_D | SEG_E | SEG_G,         // d
   SEG_A | SEG_B | SEG_C | SEG_D | SEG_E | SEG_F, // O
   SEG_C | SEG_E | SEG_G,                         // n
   SEG_A | SEG_D | SEG_E | SEG_F | SEG_G          // E
   };   
const uint8_t DISP_FAULT[] = {
   0,                                             // (blank)
   SEG_A | SEG_E | SEG_F | SEG_G,                 // F
   0,                                             // (blank)
   0                                              // (blank)
   };

const uint8_t DISP_OFF[] = {
   SEG_A | SEG_B | SEG_C | SEG_D | SEG_E | SEG_F, // O
   SEG_A | SEG_E | SEG_F | SEG_G,                 // F
   SEG_A | SEG_E | SEG_F | SEG_G,                 // F
   0                                              // (blank)
   };
const uint8_t DISP_ALL_ON[] = {0xff, 0xff, 0xff, 0xff};

//
// Types
//
typedef enum {
  SYS_OFF,
  SYS_ON,
  SYS_CAL,
  SYS_FAULT
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

typedef struct {
  bool calValid;
  uint16_t calTemperatures[N_CAL_POINTS];
  uint16_t presetTemperature[3];
} SettingsType;

typedef struct {
  SettingsType data;
  uint16_t checksum;
} PersistentSettingsType;

const SettingsType defaultSettings = {
  0,               // calValid
  { 0, 0, 0, 0 },  // calTemperatures
  {280, 320, 350 } // presetTemperatures
};

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
int btnPushIndex;
unsigned int btnPushTime;
bool btnPushPresetElapsed;
uint8_t calIndex;
uint16_t calTemp;
PersistentSettingsType settings;


// Simple self test - turn all all the LEDs and segments for 2 seconds
void self_test()
{
  display.setSegments(DISP_ALL_ON);
  digitalWrite(LED_READY, HIGH);
  digitalWrite(LED_HEAT, HIGH);
  delay(2000);
}

uint16_t calcSettingsChecksum()
{
  // Calculates and returns the checksum of the settings
  uint8_t i;
  uint16_t cs = 0;
  uint8_t* data = (uint8_t*)(&settings.data);
  for(i=0; i < sizeof(SettingsType); i++) {
    cs += *data;
    cs <<= 1;
    data++;
  }
  
  return cs;
}


// Read the settings stored in the EEPROM. If no settings are stored,
// revert to the default
void readSettings()
{
  // Read the data from the EEPROM
  uint8_t i;
  uint8_t* data = (uint8_t*)&settings;
  for(i=0; i < sizeof(settings); i++) {
    *data = EEPROM.read(i);
    data++;
  }
    
  // Calculate the checksum and compare to the read value
  if (calcSettingsChecksum() != settings.checksum) {
    Serial.println("Invalid!");
    // Invalid data - copy the default values
    data = (uint8_t*)&settings;
    uint8_t* def = (uint8_t*)&defaultSettings;
    
    for(i=0; i < sizeof(settings); i++) {
      *data = *def;
      data++;
      def++;
    }

  }
}

// Store the settings in the EEPROM
void storeSettings()
{
  // Calculate the checksum
  settings.checksum = calcSettingsChecksum();
  
  // Write the data to the EEPROM
  uint8_t i;
  uint8_t* data = (uint8_t*)&settings;
  for(i=0; i < sizeof(settings); i++) {
    EEPROM.write(i, *data);
    data++;
  }
}

void setup()
{
  Serial.begin(115200);

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
  
  display.setBrightness(BRIGHTNESS_ON);
  
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
  
  readSettings();

  analogToTempr.linest(N_CAL_POINTS, (int*)&CAL_POINTS, (int*)&settings.data.calTemperatures);
  
  temperatureSetpoint = 250; // TODO: Read from EEPROM
  
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

void switchToFault(uint8_t code)
{
  systemState = SYS_FAULT;
  controller.setOnOffState(false);
  display.setSegments(DISP_FAULT);
  display.showNumberDec(code, false, 1, 2);
}

void switchToOn()
{
  // Check if the unit is calibrated
  if (!(settings.data.calValid)) {
    // Not calibrated - fault
    switchToFault(FAULT_CALIBRATION);
  }
  else {
    systemState = SYS_ON;
    controller.setOnOffState(true);
    subState = 0;
    display.setBrightness(BRIGHTNESS_ON);
    display.showNumberDec(temperatureSetpoint);
  }
}

void switchToCal()
{
  systemState = SYS_CAL;
  controller.setOnOffState(false);
  subState = 0;
  display.setSegments(DISP_CAL);
  btnPushTime = 0; // Variable reuse
  calIndex = 0;
}



void loopOff()
{
  if (updateReport.btnOnOffChanged && updateReport.btnOnOffValue) {
    // On/Off button was pressed. If PRESET3 button is also pressed,
    // we switch to calibtation mode
      if (updateReport.btnValue[2] == 1)
        switchToCal();
      else
        switchToOn();
        
    return;
  }

  if (updateReport.temperatureUpdate && (subState == 0)) {  
    // Check if the iron handle is too hot, and display "HOT" if
    // so. Otherwise display nothing
    if ((updateReport.temperature > TEMPERATURE_HOT) && (updateReport.temperature < TEMPERATURE_FAULT)) {
      display.setBrightness(BRIGHTNESS_ON);
      display.setSegments(DISP_HOT);
    }
    else {
      display.setBrightness(BRIGHTNESS_OFF);
      display.setSegments(DISP_OFF);
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
          settings.data.presetTemperature[btnPushIndex] = temperatureSetpoint;
          storeSettings();
          display.showNumberDec(temperatureSetpoint);
          btnPushPresetElapsed = false;
          btnPushIndex = -1;
        }
        else {
          // Released after short push - set the setpoint to the preset value
          temperatureSetpoint = settings.data.presetTemperature[i];
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

void loopCal()
{
 
  // At any stage, if the On/Off button is pressed,
  // the calibration process is aborted and the system
  // turns off
  if (updateReport.btnOnOffChanged && updateReport.btnOnOffValue) {
    switchToOff();
    return;
  }

  switch(subState) {
  case 0:
    btnPushTime++;
    
    // Display "CAL" for several seconds
    if (btnPushTime > CAL_TIME) {
      subState++;
      btnPushTime = 0; // Reuse as calibration point
      display.setSegments(DISP_BLANK);
      calTemp = 1000;
    }
    break;


  case 1:
    // Set temperature phase
    controller.setOnOffState(true);
    controller.setSetpoint(CAL_POINTS[calIndex]);
    display.setSegments(DISP_BLANK);
    display.showNumberDec(calIndex+1, false, 1, 0);
    subState = 2;
    break;
    
  case 2:
    // Heatup phase - wait until the temperature stabilizes
    if (controller.isStable()) {
      display.showNumberDec(calTemp);
      subState = 3;
    }
    break;
    
  case 3:
    // User input phase
    ///////////////////
    
    // Let the user change the temperature reading
    if (updateReport.knobValue != 0) {
      if ((updateReport.knobValue == 1) && (calTemp < TEMPERATURE_MAX)) {
        calTemp += TEMPERATURE_STEP;
        display.showNumberDec(calTemp);
      }
      else if ((updateReport.knobValue == -1) && (calTemp > 100)) {
        calTemp -= TEMPERATURE_STEP;
        display.showNumberDec(calTemp);
      }
    }
    
    // When done, the user should press the "Preset 3" button
    if ((updateReport.btnChanged[2] == 1) && (updateReport.btnValue[2] == 1)) {
      // Store the user input
      settings.data.calTemperatures[calIndex] = calTemp;
      
      if (calIndex == (N_CAL_POINTS-1)) {
        // Calibration done
        subState = 4;
      }
      else {
        // Next calibration point
        calIndex++;
        subState = 1;
      }
    }
    break;
    
  case 4:
    // Store Phase
    //////////////

    // Turn off the heating
    controller.setOnOffState(false  );
    
    // Display "DONE"
    display.setSegments(DISP_DONE);
    
    // Store the calibration data in the EEPROM
    settings.data.calValid = true;
    storeSettings();
    
    // Swith to the final state
    subState = 5;
    break;
    
  case 5:
    // End-of-process Phase
    ///////////////////////
    // Wait until user turns the unit off
    break;
  }
    
}

void loopFault()
{
 
  // Wait until the unit is turned off
  if (updateReport.btnOnOffChanged && updateReport.btnOnOffValue) 
    switchToOff();
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
    
/*
    // Report
    Serial.print(meas);
    Serial.print('\t');
    Serial.print(temperature);
    Serial.print('\t');
    Serial.print(controller.isHeaterOn());
    Serial.print('\t');
    Serial.println(controller.getSetpoint());
*/
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
	
  case SYS_CAL:
    loopCal();
    break;

  case SYS_FAULT:
    loopFault();
    break;
  };
}

