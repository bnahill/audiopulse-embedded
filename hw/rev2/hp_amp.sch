EESchema Schematic File Version 2
LIBS:symbols
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
EELAYER 25 0
EELAYER END
$Descr A 11000 8500
encoding utf-8
Sheet 6 6
Title "Android DPOAE Interface"
Date "13 may 2015"
Rev "2b"
Comp "Sana AudioPulse"
Comment1 "Ben Nahill <bnahill@gmail.com>"
Comment2 ""
Comment3 ""
Comment4 ""
$EndDescr
Text HLabel 1100 3700 0    60   Input ~ 0
INL+
Text HLabel 1100 2700 0    60   Input ~ 0
INL-
Text HLabel 1100 5700 0    60   Input ~ 0
INR-
Text HLabel 1100 4700 0    60   Input ~ 0
INR+
$Comp
L OPAMP_QUAD U7
U 3 1 534EF55D
P 2400 2600
F 0 "U7" H 2475 2450 60  0000 L CNN
F 1 "LTC6242" H 2450 2750 60  0000 L CNN
F 2 "lib:LT_DFN16_QUAD" H 2400 2600 60  0001 C CNN
F 3 "~" H 2400 2600 60  0000 C CNN
	3    2400 2600
	1    0    0    -1  
$EndComp
$Comp
L OPAMP_QUAD U7
U 5 1 534EF5CB
P 3050 7050
F 0 "U7" H 3125 6900 60  0000 L CNN
F 1 "LTC6242" H 3100 7200 60  0000 L CNN
F 2 "lib:LT_DFN16_QUAD" H 3050 7050 60  0001 C CNN
F 3 "~" H 3050 7050 60  0000 C CNN
	5    3050 7050
	1    0    0    -1  
$EndComp
$Comp
L RES R35
U 1 1 534EF854
P 1900 2700
F 0 "R35" H 1970 2665 50  0000 L TNN
F 1 "6.8k (1%)" H 1900 2755 30  0000 C BNN
F 2 "lib:SMD0402" H 1900 2700 60  0001 C CNN
F 3 "~" H 1900 2700 60  0000 C CNN
	1    1900 2700
	1    0    0    -1  
$EndComp
$Comp
L RES R31
U 1 1 534EF861
P 1500 2700
F 0 "R31" H 1570 2665 50  0000 L TNN
F 1 "6.8k (1%)" H 1500 2755 30  0000 C BNN
F 2 "lib:SMD0402" H 1500 2700 60  0001 C CNN
F 3 "~" H 1500 2700 60  0000 C CNN
	1    1500 2700
	1    0    0    -1  
$EndComp
$Comp
L CAP C55
U 1 1 534EF869
P 2350 2200
F 0 "C55" H 2370 2170 50  0000 L TNN
F 1 "470p (5%)" H 2375 2220 30  0000 L BNN
F 2 "lib:SMD0402" H 2350 2200 60  0001 C CNN
F 3 "~" H 2350 2200 60  0000 C CNN
	1    2350 2200
	1    0    0    -1  
$EndComp
$Comp
L CAP C51
U 1 1 534EF876
P 2150 2900
F 0 "C51" H 2170 2870 50  0000 L TNN
F 1 "470p (5%)" H 2175 2920 30  0000 L BNN
F 2 "lib:SMD0402" H 2150 2900 60  0001 C CNN
F 3 "~" H 2150 2900 60  0000 C CNN
	1    2150 2900
	0    -1   -1   0   
$EndComp
$Comp
L OPAMP_QUAD U7
U 4 1 534EFB0E
P 2400 3600
F 0 "U7" H 2475 3450 60  0000 L CNN
F 1 "LTC6242" H 2450 3750 60  0000 L CNN
F 2 "lib:LT_DFN16_QUAD" H 2400 3600 60  0001 C CNN
F 3 "~" H 2400 3600 60  0000 C CNN
	4    2400 3600
	1    0    0    -1  
$EndComp
$Comp
L RES R36
U 1 1 534EFB14
P 1900 3700
F 0 "R36" H 1970 3665 50  0000 L TNN
F 1 "6.8k (1%)" H 1900 3755 30  0000 C BNN
F 2 "lib:SMD0402" H 1900 3700 60  0001 C CNN
F 3 "~" H 1900 3700 60  0000 C CNN
	1    1900 3700
	1    0    0    -1  
$EndComp
$Comp
L RES R32
U 1 1 534EFB1A
P 1500 3700
F 0 "R32" H 1570 3665 50  0000 L TNN
F 1 "6.8k (1%)" H 1500 3755 30  0000 C BNN
F 2 "lib:SMD0402" H 1500 3700 60  0001 C CNN
F 3 "~" H 1500 3700 60  0000 C CNN
	1    1500 3700
	1    0    0    -1  
$EndComp
$Comp
L CAP C56
U 1 1 534EFB20
P 2350 3200
F 0 "C56" H 2370 3170 50  0000 L TNN
F 1 "470p (5%)" H 2375 3220 30  0000 L BNN
F 2 "lib:SMD0402" H 2350 3200 60  0001 C CNN
F 3 "~" H 2350 3200 60  0000 C CNN
	1    2350 3200
	1    0    0    -1  
$EndComp
$Comp
L CAP C52
U 1 1 534EFB26
P 2150 3900
F 0 "C52" H 2170 3870 50  0000 L TNN
F 1 "470p (5%)" H 2175 3920 30  0000 L BNN
F 2 "lib:SMD0402" H 2150 3900 60  0001 C CNN
F 3 "~" H 2150 3900 60  0000 C CNN
	1    2150 3900
	0    -1   -1   0   
$EndComp
$Comp
L OPAMP_QUAD U7
U 1 1 534EFB41
P 2400 4600
F 0 "U7" H 2475 4450 60  0000 L CNN
F 1 "LTC6242" H 2450 4750 60  0000 L CNN
F 2 "lib:LT_DFN16_QUAD" H 2400 4600 60  0001 C CNN
F 3 "~" H 2400 4600 60  0000 C CNN
	1    2400 4600
	1    0    0    -1  
$EndComp
$Comp
L RES R37
U 1 1 534EFB47
P 1900 4700
F 0 "R37" H 1970 4665 50  0000 L TNN
F 1 "6.8k (1%)" H 1900 4755 30  0000 C BNN
F 2 "lib:SMD0402" H 1900 4700 60  0001 C CNN
F 3 "~" H 1900 4700 60  0000 C CNN
	1    1900 4700
	1    0    0    -1  
$EndComp
$Comp
L RES R33
U 1 1 534EFB4D
P 1500 4700
F 0 "R33" H 1570 4665 50  0000 L TNN
F 1 "6.8k (1%)" H 1500 4755 30  0000 C BNN
F 2 "lib:SMD0402" H 1500 4700 60  0001 C CNN
F 3 "~" H 1500 4700 60  0000 C CNN
	1    1500 4700
	1    0    0    -1  
$EndComp
$Comp
L CAP C57
U 1 1 534EFB53
P 2350 4200
F 0 "C57" H 2370 4170 50  0000 L TNN
F 1 "470p (5%)" H 2375 4220 30  0000 L BNN
F 2 "lib:SMD0402" H 2350 4200 60  0001 C CNN
F 3 "~" H 2350 4200 60  0000 C CNN
	1    2350 4200
	1    0    0    -1  
$EndComp
$Comp
L CAP C53
U 1 1 534EFB59
P 2150 4900
F 0 "C53" H 2170 4870 50  0000 L TNN
F 1 "470p (5%)" H 2175 4920 30  0000 L BNN
F 2 "lib:SMD0402" H 2150 4900 60  0001 C CNN
F 3 "~" H 2150 4900 60  0000 C CNN
	1    2150 4900
	0    -1   -1   0   
$EndComp
$Comp
L OPAMP_QUAD U7
U 2 1 534EFB74
P 2400 5600
F 0 "U7" H 2475 5450 60  0000 L CNN
F 1 "LTC6242" H 2450 5750 60  0000 L CNN
F 2 "lib:LT_DFN16_QUAD" H 2400 5600 60  0001 C CNN
F 3 "~" H 2400 5600 60  0000 C CNN
	2    2400 5600
	1    0    0    -1  
$EndComp
$Comp
L RES R38
U 1 1 534EFB7A
P 1900 5700
F 0 "R38" H 1970 5665 50  0000 L TNN
F 1 "6.8k (1%)" H 1900 5755 30  0000 C BNN
F 2 "lib:SMD0402" H 1900 5700 60  0001 C CNN
F 3 "~" H 1900 5700 60  0000 C CNN
	1    1900 5700
	1    0    0    -1  
$EndComp
$Comp
L RES R34
U 1 1 534EFB80
P 1500 5700
F 0 "R34" H 1570 5665 50  0000 L TNN
F 1 "6.8k (1%)" H 1500 5755 30  0000 C BNN
F 2 "lib:SMD0402" H 1500 5700 60  0001 C CNN
F 3 "~" H 1500 5700 60  0000 C CNN
	1    1500 5700
	1    0    0    -1  
$EndComp
$Comp
L CAP C58
U 1 1 534EFB86
P 2350 5200
F 0 "C58" H 2370 5170 50  0000 L TNN
F 1 "470p (5%)" H 2375 5220 30  0000 L BNN
F 2 "lib:SMD0402" H 2350 5200 60  0001 C CNN
F 3 "~" H 2350 5200 60  0000 C CNN
	1    2350 5200
	1    0    0    -1  
$EndComp
$Comp
L CAP C54
U 1 1 534EFB8C
P 2150 5900
F 0 "C54" H 2170 5870 50  0000 L TNN
F 1 "470p (5%)" H 2175 5920 30  0000 L BNN
F 2 "lib:SMD0402" H 2150 5900 60  0001 C CNN
F 3 "~" H 2150 5900 60  0000 C CNN
	1    2150 5900
	0    -1   -1   0   
$EndComp
$Comp
L +5VA #PWR72
U 1 1 534FDA74
P 6550 4150
F 0 "#PWR72" H 6550 4280 20  0001 C CNN
F 1 "+5VA" H 6550 4250 30  0000 C CNN
F 2 "" H 6550 4150 60  0000 C CNN
F 3 "" H 6550 4150 60  0000 C CNN
	1    6550 4150
	1    0    0    -1  
$EndComp
$Comp
L +5VA #PWR73
U 1 1 534FDB27
P 7200 3850
F 0 "#PWR73" H 7200 3980 20  0001 C CNN
F 1 "+5VA" H 7200 3950 30  0000 C CNN
F 2 "" H 7200 3850 60  0000 C CNN
F 3 "" H 7200 3850 60  0000 C CNN
	1    7200 3850
	1    0    0    -1  
$EndComp
$Comp
L CPOL C66
U 1 1 534FDB35
P 7200 4050
F 0 "C66" H 7230 4020 50  0000 L TNN
F 1 "10u" H 7230 4070 30  0000 L BNN
F 2 "lib:SMD0805" H 7200 4050 60  0001 C CNN
F 3 "~" H 7200 4050 60  0000 C CNN
	1    7200 4050
	0    1    1    0   
$EndComp
$Comp
L CAP C67
U 1 1 534FDB42
P 7450 4050
F 0 "C67" H 7470 4020 50  0000 L TNN
F 1 "100n" H 7475 4070 30  0000 L BNN
F 2 "lib:SMD0402" H 7450 4050 60  0001 C CNN
F 3 "~" H 7450 4050 60  0000 C CNN
	1    7450 4050
	0    -1   -1   0   
$EndComp
$Comp
L CAP C68
U 1 1 534FDB48
P 7650 4050
F 0 "C68" H 7670 4020 50  0000 L TNN
F 1 "10n" H 7675 4070 30  0000 L BNN
F 2 "lib:SMD0402" H 7650 4050 60  0001 C CNN
F 3 "~" H 7650 4050 60  0000 C CNN
	1    7650 4050
	0    -1   -1   0   
$EndComp
Text HLabel 5950 4250 2    60   Output ~ 0
HPL
$Comp
L +5VA #PWR67
U 1 1 534FE1CA
P 3050 6600
F 0 "#PWR67" H 3050 6730 20  0001 C CNN
F 1 "+5VA" H 3050 6700 30  0000 C CNN
F 2 "" H 3050 6600 60  0000 C CNN
F 3 "" H 3050 6600 60  0000 C CNN
	1    3050 6600
	1    0    0    -1  
$EndComp
$Comp
L AGND_OUT #PWR68
U 1 1 534FE2F2
P 3050 7700
F 0 "#PWR68" H 3050 7700 40  0001 C CNN
F 1 "AGND_OUT" H 3050 7630 50  0000 C CNN
F 2 "" H 3050 7700 60  0000 C CNN
F 3 "" H 3050 7700 60  0000 C CNN
	1    3050 7700
	1    0    0    -1  
$EndComp
$Comp
L TPA6130A2 U8
U 1 1 5356E12C
P 4900 4350
F 0 "U8" H 5200 3800 60  0000 C CNN
F 1 "TPA6130A2" H 5150 4900 60  0000 L CNN
F 2 "lib:TPA8130A2-QFN20" H 4900 4350 60  0001 C CNN
F 3 "~" H 4900 4350 60  0000 C CNN
	1    4900 4350
	1    0    0    -1  
$EndComp
$Comp
L CAP C59
U 1 1 5356ECB4
P 3750 4150
F 0 "C59" H 3770 4120 50  0000 L TNN
F 1 "1u" H 3775 4170 30  0000 L BNN
F 2 "lib:SMD0402" H 3750 4150 60  0001 C CNN
F 3 "~" H 3750 4150 60  0000 C CNN
	1    3750 4150
	1    0    0    -1  
$EndComp
$Comp
L CAP C61
U 1 1 5356ECC1
P 4000 4250
F 0 "C61" H 4020 4220 50  0000 L TNN
F 1 "1u" H 4025 4270 30  0000 L BNN
F 2 "lib:SMD0402" H 4000 4250 60  0001 C CNN
F 3 "~" H 4000 4250 60  0000 C CNN
	1    4000 4250
	1    0    0    -1  
$EndComp
$Comp
L CAP C60
U 1 1 5356ECD1
P 3750 4450
F 0 "C60" H 3770 4420 50  0000 L TNN
F 1 "1u" H 3775 4470 30  0000 L BNN
F 2 "lib:SMD0402" H 3750 4450 60  0001 C CNN
F 3 "~" H 3750 4450 60  0000 C CNN
	1    3750 4450
	1    0    0    -1  
$EndComp
$Comp
L CAP C62
U 1 1 5356ECD7
P 4000 4550
F 0 "C62" H 4020 4520 50  0000 L TNN
F 1 "1u" H 4025 4570 30  0000 L BNN
F 2 "lib:SMD0402" H 4000 4550 60  0001 C CNN
F 3 "~" H 4000 4550 60  0000 C CNN
	1    4000 4550
	1    0    0    -1  
$EndComp
$Comp
L CAP C65
U 1 1 5356F2A6
P 4950 3050
F 0 "C65" H 4970 3020 50  0000 L TNN
F 1 "1u" H 4975 3070 30  0000 L BNN
F 2 "lib:SMD0402" H 4950 3050 60  0001 C CNN
F 3 "~" H 4950 3050 60  0000 C CNN
	1    4950 3050
	1    0    0    -1  
$EndComp
$Comp
L +5VA #PWR70
U 1 1 5356F633
P 4700 3250
F 0 "#PWR70" H 4700 3380 20  0001 C CNN
F 1 "+5VA" H 4700 3350 30  0000 C CNN
F 2 "" H 4700 3250 60  0000 C CNN
F 3 "" H 4700 3250 60  0000 C CNN
	1    4700 3250
	1    0    0    -1  
$EndComp
$Comp
L CAP C64
U 1 1 5356F6D2
P 4500 3450
F 0 "C64" H 4520 3420 50  0000 L TNN
F 1 "10n" H 4525 3470 30  0000 L BNN
F 2 "lib:SMD0402" H 4500 3450 60  0001 C CNN
F 3 "~" H 4500 3450 60  0000 C CNN
	1    4500 3450
	0    -1   -1   0   
$EndComp
$Comp
L CAP C63
U 1 1 5356F6F4
P 4350 3450
F 0 "C63" H 4370 3420 50  0000 L TNN
F 1 "100n" H 4375 3470 30  0000 L BNN
F 2 "lib:SMD0402" H 4350 3450 60  0001 C CNN
F 3 "~" H 4350 3450 60  0000 C CNN
	1    4350 3450
	0    -1   -1   0   
$EndComp
Text HLabel 5950 4550 2    60   Output ~ 0
HPR
Text HLabel 4200 5150 0    60   Input ~ 0
~SD
Text HLabel 4200 5250 0    60   BiDi ~ 0
SDA
Text HLabel 4200 5350 0    60   BiDi ~ 0
SCL
$Comp
L TP TP8
U 1 1 53575E3E
P 3300 3700
F 0 "TP8" H 3300 3600 60  0000 C CNN
F 1 "TP" H 3300 3800 60  0000 C CNN
F 2 "lib:TP_HOLE" H 3300 3700 60  0001 C CNN
F 3 "~" H 3300 3700 60  0000 C CNN
	1    3300 3700
	1    0    0    -1  
$EndComp
$Comp
L TP TP9
U 1 1 53575F6B
P 3300 4000
F 0 "TP9" H 3300 3900 60  0000 C CNN
F 1 "TP" H 3300 4100 60  0000 C CNN
F 2 "lib:TP_HOLE" H 3300 4000 60  0001 C CNN
F 3 "~" H 3300 4000 60  0000 C CNN
	1    3300 4000
	1    0    0    -1  
$EndComp
$Comp
L TP TP10
U 1 1 53576008
P 3300 4700
F 0 "TP10" H 3300 4600 60  0000 C CNN
F 1 "TP" H 3300 4800 60  0000 C CNN
F 2 "lib:TP_HOLE" H 3300 4700 60  0001 C CNN
F 3 "~" H 3300 4700 60  0000 C CNN
	1    3300 4700
	1    0    0    -1  
$EndComp
$Comp
L TP TP11
U 1 1 5357600E
P 3300 5000
F 0 "TP11" H 3300 4900 60  0000 C CNN
F 1 "TP" H 3300 5100 60  0000 C CNN
F 2 "lib:TP_HOLE" H 3300 5000 60  0001 C CNN
F 3 "~" H 3300 5000 60  0000 C CNN
	1    3300 5000
	1    0    0    -1  
$EndComp
$Comp
L CAP C72
U 1 1 53A98FC3
P 6350 3900
F 0 "C72" H 6370 3870 50  0000 L TNN
F 1 "1u" H 6375 3920 30  0000 L BNN
F 2 "lib:SMD0402" H 6350 3900 60  0001 C CNN
F 3 "~" H 6350 3900 60  0000 C CNN
	1    6350 3900
	0    -1   -1   0   
$EndComp
Wire Wire Line
	3000 4150 3650 4150
Wire Wire Line
	2900 4250 3900 4250
Wire Wire Line
	1650 2700 1750 2700
Wire Wire Line
	1700 2700 1700 2200
Wire Wire Line
	1700 2200 2250 2200
Connection ~ 1700 2700
Wire Wire Line
	2200 2500 2050 2500
Wire Wire Line
	2450 2200 2800 2200
Wire Wire Line
	2800 2200 2800 2600
Wire Wire Line
	2700 2600 3000 2600
Wire Wire Line
	2050 2700 2200 2700
Wire Wire Line
	2150 2700 2150 2800
Connection ~ 2150 2700
Wire Wire Line
	1350 2700 1100 2700
Wire Wire Line
	2150 3100 2150 3000
Wire Wire Line
	1650 3700 1750 3700
Wire Wire Line
	1700 3700 1700 3200
Wire Wire Line
	1700 3200 2250 3200
Connection ~ 1700 3700
Wire Wire Line
	2200 3500 2050 3500
Wire Wire Line
	2450 3200 2800 3200
Wire Wire Line
	2800 3200 2800 3600
Wire Wire Line
	2700 3600 2900 3600
Wire Wire Line
	2050 3700 2200 3700
Wire Wire Line
	2150 3700 2150 3800
Connection ~ 2150 3700
Wire Wire Line
	1350 3700 1100 3700
Wire Wire Line
	2150 4100 2150 4000
Wire Wire Line
	1650 4700 1750 4700
Wire Wire Line
	1700 4700 1700 4200
Wire Wire Line
	1700 4200 2250 4200
Connection ~ 1700 4700
Wire Wire Line
	2200 4500 2050 4500
Wire Wire Line
	2800 4600 2700 4600
Wire Wire Line
	2050 4700 2200 4700
Wire Wire Line
	2150 4700 2150 4800
Connection ~ 2150 4700
Wire Wire Line
	1350 4700 1100 4700
Wire Wire Line
	2150 5100 2150 5000
Wire Wire Line
	1650 5700 1750 5700
Wire Wire Line
	1700 5700 1700 5200
Wire Wire Line
	1700 5200 2250 5200
Connection ~ 1700 5700
Wire Wire Line
	2200 5500 2050 5500
Wire Wire Line
	2800 5200 2800 5600
Wire Wire Line
	2800 5600 2700 5600
Wire Wire Line
	2050 5700 2200 5700
Wire Wire Line
	2150 5700 2150 5800
Connection ~ 2150 5700
Wire Wire Line
	1350 5700 1100 5700
Wire Wire Line
	2150 6100 2150 6000
Connection ~ 2800 5200
Wire Wire Line
	2900 3600 2900 4250
Connection ~ 2800 3600
Wire Wire Line
	3000 2600 3000 4150
Connection ~ 2800 2600
Wire Wire Line
	2900 4550 3900 4550
Wire Wire Line
	2900 5200 2900 4550
Wire Wire Line
	2450 5200 2900 5200
Wire Wire Line
	2800 4200 2450 4200
Wire Wire Line
	7200 3850 7200 3950
Wire Wire Line
	7200 3900 7650 3900
Wire Wire Line
	7650 3900 7650 3950
Connection ~ 7200 3900
Wire Wire Line
	7450 3900 7450 3950
Connection ~ 7450 3900
Wire Wire Line
	7200 4150 7200 4300
Wire Wire Line
	7200 4250 7650 4250
Wire Wire Line
	7450 4250 7450 4150
Connection ~ 7200 4250
Wire Wire Line
	7650 4250 7650 4150
Connection ~ 7450 4250
Wire Wire Line
	3050 6600 3050 6850
Wire Wire Line
	3050 7400 3050 7700
Wire Wire Line
	6550 4150 6550 4450
Wire Wire Line
	2050 5500 2050 5300
Wire Wire Line
	2050 5300 2800 5300
Connection ~ 2800 5300
Wire Wire Line
	2050 4500 2050 4300
Wire Wire Line
	2050 4300 2800 4300
Connection ~ 2800 4300
Wire Wire Line
	2050 3500 2050 3300
Wire Wire Line
	2050 3300 2800 3300
Connection ~ 2800 3300
Wire Wire Line
	2050 2500 2050 2300
Wire Wire Line
	2050 2300 2800 2300
Connection ~ 2800 2300
Wire Wire Line
	2800 4450 3650 4450
Connection ~ 2800 4450
Wire Wire Line
	2800 4200 2800 4700
Wire Wire Line
	3850 4150 4300 4150
Wire Wire Line
	4100 4250 4300 4250
Wire Wire Line
	4300 4450 3850 4450
Wire Wire Line
	4300 4550 4100 4550
Wire Wire Line
	4200 3650 4200 4950
Wire Wire Line
	4200 4350 4300 4350
Wire Wire Line
	5000 3750 5000 3200
Wire Wire Line
	5000 3200 5100 3200
Wire Wire Line
	5100 3200 5100 3050
Wire Wire Line
	5100 3050 5050 3050
Wire Wire Line
	4850 3050 4800 3050
Wire Wire Line
	4800 3050 4800 3200
Wire Wire Line
	4800 3200 4900 3200
Wire Wire Line
	4900 3200 4900 3750
Wire Wire Line
	4200 3650 4800 3650
Wire Wire Line
	4800 3650 4800 3750
Connection ~ 4200 4350
Wire Wire Line
	4700 3250 4700 3750
Wire Wire Line
	4350 3300 4700 3300
Wire Wire Line
	4500 3300 4500 3350
Connection ~ 4700 3300
Wire Wire Line
	4350 3300 4350 3350
Connection ~ 4500 3300
Wire Wire Line
	4350 3550 4350 3650
Connection ~ 4350 3650
Wire Wire Line
	4500 3550 4500 3650
Connection ~ 4500 3650
Wire Wire Line
	5950 4550 5500 4550
Wire Wire Line
	5950 4250 5500 4250
Wire Wire Line
	5500 4350 6350 4350
Connection ~ 6350 4350
Wire Wire Line
	6550 4450 5500 4450
Wire Wire Line
	6350 5050 5000 5050
Wire Wire Line
	5100 5050 5100 4950
Connection ~ 6350 5050
Wire Wire Line
	5000 5050 5000 4950
Connection ~ 5100 5050
Wire Wire Line
	4200 5150 4700 5150
Wire Wire Line
	4700 5150 4700 4950
Wire Wire Line
	4200 5250 4800 5250
Wire Wire Line
	4800 5250 4800 4950
Wire Wire Line
	4200 5350 4900 5350
Wire Wire Line
	4900 5350 4900 4950
Wire Wire Line
	6350 3650 5100 3650
Wire Wire Line
	5100 3650 5100 3750
Wire Wire Line
	3200 3700 3000 3700
Connection ~ 3000 3700
Wire Wire Line
	3200 4000 2900 4000
Connection ~ 2900 4000
Wire Wire Line
	2800 4700 3200 4700
Connection ~ 2800 4600
Wire Wire Line
	3200 5000 2900 5000
Connection ~ 2900 5000
Wire Wire Line
	6350 3650 6350 3800
Wire Wire Line
	6350 4000 6350 5250
Wire Wire Line
	5500 4150 5850 4150
Wire Wire Line
	5850 4150 5850 3650
Connection ~ 5850 3650
$Comp
L AGND_OUT #PWR71
U 1 1 582AD0F9
P 6350 5250
F 0 "#PWR71" H 6350 5250 40  0001 C CNN
F 1 "AGND_OUT" H 6350 5180 50  0000 C CNN
F 2 "" H 6350 5250 60  0000 C CNN
F 3 "" H 6350 5250 60  0000 C CNN
	1    6350 5250
	1    0    0    -1  
$EndComp
$Comp
L AGND_OUT #PWR74
U 1 1 582AD17A
P 7200 4300
F 0 "#PWR74" H 7200 4300 40  0001 C CNN
F 1 "AGND_OUT" H 7200 4230 50  0000 C CNN
F 2 "" H 7200 4300 60  0000 C CNN
F 3 "" H 7200 4300 60  0000 C CNN
	1    7200 4300
	1    0    0    -1  
$EndComp
$Comp
L AGND_OUT #PWR69
U 1 1 582AD1F4
P 4200 4950
F 0 "#PWR69" H 4200 4950 40  0001 C CNN
F 1 "AGND_OUT" H 4200 4880 50  0000 C CNN
F 2 "" H 4200 4950 60  0000 C CNN
F 3 "" H 4200 4950 60  0000 C CNN
	1    4200 4950
	1    0    0    -1  
$EndComp
$Comp
L AGND_OUT #PWR66
U 1 1 582AD26E
P 2150 6100
F 0 "#PWR66" H 2150 6100 40  0001 C CNN
F 1 "AGND_OUT" H 2150 6030 50  0000 C CNN
F 2 "" H 2150 6100 60  0000 C CNN
F 3 "" H 2150 6100 60  0000 C CNN
	1    2150 6100
	1    0    0    -1  
$EndComp
$Comp
L AGND_OUT #PWR65
U 1 1 582AD3C6
P 2150 5100
F 0 "#PWR65" H 2150 5100 40  0001 C CNN
F 1 "AGND_OUT" H 2150 5030 50  0000 C CNN
F 2 "" H 2150 5100 60  0000 C CNN
F 3 "" H 2150 5100 60  0000 C CNN
	1    2150 5100
	1    0    0    -1  
$EndComp
$Comp
L AGND_OUT #PWR64
U 1 1 582AD568
P 2150 4100
F 0 "#PWR64" H 2150 4100 40  0001 C CNN
F 1 "AGND_OUT" H 2150 4030 50  0000 C CNN
F 2 "" H 2150 4100 60  0000 C CNN
F 3 "" H 2150 4100 60  0000 C CNN
	1    2150 4100
	1    0    0    -1  
$EndComp
$Comp
L AGND_OUT #PWR63
U 1 1 582AD779
P 2150 3100
F 0 "#PWR63" H 2150 3100 40  0001 C CNN
F 1 "AGND_OUT" H 2150 3030 50  0000 C CNN
F 2 "" H 2150 3100 60  0000 C CNN
F 3 "" H 2150 3100 60  0000 C CNN
	1    2150 3100
	1    0    0    -1  
$EndComp
$EndSCHEMATC
