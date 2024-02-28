#include <stdio.h>
#include <stdlib.h>

#define BIT_VALUE(value, noBit) ((value >> noBit) & 1)


void delay()
{
    int a,b;
    for (a=0; a<=147483647; a++);
    {
        for (b=0; b<=512; b++);
        {

        }
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
    printf("Tablica: ");
    for (int i = 0; i < 6; i++)
    {
        printf("%d ", Tablica[i]);
    }
    printf("\n");
    Tablica[0] = xor(x);
}

void lfsr(int seed)
{
    int lfsr_value = seed;
    while (1) {
        generator(lfsr_value);
        delay();
        lfsr_value = lfsr_value >> 1 | ((xor(lfsr_value) & 1) << 5);
    }
}



int main()
{
    lfsr(1);
    return 0;
}
