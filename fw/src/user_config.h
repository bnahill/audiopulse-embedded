/******************************************************************************
 *
 * Freescale Semiconductor Inc.
 * (c) Copyright 2004-2009 Freescale Semiconductor, Inc.
 * Modifications (c) Copyright 2013 Ben Nahill
 * ALL RIGHTS RESERVED.
 *
 **************************************************************************//*!
 *
 * @file user_config.h
 *
 * @author
 *
 * @version
 *
 * @date May-28-2009
 *
 * @brief The file contains User Modifiable Macros for Keyboard Application
 *
 *****************************************************************************/
#include "derivative.h"

#define  HIGH_SPEED_DEVICE	(0)

#define MCGOUTCLK_48_MHZ


#define USE_HID_CLASS     (1)
#define USE_AUDIO_CLASS   (1)
#define COMPOSITE_DEV     (1)

/* Below two MACROS are required for Keyboard Application to execute */
#define LONG_SEND_TRANSACTION       /* support to send large data pkts */
#define LONG_RECEIVE_TRANSACTION    /* support to receive large data pkts */

#define USB_IN_PKT_SIZE 64			/* Define the maximum data length received from the host */
#define USB_OUT_PKT_SIZE 64			/* Define the maximum data length received from the host */

/* User Defined MACRO to set number of Timer Objects */
#define MAX_TIMER_OBJECTS		    5

#if MAX_TIMER_OBJECTS
/* When Enabled Timer Callback is invoked with an argument */
#define TIMER_CALLBACK_ARG
#undef TIMER_CALLBACK_ARG
#endif

//#ifndef _MC9S08JS16_H
#define USB_PACKET_SIZE  uint_16 /* support 16/32 bit packet size */
//#else
//#define USB_PACKET_SIZE  uint_8 /* support 8 bit packet size */
//#endif

#ifndef _MCF51JM128_H
/* Use double buffered endpoints 5 & 6. To be only used with S08 cores */
#define DOUBLE_BUFFERING_USED
#endif



