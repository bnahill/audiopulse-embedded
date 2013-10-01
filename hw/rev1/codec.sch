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
LIBS:special
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
LIBS:symbols
LIBS:audio_iface_rev1-cache
EELAYER 27 0
EELAYER END
$Descr A4 11693 8268
encoding utf-8
Sheet 3 5
Title ""
Date "1 oct 2013"
Rev ""
Comp ""
Comment1 ""
Comment2 ""
Comment3 ""
Comment4 ""
$EndDescr
$Comp
L AK4621 U?
U 1 1 523B94A4
P 6100 3300
F 0 "U?" H 6100 2250 60  0000 C CNN
F 1 "AK4621" H 6100 3850 60  0000 C CNN
F 2 "" H 6100 3300 60  0000 C CNN
F 3 "" H 6100 3300 60  0000 C CNN
	1    6100 3300
	1    0    0    -1  
$EndComp
Text GLabel 5250 3550 0    60   Input ~ 0
5V
$Comp
L AGND #PWR?
U 1 1 523B94AB
P 4950 3800
F 0 "#PWR?" H 4950 3800 40  0001 C CNN
F 1 "AGND" H 4950 3730 50  0000 C CNN
F 2 "" H 4950 3800 60  0000 C CNN
F 3 "" H 4950 3800 60  0000 C CNN
	1    4950 3800
	1    0    0    -1  
$EndComp
$Comp
L CAP C?
U 1 1 523B94B1
P 4850 3550
F 0 "C?" H 4870 3520 50  0000 L TNN
F 1 "0.1u" H 4875 3570 30  0000 L BNN
F 2 "" H 4850 3550 60  0000 C CNN
F 3 "" H 4850 3550 60  0000 C CNN
	1    4850 3550
	0    -1   -1   0   
$EndComp
$Comp
L CAP C?
U 1 1 523B94B7
P 4700 3550
F 0 "C?" H 4720 3520 50  0000 L TNN
F 1 "10u" H 4725 3570 30  0000 L BNN
F 2 "" H 4700 3550 60  0000 C CNN
F 3 "" H 4700 3550 60  0000 C CNN
	1    4700 3550
	0    -1   -1   0   
$EndComp
Wire Wire Line
	5250 3550 5500 3550
Wire Wire Line
	7300 2850 6700 2850
Wire Wire Line
	6700 2950 7300 2950
Wire Wire Line
	7300 3050 6700 3050
Wire Wire Line
	6700 3150 7300 3150
Wire Wire Line
	5500 3050 4100 3050
Wire Wire Line
	4100 2950 5500 2950
Wire Wire Line
	6700 3250 7800 3250
Wire Wire Line
	4950 3450 4950 3800
Wire Wire Line
	4950 3450 5500 3450
Wire Wire Line
	4700 3750 4950 3750
Wire Wire Line
	4700 3750 4700 3650
Connection ~ 4950 3750
Wire Wire Line
	4700 3450 4700 3350
Wire Wire Line
	4600 3350 5500 3350
Wire Wire Line
	4850 3350 4850 3450
Connection ~ 4850 3350
Wire Wire Line
	4850 3650 4850 3750
Connection ~ 4850 3750
Text GLabel 4600 3350 0    60   Input ~ 0
5V
Connection ~ 4700 3350
Text HLabel 4100 3150 0    60   Input ~ 0
AINL+
Text HLabel 4100 3250 0    60   Input ~ 0
AINL-
Text HLabel 4100 2950 0    60   Input ~ 0
AINR+
Text HLabel 4100 3050 0    60   Input ~ 0
AINR-
Wire Wire Line
	4100 3150 5500 3150
Wire Wire Line
	5500 3250 4100 3250
Text HLabel 7300 2850 2    60   Output ~ 0
AOUTR+
Text HLabel 7300 2950 2    60   Output ~ 0
AOUTR-
Text HLabel 7300 3050 2    60   Output ~ 0
AOUTL+
Text HLabel 7300 3150 2    60   Output ~ 0
AOUTL-
$Comp
L DGND #PWR?
U 1 1 523B95EE
P 7800 3350
F 0 "#PWR?" H 7800 3350 40  0001 C CNN
F 1 "DGND" H 7800 3280 40  0000 C CNN
F 2 "" H 7800 3350 60  0000 C CNN
F 3 "" H 7800 3350 60  0000 C CNN
	1    7800 3350
	1    0    0    -1  
$EndComp
Wire Wire Line
	7800 3250 7800 3350
Text GLabel 7300 3350 2    60   Input ~ 0
3V
Wire Wire Line
	7300 3350 6700 3350
Text GLabel 7100 3450 2    60   Input ~ 0
5V
Wire Wire Line
	7100 3450 6700 3450
$Comp
L CAP C?
U 1 1 523B9668
P 5200 2650
F 0 "C?" H 5220 2620 50  0000 L TNN
F 1 "0.1u" H 5225 2670 30  0000 L BNN
F 2 "" H 5200 2650 60  0000 C CNN
F 3 "" H 5200 2650 60  0000 C CNN
	1    5200 2650
	0    -1   -1   0   
$EndComp
$Comp
L CAP C?
U 1 1 523B9675
P 5000 2650
F 0 "C?" H 5020 2620 50  0000 L TNN
F 1 "10u" H 5025 2670 30  0000 L BNN
F 2 "" H 5000 2650 60  0000 C CNN
F 3 "" H 5000 2650 60  0000 C CNN
	1    5000 2650
	0    -1   -1   0   
$EndComp
Wire Wire Line
	5500 2850 5400 2850
Wire Wire Line
	5400 2850 5400 2450
Wire Wire Line
	5400 2450 5000 2450
Wire Wire Line
	5000 2450 5000 2550
Wire Wire Line
	5200 2550 5200 2450
Connection ~ 5200 2450
$Comp
L AGND #PWR?
U 1 1 523B96B7
P 5100 2850
F 0 "#PWR?" H 5100 2850 40  0001 C CNN
F 1 "AGND" H 5100 2780 50  0000 C CNN
F 2 "" H 5100 2850 60  0000 C CNN
F 3 "" H 5100 2850 60  0000 C CNN
	1    5100 2850
	1    0    0    -1  
$EndComp
Wire Wire Line
	5100 2850 5100 2800
Wire Wire Line
	5000 2800 5200 2800
Wire Wire Line
	5200 2800 5200 2750
Wire Wire Line
	5000 2800 5000 2750
Connection ~ 5100 2800
$EndSCHEMATC
