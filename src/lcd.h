#ifndef LCD_H_
#define LCD_H_

#include <xil_types.h>

#define LCD_X_SIZE     240
#define LCD_Y_SIZE     320

extern u8 SmallFont[];
extern u8 BigFont[];
extern u8 SevenSegNumFont[];

u32 LCD_Read(char VL);
void LCD_Write_COM(char VL);  
void LCD_Write_DATA(char VL);
void LCD_Write_DATA16(char VH, char VL);

void lcd_init(void);
void lcd_setXY(int x1, int y1, int x2, int y2);
void lcd_setColor(u8 r, u8 g, u8 b);
void lcd_setColorBg(u8 r, u8 g, u8 b);

void lcd_rect(int x1, int y1, int x2, int y2);

void lcd_setFont(u8 *font);
void lcd_print(char *st, int x, int y);

#endif /* LCD_H_ */
