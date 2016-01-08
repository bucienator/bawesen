
#define __DELAY_BACKWARD_COMPATIBLE__
#include <util/delay.h>
#include <avr/io.h>
#include "BME280_driver/bme280.h"
#include "spi.h"

static void bme280_chip_select_active()
{
	PORTA &= ~(1 << PORTA7); // SS is low, transaction start
}

static void bme280_chip_select_inactive()
{
	PORTA |= (1 << PORTA7); // SS is high
}

static s8 bme280_bus_read(u8 device_addr, u8 register_addr, u8 * register_data, u8 rd_len)
{
	bme280_chip_select_active();

	register_addr |= (1<<7);
	SPI_MasterTranscieve(register_addr);
	while(rd_len--) {
		*register_data++ = SPI_MasterTranscieve(0);
	}

	bme280_chip_select_inactive();
	return 0;
}

static s8 bme280_bus_write(u8 device_addr, u8 register_addr, u8 const * register_data, u8 rd_len)
{
	bme280_chip_select_active();

	register_addr &= ~(1<<7);
	while(rd_len--) {
		SPI_MasterTranscieve(register_addr++);
		SPI_MasterTranscieve(*register_data++);
	}

	bme280_chip_select_inactive();
	return 0;
}

static void bme280_delay(u16 ms)
{
	_delay_ms(ms);
}

void bme280_init_interface(struct bme280_t * driver)
{
	DDRA |= (1<<DDA7); // SS for BME280
	bme280_chip_select_inactive();
	driver->bus_read = bme280_bus_read;
	driver->bus_write = bme280_bus_write;
	driver->delay_msec = bme280_delay;
}