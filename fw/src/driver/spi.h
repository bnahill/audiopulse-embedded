/*!
 (C) Copyright 2015 Ben Nahill

 This program is free software: you can redistribute it and/or modify
 it under the terms of the GNU General Public License as published by
 the Free Software Foundation, either version 3 of the License, or
 (at your option) any later version.

 This program is distributed in the hope that it will be useful,
 but WITHOUT ANY WARRANTY; without even the implied warranty of
 MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 GNU General Public License for more details.

 You should have received a copy of the GNU General Public License
 along with this program.  If not, see <http://www.gnu.org/licenses/>.

 @file spi.h
 @brief
 @author Ben Nahill <bnahill@gmail.com>
 */

#ifndef __APULSE_SPI_H_
#define __APULSE_SPI_H_

#include <derivative.h>

#include <stdint.h>
#include <driver/gpio.h>
#include "clocks.h"
#include <apulse_math.h>

class SPI;

class SPI_slave {
public:
	constexpr SPI_slave(SPI &spi, GPIOPin NCS, GPIOPin::mux_t NCS_mux, uint32_t CTAR,
	          uint32_t ncs_index) :
	    spi(spi), NCS(NCS), mux((GPIOPin::mux_t)2), CTAR(CTAR), ncs_index(ncs_index),
	    index(~0){
	}
	void init();

    SPI &spi;

protected:
	friend class SPI;

	uint32_t CTAR;
	GPIOPin const NCS;// = {PTD_BASE_PTR, 0};
	uint32_t const mux;
	//GPIOPin::mux_t const mux;// = GPIOPin::MUX_ALT2;
	uint32_t ncs_index;
	uint32_t index;
};

/*!
 * \brief A class for simple serial communication
 */
class SPI {
public:
    constexpr SPI(SPI_MemMapPtr spi, GPIOPin const MOSI, GPIOPin::mux_t MOSI_mux,
	    GPIOPin const MISO, GPIOPin::mux_t MISO_mux,
	    GPIOPin const SCLK, GPIOPin::mux_t SCLK_mux,
		uint32_t dma_ch_tx, uint32_t dma_ch_rx) :
		nslaves(0), spi(spi), MOSI(MOSI), MOSI_mux(MOSI_mux),
		MISO(MISO), MISO_mux(MISO_mux), SCLK(SCLK), SCLK_mux(SCLK_mux),
		dma_ch_tx(dma_ch_tx), dma_ch_rx(dma_ch_rx),
        current_cb(nullptr), current_arg(nullptr), current_slave(nullptr),
		is_init(false), busy(true)
	{


	}

    /*!
     * \brief Initialize all hardware necessary as well as internal data
     *        structures
     */
	void init(){
		if(is_init)
			return;

		if(spi == SPI0_BASE_PTR){
			SIM_SCGC6 |= SIM_SCGC6_SPI0_MASK;
		} else if(spi == SPI1_BASE_PTR){
			SIM_SCGC6 |= SIM_SCGC6_SPI1_MASK;
		} else {
			while(true);
		}

		SCLK.configure(SCLK_mux, true);
		MISO.configure(MISO_mux, true);
		MOSI.configure(MOSI_mux, true);

        spi->MCR = SPI_MCR_HALT_MASK;

		dma_init();

		is_init = true;
		busy = false;
	}

    void dma_init();

	void register_slave(SPI_slave &slave);

	typedef void (*spi_cb_t)(void *);

	/*!
	 @brief Transfer a bunch of data over SPI
	 @param slave The SPI slave to use
	 @param tx_buf The source of data to write (null if zeros)
	 @param rx_buf The destination for data being read (null if zeros)
	 @param count The number of bytes to transfer at once
	 @param cb Callback (null if blocking)
	 @param arg Callback argument
	 */
	bool transfer(SPI_slave &slave,
	              uint8_t const * tx_buf, uint8_t * rx_buf,
	              uint32_t count,
                  spi_cb_t cb, void * arg);

	void handle_complete_isr(){
        volatile uint16_t errs;

		busy = 0;

        errs = DMA_ERR;

        if(current_slave)
            current_slave->NCS.set();

        spi->MCR |= SPI_MCR_HALT_MASK;

		if(current_cb){
			current_cb(current_arg);
		}

		// Clear all possible interrupts
		DMA_CINT = DMA_CINT_CINT(dma_ch_rx);
		NVIC_ClearPendingIRQ((IRQn_Type)dma_ch_rx);
        //DMA_CINT = DMA_CINT_CINT(dma_ch_tx);
        //NVIC_ClearPendingIRQ((IRQn_Type)dma_ch_tx);
	}

protected:
	uint32_t nslaves;

	uint32_t dma_ch_tx, dma_ch_rx;

	spi_cb_t current_cb;
	void *   current_arg;
    SPI_slave * current_slave;

	bool busy;
	bool is_init;

	SPI_MemMapPtr const spi;// = SPI0_BASE_PTR;
	GPIOPin const MOSI;// = {PTD_BASE_PTR, 2};
	GPIOPin::mux_t MOSI_mux;// = GPIOPin::MUX_ALT2;
	GPIOPin const MISO;// = {PTD_BASE_PTR, 2};
	GPIOPin::mux_t MISO_mux;// = GPIOPin::MUX_ALT2;
	GPIOPin const SCLK;// = {PTD_BASE_PTR, 1};
	GPIOPin::mux_t SCLK_mux;// = GPIOPin::MUX_ALT2;

};



#endif // __APULSE_SPI_H_
