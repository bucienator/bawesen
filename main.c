/*
 * thermo.c
 *
 * Created: 2015.11.30. 13:46:26
 * Author : Peti
 */ 

#include <avr/io.h>


int main(void)
{
	DDRA = DDRA | (1 << DDA6);
	PUEA = PUEA & ~(1 << 6);
    /* Replace with your application code */
	unsigned i = 10000;
    while (1) 
    {
		i = 32658;
		while(--i);
		PORTA = PORTA | (1 << PORTA6);
		i = 32658;
		while(--i);
		PORTA = PORTA & ~(1 << PORTA6);
    }
}

