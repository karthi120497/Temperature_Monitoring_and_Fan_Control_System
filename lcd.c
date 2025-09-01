#include "lcd.h"

// Pulse EN to latch data
static void LCD_Pulse(void) {
    EN = 1;
    __delay_us(40);
    EN = 0;
    __delay_us(40);
}

// Send 4-bit nibble
static void LCD_Send4Bits(unsigned char data) {
    D4 = (data >> 0) & 1;
    D5 = (data >> 1) & 1;
    D6 = (data >> 2) & 1;
    D7 = (data >> 3) & 1;
    LCD_Pulse();
}

// Send command or data (8-bit, split into two nibbles)
static void LCD_Send(unsigned char data, unsigned char rs) {
    RS = rs; // RS=0 ? command, RS=1 ? data
    LCD_Send4Bits(data >> 4);   // higher nibble
    LCD_Send4Bits(data & 0x0F); // lower nibble
    __delay_ms(2);
}

void LCD_Cmd(unsigned char cmd) {
    LCD_Send(cmd, 0);
}

void LCD_Char(unsigned char data) {
    LCD_Send(data, 1);
}

void LCD_Write_String(const char *str) {
    while(*str) {
        LCD_Char(*str++);
    }
}

void LCD_Set_Cursor(unsigned char row, unsigned char column) {
    unsigned char pos;
    if(row == 1) pos = 0x80 + (column-1);
    else if(row == 2) pos = 0xC0 + (column-1);
    LCD_Cmd(pos);
}

void LCD_Clear(void) {
    LCD_Cmd(0x01);
    __delay_ms(2);
}

void LCD_Init(void) {
    // Configure PORTD as output
    TRISD = 0x00;
    RS = 0; EN = 0;

    __delay_ms(20);        
    LCD_Send4Bits(0x03);   // 8-bit mode init
    __delay_ms(5);
    LCD_Send4Bits(0x03);
    __delay_us(150);
    LCD_Send4Bits(0x03);
    LCD_Send4Bits(0x02);   // set 4-bit mode

    LCD_Cmd(0x28); // 4-bit, 2 line, 5x7
    LCD_Cmd(0x0C); // Display ON, Cursor OFF
    LCD_Cmd(0x06); // Entry mode
    LCD_Clear();
}
