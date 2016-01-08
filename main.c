/*
 * thermo.c
 *
 * Created: 2015.11.30. 13:46:26
 * Author : Peti
 */ 

#include <avr/io.h>
#include <util/delay.h>

#include "uclib/libuart/uart.h"
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

int main(void)
{
	// power saver - uart
	PRR &= ~(1 << PRUSART0);
	init_uart();

	//power saver - spi
	PRR &= ~(1 << PRSPI);
	SPI_MasterInit();

	struct bme280_t sens;
	bme280_init_interface(&sens);
	bme280_init(&sens);
	bme280_set_power_mode(BME280_SLEEP_MODE);
	bme280_set_standby_durn(BME280_STANDBY_TIME_250_MS);
	bme280_set_oversamp_humidity(BME280_OVERSAMP_1X);
	bme280_set_oversamp_temperature(BME280_OVERSAMP_8X);
	bme280_set_oversamp_pressure(BME280_OVERSAMP_1X);
	bme280_set_power_mode(BME280_NORMAL_MODE);

	while(1)
	{
		s32 temp;
		bme280_read_uncomp_temperature(&temp);
		s32 tempc = bme280_compensate_temperature_int32(temp);
		transmit_decimal(tempc);

		bme280_read_uncomp_humidity(&temp);
		tempc = bme280_compensate_humidity_int32(temp);
		transmit_decimal(tempc>>10);

		_delay_ms(1000);
	}
}

