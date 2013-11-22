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
 * @file TPM.c
 *
 * @author
 *
 * @version
 *
 * @date    
 *
 * @brief   This file configures Real Time Counter (RTC) for Timer 
 *          Implementation
 *****************************************************************************/

/******************************************************************************
 * Includes
 *****************************************************************************/
#include <string.h>
#include "derivative.h"     /* include peripheral declarations */
#include "types.h"          /* Contains User Defined Data Types */
#include "RealTimerCounter.h"
#if MAX_TIMER_OBJECTS

/*****************************************************************************
 * Local Functions Prototypes
 *****************************************************************************/
static uint_8 TimerInit(void);
static void EnableTimerInterrupt(void);
static void DisableTimerInterrupt(void);

#if (defined(_MC9S08MM128_H) || defined(_MC9S08JE128_H))
#pragma CODE_SEG NON_BANKED
#endif
void interrupt VectorNumber_Vtpm2ovf Timer_ISR(void);
void interrupt VectorNumber_Vtpm2ch0 Timer1_ISR(void);
#if (defined(_MC9S08MM128_H) || defined(_MC9S08JE128_H))
#pragma CODE_SEG DEFAULT
#endif

/****************************************************************************
 * Global Variables
 ****************************************************************************/
/* Array of Timer Objects */
TIMER_OBJECT g_TimerObjectArray[MAX_TIMER_OBJECTS];
/*****************************************************************************
 * Global Functions Prototypes
 *****************************************************************************/
uint_8 TimerQInitialize(uint_8 controller_ID);
uint_8 AddTimerQ(PTIMER_OBJECT pTimerObject);
uint_8 RemoveTimerQ(uint_8 index);

/*****************************************************************************
 * Global Functions
 *****************************************************************************/
#if (defined(_MC9S08MM128_H) || defined(_MC9S08JE128_H))
#pragma CODE_SEG DEFAULT
#endif
/******************************************************************************
 *
 *   @name        TimerQInitialize
 *
 *   @brief       Initializes RTC, Timer Object Queue and System Clock Counter
 *
 *	 @param       controller_ID    : Controller ID
 *
 *   @return      None
 *****************************************************************************
 * This function initializes System Clock Counter, Timer Queue and Initializes
 * System Timer
 *****************************************************************************/
uint_8 TimerQInitialize(uint_8 controller_ID)
{
    UNUSED (controller_ID)
	(void)memset(g_TimerObjectArray, (int)NULL, sizeof(g_TimerObjectArray));
	return TimerInit();
}

/******************************************************************************
 *
 *   @name        AddTimerQ
 *
 *   @brief       Adds Timer Object to Timer Queue
 *
 *	 @param       pTimerObject	: Pointer to Timer Object
 *
 *   @return      None
 *****************************************************************************
 * Adds Timer Object to Timer Queue
 *****************************************************************************/
uint_8 AddTimerQ(PTIMER_OBJECT pTimerObject)
{
	uint_8 index;
	if(pTimerObject == NULL)
		return (uint_8)ERR_INVALID_PARAM;
	if(pTimerObject->msCount == (unsigned int)INVALID_TIME_COUNT)
		return (uint_8)ERR_INVALID_PARAM;

	for(index = 0; index < MAX_TIMER_OBJECTS; index++)
	{
	  /* Disable Timer Interrupts */
		DisableTimerInterrupt();
		if(g_TimerObjectArray[index].pfnTimerCallback == NULL)
		{
			(void)memcpy(&g_TimerObjectArray[index], pTimerObject, sizeof(TIMER_OBJECT)); 
			/* Enable Timer Interrupts */
			EnableTimerInterrupt();
			break;
		}
		/* Enable Timer Interrupts */
		EnableTimerInterrupt();
	}
	if(index == MAX_TIMER_OBJECTS)
		return (uint_8)ERR_TIMER_QUEUE_FULL;
	return index;
}

/******************************************************************************
 *
 *   @name        RemoveTimerQ
 *
 *   @brief       Removes Timer Object from Timer Queue
 *
 *	 @param       index	: Index of Timer Object
 *
 *   @return      None
 *****************************************************************************
 * Removes Timer Object from Timer Queue
 *****************************************************************************/
uint_8 RemoveTimerQ(uint_8 index)
{
	if(index >= MAX_TIMER_OBJECTS)
		return (uint_8)ERR_INVALID_PARAM;
	/* Disable Timer Interrupts */
	DisableTimerInterrupt();
	(void)memset(&g_TimerObjectArray[index], (int)NULL, sizeof(TIMER_OBJECT));
	/* Enable Timer Interrupts */
	EnableTimerInterrupt();
	return (uint_8)ERR_SUCCESS;
}

/******************************************************************************
 *   @name        TimerInit
 *
 *   @brief       This is RTC initialization function
 *
 *   @return      None
 *
 ******************************************************************************
 * Intiializes the RTC module registers
 *****************************************************************************/
static uint_8 TimerInit(void)
{
    PTAD_PTAD5 = 1;
    PTAD_PTAD6 = 1;   
    PTBD_PTBD0 = 1;
    PTADD_PTADD5 = 1;
    PTADD_PTADD6 = 1;
    PTBDD_PTBDD0 = 1;
    TPM2SC = 0;
    TPM2MOD = 1500;  
    TPM2SC_PS = 0x04;
    EnableTimerInterrupt();
    TPM2SC_CLKSx = 0x1;  /* Select Internal Clock Source */
    return ERR_SUCCESS;
}

/******************************************************************************
 *   @name        EnableTimerInterrupt
 *
 *   @brief       This routine enables Timer Interrupt
 *
 *   @return      None
 *
 ******************************************************************************
 * Enables RTC Timer Interrupt
 *****************************************************************************/
static void EnableTimerInterrupt(void)
{
	/* Enable Timer Interrupt */
    TPM2SC_TOIE = 0x1;
	return;
}

/******************************************************************************
 *   @name        DisableTimerInterrupt
 *
 *   @brief       This routine disables Timer Interrupt
 *
 *   @return      None
 *
 ******************************************************************************
 * Disables RTC Timer Interrupt
 *****************************************************************************/
static void DisableTimerInterrupt(void)
{
	/* Disable Timer Interrupt */
    TPM2SC_TOIE = 0x0;
	return;
}

/******************************************************************************
 *   @name        Timer_ISR
 *
 *   @brief       This routine services RTC Interrupt
 *
 *	 @param       None
 *
 *   @return      None
 *
 ******************************************************************************
 * Services RTC Interrupt. If a Timer Object expires, then removes the object 
 * from Timer Queue and Calls the callback function (if registered)
 *****************************************************************************/
static void TimerHandler(void);
#if (defined(_MC9S08MM128_H) || defined(_MC9S08JE128_H))
#pragma CODE_SEG NON_BANKED
#endif

void interrupt VectorNumber_Vtpm2ovf Timer_ISR(void)
{
    TimerHandler();
}

#if (defined(_MC9S08MM128_H) || defined(_MC9S08JE128_H))
#pragma CODE_SEG DEFAULT
#endif

#if (defined(_MC9S08MM128_H) || defined(_MC9S08JE128_H))
#pragma CODE_SEG NON_BANKED
#endif
void interrupt VectorNumber_Vtpm2ch0 Timer1_ISR(void)
{
    TimerHandler();
}
#if (defined(_MC9S08MM128_H) || defined(_MC9S08JE128_H))
#pragma CODE_SEG DEFAULT
#endif

static void TimerHandler(void)
{
	uint_8 index;
    if(TPM2SC & TPM1SC_TOF_MASK)
    {
		/* Clear RTC Interrupt */
        TPM2SC_TOF = 0x0;    
		/* Call Pending Timer CallBacks */
		for (index = 0; index < MAX_TIMER_OBJECTS; index++)
		{
			PTIMER_OBJECT ptemp = &g_TimerObjectArray[index];
			if(ptemp->pfnTimerCallback == NULL)
			{
				continue;
			}
			ptemp->msCount--;
			if (ptemp->msCount == 0) 
			{
			    PFNTIMER_CALLBACK pfnTimerCallback = ptemp->pfnTimerCallback;
#ifdef TIMER_CALLBACK_ARG
			    void *parg = ptemp->arg;
#endif
			    (void)RemoveTimerQ(index);
#ifdef TIMER_CALLBACK_ARG
				pfnTimerCallback(parg);
#else
				pfnTimerCallback();
#endif
			}
		}
	}
}
#endif
