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

#ifndef __APULSE_CLOCKS_H_
#define __APULSE_CLOCKS_H_

#include <derivative.h>


class Clock {
public:
	/*!
	 * @brief Configure clocks from RESET state
	 *
	 * Doing this after initialization could be really dumb
	 */
	static void setupClocks();

	static void switchFEItoFBE();

	static void switchFBEtoPBE();

	static void switchPBEtoPEE();

private:
	static constexpr uint32_t clk_xtal = 8000000;
	static constexpr uint32_t clk_pll_div = 4; // Make 2MHz
	static constexpr uint32_t clk_pll_mul = 24; // Make 48MHz
	static constexpr uint32_t clk_core_div = 1;    // Use 48MHz for everything
	static constexpr uint32_t clk_bus_div = 1;
	static constexpr uint32_t clk_flexbus_div = 1;
	static constexpr uint32_t clk_flash_div = 2; // Max 25MHz
};

#endif // __APULSE_CLOCKS_H_
