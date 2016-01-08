#ifndef _UART_H
#define _UART_H

void init_uart();

#ifdef UART_CAN_SEND
	#ifndef TX_BUFFER_SIZE
	#define TX_BUFFER_SIZE 8
	#endif
	void transmit_string(const char * str);
	void transmit_char(const char c);
#endif

#ifdef UART_CAN_RECEIVE
	#ifndef RX_BUFFER_SIZE
	#define RX_BUFFER_SIZE 8
	#endif
	char receive_char(char * data, char block);
#endif

#endif /* _UART_H */
