#include <avr/interrupt.h>

#include "uart.h"

#ifdef UART_CAN_SEND
	static volatile char txBuffer[TX_BUFFER_SIZE];
	static volatile unsigned char txBufferStart; // index of first transmitable char
	static volatile unsigned char txBufferEnd;  // index of next fillable position
	// if bufferStart == bufferEnd, then the bufer is either full, or empty.
	// when it is empty, bufferStart MSB is 1.
#endif

#ifdef UART_CAN_RECEIVE
	static volatile char rxBuffer[RX_BUFFER_SIZE];
	static volatile unsigned char rxBufferStart;  
	static volatile unsigned char rxBufferEnd;
#endif

void init_uart()
{

	#define BAUD 1200
	#include<util/setbaud.h>

	UBRR0H = UBRRH_VALUE;
	UBRR0L = UBRRL_VALUE;
#if USE_2X
	#warning use2x
	UCSR0A |=  (1 << U2X0);
#else
	UCSR0A &= ~(1 << U2X0);
#endif

#ifdef UART_CAN_SEND
	UCSR0B |= (1<<TXEN0); 
	txBufferStart=0x80; // empty buffer: MSB = 1;
	txBufferEnd=0;
#endif

#ifdef UART_CAN_RECEIVE
	UCSR0B |= (1<<RXEN0) + (1<<RXCIE0);
	rxBufferStart=0x80; // empty buffer: MSB = 1;
	rxBufferEnd=0;
#endif

	UCSR0B &= ~(1<<UCSZ02);
	UCSR0C = (3<<UCSZ00);

}

#ifdef UART_CAN_SEND

ISR(USART0_UDRE_vect) {

	if(txBufferStart & 0x80) {
		// if tx buffer empty, disable interrupt
		UCSR0B &= ~(1<<UDRIE0);
	} else {
		UDR0 = txBuffer[txBufferStart];
		txBufferStart = (txBufferStart + 1) % TX_BUFFER_SIZE;
		if(txBufferStart == txBufferEnd) {
			txBufferStart |= 0x80;
		}
	}

}

void transmit_char(const char c)
{
	//while ( !(UCSR0A & (1<<UDRE0)) );

	//UDR0 = c;

	// wait if buffer full:
	if ( txBufferStart == txBufferEnd ) {
		//PORTD |= (1<<DDD7);
		//return;
		while ( txBufferStart == txBufferEnd );
	}

	//PORTD &= ~(1<<DDD7);
	// load data to buffer
	cli();
	txBufferStart &= ~0x80; // clear MSB if buffer was empty;
	txBuffer[txBufferEnd] = c;
	txBufferEnd = (txBufferEnd + 1) % TX_BUFFER_SIZE;
	sei();
	
	// enable transmit interrupt
	UCSR0B |= (1<<UDRIE0); 

}


void transmit_string(const char * str)
{
	while( *str != '\0') {
		transmit_char(*str);
		str++;
	}
}

#endif /* UART_CAN_SEND */





#ifdef UART_CAN_RECEIVE


ISR(USART0_RX_vect) {

	if(rxBufferStart == rxBufferEnd) {
		unsigned char tmp = UDR0;
		// drop content... :(
		//PORTD |= (1<<DDD6);
	}

	if(rxBufferStart & 0x80) {
		// if rx buffer empty, unset flag
		rxBufferStart &= 0x7f;
	}

	rxBuffer[rxBufferStart] = UDR0;
	rxBufferStart = (rxBufferStart + 1) % RX_BUFFER_SIZE;

}

char receive_char(char * data, char block)
{
	rxBufferEnd++;
	rxBufferEnd--;
	// wait if buffer empty:
	if ( rxBufferStart & 0x80 ) {
		if(!block)
			return 0;

		while ( rxBufferStart & 0x80 );
	}

	// load data to buffer
	cli();
	*data = rxBuffer[rxBufferEnd];
	rxBufferEnd = (rxBufferEnd + 1) % RX_BUFFER_SIZE;
	if(rxBufferStart == rxBufferEnd) {
		rxBufferStart |= 0x80;
	}
	sei();
	
	return 1;	
}

#endif /* UART_CAN_RECEIVE */
