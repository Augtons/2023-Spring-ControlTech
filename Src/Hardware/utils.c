#include "utils.h"

void simple_delay(uint32_t ticks) {
    for(int i = 0; i < ticks; i++) {
        __nop();
    }
}
