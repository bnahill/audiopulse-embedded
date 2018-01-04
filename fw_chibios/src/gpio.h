#ifndef __APULSE_GPIO_H
#define __APULSE_GPIO_H

#include <hal.h>

class GPIOPin {
public:
    constexpr GPIOPin(GPIO_TypeDef &port, uint8_t pin) :
        port(port), pin(pin)
    {}

    void clear() const{
        port.BSRR = 0x10000 << pin;
    }

    void set() const{
        port.BSRR = 1 << pin;
    }

    void assign(bool v) const {
        if(v){
            set();
        } else {
            clear();
        }
    }

    uint8_t get_pin() const {return pin;}
    GPIO_TypeDef &get_port() const {return port;}

    ioportid_t get_ch_port() const {return reinterpret_cast<ioportid_t>(&port);}
    uint32_t get_ch_line() const {return reinterpret_cast<uint32_t>(&port) |
                                         static_cast<uint32_t>(pin);}
    uint8_t get_ch_pad() const {return pin;}

protected:
    uint8_t const pin;
    GPIO_TypeDef &port;
};

#endif // __APULSE_GPIO_H
