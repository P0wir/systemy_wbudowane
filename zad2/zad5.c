/*
 * File:   main.c
 * Author: local
 *
 * Created on April 10, 2024, 1:34 PM
 */
#pragma config POSCMOD = NONE             // Primary Oscillator Select (primary oscillator disabled)
#pragma config OSCIOFNC = OFF           // Primary Oscillator Output Function (OSC2/CLKO/RC15 functions as CLKO (FOSC/2))
#pragma config FCKSM = CSDCMD           // Clock Switching and Monitor (Clock switching and Fail-Safe Clock Monitor are disabled)
#pragma config FNOSC = FRC              // Oscillator Select (Fast RC Oscillator without Postscaler)
#pragma config IESO = OFF               // Internal External Switch Over Mode (IESO mode (Two-Speed Start-up) disabled)
// CONFIG1
#pragma config WDTPS = PS32768 // Watchdog Timer Postscaler (1:32,768)
#pragma config FWPSA = PR128 // WDT Prescaler (1:128)
#pragma config WINDIS = ON // Watchdog Timer Window Mode disabled
#pragma config FWDTEN = OFF // Watchdog Timer disabled
#pragma config ICS = PGx2 // Emulator/debugger uses EMUC2/EMUD2
#pragma config GWRP = OFF // Writes to program memory allowed
#pragma config GCP = OFF // Code protection is disabled
#pragma config JTAGEN = OFF // JTAG port is disabled

#define FCY 4000000UL

#define LCD_E LATDbits.LATD4
#define LCD_RW LATDbits.LATD5
#define LCD_RS LATBbits.LATB15
#define LCD_DATA LATE

#define LCD_CLEAR 0x01
#define LCD_HOME 0x02
#define LCD_ON 0x0C
#define LCD_OFF 0x08
#define LCD_CONFIG 0x38
#define LCD_CURSOR 0x80
#define LINE1 0x00
#define LINE2 0x40
#define LCD_SHIFT_R 0x1D
#define LCD_SHIFT_L 0x1B

#include "xc.h"
#include <libpic30.h>

void __delay_us(unsigned long us) {
    __delay32(us * FCY / 1000000);
}

void __delay_ms(unsigned long ms) {
    __delay32(ms * FCY / 1000);
}

void LCD_sendCommand(unsigned char command) {
    LCD_RW = 0;
    LCD_RS = 0;
    LCD_E = 1;
    LCD_DATA = command;
    __delay_us(50);
    LCD_E = 0;
}

void LCD_sendData(unsigned char data) {
    LCD_RW = 0;
    LCD_RS = 1;
    LCD_E = 1;
    LCD_DATA = data;
    __delay_us(50);
    LCD_E = 0;
}

void LCD_print(unsigned char *string) {
    while (*string) {
        LCD_sendData(*string++);
    }
}

void LCD_setCursor(unsigned char row, unsigned char col) {
    unsigned char address;
    if (row == 1) {
        address = LCD_CURSOR + LINE1 + col;
    } else if (row == 2) {
        address = LCD_CURSOR + LINE2 + col;
    }
    LCD_sendCommand(address);
}

void LCD_clear() {
    LCD_sendCommand(LCD_CLEAR);
    __delay_ms(2);
}

void LCD_init() {
    __delay_ms(20);
    LCD_sendCommand(LCD_CONFIG);
    __delay_us(50);
    LCD_sendCommand(LCD_ON);
    __delay_us(50);
    LCD_clear();
}

void LCD_printChar(char c) {
    LCD_sendData((unsigned char)c);
}

void LCD_printTime(int minutes, int seconds) {
    LCD_printChar((minutes / 10) + '0');
    LCD_printChar((minutes % 10) + '0');
    LCD_printChar(':');
    LCD_printChar((seconds / 10) + '0');
    LCD_printChar((seconds % 10) + '0');
}

void display_time(unsigned char row, int minutes, int seconds) {
    LCD_setCursor(row, 0);
    LCD_printTime(minutes, seconds);
}

int main(void) {
    TRISA = 0x0000;
    TRISB = 0xFFFF;
    TRISD = 0xFFFF; // Configure PORTD as input

    LCD_init();

    int player1_time = 600;
    int player2_time = 600;
    int current_player = 1;
    int is_started = 0;

    int prevS6 = PORTDbits.RD6;
    int currentS6;
    int prevS7 = PORTDbits.RD7;
    int currentS7;

    LCD_clear();
    display_time(1, player1_time / 60, player1_time % 60);
    display_time(2, player2_time / 60, player2_time % 60);

    while (1) {
        prevS7 = PORTDbits.RD7; 
        __delay_ms(998); 
        currentS7 = PORTDbits.RD7;

        if (currentS7 != prevS7) {
            if (currentS7 == 1) {
                is_started = 1; 
            }
            prevS7 = currentS7;
        }

        if (is_started) {
            prevS6 = PORTDbits.RD6;
            __delay_ms(998);
            currentS6 = PORTDbits.RD6;

            if (currentS6 != prevS6) {
                if (currentS6 == 1) {
                    current_player = (current_player == 1) ? 2 : 1;
                }
                prevS6 = currentS6;
            }

            if (current_player == 1) {
                if (player1_time > 0) {
                    player1_time--;
                    display_time(1, player1_time / 60, player1_time % 60);
                }
            } else {
                if (player2_time > 0) {
                    player2_time--;
                    display_time(2, player2_time / 60, player2_time % 60);
                }
            }

            if (player1_time == 0 || player2_time == 0) {
                break;
            }
        }
    }

    LCD_clear();
    __delay_ms(2);
    if (player1_time == 0) {
        LCD_setCursor(1, 0);
        LCD_print((unsigned char *)"Player 2 Wins!");
    } else if (player2_time == 0) {
        LCD_setCursor(1, 0);
        LCD_print((unsigned char *)"Player 1 Wins!");
    }

    return 0;
}
