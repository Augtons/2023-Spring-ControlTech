#ifndef _74HC138_H_
#define _74HC138_H_

#include "gpio_type.h"

#ifdef __cplusplus
extern "C" {
#endif

struct hc138_t
{
    gpio_t a0;
    gpio_t a1;
    gpio_t a2;
};

typedef struct hc138_t hc138_t;

void hc138_turn(hc138_t *hc138, uint8_t num);

#ifdef __cplusplus
}
#endif


#endif
