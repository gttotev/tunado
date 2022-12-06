#include <string.h>
#include <fastmath.h>

#include "ui.h"
#include "lcd.h"

#define BIGFONT_CHAR_PX 16

#define COLOR_BG 0xCD, 0xEA, 0xC0
#define COLOR_TXT COLOR_BG
#define COLOR_ACTIVE 0xE0, 0xAF, 0xA0
#define COLOR_INACTIVE 0x61, 0x29, 0x40

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
void draw_init_mainmenu(int pos) {
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
void draw_erase_mainmenu(int pos) {
    int i;
    lcd_setColor(COLOR_BG);
    MAINMENU_CUR(pos);
    for (i = 0; i < MAINMENU_SIZE; ++i) lcd_rect(
        MAINMENU_X, MAINMENU_Y(i),
        strlen(mainmenu_txt[i])*BIGFONT_CHAR_PX,
        BIGFONT_CHAR_PX
    );
}

#define FFTHIST_SIZE 255
#define FFTHIST_Y(i) ((i) + (LCD_Y_SIZE-FFTHIST_SIZE)/2)
static float fft_mag[FFT_SIZE];
static int fft_hists[FFTHIST_SIZE];
void draw_ffthist(float complex *a) {
    int i, x, bl, bh, m = fft_max(a, FFT_SIZE, fft_mag);
    float scale = 1 / sqrtf(fft_mag[m]);

    bl = m - FFTHIST_SIZE/2;
    if (bl < 1) bl = 1;
    bh = bl + FFTHIST_SIZE;
    if (bh > FFT_SIZE/2) {
        bh = FFT_SIZE/2;
        bl = bh - FFTHIST_SIZE;
    }

    for (i = 0; i < FFTHIST_SIZE; ++i) {
        x = sqrtf(fft_mag[bl+i]) * scale * LCD_X_SIZE;
        if (x == fft_hists[i]) continue;
        if (x > fft_hists[i]) {
            lcd_setColor(COLOR_INACTIVE);
            lcd_rect(fft_hists[i], FFTHIST_Y(i), x-fft_hists[i], 1);
        } else {
            lcd_setColor(COLOR_BG);
            lcd_rect(x, FFTHIST_Y(i), fft_hists[i]-x, 1);
        }
        fft_hists[i] = x;
    }
}
void draw_erase_ffthist() {
    int i;
    lcd_setColor(COLOR_BG);
    for (i = 0; i < FFTHIST_SIZE; ++i) {
        lcd_rect(0, FFTHIST_Y(i), fft_hists[i], 1);
        fft_hists[i] = 0;
    }
}
