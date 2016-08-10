EESchema Schematic File Version 2
LIBS:power
LIBS:device
LIBS:transistors
LIBS:conn
LIBS:linear
LIBS:regul
LIBS:74xx
LIBS:cmos4000
LIBS:adc-dac
LIBS:memory
LIBS:xilinx
LIBS:microcontrollers
LIBS:dsp
LIBS:microchip
LIBS:analog_switches
LIBS:motorola
LIBS:texas
LIBS:intel
LIBS:audio
LIBS:interface
LIBS:digital-audio
LIBS:philips
LIBS:display
LIBS:cypress
LIBS:siliconi
LIBS:opto
LIBS:atmel
LIBS:contrib
LIBS:valves
LIBS:tactile_switch
LIBS:PowerButton-cache
EELAYER 25 0
EELAYER END
$Descr A4 11693 8268
encoding utf-8
Sheet 1 1
Title ""
Date ""
Rev ""
Comp ""
Comment1 ""
Comment2 ""
Comment3 ""
Comment4 ""
$EndDescr
$Comp
L Led_x2 D1
U 1 1 57A33552
P 4850 2850
F 0 "D1" H 4850 3075 50  0000 C CNN
F 1 "Led_x2" H 4850 2600 50  0000 C CNN
F 2 "LEDs:LED-5MM-3" H 4850 2850 50  0001 C CNN
F 3 "" H 4850 2850 50  0000 C CNN
	1    4850 2850
	1    0    0    -1  
$EndComp
$Comp
L CONN_01X04 P1
U 1 1 57A33580
P 6050 2900
F 0 "P1" H 6050 3150 50  0000 C CNN
F 1 "CONN_01X04" V 6150 2900 50  0000 C CNN
F 2 "PinHead1-4:PINHeader1-4" H 6050 2900 50  0001 C CNN
F 3 "" H 6050 2900 50  0000 C CNN
	1    6050 2900
	1    0    0    -1  
$EndComp
$Comp
L GND #PWR01
U 1 1 57A335D3
P 5750 3200
F 0 "#PWR01" H 5750 2950 50  0001 C CNN
F 1 "GND" H 5750 3050 50  0000 C CNN
F 2 "" H 5750 3200 50  0000 C CNN
F 3 "" H 5750 3200 50  0000 C CNN
	1    5750 3200
	1    0    0    -1  
$EndComp
Wire Wire Line
	5750 3200 5750 3050
Wire Wire Line
	5750 3050 5850 3050
Wire Wire Line
	4550 2850 4500 2850
Wire Wire Line
	4500 2850 4500 3000
$Comp
L Tactile_Switch K1
U 1 1 57A33639
P 5000 3650
F 0 "K1" H 5000 3400 60  0000 C CNN
F 1 "Tactile_Switch" H 5000 3950 60  0000 C CNN
F 2 "Buttons_Switches_ThroughHole:Push_E-Switch_KS01Q01" H 5050 3750 60  0001 C CNN
F 3 "" H 5050 3750 60  0000 C CNN
	1    5000 3650
	1    0    0    -1  
$EndComp
Connection ~ 5350 3550
Wire Wire Line
	4650 3550 4650 3850
Connection ~ 4650 3750
$Comp
L GND #PWR02
U 1 1 57A3369E
P 4650 3850
F 0 "#PWR02" H 4650 3600 50  0001 C CNN
F 1 "GND" H 4650 3700 50  0000 C CNN
F 2 "" H 4650 3850 50  0000 C CNN
F 3 "" H 4650 3850 50  0000 C CNN
	1    4650 3850
	1    0    0    -1  
$EndComp
$Comp
L GND #PWR03
U 1 1 57A336B8
P 4500 3000
F 0 "#PWR03" H 4500 2750 50  0001 C CNN
F 1 "GND" H 4500 2850 50  0000 C CNN
F 2 "" H 4500 3000 50  0000 C CNN
F 3 "" H 4500 3000 50  0000 C CNN
	1    4500 3000
	1    0    0    -1  
$EndComp
Wire Wire Line
	5150 2750 5250 2750
Wire Wire Line
	5250 2950 5150 2950
Wire Wire Line
	5350 3550 5650 3550
Wire Wire Line
	5350 3550 5350 3750
$Comp
L R R1
U 1 1 57A336E6
P 5400 2750
F 0 "R1" V 5480 2750 50  0000 C CNN
F 1 "220R" V 5400 2750 50  0000 C CNN
F 2 "Resistors_ThroughHole:Resistor_Horizontal_RM10mm" V 5330 2750 50  0001 C CNN
F 3 "" H 5400 2750 50  0000 C CNN
	1    5400 2750
	0    1    1    0   
$EndComp
$Comp
L R R2
U 1 1 57A33768
P 5400 2950
F 0 "R2" V 5480 2950 50  0000 C CNN
F 1 "220R" V 5400 2950 50  0000 C CNN
F 2 "Resistors_ThroughHole:Resistor_Horizontal_RM10mm" V 5330 2950 50  0001 C CNN
F 3 "" H 5400 2950 50  0000 C CNN
	1    5400 2950
	0    1    1    0   
$EndComp
Wire Wire Line
	5550 2750 5850 2750
Wire Wire Line
	5850 2850 5600 2850
Wire Wire Line
	5600 2850 5600 2950
Wire Wire Line
	5600 2950 5550 2950
Wire Wire Line
	5850 2950 5650 2950
Wire Wire Line
	5650 2950 5650 3550
$EndSCHEMATC
