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
Sheet 4 6
Title "Android DPOAE Interface"
Date "13 may 2015"
Rev "2b"
Comp "Sana AudioPulse"
Comment1 "Ben Nahill <bnahill@gmail.com>"
Comment2 ""
Comment3 ""
Comment4 ""
$EndDescr
Text HLabel 4100 5950 2    60   Output ~ 0
HP_~SD
$Comp
L CAP C34
U 1 1 524A6DA1
P 5550 1250
F 0 "C34" H 5570 1220 50  0000 L TNN
F 1 "100n" H 5575 1270 30  0000 L BNN
F 2 "lib:SMD0402" H 5550 1250 60  0001 C CNN
F 3 "~" H 5550 1250 60  0000 C CNN
	1    5550 1250
	0    -1   -1   0   
$EndComp
$Comp
L CAP C33
U 1 1 524A6DB4
P 4950 1250
F 0 "C33" H 4970 1220 50  0000 L TNN
F 1 "100n" H 4975 1270 30  0000 L BNN
F 2 "lib:SMD0402" H 4950 1250 60  0001 C CNN
F 3 "~" H 4950 1250 60  0000 C CNN
	1    4950 1250
	0    -1   -1   0   
$EndComp
$Comp
L CAP C31
U 1 1 524A6DBA
P 4650 1250
F 0 "C31" H 4670 1220 50  0000 L TNN
F 1 "2.2u" H 4675 1270 30  0000 L BNN
F 2 "lib:SMD0603" H 4650 1250 60  0001 C CNN
F 3 "~" H 4650 1250 60  0000 C CNN
	1    4650 1250
	0    -1   -1   0   
$EndComp
$Comp
L CAP C32
U 1 1 524A7184
P 4800 1250
F 0 "C32" H 4820 1220 50  0000 L TNN
F 1 "1u" H 4825 1270 30  0000 L BNN
F 2 "lib:SMD0402" H 4800 1250 60  0001 C CNN
F 3 "~" H 4800 1250 60  0000 C CNN
	1    4800 1250
	0    -1   -1   0   
$EndComp
$Comp
L XTAL X1
U 1 1 524D93F7
P 8000 4350
F 0 "X1" H 8000 4290 40  0000 C TNN
F 1 "7A-16.000MAAJ-T" H 8000 4410 40  0000 C BNN
F 2 "lib:TXC_7A" H 8000 4350 60  0001 C CNN
F 3 "~" H 8000 4350 60  0000 C CNN
	1    8000 4350
	0    1    -1   0   
$EndComp
$Comp
L CAP C35
U 1 1 524D946B
P 7750 4250
F 0 "C35" H 7770 4220 50  0000 L TNN
F 1 "CAP" H 7775 4270 30  0000 L BNN
F 2 "lib:SMD0402" H 7750 4250 60  0001 C CNN
F 3 "~" H 7750 4250 60  0000 C CNN
	1    7750 4250
	-1   0    0    -1  
$EndComp
$Comp
L CAP C36
U 1 1 524D9478
P 7750 4450
F 0 "C36" H 7770 4420 50  0000 L TNN
F 1 "CAP" H 7775 4470 30  0000 L BNN
F 2 "lib:SMD0402" H 7750 4450 60  0001 C CNN
F 3 "~" H 7750 4450 60  0000 C CNN
	1    7750 4450
	-1   0    0    -1  
$EndComp
Text HLabel 3150 4100 0    60   Output ~ 0
UART0_TX
Text HLabel 3150 4000 0    60   Input ~ 0
UART0_RX
Text HLabel 8450 1650 2    60   BiDi ~ 0
USB_D+
Text HLabel 8450 1550 2    60   BiDi ~ 0
USB_D-
Text Label 8350 1450 0    60   ~ 0
~RESET
Text HLabel 4600 4100 2    60   Output ~ 0
CODEC_PDN
Text Notes 2550 3000 0    40   ~ 0
Fs*256: 25.476MHz (for 96kHz)
Text Notes 2550 3250 2    40   ~ 0
>Fs*48
Text Notes 2550 4650 2    40   ~ 0
Fs*2
Text Label 2000 1100 2    60   ~ 0
SWDIO
Text Label 2000 1300 2    60   ~ 0
SWCLK
Text Label 2000 1000 2    60   ~ 0
~RESET
Text Label 2000 1200 2    60   ~ 0
SWO
$Comp
L RES R20
U 1 1 524F1C32
P 9300 1150
F 0 "R20" H 9370 1115 50  0000 L TNN
F 1 "10k" H 9300 1205 30  0000 C BNN
F 2 "lib:SMD0402" H 9300 1150 60  0001 C CNN
F 3 "~" H 9300 1150 60  0000 C CNN
	1    9300 1150
	0    -1   -1   0   
$EndComp
$Comp
L +3V #PWR31
U 1 1 5251A974
P 1600 1300
F 0 "#PWR31" H 1600 1260 30  0001 C CNN
F 1 "+3V" H 1600 1410 30  0000 C CNN
F 2 "" H 1600 1300 60  0000 C CNN
F 3 "" H 1600 1300 60  0000 C CNN
	1    1600 1300
	1    0    0    -1  
$EndComp
$Comp
L RES R23
U 1 1 5251B82C
P 8300 5850
F 0 "R23" H 8370 5815 50  0000 L TNN
F 1 "220" H 8300 5905 30  0000 C BNN
F 2 "lib:SMD0402" H 8300 5850 60  0001 C CNN
F 3 "~" H 8300 5850 60  0000 C CNN
	1    8300 5850
	0    -1   -1   0   
$EndComp
Text HLabel 4100 6450 2    60   Output ~ 0
SPI1_CS_CODEC
$Comp
L RES R24
U 1 1 5352B52D
P 8500 5850
F 0 "R24" H 8570 5815 50  0000 L TNN
F 1 "7.5" H 8500 5905 30  0000 C BNN
F 2 "lib:SMD0402" H 8500 5850 60  0001 C CNN
F 3 "~" H 8500 5850 60  0000 C CNN
	1    8500 5850
	0    -1   -1   0   
$EndComp
Text HLabel 3000 3500 0    60   BiDi ~ 0
I2C_SCL
Text HLabel 3000 3400 0    60   BiDi ~ 0
I2C_SDA
$Comp
L RES R21
U 1 1 53570B82
P 1850 3350
F 0 "R21" H 1920 3315 50  0000 L TNN
F 1 "10k" H 1850 3405 30  0000 C BNN
F 2 "lib:SMD0402" H 1850 3350 60  0001 C CNN
F 3 "~" H 1850 3350 60  0000 C CNN
	1    1850 3350
	0    -1   -1   0   
$EndComp
$Comp
L RES R22
U 1 1 53570B8F
P 2000 3350
F 0 "R22" H 2070 3315 50  0000 L TNN
F 1 "10k" H 2000 3405 30  0000 C BNN
F 2 "lib:SMD0402" H 2000 3350 60  0001 C CNN
F 3 "~" H 2000 3350 60  0000 C CNN
	1    2000 3350
	0    -1   -1   0   
$EndComp
$Comp
L +3V #PWR34
U 1 1 53570C75
P 1850 3050
F 0 "#PWR34" H 1850 3010 30  0001 C CNN
F 1 "+3V" H 1850 3160 30  0000 C CNN
F 2 "" H 1850 3050 60  0000 C CNN
F 3 "" H 1850 3050 60  0000 C CNN
	1    1850 3050
	1    0    0    -1  
$EndComp
Text HLabel 4100 7250 2    60   Output ~ 0
ANALOG_EN
$Comp
L RES R30
U 1 1 5363BE41
P 8700 5850
F 0 "R30" H 8770 5815 50  0000 L TNN
F 1 "39" H 8700 5905 30  0000 C BNN
F 2 "lib:SMD0402" H 8700 5850 60  0001 C CNN
F 3 "~" H 8700 5850 60  0000 C CNN
	1    8700 5850
	0    -1   -1   0   
$EndComp
$Comp
L LED3 D1
U 1 1 53645218
P 8500 6300
F 0 "D1" H 8500 6650 50  0000 C CNN
F 1 "LTST-C19FD1WT" H 8500 5950 50  0000 C CNN
F 2 "lib:LED-6PLCC" H 8500 6500 60  0001 C CNN
F 3 "~" H 8500 6500 60  0000 C CNN
	1    8500 6300
	0    1    1    0   
$EndComp
$Comp
L OSC_W_EN X2
U 1 1 53695566
P 2000 2650
F 0 "X2" H 2150 2500 60  0000 C CNN
F 1 "ASFLM2-12.288MHZ" H 2000 2800 60  0000 C CNN
F 2 "lib:ASFLM" H 2000 2650 60  0001 C CNN
F 3 "" H 2000 2650 60  0000 C CNN
	1    2000 2650
	1    0    0    -1  
$EndComp
$Comp
L +3V #PWR35
U 1 1 536955E4
P 2550 2300
F 0 "#PWR35" H 2550 2260 30  0001 C CNN
F 1 "+3V" H 2550 2410 30  0000 C CNN
F 2 "" H 2550 2300 60  0000 C CNN
F 3 "" H 2550 2300 60  0000 C CNN
	1    2550 2300
	1    0    0    -1  
$EndComp
$Comp
L CAP C71
U 1 1 53697AF9
P 2650 2500
F 0 "C71" H 2670 2470 50  0000 L TNN
F 1 "10n" H 2675 2520 30  0000 L BNN
F 2 "lib:SMD0402" H 2650 2500 60  0001 C CNN
F 3 "~" H 2650 2500 60  0000 C CNN
	1    2650 2500
	0    -1   -1   0   
$EndComp
$Comp
L CONN_01X06 P3
U 1 1 57D64DBC
P 2200 1250
F 0 "P3" H 2200 1600 50  0000 C CNN
F 1 "CONN_01X06" V 2300 1250 50  0000 C CNN
F 2 "lib:HDR_6X1_50MIL" H 2200 1250 50  0001 C CNN
F 3 "" H 2200 1250 50  0000 C CNN
	1    2200 1250
	1    0    0    -1  
$EndComp
Wire Wire Line
	4650 1000 4650 1150
Wire Wire Line
	4950 1400 4950 1350
Wire Wire Line
	4650 1400 4650 1350
Wire Wire Line
	4950 1000 4950 1150
Wire Wire Line
	4800 1000 4800 1150
Wire Wire Line
	4800 1350 4800 1400
Wire Wire Line
	7850 4450 8150 4450
Wire Wire Line
	7850 4250 8400 4250
Connection ~ 8000 4450
Connection ~ 8000 4250
Wire Wire Line
	7650 4250 7550 4250
Wire Wire Line
	7550 4250 7550 4450
Wire Wire Line
	7550 4450 7650 4450
Wire Wire Line
	7450 4400 7450 4350
Wire Wire Line
	7450 4350 7550 4350
Connection ~ 7550 4350
Wire Wire Line
	8450 1550 8200 1550
Wire Wire Line
	8200 1650 8450 1650
Wire Wire Line
	1600 1300 1600 1400
Wire Wire Line
	1600 1400 2000 1400
Wire Wire Line
	1600 1600 1600 1500
Wire Wire Line
	1600 1500 2000 1500
Wire Wire Line
	8300 6000 8300 6100
Wire Wire Line
	8300 6650 8300 6500
Wire Wire Line
	8150 4450 8150 4350
Wire Wire Line
	8150 4350 8400 4350
Wire Wire Line
	8500 6000 8500 6100
Wire Wire Line
	8500 6650 8500 6500
Wire Wire Line
	2000 3500 2000 3650
Wire Wire Line
	1850 3500 1850 3750
Wire Wire Line
	1850 3050 1850 3200
Wire Wire Line
	1850 3100 2000 3100
Wire Wire Line
	2000 3100 2000 3200
Connection ~ 1850 3100
Wire Wire Line
	8700 6650 8700 6500
Wire Wire Line
	8700 6000 8700 6100
Wire Wire Line
	2550 2300 2550 2600
Wire Wire Line
	2550 2600 2300 2600
Wire Wire Line
	1600 2700 1600 2950
Wire Wire Line
	1600 2700 1700 2700
Wire Wire Line
	1600 2850 2650 2850
Wire Wire Line
	2650 2850 2650 2600
Connection ~ 1600 2850
Wire Wire Line
	2650 2400 2650 2350
Wire Wire Line
	2650 2350 2550 2350
Connection ~ 2550 2350
$Comp
L SAM_S70N_TFBGA U4
U 1 1 5822BFD9
P 3900 3850
F 0 "U4" H 3900 4700 60  0000 C CNN
F 1 "SAM_S70N_TFBGA" H 3900 4800 60  0000 C CNN
F 2 "lib:ATMEL_TFBGA100" H 4000 4100 60  0001 C CNN
F 3 "" H 4000 4100 60  0001 C CNN
	1    3900 3850
	1    0    0    -1  
$EndComp
$Comp
L SAM_S70N_TFBGA U4
U 2 1 5822C076
P 8800 4200
F 0 "U4" H 8800 5050 60  0000 C CNN
F 1 "SAM_S70N_TFBGA" H 8800 5150 60  0000 C CNN
F 2 "lib:ATMEL_TFBGA100" H 8900 4450 60  0001 C CNN
F 3 "" H 8900 4450 60  0001 C CNN
	2    8800 4200
	1    0    0    -1  
$EndComp
$Comp
L SAM_S70N_TFBGA U4
U 3 1 5822C0F1
P 3300 6600
F 0 "U4" H 3300 7450 60  0000 C CNN
F 1 "SAM_S70N_TFBGA" H 3300 7550 60  0000 C CNN
F 2 "lib:ATMEL_TFBGA100" H 3400 6850 60  0001 C CNN
F 3 "" H 3400 6850 60  0001 C CNN
	3    3300 6600
	1    0    0    -1  
$EndComp
$Comp
L SAM_S70N_TFBGA U4
U 4 1 5822C1FA
P 7650 1900
F 0 "U4" H 7650 2750 60  0000 C CNN
F 1 "SAM_S70N_TFBGA" H 7650 2850 60  0000 C CNN
F 2 "lib:ATMEL_TFBGA100" H 7750 2150 60  0001 C CNN
F 3 "" H 7750 2150 60  0001 C CNN
	4    7650 1900
	1    0    0    -1  
$EndComp
Text Label 8200 4050 2    60   ~ 0
SWDIO
Wire Wire Line
	8200 4050 8400 4050
Text Label 8200 3950 2    60   ~ 0
SWO
Wire Wire Line
	8400 3950 8200 3950
Text Label 8200 4150 2    60   ~ 0
SWCLK
Wire Wire Line
	8200 4150 8400 4150
Text HLabel 3000 3200 0    60   Output ~ 0
CODEC_BCLK
Text HLabel 3000 3100 0    60   Output ~ 0
CODEC_MCLK
Text HLabel 4600 4600 2    60   Input ~ 0
CODEC_SDIN
Text HLabel 3200 4600 0    60   Output ~ 0
CODEC_LRCK
Wire Wire Line
	3200 4600 3500 4600
Wire Wire Line
	3500 3200 3000 3200
Wire Wire Line
	3000 3100 3500 3100
Text HLabel 4600 3200 2    60   Output ~ 0
CODEC_SDO
Text HLabel 4100 6850 2    60   Output ~ 0
SPI1_SOUT
Wire Wire Line
	3100 2700 3100 3100
Connection ~ 3100 3100
Wire Wire Line
	1700 2600 1350 2600
Wire Wire Line
	3100 2700 2300 2700
Wire Wire Line
	4600 3200 4300 3200
Wire Wire Line
	4600 4600 4300 4600
Wire Wire Line
	3150 4000 3500 4000
Wire Wire Line
	3150 4100 3500 4100
Text Notes 3500 4000 2    40   ~ 0
URXD0
Text Notes 3500 4100 2    40   ~ 0
UTXD0
Text Notes 3500 3200 2    40   ~ 0
I2SC0_CK
Text Notes 3500 3100 2    40   ~ 0
I2SC0_MCK
Text Notes 3500 4600 2    40   ~ 0
I2SC0_WS
Text Notes 4300 4600 0    40   ~ 0
I2SC0_DI
Text Notes 4300 3200 0    40   ~ 0
I2SC0_DO
Text Notes 3500 3400 2    40   ~ 0
TWD0
Text Notes 3500 3500 2    40   ~ 0
TWCK0
Wire Wire Line
	3000 3400 3500 3400
Wire Wire Line
	3000 3500 3500 3500
Wire Wire Line
	2000 3650 3200 3650
Wire Wire Line
	3200 3650 3200 3400
Connection ~ 3200 3400
Wire Wire Line
	1850 3750 3100 3750
Wire Wire Line
	3100 3750 3100 3500
Connection ~ 3100 3500
Text HLabel 4100 6950 2    60   Input ~ 0
SPI1_SIN
Text HLabel 4100 6750 2    60   Output ~ 0
SPI1_SCK
Wire Wire Line
	4100 6450 3700 6450
Wire Wire Line
	4100 6750 3700 6750
Wire Wire Line
	3700 6850 4100 6850
Wire Wire Line
	4100 6950 3700 6950
Text Notes 3700 6450 0    40   ~ 0
SPI0_NPCS1
Text Notes 3700 6750 0    40   ~ 0
SPI0_SPCK
Text Notes 3700 6850 0    40   ~ 0
SPI0_MOSI
Text Notes 3700 6950 0    40   ~ 0
SPI0_MISO
$Comp
L RES R39
U 1 1 5823BED5
P 9850 1600
F 0 "R39" H 9920 1565 50  0000 L TNN
F 1 "5.62k" H 9850 1655 30  0000 C BNN
F 2 "lib:SMD0402" H 9850 1600 60  0001 C CNN
F 3 "~" H 9850 1600 60  0000 C CNN
	1    9850 1600
	0    -1   -1   0   
$EndComp
$Comp
L CAP C30
U 1 1 5823C2DD
P 9650 1600
F 0 "C30" H 9670 1570 50  0000 L TNN
F 1 "10p" H 9675 1620 30  0000 L BNN
F 2 "lib:SMD0402" H 9650 1600 60  0001 C CNN
F 3 "~" H 9650 1600 60  0000 C CNN
	1    9650 1600
	0    -1   -1   0   
$EndComp
Wire Wire Line
	8200 1350 9850 1350
Wire Wire Line
	9850 1350 9850 1450
Wire Wire Line
	9650 1500 9650 1350
Connection ~ 9650 1350
Wire Wire Line
	9750 1900 9750 1850
Wire Wire Line
	9650 1850 9850 1850
Wire Wire Line
	9850 1850 9850 1750
Wire Wire Line
	9650 1850 9650 1700
Connection ~ 9750 1850
Wire Wire Line
	9300 1300 9300 1450
Wire Wire Line
	9300 1450 8200 1450
$Comp
L +3V #PWR46
U 1 1 5823E640
P 9300 950
F 0 "#PWR46" H 9300 910 30  0001 C CNN
F 1 "+3V" H 9300 1060 30  0000 C CNN
F 2 "" H 9300 950 60  0000 C CNN
F 3 "" H 9300 950 60  0000 C CNN
	1    9300 950 
	1    0    0    -1  
$EndComp
Wire Wire Line
	9300 950  9300 1000
Wire Wire Line
	9450 1800 9450 2350
Wire Wire Line
	9450 1800 8200 1800
Wire Wire Line
	8200 1900 9450 1900
Connection ~ 9450 1900
Wire Wire Line
	8200 2000 9450 2000
Connection ~ 9450 2000
Wire Wire Line
	8200 2100 9450 2100
Connection ~ 9450 2100
Wire Wire Line
	8200 2200 9450 2200
Connection ~ 9450 2200
$Comp
L FB FB3
U 1 1 5823F71A
P 6750 1150
F 0 "FB3" H 6820 1115 50  0000 L TNN
F 1 "BLM18PG471SN1D" H 6685 1225 30  0000 C BNN
F 2 "lib:SMD0603" H 6730 1170 60  0001 C CNN
F 3 "" H 6730 1170 60  0000 C CNN
	1    6750 1150
	1    0    0    -1  
$EndComp
$Comp
L +3V #PWR40
U 1 1 5823F806
P 6250 950
F 0 "#PWR40" H 6250 910 30  0001 C CNN
F 1 "+3V" H 6250 1060 30  0000 C CNN
F 2 "" H 6250 950 60  0000 C CNN
F 3 "" H 6250 950 60  0000 C CNN
	1    6250 950 
	1    0    0    -1  
$EndComp
Wire Wire Line
	6250 950  6250 1650
Wire Wire Line
	6250 1150 6600 1150
Wire Wire Line
	6900 1150 7100 1150
Wire Wire Line
	6050 1200 6050 1400
Wire Wire Line
	6050 1250 7100 1250
$Comp
L CAP C16
U 1 1 5823FB23
P 6050 1100
F 0 "C16" H 6070 1070 50  0000 L TNN
F 1 "10n" H 6075 1120 30  0000 L BNN
F 2 "lib:SMD0402" H 6050 1100 60  0001 C CNN
F 3 "~" H 6050 1100 60  0000 C CNN
	1    6050 1100
	0    -1   -1   0   
$EndComp
Wire Wire Line
	4900 950  6250 950 
Wire Wire Line
	6050 950  6050 1000
Connection ~ 6050 1250
Wire Wire Line
	6250 1350 7100 1350
Connection ~ 6250 1150
Wire Wire Line
	6250 1650 7100 1650
Connection ~ 6250 1350
$Comp
L +1V2 #PWR38
U 1 1 58240290
P 6150 1800
F 0 "#PWR38" H 6150 1650 50  0001 C CNN
F 1 "+1V2" H 6150 1940 50  0000 C CNN
F 2 "" H 6150 1800 50  0000 C CNN
F 3 "" H 6150 1800 50  0000 C CNN
	1    6150 1800
	1    0    0    -1  
$EndComp
Wire Wire Line
	6150 1800 6150 2250
Wire Wire Line
	6150 1850 7100 1850
Connection ~ 6150 1850
Wire Wire Line
	6150 2050 7100 2050
Connection ~ 6150 1950
$Comp
L FB FB4
U 1 1 582405A9
P 6750 1950
F 0 "FB4" H 6820 1915 50  0000 L TNN
F 1 "BLM18PG471SN1D" H 6685 2025 30  0000 C BNN
F 2 "lib:SMD0603" H 6730 1970 60  0001 C CNN
F 3 "" H 6730 1970 60  0000 C CNN
	1    6750 1950
	1    0    0    -1  
$EndComp
Wire Wire Line
	6900 1950 7100 1950
Wire Wire Line
	6600 1950 6150 1950
$Comp
L CAP C29
U 1 1 582407E2
P 7050 2300
F 0 "C29" H 7070 2270 50  0000 L TNN
F 1 "100n" H 7075 2320 30  0000 L BNN
F 2 "lib:SMD0402" H 7050 2300 60  0001 C CNN
F 3 "~" H 7050 2300 60  0000 C CNN
	1    7050 2300
	0    -1   -1   0   
$EndComp
Wire Wire Line
	7050 2550 7050 2400
Wire Wire Line
	7050 2200 7050 1950
Connection ~ 7050 1950
Wire Wire Line
	4650 1400 5550 1400
Wire Wire Line
	4850 1400 4850 1500
Connection ~ 4850 1400
Connection ~ 4800 1400
Connection ~ 4950 1400
Wire Wire Line
	4650 1000 5550 1000
Wire Wire Line
	4900 1000 4900 950 
Connection ~ 6050 950 
Connection ~ 4900 1000
Connection ~ 4950 1000
Connection ~ 4800 1000
Text Label 1700 2600 2    60   ~ 0
CLK_EN
Text Label 8200 3650 2    60   ~ 0
CLK_EN
Wire Wire Line
	2900 6950 2100 6950
Text Label 2100 6950 2    60   ~ 0
LED2
Wire Wire Line
	3050 3300 3500 3300
Text Label 8500 5500 1    60   ~ 0
LED2
Wire Wire Line
	8500 5500 8500 5700
Text Label 8300 5500 1    60   ~ 0
LED1
Wire Wire Line
	8300 5500 8300 5700
Text Label 3050 3300 2    60   ~ 0
LED1
Wire Wire Line
	3500 4300 3050 4300
Text Label 3050 4300 2    60   ~ 0
LED3
Text Label 8700 5500 1    60   ~ 0
LED3
Wire Wire Line
	8700 5500 8700 5700
Wire Wire Line
	5550 1400 5550 1350
Wire Wire Line
	5550 1000 5550 1150
$Comp
L CAP C73
U 1 1 58241E42
P 5100 1250
F 0 "C73" H 5120 1220 50  0000 L TNN
F 1 "100n" H 5125 1270 30  0000 L BNN
F 2 "lib:SMD0402" H 5100 1250 60  0001 C CNN
F 3 "~" H 5100 1250 60  0000 C CNN
	1    5100 1250
	0    -1   -1   0   
$EndComp
Wire Wire Line
	5100 1400 5100 1350
Wire Wire Line
	5100 1000 5100 1150
$Comp
L CAP C74
U 1 1 58241E6F
P 5250 1250
F 0 "C74" H 5270 1220 50  0000 L TNN
F 1 "100n" H 5275 1270 30  0000 L BNN
F 2 "lib:SMD0402" H 5250 1250 60  0001 C CNN
F 3 "~" H 5250 1250 60  0000 C CNN
	1    5250 1250
	0    -1   -1   0   
$EndComp
Wire Wire Line
	5250 1400 5250 1350
Wire Wire Line
	5250 1000 5250 1150
Connection ~ 5100 1000
Connection ~ 5250 1000
Connection ~ 5250 1400
Connection ~ 5100 1400
$Comp
L CAP C77
U 1 1 582421FD
P 5400 1250
F 0 "C77" H 5420 1220 50  0000 L TNN
F 1 "100n" H 5425 1270 30  0000 L BNN
F 2 "lib:SMD0402" H 5400 1250 60  0001 C CNN
F 3 "~" H 5400 1250 60  0000 C CNN
	1    5400 1250
	0    -1   -1   0   
$EndComp
Wire Wire Line
	5400 1400 5400 1350
Wire Wire Line
	5400 1000 5400 1150
Connection ~ 5400 1000
Connection ~ 5400 1400
$Comp
L CAP C81
U 1 1 58242974
P 6600 2350
F 0 "C81" H 6620 2320 50  0000 L TNN
F 1 "100n" H 6625 2370 30  0000 L BNN
F 2 "lib:SMD0402" H 6600 2350 60  0001 C CNN
F 3 "~" H 6600 2350 60  0000 C CNN
	1    6600 2350
	0    -1   -1   0   
$EndComp
Wire Wire Line
	6600 2050 6600 2250
$Comp
L CAP C78
U 1 1 5824297B
P 6150 2350
F 0 "C78" H 6170 2320 50  0000 L TNN
F 1 "100n" H 6175 2370 30  0000 L BNN
F 2 "lib:SMD0402" H 6150 2350 60  0001 C CNN
F 3 "~" H 6150 2350 60  0000 C CNN
	1    6150 2350
	0    -1   -1   0   
$EndComp
$Comp
L CAP C79
U 1 1 58242982
P 6300 2350
F 0 "C79" H 6320 2320 50  0000 L TNN
F 1 "100n" H 6325 2370 30  0000 L BNN
F 2 "lib:SMD0402" H 6300 2350 60  0001 C CNN
F 3 "~" H 6300 2350 60  0000 C CNN
	1    6300 2350
	0    -1   -1   0   
$EndComp
Wire Wire Line
	6300 2050 6300 2250
$Comp
L CAP C80
U 1 1 58242989
P 6450 2350
F 0 "C80" H 6470 2320 50  0000 L TNN
F 1 "100n" H 6475 2370 30  0000 L BNN
F 2 "lib:SMD0402" H 6450 2350 60  0001 C CNN
F 3 "~" H 6450 2350 60  0000 C CNN
	1    6450 2350
	0    -1   -1   0   
$EndComp
Wire Wire Line
	6450 2050 6450 2250
Connection ~ 6600 2050
Connection ~ 6450 2050
Connection ~ 6300 2050
Wire Wire Line
	6150 2450 6150 2550
Wire Wire Line
	6150 2500 6600 2500
Wire Wire Line
	6300 2500 6300 2450
Connection ~ 6150 2500
Wire Wire Line
	6450 2500 6450 2450
Connection ~ 6300 2500
Wire Wire Line
	6600 2500 6600 2450
Connection ~ 6450 2500
Connection ~ 6150 2050
Wire Wire Line
	4100 7250 3700 7250
Text Notes 2850 6950 2    40   ~ 0
PWMC0_H0
Text Notes 3500 4300 2    40   ~ 0
PWMC0_H1
Text Notes 3500 3300 2    40   ~ 0
PWMC0_H1
Wire Wire Line
	4100 5950 3700 5950
Wire Wire Line
	4600 4100 4300 4100
Wire Wire Line
	8200 3650 8400 3650
$Comp
L GND #PWR44
U 1 1 582AE6F9
P 8500 6650
F 0 "#PWR44" H 8500 6400 50  0001 C CNN
F 1 "GND" H 8500 6500 50  0000 C CNN
F 2 "" H 8500 6650 50  0000 C CNN
F 3 "" H 8500 6650 50  0000 C CNN
	1    8500 6650
	1    0    0    -1  
$EndComp
$Comp
L GND #PWR45
U 1 1 582AE81B
P 8700 6650
F 0 "#PWR45" H 8700 6400 50  0001 C CNN
F 1 "GND" H 8700 6500 50  0000 C CNN
F 2 "" H 8700 6650 50  0000 C CNN
F 3 "" H 8700 6650 50  0000 C CNN
	1    8700 6650
	1    0    0    -1  
$EndComp
$Comp
L GND #PWR43
U 1 1 582AE88C
P 8300 6650
F 0 "#PWR43" H 8300 6400 50  0001 C CNN
F 1 "GND" H 8300 6500 50  0000 C CNN
F 2 "" H 8300 6650 50  0000 C CNN
F 3 "" H 8300 6650 50  0000 C CNN
	1    8300 6650
	1    0    0    -1  
$EndComp
$Comp
L GND #PWR42
U 1 1 582AE8FD
P 7450 4400
F 0 "#PWR42" H 7450 4150 50  0001 C CNN
F 1 "GND" H 7450 4250 50  0000 C CNN
F 2 "" H 7450 4400 50  0000 C CNN
F 3 "" H 7450 4400 50  0000 C CNN
	1    7450 4400
	1    0    0    -1  
$EndComp
$Comp
L GND #PWR41
U 1 1 582AEB06
P 7050 2550
F 0 "#PWR41" H 7050 2300 50  0001 C CNN
F 1 "GND" H 7050 2400 50  0000 C CNN
F 2 "" H 7050 2550 50  0000 C CNN
F 3 "" H 7050 2550 50  0000 C CNN
	1    7050 2550
	1    0    0    -1  
$EndComp
$Comp
L GND #PWR39
U 1 1 582AECED
P 6150 2550
F 0 "#PWR39" H 6150 2300 50  0001 C CNN
F 1 "GND" H 6150 2400 50  0000 C CNN
F 2 "" H 6150 2550 50  0000 C CNN
F 3 "" H 6150 2550 50  0000 C CNN
	1    6150 2550
	1    0    0    -1  
$EndComp
$Comp
L GND #PWR37
U 1 1 582AED5E
P 6050 1400
F 0 "#PWR37" H 6050 1150 50  0001 C CNN
F 1 "GND" H 6050 1250 50  0000 C CNN
F 2 "" H 6050 1400 50  0000 C CNN
F 3 "" H 6050 1400 50  0000 C CNN
	1    6050 1400
	1    0    0    -1  
$EndComp
$Comp
L GND #PWR36
U 1 1 582AEEBD
P 4850 1500
F 0 "#PWR36" H 4850 1250 50  0001 C CNN
F 1 "GND" H 4850 1350 50  0000 C CNN
F 2 "" H 4850 1500 50  0000 C CNN
F 3 "" H 4850 1500 50  0000 C CNN
	1    4850 1500
	1    0    0    -1  
$EndComp
$Comp
L GND #PWR47
U 1 1 582AEF2E
P 9450 2350
F 0 "#PWR47" H 9450 2100 50  0001 C CNN
F 1 "GND" H 9450 2200 50  0000 C CNN
F 2 "" H 9450 2350 50  0000 C CNN
F 3 "" H 9450 2350 50  0000 C CNN
	1    9450 2350
	1    0    0    -1  
$EndComp
$Comp
L GND #PWR48
U 1 1 582AF005
P 9750 1900
F 0 "#PWR48" H 9750 1650 50  0001 C CNN
F 1 "GND" H 9750 1750 50  0000 C CNN
F 2 "" H 9750 1900 50  0000 C CNN
F 3 "" H 9750 1900 50  0000 C CNN
	1    9750 1900
	1    0    0    -1  
$EndComp
$Comp
L GND #PWR33
U 1 1 582AF098
P 1600 2950
F 0 "#PWR33" H 1600 2700 50  0001 C CNN
F 1 "GND" H 1600 2800 50  0000 C CNN
F 2 "" H 1600 2950 50  0000 C CNN
F 3 "" H 1600 2950 50  0000 C CNN
	1    1600 2950
	1    0    0    -1  
$EndComp
$Comp
L GND #PWR32
U 1 1 582AF27F
P 1600 1600
F 0 "#PWR32" H 1600 1350 50  0001 C CNN
F 1 "GND" H 1600 1450 50  0000 C CNN
F 2 "" H 1600 1600 50  0000 C CNN
F 3 "" H 1600 1600 50  0000 C CNN
	1    1600 1600
	1    0    0    -1  
$EndComp
$EndSCHEMATC
