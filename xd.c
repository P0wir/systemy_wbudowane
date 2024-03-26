if (currentS7 != prevS7) {
            if (currentS7 == 1) {
                // Przesuń kolej w prawo
                portValue >>= 1;
                // Jeśli kolejka zmniejszy się poniżej 0000001, ustaw ją na 1100000
                if (portValue < 0b0000001) {
                    portValue = 0b1100000;
                }
            }
        }

#include "xc.h"
#include <libpic30.h>

int main(void) {
    int podprogram = 0;
    TRISD = 0xFFFF;
    unsigned char currentS6 = 0, currentS7 = 0, prevS6 = 0, prevS7 = 0;

    while(1) {
        // Odczyt stanu przycisków
        prevS6 = currentS6;
        prevS7 = currentS7;
        currentS6 = PORTDbits.RD6;
        currentS7 = PORTDbits.RD7;

        // Sprawdzenie zmiany stanu przycisków i zmiana podprogramu
        if (currentS6 != prevS6) {
            if (currentS6 == 1) {
                podprogram++;
            }
        }
        
        if (currentS7 != prevS7) {
            if (currentS7 == 1) {
                podprogram--;
            }
        }

        // Wykonanie działań zależnych od wartości podprogramu
        if (podprogram == 1) {
            unsigned portValue = 0;
            TRISA = 0x0000;
            while(1) {
                LATA = portValue;
                __delay32(1000000);
                portValue++;
            }
        } else if (podprogram == 0) {
            unsigned portValue = 255;
            TRISA = 0x0000;
            while(1) {
                LATA = portValue;
                __delay32(1000000);
                portValue--;
            }
        }
    }
    return 0;
}


