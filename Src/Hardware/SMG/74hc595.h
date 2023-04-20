#ifndef _74HC595_H_
#define _74HC595_H_

#include "gpio_type.h"

#ifdef __cplusplus
extern "C" {
#endif

struct hc595_t
{
    gpio_t sclk;//PB5
    gpio_t lclk;//PB4
    gpio_t ds;//PB3
};

typedef struct hc595_t hc595_t;

void hc595_show(hc595_t *hc595, uint8_t byte);

#ifdef __cplusplus
}
#endif


#endif
