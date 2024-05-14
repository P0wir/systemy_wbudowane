#pragma config POSCMOD = NONE // XT Oscillator mode selected
#pragma config OSCIOFNC = OFF // OSC2/CLKO/RC15 as port I/O (RC15)
#pragma config FCKSM = CSDCMD // Clock Switching and Monitor disabled
#pragma config FNOSC = FRC // Primary Oscillator (XT, HS, EC)
#pragma config IESO = OFF // Int Ext Switch Over Mode enabled

#pragma config WDTPS = PS32768 // Watchdog Timer Postscaler (1:32,768)
#pragma config FWPSA = PR128 // WDT Prescaler (1:128)
#pragma config WINDIS = ON // Watchdog Timer Window Mode disabled
#pragma config FWDTEN = OFF // Watchdog Timer disabled
#pragma config ICS = PGx2 //

#pragma config GWRP = OFF // Writes to program memory allowed
#pragma config GCP = OFF // Code protection is disabled
#pragma config JTAGEN = OFF // JTAG port is disabled

#include "xc.h"
#include "libpic30.h"

#include <math.h>

#define BIT_VALUE(value, noBit) ((value >> noBit) & 1)

int IntToGray(unsigned char input)
{
    return (input >>1) ^ input;
}

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
    //TRISA = 0x0000;
    delay_ms(50);
    portValue = dziesietna;
    LATA = portValue;
    printf("%d\n", LATA);
    Tablica[0] = xor(x);
}

unsigned portValue = 0, bcd = 0, a = 1;
char prevS6 = 0, currentS6 = 0, prevS7 = 0, currentS7 = 0, program = 0;

void podprogram1(){
    portValue++;
    LATA = portValue;
}

void podprogram2(){
    portValue--;
    LATA = portValue;
}

void podprogram3(){
    portValue = IntToGray(portValue);
    portValue++;
    LATA = portValue;
    
}

void podprogram4(){
    portValue = IntToGray(portValue);
    portValue--;
    LATA = portValue;
    
}

void podprogram5(){
    portValue = (((bcd/10)<<4)|(bcd%10));
    if(bcd > 100)
    {
        bcd = 0;
    }
    else
    {
        bcd++;
    }
    LATA = portValue;
}

void podprogram6(){
    portValue = (((bcd/10)<<4)|(bcd%10));
    if(bcd < 0)
    {
        bcd = 99;
    }
    else
    {
        bcd--;
    }
    LATA = portValue;
}

void snake(){
    static int left = 1;
    if(left == 1) {
        portValue = portValue * 2;
    if (portValue == 224) {
        left = 2;
    }
    } else if(left == 2) {
        portValue = portValue / 2;
        if (portValue == 7) {
        left = 1;
        }
    }
}

void queue(){
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
            portValue = 0;
        }
        else {
            portValue += a;
            a = a*2;
        }
    }

void __attribute__((interrupt, no_auto_psv)) _T1Interrupt(void){
    if(program == 0){
        podprogram1();
    }
    if(program == 1){
        podprogram2();
    }
    if(program == 2){
        podprogram3();
    }
    if(program == 3){
        podprogram4();
    }
    if(program == 4){
        podprogram5();
    }
    if(program == 5){
        podprogram6();
    }
    if(program == 6){
        podprogram7();
    }
    if(program == 7){
        podprogram8();
    }
    if(program == 8){
        int lfsr_value = 105;
        while (1) {
        generator(lfsr_value);
        __delay32(10000000);
        lfsr_value = lfsr_value >> 1 | ((xor(lfsr_value) & 1) << 5);
    }
    }
    _T1IF = 0;
}

int main(void) {
    TRISA = 0x0000;
    TRISD = 0xFFFF;
    T1CON = 0x8010; // 0b10000000100000
    PR1 = 0xFFFF;
    _T1IE = 1;
    _T1IP = 1;
    while(1){
        prevS6 = PORTDbits.RD6;
        prevS7 = PORTDbits.RD7;
        __delay32(1500);
        currentS6 = PORTDbits.RD6;
        currentS7 = PORTDbits.RD7;
        if (currentS6 - prevS6 == -1){
            bcd = 0;
            portValue = 0;
            a = 1;
            program--;
            
            if(program == 5){
                bcdCounter = 9;
                bcdValue = 9;
                portValue = 9;
            }
            if(program < 0) program = 8;
        }
        if (currentS7 - prevS7 == -1){
            portValue = 0;
            bcValue = 0;
            a = 1;
            program++;
            
            if(program == 5){
                bcd = 99;
                portValue = 9;
            }
            if(program == 1 || program == 3){
                portValue = 255;
            }
            
            if(program > 8) program = 0;
        }
    }
    
    return 0;
}
