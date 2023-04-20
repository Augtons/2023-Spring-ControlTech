#include "smg.h"

const uint8_t smg_duan[] = {
    0xFC,        // 0
    0x60,        // 1
    0xDA,        // 2
    0xF2,        // 3
    0x66,        // 4
    0xB6,        // 5
    0xBE,        // 6
    0xE0,        // 7
    0xFE,        // 8
    0xF6,        // 9

    0xEE,        // A
    0x3E,        // B
    0x9C,        // C
    0x7A,        // D
    0x9E,        // E
    0x8E,        // F
  
    0x7C,        //V
};

void smg_init(hc138_t *hc138, hc595_t *hc595, smg_t *ret_smg) {
  smg_t smg = {
    .hc138 = *hc138,
    .hc595 = *hc595,
    .buf = {0},
    .flash_pointer = 0
  };
  *ret_smg = smg;
}

void smg_from_string(smg_t *smg, const char *str) {
    char *pointer = (char*)str;
    int i = 0;
    for (i = 0; i < 8; pointer++) {
        switch (*pointer) {
            case '\0':
                return;
            case '_':
                smg->buf[i] = 0x00;
                break;
            case '.':
                if (i > 0) {
                    smg->buf[i-1] |= 0x1;
                }
                break;
            case '0' ... '9':
                smg->buf[i] = smg_duan[*pointer - '0'];
                break;
            case 'A' ... 'F':
                smg->buf[i] = smg_duan[*pointer - 'A' + 10];
                break;
            case 'a' ... 'f':
                smg->buf[i] = smg_duan[*pointer - 'a' + 10];
                break;
            case 'v':
            case 'V':
                smg->buf[i] = 0x7C;
                break;
            default:
                break;
        }
        if (*pointer != '.') {
            i++;
        }
    }
    if (i == 8 && *pointer == '.') {
        smg->buf[i-1] |= 0x1;
    }
}

void smg_set(smg_t *smg, uint8_t index, char value) {
    if (index >= 0 && index < 8) {
        switch (value) {
            case '0' ... '9':
                smg->buf[index] = smg_duan[value - '0'];
                break;
            case 'a' ... 'f':
                smg->buf[index] = smg_duan[value - 'a' + 10];
                break;
            case 'A' ... 'F':
                smg->buf[index] = smg_duan[value - 'A' + 10];
                break;
            default:
                break;
        }
        
    }
}

void smg_dot(smg_t *smg, uint8_t index, uint8_t add_or_remove) {
    if (index >= 0 && index < 8) {
        if (add_or_remove) {
            smg->buf[index] |= 0x1;
        } else {
            smg->buf[index] &= ~0x1;
        }
    }
}

void smg_flush(smg_t *smg) {
    if (smg->flash_pointer == 8) {
        smg->flash_pointer = 0;
    }
    uint8_t i = smg->flash_pointer;
    hc595_show(&smg->hc595, 0x00);
    hc138_turn(&smg->hc138, i);
    hc595_show(&smg->hc595, smg->buf[i]);
    smg->flash_pointer ++;
}
