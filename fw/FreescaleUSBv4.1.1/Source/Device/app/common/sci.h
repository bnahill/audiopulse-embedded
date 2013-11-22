#ifndef __SCI_H_
#define __SCI_H_
/**HEADER********************************************************************
* 
* Copyright (c) 2004 - 2010 Freescale Semiconductor;
* All Rights Reserved
*
*************************************************************************** 
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
**************************************************************************
*
* $FileName: sci.h$
* $Version : 
* $Date    : 
*
* Comments:
*
*   
*
*END************************************************************************/
#include "derivative.h"

#define BUFF_SIZE  200
#if (defined MCU_MK20D7) || (defined MCU_MK40D7) 
	#ifdef MCGOUTCLK_72_MHZ
		#define SYSTEM_CLOCK   ((uint_32)72000000)	
	#else
		#define SYSTEM_CLOCK   ((uint_32)48000000)
	#endif
#else
	#define SYSTEM_CLOCK   ((uint_32)48000000)
#endif
#define BUS_CLOCK      ((uint_32)(SYSTEM_CLOCK) / 2)

void sci2_init(void) ;
void sci1_init(void) ;
void sci_init(void) ;
void TERMIO_PutChar(char send);
void sci2_PutChar(char send);
char TERMIO_GetChar(void); 

#endif