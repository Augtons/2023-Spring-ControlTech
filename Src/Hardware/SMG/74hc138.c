#include "74hc138.h"

void hc138_turn(hc138_t *hc138, uint8_t num) {
    gpio_write(&hc138->a2, (GPIO_PinState)((num >> 2) & 0x1));
    gpio_write(&hc138->a1, (GPIO_PinState)((num >> 1) & 0x1));
    gpio_write(&hc138->a0, (GPIO_PinState)(num & 0x1));
}
