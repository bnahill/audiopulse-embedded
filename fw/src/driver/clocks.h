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
#include <driver/gpio.h>

/*!
 @brief A constant configuration for clocks to make it easy to switch between
 configurations
 
 @TODO Add USB division
 */
class ClockConfig {
public:
	constexpr ClockConfig(
		uint32_t const mcgoutclk,
		uint32_t const busclk,
		
		uint32_t const clk_xtal,
		uint32_t const clk_pll_div,
		uint32_t const clk_pll_mul,
		uint32_t const clk_core_div,
		uint32_t const clk_bus_div,
		uint32_t const clk_flexbus_div,
		uint32_t const clk_flash_div) :
		mcgoutclk(mcgoutclk), busclk(busclk),
		clk_xtal(clk_xtal), clk_pll_div(clk_pll_div),
		clk_pll_mul(clk_pll_mul), clk_core_div(clk_core_div),
		clk_bus_div(clk_bus_div), clk_flexbus_div(clk_flexbus_div),
		clk_flash_div(clk_flash_div)
		{}

	uint32_t const mcgoutclk;
	uint32_t const busclk;
	
	uint32_t const clk_xtal;
	uint32_t const clk_pll_div;
	uint32_t const clk_pll_mul;
	uint32_t const clk_core_div;
	uint32_t const clk_bus_div;
	uint32_t const clk_flexbus_div;
	uint32_t const clk_flash_div;
};

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

	typedef enum {
		CLKOUT_OFF       = 0,
		CLKOUT_FLASHCLK  = 2,
		CLKOUT_LPO       = 3,
		CLKOUT_MCGIRCLK  = 4,
		CLKOUT_RTC       = 5,
		CLKOUT_OSCERCLK0 = 6
	} clkout_src_t;

	static void set_clkout(clkout_src_t src){
		if(src == CLKOUT_OFF){
			clkout.configure(GPIOPin::MUX_ANALOG);
		} else {
			SIM_SOPT2 = (SIM_SOPT2 & ~SIM_SOPT2_CLKOUTSEL_MASK) |
			            (SIM_SOPT2_CLKOUTSEL(src));
			clkout.configure(GPIOPin::MUX_ALT5);
		}
	}


	static constexpr ClockConfig clk48 = {
		48000000, // Run OUTCLK at 48MHz
		48000000, // Bus clock is same
		8000000,  // xtal is 8MHz
		4,        // PLL input is 8MHz / 4
		24,       // Multiply that by 24 = 48MHz
		1,        // Core is MCGOUTCLK / 1
		1,        // Bus is MCGOUTCLK / 1
		1,        // Flexbus is MCGOUTCLK / 1
		2         // Flash is MCGOUTCLK / 2
	};
	
	static constexpr ClockConfig clk72 = {
		72000000, // Run OUTCLK at 72MHz
		36000000, // Bus clock is half that
		8000000,  // xtal is 8MHz
		4,        // PLL input is 8MHz / 4
		36,       // Multiply that by 36 = 72MHz
		1,        // Core is MCGOUTCLK / 1
		1,        // Bus is MCGOUTCLK / 2
		1,        // Flexbus is MCGOUTCLK / 2 = 36MHz
		3         // Flash is MCGOUTCLK / 3 = 24MHz
	};
	
	//! Select the configuration to use
	static constexpr ClockConfig config = clk48;
	
private:
	static constexpr GPIOPin clkout = {PTC_BASE_PTR, 3};
};

#endif // __APULSE_CLOCKS_H_
