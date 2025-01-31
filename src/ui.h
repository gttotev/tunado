#ifndef UI_H
#define UI_H

#include "sample.h"
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

	float complex fft_a[FFT_SIZE];

	int a4;
};

extern struct TunerHSM tuner_ao;

void hsm_init();

void bsp_init();
void bsp_timerArm();
void bsp_timerDisarm();

void lcd_init();
void draw_bg();

void draw_mainmenu_init(int pos);
void draw_mainmenu_erase(int pos);
int draw_mainmenu(int pos, int delta);

void draw_octave_init();
void draw_octave_erase();

void draw_ffthist_init();
void draw_ffthist_erase();
void draw_ffthist(float complex *a);

void draw_tuner_init();
void draw_tuner_erase();
void draw_tuner(float complex *a, int a4);

void draw_a4tuner_init();
void draw_a4tuner_erase();
void draw_a4tuner(int a4);

#endif
