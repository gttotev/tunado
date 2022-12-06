#ifndef UI_H
#define UI_H

#include "qpn_port.h"

enum TunerSig {
    SIG_ENC_UP = Q_USER_SIG,
    SIG_ENC_DN,
    SIG_ENC_CL,
    SIG_BTN_UP, 
    SIG_BTN_DN,
    SIG_BTN_LF,
    SIG_BTN_RG,
    SIG_BTN_CT,
    SIG_FFT_DONE,
};

struct TunerHSM {
    QActive super;
    int menu;
};

extern struct TunerHSM tuner_ao;

void hsm_init();

void bsp_init();
void bsp_timerArm();
void bsp_timerDisarm();

void lcd_init();
void draw_bg();
void draw_init_mainmenu(int pos);
void draw_erase_mainmenu(int pos);
int draw_mainmenu(int pos, int delta);

#endif
