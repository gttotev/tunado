#include "ui.h"

struct TunerHSM tuner_ao;

static QState tuner_initial(struct TunerHSM *me);
static QState tuner_mainmenu(struct TunerHSM *me);
static QState tuner_ffthist(struct TunerHSM *me);

void hsm_init() {
    QActive_ctor(&tuner_ao.super, (QStateHandler)tuner_initial);
}

static QState tuner_initial(struct TunerHSM *me) {
    draw_bg();
    return Q_TRAN(tuner_mainmenu);
}

static QState tuner_mainmenu(struct TunerHSM *me) {
    switch (Q_SIG(me)) {
    case Q_ENTRY_SIG:
        draw_init_mainmenu(me->menu);
        return Q_HANDLED();
    case Q_EXIT_SIG:
        draw_erase_mainmenu(me->menu);
        return Q_HANDLED();
    case SIG_BTN_DN:
    case SIG_ENC_UP:
        me->menu = draw_mainmenu(me->menu, 1);
        return Q_HANDLED();
    case SIG_BTN_UP:
    case SIG_ENC_DN:
        me->menu = draw_mainmenu(me->menu, -1);
        return Q_HANDLED();
    case SIG_BTN_CT:
    case SIG_ENC_CL:
        switch (me->menu) {
        case 1:
            return Q_TRAN(tuner_ffthist);
        }
        return Q_HANDLED();
    }
    return Q_SUPER(QHsm_top);
}

static QState tuner_ffthist(struct TunerHSM *me) {
    switch (Q_SIG(me)) {
    case SIG_FFT_DONE:
        return Q_HANDLED();
    case SIG_BTN_LF:
    case SIG_ENC_DN:
        return Q_TRAN(tuner_mainmenu);
    }
    return Q_SUPER(QHsm_top);
}
