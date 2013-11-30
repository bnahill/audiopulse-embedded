#include <bsp/platform.h>

namespace K20 {

void Clock::setupClocks(){
	SIM_CLKDIV1 = SIM_CLKDIV1_OUTDIV1(clk_core_div - 1) |
				  SIM_CLKDIV1_OUTDIV2(clk_bus_div - 1) |
				  SIM_CLKDIV1_OUTDIV3(clk_flexbus_div - 1) |
				  SIM_CLKDIV1_OUTDIV4(clk_flash_div - 1);
	switchFEItoFBE();
	switchFBEtoPBE();
	switchPBEtoPEE();
}

void Clock::switchFEItoFBE(){
	// Enable the external oscillator
	OSC_CR = OSC_CR_ERCLKEN_MASK;

	MCG_C6 = 0;// PLLS = 0(FLL);
	MCG_C2 = MCG_C2_RANGE0(1) | MCG_C2_EREFS0_MASK | MCG_C2_IRCS_MASK;
	
	// Wait for oscillator startup
	while (!(MCG_S & MCG_S_OSCINIT0_MASK));
	
	Platform::led.set();

	// Switch to oscillator, bypassing FLL
	MCG_C1 = MCG_C1_CLKS(2) | MCG_C1_FRDIV(3);

	// Wait for switch
	while(((MCG_S & MCG_S_CLKST_MASK) >> 2) != 2);

}

void Clock::switchFBEtoPBE(){
	// Configure PLL divider
	MCG_C5 = MCG_C5_PRDIV0(clk_pll_div - 1);
	// Configure PLL multiplier
	MCG_C6 = MCG_C6_VDIV0(clk_pll_mul - 24);

	// Enable that PLL
	MCG_C5 = MCG_C5_PRDIV0(clk_pll_div - 1) | MCG_C5_PLLCLKEN0_MASK;

	// Wait for lock
	while(!(MCG_S & MCG_S_LOCK0_MASK));
}

void Clock::switchPBEtoPEE(){
	// Switch PLLS to PLL
	MCG_C6 = MCG_C6_VDIV0(clk_pll_mul - 24) | MCG_C6_PLLS_MASK;

	// Switch main clock to PLL
	MCG_C1 = MCG_C1_CLKS(0);
	while(((MCG_S & MCG_S_CLKST_MASK) >> 2) != 3);
}

};