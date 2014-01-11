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

#include <driver/tpa6130a2.h>

bool TPA6130A2::enabled = false;
uint8_t TPA6130A2::volume = 26;

void TPA6130A2::init(){
	if(I2C == I2C0_BASE_PTR){
		SIM_SCGC4 |= SIM_SCGC4_I2C0_MASK;
	} else if(I2C == I2C1_BASE_PTR){
		SIM_SCGC4 |= SIM_SCGC4_I2C1_MASK;
	}

	nSD.clear();
	nSD.configure(GPIOPin::MUX_GPIO);
	nSD.make_output();

	SCL.configure(SCL_mux, true, true);
	SDA.configure(SDA_mux, true, true);

	I2C->F = I2C_F_MULT(0) |  // / 1 = 48MHz
	         I2C_F_ICR(0x27); // / 480 = 100kHz

	I2C->C1 = I2C_C1_IICEN_MASK;

	nSD.set();

	// ENABLE IT
	write_reg(1, 0xC0);
	// Half volume
	write_reg(2, 20);

	enable();
}

void TPA6130A2::enable(){
	if(!enabled){
		nSD.set();
		// ENABLE IT
		write_reg(1, 0xC0);
		// Half volume
		write_reg(2, volume);

		// Always enabled for now...
		enabled = true;
	}
}

void TPA6130A2::disable(){
	nSD.clear();
	enabled = false;
}

void TPA6130A2::write_reg (uint8_t addr, uint8_t val ) {
	I2C->C1 |= I2C_C1_TX_MASK;
	I2C->C1 |= I2C_C1_MST_MASK; // Assert start
	I2C->D = i2c_addr; // Write bus address
	
	while((I2C->S & I2C_S_IICIF_MASK) == 0); // Wait for 
	I2C->S |= I2C_S_IICIF_MASK; // Clear

	I2C->D = addr; // Write register address

	while((I2C->S & I2C_S_IICIF_MASK) == 0); // Wait for 
	I2C->S |= I2C_S_IICIF_MASK; // Clear

	I2C->D = val; // Write value

	while((I2C->S & I2C_S_IICIF_MASK) == 0); // Wait for 
	I2C->S |= I2C_S_IICIF_MASK; // Clear

	I2C->C1 &= ~I2C_C1_MST_MASK;
	I2C->C1 &= ~I2C_C1_TX_MASK;

	while(I2C->S & I2C_S_BUSY_MASK); // Wait for STOP
}

void TPA6130A2::write_multiple(uint8_t start_addr, uint8_t const * data, uint8_t n){
	I2C->C1 |= I2C_C1_TX_MASK;
	I2C->C1 |= I2C_C1_MST_MASK; // Assert start
	I2C->D = i2c_addr; // Write bus address

	while((I2C->S & I2C_S_IICIF_MASK) == 0); // Wait for
	I2C->S |= I2C_S_IICIF_MASK; // Clear

	I2C->D = start_addr; // Write register address

	while((I2C->S & I2C_S_IICIF_MASK) == 0); // Wait for
	I2C->S |= I2C_S_IICIF_MASK; // Clear

	while(n--){
		I2C->D = *data++; // Write value

		while((I2C->S & I2C_S_IICIF_MASK) == 0); // Wait for
		I2C->S |= I2C_S_IICIF_MASK; // Clear
	}

	I2C->C1 &= ~I2C_C1_MST_MASK;
	I2C->C1 &= ~I2C_C1_TX_MASK;

	while(I2C->S & I2C_S_BUSY_MASK); // Wait for STOP
}

uint8_t TPA6130A2::read_reg(uint8_t addr){
	I2C->C1 |= I2C_C1_MST_MASK; // Assert start
	I2C->D = i2c_addr | 1;

	while((I2C->S & I2C_S_IICIF_MASK) == 0); // Wait for
	I2C->S |= I2C_S_IICIF_MASK; // Clear

	return 0;
}


