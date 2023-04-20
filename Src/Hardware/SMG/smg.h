#ifndef _SMG_H_
#define _SMG_H_

#include "74hc138.h"
#include "74hc595.h"

#ifdef __cplusplus
extern "C" {
#endif

struct smg_t
{
    hc138_t hc138;
    hc595_t hc595;

    uint8_t buf[8];
    uint8_t flash_pointer;
};

typedef struct smg_t smg_t;

extern const uint8_t smg_duan[];

void smg_init(hc138_t *hc138, hc595_t *hc595, smg_t *ret_smg);

void smg_from_string(smg_t *smg, const char *str);

void smg_set(smg_t *smg, uint8_t index, char value);

void smg_dot(smg_t *smg, uint8_t index, uint8_t add_or_remove);

void smg_test(smg_t *smg);

void smg_flush(smg_t *smg);


#ifdef __cplusplus
}
#endif


#endif
