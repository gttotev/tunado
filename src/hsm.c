#include "ui.h"

struct TunerHSM tuner_ao;

static QState tuner_initial(struct TunerHSM *me);
static QState tuner_wait(struct TunerHSM *me);

void hsm_init() {
    QActive_ctor(&tuner_ao.super, (QStateHandler)tuner_initial);
}

static QState tuner_initial(struct TunerHSM *me) {
    return Q_TRAN(tuner_wait);
}

static QState tuner_wait(struct TunerHSM *me) {
    return Q_SUPER(QHsm_top);
}
