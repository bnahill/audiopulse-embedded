/******************************************************************************
 *
 * Freescale Semiconductor Inc.
 * (c) Copyright 2004-2010 Freescale Semiconductor, Inc.
 * ALL RIGHTS RESERVED.
 *
 ******************************************************************************
 *
 * THIS SOFTWARE IS PROVIDED BY FREESCALE "AS IS" AND ANY EXPRESSED OR 
 * IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES 
 * OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED.  
 * IN NO EVENT SHALL FREESCALE OR ITS CONTRIBUTORS BE LIABLE FOR ANY DIRECT, 
 * INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES 
 * (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR 
 * SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) 
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, 
 * STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING 
 * IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF 
 * THE POSSIBILITY OF SUCH DAMAGE.
 *
 **************************************************************************//*!
 *
 * @file main.c
 *
 * @author
 *
 * @version
 *
 * @date
 *
 * @brief   This software is the USB driver stack for S08 family
 *****************************************************************************/
#include "types.h"
#include "derivative.h" /* include peripheral declarations */
#include "user_config.h"
#include "RealTimerCounter.h"
#include "usb_devapi.h"
/*****************************************************************************
 * Global Functions Prototypes
 *****************************************************************************/
#if MAX_TIMER_OBJECTS
extern uint_8 TimerQInitialize(uint_8 ControllerId);
#endif
#if ((defined _MCF51MM256_H) || (defined _MCF51JE256_H))
void interrupt VectorNumber_Vkbi2 Kbi_ISR(void);
#endif

#if (defined(_MC9S08MM128_H) || defined(_MC9S08JE128_H))
#pragma CODE_SEG NON_BANKED
#endif 
void interrupt VectorNumber_Virq IRQ_ISR(void);
#if (defined(_MC9S08MM128_H) || defined(_MC9S08JE128_H))
#pragma CODE_SEG DEFAULT
#endif


extern void TestApp_Init(void);
extern void TestApp_Task(void);
#if (defined _MC9S08JE128_H) || defined (_MC9S08JM16_H) || defined(_MC9S08JM60_H) || defined (_MC9S08JS16_H) || defined (_MC9S08MM128_H)
extern void USB_Engine(void);
#endif
/*****************************************************************************
 * Local Functions Prototypes
 *****************************************************************************/
#if (defined(_MCF51JM128_H) || defined(_MC9S08JM16_H) || defined(_MC9S08JM60_H) || defined(_MC9S08JS16_H))  
void interrupt VectorNumber_Vkeyboard Kbi_ISR(void);
#endif
void Watchdog_Reset(void);
void display_led(uint_8 val);
static void Init_Sys(void);
static void Mcu_Init(void);
static void MCG_Init(void);
static void Kbi_Init(void);
static void all_led_off(void);
/****************************************************************************
 * Global Variables
 ****************************************************************************/
#define TIMER_NOT_STARTED      0xFF

volatile uint_8 kbi_stat = 0x00;		/* Status of the Key Pressed */
uint_8 kbi_debounce_tmr = TIMER_NOT_STARTED;
boolean start_kbi_debounce_tmr;
/*****************************************************************************
 * Global Functions
 *****************************************************************************/
#if (defined(_MC9S08MM128_H) || defined(_MC9S08JE128_H))
#pragma CODE_SEG DEFAULT
/* unsecure flash */
const uint_8 sec@0xffbf = 0x02; 
/* checksum bypass */
const uint_8 checksum_bypass@0xffba = 0x0; 
#endif

#if ((defined _MCF51MM256_H) || (defined _MCF51JE256_H))
/* unsecure flash */
const uint_8 sec@0x040f = 0x00; 
/* checksum bypass */
const uint_8 checksum_bypass@0x040a = 0x0; 
#endif /* (defined _MCF51MM256_H) || (defined _MCF51JE256_H) */

#ifdef TIMER_CALLBACK_ARG
static void KbiDebounceTimerCallback(void * arg);
#else
static void KbiDebounceTimerCallback(void);
#endif

void Watchdog_Reset(void)
{
  SRS = 0x55;
  SRS = 0xAA;
}

/******************************************************************************
 * @name        main
 *
 * @brief       This routine is the starting point of the application
 *
 * @param       None
 *
 * @return      None
 *
 *****************************************************************************
 * This function initializes the system, enables the interrupts and calls the
 * application
 *****************************************************************************/
void main(void)
{
    Init_Sys();        /* Initialize the system */
#if MAX_TIMER_OBJECTS
    (void)TimerQInitialize(0);
#endif
    (void)TestApp_Init(); /* Initialize the USB Test Application */
    
    while(TRUE)
    {
       Watchdog_Reset();

#if (defined (_MC9S08JE128_H) || defined (_MC9S08JM16_H) || defined (_MC9S08JM60_H) || defined (_MC9S08JS16_H) || defined (_MC9S08MM128_H))
	if(USB_PROCESS_PENDING()){
		USB_Engine();
	}
#endif

       /* Handle the KBI debounce timer */
       #if MAX_TIMER_OBJECTS
       if((start_kbi_debounce_tmr) && (kbi_debounce_tmr == TIMER_NOT_STARTED))
       {
         TIMER_OBJECT TimerObject;

         TimerObject.msCount = 50; /* 50ms count */
         TimerObject.pfnTimerCallback = KbiDebounceTimerCallback;
         kbi_debounce_tmr = AddTimerQ(&TimerObject);
         
         if(kbi_debounce_tmr == (uint_8)ERR_TIMER_QUEUE_FULL)
         {
           /* Could not add the debounce timer. Call the KBI callback directly */
           #ifdef TIMER_CALLBACK_ARG
            KbiDebounceTimerCallback(0);
           #else
            KbiDebounceTimerCallback();
           #endif
         }
         start_kbi_debounce_tmr = FALSE;
       }
       #else
        #warning "MAX_TIMER_OBJECTS is 0. The KBI is possible to detect false press events (cannot add KBI debounce TMR) "
        #ifdef TIMER_CALLBACK_ARG
          KbiDebounceTimerCallback(0);
        #else
          KbiDebounceTimerCallback();
        #endif
       #endif
       
       
       /* Call the application task */
       TestApp_Task();
       
    }
}

/******************************************************************************
 * @name       display_led
 *
 * @brief      Displays 8bit value on Board LEDs
 *
 * @param	   val
 *
 * @return     None
 *
 *****************************************************************************
 * This function displays 8 bit value on Board LED
 *****************************************************************************/
void display_led(uint_8 val)
{
    uint_8 i = 0;
    all_led_off();
#ifndef _MC9S08JS16_H
    while(i < 8)
    {
        if(val & (1 << i))
        {

            switch(i)
            {
                case 0:
                    /* D0 */
                    PTDD_PTDD2 = 0;
                    break;
                case 1:
                    /* D1 */
                    PTFD_PTFD5 = 0;
                    break;
                case 2:
                    /* D2 */
                    PTCD_PTCD4 = 0;
                    break;
                case 3:
                    /* D3 */
                    PTCD_PTCD2 = 0;
                    break;
                case 4:
                    /* D4 */
                    PTFD_PTFD1 = 0;
                    break;
                case 5:
                    /* D5 */
                    PTFD_PTFD0 = 0;
                    break;
                case 6:
                    /* D6 */
                    PTED_PTED3 = 0;
                    break;
                case 7:
                    /* D7 */
                    PTED_PTED2 = 0;
                    break;
            }
        }
        i++;
    }
#else
    /* JS16 LED Display */
    val &= 0x3;
    if(val & 0x1)
        PTAD_PTAD5 = 0;
    if(val & 0x2)
        PTAD_PTAD0 = 0;
#endif
}
/*****************************************************************************
 * Local Functions
 *****************************************************************************/
/*****************************************************************************
 *
 *    @name     Init_Sys
 *
 *    @brief    This function Initializes the system
 *
 *    @param    None
 *
 *    @return   None
 *
 ****************************************************************************
 * Intializes the MCU, MCG, KBI, RTC modules
 ***************************************************************************/
static void Init_Sys(void)
{
    Mcu_Init(); /* initialize the MCU registers */
    MCG_Init(); /* initialize the MCG to generate 24MHz bus clock */
    Kbi_Init(); /* initialize the KBI registers */
}

/*****************************************************************************
 * @name     MCU_Init
 *
 * @brief:   Initialization of the MCU.
 *
 * @param  : None
 *
 * @return : None
 *****************************************************************************
 * It will configure the MCU to disable STOP and COP Modules.
 * It also set the MCG configuration and bus clock frequency.
 ****************************************************************************/
static void Mcu_Init()
{
#if (defined(_MCF51JM128_H) || defined(_MC9S08JM16_H) || defined(_MC9S08JM60_H))    
	/* System Options 1 Register */
    SOPT1 = 0xF0; /*Enable the COP,  and enable the MCU stop mode*/
                      /*
                 *  0b11110000
                 *    ||||||||__ bit0: Reserved, Read Only bit
                 *    |||||||___ bit1: Reserved, Read Only bit
                 *    ||||||____ bit2: Reserved, Read Only bit
                 *    |||||_____ bit3: Reserved, Read Only bit
                 *    ||||______ bit4: WAIT Mode Enable
                 *    |||_______ bit5: STOP Mode enable
                 *    ||________ bit6: COP Watchdog Timeout
                 *    |_________ bit7: COP Watchdog Timeout
                 */
  /* System Options 2 Register */
    SOPT2 = 0x00; /*
                 *  0b00000000
                 *    ||||||||__ bit0: ACMP output not connected to
                 *    |||||||                  TPM1 input channel 0
                 *    |||||||___ bit1: Input filter disabled on SPI2
                 *    ||||||           (allows for higher maximum baud rate)
                 *    ||||||____ bit2: Input filter disabled on SPI21
                 *    |||||            (allows for higher maximum baud rate)
                 *    |||||_____ bit3: Clock source to CMT is BUSCLK
                 *    ||||______ bit4: BUSCLK not avail on the extern pin PTF4
                 *    |||_______ bit5: 0 Little Endian
                 *    ||________ bit6: COP Window Mode - Normal mode
                 *    |_________ bit7: COP Watchdog Clock Select -
                 *                     Internal 1 kHz LPOCLK is source to COP
                 */
    /* System Power Management Status and Control 1 Register */
    SPMSC1 = 0x40;/*
                 *  0b01000000
                 *    ||||||||__ bit0: Bandgap buffer disabled
                 *    |||||||___ bit1: Reserved, Read Only bit
                 *    ||||||____ bit2: Low-Voltage Detect(LVD) logic disabled
                 *    |||||_____ bit3: LVD disabled during stop mode
                 *    ||||______ bit4: LVDF does not generate hardware resets
                 *    |||_______ bit5: Hardware interrupt disabled(use polling)
                 *    ||________ bit6: If LVWF = 1, a low-voltage
                 *    |                condition has occurred
                 *    |_________ bit7: Low-voltage warning is not present
                 */
  /* System Power Management Status and Control 2 Register */
  SPMSC2 = 0x00;/*
                 *  0b00000000
                 *    ||||||||__ bit0: Stop3 low power mode enabled
                 *    |||||||___ bit1: Reserved, Read Only bit
                 *    ||||||____ bit2: Writing 1 to PPDACK clears the PPDF bit
                 *    |||||_____ bit3: read-only status : Microcontroller has
                 *    ||||             not recovered from Stop2 mode
                 *    ||||______ bit4: Low trip point selected
                 *    |||_______ bit5: Low trip point selected
                 *    ||________ bit6: Reserved, Read Only bit
                 *    |_________ bit7: Reserved, Read Only bit
                 */


    /*IO configuration*/
    PTAD = 0x00;        /*only PTA0--PTA5 are available for JM128*/
    PTADD = 0xFF;   /*Set output direction*/
    PTAPE = 0x00;       /*Pull-up disable*/
    PTASE = 0x00;   /*slew rate control*/
    PTADS = 0x00;     /*drive strength*/


    PTBD = 0x00;            /*PTB0--PTB7 are all available for JM128*/
    PTBDD = 0xFF;     /*Set output direction
                      PTB.0--3 are used to output to drive the LEDs(1-4) */
    PTBPE = 0x00;       /*Pull-up disable*/
    PTBSE = 0x00;       /*slew rate control*/
    PTBDS = 0x00;       /*drive strength*/

    PTCD = 0x14;            /*PTC7 is not  available for JM128*/
    PTCDD = 0xFF;     /*Set output direction*/
    PTCPE = 0x00;       /*Pull-up disable*/
    PTCSE = 0x00;       /*slew rate control*/
    PTCDS = 0x00;         /*drive strength*/


    PTDD = 0x04;            /*PTD0--PTD7 are all available for JM128*/
    PTDDD = 0xFF;     /*Set output direction*/
    PTDPE = 0x00;         /*Pull-up disable*/
    PTDSE = 0x00;       /*slew rate control*/
    PTDDS = 0x00;         /*drive strength*/

    PTED =  0x0C;           /*PTE0--PTE7 are all available for JM128*/
    PTEDD = 0xFF;     /*Set output direction*/
    PTEPE = 0x00;         /*Pull-up disable*/
    PTESE = 0x00;       /*slew rate control*/
    PTEDS = 0x00;         /*drive strength*/

    PTFD = 0x63;            /*PTF0--PTF7 are all available for JM128*/
    PTFDD = 0xFF;     /*Set output direction*/
    PTFPE = 0x00;       /*Pull-up disable*/
    PTFSE = 0x00;       /*slew rate control*/
    PTFDS = 0x00;         /*drive strength*/

    PTGD = 0x00;            /*PTG6 & PTG7 are not available for JM128*/
    PTGDD = 0xF0;     /*PTG0-3 used for KBI input*/
    PTGPE = 0x0F;         /*Pull-up disable    */
    PTGSE = 0x00;       /*slew rate control*/
    PTGDS = 0x00;         /*drive strength */
#endif
#ifdef _MC9S08JS16_H
    SOPT1 = 0xF2;             /*Enable the COP,  and enable the MCU stop mode*/
                              /*
                         *  0b11110010
                         *    ||||||||__ bit0:
                         *    |||||||___ bit1:
                         *    ||||||____ bit2:
                         *    |||||_____ bit3:
                         *    ||||______ bit4:
                         *    |||_______ bit5: STOP Mode enable
                         *    ||________ bit6: COP configuration
                         *    |_________ bit7: COP configuration
                         */
    SOPT2 = 0x00;
                              /*
                         *  0b00000000
                         *    ||||||||__ bit0:   ACIC       ACMP output connect to TPM channel 0
                         *    |||||||___ bit1:   SPI2FE     SPI2 input filter enable
                         *    ||||||____ bit2:   SPI1FE     SPI1 input filter enable
                         *    |||||_____ bit3:
                         *    ||||______ bit4:
                         *    |||_______ bit5:   COPW COP Window
                         *    ||________ bit6:   COPCLKS    COP wactchdog clock select
                         *    |_________ bit7:   COPCLKS    COP wactchdog clock select
                         */
    SPMSC1 = 0x40;
                              /*
                         *  0b01000000
                         *    ||||||||__ bit0:   BGBE       Bandgap Buffer enable
                         *    |||||||___ bit1:   0
                         *    ||||||____ bit2:   LVDE       Low_Voltage detect enable
                         *    |||||_____ bit3:   LVDSE      Low_Voltage Detect in stop mode
                         *    ||||______ bit4:   LVDRE      Low_Voltage Detect Reset Enable
                         *    |||_______ bit5:   LVWIE      Low_Voltage Warning interrupt Enable
                         *    ||________ bit6:   LVWACK Low-Voltage Warning Acknowledge
                         *    |_________ bit7:   LVWF       Low_Voltage Warning Flag
                         */
  SPMSC2 = 0x00;
                            /*
                         *  0b00000000
                         *    ||||||||__ bit0:   PPDC       Partlal Power Down Control
                         *    |||||||___ bit1:   0
                         *    ||||||____ bit2:   PPDACK Partlal Power Down Acknowledge
                         *    |||||_____ bit3:   PPDF       partlal Power Down Flag
                         *    ||||______ bit4:   LVWV       Low-Voltage Warning Voltage Select
                         *    |||_______ bit5:   LVDV       Low_Voltage Detect Voltage select
                         *    ||________ bit6:   0
                         *    |_________ bit7:   0
                         */
                         
    /* IRQSC = IRQSC_IRQIE_MASK; */  /*enable the IRQ interrupt */

    /*IO configuration*/
    PTAD = 0x21;            /**/
    PTADD = 0x21;     /*Set output direction*/
    PTAPE = 0xC2;         /*Pull-up disable*/
    PTASE = 0x00;       /*slew rate control*/
    PTADS = 0x00;       /*drive strength*/

    PTBD = 0x00;            /**/
    PTBDD = 0x00;     /*Set output direction*/
    PTBPE = 0x02;       /*Pull-up disable*/
    PTBSE = 0x00;       /*slew rate control*/
    PTBDS = 0x00;       /*drive strength*/
#endif
#if (defined(_MC9S08MM128_H) || defined(_MC9S08JE128_H))
    SOPT2 = SOPT2 | 0x10;
    SIMCO = 0x04;
    SOPT1 = 0x23; /* disable COP; enable stop an wait mode */
    /* Configure LED io pins to be outputs */
    /* Enable Bus Clock for SCI1 */   
    SCGC1 |= SCGC1_SCI1_MASK;
    SOPT3 |= SOPT3_SCI1PS_MASK;
    
    PTEDD=(1<<3) | (1<<2);
    PTFDD=(1<<5) | (1<<1) | (1<<0);
    PTCDD=(1<<4) | (1<<2);
    PTDDD=(1<<2);
    /* Enable internal pull-ups on port E pins to get switches working. */
    PTGPE=(1<<1) | (1<<0);

    PTED_PTED2=0;
    PTEDD_PTEDD2=1;
        
    IRQSC = IRQSC_IRQIE_MASK | IRQSC_IRQPE_MASK; /*enable the IRQ interrupt for SW4 */    
#endif
#if ((defined _MCF51MM256_H) || (defined _MCF51JE256_H))
  /* Disable watchdog. */
    SOPT1 = 0x23; /* disable COP; enable bkgd, stop and wait mode */
 
    SOPT2 = 0x06; /* set right endianess for USB module */
    SOPT3 = SOPT3_SCI1PS_MASK;/* route TX1,RX1 to PTD6,PTD7 */

  /* Configure LED io pins to be outputs. 
   * EVB51JM128: LED to port mapping: 
   *  LED1  LED2 LED3 LED4 LED5 LED6 LED7 LED8
   *   |     |    |    |    |    |    |    |
   *   ^     ^    ^    ^    ^    ^    ^    ^
   *  PTE2  PTE3 PTF0 PTF1 PTF5 PTD2 PTC4 PTC6
   * DEMO51JM128: LED to port mapping: 
   *  LED1  LED2 LED3 LED4 LED5 LED6 LED7 LED8
   *   |     |    |    |    |    |    |    |
   *   ^     ^    ^    ^    ^    ^    ^    ^
   *  PTE2  PTE3 PTF0 PTF1 PTC2 PTC4 PTF5 PTD2
   */
  PTEDD= PTED_PTED2_MASK | PTED_PTED3_MASK;
  PTFDD= PTFD_PTFD0_MASK | PTFD_PTFD1_MASK | PTFD_PTFD5_MASK;
  PTCDD= PTCD_PTCD4_MASK 
 #ifdef EVB
          | PTCD_PTCD6_MASK
 #else
          | PTCD_PTCD2_MASK          
 #endif          
          ;
          
  PTDDD= PTDD_PTDD2_MASK;
  
  /* Enable internal pull-ups on port E pins to get switches working.
   * EVB51JM128: Button-to-port mapping:
   *  SW1   SW2   SW3   SW4
   *   |     |     |     |
   *   ^     ^     ^     ^
   *  PTG1  PTG2  PTG3  PTD5
   * DEMO51JM128: Button-to-port mapping:
   *  SW1   SW2   SW3   SW4
   *   |     |     |     |
   *   ^     ^     ^     ^
   *  PTG0  PTG1  PTG2  PTG3
   */
  PTGPE= PTGPE_PTGPE1_MASK | PTGPE_PTGPE2_MASK | PTGPE_PTGPE3_MASK
  #ifndef EVB
    | PTGPE_PTGPE0_MASK  
  #endif
   ;
  PTDPE= PTDPE_PTDPE5_MASK ;
  
  IRQSC = IRQSC_IRQIE_MASK | IRQSC_IRQPE_MASK; /*enable the IRQ interrupt for SW4 */    

#endif
}

/*****************************************************************************
 * @name     MCG_Init
 *
 * @brief:   Initialization of the Multiple Clock Generator.
 *
 * @param  : None
 *
 * @return : None
 *****************************************************************************
 * Provides clocking options for the device, including a phase-locked
 * loop(PLL) and frequency-locked loop (FLL) for multiplying slower reference
 * clock sources
 ****************************************************************************/
static void MCG_Init()
{
#if (defined(_MCF51JM128_H) || defined(_MC9S08JM16_H) || defined(_MC9S08JM60_H) || defined(_MC9S08JS16_H))  
    /* the MCG is default set to FEI mode, it should be change to FBE mode*/
    /* MCG Control Register 2 */
    MCGC2 = 0x36;
                   /*
                *  0b00110110
                *    ||||||||__ bit0: External ref clock disabled in stop
                *    |||||||___ bit1: MCGERCLK active
                *    ||||||____ bit2: Oscillator requested
                *    |||||_____ bit3: FLL/PLL not disabled in bypass modes
                *    ||||______ bit4: Configure crystal oscillator for
                *    |||              high gain operation
                *    |||_______ bit5: High frequency range selected for
                *    ||               crystal oscillator of 1 MHz to 16 MHz
                *    ||________ bit6: Divides selected clock by 1
                *    |_________ bit7: Divides selected clock by 1
                */
  /* MCG Status and Control Register */
    while(!(MCGSC & 0x02)){};       /*Wait for the OSC stable*/
                   /*
                *  0b00000010
                *    ||||||||__ bit0:   FTRIM       MCG fine trim
                *    |||||||___ bit1:   OSCINIT OSC initialization
                *    ||||||____ bit2:   CLKST       Clock Mode status
                *    |||||_____ bit3:   CLKST
                *    ||||______ bit4:   IREFST  Internal reference status
                *    |||_______ bit5:   PLLST       PLL select status
                *    ||________ bit6:   LOCK        Lock Status
                *    |_________ bit7:   LOLS        Loss of lock status
                */
  /* MCG Control Register 1 */
    MCGC1 = 0x9B;
                   /*
                *  0b10011011
                *    ||||||||__ bit0: Internal ref clock stays enabled in stop
                *    |||||||          if IRCLKEN is set or if MCG is in FEI,
                *    |||||||          FBI or BLPI mode before entering stop.
                *    |||||||___ bit1: MCGIRCLK active
                *    ||||||____ bit2: External reference clock selected
                *    |||||_____ bit3: External Reference Divider
                *    ||||______ bit4: External Reference Divider
                *    |||_______ bit5: External Reference Divider
                *    ||________ bit6: External reference clock is selected
                *    |_________ bit7: External reference clock is selected
                */

    /* check the external reference clock is selected or not*/
    while((MCGSC & 0x1C ) != 0x08){};

    /* Switch to PBE mode from FBE*/
    /* MCG Control Register 3 */
    MCGC3 = 0x48;
                     /*
                *  0b01001000
                *    ||||||||__ bit0: VCO Divider - Multiply by 32
                *    |||||||___ bit1: VCO Divider - Multiply by 32
                *    ||||||____ bit2: VCO Divider - Multiply by 32
                *    |||||_____ bit3: VCO Divider - Multiply by 32
                *    ||||______ bit4: Divide-by-32 is disabled
                *    |||_______ bit5: Clock monitor is disabled
                *    ||________ bit6: PLL is selected
                *    |_________ bit7: Generate an interrupt request on loss of lock
                */
    while ((MCGSC & 0x48) != 0x48){};       /*wait for the PLL is locked & */

    /*Switch to PEE mode from PBE mode*/
    MCGC1 &= 0x3F;
    while((MCGSC & 0x6C) != 0x6C){};
#endif
#if (defined(_MC9S08MM128_H) || defined(_MC9S08JE128_H))
 //------FBE MODE------  
  /* Assume 16MHz external clock source connected. */
  // RANGE = 1; HGO = 1; ERCLKEN=1; EREFS = 1; BDIV = 000
  MCGC2 = 0x36;
  
  // DIV32 =1
  MCGC3 = 0x10;
  
  // CLKS = 10; RDIV = 100; IREFS = 0;
  MCGC1 = 0xA0; 


  // Wait for Reference Status bit to update
  while (MCGSC_IREFST); 
  // Wait for clock status bits to update 
  while (MCGSC_CLKST != 0b10);       

//------PBE MODE------ 

  // PLLS =1; DIV32 = 1; VDIV = 1001 
  MCGC3 = 0x5c;
  
  // wait for PLL status bit to update
  while (!MCGSC_PLLST); 
  // Wait for LOCK bit to set 
  while (!MCGSC_LOCK);
  
//------PEE MODE------   

  // CLKS = 00; RDIV = 100; IREFS = 0
  MCGC1 = 0x20;

// Wait for clock status bits to update 
  while (MCGSC_CLKST != 0b11);       ;
    
  /* Now MCGOUT=48MHz, BUS_CLOCK=24MHz */  
#endif
#if ((defined _MCF51MM256_H) || (defined _MCF51JE256_H))
 /* Assume 12MHz external clock source connected. */
  
  /* In order to use the USB we need to enter PEE mode and MCGOUT set to 48 MHz. 
     Out of reset MCG is in FEI mode. */
  
  //------FBE MODE------  
  /* Assume 16MHz external clock source connected. */
  // RANGE = 1; HGO = 1; ERCLKEN=1; EREFS = 1; BDIV = 000
  MCGC2 = 0x36;
  
  // DIV32 =1
  MCGC3 = 0x10;
  
  // CLKS = 10; RDIV = 100; IREFS = 0;
  MCGC1 = 0xA0; 


  // Wait for Reference Status bit to update
  while (MCGSC_IREFST)
  {
  }
  // Wait for clock status bits to update 
  while (MCGSC_CLKST != 0b10) 
  {
  }

//------PBE MODE------ 

  // PLLS =1; DIV32 = 1; VDIV = 1001 
  MCGC3 = 0x5c;
  
  // wait for PLL status bit to update
  while (!MCGSC_PLLST) 
  {
  }
  // Wait for LOCK bit to set 
  while (!MCGSC_LOCK) 
  {
  }
  
//------PEE MODE------   

  // CLKS = 00; RDIV = 100; IREFS = 0
  MCGC1 = 0x20;

// Wait for clock status bits to update 
  while (MCGSC_CLKST != 0b11) 
  {
  }
#endif
}

/******************************************************************************
 * @name        Kbi_Init
 *
 * @brief       This routine is the initialization function
 *
 * @param       None
 *
 * @return      None
 *
 ******************************************************************************
 * This function initializes the keyboard interrupt module
 *****************************************************************************/
 static void Kbi_Init(void)
{
#if (defined(_MC9S08MM128_H) || defined(_MCF51MM256_H) || (defined _MCF51JE256_H) || defined(_MC9S08JE128_H))
    PTCPE |= 0x40;
    PTCD &= 0xBF;
    KBI2SC_KB2ACK = 1;   
    KBI2SC_KBI2MOD = 0;
    KBI2ES = 0x00;
    KBI2PE = 0x02;
    KBI2SC_KB2IE = 1;
#endif
#ifdef _MCF51JM128_H
    PTGDD &= 0xF0;     /* set PTG0-3 to input */
    PTGPE |= 0x0F;     /* enable PTG0-3 pull-up resistor */
    KBI1SC_KBIE = 1;
    KBI1SC_KBACK = 1;
    KBI1ES = 0x00;
    KBI1PE = 0xC3;      /* enable KBI0,1,6,7 */
#endif
#if (defined(_MC9S08JM16_H) || defined(_MC9S08JM60_H))	
    PTGDD &= 0xF0;     /* set PTG0-3 to input */
    PTGPE |= 0x0F;     /* enable PTG0-3 pull-up resistor */
    KBISC_KBIE = 1;
    KBISC_KBACK = 1;
    KBIES = 0x00;
    KBIPE = 0xC3;      /* enable KBI0,1,6,7 */
#endif
#ifdef _MC9S08JS16_H
    PTAD |= 0x21;
    PTADD |= 0x21;

    PTAPE = 0xC2;      /*enable PTG0-3 pull-up resistor*/
    PTBPE = 0x08;

    /*
      Clear Interrupt Status Register
      and Interrupt ACK to avoid spurious KBI Interrupt
    */
    KBISC_KBIE = 1;
    KBISC_KBACK = 1;
    KBIES = 0x00;
    KBIPE = 0xC2;      /*enable KBI0,1,6,7*/
#endif
}

/******************************************************************************
 * @name       Kbi_ISR
 *
 * @brief      The routine is the KBI interrupt service routine
 *
 * @param	   None
 *
 * @return     None
 *
 *****************************************************************************
 * This function is hooked on to the KBI interrupt and is called each time
 * KBI interrupt is generated
 *****************************************************************************/
#if (defined(_MCF51JM128_H) || defined(_MC9S08JM16_H) || defined(_MC9S08JM60_H) || defined(_MC9S08JS16_H))  
void interrupt VectorNumber_Vkeyboard Kbi_ISR(void)
#endif

#if (defined(_MC9S08MM128_H) || defined(_MC9S08JE128_H))
#pragma CODE_SEG NON_BANKED
void interrupt VectorNumber_Vkeyboard2 Kbi_ISR(void)
#endif

#if ((defined _MCF51MM256_H) || (defined _MCF51JE256_H))
void interrupt VectorNumber_Vkbi2 Kbi_ISR(void)
#endif

{  
 /* Start a debounce timer to sample the GPIO after 50ms. 
  * This removes the spikes in detecting a pressed keyboard.
  */
 start_kbi_debounce_tmr = TRUE;  

 /* Acknowledge the interrupt */
#if (defined(_MCF51MM256_H) || defined(_MC9S08MM128_H) || (defined _MCF51JE256_H) || defined(_MC9S08JE128_H))    
    KBI2SC_KB2ACK = 1;     /* clear KBI interrupt */  
#endif
#ifdef _MC9S08JS16_H
    KBISC_KBACK = 1;     /* clear KBI interrupt (for S08)*/
#endif	  
#if (defined(_MCF51JM128_H) || defined(_MC9S08JM16_H) || defined(_MC9S08JM60_H))        
	#ifdef _MCF51JM128_H
	    KBI1SC_KBACK = 1;     /* clear KBI interrupt (for CFv1)*/
	#else
	    KBISC_KBACK = 1;     /* clear KBI interrupt (for S08)*/
	#endif
#endif		      
}

#if (defined(_MC9S08MM128_H) || defined(_MC9S08JE128_H))
#pragma CODE_SEG DEFAULT
#endif

/******************************************************************************
 * @name       KbiDebounceTimerCallback
 *
 * @brief      The routine samples the GPIO input to determine the pressed key
 *
 * @param	     None
 *
 * @return     None
 *
 *****************************************************************************
 * This function is hooked on to the KBI interrupt and is called 50ms after the.
 * KBI interrupt is generated
 *****************************************************************************/
#ifdef TIMER_CALLBACK_ARG
static void KbiDebounceTimerCallback(void * arg)
#else
static void KbiDebounceTimerCallback(void)
#endif
{
  #ifdef TIMER_CALLBACK_ARG
    UNUSED (arg)
  #endif
 /* Reset the debounce timer */
 kbi_debounce_tmr = TIMER_NOT_STARTED;
 
 /* Sample the GPIO */
 #if (defined(_MCF51MM256_H) || defined(_MC9S08MM128_H) || (defined _MCF51JE256_H) || defined(_MC9S08JE128_H))
    if(!PTCD_PTCD6)
        kbi_stat = 2;
 #endif
 #ifdef _MC9S08JS16_H
    if(!PTAD_PTAD1)
        kbi_stat |= 0x02;

    if(!PTAD_PTAD6)
        kbi_stat |= 0x04;

    if(!PTAD_PTAD7)
        kbi_stat |= 0x08;     	 
 #endif	  
 #if (defined(_MCF51JM128_H) || defined(_MC9S08JM16_H) || defined(_MC9S08JM60_H))    
    kbi_stat = (uint_8)~(PTGD & 0x0F); /* get which buttons were pressed */
 #endif	
 
}

/******************************************************************************
 * @name       IRQ_ISR
 *
 * @brief      The IRQ is used to wakup the USB and MCU.
 *
 * @param	   None
 *
 * @return     None
 *
 *****************************************************************************
 * This function is hooked on to the IRQ interrupt and is called each time
 * we get an IRQ interrupt
 *****************************************************************************/
#if (defined(_MC9S08MM128_H) || defined(_MC9S08JE128_H))
#pragma CODE_SEG NON_BANKED
#endif 
void interrupt VectorNumber_Virq IRQ_ISR(void)
{
    IRQSC_IRQACK = 1; /* acknoledge IRQ */
    
 #if (defined(_MCF51MM256_H) || defined(_MC9S08MM128_H) || (defined _MCF51JE256_H) || defined(_MC9S08JE128_H))
    /* Temporary setting of the kbi_stat with KBI_PTG2_MASK */
    kbi_stat |= 0x08;
 #endif
}
#if (defined(_MC9S08MM128_H) || defined(_MC9S08JE128_H))
#pragma CODE_SEG DEFAULT
#endif

/******************************************************************************
 * @name       all_led_off
 *
 * @brief      Switch OFF all LEDs on Board
 *
 * @param	   None
 *
 * @return     None
 *
 *****************************************************************************
 * This function switch OFF all LEDs on Board
 *****************************************************************************/
static void all_led_off(void)
{
#ifndef _MC9S08JS16_H
    /* D0 */
    PTDD_PTDD2 = 1;
    /* D1 */
    PTFD_PTFD5 = 1;
    /* D2 */
    PTCD_PTCD4 = 1;
    /* D3 */
    PTCD_PTCD2 = 1;
    /* D4 */
    PTFD_PTFD1 = 1;
    /* D5 */
    PTFD_PTFD0 = 1;
    /* D6 */
    PTED_PTED3 = 1;
    /* D7 */
    PTED_PTED2 = 1;
#else
    PTAD_PTAD0 = 1;
    PTAD_PTAD5 = 1;
#endif
}
/* EOF */
