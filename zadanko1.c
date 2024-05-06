/*
 * File:   main.c
 * Author: local
 *
 * Created on March 20, 2024, 3:32 PM
 */

#include <time.h>

#define BIT_VALUE(value, noBit) ((value >> noBit) & 1)

LATA = 0;
unsigned portValue = 0;
int bcd = 0;
int a = 1;

int IntToGray(unsigned char input)
{
    return (input >>1) ^ input;
}

#define BIT_VALUE(value, noBit) ((value >> noBit) & 1)

void delay_ms(unsigned int milliseconds) {
    clock_t start_time = clock(); // Pobierz czas rozpoczêcia
    unsigned int wait_time = milliseconds * (CLOCKS_PER_SEC / 1000);

    while (clock() < start_time + wait_time) {
        // Czekaj a¿ up³ynie odpowiedni czas
    }
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


int main(void) {
    int isDefined = 0;
    int podprogram = 1;
    char currentS6 = 0, prevS6 = 0, currentS7 = 0, prevS7 = 0;
    while(1) {
        LATA = portValue;
        delay_ms(50);
        printf("%d\n", LATA);
        if (podprogram == 1) {
            portValue++;
            if(isDefined == 0)
            {
                portValue = 1;
                isDefined = 1;
            }
        }
        else if (podprogram == 2) {
            if(isDefined == 0)
            {
                portValue = 255;
                isDefined = 1;
            }
            else{
                portValue--;
            }
        }
        else if (podprogram == 3) {
            portValue = IntToGray(portValue);
            portValue++;
            if(isDefined == 0)
            {
                portValue = 0;
                isDefined = 1;
            }
        }
        else if (podprogram == 4) {
            if(isDefined == 0)
            {
                portValue = 255;
                isDefined = 1;
            }
            else {
                LATA = IntToGray(portValue);
                portValue--;
            }
        }
        else if (podprogram == 5) {
            if(isDefined == 0)
            {
                bcd = 0;
                isDefined = 1;
            }
            else
            {
                portValue = (((bcd/10)<<4)|(bcd%10));
                if(bcd > 100)
                {
                    bcd = 0;
                }
                else
                {
                    bcd++;
                }
            }
        }
        else if (podprogram == 6) {
            if(isDefined == 0)
            {
                bcd = 99;
                isDefined = 1;
            }
            else
            {
                portValue = (((bcd/10)<<4)|(bcd%10));
                if(bcd < 0)
                {
                    bcd = 99;
                }
                else
                {
                    bcd--;
                }
            }
        }
        else if (podprogram == 7) {
        static int left = 1;

        if(isDefined == 0) {
            portValue = 7;
            isDefined = 1;
        } else {
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
    }
    else if (podprogram == 8) {
    if (isDefined == 0) {
        portValue = 1;
        isDefined = 1;
        int a = 1; // Zainicjowanie zmiennej 'a' w przypadku, gdy 'isDefined' wynosi 0
    } else {
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
            isDefined = 0;
        }
        else {
            portValue += a;
            a = a*2;
        }
    }
    }
    else if (podprogram == 9) {
    int lfsr_value = 105;
    while (1) {
        generator(lfsr_value);
        //__delay32(10000000);
        lfsr_value = lfsr_value >> 1 | ((xor(lfsr_value) & 1) << 5);
    }
}
    }

    return 0;
}
