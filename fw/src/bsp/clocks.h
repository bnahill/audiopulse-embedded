

#ifndef __APULSE_SYSTEM_H_
#define __APULSE_SYSTEM_H_

#include <derivative.h>

namespace K20 {

class Clock {
public:
	/*!
	 * @brief Configure clocks from RESET state
	 * 
	 * Doing this after initialization could be really dumb
	 */
	static void setupClocks(){
		SIM_CLKDIV1 = SIM_CLKDIV1_OUTDIV1(clk_core_div - 1) |
		              SIM_CLKDIV1_OUTDIV2(clk_bus_div - 1) |
		              SIM_CLKDIV1_OUTDIV3(clk_flexbus_div - 1) |
		              SIM_CLKDIV1_OUTDIV4(clk_flash_div - 1);
		switchFEItoFBE();
		switchFBEtoPBE();
		switchPBEtoPEE();
	}
	
	static void switchFEItoFBE(){
		// Enable the external oscillator
		OSC_CR = OSC_CR_ERCLKEN_MASK;
		
		MCG_C6 = 0;// PLLS = 0(FLL);
		MCG_C2 = MCG_C2_RANGE0(1) | MCG_C2_EREFS0_MASK | MCG_C2_IRCS_MASK;// LP = 0;

		// Wait for oscillator startup
		while (!(MCG_S & MCG_S_OSCINIT0_MASK));
		
		// Switch to oscillator, bypassing FLL
		MCG_C1 = MCG_C1_CLKS(2) | MCG_C1_FRDIV(3);
		
		// Wait for switch
		while(((MCG_S & MCG_S_CLKST_MASK) >> 2) != 2);
		
	}
	
	static void switchFBEtoPBE(){
		// Configure PLL divider
		MCG_C5 = MCG_C5_PRDIV0(clk_pll_div - 1);
		// Configure PLL multiplier
		MCG_C6 = MCG_C6_VDIV0(clk_pll_mul - 24);
		
		// Enable that PLL
		MCG_C5 = MCG_C5_PRDIV0(clk_pll_div - 1) | MCG_C5_PLLCLKEN0_MASK;
		
		// Wait for lock
		while(!(MCG_S & MCG_S_LOCK0_MASK));
	}
	
	static void switchPBEtoPEE(){
		// Switch PLLS to PLL
		MCG_C6 = MCG_C6_VDIV0(clk_pll_mul - 24) | MCG_C6_PLLS_MASK;
		
		// Switch main clock to PLL
		MCG_C1 = MCG_C1_CLKS(0);
		while(((MCG_S & MCG_S_CLKST_MASK) >> 2) != 3);
	}
	
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

#endif // __APULSE_SYSTEM_H_
