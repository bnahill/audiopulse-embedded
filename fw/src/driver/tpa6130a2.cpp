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

void TPA6130A2::write_reg (uint8_t addr, uint8_t val ) {
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
}
