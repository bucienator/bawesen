#include <avr/interrupt.h>

#include "spi.h"

void SPI_MasterInit()
{
	/* Set MOSI and SCK output, all others input */
	DDRA |= (1<<DDA6)|(1<<DDA4); // MOSI, SCK are all output
	/* Enable SPI, Master, set clock rate fck/16 */
	SPCR = (1<<SPE)|(1<<MSTR)|(1<<SPR0);
}

char SPI_MasterTranscieve(char cData)
{
	/* Start transmission */
	SPDR = cData;
	/* Wait for transmission complete */
	while(!(SPSR & (1<<SPIF)));
	char ret = SPDR;
	return ret;
}