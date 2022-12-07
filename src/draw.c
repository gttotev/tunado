#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <fastmath.h>

#include "ui.h"
#include "lcd.h"

#define BIGFONT_PX(x) ((x)*16)
#define SMLFONT_PX_X(x) ((x)*8)
#define SMLFONT_PX_Y(y) ((y)*12)
#define SVNFONT_PX_X(x) ((x)*32)
#define SVNFONT_PX_Y(y) ((y)*50)

#define COLOR_BG 0xCD, 0xEA, 0xC0
#define COLOR_TXT COLOR_BG
#define COLOR_ACTIVE 0xE0, 0xAF, 0xA0
#define COLOR_INACTIVE 0x61, 0x29, 0x40

#define MIN(x, y) ((x) < (y) ? (x) : (y))
#define MAX(x, y) ((x) > (y) ? (x) : (y))
#define CLAMP(x, a, b) MAX(a, MIN(b, x))

void draw_bg() {
    lcd_setColor(COLOR_BG);
    lcd_rect(0, 0, LCD_X_SIZE, LCD_Y_SIZE);
}

#define MAINMENU_SIZE 3
#define MAINMENU_X 15
#define MAINMENU_Y(i) (100 + 50*(i))
#define MAINMENU_CUR(pos) \
    lcd_rect(MAINMENU_X-14, MAINMENU_Y(pos)+2, 12, 12)
static char mainmenu_txt[MAINMENU_SIZE][14] = {
    "Tuner",
    "FFT Histogram",
    "Octave Select",
};

void draw_mainmenu_init(int pos) {
    int i;
    lcd_setFont(BigFont);
    lcd_setColor(COLOR_TXT);
    lcd_setColorBg(COLOR_INACTIVE);
    for (i = 0; i < MAINMENU_SIZE; ++i) {
        lcd_print(mainmenu_txt[i], MAINMENU_X, MAINMENU_Y(i));
    }
    lcd_setColor(COLOR_ACTIVE);
    MAINMENU_CUR(pos);
}

int draw_mainmenu(int pos, int delta) {
    int res = (MAINMENU_SIZE + pos + delta) % MAINMENU_SIZE;
    lcd_setColor(COLOR_BG);
    MAINMENU_CUR(pos);
    lcd_setColor(COLOR_ACTIVE);
    MAINMENU_CUR(res);
    return res;
}

void draw_mainmenu_erase(int pos) {
    int i;
    lcd_setColor(COLOR_BG);
    MAINMENU_CUR(pos);
    for (i = 0; i < MAINMENU_SIZE; ++i) lcd_rect(
        MAINMENU_X, MAINMENU_Y(i),
        BIGFONT_PX(strlen(mainmenu_txt[i])), BIGFONT_PX(1)
    );
}

#define FFTHIST_SIZE 255
#define FFTHIST_TXT(i) (FFTHIST_SIZE + SMLFONT_PX_Y(i))
static float fft_mag[FFT_SIZE];
static int fft_lastbl, fft_hists[FFTHIST_SIZE];
static char *fft_txts[] = {
    "Max: bin xxxx(xxxx Hz)",
    "Range of bins shown:",
    "xxxx(xxxx Hz) - xxxx(xxxx Hz)",
};

void draw_ffthist_init() {
    int i;
    char buf[32];
    lcd_setFont(SmallFont);
    lcd_setColor(COLOR_BG);
    lcd_setColorBg(COLOR_INACTIVE);
    snprintf(buf, 32, "FFT: %d | bins: %.3f Hz", FFT_SIZE, FFT_RES);
    lcd_print(buf, 0, FFTHIST_TXT(0));
    for (i = 0; i < 3; ++i) {
        lcd_print(fft_txts[i], 0, FFTHIST_TXT(i+1));
    }
}

void draw_ffthist(float complex *a) {
    char buf[5];
    int i, x, bl, bh, m = fft_max(a, FFT_SIZE, fft_mag);
    float scale = LCD_X_SIZE / sqrtf(fft_mag[m]);

    bl = m - FFTHIST_SIZE/2;
    if (bl < 1) bl = 1;
    bh = bl + FFTHIST_SIZE;
    if (bh > FFT_SIZE/2) {
        bh = FFT_SIZE/2;
        bl = bh - FFTHIST_SIZE;
    }

    for (i = 0; i < FFTHIST_SIZE; ++i) {
        x = sqrtf(fft_mag[bl+i]) * scale;
        if (x == fft_hists[i]) continue;
        if (x > fft_hists[i]) {
            lcd_setColor(COLOR_ACTIVE);
            lcd_rect(fft_hists[i], i, x-fft_hists[i], 1);
        } else {
            lcd_setColor(COLOR_BG);
            lcd_rect(x, i, fft_hists[i]-x, 1);
        }
        fft_hists[i] = x;
    }

    lcd_setColor(COLOR_BG);
    lcd_setColorBg(COLOR_INACTIVE);
    snprintf(buf, 5, "%4d", m);
    lcd_print(buf, SMLFONT_PX_X(9), FFTHIST_TXT(1));
    snprintf(buf, 5, "%4d", (int)(m*FFT_RES));
    lcd_print(buf, SMLFONT_PX_X(14), FFTHIST_TXT(1));

    if (bl != fft_lastbl) {
        fft_lastbl = bl;
        snprintf(buf, 5, "%4d", bl);
        lcd_print(buf, SMLFONT_PX_X(0), FFTHIST_TXT(3));
        snprintf(buf, 5, "%4d", (int)(bl*FFT_RES));
        lcd_print(buf, SMLFONT_PX_X(5), FFTHIST_TXT(3));
        snprintf(buf, 5, "%4d", bh);
        lcd_print(buf, SMLFONT_PX_X(16), FFTHIST_TXT(3));
        snprintf(buf, 5, "%4d", (int)(bh*FFT_RES));
        lcd_print(buf, SMLFONT_PX_X(21), FFTHIST_TXT(3));
    }
}

void draw_ffthist_erase() {
    int i;
    lcd_setColor(COLOR_BG);
    for (i = 0; i < FFTHIST_SIZE; ++i) {
        lcd_rect(0, i, fft_hists[i], 1);
        fft_hists[i] = 0;
    }
    lcd_rect(0, FFTHIST_TXT(0), LCD_X_SIZE, SMLFONT_PX_Y(4));
    fft_lastbl = 0;
}

#define TUNER_HZ_X ((LCD_X_SIZE - SVNFONT_PX_X(4))/2)
#define TUNER_HZ_Y 256
#define TUNER_BAR_X (LCD_X_SIZE/2)
#define TUNER_BAR_Y (LCD_Y_SIZE/2)
#define TUNER_BAR_H 16
#define TUNER_BAR_PX(c) ((int)((LCD_X_SIZE/100.0f)*c))
static int tuner_bar;

void draw_tuner_init() {
    lcd_setFont(BigFont);
    lcd_setColor(COLOR_BG);
    lcd_setColorBg(COLOR_INACTIVE);
    lcd_print(
        "Hz", TUNER_HZ_X + SVNFONT_PX_X(3)/2,
        TUNER_HZ_Y + SVNFONT_PX_Y(1)
    );
}

void draw_tuner_erase() {
    lcd_setColor(COLOR_BG);
    lcd_rect(
        TUNER_HZ_X, TUNER_HZ_Y,
        SVNFONT_PX_X(4), SVNFONT_PX_Y(1)+BIGFONT_PX(1)
    );
    lcd_rect(
        MIN(TUNER_BAR_X, tuner_bar + TUNER_BAR_X), TUNER_BAR_Y,
        abs(tuner_bar), TUNER_BAR_H
    );
    tuner_bar = 0;
}

static void adjust_bar(int bar) {
    int a = MIN(TUNER_BAR_X, bar + TUNER_BAR_X);
    int b = MIN(TUNER_BAR_X, tuner_bar + TUNER_BAR_X);
    if (a > b) {
        lcd_setColor(COLOR_BG);
        lcd_rect(b, TUNER_BAR_Y, a-b, TUNER_BAR_H);
    } else if (a < b) {
        lcd_setColor(COLOR_ACTIVE);
        lcd_rect(a, TUNER_BAR_Y, b-a, TUNER_BAR_H);
    }
    a = MAX(TUNER_BAR_X, bar + TUNER_BAR_X);
    b = MAX(TUNER_BAR_X, tuner_bar + TUNER_BAR_X);
    if (a > b) {
        lcd_setColor(COLOR_ACTIVE);
        lcd_rect(b, TUNER_BAR_Y, a-b, TUNER_BAR_H);
    } else if (a < b) {
        lcd_setColor(COLOR_BG);
        lcd_rect(a, TUNER_BAR_Y, b-a, TUNER_BAR_H);
    }
}

void draw_tuner(float complex *a) {
    char buf[5];
    int bar, freq = fft_max(a, FFT_SIZE, fft_mag) * FFT_RES;

    bar = TUNER_BAR_PX(CLAMP(freq - 440, -50, 50));
    if (bar != tuner_bar) {
        adjust_bar(bar);
        tuner_bar = bar;
    }

    lcd_setColor(COLOR_BG);
    lcd_setColorBg(COLOR_INACTIVE);
    lcd_setFont(SevenSegNumFont);
    snprintf(buf, 5, "%04d", freq);
    lcd_print(buf, TUNER_HZ_X, TUNER_HZ_Y);
}
