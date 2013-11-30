#ifndef __APULSE_CLOCKS_H_
#define __APULSE_CLOCKS_H_

#include <derivative.h>

namespace K20 {

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

};

#endif // __APULSE_CLOCKS_H_
