/*
 * This file is subject to the terms of the GFX License. If a copy of
 * the license was not distributed with this file, you can obtain one at:
 *
 *              http://ugfx.org/license.html
 */

/**
 * @file    drivers/gdisp/SSD1963/gdisp_lld_board_template.h
 * @brief   GDISP Graphic Driver subsystem board interface for the SSD1963 display.
 *
 * @addtogroup GDISP
 * @{
 */
#include "ssd1963.h"

#ifndef _GDISP_LLD_BOARD_H
#define _GDISP_LLD_BOARD_H

#define STM32F4XX
//#define GDISP_USE_GPIO
#define GDISP_USE_FSMC
#define GDISP_USE_DMA
#define GDISP_DMA_STREAM STM32_DMA2_STREAM6

#if defined(GDISP_USE_GPIO)
    // Define port for lcd reset pin
	//#define GDISP_CMD_ALTERNATE_RESET_PORT GPIOA

	#define GDISP_CMD_PORT GPIOC
	#define GDISP_DATA_PORT GPIOD

	#define GDISP_CS 0
	#define GDISP_RS 1
	#define GDISP_WR 2
	#define GDISP_RD 3
	#define GDISP_RESET 4 // Select the LCD reset pin No. Port is defined in GDISP_CMD_PORT or GDISP_CMD_ALTERNATE_RESET_PORT (if defined)

	#define Set_CS		palSetPad(GDISP_CMD_PORT, GDISP_CS);
	#define Clr_CS		palClearPad(GDISP_CMD_PORT, GDISP_CS);
	#define Set_RS		palSetPad(GDISP_CMD_PORT, GDISP_RS);
	#define Clr_RS		palClearPad(GDISP_CMD_PORT, GDISP_RS);
	#define Set_WR		palSetPad(GDISP_CMD_PORT, GDISP_WR);
	#define Clr_WR		palClearPad(GDISP_CMD_PORT, GDISP_WR);
	#define Set_RD		palSetPad(GDISP_CMD_PORT, GDISP_RD);
	#define Clr_RD		palClearPad(GDISP_CMD_PORT, GDISP_RD);

	#if defined(GDISP_CMD_ALTERNATE_RESET_PORT)
		#define Set_Up_Reset palSetPadMode(GDISP_CMD_ALTERNATE_RESET_PORT, GDISP_RESET, PAL_MODE_OUTPUT_PUSHPULL)
	#else
		#define Set_Up_Reset palSetPadMode(GDISP_CMD_PORT, GDISP_RESET, PAL_MODE_OUTPUT_PUSHPULL)
	#endif

	#if defined(GDISP_CMD_ALTERNATE_RESET_PORT)
		#define Set_Reset palSetPad(GDISP_CMD_ALTERNATE_RESET_PORT, GDISP_RESET)
		#define Clr_Reset palClearPad(GDISP_CMD_ALTERNATE_RESET_PORT, GDISP_RESET)
	#else
		#define Set_Reset palSetPad(GDISP_CMD_PORT, GDISP_RESET)
		#define Clr_Reset palClearPad(GDISP_CMD_PORT, GDISP_RESET)
	#endif
#endif

#if defined(GDISP_USE_FSMC)
  #define GDISP_REG ((volatile uint16_t *) 0x60000000)[0] /* RS = 0 */
  #define GDISP_RAM ((volatile uint16_t *) 0x60020000)[0] /* RS = 1 */
#endif
/**
 * @brief   Send data to the index register.
 *
 * @param[in] index		The index register to set
 *
 * @notapi
 */
static inline void write_index(uint16_t index)
{
#if defined(GDISP_USE_FSMC)
    GDISP_REG = index;
#endif
#if defined(GDISP_USE_GPIO)
    Set_CS;
    Set_RS;
    Set_WR;
    Clr_RD;
    palWritePort(GDISP_DATA_PORT, index);
    Clr_CS;
#endif
}

/**
 * @brief   Send data to the lcd.
 *
 * @param[in] data		The data to send
 * 
 * @notapi
 */
static inline void write_data(uint16_t data)
{
#if defined(GDISP_USE_FSMC)
    GDISP_RAM = data;
#endif
#if defined(GDISP_USE_GPIO)

    Set_CS;
    Clr_RS;
    Set_WR;
    Clr_RD;
    palWritePort(GDISP_DATA_PORT, data);
    Clr_CS;
#endif
}

/**
 * @brief   Initialise the board for the display.
 * @notes	Performs the following functions:
 *			1. initialise the io port used by your display
 *			2. initialise the reset pin (initial state not-in-reset)
 *			3. initialise the chip select pin (initial state not-active)
 *			4. initialise the backlight pin (initial state back-light off)
 *
 * @notapi
 */
static inline void pre_init_board(void)
{
#if defined(GDISP_USE_FSMC)
    const unsigned char FSMC_Bank = 0;

#if defined(STM32F1XX) || defined(STM32F3XX)
    /* FSMC setup for F1/F3 */
    rccEnableAHB(RCC_AHBENR_FSMCEN, 0);

#if defined(GDISP_USE_DMA) && defined(GDISP_DMA_STREAM)
  #error "DMA not implemented for F1/F3 Devices"
#endif
#elif defined(STM32F4XX) || defined(STM32F2XX)
    /* STM32F2-F4 FSMC init */
    rccEnableAHB3(RCC_AHB3ENR_FSMCEN, 0);

#if defined(GDISP_USE_DMA) && defined(GDISP_DMA_STREAM)

    if (dmaStreamAllocate(GDISP_DMA_STREAM, 0, NULL, NULL))
        gfxExit();
    dmaStreamSetMemory0(GDISP_DMA_STREAM, &GDISP_RAM);
    dmaStreamSetMode(GDISP_DMA_STREAM,
                     STM32_DMA_CR_PL(0) | STM32_DMA_CR_PSIZE_HWORD
                     | STM32_DMA_CR_MSIZE_HWORD | STM32_DMA_CR_DIR_M2M);
#endif
#else
    #error "FSMC not implemented for this device"
#endif

    /* set pins to FSMC mode */
    IOBus busD = { GPIOD, (1 << 0) | (1 << 1) | (1 << 4) | (1 << 5) | (1 << 7)
                   | (1 << 8) | (1 << 9) | (1 << 10) | (1 << 11) | (1 << 14)
                   | (1 << 15), 0 };

    IOBus busE = { GPIOE, (1 << 7) | (1 << 8) | (1 << 9) | (1 << 10) | (1 << 11)
                   | (1 << 12) | (1 << 13) | (1 << 14) | (1 << 15), 0 };

    palSetBusMode(&busD, PAL_MODE_ALTERNATE(12));
    palSetBusMode(&busE, PAL_MODE_ALTERNATE(12));

    /* FSMC timing */
    FSMC_Bank1->BTCR[FSMC_Bank + 1] = (FSMC_BTR1_ADDSET_1 | FSMC_BTR1_ADDSET_3)
                                      | (FSMC_BTR1_DATAST_1 | FSMC_BTR1_DATAST_3)
                                      | (FSMC_BTR1_BUSTURN_1 | FSMC_BTR1_BUSTURN_3);
    /*FSMC_Bank1->BTCR[FSMC_Bank + 1] = (FSMC_BTR1_ADDSET_2 | FSMC_BTR1_ADDSET_1) \
          | (FSMC_BTR1_DATAST_2 | FSMC_BTR1_DATAST_1) \
          | FSMC_BTR1_BUSTURN_0;*/

    /* Bank1 NOR/SRAM control register configuration
     * This is actually not needed as already set by default after reset */
    FSMC_Bank1->BTCR[FSMC_Bank] = FSMC_BCR1_MWID_0 | FSMC_BCR1_WREN
                                  | FSMC_BCR1_MBKEN;
#endif
#if defined(GDISP_USE_GPIO)

    IOBus busCMD = {GDISP_CMD_PORT, (1 << GDISP_CS) | (1 << GDISP_RS) | (1 << GDISP_WR) | (1 << GDISP_RD), 0};
    IOBus busDATA = {GDISP_DATA_PORT, 0xFFFFF, 0};
    palSetBusMode(&busCMD, PAL_MODE_OUTPUT_PUSHPULL);
    palSetBusMode(&busDATA, PAL_MODE_OUTPUT_PUSHPULL);
#endif
}

static inline void init_board_ugfx(void)
{

}

static inline void init_board(void)
{
    // Code converted based on the source code that came with the lcd

#define  HDP 799    // HDP 0x31f
#define  HT 928     // HT 0x3a0
#define  HPS 46     // HPS 0x02e
#define  LPS 15     // LPS 0x00f
#define  HPW 48     // HPW 0x30

#define  VDP 479    // VDP 0x1df
#define  VT  525    // VT 0x20d
#define  VPS 16     // VPS 0x016
#define  FPS 8      // FPS 0x008
#define  VPW 16  // VPW 0x010

#if defined(GDISP_USE_GPIO)
    // Setup reset pin as output
    Set_Up_Reset;

    Set_Reset;

    chThdSleepMilliseconds(5);

    Clr_Reset;

    chThdSleepMilliseconds(5);

    Set_Reset;

    Set_CS;
    Set_RD;
    Set_WR;

    chThdSleepMilliseconds(5);

    Clr_CS;
#endif

    write_index(0x00E2);	// PLL multiplier, set PLL clock to 120M
    write_data(0x0023);		// N=0x36 for 6.5M, 0x23 for 10M crystal (0x21 advised in the manual)
    write_data(0x0002);     // M=3 (0x02 advised in the manual)
    write_data(0x0054);     // Dummy byte

    write_index(0x00E0);	// PLL enable
    write_data(0x0001);

    chThdSleepMilliseconds(1);

    write_index(0x00E0);
    write_data(0x0003);

    chThdSleepMilliseconds(5);

    write_index(0x0001);	// Software reset

    chThdSleepMilliseconds(5);

    write_index(0x00E6);	// PLL setting for PCLK, depends on resolution
    write_data(0x0003);
    write_data(0x00ff);
    write_data(0x00ff);

    write_index(0x00B0);	// LCD SPECIFICATION
    write_data(0x0027);
    write_data(0x0000);
    write_data((HDP>>8) & 0X00FF);  //Set HDP
    write_data(HDP & 0X00FF);
    write_data((VDP>>8) & 0X00FF);  //Set VDP
    write_data(VDP & 0X00FF);
    write_data(0x0000);

    write_index(0x00B4);  //HSYNC
    write_data((HT>>8) & 0X00FF); //Set HT
    write_data(HT & 0X00FF);
    write_data((HPS>>8) & 0X00FF); //Set HPS
    write_data(HPS & 0X00FF);
    write_data(HPW); //Set HPW
    write_data((LPS>>8) & 0X00FF); //Set HPS
    write_data(LPS & 0X00FF);
    write_data(0x0000);

    write_index(0x00B6); //VSYNC
    write_data((VT>>8) & 0X00FF);   //Set VT
    write_data(VT & 0X00FF);
    write_data((VPS>>8) & 0X00FF);  //Set VPS
    write_data(VPS & 0X00FF);
    write_data(VPW); //Set VPW
    write_data((FPS>>8) & 0X00FF);  //Set FPS
    write_data(FPS & 0X00FF);

    write_index(0x00BA);
    write_data(0x000F);    //GPIO[3:0] out 1

    write_index(0x00B8);
    write_data(0x0007);    //GPIO3=input, GPIO[2:0]=output
    write_data(0x0001);    //GPIO0 normal

    write_index(0x0036); //rotation
    write_data(0x0000);

    write_index(0x00F0); // Set pixel data interface
    write_data(SSD1963_PDI_16BIT565);  // Interface set to 16BIT565

    chThdSleepMilliseconds(5);

    write_index(0x0029); // Show the image on the display device

    write_index(0x00d0);
    write_data(0x000d);
}

static inline void post_init_board(void)
{
#if defined(GDISP_USE_FSMC)
    const unsigned char FSMC_Bank = 0;
    /* FSMC delay reduced as the controller now runs at full speed */
    FSMC_Bank1->BTCR[FSMC_Bank + 1] = FSMC_BTR1_ADDSET_0 | FSMC_BTR1_DATAST_2  | FSMC_BTR1_BUSTURN_0;
    FSMC_Bank1->BTCR[FSMC_Bank] = FSMC_BCR1_MWID_0 | FSMC_BCR1_WREN | FSMC_BCR1_MBKEN;
#endif
#if defined(GDISP_USE_GPIO)
    /* Nothing to do here */
#endif
}

/**
 * @brief   Set or clear the lcd reset pin.
 *
 * @param[in] state		TRUE = lcd in reset, FALSE = normal operation
 * 
 * @notapi
 */
static inline void setpin_reset(bool_t state)
{
    (void) state;
    /* Nothing to do here */

}

/**
 * @brief   Set the lcd back-light level.
 *
 * @param[in] percent		0 to 100%
 * 
 * @notapi
 */
static inline void set_backlight(uint8_t percent)
{
    //duty_cycle is 00..FF
    //Work in progress: the SSD1963 has a built-in PWM, its output can
    //be used by a Dynamic Background Control or by a host (user)
    //Check your LCD's hardware, the PWM connection is default left open and instead
    //connected to a LED connection on the breakout board
    write_index(SSD1963_SET_PWM_CONF); //set PWM for BackLight
    write_data(0x0001);
    write_data(percent & 0x00FF);
    write_data(0x0001); //controlled by host (not DBC), enabled
    write_data(0x00FF);
    write_data(0x0060); //don't let it go too dark, avoid a useless LCD
    write_data(0x000F); //prescaler ???

}

/**
 * @brief   Take exclusive control of the bus
 *
 * @notapi
 */
static inline void acquire_bus(void)
{}

/**
 * @brief   Release exclusive control of the bus
 *
 * @notapi
 */
static inline void release_bus(void)
{}

#if GDISP_HARDWARE_READPIXEL || GDISP_HARDWARE_SCROLL || defined(__DOXYGEN__)
/**
 * @brief   Read data from the lcd.
 *
 * @return	The data from the lcd
 * @note	The chip select may need to be asserted/de-asserted
 * 			around the actual spi read
 * 
 * @notapi
 */
static inline uint16_t read_data(void)
{
    return GDISP_RAM;
}
#endif

#endif /* _GDISP_LLD_BOARD_H */
/** @} */
