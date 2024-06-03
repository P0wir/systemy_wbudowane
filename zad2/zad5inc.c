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

#include "xc.h"
#include <libpic30.h>

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


void __delay_us(unsigned long us){
    __delay32(us*FCY/1000000);
}

void __delay_ms(unsigned long ms){
    __delay32(ms*FCY/1000);
}

void LCD_sendCommand(unsigned char command){
    LCD_RW = 0;
    LCD_RS = 0;
    LCD_E = 1;
    LCD_DATA = command;
    __delay_us(50);
    LCD_E = 0;
}

void LCD_sendData(unsigned char data){
    LCD_RW = 0;
    LCD_RS = 1;
    LCD_E = 1;
    LCD_DATA = data;
    __delay_us(50);
    LCD_E = 0;
}

void LCD_print(unsigned char* string){
    while(*string){
        LCD_sendData(*string++);
    }
}

void LCD_setCursor(unsigned char row, unsigned char col){
    unsigned char address;
    if (row == 1){
        address = LCD_CURSOR + LINE1 + col;
    }
    if (row == 2){
        address = LCD_CURSOR + LINE2 + col;
    }
    LCD_sendCommand(address);
}

void LCD_init(){
    __delay_ms(20);
    LCD_sendCommand(LCD_CONFIG);
    __delay_us(50);
    LCD_sendCommand(LCD_ON);
    __delay_us(50);
    LCD_sendCommand(LCD_CLEAR);
    __delay_ms(2);
}

void LCD_clear() {
    LCD_sendCommand(LCD_CLEAR);
    __delay_ms(2);
}

int main(void) {
    TRISB = 0x7FFF;
    TRISD = 0b11000000;
    TRISE = 0x0000;
    AD1CON1 = 0x80E4;               //AD1CON1 = 0b1000000011100100;
    AD1CON2 = 0x0404;               //AD1CON2 = 0b0000010000000100;
    AD1CON3 = 0x0F00;               //AD1CON3 = 0b0000111100000000;
    AD1CHS = 0;
    AD1CSSL = 0x0020;               //AD1CSSL = 0b0000000000100000;

    unsigned char currentS6 = 0, currentS7 = 0, prevS6 = 0, prevS7 = 0;

    int player1_time = 600;
    int player2_time = 600;
    int current_player = 2;

    char buffer[16];

    LCD_init();

    while(1) {
        int minutes1 = player1_time / 60;
        int seconds1 = player1_time % 60;
        int minutes2 = player2_time / 60;
        int seconds2 = player2_time % 60;

        LCD_setCursor(1, 0);
        sprintf(buffer, "Player 1: %02d:%02d", minutes1, seconds1);
        LCD_print((unsigned char *)buffer);

        LCD_setCursor(2, 0);
        sprintf(buffer, "Player 2: %02d:%02d", minutes2, seconds2);
        LCD_print((unsigned char *)buffer);

        prevS6 = PORTDbits.RD6;
        prevS7 = PORTDbits.RD7;
        __delay_ms(1000);
        currentS6 = PORTDbits.RD6;
        currentS7 = PORTDbits.RD7;

        if (currentS6 - prevS6 == -1) {
            if(current_player == 1)
            {
                current_player = 2;
            }
            else
            {
                current_player = 1;
            }
        }

        if (currentS7 - prevS7 == -1) {
            if (current_player == 1) {
                player1_time += 30;
            } else {
                player2_time += 30;
            }
        }

        if (current_player == 1 && player1_time > 0) {
            player1_time--;
        } else if (current_player == 2 && player2_time > 0) {
            player2_time--;
        }

        if (player1_time == 0 || player2_time == 0) {
            LCD_clear();
            if (player1_time == 0) {
                LCD_setCursor(1, 0);
                LCD_print("Player 2 wins");
            } else {
                LCD_setCursor(1, 0);
                LCD_print("Player 1 wins");
            }
        }
    }

    return 0;
}
