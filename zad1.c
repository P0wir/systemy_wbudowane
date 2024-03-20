/*
 * File:   main.c
 * Author: local
 *
 * Created on March 20, 2024, 3:32 PM
 */

// CONFIG2
#pragma config POSCMOD = HS             // Primary Oscillator Select (HS Oscillator mode selected)
#pragma config OSCIOFNC = OFF           // Primary Oscillator Output Function (OSC2/CLKO/RC15 functions as CLKO (FOSC/2))
#pragma config FCKSM = CSDCMD           // Clock Switching and Monitor (Clock switching and Fail-Safe Clock Monitor are disabled)
#pragma config FNOSC = PRIPLL           // Oscillator Select (Primary Oscillator with PLL module (HSPLL, ECPLL))
#pragma config IESO = OFF               // Internal External Switch Over Mode (IESO mode (Two-Speed Start-up) disabled)

// CONFIG1
#pragma config WDTPS = PS32768          // Watchdog Timer Postscaler (1:32,768)
#pragma config FWPSA = PR128            // WDT Prescaler (Prescaler ratio of 1:128)
#pragma config WINDIS = ON              // Watchdog Timer Window (Standard Watchdog Timer enabled,(Windowed-mode is disabled))
#pragma config FWDTEN = OFF             // Watchdog Timer Enable (Watchdog Timer is disabled)
#pragma config ICS = PGx2               // Comm Channel Select (Emulator/debugger uses EMUC2/EMUD2)
#pragma config GWRP = OFF               // General Code Segment Write Protect (Writes to program memory are allowed)
#pragma config GCP = OFF                // General Code Segment Code Protect (Code protection is disabled)
#pragma config JTAGEN = OFF             // JTAG Port Enable (JTAG port is disabled)

#include "xc.h"
#include <libpic30.h>

int main(void) {
    int podprogram = 0;
    TRISD = 0xFFFF;
    char currentS6 = 0, currentS7 = 0, prevS6 = 0, prevS7 = 0;
    prevS6 = PORTDbits.RD6;
    prevS7 = PORTDbits.RD7;
    currentS6 = PORTDbits.RD6;
    currentS7 = PORTDbits.RD7;
    if (currentS6 - prevS6 == 1){
        podprogram++;
    }
    if (currentS7 - prevS7 == 1){
        podprogram--;
    }
    if(podprogram == 1)
    {
        unsigned portValue = 0;
        TRISA = 0x0000;
        while(1) {
        LATA = portValue;
        __delay32(1000000);
        portValue++;
    }
    }
    if(podprogram == 0)
    {
        unsigned portValue = 255;
        TRISA = 0x0000;
        while(1) {
        LATA = portValue;
        __delay32(1000000);
        portValue--;
    }
    }
    return 0;
}
