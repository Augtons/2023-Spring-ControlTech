#ifndef _GPIO_TYPE_H_
#define _GPIO_TYPE_H_

#include "main.h"
#include "stdint.h"
#include "utils.h"

#ifdef __cplusplus
extern "C" {
#endif

struct gpio_t
{
    GPIO_TypeDef   *base;
    uint16_t        pin;
};

typedef struct gpio_t gpio_t;

GPIO_PinState gpio_read(gpio_t *gpio);

void gpio_write(gpio_t *gpio, GPIO_PinState state);


#ifdef __cplusplus
}
#endif

#endif

