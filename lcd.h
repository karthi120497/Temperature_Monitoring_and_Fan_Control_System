#ifndef LCD_H
#define LCD_H

#include <xc.h>
#define _XTAL_FREQ 20000000UL  

// --- Connections (adjust if needed) ---
#define RS RD0
#define EN RD1
#define D4 RD2
#define D5 RD3
#define D6 RD4
#define D7 RD5

// --- LCD Functions ---
void LCD_Init(void);
void LCD_Cmd(unsigned char cmd);
void LCD_Char(unsigned char data);
void LCD_Write_String(const char *str);
void LCD_Set_Cursor(unsigned char row, unsigned char column);
void LCD_Clear(void);

#endif
