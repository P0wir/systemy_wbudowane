/*
 * File:   newmainXC16.c
 * Author: local
 *
 * Created on 29 maja 2024, 15:16
 */

// CONFIGURATION BITS
#pragma config POSCMOD = NONE             // Primary Oscillator Select (primary oscillator disabled)
#pragma config OSCIOFNC = OFF             // Primary Oscillator Output Function (OSC2/CLKO/RC15 functions as CLKO (FOSC/2))
#pragma config FCKSM = CSDCMD             // Clock Switching and Monitor (Clock switching and Fail-Safe Clock Monitor are disabled)
#pragma config FNOSC = FRC                // Oscillator Select (Fast RC Oscillator without Postscaler)
#pragma config IESO = OFF                 // Internal External Switch Over Mode (IESO mode (Two-Speed Start-up) disabled)
#pragma config WDTPS = PS32768            // Watchdog Timer Postscaler (1:32,768)
#pragma config FWPSA = PR128              // WDT Prescaler (1:128)
#pragma config WINDIS = ON                // Watchdog Timer Window Mode disabled
#pragma config FWDTEN = OFF               // Watchdog Timer disabled
#pragma config ICS = PGx2                 // Emulator/debugger uses EMUC2/EMUD2
#pragma config GWRP = OFF                 // Writes to program memory allowed
#pragma config GCP = OFF                  // Code protection is disabled
#pragma config JTAGEN = OFF               // JTAG port is disabled

#include "xc.h"
#include <libpic30.h>
#include <stdbool.h>

// Define macros for better readability
#define FCY         4000000UL   // Working frequency of the oscillator

// Define pins as macros
#define LCD_E       LATDbits.LATD4  
#define LCD_RW      LATDbits.LATD5
#define LCD_RS      LATBbits.LATB15
#define LCD_DATA    LATE

// Define command values as macros
#define LCD_CLEAR       0x01
#define LCD_HOME        0x02
#define LCD_ON          0x0C
#define LCD_OFF         0x08
#define LCD_CONFIG      0x38
#define LCD_CURSOR      0x80
#define LINE1           0x00
#define LINE2           0x40
#define LCD_CUST_CHAR   0x40
#define LCD_SHIFT_R     0x1D
#define LCD_SHIFT_L     0x1B

// Define delay functions
void __delay_us(unsigned long us){
    __delay32(us*FCY/1000000);
}

void __delay_ms(unsigned long ms){
    __delay32(ms*FCY/1000);
}

// Define functions to send commands and data to the LCD
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
    if(row == 1){
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

unsigned int display(void){
    AD1CON1bits.SAMP = 1;
    while(!AD1CON1bits.DONE);
    return ADC1BUF0;
}

void time_display(unsigned int time){
    LCD_setCursor(1, 0);
    LCD_print("Czas: ");
    LCD_sendData('0' + time / 60);
    LCD_sendData(':');
    LCD_sendData('0' + (time % 60) / 10);
    LCD_sendData('0' + time % 10);
}

void power_display(unsigned int power){
    char powerStr[5];
    snprintf(powerStr, sizeof(powerStr), "%u%%", power);
    LCD_setCursor(2, 0);
    LCD_print("Moc: ");
    LCD_print(powerStr);
}

int main(void) {
    char currentS6 = 0, prevS6 = 0, currentS7 = 0, prevS7 = 0, currentS3 = 0, prevS3 = 0, currentS4 = 0, prevS4 = 0;
    unsigned int power = 0;
    unsigned int time = 0;
    bool start = false;
    
    TRISB = 0x7FFF;  
    TRISD = 0xFFE7;
    TRISE = 0x0000;
    
    // ADC Configuration
    AD1CON1 = 0x80E4; 
    AD1CON2 = 0x0404;
    AD1CON3 = 0x0F00;
    AD1CHS = 0;
    AD1CSSL = 0x0020;
    
    LCD_init(); 
   
    while(1) {
        power = display() / 10; 
        if (power > 100) power = 100; 
        
        prevS6 = PORTDbits.RD6; 
        prevS7 = PORTDbits.RD7;
        prevS3 = PORTAbits.RA7;
        prevS4 = PORTDbits.RD13;
        __delay_ms(1000); 
        currentS6 = PORTDbits.RD6;
        currentS7 = PORTDbits.RD7;
        currentS3 = PORTAbits.RA7;
        currentS4 = PORTDbits.RD13;
        
        if(currentS6 - prevS6 == 1) {
            time += 30;
        }
       
        if(currentS7 - prevS7 == 1) {  
            start = true;
        }
        
        if(currentS3 - prevS3 == 1) {  
            time = 0;
            start = false;
        }

        if(currentS4 - prevS4 == 1) {
            start = false;
        }
        
        if(start && time > 0) {
            time--;
        }
        
        if(time == 0) {
            start = false;
        }
        
        power_display(power);
        time_display(time);
    }
}
