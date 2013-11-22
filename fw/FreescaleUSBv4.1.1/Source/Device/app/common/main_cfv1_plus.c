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
 * @file main_cfv1_plus.c
 *
 * @author
 *
 * @version
 *
 * @date
 *
 * @brief   This software is the USB driver stack for S08 family
 *****************************************************************************/
#include <hidef.h>
#include "types.h"
#include "derivative.h" /* include peripheral declarations */
#include "user_config.h"
#include "RealTimerCounter.h"

/*****************************************************************************
 * Macros
 *****************************************************************************/
#define XTAL8					(3)


/*****************************************************************************
 * Global Functions Prototypes
 *****************************************************************************/
#if MAX_TIMER_OBJECTS
extern uint_8 TimerQInitialize(uint_8 ControllerId);
#endif
extern void TestApp_Init(void);
extern void TestApp_Task(void);

/*****************************************************************************
 * Local Functions Prototypes
 *****************************************************************************/
void Watchdog_Reset(void);
void display_led(uint_8 val);
static void Init_Sys(void);
static void Mcu_Init(void);
static void MCG_Init(void);

static void StartKeyPressSimulationTimer(void);
static void KeyPressSimulationTimerCallback(void);


/****************************************************************************
 * Global Variables
 ****************************************************************************/
uint_8 kbi_stat_sim;
volatile uint_8 kbi_stat;	   /* Status of the Key Pressed */

void Watchdog_Reset(void)
{
	SIM_SRVCOP = 0x55;
	SIM_SRVCOP = 0xAA;
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
    Init_Sys();        /* initial the system */
#if MAX_TIMER_OBJECTS
    (void)TimerQInitialize(0);
#endif
    (void)TestApp_Init(); /* Initialize the USB Test Application */
#if MAX_TIMER_OBJECTS
    StartKeyPressSimulationTimer();
#endif
    while(TRUE)
    {
       Watchdog_Reset();

       /* Call the application task */
       TestApp_Task();
    }
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
	/* Enable clock source for UART0 module */
	SIM_SCGC1 |= SIM_SCGC1_UART0_MASK;
	/* Enable clock source for MTIM, FTM0 FTM1 modules */
	SIM_SCGC3 |= (SIM_SCGC3_MTIM_MASK|SIM_SCGC3_FTM1_MASK|SIM_SCGC3_FTM0_MASK);
	/* Enable clock source for USB module */
	SIM_SCGC6 |= SIM_SCGC6_USBOTG_MASK;
	
	/* Enable Port Muxing of Uart0 */
	MXC_PTAPF1 |= MXC_PTAPF1_A7(2);
	MXC_PTDPF1 |= MXC_PTDPF1_D6(2);

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
	/* Select MCGOUT */
    SIM_CLKOUT = SIM_CLKOUT_CLKOUTDIV(0)|SIM_CLKOUT_CS(3);
    
    /* First move to FBE mode */
    /* Enable external oscillator, RANGE=1, HGO=1, EREFS=1, LP=0, IRCS=0 */
    MCG_C2 = MCG_C2_RANGE(1) | MCG_C2_HGO_MASK | MCG_C2_EREFS_MASK;
        
    /* Select external oscillator and Reference Divider and clear IREFS to start ext. osc. */
    // CLKS=2, FRDIV=3, IREFS=0, IRCLKEN=0, IREFSTEN=0
    MCG_C1 = MCG_C1_CLKS(2) | MCG_C1_FRDIV(3); 
    /* Wait for oscillator to initialize */
    while (!(MCG_S & MCG_S_OSCINIT_MASK)){};
    /* Wait for Reference clock Status bit to clear */
    while (MCG_S & MCG_S_IREFST_MASK){}; 
    /* Wait for clock status bits to show clock source is ext. ref. clk. */
    while (((MCG_S & MCG_S_CLKST_MASK) >> MCG_S_CLKST_SHIFT) != 0x2){};    
 
    /* Now in FBE
    /* Configure PLL Ref Divider, PLLCLKEN=0, PLLSTEN=0, PRDIV=5
       The crystal frequency is used to select the PRDIV value. Only even frequency crystals are supported
       that will produce a 2MHz reference clock to the PLL. */
    MCG_C5 = MCG_C5_PRDIV(XTAL8); // Set PLL ref. divider to match the crystal used

    /* Ensure MCG_C6 is at the reset default of 0. LOLIE disabled, PLL disabled, clk. monitor disabled, PLL VCO divider is clear */
    MCG_C6 = 0x0;   
 
    /* Set system options dividers
       MCG=PLL, core = MCG  */ 
	SIM_CLKDIV0 =  SIM_CLKDIV0_OUTDIV(0);
    /* Set the VCO divider and enable the PLL for 48MHz, LOLIE=0, PLLS=1, CME=0, VDIV=0 */
    MCG_C6 = MCG_C6_PLLS_MASK; //VDIV = 0 (x24)    
    
    /* Wait for PLL status bit to set */
    while (!(MCG_S & MCG_S_PLLST_MASK)){}; 
    /* Wait for LOCK bit to set */
    while (!(MCG_S & MCG_S_LOCK_MASK)){}; 
    
    /* Now running PBE Mode */
    
    /* Transition into PEE by setting CLKS to 0 */
    /* CLKS=0, FRDIV=3, IREFS=0, IRCLKEN=0, IREFSTEN=0 */
    MCG_C1 &= ~MCG_C1_CLKS_MASK;

    /* Wait for clock status bits to update */
    while (((MCG_S & MCG_S_CLKST_MASK) >> MCG_S_CLKST_SHIFT) != 0x3){};
            
    SIM_OSC1 = ((SIM_OSC1 & ~SIM_OSC1_OSC1RANGE_MASK) | SIM_OSC1_OSC1RANGE(0));
    /* Clear HGO for low power operation */
    SIM_OSC1 &= ~SIM_OSC1_OSC1HGO_MASK;

    /* Select oscillator for external reference */
    SIM_OSC1 |= SIM_OSC1_OSC1EREFS_MASK;
    /* Enable oscillator */
    SIM_OSC1 |= SIM_OSC1_OSC1EN_MASK;

    /* Enable the ER clock of oscillators */
    SIM_SCGC5 |=SIM_SCGC5_OSC1_MASK | SIM_SCGC5_OSC2_MASK;
    OSC1_CR = OSC_CR_ERCLKEN_MASK | OSC_CR_EREFSTEN_MASK;
    OSC2_CR = OSC_CR_ERCLKEN_MASK | OSC_CR_EREFSTEN_MASK;    
      
    /* Now running PEE Mode */
    SIM_SOPT3 |= SIM_SOPT3_RWE_MASK; 
    SIM_SOPT1 |= SIM_SOPT1_REGE_MASK;
    SIM_SOPT7 |= 0x80;
    /* Ensure PLL is selected as USB CLK source */
    SIM_CLKDIV1 = 0x00;                 
}
#if MAX_TIMER_OBJECTS
/******************************************************************************
 * @name        StartKeyPressSimulationTimer
 *
 * @brief       This routine is used to start the timer for generating key press events
 *              Used on boards without buttons
 *
 * @param       None
 *
 * @return      None
 *
 *****************************************************************************
 *
 *****************************************************************************/
static void StartKeyPressSimulationTimer(void)
{
  TIMER_OBJECT TimerObject;
			
  TimerObject.msCount = KEY_PRESS_SIM_TMR_INTERVAL;
  TimerObject.pfnTimerCallback = (void*)KeyPressSimulationTimerCallback;
  (void)AddTimerQ(&TimerObject);
}

/******************************************************************************
 * @name        KeyPressSimulationTimerCallback
 *
 * @brief       This routine is used to generate simulated key press events
 *              It is called at KeyPressSimulationTimer expire time
 *              Used on boards without buttons
 *
 * @param       None
 *
 * @return      None
 *
 *****************************************************************************
 *
 *****************************************************************************/
static void KeyPressSimulationTimerCallback(void)
{
	 /* generate the button press */
	 kbi_stat_sim = (uint_8)((kbi_stat_sim << 1) & (uint_8)0x0F);
	 
	 if(kbi_stat_sim == 0)
	 {
	  kbi_stat_sim = 1;
	 }
	 
	 kbi_stat = kbi_stat_sim;
	 
	 /* re-trigger the timer */
	 StartKeyPressSimulationTimer();
}
/******************************************************************************
 * @name        display_led
 *
 * @brief       Display LEDs
 *
 * @param       Bit map corresponding with displayed LEDs
 *
 * @return      None
 *
 *****************************************************************************
 *
 *****************************************************************************/
#endif /* End of #if MAX_TIMER_OBJECTS*/
void display_led(uint_8 val)
{
	UNUSED(val)
}
