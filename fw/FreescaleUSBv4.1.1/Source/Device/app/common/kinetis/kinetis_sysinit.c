/*
 *    kinetis_sysinit.c - Default init routines for
 *                     		Kinetis ARM systems
 *    Copyright © 2010 Freescale semiConductor Inc. All Rights Reserved.
 */

#include "kinetis_sysinit.h"
#include "derivative.h"

#ifndef __GNUC__
	#pragma overload void __init_hardware();
#endif

void __init_hardware()
{
	/* 
		Disable the Watchdog because it may reset the core before entering main().
		There are 2 unlock words which shall be provided in sequence before  
		accessing the control register.
	*/
#ifdef MCU_MKL25Z4
    SIM_COPC = 0x00;
#else
	*((volatile unsigned short *)KINETIS_WDOG_UNLOCK_ADDR)	= KINETIS_WDOG_UNLOCK_SEQ_1;                
	*((volatile unsigned short *)KINETIS_WDOG_UNLOCK_ADDR)	= KINETIS_WDOG_UNLOCK_SEQ_2;
	*((volatile unsigned short *)KINETIS_WDOG_STCTRLH_ADDR)	= KINETIS_WDOG_DISABLED_CTRL;               
#endif
}
