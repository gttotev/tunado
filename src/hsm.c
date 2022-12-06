#include "ui.h"
#include "lcd.h"

struct TunerHSM tuner_ao;

static QState tuner_initial(struct TunerHSM *me);
static QState tuner_mainmenu(struct TunerHSM *me);

void hsm_init() {
    QActive_ctor(&tuner_ao.super, (QStateHandler)tuner_initial);
}

static QState tuner_initial(struct TunerHSM *me) {
    lcd_setColor(0xCD, 0xEA, 0xC0);
    lcd_rect(0, 0, LCD_X_SIZE, LCD_Y_SIZE);
    return Q_TRAN(tuner_mainmenu);
}

static QState tuner_mainmenu(struct TunerHSM *me) {
    switch (Q_SIG(me)) {
    case Q_ENTRY_SIG:
        lcd_setColor(0xFF, 0xFF, 0xFF);
        lcd_setFont(BigFont);
        lcd_setColorBg(0, 0x4E, 0x89);
        lcd_print("Tuner", 70, 100);
        lcd_setColorBg(0x61, 0x29, 0x40);
        lcd_print("FFT histogram", 40, 150);
        lcd_print("Octave select", 40, 200);
        me->menu = 0;
        break;
    // case SIG_BTN_DN:
    // case SIG_ENC_DN:
    //     lcd_setColor(0xFF, 0xFF, 0xFF);
    //     lcd_setFont(BigFont);
    //     lcd_setColorBg(0, 0x4E, 0x89);
    //     lcd_print("Tuner", 70, 100);
    //     lcd_setColorBg(0x61, 0x29, 0x40);
    //     lcd_print("FFT histogram", 60, 150);
    //     lcd_print("Octave select", 60, 200);
    }
    return Q_SUPER(QHsm_top);
}
