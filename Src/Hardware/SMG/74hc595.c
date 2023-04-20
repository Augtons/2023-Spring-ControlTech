#include "74hc595.h"
#include "utils.h"

static void shift(hc595_t *hc595) {
    gpio_t *sclk = &hc595->sclk;
    gpio_write(sclk, (GPIO_PinState)0);
    simple_delay(100);
    gpio_write(sclk, (GPIO_PinState)1);
}

static void show(hc595_t *hc595) {
    gpio_t *lclk = &hc595->lclk;
    gpio_write(lclk, (GPIO_PinState)0);
    simple_delay(100);
    gpio_write(lclk, (GPIO_PinState)1);
}

// low -> is Q0
void hc595_show(hc595_t *hc595, uint8_t byte) {
    gpio_t *ds = &hc595->ds;
    uint8_t temp = byte;
    for(int i = 0; i < 8; i++) {
        gpio_write(ds, (GPIO_PinState)(temp & 0x1));
        shift(hc595);
        temp >>= 1;
    }
    show(hc595);
}
