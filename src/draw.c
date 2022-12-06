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

static int fft_hists[FFT_SIZE/2];
void draw_ffthist(float complex *a) {
    int i, x, m = fft_max(a, FFT_SIZE);
    float scale = 1.0f / cabsf(a[m]);
    lcd_setColor(COLOR_INACTIVE);
    for (i = 1; i < FFT_SIZE/2; i += 2) {
        x = cabsf(a[i]) * scale * LCD_X_SIZE;
        lcd_rect(0, 32 + i/2, x, 1);
    }
}
void draw_erase_ffthist() {
    int i;
    for (i = 1; i < FFT_SIZE/2; ++i) fft_hists[i] = 0;
}
