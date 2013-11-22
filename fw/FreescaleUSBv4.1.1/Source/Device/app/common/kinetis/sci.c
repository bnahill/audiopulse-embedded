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
*
* Comments:  This file includes operation of serial communication interface.
*
*
*END************************************************************************/
#include "sci.h"
#include "types.h"
#include "derivative.h"
#include "user_config.h"

char   buff[200];
uint_32 buff_index;

#ifndef MCU_MK70F12
	#if (defined MCU_MK20D7) || (defined MCU_MK40D7)
		#ifdef MCGOUTCLK_72_MHZ
			#define SYSCLK 72000
		#else
			#define SYSCLK 48000
		#endif
	#else
		#define SYSCLK 48000
	#endif
#else
#define SYSCLK 60000 // see periph_clk_khz in Init_Sys()
#endif
#define UART_BAUDRATE 115200

/*FUNCTION*-------------------------------------------------------------------
*
* Function Name    : sci1_init
* Returned Value   :
* Comments         : This function initilizes the SCI 1 baud rate.
*    
*
*END*----------------------------------------------------------------------*/
void sci_init(void) 
{
#if (defined MCU_MK20D5) || (defined MCU_MK20D7) || (defined MCU_MK40D7)
	register uint_16 ubd;
	int system_clk_khz;
	
	/* Calculate the system clock value expressed in kHz */
	system_clk_khz = SYSCLK / (((SIM_CLKDIV1 & SIM_CLKDIV1_OUTDIV1_MASK) >> 28)+ 1);
	
	/* Enable all of the port clocks. These have to be enabled to configure
     * pin muxing options, so most code will need all of these on anyway.
     */    
    SIM_SCGC5 |= (SIM_SCGC5_PORTA_MASK
                  | SIM_SCGC5_PORTB_MASK
                  | SIM_SCGC5_PORTC_MASK
                  | SIM_SCGC5_PORTD_MASK
                  | SIM_SCGC5_PORTE_MASK );
	
	/* Enable the UART1_TXD function on PTE0 */
	PORTE_PCR0 = PORT_PCR_MUX(0x3); // UART is alt3 function for this pin

	/* Enable the UART1_RXD function on PTE1 */
	PORTE_PCR1 = PORT_PCR_MUX(0x3); // UART is alt3 function for this pin
	
	SIM_SCGC4 |= SIM_SCGC4_UART1_MASK;
	
	UART1_C2 &= ~(UART_C2_TE_MASK | UART_C2_RE_MASK );
	UART1_C1 = 0;
	
	ubd = (uint_16)((system_clk_khz * 1000)/(UART_BAUDRATE * 16));
	
	UART1_BDH |= UART_BDH_SBR(((ubd & 0x1F00) >> 8));
	UART1_BDL = (uint_8)(ubd & UART_BDL_SBR_MASK);
	
	UART1_C2 |= (UART_C2_TE_MASK | UART_C2_RE_MASK );

#elif defined MCU_MK21D5
	register uint_16 ubd;
	int system_clk_khz;

	/* Calculate the system clock value expressed in kHz */
	system_clk_khz = SYSCLK / (((SIM_CLKDIV1 & SIM_CLKDIV1_OUTDIV1_MASK) >> 28)+ 1);
	
	/* Enable all of the port clocks. These have to be enabled to configure
     * pin muxing options, so most code will need all of these on anyway.
     */    
    SIM_SCGC5 |= (	SIM_SCGC5_PORTA_MASK
                  | SIM_SCGC5_PORTB_MASK
                  | SIM_SCGC5_PORTC_MASK
                  | SIM_SCGC5_PORTD_MASK
                  | SIM_SCGC5_PORTE_MASK );

    /* Enable the UART2_TXD function on PTE16 */
    PORTE_PCR16 = PORT_PCR_MUX(0x03);

    /* Enable the UART2_RXD function on PTE17 */
    PORTE_PCR17 = PORT_PCR_MUX(0x03);

    SIM_SCGC4 |= SIM_SCGC4_UART2_MASK;

    UART2_C2 &= ~(UART_C2_TE_MASK | UART_C2_RE_MASK );
    UART2_C1 = 0;

    ubd = (uint_16)((system_clk_khz * 1000)/(UART_BAUDRATE * 16));

	UART2_BDH |= UART_BDH_SBR(((ubd & 0x1F00) >> 8));
	UART2_BDL = (uint_8)(ubd & UART_BDL_SBR_MASK);

    UART2_C2 |= (UART_C2_TE_MASK | UART_C2_RE_MASK );
#elif(defined MCU_MKL25Z4)
	register uint_16 ubd;
	
	SIM_SOPT2 |= SIM_SOPT2_UART0SRC(1);
	
	/* Enable all of the port clocks. These have to be enabled to configure
     * pin muxing options, so most code will need all of these on anyway.
     */    
    SIM_SCGC5 |= (	SIM_SCGC5_PORTA_MASK
                  | SIM_SCGC5_PORTB_MASK
                  | SIM_SCGC5_PORTC_MASK
                  | SIM_SCGC5_PORTD_MASK
                  | SIM_SCGC5_PORTE_MASK );
	
	/* UART port config for TWR-KL25Z48M */
    /* Enable the UART2_TXD function on PTE14 */
	//PORTA_PCR14 = PORT_PCR_MUX(0x03); // UART is alt3 function for this pin

	/* Enable the UART2_RXD function on PTE15 */
	//PORTA_PCR15 = PORT_PCR_MUX(0x03); // UART is alt3 function for this pin
	
	/* UART port config for FRDM-KL25Z */
	/* PORTA_PCR1: ISF=0,MUX=2 */
	PORTA_PCR1 = (uint32_t)((PORTA_PCR1 & (uint32_t)~(uint32_t)(
	              PORT_PCR_ISF_MASK |
	              PORT_PCR_MUX(0x05)
	             )) | (uint32_t)(
	              PORT_PCR_MUX(0x02)
	             ));                                                  
	/* PORTA_PCR2: ISF=0,MUX=2 */
	PORTA_PCR2 = (uint32_t)((PORTA_PCR2 & (uint32_t)~(uint32_t)(
	              PORT_PCR_ISF_MASK |
	              PORT_PCR_MUX(0x05)
	             )) | (uint32_t)(
	              PORT_PCR_MUX(0x02)
	             )); 
	
	
	SIM_SCGC4 |= SIM_SCGC4_UART0_MASK;
	
	UART0_C2 &= ~(UART_C2_TE_MASK | UART_C2_RE_MASK );
	UART0_C1 = 0;
	
	ubd = (uint_16)((48000 * 1000)/(UART_BAUDRATE * 16));
	
	UART0_BDH = UART_BDH_SBR(((ubd & 0x1F00) >> 8));
	UART0_BDL = (uint_8)(ubd & UART_BDL_SBR_MASK);
	UART0_C2 |= (UART_C2_TE_MASK | UART_C2_RE_MASK );
#elif defined MCU_MK70F12
    register uint_16 sbr, brfa;
    uint_8 temp;

    /* Enable the UART2_TXD function on PTD3 */
    PORTE_PCR16 = PORT_PCR_MUX(0x3); // UART is alt3 function for this pin

    /* Enable the UART2_RXD function on PTD2 */
    PORTE_PCR17 = PORT_PCR_MUX(0x3); // UART is alt3 function for this pin

    /* Enable the clock  */ 
    SIM_SCGC4 |= SIM_SCGC4_UART2_MASK;

    /* Make sure that the transmitter and receiver are disabled while we 
     * change settings.
     */
    UART_C2_REG(UART2_BASE_PTR) &= ~(UART_C2_TE_MASK
            | UART_C2_RE_MASK );

    /* Configure the UART for 8-bit mode, no parity */
    UART_C1_REG(UART2_BASE_PTR) = 0;    /* We need all default settings, so entire register is cleared */

    /* Calculate baud settings */
    sbr = (uint_16)((SYSCLK*1000)/(UART_BAUDRATE * 16));

    /* Save off the current value of the UARTx_BDH except for the SBR field */
    temp = UART_BDH_REG(UART2_BASE_PTR) & ~(UART_BDH_SBR(0x1F));

    UART_BDH_REG(UART2_BASE_PTR) = temp |  UART_BDH_SBR(((sbr & 0x1F00) >> 8));
    UART_BDL_REG(UART2_BASE_PTR) = (uint_8)(sbr & UART_BDL_SBR_MASK);

    /* Determine if a fractional divider is needed to get closer to the baud rate */
    brfa = (((SYSCLK*32000)/(UART_BAUDRATE * 16)) - (sbr * 32));

    /* Save off the current value of the UARTx_C4 register except for the BRFA field */
    temp = UART_C4_REG(UART2_BASE_PTR) & ~(UART_C4_BRFA(0x1F));

    UART_C4_REG(UART2_BASE_PTR) = temp |  UART_C4_BRFA(brfa);    

    /* Enable receiver and transmitter */
    UART_C2_REG(UART2_BASE_PTR) |= (UART_C2_TE_MASK
            | UART_C2_RE_MASK );
#else 	
	register uint_16 ubd;
	int periph_clk_khz;  
	
	/* Calculate the peripheral clock value expressed in kHz */
	periph_clk_khz = SYSCLK / (((SIM_CLKDIV1 & SIM_CLKDIV1_OUTDIV2_MASK) >> 24)+ 1);
	
	/* Enable all of the port clocks. These have to be enabled to configure
     * pin muxing options, so most code will need all of these on anyway.
     */    
	SIM_SCGC5 |= (SIM_SCGC5_PORTA_MASK
                  | SIM_SCGC5_PORTB_MASK
                  | SIM_SCGC5_PORTC_MASK
                  | SIM_SCGC5_PORTD_MASK
                  | SIM_SCGC5_PORTE_MASK );
	
    /* Enable the UART3_TXD function on PTC17 */
	PORTC_PCR17 = PORT_PCR_MUX(0x3); // UART is alt3 function for this pin

	/* Enable the UART3_RXD function on PTC16 */
	PORTC_PCR16 = PORT_PCR_MUX(0x3); // UART is alt3 function for this pin
	
	SIM_SCGC4 |= SIM_SCGC4_UART3_MASK;
	
	UART3_C2 &= ~(UART_C2_TE_MASK | UART_C2_RE_MASK );
	UART3_C1 = 0;
	
	ubd = (uint_16)((periph_clk_khz * 1000)/(UART_BAUDRATE * 16));
	
	UART3_BDH |= UART_BDH_SBR(((ubd & 0x1F00) >> 8));
	UART3_BDL = (uint_8)(ubd & UART_BDL_SBR_MASK);
	
	UART3_C2 |= (UART_C2_TE_MASK | UART_C2_RE_MASK );
#endif
}

/*FUNCTION*-------------------------------------------------------------------
*
* Function Name    : TERMIO_PutChar
* Returned Value   :
* Comments         :
*                     This function sends a char via SCI.
*
*END*----------------------------------------------------------------------*/
#ifdef __CC_ARM
int sendchar (int ch)
#else
void uart_putchar (char ch)
#endif
{
#if(defined MCU_MK20D5) || (defined MCU_MK20D7) || (defined MCU_MK40D7) 
    /* Wait until space is available in the FIFO */
    while(!(UART1_S1 & UART_S1_TDRE_MASK)){};

    /* Send the character */
    UART1_D = (uint_8)ch;	
#elif (defined MCU_MK21D5)
    /* Wait until space is available in the FIFO */
    while(!(UART2_S1 & UART_S1_TDRE_MASK)){};

    /* Send the character */
    UART2_D = (uint_8)ch;	
#elif(defined MCU_MKL25Z4)
    /* Wait until space is available in the FIFO */
    while(!(UART0_S1 & UART_S1_TDRE_MASK)){};

    /* Send the character */
    UART0_D = (uint_8)ch;
#elif defined MCU_MK70F12
    /* Wait until space is available in the FIFO */
    while(!(UART2_S1 & UART_S1_TDRE_MASK)){};

    /* Send the character */
    UART2_D = (uint_8)ch;
#else   
    /* Wait until space is available in the FIFO */
    while(!(UART3_S1 & UART_S1_TDRE_MASK)){};

    /* Send the character */
    UART3_D = (uint_8)ch;
#endif
#ifdef __CC_ARM
    return 0;
#endif    
}

/*FUNCTION*-------------------------------------------------------------------
*
* Function Name    : TERMIO_GetChar
* Returned Value   : the char get via SCI
* Comments         :
*                     This function gets a char via SCI.
*
*END*----------------------------------------------------------------------*/
#ifdef __CC_ARM
int getkey(void)
#else
char uart_getchar (void)
#endif
{
#if (defined MCU_MK20D5) || (defined MCU_MK20D7) || (defined MCU_MK40D7) 
    /* Wait until character has been received */
    while (!(UART1_S1 & UART_S1_RDRF_MASK));

    /* Return the 8-bit data from the receiver */
    return UART1_D;	
#elif (defined MCU_MK21D5)
    while (!(UART2_S1 & UART_S1_RDRF_MASK));

    /* Return the 8-bit data from the receiver */
    return UART2_D;	
#elif(defined MCU_MKL25Z4)
    while (!(UART0_S1 & UART_S1_RDRF_MASK));

    /* Return the 8-bit data from the receiver */
    return UART0_D;
#elif defined MCU_MK70F12
    /* Wait until character has been received */
    while (!(UART2_S1 & UART_S1_RDRF_MASK));

    /* Send the character */
    return UART2_D;
#else
    /* Wait until character has been received */
    while (!(UART3_S1 & UART_S1_RDRF_MASK));

    /* Return the 8-bit data from the receiver */
    return UART3_D;
#endif
}

/********************************************************************/
void out_char (char ch)
{
#ifndef __CC_ARM
	uart_putchar(ch);
#endif
}


/*FUNCTION*-------------------------------------------------------------------
*
* Function Name    : sci2_PutChar 
* Returned Value   :
* Comments         : send a charactor through UART2
*    
*
*END*----------------------------------------------------------------------*/
void sci2_PutChar(char send)
{

}
