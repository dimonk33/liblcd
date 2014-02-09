/*
 * This file is part of the libopencm3 project.
 *
 * Copyright (C) 2010 Thomas Otto <tommi@viadmin.org>
 *
 * This library is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Lesser General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public License
 * along with this library.  If not, see <http://www.gnu.org/licenses/>.
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <libopencm3/stm32/rcc.h>
#include <libopencm3/stm32/flash.h>
#include <libopencm3/stm32/gpio.h>
#include <libopencm3/stm32/usart.h>
#include <libopencm3/stm32/timer.h>
#include <libopencm3/cm3/nvic.h>
#include <libopencm3/stm32/spi.h>

#include <eadog.h>
#include <glib.h>

#include "./fonts/fonts.h"

#define EADOG_SPI         SPI2
#define EADOG_RESET_PIN   GPIO9
#define EADOG_RESET_PORT  GPIOB
#define EADOG_A0_PIN      GPIO10
#define EADOG_A0_PORT     GPIOB

static void eadog_reset(void *priv, bool enable);
static void eadog_data(void *priv, bool enable);
static void eadog_write(void *priv, const uint8_t *data, size_t len);

static void eadog_reset(void *priv, bool enable)
{
    (void)priv;

    if (enable)
        gpio_clear(EADOG_RESET_PORT, EADOG_RESET_PIN);
    else
	gpio_set(EADOG_RESET_PORT, EADOG_RESET_PIN);
}

static void eadog_data(void *priv, bool enable)
{
    (void)priv;

    if (enable)
	gpio_set(EADOG_A0_PORT, EADOG_A0_PIN);
    else
        gpio_clear(EADOG_A0_PORT, EADOG_A0_PIN);

}

static void eadog_write(void *priv, const uint8_t *data, size_t len)
{
    (void)priv;

    spi_set_nss_low(EADOG_SPI);

    while (len--) {
        spi_send(EADOG_SPI, *data++);
        DELAY(700);
    }

    spi_set_nss_high(EADOG_SPI);

}

static void gpio_setup(void)
{
    /* Enable GPIOB clock. */
    rcc_peripheral_enable_clock(&RCC_APB2ENR, RCC_APB2ENR_IOPBEN);

    /* A0 of EADOG */
    gpio_set_mode(EADOG_A0_PORT, GPIO_MODE_OUTPUT_50_MHZ,
                  GPIO_CNF_OUTPUT_PUSHPULL, EADOG_A0_PIN);
    /* Reset of EADOG */
    gpio_set_mode(EADOG_RESET_PORT, GPIO_MODE_OUTPUT_50_MHZ,
                  GPIO_CNF_OUTPUT_PUSHPULL, EADOG_RESET_PIN);

    /* EADOG/SPI2 clock and MOSI and NSS(CS1) */
    gpio_set_mode(GPIOB, GPIO_MODE_OUTPUT_50_MHZ,
                  GPIO_CNF_OUTPUT_ALTFN_PUSHPULL, GPIO12);
    gpio_set_mode(GPIOB, GPIO_MODE_OUTPUT_50_MHZ,
                  GPIO_CNF_OUTPUT_ALTFN_PUSHPULL, GPIO13);
    gpio_set_mode(GPIOB, GPIO_MODE_OUTPUT_50_MHZ,
                  GPIO_CNF_OUTPUT_ALTFN_PUSHPULL, GPIO15);
}

static void spi_setup(void)
{
    /* The EADOG display is connected to SPI2, so initialise it. */

    rcc_peripheral_enable_clock(&RCC_APB1ENR, RCC_APB1ENR_SPI2EN);

    spi_set_unidirectional_mode(EADOG_SPI);       /* We want to send only. */
    spi_disable_crc(EADOG_SPI);                   /* No CRC for this slave. */
    spi_set_dff_8bit(EADOG_SPI);                  /* 8-bit dataword-length */
    spi_set_full_duplex_mode(EADOG_SPI);          /* Not receive-only */

    /* We want to handle the CS signal in software. */

    spi_enable_software_slave_management(EADOG_SPI);
    spi_set_nss_high(EADOG_SPI);

    /* PCLOCK/256 as clock. */

    spi_set_baudrate_prescaler(EADOG_SPI, SPI_CR1_BR_FPCLK_DIV_256);

    /* We want to control everything and generate the clock -> master. */

    spi_set_master_mode(EADOG_SPI);
    spi_set_clock_polarity_1(EADOG_SPI); /* SCK idle state high. */

    /* Bit is taken on the second (rising edge) of SCK. */

    spi_set_clock_phase_1(EADOG_SPI);
    spi_enable_ss_output(EADOG_SPI);
    spi_enable(EADOG_SPI);
}

int main(void)
{
    struct eadog eadog = {
        .reset = eadog_reset,
        .data  = eadog_data,
        .write = eadog_write
    };

    struct glib_dev gdev;

    rcc_clock_setup_in_hse_8mhz_out_24mhz();

    gpio_setup();
    spi_setup();

    eadog_init(&eadog, NULL);
    glib_init(&gdev, (struct glib_lcd*)&eadog);

    glib_font_set(&gdev, &font_courier_new12);
    glib_print(&gdev, 0, 12, "Hello World!");
    glib_print(&gdev, 0, 24, "Привет мир!");
    glib_flush(&gdev);

    for (;;) {
        __asm__("nop");
    }

    return 0;
}
