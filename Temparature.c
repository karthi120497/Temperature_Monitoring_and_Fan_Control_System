#include <xc.h>
#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>
#include "lcd.h"

#define _XTAL_FREQ 20000000UL

#pragma config FOSC = HS, WDTE = OFF, PWRTE = ON, BOREN = ON, LVP = OFF, CPD = OFF, WRT = OFF, CP = OFF

// ---------- ADC ----------
static void ADC_Init(void) {
   
    ADCON1 = 0x80;     // AN0 analog pin  
    ADCON0 = 0x41;     
}

static uint16_t ADC_Read(uint8_t ch) {
    ADCON0 &= 0xC5;             // clear channel bits
    ADCON0 |= (ch << 3);        // select channel
    __delay_ms(2);              
    GO_nDONE = 1;               
    while (GO_nDONE);           
    return ((uint16_t)ADRESH << 8) | ADRESL;
}

// ---------- PWM  ----------
static void PWM_Init(void) {
    TRISC2 = 0;                 
    PR2 = 62;
    T2CON = 0b00000100;        
    CCP1CON = 0b00001100;      
}

static void PWM_SetPercent(uint8_t pct) {
    if (pct > 100) pct = 100;
    uint16_t max = (uint16_t)(PR2 + 1) * 4;     
    uint16_t duty = (uint16_t)((uint32_t)max * pct / 100U);
    CCPR1L = (uint8_t)(duty >> 2);
    CCP1CON &= 0xCF;
    CCP1CON |= (uint8_t)((duty & 0x03) << 4);
}

// ---------- Motor  ----------
static void Motor_Init(void) {
    TRISB0 = 0; TRISB1 = 0;  // Set direction IN1=1, IN2=0
    RB0 = 1; 
    RB1 = 0;
}

// ---------- Temperature to PWM ----------
static uint8_t temp_to_pwm(float tC) {
    if (tC <= 25.0f) return 0;
    if (tC >= 40.0f) return 100;
    float pct = (tC - 25.0f) * (100.0f / 15.0f);
    if (pct < 0) pct = 0;
    if (pct > 100) pct = 100;
    return (uint8_t)(pct + 0.5f);
}

void main(void) {
    char line[17];
    uint16_t adc;
    float tC;
    uint8_t pwm;

    // LCD on PORTD
    TRISD = 0x00; 

    LCD_Init();
    LCD_Clear();
    LCD_Set_Cursor(1,1);
    LCD_Write_String("Temp & Fan Ctrl");

    ADC_Init();
    PWM_Init();
    Motor_Init();

    while (1) {
        adc = ADC_Read(0);            // AN0
        tC = (float)adc * 4.8828f / 10.0f;

        pwm = temp_to_pwm(tC);
        PWM_SetPercent(pwm);

        // LCD display
        snprintf(line, sizeof(line), "Temp:%5.1f C", tC);
        LCD_Set_Cursor(1,1);
        LCD_Write_String("                ");
        LCD_Set_Cursor(1,1);
        LCD_Write_String(line);

        snprintf(line, sizeof(line), "Fan:%3u%%       ", pwm);
        LCD_Set_Cursor(2,1);
        LCD_Write_String("                ");
        LCD_Set_Cursor(2,1);
        LCD_Write_String(line);

        __delay_ms(300);
    }
}
