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

unsigned portValue = 0;
int bcd = 0;
int a = 1;

int IntToGray(unsigned char input)
{
    return (input >>1) ^ input;
}

#define BIT_VALUE(value, noBit) ((value >> noBit) & 1)

int xor(int liczba)
{
    int Tablica[4] = {0};
    for (int i = 0; i < 4; i++)
    {
        if (BIT_VALUE(51, i) == 1)
        {
            Tablica[i] = BIT_VALUE(liczba, i);
        }
    }
    int b = 0;
    for (int i = 0; i < 4; i++)
    {
        for (int j = i + 1; j < 4; j++)
        {
            if (i != j)
            {
                b ^= (Tablica[i] ^ Tablica[j]);
            }
        }
    }
    return b;
}

void generator(int liczba)
{
    int x;
    x = liczba;
    int Tablica[6] = {0};
    for (int i = 0; i < 6; i++)
    {
        if (BIT_VALUE(liczba, i) == 1)
        {
            Tablica[i] = BIT_VALUE(liczba, i);
        }
    }
    int dziesietna = 1;
    for (int i = 0; i < 6; i++)
    {
        dziesietna += Tablica[i] * (1 << i);
    }
    unsigned portValue = 0;
    TRISA = 0x0000;
    portValue = dziesietna;
    LATA = portValue;
    Tablica[0] = xor(x);
}

void lfsr(int seed)
{
    while (1) {
        int lfsr_value = seed;
        while (1) {
            generator(lfsr_value);
            __delay32(10000000);
            lfsr_value = lfsr_value >> 1 | ((xor(lfsr_value) & 1) << 5);
        }
    }
}

void podprogram1(void) {
        LATA = portValue;
        __delay32(1000000);
        portValue++;          
}


void podprogram2(void) {
        LATA = portValue;
        __delay32(1000000);
        portValue--;
}

void podprogram3(void) {
        LATA = IntToGray(portValue);
        __delay32(1000000);
        portValue++;
}

void podprogram4(void) {
        LATA = IntToGray(portValue);
        __delay32(1000000);
        portValue--;
}

void podprogram5(void) {
        LATA = portValue;
        __delay32(10000000);
        portValue = (((bcd/10)<<4)|(bcd%10));
        if(bcd > 99)
        {
            bcd=0;
        }
        else
        {
            bcd++;
        }
}

void podprogram6(void) {
        LATA = portValue;
        __delay32(10000000);
        portValue = (((bcd/10)<<4)|(bcd%10));
        if(bcd < 0)
        {
            bcd=99;
        }
        else
        {
            bcd--;
        }
}

void podprogram7(void) {
    unsigned portValue = 7;
    char currentS6 = 0, prevS6 = 0, currentS7 = 0, prevS7 = 0;
    int left = 1;
    while(1) {
    TRISA = 0x0000;
    LATA = portValue;
        if (currentS6 - prevS6 == -1)
        {
            podprogram8();
        }

        if (currentS7 - prevS7 == -1) 
        {
            podprogram6();
        }
    __delay32(10000000);
    if(left == 1)
    {
        portValue = portValue*2;
        if (portValue == 224)
        {
            left = 2;
        }
    }
    else if(left==2)
    {
        portValue = portValue/2;
        if (portValue == 7)
        {
            left = 1;
        }
    }
    }
}

void podprogram8(void) {
    unsigned portValue = 1;
    TRISD = 0xFFFF;
    char currentS6 = 0, prevS6 = 0, currentS7 = 0, prevS7 = 0;
    TRISA = 0x0000;
    int a = 1;
    while(1)
    {
        LATA = portValue;
        __delay32(1000000);           
        if (portValue == 128) {
            a = 1;
            portValue = 129;
        }
        else if(portValue == 192)
        {
            a = 1;
            portValue = 193;
        }
        else if(portValue == 224)
        {
            a = 1;
            portValue = 225;
        }
        else if(portValue == 240)
        {
            a = 1;
            portValue = 241;
        }
        else if(portValue == 248)
        {
            a = 1;
            portValue = 249;
        }
        else if(portValue == 252)
        {
            a = 1;
            portValue = 253;
        }
        else if(portValue == 254)
        {
            a = 1;
            portValue = 255;
        }
        else if(portValue == 255)
        {
            break;
        }
        else {
            portValue += a;
            a *= 2;
        }
        
    }
}

int main(void) { 
    int podprogram = 1;
    TRISA = 0x0000;
    TRISD = 0xFFFF;
    char currentS6 = 0, prevS6 = 0, currentS7 = 0, prevS7 = 0;
    while(1) {
        prevS6 = PORTDbits.RD6; 
        prevS7 = PORTDbits.RD7;
        __delay32(150000);

        currentS6 = PORTDbits.RD6;
        currentS7 = PORTDbits.RD7;

        if (currentS6 - prevS6 == -1)
        {
            podprogram++;
            if (podprogram > 9) 
                podprogram = 1; 
        }

        if (currentS7 - prevS7 == -1) 
        {
            podprogram--;
            if (podprogram < 1)
                podprogram = 9; 
        }

        if (podprogram == 1) {
            podprogram1();
        } else if (podprogram == 2) {
            podprogram2();
        } else if (podprogram == 3) {
            podprogram3();
        } else if (podprogram == 4) {
            podprogram4();
        } else if (podprogram == 5) {
            podprogram5();
        } else if (podprogram == 6) {
            podprogram6();
        } else if (podprogram == 7) {
            podprogram7();
        }  else if (podprogram == 8) {
           podprogram8();
        } else if (podprogram == 9) {
            lfsr(105);
        } else {
            break;
        }
    }
    return 0;
}
