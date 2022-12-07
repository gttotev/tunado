#ifndef LCD_H_
#define LCD_H_

#include <xil_types.h>

#define LCD_X_SIZE     240
#define LCD_Y_SIZE     320

#define LCD_COLOR(r, g, b) \
	((r & 0x0F8) | g >> 5), ((g & 0x1C) << 3 | b >> 3)
#define LCD_COLORS(x) LCD_COLOR(x)

extern u8 SmallFont[];
extern u8 BigFont[];
extern u8 SevenSegNumFont[];

void LCD_Write_COM(char VL);
void LCD_Write_DATA(char VL);
void LCD_Write_DATA16(char VH, char VL);

void lcd_init(void);
void lcd_setXY(int x1, int y1, int x2, int y2);
void lcd_setColor(u8 r, u8 g, u8 b);
void lcd_setColorBg(u8 r, u8 g, u8 b);

void lcd_rect(int x, int y, int xlen, int ylen);

void lcd_setFont(u8 *font);
void lcd_print(char *st, int x, int y);

#endif /* LCD_H_ */
