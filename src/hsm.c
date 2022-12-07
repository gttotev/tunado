#include "ui.h"

struct TunerHSM tuner_ao;

static QState tuner_initial(struct TunerHSM *me);
static QState tuner_mainmenu(struct TunerHSM *me);
static QState tuner_fft(struct TunerHSM *me);
static QState tuner_ffthist(struct TunerHSM *me);
static QState tuner_a4tuner(struct TunerHSM *me);
static QState tuner_tuner(struct TunerHSM *me);
static QState tuner_a4(struct TunerHSM *me);

void hsm_init() {
    QActive_ctor(&tuner_ao.super, (QStateHandler)tuner_initial);
    tuner_ao.a4 = 440;
}

static QState tuner_initial(struct TunerHSM *me) {
    draw_bg();
    return Q_TRAN(tuner_mainmenu);
}

static QState tuner_mainmenu(struct TunerHSM *me) {
    switch (Q_SIG(me)) {
    case Q_ENTRY_SIG:
        draw_mainmenu_init(me->menu);
        return Q_HANDLED();
    case Q_EXIT_SIG:
        draw_mainmenu_erase(me->menu);
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
        case 0:
            return Q_TRAN(tuner_tuner);
        case 1:
            return Q_TRAN(tuner_ffthist);
        }
        return Q_HANDLED();
    }
    return Q_SUPER(QHsm_top);
}

static QState tuner_fft(struct TunerHSM *me) {
    switch (Q_SIG(me)) {
    case Q_ENTRY_SIG:
        me->fft_on = 1;
        return Q_HANDLED();
    case Q_EXIT_SIG:
        me->fft_on = 0;
        return Q_HANDLED();
    case SIG_BTN_LF:
        return Q_TRAN(tuner_mainmenu);
    }
    return Q_SUPER(QHsm_top);
}

static QState tuner_ffthist(struct TunerHSM *me) {
    switch (Q_SIG(me)) {
    case Q_ENTRY_SIG:
        draw_ffthist_init();
        return Q_HANDLED();
    case Q_EXIT_SIG:
        draw_ffthist_erase();
        return Q_HANDLED();
    case SIG_FFT_DONE:
        draw_ffthist(me->fft_a);
        return Q_HANDLED();
    }
    return Q_SUPER(tuner_fft);
}

static QState tuner_a4tuner(struct TunerHSM *me) {
    switch (Q_SIG(me)) {
    case Q_ENTRY_SIG:
        draw_a4tuner_init();
        return Q_HANDLED();
    case Q_EXIT_SIG:
        draw_a4tuner_erase();
        return Q_HANDLED();
    case SIG_ENC_DN:
        if (me->a4 > 420) --me->a4;
        draw_a4tuner(me->a4);
        return Q_TRAN(tuner_a4);
    case SIG_ENC_UP:
        if (me->a4 < 460) ++me->a4;
        draw_a4tuner(me->a4);
        return Q_TRAN(tuner_a4);
    }
    return Q_SUPER(tuner_fft);
}

static QState tuner_tuner(struct TunerHSM *me) {
    switch (Q_SIG(me)) {
    case Q_ENTRY_SIG:
        draw_tuner_init();
        return Q_HANDLED();
    case Q_EXIT_SIG:
        draw_tuner_erase();
        return Q_HANDLED();
    case SIG_FFT_DONE:
        draw_tuner(me->fft_a, me->a4);
        return Q_HANDLED();
    }
    return Q_SUPER(tuner_a4tuner);
}

static QState tuner_a4(struct TunerHSM *me) {
    switch (Q_SIG(me)) {
    case Q_ENTRY_SIG:
        bsp_timerArm();
        return Q_HANDLED();
    case Q_TIMEOUT_SIG:
        return Q_TRAN(tuner_tuner);
    }
    return Q_SUPER(tuner_a4tuner);
}
