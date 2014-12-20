#ifndef __APULSE_PWM_GPIO_H
#define __APULSE_PWM_GPIO_H

#include <derivative.h>
#include <driver/gpio.h>

class PWMFTM {
public:
	constexpr PWMFTM(FTM_MemMapPtr ftm, GPIOPin::mux_t mux) :
		ftm(ftm), mux(mux){}

	void init() const {
		if(ftm == FTM0_BASE_PTR){
			SIM_SCGC6 |= SIM_SCGC6_FTM0_MASK;
		} else if(ftm == FTM1_BASE_PTR){
			SIM_SCGC6 |= SIM_SCGC6_FTM1_MASK;
		} else if(ftm == FTM2_BASE_PTR){
			SIM_SCGC6 |= SIM_SCGC6_FTM2_MASK;
		}
	}

	void start(uint16_t period = 0xFFFF) const {
		FTM_MODE_REG(ftm) = FTM_MODE_FTMEN_MASK;
		FTM_CNT_REG(ftm) = 0;
		FTM_MOD_REG(ftm) = period;
		// Start!
		FTM_SC_REG(ftm) = FTM_SC_CLKS(1) | FTM_SC_PS(0);
	}

	inline void setCnSC(uint8_t channel, uint8_t value) const {
		FTM_CnSC_REG(ftm, channel) = value;
	}

	inline void setCnV(uint8_t channel, uint16_t value) const {
		FTM_CnV_REG(ftm, channel) = value;
	}

	GPIOPin::mux_t const mux;

protected:
	FTM_MemMapPtr const ftm;
};

class PWMGPIOPin {
public:
	constexpr PWMGPIOPin(PWMFTM const ftm, GPIOPin const gpio, uint8_t const channel)
		: ftm(ftm), channel(channel), gpio(gpio){
	}
	void init() const {
		gpio.set_mux(ftm.mux);
	}
	void setValue(uint16_t value) const {
		ftm.setCnV(channel, value);
		ftm.setCnSC(channel,
					FTM_CnSC_MSB_MASK | FTM_CnSC_MSA_MASK | FTM_CnSC_ELSB_MASK
					);
	}

protected:
	uint8_t const channel;
	PWMFTM const ftm;
	GPIOPin const gpio;
};

#endif // __APULSE_PWM_GPIO_H
