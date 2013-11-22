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
 * @file main_cfv2.c
 *
 * @author
 *
 * @version
 *
 * @date
 *
 * @brief   This software is the USB driver stack for Coldfire V2 family
 *****************************************************************************/
#include "types.h"      	/* User Defined Data Types */
#include "derivative.h" 	/* include peripheral declarations */
#include "user_config.h"
#include "RealTimerCounter_cfv2.h"
#include "wdt_cfv2.h"
#include "exceptions_cfv2.h"
#include "init_hw.h"

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
static void Init_Sys(void);
       void Watchdog_Reset(void);
static void all_led_off(void);

#ifdef __MCF52259_H__ 
static void Keyboard_Poll(void);
#endif

#ifdef __MCF52277_H__	
	static void StartKeyPressSimulationTimer(void);
	#ifdef TIMER_CALLBACK_ARG
		static void KeyPressSimulationTimerCallback(void * arg);
	#else
		static void KeyPressSimulationTimerCallback(void);
	#endif	
	uint_8 kbi_stat_sim;
#endif

void display_led(uint_8 val);
/****************************************************************************
 * Global Variables
 ****************************************************************************/
#define TIMER_NOT_STARTED      0xFF

volatile uint_8 kbi_stat;	   /* Status of the Key Pressed */

#ifdef __MCF52259_H__
	volatile uint_8 kbi_gpio_state = MCF_GPIO_SETDD_SETDD5 | MCF_GPIO_SETDD_SETDD6 | MCF_GPIO_SETDD_SETDD7;	
	uint_8 	usr_sw_poll_tmr = TIMER_NOT_STARTED;
#endif


/*****************************************************************************
 * Global Functions
 *****************************************************************************/
#ifdef __MCF52259_H__ 
	#ifdef TIMER_CALLBACK_ARG
		static void SwitchIntervalPollingTimerCallback(void * arg);
	#else
		static void SwitchIntervalPollingTimerCallback(void);
	#endif
#endif


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
    
    #ifdef __MCF52277_H__
	 StartKeyPressSimulationTimer();
	#endif
    
    while(TRUE)
    {
       Watchdog_Reset();
	   
	   #ifdef __MCF52259_H__ 
	    	Keyboard_Poll();			
       #endif         
       
       /* Call the application task */
       TestApp_Task();
       
    }
}

/******************************************************************************
 * @name        Keyboard_Poll
 *
 * @brief       This routine is used to poll the buttons
 *
 * @param       None
 *
 * @return      None
 *
 *****************************************************************************
 *
 *****************************************************************************/
#ifdef __MCF52259_H__ 
static void Keyboard_Poll(void)
{
	#if MAX_TIMER_OBJECTS
			if(usr_sw_poll_tmr == TIMER_NOT_STARTED)
			{			
				TIMER_OBJECT TimerObject;
			
				TimerObject.msCount = 100;	/* 100 msec. count */
				TimerObject.pfnTimerCallback = SwitchIntervalPollingTimerCallback;
				usr_sw_poll_tmr = AddTimerQ(&TimerObject);
				
				if(usr_sw_poll_tmr == ERR_TIMER_QUEUE_FULL)
				{
					/* Could not add the switch polling timer. */
					#ifdef TIMER_CALLBACK_ARG
						SwitchIntervalPollingTimerCallback(0);
					#else
						SwitchIntervalPollingTimerCallback();
					#endif
				}
			}
	#else
			#warning "MAX_TIMER_OBJECTS is 0. The user switch is continuously polled."
			#ifdef TIMER_CALLBACK_ARG
				SwitchIntervalPollingTimerCallback(0);
			#else
				SwitchIntervalPollingTimerCallback();
			#endif	
	#endif 
}
#endif

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
#ifdef __MCF52277_H__
static void StartKeyPressSimulationTimer(void)
{
  TIMER_OBJECT TimerObject;
			
  TimerObject.msCount = KEY_PRESS_SIM_TMR_INTERVAL;
  TimerObject.pfnTimerCallback = KeyPressSimulationTimerCallback;
  (void)AddTimerQ(&TimerObject);
}
#endif

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
#ifdef __MCF52277_H__

#ifdef TIMER_CALLBACK_ARG
static void KeyPressSimulationTimerCallback(void * arg)
#else
static void KeyPressSimulationTimerCallback(void)
#endif
{
 #ifdef TIMER_CALLBACK_ARG
 UNUSED(arg)
 #endif
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
#endif /* __MCF52277_H__ */

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

	#if(defined(__MCF52259_H__) || defined(__MCF52221_H__))
		/* MCF52259EVB & M52221DEMO LED Display */
		val &= 0x0F;
		if(val & 0x1)
			/* D10 (MCF52259EVBEVB) LED1 (M52221DEMO) */
			/* Set PTC[0] */
			MCF_GPIO_SETTC = MCF_GPIO_SETTC_SETTC0;
		if(val & 0x2)
			/* D11 (MCF52259EVBEVB) LED2 (M52221DEMO) */
			/* Set PTC[1] */
			MCF_GPIO_SETTC = MCF_GPIO_SETTC_SETTC1;
		if(val & 0x4)
			/* D12 (MCF52259EVBEVB) LED3 (M52221DEMO) */ 			
			/* Set PTC[2] */
			MCF_GPIO_SETTC = MCF_GPIO_SETTC_SETTC2;
		if(val & 0x8)
			/* D13 (MCF52259EVBEVB) LED4 (M52221DEMO) */
			/* Set PTC[3] */		
			MCF_GPIO_SETTC = MCF_GPIO_SETTC_SETTC3;
	#else
	 (void)val;
	 (void)i;
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
    GPIO_Init(); /* initialize the MCU registers */   

    /* Initialize interrupts for USB and PIT0 sources */
    Int_Ctl_int_init(USB_INT_CNTL, USB_ISR_SRC, 4,4, TRUE);
    Int_Ctl_int_init(PIT0_INT_CNTL, PIT0_ISR_SRC, 2,2, TRUE);
    
    /* Initialize interrupts for IRQ1 & IRQ7 sources, only for MCF52221 */
    #ifdef __MCF52221_H__
    	Int_Ctl_int_init(IRQ1_INT_CNTL, IRQ1_ISR_SRC, 3,2, TRUE); 
    	Int_Ctl_int_init(IRQ7_INT_CNTL, IRQ7_ISR_SRC, 3,3, TRUE);     	   
    #endif
    #ifdef __MCF52259_DEMO_ 
					 	
	Int_Ctl_int_init(IRQ1_INT_CNTL, IRQ1_ISR_SRC, 3,2, TRUE); 
	Int_Ctl_int_init(IRQ5_INT_CNTL, IRQ5_ISR_SRC, 3,3, TRUE); 
	#endif
    
}

/******************************************************************************
* @name       SwitchIntervalPollingTimerCallback
*
* @brief      The routine samples the GPIO input to determine the pressed key
*
* @param	  None
*
* @return     None
*
*****************************************************************************
* 
*****************************************************************************/
#ifdef __MCF52259_H__
#ifdef TIMER_CALLBACK_ARG
static void SwitchIntervalPollingTimerCallback(void * arg)
#else
static void SwitchIntervalPollingTimerCallback(void)
#endif
{
 uint_8 kbi_gpio_new_state = 0;	
 	#ifdef TIMER_CALLBACK_ARG
   		UNUSED (arg)
 	#endif
 	/* Reset the the switch polling timer */
 	usr_sw_poll_tmr = TIMER_NOT_STARTED;	
 
 #ifdef __MCF52259_H__		
 	/* Sample the GPIO */
 	kbi_gpio_new_state = (uint_8)(MCF_GPIO_SETDD & (uint_8)(MCF_GPIO_SETDD_SETDD5 | MCF_GPIO_SETDD_SETDD6 | MCF_GPIO_SETDD_SETDD7));
 	
 	kbi_gpio_state ^= kbi_gpio_new_state; 	 	
 	
 	
 	if((kbi_gpio_state & MCF_GPIO_SETDD_SETDD5) && !(kbi_gpio_new_state & MCF_GPIO_SETDD_SETDD5))
 	{ 		
 		kbi_stat |= 0x02;
 	}
 
 	if((kbi_gpio_state & MCF_GPIO_SETDD_SETDD6) && !(kbi_gpio_new_state & MCF_GPIO_SETDD_SETDD6))
 	{ 		
 		kbi_stat |= 0x04;
 	}
 	 	
	if((kbi_gpio_state & MCF_GPIO_SETDD_SETDD7) && !(kbi_gpio_new_state & MCF_GPIO_SETDD_SETDD7))
	{		
 		kbi_stat |= 0x08;
	} 	 	
 	
 	kbi_gpio_state = kbi_gpio_new_state;
 #endif 	
}
#endif


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
#ifdef __MCF52221_H__
void __declspec(interrupt) IRQ_ISR(void)
{
	 	if(MCF_EPORT_EPFR & MCF_EPORT_EPFR_EPF1)
	 	{
	 		kbi_stat |= 0x02; 						/* Update the kbd state */
	 		MCF_EPORT_EPFR |= MCF_EPORT_EPFR_EPF1;	/* Clear the bit by writting a 1 to it */
	 	}
	 		
	 	
	 	if(MCF_EPORT_EPFR & MCF_EPORT_EPFR_EPF7)
	 	{
	 		kbi_stat |= 0x04;						/* Update the kbd state */
	 		MCF_EPORT_EPFR |= MCF_EPORT_EPFR_EPF7;	/* Clear the bit by writting a 1 to it */	 			
	 	}	 	
}
#endif

#ifdef __MCF52259_DEMO_
void __declspec(interrupt) IRQ_ISR(void)
{
	 	if(MCF_EPORT_EPFR & MCF_EPORT_EPFR_EPF1)
	 	{
	 		kbi_stat |= 0x02; 						/* Update the kbd state */
	 		MCF_EPORT_EPFR |= MCF_EPORT_EPFR_EPF1;	/* Clear the bit by writting a 1 to it */	 			
	 	}
	 		
	 	
	 	if(MCF_EPORT_EPFR & MCF_EPORT_EPFR_EPF5)
	 	{
	 		kbi_stat |= 0x08;						/* Update the kbd state */
	 		MCF_EPORT_EPFR |= MCF_EPORT_EPFR_EPF5;	/* Clear the bit by writting a 1 to it */	 			
	 	}	 	
}
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
	#if(defined(__MCF52259_H__) || defined(__MCF52221_H__))
		/* MCF52259EVB: D10, D11, D12 and D13 */
		/* M52221DEMO: LED1, LED2, LED3, LED4 */
		/* Clear PTC[0], PTC[1], PTC[2] and PTC[3] */
		MCF_GPIO_CLRTC = (vuint8) ~(MCF_GPIO_CLRTC_CLRTC0 | 
						   			MCF_GPIO_CLRTC_CLRTC1 | 
						   			MCF_GPIO_CLRTC_CLRTC2 | 
						   			MCF_GPIO_CLRTC_CLRTC3);
    #endif
}
/* EOF */
