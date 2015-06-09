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

protected:
	friend class SPI;

	SPI &spi;
	uint32_t CTAR;
	GPIOPin const NCS;// = {PTD_BASE_PTR, 0};
	uint32_t const mux;
	//GPIOPin::mux_t const mux;// = GPIOPin::MUX_ALT2;
	uint32_t ncs_index;
	uint32_t index;
};

class SPI {
public:
	SPI(SPI_MemMapPtr spi, GPIOPin const MOSI, GPIOPin::mux_t MOSI_mux,
	    GPIOPin const MISO, GPIOPin::mux_t MISO_mux,
	    GPIOPin const SCLK, GPIOPin::mux_t SCLK_mux,
		uint32_t dma_ch_tx, uint32_t dma_ch_rx) :
		nslaves(0), spi(spi), MOSI(MOSI), MOSI_mux(MOSI_mux),
		MISO(MISO), MISO_mux(MISO_mux), SCLK(SCLK), SCLK_mux(SCLK_mux),
		dma_ch_tx(dma_ch_tx), dma_ch_rx(dma_ch_rx),
		is_init(false), busy(true)
	{


	}

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

		/*
		So the AK4621 specifies that the first clock edge it sees is rising and
		that it will register on that rising edge. CPOL=CPHA=0
		*/
		//SPI->CTAR[0] =
		//	SPI_CTAR_FMSZ(15) |     // 16 bit frames
		//	SPI_CTAR_CPOL_MASK |   // Clock idle high
		//	SPI_CTAR_CPHA_MASK |   // Sample following edge
		//	SPI_CTAR_PCSSCK(2) |   // PCS to SCK prescaler = 5
		//	SPI_CTAR_PASC(2)   |   // Same delay before end of PCS
		//	SPI_CTAR_PDT(3)    |   // Same delay after end of PCS
		//	SPI_CTAR_PBR(0)    |   // Use sysclk / 2
		//	SPI_CTAR_CSSCK(6)  |   // Base delay is 128*Tsys
		//	SPI_CTAR_ASC(3)    |   // Unit delay before end of PCS
		//	SPI_CTAR_DT(3)     |   // Same after end of PCS
		//	SPI_CTAR_BR(8);        // Scale by 256

		spi->MCR =
				SPI_MCR_MSTR_MASK |    // Master
				SPI_MCR_DCONF(0) |     // SPI mode
				SPI_MCR_PCSIS(1) |     // All nCS idle high
				SPI_MCR_DIS_RXF_MASK | // No RX FIFO
		        SPI_MCR_DIS_TXF_MASK | // No TX FIFO
				SPI_MCR_SMPL_PT(0);    // Sample on clock edge

		dma_init();

		is_init = true;
		busy = false;
	}

	void dma_init(){
		// General DMA setup
		DMA_CR =
			DMA_CR_EMLM_MASK |      // Enable minor looping
			DMA_CR_ERCA_MASK;       // Enable RR arbitration

		DMAMUX_CHCFG_REG(DMAMUX, dma_ch_tx) = 0;
		DMAMUX_CHCFG_REG(DMAMUX, dma_ch_rx) = 0;

		//
		// Set up RX channel
		//

		DMA_DADDR_REG(DMA0, dma_ch_rx) = (uint32_t)0; // Transmit FIFO
		DMA_SADDR_REG(DMA0, dma_ch_rx) = (uint32_t)&spi->POPR;
		DMA_SOFF_REG(DMA0, dma_ch_rx) = 1; // Increment source!
		DMA_DOFF_REG(DMA0, dma_ch_rx) = 0; // Don't increment destination!
		DMA_ATTR_REG(DMA0, dma_ch_rx) = DMA_ATTR_SMOD(0) | DMA_ATTR_SSIZE(0) | // 8-bit src
					                    DMA_ATTR_DMOD(0) | DMA_ATTR_DSIZE(0);  // 8-bit dst
		DMA_NBYTES_MLNO_REG(DMA0, dma_ch_rx) = 1;
		DMA_NBYTES_MLOFFNO_REG(DMA0, dma_ch_rx) = 1;
		DMA_NBYTES_MLOFFYES_REG(DMA0, dma_ch_rx) = 1;
		// CITER and BITER must be the same
		DMA_CITER_ELINKNO_REG(DMA0, dma_ch_rx) = DMA_CITER_ELINKNO_CITER(0);
		DMA_BITER_ELINKNO_REG(DMA0, dma_ch_rx) = DMA_BITER_ELINKNO_BITER(0);
		DMA_SLAST_REG(DMA0, dma_ch_rx) = 0;

		DMA_DLAST_SGA_REG(DMA0, dma_ch_rx) = 0;

		DMA_CSR_REG(DMA0, dma_ch_rx) = DMA_CSR_INTMAJOR_MASK | // Major loop IRQ
					DMA_CSR_INTHALF_MASK | // Also at half
					DMA_CSR_BWC(0); // 0-cycle delay

		DMA_SERQ = DMA_SERQ_SERQ(dma_ch_rx); // Enable ch 0 requests

		//
		// Set up TX channel
		//

		DMA_DADDR_REG(DMA0, dma_ch_tx) = (uint32_t)&spi->PUSHR; // Transmit FIFO
		DMA_SADDR_REG(DMA0, dma_ch_tx) = (uint32_t)0;
		DMA_SOFF_REG(DMA0, dma_ch_tx) = 1; // Increment source!
		DMA_DOFF_REG(DMA0, dma_ch_tx) = 0; // Don't increment destination!
		DMA_ATTR_REG(DMA0, dma_ch_tx) =
			DMA_ATTR_SMOD(0) | DMA_ATTR_SSIZE(0) | // 8-bit src
			DMA_ATTR_DMOD(0) | DMA_ATTR_DSIZE(0);  // 8-bit dst
		DMA_NBYTES_MLNO_REG(DMA0, dma_ch_tx) = 1;
		DMA_NBYTES_MLOFFNO_REG(DMA0, dma_ch_tx) = 1;
		DMA_NBYTES_MLOFFYES_REG(DMA0, dma_ch_tx) = 1;
		// CITER and BITER must be the same
		DMA_CITER_ELINKNO_REG(DMA0, dma_ch_tx) = DMA_CITER_ELINKNO_CITER(0);
		DMA_BITER_ELINKNO_REG(DMA0, dma_ch_tx) = DMA_BITER_ELINKNO_BITER(0);
		DMA_SLAST_REG(DMA0, dma_ch_tx) = 0;

		DMA_DLAST_SGA_REG(DMA0, dma_ch_tx) = 0;

		// No interrupt from TX channel
		DMA_CSR_REG(DMA0, dma_ch_tx) = DMA_CSR_BWC(0); // 0-cycle delay

		DMA_SERQ = DMA_SERQ_SERQ(dma_ch_tx); // Enable ch 0 requests


		//
		// Connect the DMA sources
		//

		if(spi == SPI0_BASE_PTR){
			DMAMUX_CHCFG_REG(DMAMUX, dma_ch_tx) = DMAMUX_CHCFG_SOURCE(15) |
								                  DMAMUX_CHCFG_ENBL_MASK;
			DMAMUX_CHCFG_REG(DMAMUX, dma_ch_rx) = DMAMUX_CHCFG_SOURCE(14) |
								                  DMAMUX_CHCFG_ENBL_MASK;
		} else if(spi == SPI1_BASE_PTR){
			DMAMUX_CHCFG_REG(DMAMUX, dma_ch_tx) = DMAMUX_CHCFG_SOURCE(16) |
								                  DMAMUX_CHCFG_ENBL_MASK;
			DMAMUX_CHCFG_REG(DMAMUX, dma_ch_rx) = DMAMUX_CHCFG_SOURCE(16) |
						                          DMAMUX_CHCFG_ENBL_MASK;
		}

		DMA_CSR_REG(DMA0, dma_ch_rx) |= DMA_CSR_ACTIVE_MASK; // Set active
		DMA_CSR_REG(DMA0, dma_ch_tx) |= DMA_CSR_ACTIVE_MASK; // Set active

		spi->RSER = SPI_RSER_RFDF_RE_MASK | SPI_RSER_RFDF_DIRS_MASK |
		            SPI_RSER_TFFF_RE_MASK | SPI_RSER_TFFF_DIRS_MASK;
	}

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
	              spi_cb_t cb, void * arg){
		static uint8_t zero = 0;
		static uint8_t dummy_dest = 0;

		// Check to make sure slave is assigned
		if(slave.index == ~0)
			return false;

		if((not rx_buf) and (not tx_buf))
			return false;

		if(busy)
			return false;

		if(count == 0)
			return true;

		busy = true;

		current_cb = cb;
		current_arg = arg;

		DMA_CERR = DMA_CERR_CERR(dma_ch_rx);
		DMA_CINT = DMA_CINT_CINT(dma_ch_rx);

		DMA_CITER_ELINKNO_REG(DMA0, dma_ch_rx) = DMA_CITER_ELINKNO_CITER(count);
		DMA_BITER_ELINKNO_REG(DMA0, dma_ch_rx) = DMA_BITER_ELINKNO_BITER(count);
		DMA_SLAST_REG(DMA0, dma_ch_rx) = -((int32_t)count);

		// Start receive
		DMA_SSRT = DMA_SSRT_SSRT(dma_ch_rx);

		NVIC_EnableIRQ((IRQn_Type)dma_ch_rx);

		if(rx_buf){
			DMA_DADDR_REG(DMA0, dma_ch_rx) = (uint32_t)rx_buf;
			DMA_DOFF_REG(DMA0, dma_ch_rx) = 1; // Increment dest!
		} else {
			DMA_DADDR_REG(DMA0, dma_ch_rx) = (uint32_t)&dummy_dest;
			DMA_DOFF_REG(DMA0, dma_ch_rx) = 0; // Increment source!
		}


		DMA_CERR = DMA_CERR_CERR(dma_ch_tx);
		DMA_CINT = DMA_CINT_CINT(dma_ch_tx);

		DMA_CITER_ELINKNO_REG(DMA0, dma_ch_tx) = DMA_CITER_ELINKNO_CITER(count - 1);
		DMA_BITER_ELINKNO_REG(DMA0, dma_ch_tx) = DMA_BITER_ELINKNO_BITER(count - 1);
		DMA_SLAST_REG(DMA0, dma_ch_tx) = -((int32_t)count - 1);


		if(tx_buf){
			// Point to actual buffer
			DMA_SADDR_REG(DMA0, dma_ch_tx) = (uint32_t)tx_buf + 1;
			DMA_SOFF_REG(DMA0, dma_ch_tx) = 1; // Increment source!
		} else {
			// Point to a zero and send that
			DMA_SOFF_REG(DMA0, dma_ch_tx) = 0; // Don't increment source!
			DMA_SADDR_REG(DMA0, dma_ch_tx) = (uint32_t)&zero;
			tx_buf = &zero;
		}

		spi->PUSHR = SPI_PUSHR_CONT_MASK |
		             SPI_PUSHR_CTAS(slave.index) |
		             SPI_PUSHR_PCS(1 << slave.ncs_index) |
		             SPI_PUSHR_TXDATA(tx_buf[0]);

		// Start receive
		DMA_SSRT = DMA_SSRT_SSRT(dma_ch_tx);

		if(not current_cb){
			// Wait for it!
			while(busy);
		}


		return true;
	}

	void handle_complete_isr(){
		busy = 0;

		if(current_cb){
			current_cb(current_arg);
		}

		// Clear all possible interrupts
		DMA_CINT = DMA_CINT_CINT(dma_ch_rx);
		NVIC_ClearPendingIRQ((IRQn_Type)dma_ch_rx);
		DMA_CINT = DMA_CINT_CINT(dma_ch_tx);
		NVIC_ClearPendingIRQ((IRQn_Type)dma_ch_tx);
	}

protected:
	uint32_t nslaves;

	uint32_t dma_ch_tx, dma_ch_rx;

	spi_cb_t current_cb;
	void *   current_arg;

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
