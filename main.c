/*
 * thermo.c
 *
 * Created: 2015.11.30. 13:46:26
 * Author : Peti
 */ 

#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h>

#include "uart.h"
#include "spi.h"
#include "BME280_driver/bme280.h"
#include "bme280_interface.h"

void transmit_decimal(s32 n)
{
	if(n == 0) {
		transmit_string("0\r\n");
	}
	else
	{
		char buffer[16];
		char *p = buffer;
		while(n > 0) {
			*p = '0' + (n%10);
			++p;
			n/=10;
		}
		while(p != buffer) {
			transmit_char(*--p);
		}
		transmit_string("\r\n");
	}

}

void transmit_hexadecimal(unsigned char n)
{
	if(n == 0) {
		transmit_string("0x00\r\n");
	}
	else
	{
		transmit_string("0x");
		char buffer[4];
		char *p = buffer;
		while(n > 0) {
			*p = '0' + (n%16);
			++p;
			n/=16;
		}
		while(p != buffer) {
			transmit_char(*--p);
		}
		transmit_string("\r\n");
	}

}

void init_bme280(struct bme280_t * sens)
{
	bme280_init_interface(sens);
	bme280_init(sens);
	bme280_set_power_mode(sens, BME280_SLEEP_MODE);
	bme280_set_standby_durn(sens, BME280_STANDBY_TIME_250_MS);
	bme280_set_oversamp_humidity(sens, BME280_OVERSAMP_1X);
	bme280_set_oversamp_temperature(sens, BME280_OVERSAMP_8X);
	bme280_set_oversamp_pressure(sens, BME280_OVERSAMP_1X);
	bme280_set_power_mode(sens, BME280_NORMAL_MODE);
}

void measure(struct bme280_t * sens)
{
		s32 temp;
		bme280_read_uncomp_temperature(sens, &temp);
		s32 tempc = bme280_compensate_temperature_int32(sens, temp);
		transmit_decimal(tempc);

		bme280_read_uncomp_humidity(sens, &temp);
		tempc = bme280_compensate_humidity_int32(sens, temp);
		transmit_decimal(tempc>>10);
}

int main(void)
{
	cli();
	CCP = 0xd8;
	CLKPR = 3; // 1/8 clock prescaler
	sei();

	// power saver - uart
	PRR &= ~(1 << PRUSART0);
	init_uart();

	//power saver - spi
	PRR &= ~(1 << PRSPI);
	SPI_MasterInit();

	struct bme280_t s1, s2;
	s1.dev_addr = 1;
	s2.dev_addr = 2;
	init_bme280(&s1);
	init_bme280(&s2);

	while(1)
	{
		measure(&s1);
		measure(&s2);
		transmit_string("----\r\n");
		_delay_ms(1000);
	}
}

