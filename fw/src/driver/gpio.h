/*!
 (C) Copyright 2013 Ben Nahill
 
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
 
 @file
 @brief
 @author Ben Nahill <bnahill@gmail.com>
 */

#ifndef __APULSE_GPIO_H_
#define __APULSE_GPIO_H_

#include <derivative.h>

class GPIOPin {
public:
	typedef enum {
		MUX_ANALOG = 0,
		MUX_GPIO   = 1,
		MUX_ALT2   = 2,
		MUX_ALT3   = 3,
		MUX_ALT4   = 4,
		MUX_ALT5   = 5,
		MUX_ALT6   = 6,
		MUX_ALT7   = 7
	} mux_t;

	constexpr GPIOPin(GPIO_MemMapPtr gpio, uint32_t offset) :
		gpio(gpio),
		offset(offset)
	{}

	void set() const {
		gpio->PSOR = 1 << offset;
	}

	void clear() const {
		gpio->PCOR = 1 << offset;
	}

	void toggle() const {
		gpio->PTOR = 1 << offset;
	}

	bool read() const {
		return gpio->PDIR & (1 << offset);
	}

	void make_input() const {
		gpio->PDDR &= ~(1 << offset);
	}

	void make_output() const {
		gpio->PDDR |= 1 << offset;
	}

	void set_mux(mux_t mux) const {
		get_port()->PCR[offset] = (get_port()->PCR[offset] & ~PORT_PCR_MUX_MASK) |
								  (mux << PORT_PCR_MUX_SHIFT);
	}

	void configure(mux_t mux, bool strong_drive = false,
				   bool open_drain = false, bool filter = false,
				   bool slow_slew = false, bool pull_en = false,
				   bool pull_up = false) const {
		get_port()->PCR[offset] = (mux << PORT_PCR_MUX_SHIFT) |
				(strong_drive ? PORT_PCR_DSE_MASK : 0) |
				(open_drain ? PORT_PCR_ODE_MASK : 0) |
				(filter ? PORT_PCR_PFE_MASK : 0) |
				(slow_slew ? PORT_PCR_SRE_MASK : 0) |
				(pull_en ? PORT_PCR_PE_MASK : 0) |
				(pull_up ? PORT_PCR_PS_MASK : 0);
	}


protected:
	GPIO_MemMapPtr const gpio;
	uint32_t const offset;

	PORT_MemMapPtr get_port() const {
		return get_port(gpio);
	}

	static constexpr PORT_MemMapPtr get_port(GPIO_MemMapPtr gpio){
		return (gpio == PTA_BASE_PTR) ? PORTA_BASE_PTR :
			   ((gpio == PTB_BASE_PTR) ? PORTB_BASE_PTR :
			   ((gpio == PTC_BASE_PTR) ? PORTC_BASE_PTR :
			   ((gpio == PTD_BASE_PTR) ? PORTD_BASE_PTR :
			   ((gpio == PTE_BASE_PTR) ? PORTE_BASE_PTR : 0))));
	}
};

#endif // __APULSE_GPIO_H_
