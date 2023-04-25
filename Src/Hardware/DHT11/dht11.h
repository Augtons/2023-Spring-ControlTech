#ifndef _DHT11_H_
#define _DHT11_H_

#include "gpio_type.h"

#ifdef __cplusplus
extern "C" {
#endif

struct dht11_t
{
    gpio_t data_io;
};

typedef struct dht11_t dht11_t;

void dht11_read(dht11_t* dht11, uint8_t* humidity, uint8_t* temperature);

#ifdef __cplusplus
};
#endif


#endif
