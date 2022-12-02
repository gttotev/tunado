#include "ui.h"

struct TunerHSM tuner_ao;

void hsm_init() {
    QActive_ctor(&tuner_ao.super, NULL);
}
