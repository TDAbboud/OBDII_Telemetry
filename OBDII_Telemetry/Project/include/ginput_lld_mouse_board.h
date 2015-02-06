/*
 * This file is subject to the terms of the GFX License. If a copy of
 * the license was not distributed with this file, you can obtain one at:
 *
 *              http://ugfx.org/license.html
 */

/**
 * @file    drivers/ginput/touch/ADS7843/ginput_lld_mouse_board_olimex_stm32_e407.h
 * @brief   GINPUT Touch low level driver source for the ADS7843 on an Olimex STM32E407.
 *
 * @defgroup Mouse Mouse
 * @ingroup GINPUT
 * @{
 */

#ifndef _GINPUT_LLD_MOUSE_BOARD_H
#define _GINPUT_LLD_MOUSE_BOARD_H

#include "ch.h"
#include "hal.h"

/////////////////////////////////////
// *** SPI2 PINS for STM32F4 ***   //
//---------------------------------//
// SPI PORT |   PIN NAME  |  TOUCH //
//---------------------------------//
//   MISO   |     PB14    |  DOUT  //
//   MOSI   |     PB15    |  DIN   //
//   SCK    |     PB13    |  CLK   //
//   N/A    |     PB11    |  CS    //
//   N/A    |     PB12    |  IRQ   //
/////////////////////////////////////

#define TP_CS_PORT GPIOB
#define TP_IRQ_PORT GPIOB

#define TP_IRQ_PIN 12
#define TP_CS_PIN 11

// Peripherial Clock 42MHz SPI2 SPI3
// Peripherial Clock 84MHz SPI1 SPI1 SPI2/3
#define SPI_BaudRatePrescaler_2 ((uint16_t)0x0000) // 42 MHz 21 MHZ
#define SPI_BaudRatePrescaler_4 ((uint16_t)0x0008) // 21 MHz 10.5 MHz
#define SPI_BaudRatePrescaler_8 ((uint16_t)0x0010) // 10.5 MHz 5.25 MHz
#define SPI_BaudRatePrescaler_16 ((uint16_t)0x0018) // 5.25 MHz 2.626 MHz
#define SPI_BaudRatePrescaler_32 ((uint16_t)0x0020) // 2.626 MHz 1.3125 MHz
#define SPI_BaudRatePrescaler_64 ((uint16_t)0x0028) // 1.3125 MHz 656.25 KHz
#define SPI_BaudRatePrescaler_128 ((uint16_t)0x0030) // 656.25 KHz 328.125 KHz
#define SPI_BaudRatePrescaler_256 ((uint16_t)0x0038) // 328.125 KHz 164.06 KHz

static const SPIConfig spicfg =
    {
        NULL, TP_CS_PORT, TP_CS_PIN, SPI_BaudRatePrescaler_256
    };

/**
 * @brief   Initialise the board for the touch.
 *
 * @notapi
 */
static inline void init_board(void)
{
    // Configure pins for SPI2 operation
    palSetPadMode(TP_CS_PORT, TP_CS_PIN, PAL_MODE_OUTPUT_PUSHPULL | PAL_STM32_OSPEED_HIGHEST); /* CHIP ENABLE - CS. */
    palSetPad(TP_CS_PORT, TP_CS_PIN);

    palSetPadMode(GPIOB, 13, PAL_MODE_ALTERNATE(5) | PAL_STM32_OSPEED_HIGHEST); /* SCK/CLK. */
    palSetPadMode(GPIOB, 14, PAL_MODE_ALTERNATE(5) | PAL_STM32_OSPEED_HIGHEST); /* MISO/DOUT. */
    palSetPadMode(GPIOB, 15, PAL_MODE_ALTERNATE(5) | PAL_STM32_OSPEED_HIGHEST); /* MOSI/DIN. */

    spiStart(&SPID2, &spicfg);
}

/**
 * @brief   Check whether the surface is currently touched
 * @return	TRUE if the surface is currently touched
 *
 * @notapi
 */
static inline bool_t getpin_pressed(void)
{
    return (!palReadPad(TP_IRQ_PORT, TP_IRQ_PIN)); // IRQ PORT, PIN No
}
/**
 * @brief   Aquire the bus ready for readings
 *
 * @notapi
 */
static inline void aquire_bus(void)
{
    spiAcquireBus(&SPID2);
    //TOUCHSCREEN_SPI_PROLOGUE();
    palClearPad(TP_CS_PORT, TP_CS_PIN);
}

/**
 * @brief   Release the bus after readings
 *
 * @notapi
 */
static inline void release_bus(void)
{
    palSetPad(TP_CS_PORT, TP_CS_PIN);
    spiReleaseBus(&SPID2);
    //TOUCHSCREEN_SPI_EPILOGUE();
}

/**
 * @brief   Read a value from touch controller
 * @return	The value read from the controller
 *
 * params[in] port	The controller port to read.
 *
 * @notapi
 */
static inline uint16_t read_value(uint16_t port)
{
    static uint8_t txbuf[3] = {0};
    static uint8_t rxbuf[3] = {0};
    uint16_t ret;

    txbuf[0] = port;

    spiExchange(&SPID2, 3, txbuf, rxbuf);

    ret = (rxbuf[1] << 5) | (rxbuf[2] >> 3);

    return ret;
}

#endif /* _GINPUT_LLD_MOUSE_BOARD_H */
/** @} */
