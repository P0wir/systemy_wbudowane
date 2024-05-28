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
    }
    if (row == 2) {
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

void display_time(unsigned char row, int minutes, int seconds) {
    char buffer[9];
    sprintf(buffer, "%02d:%02d", minutes, seconds);
    LCD_setCursor(row, 0);
    LCD_print((unsigned char *)buffer);
}

int main(void) {
    TRISA = 0X0000;
    TRISB = 0XFFFF;

    LCD_init();

    int player1_time = 600;
    int player2_time = 600;
    int current_player = 1;

    int prevS6 = PORTDbits.RD6;
    int currentS6;

    LCD_clear();
    display_time(1, player1_time / 60, player1_time % 60);
    display_time(2, player2_time / 60, player2_time % 60);

    while (1) {
        currentS6 = PORTDbits.RD6;
        __delay32(15000);  // Delay to debounce the button

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

        __delay_ms(1000);

        if (player1_time == 0 || player2_time == 0) {
            break;
        }
    }

    LCD_clear();
    if (player1_time == 0) {
        LCD_setCursor(1, 0);
        LCD_print((unsigned char *)"Player 2 Wins!");
    } else if (player2_time == 0) {
        LCD_setCursor(1, 0);
        LCD_print((unsigned char *)"Player 1 Wins!");
    }

    return 0;
}
