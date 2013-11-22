/**HEADER********************************************************************
* 
* Copyright (c) 2010 Freescale Semiconductor;
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
* Comments:  This file includes operation of serial communication interface.
*
*END************************************************************************/
#include "types.h"
#include "sci.h"
#include "derivative.h"
#include "user_config.h"

#if (defined _MCF51MM256_H) || (defined _MCF51JE256_H) || (defined _MCF51JM128_H)
#include <Uart.h>
#endif

char   buff[200];
uint_32 buff_index;

#ifndef _MC9S08JS16_H
void interrupt VectorNumber_Vsci2rx SCI2RX_ISR(void);
#else
#define SCI1BDH     SCIBDH 
#define SCI1BDL     SCIBDL
#define SCI1C1      SCIC1 
#define SCI1C2      SCIC2  
#define SCI1S1      SCIS1
#define SCI1S2      SCIS2
#define SCI1C3      SCIC3
#define SCI1D       SCID
#define SCI1S1_TDRE SCIS1_TDRE
#define SCI1S1_RDRF SCIS1_RDRF
#endif


/*FUNCTION*-------------------------------------------------------------------
*
* Function Name    : sci_init
* Returned Value   :
* Comments         : This function initilizes the SCI 1 baud rate.
*    
*
*END*----------------------------------------------------------------------*/
void sci_init(void) 
{

#if (defined _MC9S08MM128_H) || (defined _MC9S08JE128_H)
  /* Enable Bus Clock for SCI1 */   
  SCGC1 |= SCGC1_SCI1_MASK;
  SOPT3 |= SOPT3_SCI1PS_MASK;
#endif  /* (defined _MC9S08MM128_H) */

 /* Configure SCI baud rate = 20K @16M */
  SCI1BDH = 0; 
  SCI1BDL = 0x0D;
  SCI1C1  = 0;
  SCI1C2  = 0x0C;
  SCI1S2  = 0x04;
}

/*FUNCTION*-------------------------------------------------------------------
*
* Function Name    : TERMIO_PutChar
* Returned Value   :
* Comments         :
*                     This function sends a char via SCI.
*
*END*----------------------------------------------------------------------*/
void TERMIO_PutChar(char send) 
{
  char dummy;
  
  while(!SCI1S1_TDRE){};
  dummy = (char)SCI1S1;
  SCI1D  = (unsigned char)send;    
}

/*FUNCTION*-------------------------------------------------------------------
*
* Function Name    : TERMIO_GetChar
* Returned Value   : the char get via SCI
* Comments         :
*                     This function gets a char via SCI.
*
*END*----------------------------------------------------------------------*/
char TERMIO_GetChar(void) 
{
  char dummy;
  
  while(!SCI1S1_RDRF){};
  dummy = (char)SCI1S1;
  return (char)SCI1D;  
}

#if (defined _MCF51MM256_H) || (defined _MCF51JE256_H) || (defined _MCF51JM128_H)
/*FUNCTION*-------------------------------------------------------------------
*
* Function Name    : Initialize UART
* Returned Value   :
* Comments         :
*                    This function initializes the UART for console printf/scanf
*
*END*----------------------------------------------------------------------*/
UARTError InitializeUART(UARTBaudRate baudRate)
{
 uint_32 	baud_divisor;
 
 /* Calculate baud settings */
 /* baud_divisor = clock_speed/ baudrate + 0.5 */
 baud_divisor = (BUS_CLOCK + (8 * (uint_32)baudRate)) / (16 * (uint_32)baudRate);
  
 if (baud_divisor > 0x1fff) 
 {
  return kUARTUnknownBaudRate;
 }
   
 SCI1BDH = (uint_8)((baud_divisor >> 8) & 0xFF);
 SCI1BDL = (uint_8)(baud_divisor & 0xFF);

 return kUARTNoError;
}

/*FUNCTION*-------------------------------------------------------------------
*
* Function Name    : WriteUARTN
* Returned Value   :
* Comments         :
*                    This function writes N bytes on the SCI1 console output
*
*END*----------------------------------------------------------------------*/
UARTError WriteUARTN(const void* bytes, unsigned long length) 
{
  int i;
  char* src = (char*)bytes;
  
  for(i = 0; i< length; i++) 
  {
   TERMIO_PutChar(*src++);
  }

 return kUARTNoError;  
}

/*FUNCTION*-------------------------------------------------------------------
*
* Function Name    : ReadUARTN
* Returned Value   :
* Comments         :
*                    This function reads N bytes on the SCI1 console input
*
*END*----------------------------------------------------------------------*/
UARTError ReadUARTN(void* bytes, unsigned long length)
{
  int i;
  char *dst = (char*)bytes;
  
  for(i = 0; i< length; i++) 
  {
   *dst++ = TERMIO_GetChar();
  }
  
  return kUARTNoError;
}

#endif  /* (defined _MCF51MM256_H) || (defined _MCF51JE256_H) || (defined _MCF51JM128_H) */

/* EOF */