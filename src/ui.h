#ifndef UI_H
#define UI_H

#include "qpn_port.h"

#define LCD_X

enum TunerSig {
    SIG_ENC_UP = Q_USER_SIG,
    SIG_ENC_DN,
    SIG_ENC_CL,
    SIG_BTN_UP, 
    SIG_BTN_DN,
    SIG_BTN_LF,
    SIG_BTN_RG,
};

struct TunerHSM {
    QActive super;
};

extern struct TunerHSM tuner_ao;

void hsm_init();

void bsp_init();

void lcd_init();

#endif
