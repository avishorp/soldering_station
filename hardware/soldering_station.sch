EESchema Schematic File Version 2
LIBS:power
LIBS:device
LIBS:texas
LIBS:linear
LIBS:Symbols_Socket-DIN41612_RevA
LIBS:xilinx
LIBS:video
LIBS:valves
LIBS:ttl_ieee
LIBS:transistors
LIBS:transf
LIBS:supertex
LIBS:stm32
LIBS:stm8
LIBS:special
LIBS:siliconi
LIBS:sensors
LIBS:relays
LIBS:regul
LIBS:references
LIBS:pspice
LIBS:powerint
LIBS:philips
LIBS:opto
LIBS:nxp_armmcu
LIBS:msp430
LIBS:motorola
LIBS:microcontrollers
LIBS:microchip_pic16mcu
LIBS:microchip_pic12mcu
LIBS:microchip_pic10mcu
LIBS:microchip1
LIBS:microchip
LIBS:memory
LIBS:logo
LIBS:interface
LIBS:intel
LIBS:graphic
LIBS:gennum
LIBS:ftdi
LIBS:elec-unifil
LIBS:dsp
LIBS:display
LIBS:digital-audio
LIBS:dc-dc
LIBS:cypress
LIBS:contrib
LIBS:conn
LIBS:cmos_ieee
LIBS:cmos4000
LIBS:brooktre
LIBS:audio
LIBS:atmel
LIBS:analog_switches
LIBS:adc-dac
LIBS:ac-dc
LIBS:74xx
LIBS:74xgxx
LIBS:arduino
LIBS:soldering_station-cache
EELAYER 27 0
EELAYER END
$Descr A4 11693 8268
encoding utf-8
Sheet 1 1
Title "Arduino Controlled Soldering Station"
Date "24 feb 2014"
Rev ""
Comp ""
Comment1 ""
Comment2 ""
Comment3 ""
Comment4 ""
$EndDescr
$Comp
L PNP Q2
U 1 1 530A7504
P 6600 2200
F 0 "Q2" H 7050 2100 60  0000 R CNN
F 1 "BC557" H 7150 2250 60  0000 R CNN
F 2 "~" H 6600 2200 60  0000 C CNN
F 3 "~" H 6600 2200 60  0000 C CNN
	1    6600 2200
	1    0    0    1   
$EndComp
$Comp
L LEMO5 K1
U 1 1 530A769B
P 5150 2950
F 0 "K1" H 5350 3250 70  0001 C CNN
F 1 "LEMO5" H 5450 2650 70  0001 C CNN
F 2 "" H 5150 2950 60  0000 C CNN
F 3 "" H 5150 2950 60  0000 C CNN
	1    5150 2950
	1    0    0    -1  
$EndComp
Wire Wire Line
	2850 2800 4700 2800
Wire Wire Line
	3650 3100 4700 3100
Text Label 4250 2800 0    60   ~ 0
HEATER1
Text Label 4250 3100 0    60   ~ 0
HEATER2
Wire Wire Line
	5600 2800 7450 2800
Wire Wire Line
	5600 3100 6350 3100
Text Label 5750 2800 0    60   ~ 0
SENSE1
Text Label 5750 3100 0    60   ~ 0
SENSE2
$Comp
L R R7
U 1 1 530A773C
P 6700 3050
F 0 "R7" H 6850 3150 40  0000 C CNN
F 1 "1K" H 6850 3050 40  0000 C CNN
F 2 "~" V 6630 3050 30  0000 C CNN
F 3 "~" H 6700 3050 30  0000 C CNN
	1    6700 3050
	1    0    0    -1  
$EndComp
Wire Wire Line
	6350 3100 6350 3300
Wire Wire Line
	6350 3300 6700 3300
Wire Wire Line
	6700 2800 6700 2400
$Comp
L R R6
U 1 1 530A779C
P 6700 1650
F 0 "R6" H 6850 1750 40  0000 C CNN
F 1 "100" H 6850 1650 40  0000 C CNN
F 2 "~" V 6630 1650 30  0000 C CNN
F 3 "~" H 6700 1650 30  0000 C CNN
	1    6700 1650
	1    0    0    -1  
$EndComp
Wire Wire Line
	6700 2000 6700 1900
$Comp
L LM6361 U2
U 1 1 530A77C8
P 5100 1800
F 0 "U2" H 5250 2100 60  0000 C CNN
F 1 "LM6361" H 5250 2000 60  0000 C CNN
F 2 "" H 5100 1800 60  0000 C CNN
F 3 "" H 5100 1800 60  0000 C CNN
	1    5100 1800
	1    0    0    -1  
$EndComp
$Comp
L R R5
U 1 1 530A77E0
P 6050 2200
F 0 "R5" V 6250 2200 40  0000 C CNN
F 1 "100" V 6150 2200 40  0000 C CNN
F 2 "~" V 5980 2200 30  0000 C CNN
F 3 "~" H 6050 2200 30  0000 C CNN
	1    6050 2200
	0    -1   -1   0   
$EndComp
Wire Wire Line
	6300 2200 6400 2200
Wire Wire Line
	5600 1800 5750 1800
Wire Wire Line
	5750 1800 5750 2200
Wire Wire Line
	5750 2200 5800 2200
Wire Wire Line
	6700 3300 6700 3600
Wire Wire Line
	5000 1400 5000 1250
Wire Wire Line
	3800 1250 7850 1250
Wire Wire Line
	6700 1250 6700 1400
Wire Wire Line
	6700 1900 5900 1900
Wire Wire Line
	5900 1900 5900 2050
Wire Wire Line
	5900 2050 5550 2050
Wire Wire Line
	5550 2050 5550 2450
Wire Wire Line
	5550 2450 4500 2450
Wire Wire Line
	4500 2450 4500 1900
Wire Wire Line
	4500 1900 4600 1900
Wire Wire Line
	5000 2200 5000 2250
$Comp
L GND #PWR5
U 1 1 530A7866
P 5000 2250
F 0 "#PWR5" H 5000 2250 30  0001 C CNN
F 1 "GND" H 5000 2180 30  0001 C CNN
F 2 "" H 5000 2250 60  0000 C CNN
F 3 "" H 5000 2250 60  0000 C CNN
	1    5000 2250
	1    0    0    -1  
$EndComp
$Comp
L GND #PWR8
U 1 1 530A7873
P 6700 3600
F 0 "#PWR8" H 6700 3600 30  0001 C CNN
F 1 "GND" H 6700 3530 30  0001 C CNN
F 2 "" H 6700 3600 60  0000 C CNN
F 3 "" H 6700 3600 60  0000 C CNN
	1    6700 3600
	1    0    0    -1  
$EndComp
$Comp
L R R3
U 1 1 530A788A
P 4250 1500
F 0 "R3" H 4400 1600 40  0000 C CNN
F 1 "620" H 4400 1500 40  0000 C CNN
F 2 "~" V 4180 1500 30  0000 C CNN
F 3 "~" H 4250 1500 30  0000 C CNN
	1    4250 1500
	1    0    0    -1  
$EndComp
$Comp
L R R4
U 1 1 530A7890
P 4250 2150
F 0 "R4" H 4400 2300 40  0000 C CNN
F 1 "4.7K" H 4400 2200 40  0000 C CNN
F 2 "~" V 4180 2150 30  0000 C CNN
F 3 "~" H 4250 2150 30  0000 C CNN
	1    4250 2150
	1    0    0    -1  
$EndComp
Connection ~ 5000 1250
Wire Wire Line
	4250 1750 4250 1900
Wire Wire Line
	4250 2400 4250 2500
$Comp
L GND #PWR4
U 1 1 530A78E5
P 4250 2500
F 0 "#PWR4" H 4250 2500 30  0001 C CNN
F 1 "GND" H 4250 2430 30  0001 C CNN
F 2 "" H 4250 2500 60  0000 C CNN
F 3 "" H 4250 2500 60  0000 C CNN
	1    4250 2500
	1    0    0    -1  
$EndComp
Wire Wire Line
	3900 1800 4400 1800
Wire Wire Line
	4400 1800 4400 1700
Wire Wire Line
	4400 1700 4600 1700
Connection ~ 4250 1800
$Comp
L C C2
U 1 1 530A7994
P 3900 2000
F 0 "C2" H 4000 1900 40  0000 L CNN
F 1 "0.1uF" H 3950 1850 40  0000 L CNN
F 2 "~" H 3938 1850 30  0000 C CNN
F 3 "~" H 3900 2000 60  0000 C CNN
	1    3900 2000
	1    0    0    -1  
$EndComp
Wire Wire Line
	3900 2200 3900 2400
Wire Wire Line
	3700 2400 4250 2400
$Comp
L LM6361 U4
U 1 1 530A79EB
P 7950 2900
F 0 "U4" H 8100 3200 60  0000 C CNN
F 1 "LM6361" H 8100 3100 60  0000 C CNN
F 2 "" H 7950 2900 60  0000 C CNN
F 3 "" H 7950 2900 60  0000 C CNN
	1    7950 2900
	1    0    0    -1  
$EndComp
Connection ~ 6700 2800
$Comp
L R R8
U 1 1 530A7A13
P 7450 3900
F 0 "R8" H 7600 3900 40  0000 C CNN
F 1 "62K" H 7600 3800 40  0000 C CNN
F 2 "~" V 7380 3900 30  0000 C CNN
F 3 "~" H 7450 3900 30  0000 C CNN
	1    7450 3900
	1    0    0    -1  
$EndComp
$Comp
L R R9
U 1 1 530A7A19
P 7950 3650
F 0 "R9" V 7850 3650 40  0000 C CNN
F 1 "350K" V 7750 3650 40  0000 C CNN
F 2 "~" V 7880 3650 30  0000 C CNN
F 3 "~" H 7950 3650 30  0000 C CNN
	1    7950 3650
	0    -1   -1   0   
$EndComp
Wire Wire Line
	7450 3000 7450 3650
Wire Wire Line
	7450 4150 7450 4350
Wire Wire Line
	7450 3650 7700 3650
Wire Wire Line
	8200 3650 8550 3650
Wire Wire Line
	8550 2900 8550 3700
Wire Wire Line
	8450 2900 9050 2900
Wire Wire Line
	7850 1250 7850 2500
Connection ~ 6700 1250
Wire Wire Line
	7850 3300 7850 3400
$Comp
L GND #PWR9
U 1 1 530A7B08
P 7450 4350
F 0 "#PWR9" H 7450 4350 30  0001 C CNN
F 1 "GND" H 7450 4280 30  0001 C CNN
F 2 "" H 7450 4350 60  0000 C CNN
F 3 "" H 7450 4350 60  0000 C CNN
	1    7450 4350
	1    0    0    -1  
$EndComp
$Comp
L GND #PWR10
U 1 1 530A7B0E
P 7850 3400
F 0 "#PWR10" H 7850 3400 30  0001 C CNN
F 1 "GND" H 7850 3330 30  0001 C CNN
F 2 "" H 7850 3400 60  0000 C CNN
F 3 "" H 7850 3400 60  0000 C CNN
	1    7850 3400
	1    0    0    -1  
$EndComp
Connection ~ 8550 2900
$Comp
L MOS_N Q1
U 1 1 530A7B70
P 3550 3400
F 0 "Q1" H 3950 3450 60  0000 R CNN
F 1 "IRF540N" H 4100 3350 60  0000 R CNN
F 2 "~" H 3550 3400 60  0000 C CNN
F 3 "~" H 3550 3400 60  0000 C CNN
	1    3550 3400
	1    0    0    -1  
$EndComp
Wire Wire Line
	3650 3100 3650 3200
Wire Wire Line
	3650 3600 3650 4250
$Comp
L R R1
U 1 1 530A7BD9
P 3000 3400
F 0 "R1" V 3200 3400 40  0000 C CNN
F 1 "10K" V 3100 3400 40  0000 C CNN
F 2 "~" V 2930 3400 30  0000 C CNN
F 3 "~" H 3000 3400 30  0000 C CNN
	1    3000 3400
	0    -1   -1   0   
$EndComp
$Comp
L R R2
U 1 1 530A7BDF
P 3250 3800
F 0 "R2" H 3100 3850 40  0000 C CNN
F 1 "1M" H 3100 3750 40  0000 C CNN
F 2 "~" V 3180 3800 30  0000 C CNN
F 3 "~" H 3250 3800 30  0000 C CNN
	1    3250 3800
	1    0    0    -1  
$EndComp
Wire Wire Line
	3250 3400 3350 3400
Wire Wire Line
	3250 3400 3250 3550
Wire Wire Line
	3250 4050 3250 4150
Wire Wire Line
	3250 4150 3650 4150
Connection ~ 3650 4150
$Comp
L GND #PWR2
U 1 1 530A7CA9
P 3650 4250
F 0 "#PWR2" H 3650 4250 30  0001 C CNN
F 1 "GND" H 3650 4180 30  0001 C CNN
F 2 "" H 3650 4250 60  0000 C CNN
F 3 "" H 3650 4250 60  0000 C CNN
	1    3650 4250
	1    0    0    -1  
$EndComp
Wire Wire Line
	1950 3400 2750 3400
$Comp
L 7805 U1
U 1 1 530A7D57
P 3400 1300
F 0 "U1" H 3400 1600 60  0000 C CNN
F 1 "7805" H 3400 1500 60  0000 C CNN
F 2 "~" H 3400 1300 60  0000 C CNN
F 3 "~" H 3400 1300 60  0000 C CNN
	1    3400 1300
	1    0    0    -1  
$EndComp
Connection ~ 4250 1250
Wire Wire Line
	3400 1550 3400 1800
Wire Wire Line
	2850 2800 2850 1250
Wire Wire Line
	2400 1250 3000 1250
Connection ~ 2850 1250
$Comp
L CONN_2 P1
U 1 1 530A7E46
P 2050 1350
F 0 "P1" V 2000 1350 40  0000 C CNN
F 1 "CONN_2" V 2100 1350 40  0000 C CNN
F 2 "" H 2050 1350 60  0000 C CNN
F 3 "" H 2050 1350 60  0000 C CNN
	1    2050 1350
	-1   0    0    -1  
$EndComp
Wire Wire Line
	2400 1450 2400 1950
Wire Wire Line
	3400 1800 2400 1800
Connection ~ 2400 1800
$Comp
L GND #PWR1
U 1 1 530A7F3E
P 2400 1950
F 0 "#PWR1" H 2400 1950 30  0001 C CNN
F 1 "GND" H 2400 1880 30  0001 C CNN
F 2 "" H 2400 1950 60  0000 C CNN
F 3 "" H 2400 1950 60  0000 C CNN
	1    2400 1950
	1    0    0    -1  
$EndComp
Text Label 2000 3400 0    60   ~ 0
HEATER_CTRL
$Comp
L C C3
U 1 1 530A8059
P 8550 3900
F 0 "C3" H 8750 3950 40  0000 L CNN
F 1 "0.1uF" H 8750 3850 40  0000 L CNN
F 2 "~" H 8588 3750 30  0000 C CNN
F 3 "~" H 8550 3900 60  0000 C CNN
	1    8550 3900
	1    0    0    -1  
$EndComp
Connection ~ 8550 3650
Wire Wire Line
	8550 4100 8550 4250
Wire Wire Line
	8550 4250 7450 4250
Connection ~ 7450 4250
Text Label 8550 2900 0    60   ~ 0
TEMP_SENSE
$Comp
L ARDUINO_MINI U3
U 1 1 530BB2F1
P 5000 5550
F 0 "U3" H 5500 4600 70  0001 C CNN
F 1 "ARDUINO" H 5750 4500 70  0000 C CNN
F 2 "DIL20" H 5000 5500 60  0000 C CNN
F 3 "" H 5000 5550 60  0000 C CNN
	1    5000 5550
	1    0    0    -1  
$EndComp
Wire Wire Line
	5700 6150 6400 6150
Wire Wire Line
	5700 6050 6400 6050
Text Label 5800 6150 0    60   ~ 0
DISP_CLK
Text Label 5800 6050 0    60   ~ 0
DISP_DATA
Wire Wire Line
	5700 5950 6400 5950
Wire Wire Line
	5700 5850 6400 5850
Text Label 5800 5950 0    60   ~ 0
BTN_ONOFF
Wire Wire Line
	5700 5600 6400 5600
Text Label 5800 5600 0    60   ~ 0
HEATER_CTRL
Wire Wire Line
	5700 5500 6400 5500
Wire Wire Line
	5700 5400 6400 5400
Wire Wire Line
	5700 5300 6400 5300
Text Label 5800 5500 0    60   ~ 0
BTN_PRST1
Text Label 5800 5400 0    60   ~ 0
BTN_PRST2
Text Label 5800 5300 0    60   ~ 0
BTN_PRST3
Wire Wire Line
	5700 5200 6400 5200
Wire Wire Line
	5700 5100 6400 5100
Text Label 5800 5200 0    60   ~ 0
KNOB_1
Text Label 5800 5100 0    60   ~ 0
KNOB_2
Wire Wire Line
	5700 5000 6400 5000
Text Label 5800 5000 0    60   ~ 0
LED_STABLE
Text Label 3550 5350 0    60   ~ 0
TEMP_SENSE
Wire Wire Line
	4300 5450 3500 5450
Text Label 3550 5450 0    60   ~ 0
LED_HEATER
Wire Wire Line
	4300 5350 3500 5350
Wire Wire Line
	4250 1250 4250 1000
$Comp
L +5V #PWR3
U 1 1 530BBD66
P 4250 1000
F 0 "#PWR3" H 4250 1090 20  0001 C CNN
F 1 "+5V" H 4250 1090 30  0000 C CNN
F 2 "" H 4250 1000 60  0000 C CNN
F 3 "" H 4250 1000 60  0000 C CNN
	1    4250 1000
	1    0    0    -1  
$EndComp
$Comp
L +5V #PWR6
U 1 1 530BBD7D
P 5000 4250
F 0 "#PWR6" H 5000 4340 20  0001 C CNN
F 1 "+5V" H 5000 4340 30  0000 C CNN
F 2 "" H 5000 4250 60  0000 C CNN
F 3 "" H 5000 4250 60  0000 C CNN
	1    5000 4250
	1    0    0    -1  
$EndComp
Wire Wire Line
	5000 4400 5000 4250
Wire Wire Line
	5000 7100 5000 7250
$Comp
L GND #PWR7
U 1 1 530BBE7F
P 5000 7250
F 0 "#PWR7" H 5000 7250 30  0001 C CNN
F 1 "GND" H 5000 7180 30  0001 C CNN
F 2 "" H 5000 7250 60  0000 C CNN
F 3 "" H 5000 7250 60  0000 C CNN
	1    5000 7250
	1    0    0    -1  
$EndComp
$Comp
L CAPAPOL C1
U 1 1 530BBFFB
P 3800 1450
F 0 "C1" H 3850 1550 40  0000 L CNN
F 1 "10uF" H 3850 1350 40  0000 L CNN
F 2 "~" H 3900 1300 30  0000 C CNN
F 3 "~" H 3800 1450 300 0000 C CNN
	1    3800 1450
	1    0    0    -1  
$EndComp
Wire Wire Line
	3800 1650 3700 1650
Wire Wire Line
	3700 1650 3700 2400
Connection ~ 3900 2400
$Comp
L CONNECTOR P?
U 1 1 530BC52A
P 5350 3650
F 0 "P?" H 5700 3750 70  0001 C CNN
F 1 "CONNECTOR" H 5700 3550 70  0001 C CNN
F 2 "~" H 5350 3650 60  0000 C CNN
F 3 "~" H 5350 3650 60  0000 C CNN
	1    5350 3650
	1    0    0    -1  
$EndComp
Wire Wire Line
	5150 3400 5150 3800
Wire Wire Line
	5150 3650 5350 3650
Connection ~ 5150 3650
$Comp
L GNDPWR #PWR?
U 1 1 530BC5F3
P 5150 3800
F 0 "#PWR?" H 5150 3850 40  0001 C CNN
F 1 "GNDPWR" H 5150 3720 40  0001 C CNN
F 2 "" H 5150 3800 60  0000 C CNN
F 3 "" H 5150 3800 60  0000 C CNN
	1    5150 3800
	1    0    0    -1  
$EndComp
$EndSCHEMATC
