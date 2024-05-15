#include "xc.h"
#include "libpic30.h"

// Konfiguracja
#pragma config POSCMOD = HS             // Primary Oscillator Select (HS Oscillator mode selected)
#pragma config OSCIOFNC = OFF           // Primary Oscillator Output Function (OSC2/CLKO/RC15 functions as CLKO (FOSC/2))
#pragma config FCKSM = CSDCMD           // Clock Switching and Monitor (Clock switching and Fail-Safe Clock Monitor are disabled)
#pragma config FNOSC = PRIPLL           // Oscillator Select (Primary Oscillator with PLL module (HSPLL, ECPLL))
#pragma config IESO = OFF               // Internal External Switch Over Mode (IESO mode (Two-Speed Start-up) disabled)

#pragma config WDTPS = PS32768          // Watchdog Timer Postscaler (1:32,768)
#pragma config FWPSA = PR128            // WDT Prescaler (Prescaler ratio of 1:128)
#pragma config WINDIS = ON              // Watchdog Timer Window (Standard Watchdog Timer enabled,(Windowed-mode is disabled))
#pragma config FWDTEN = OFF             // Watchdog Timer Enable (Watchdog Timer is disabled)
#pragma config ICS = PGx2               // Comm Channel Select (Emulator/debugger uses EMUC2/EMUD2)
#pragma config GWRP = OFF               // General Code Segment Write Protect (Writes to program memory are allowed)
#pragma config GCP = OFF                // General Code Segment Code Protect (Code protection is disabled)
#pragma config JTAGEN = OFF             // JTAG Port Enable (JTAG port is disabled)


int main(void) {
    unsigned portValue = 0;
    TRISA = 0X0000;
    TRISB = 0XFFFF;

    AD1CON1 = 0X80E4;
    AD1CON2 = 0X0404;
    AD1CON3 = 0X0F00;
    AD1CHS = 0;
    AD1CSSL = 0X0020;
    char prevS6 = 0, currentS6 = 0;
    char flag = 1;

    while(1) {
        prevS6 = PORTDbits.RD6; 
        __delay32(15000);
        currentS6 = PORTDbits.RD6;
        if (currentS6 - prevS6 == -1) {
           LATA = 0; 
        }

        while(!AD1CON1bits.DONE);
        portValue = ADC1BUF0;

        if (portValue > 512) {
            flag = 1;
            for (int i = 0; i < 40; i++) {
                prevS6 = PORTDbits.RD6; 
                __delay32(1500000);
                currentS6 = PORTDbits.RD6;
                if (currentS6 - prevS6 == -1) {
                   LATA = 0;
                   flag = 0; 
                   i = 0;
                   __delay32(30000000); 
                   break; 
                }
                else if (ADC1BUF0 < 512) {
                    LATA = 0;
                    flag = 1;
                    i = 0;
                    __delay32(8000000);
                    break;
                }
                LATA = 0b0000001;
                __delay32(400000);
                LATA = 0b0000000;
                __delay32(400000); 
            }
            if (flag) {
                while (1) {
                    prevS6 = PORTDbits.RD6; 
                    __delay32(15000);
                    currentS6 = PORTDbits.RD6;
                    
                    if (currentS6 - prevS6 == -1) {
                        LATA = 0;
                        __delay32(10000000); 
                        break; 
                    }
                    
                    if (ADC1BUF0 > 512) {
                        LATA = 0b11111111;
                    } else {
                        LATA = 0;
                        __delay32(3000000);
                        break;
                    }
                }
            }
        } 
    }

    return 0;
}
