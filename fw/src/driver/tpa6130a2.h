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

/*!
 @brief A static class for controlling the TI TPA6130A2 headphone amplifier
 */
class TPA6130A2 {
public:
	/*!
	 @brief Initialize the local hardware. Actual device may be off still.
	 @post Ready to call init()
	 */
	static void init_hw();

	/*!
	 @brief Initialize the device
	 @pre init_hw() has been called
	 @pre Power has been enabled for the analog circuitry
	 
	 @note This must be called after any loss of power on analog circuitry
	 */
	static void init();
	
	/*!
	 @brief Enable the device, taking it out of a low-power state

	 @note This actually can take some time, so please don't run from ISR
	 */
	static void enable();
	
	/*!
	 @brief Disable the device, putting it in a low-power state

	 @note This does nothing right now
	 */
	static void disable();

	static inline bool is_ready(){
		return enabled;
	}
	
protected:
	static bool enabled;

	static uint8_t volume;

	/*!
	 @brief Write a register over I2C
	 */
	static void write_reg(uint8_t addr, uint8_t val);

	/*!
	 @brief Write multiple registers over I2C
	 */
	static void write_multiple(uint8_t start_addr,
	                           uint8_t const * data, uint8_t n);

	/*!
	 @brief Read a single register over I2C

	 @note This is not complete and may never be
	 */
	static uint8_t read_reg(uint8_t addr);
	
	//! @name Pin configuration
	//! @{
	static constexpr GPIOPin nSD = {PTB_BASE_PTR, 2};
	static constexpr GPIOPin SDA = {PTB_BASE_PTR, 1};
	static constexpr GPIOPin::mux_t SDA_mux = GPIOPin::MUX_ALT2;
	static constexpr GPIOPin SCL = {PTB_BASE_PTR, 0};
	static constexpr GPIOPin::mux_t SCL_mux = GPIOPin::MUX_ALT2;
	//! @}

	//! @name Hardware configuration
	//! @{
	static constexpr I2C_MemMapPtr I2C = I2C0_BASE_PTR;
	static constexpr uint8_t i2c_addr = 0b1100000 << 1;
	//! @}
};

#endif // __APULSE_TPA6130_H_
