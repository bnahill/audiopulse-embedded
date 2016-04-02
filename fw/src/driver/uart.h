#ifndef __APULSE_UART_H
#define __APULSE_UART_H

#include <derivative.h>
#include <driver/gpio.h>
#include <driver/clocks.h>

class UART {
public:
    UART(UART_MemMapPtr dev, GPIOPin const &rx, GPIOPin const &tx,
         uint32_t dma_ch_tx) :
        dev(dev), rx(rx), tx(tx), is_init(false), dma_ch_tx(dma_ch_tx),
        queue_count(0), queue_rd_head(0), queue_wr_head(0)
    {

    }

    bool init(uint32_t br_target);

    typedef void (*cb_t)(void *);

    bool send_data(uint8_t const * src, uint16_t len, cb_t cb=nullptr, void * cb_arg=nullptr){
        return is_init && enqueue(src, len, cb, cb_arg);
    }

    void handle_complete_tx_isr(){
        dequeue();
        DMA_CINT = DMA_CINT_CINT(dma_ch_tx);
        NVIC_ClearPendingIRQ((IRQn_Type)dma_ch_tx);
    }

private:
    struct transfer {
        uint8_t const * src;
        uint16_t        len;
        cb_t            cb;
        void *          cb_arg;
    };


    static constexpr uint32_t queue_len = 16;
    uint32_t volatile queue_count;
    uint32_t volatile queue_rd_head;
    uint32_t volatile queue_wr_head;
    struct transfer queue_data[queue_len];

    bool enqueue(uint8_t const * src, uint16_t len, cb_t cb, void * cb_arg);

    bool do_tx(uint8_t const * src, uint16_t len);

    void dequeue();

    bool tx_busy;
    bool is_init;
    GPIOPin const rx, tx;
    uint32_t const dma_ch_tx;

    UART_MemMapPtr const dev;
};

#endif // __APULSE_UART_H
