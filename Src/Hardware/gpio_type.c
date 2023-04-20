#include "gpio_type.h"

GPIO_PinState gpio_read(gpio_t *gpio) {
    return HAL_GPIO_ReadPin(gpio->base, gpio->pin);
}

void gpio_write(gpio_t *gpio, GPIO_PinState state) {
    HAL_GPIO_WritePin(gpio->base, gpio->pin, state);
}
