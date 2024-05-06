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

#define DELAY_MS 1000 // 1 sekunda


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

    while(1) {
        prevS6 = PORTDbits.RD6; 
        currentS6 = PORTDbits.RD6;
        // Sprawdź, czy przyciski są wciśnięte
        if (currentS6 - prevS6 = 1) {
            // Wyłącz alarm
            LATA = 0;
            continue;
        }

        while(!AD1CON1bits.DONE);
        portValue = ADC1BUF0;

        // Sprawdź, czy przekroczono wartość 127
        if (portValue > 127) {
            // Migotanie diody przez 5 sekund
            for (int i = 0; i < 5; ++i) {
                LATA = 0b00000001; // Zapal jedną diodę
                __delay32(5000000); //
                LATA = 0; // Wyłącz diodę
                __delay32(1000000); //
            }
            // Po zakończeniu migotania, zapal wszystkie diody
            LATA = 255;
        } else {
            // Jeśli wartość nie przekroczyła 127, ustaw LATA na wartość odczytaną z ADC
            LATA = portValue;
        }
    }

    return 0;
}
