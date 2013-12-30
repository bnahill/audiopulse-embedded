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

#ifndef __APULSE_TPA6130_H_
#define __APULSE_TPA6130_H_

#include <driver/gpio.h>

class TPA6130A2 {
public:
	/*!
	 @brief 
	 */
	static void init(){
		nSD.clear();
		nSD.configure(GPIOPin::MUX_GPIO);
		nSD.make_output();
		
		SCL.configure(SCL_mux, true, true);
		SDA.configure(SDA_mux, true, true);
		
		if(I2C == I2C0_BASE_PTR){
			SIM_SCGC4 |= SIM_SCGC4_I2C0_MASK;
		} else if(I2C == I2C1_BASE_PTR){
			SIM_SCGC4 |= SIM_SCGC4_I2C1_MASK;
		}
		
		I2C->F = I2C_F_MULT(2) |  // / 4  = 12MHz
				 I2C_F_ICR(0x3C); // / 60 = 200kHz
		I2C->C1 = I2C_C1_IICEN_MASK |
		          I2C_C1_TX_MASK;
		
	}
	
	static void enable(){
		nSD.set();
		// ENABLE IT
		write_reg(1, 0xC0);
		// Half volume
		write_reg(2, 32);
	}
	
	static void disable(){
		nSD.clear();
	}
	
protected:
	
	static void write_reg(uint8_t addr, uint8_t val);
	
	static constexpr GPIOPin nSD = {PTB_BASE_PTR, 2};
	static constexpr GPIOPin SDA = {PTB_BASE_PTR, 1};
	static constexpr GPIOPin::mux_t SDA_mux = GPIOPin::MUX_ALT2;
	static constexpr GPIOPin SCL = {PTB_BASE_PTR, 0};
	static constexpr GPIOPin::mux_t SCL_mux = GPIOPin::MUX_ALT2;

	static constexpr I2C_MemMapPtr I2C = I2C0_BASE_PTR;
	
	static constexpr uint8_t i2c_addr = 0b1100000 << 1;
};

#endif // __APULSE_TPA6130_H_
