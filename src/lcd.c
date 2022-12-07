#include <xil_io.h>
#include <xspi_l.h>
#include <xparameters.h>

#include "lcd.h"

#define SPI_DC          XPAR_SPI_DC_BASEADDR
#define B_RS            0x00000001

#define SPI_DTR         XPAR_SPI_BASEADDR + XSP_DTR_OFFSET
#define SPI_DRR         XPAR_SPI_BASEADDR + XSP_DRR_OFFSET
#define SPI_IISR        XPAR_SPI_BASEADDR + XSP_IISR_OFFSET
#define SPI_SR          XPAR_SPI_BASEADDR + XSP_SR_OFFSET

// Global variables
static int fch;
static int fcl;
static int bch;
static int bcl;
static struct {
	u8 *font;
	u8 x_size;
	u8 y_size;
	u8 offset;
	u8 numchars;
} cfont;


// Write command to LCD controller
void LCD_Write_COM(char VL) {
	Xil_Out32(SPI_DC, 0x0);
	Xil_Out32(SPI_DTR, VL);

	while (0 == (Xil_In32(SPI_IISR) & XSP_INTR_TX_EMPTY_MASK));
	Xil_Out32(SPI_IISR, Xil_In32(SPI_IISR) | XSP_INTR_TX_EMPTY_MASK);
}


// Write 16-bit data to LCD controller
void LCD_Write_DATA16(char VH, char VL) {
	Xil_Out32(SPI_DC, 0x01);
	Xil_Out32(SPI_DTR, VH);
	Xil_Out32(SPI_DTR, VL);

	while (0 == (Xil_In32(SPI_IISR) & XSP_INTR_TX_EMPTY_MASK));
	Xil_Out32(SPI_IISR, Xil_In32(SPI_IISR) | XSP_INTR_TX_EMPTY_MASK);
}


// Write 8-bit data to LCD controller
void LCD_Write_DATA(char VL) {
	Xil_Out32(SPI_DC, 0x01);
	Xil_Out32(SPI_DTR, VL);

	while (0 == (Xil_In32(SPI_IISR) & XSP_INTR_TX_EMPTY_MASK));
	Xil_Out32(SPI_IISR, Xil_In32(SPI_IISR) | XSP_INTR_TX_EMPTY_MASK);
}


// Initialize LCD controller
void lcd_init(void) {
	int i;

	// Reset
	LCD_Write_COM(0x01);
	for (i = 0; i < 500000; i++); //Must wait > 5ms

	LCD_Write_COM(0xCB);
	LCD_Write_DATA(0x39);
	LCD_Write_DATA(0x2C);
	LCD_Write_DATA(0x00);
	LCD_Write_DATA(0x34);
	LCD_Write_DATA(0x02);

	LCD_Write_COM(0xCF);
	LCD_Write_DATA(0x00);
	LCD_Write_DATA(0XC1);
	LCD_Write_DATA(0X30);

	LCD_Write_COM(0xE8);
	LCD_Write_DATA(0x85);
	LCD_Write_DATA(0x00);
	LCD_Write_DATA(0x78);

	LCD_Write_COM(0xEA);
	LCD_Write_DATA(0x00);
	LCD_Write_DATA(0x00);

	LCD_Write_COM(0xED);
	LCD_Write_DATA(0x64);
	LCD_Write_DATA(0x03);
	LCD_Write_DATA(0X12);
	LCD_Write_DATA(0X81);

	LCD_Write_COM(0xF7);
	LCD_Write_DATA(0x20);

	LCD_Write_COM(0xC0);   //Power control
	LCD_Write_DATA(0x23);  //VRH[5:0]

	LCD_Write_COM(0xC1);   //Power control
	LCD_Write_DATA(0x10);  //SAP[2:0];BT[3:0]

	LCD_Write_COM(0xC5);   //VCM control
	LCD_Write_DATA(0x3e);  //Contrast
	LCD_Write_DATA(0x28);

	LCD_Write_COM(0xC7);   //VCM control2
	LCD_Write_DATA(0x86);  //--

	LCD_Write_COM(0x36);   // Memory Access Control
	LCD_Write_DATA(0x48);

	LCD_Write_COM(0x3A);
	LCD_Write_DATA(0x55);

	LCD_Write_COM(0xB1);
	LCD_Write_DATA(0x00);
	LCD_Write_DATA(0x18);

	LCD_Write_COM(0xB6);   // Display Function Control
	LCD_Write_DATA(0x08);
	LCD_Write_DATA(0x82);
	LCD_Write_DATA(0x27);

	LCD_Write_COM(0x11);   //Exit Sleep
	for (i = 0; i < 100000; i++);

	LCD_Write_COM(0x29);   //Display on
	LCD_Write_COM(0x2c);

	// Default color and fonts
	fch = 0xFF;
	fcl = 0xFF;
	bch = 0x00;
	bcl = 0x00;
	lcd_setFont(SmallFont);
}


// Set boundary for drawing
void lcd_setXY(int x1, int y1, int x2, int y2) {
	--x2;
	--y2;
	LCD_Write_COM(0x2A);
	LCD_Write_DATA(x1 >> 8);
	LCD_Write_DATA(x1);
	LCD_Write_DATA(x2 >> 8);
	LCD_Write_DATA(x2);
	LCD_Write_COM(0x2B);
	LCD_Write_DATA(y1 >> 8);
	LCD_Write_DATA(y1);
	LCD_Write_DATA(y2 >> 8);
	LCD_Write_DATA(y2);
	LCD_Write_COM(0x2C);
}


// Set foreground RGB color for next drawing
void lcd_setColor(u8 r, u8 g, u8 b) {
	// 5-bit r, 6-bit g, 5-bit b
	fch = (r & 0x0F8) | g >> 5;
	fcl = (g & 0x1C) << 3 | b >> 3;
}


// Set background RGB color for next drawing
void lcd_setColorBg(u8 r, u8 g, u8 b) {
	// 5-bit r, 6-bit g, 5-bit b
	bch = (r & 0x0F8) | g >> 5;
	bcl = (g & 0x1C) << 3 | b >> 3;
}


void lcd_rect(int x, int y, int xlen, int ylen) {
	int i, area = xlen * ylen;
	lcd_setXY(x, y, x+xlen, y+ylen);
	for (i = 0; i < area; i++) LCD_Write_DATA16(fch, fcl);
}


void lcd_setFont(u8 *font) {
	cfont.font = font;
	cfont.x_size = font[0];
	cfont.y_size = font[1];
	cfont.offset = font[2];
	cfont.numchars = font[3];
}


static void print_char(u8 c, int x, int y) {
	u8 ch;
	int i, j, jlim, pi;

	lcd_setXY(x, y, x + cfont.x_size, y + cfont.y_size);

	jlim = (cfont.x_size >> 3) * cfont.y_size;
	pi = (c - cfont.offset) * jlim + 4;
	for (j = 0; j < jlim; j++) {
		ch = cfont.font[pi];
		for (i = 0; i < 8; i++) {
			if (ch & (1 << (7 - i))) LCD_Write_DATA16(fch, fcl);
			else LCD_Write_DATA16(bch, bcl);
		}
		pi++;
	}
}


// Print string
void lcd_print(char *st, int x, int y) {
	int i = 0;
	while (*st) print_char(*st++, x + cfont.x_size * i++, y);
}
