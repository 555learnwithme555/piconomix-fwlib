#ifndef __PX_USB_CDC_STDIO_H__
#define __PX_USB_CDC_STDIO_H__
/* =============================================================================
     ____    ___    ____    ___    _   _    ___    __  __   ___  __  __ TM
    |  _ \  |_ _|  / ___|  / _ \  | \ | |  / _ \  |  \/  | |_ _| \ \/ /
    | |_) |  | |  | |     | | | | |  \| | | | | | | |\/| |  | |   \  /
    |  __/   | |  | |___  | |_| | | |\  | | |_| | | |  | |  | |   /  \
    |_|     |___|  \____|  \___/  |_| \_|  \___/  |_|  |_| |___| /_/\_\

    Copyright (c) 2018 Pieter Conradie <https://piconomix.com>
 
    Permission is hereby granted, free of charge, to any person obtaining a copy
    of this software and associated documentation files (the "Software"), to
    deal in the Software without restriction, including without limitation the
    rights to use, copy, modify, merge, publish, distribute, sublicense, and/or
    sell copies of the Software, and to permit persons to whom the Software is
    furnished to do so, subject to the following conditions:

    The above copyright notice and this permission notice shall be included in
    all copies or substantial portions of the Software.

    THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
    IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
    FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
    AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
    LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
    FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS
    IN THE SOFTWARE.

    Title:          px_usb_cdc_stdio.h : Module to redirect stdio output (e.g. printf) to a USB CDC port
    Author(s):      Pieter Conradie
    Creation Date:  2018-05-21

============================================================================= */
/** 
 *  @ingroup STM32
 *  @defgroup STM32_USB_CDC_STDIO px_usb_cdc_stdio.h : USB stdio redirect module
 *  
 *  Initialises the stdout stream to output to a USB CDC port.
 *  
 *  File(s):
 *  - arch/avr/px_usb_cdc_stdio.h
 *  - arch/avr/px_usb_cdc_stdio.c
 *  
 *  This component initialises the @b stdout stream to output over USB.
 *  
 *  px_usb_cdc_stdio_put_char() will intercept all line feed characters (@b \\n) 
 *  and replace it with a carriage return, line feed sequence (@b \\r\\n). 
 *   
 *  References: 
 *  1. newlib-2.5.0/newlib/libc/sys/arm/syscalls.c
 *
 */
/// @{

/* _____STANDARD INCLUDES____________________________________________________ */
#include <stdio.h>

/* _____PROJECT INCLUDES_____________________________________________________ */
#include "px_defines.h"
#include "px_uart.h"

#ifdef __cplusplus
extern "C" {
#endif
/* _____DEFINITIONS _________________________________________________________ */

/* _____TYPE DEFINITIONS_____________________________________________________ */

/* _____GLOBAL VARIABLES_____________________________________________________ */

/* _____GLOBAL FUNCTION DECLARATIONS_________________________________________ */
/**
 *  Initialise @b stdout stream to use a USB driver.
 *  
 *  px_usb_cdc_stdio_put_char() is provided as the handler for all outgoing 
 *  data.
 */
void px_usb_cdc_stdio_init(void);

/**
 *  Function to send a byte.
 *  
 *  Every carriage return ("\n") will be intercepted and replaced with a
 *  carriage return, new line sequence ("\r\n").
 *  
 *  @param data     byte to send
 *  
 *  @return int     always returns 0
 */
int px_usb_cdc_stdio_put_char(char data);

/**
 *  Function to receive a byte.
 *  
 *  This function will block until a character has been received.
 *  
 *  
 *  @return int     received byte
 */
int px_usb_cdc_stdio_get_char(void);

/**
 *  Fetch a received byte from USB CDC circular buffer
 *  
 *  @param data Pointer to location to store received byte
 *  
 *  @retval TRUE    There is a byte received
 *  @retval FALSE   Circular buffer is empty (no bytes received)
 */
bool px_usb_cdc_stdio_rd_u8(uint8_t * data);

/**
 *  Put received byte from USB CDC driver in circular buffer.
 *  
 *  @param data         Byte received over USB CDC.
 */
void _px_usb_cdc_stdio_on_rx_byte(uint8_t data);

/**
 *  Put byte into USB CDC driver circular buffer for transmission
 *  
 *  @param data         Byte to transmit over USB CDC.
 *  
 *  @retval true        Byte has been accepted into transmit circular buffer
 *  @retval false       Transmit circular buffer is full
 */
bool px_usb_cdc_stdio_wr_u8(uint8_t data);

/**
 *  Put received data from USB CDC driver in circular buffer.
 *  
 *  @param data         Pointer to buffer containing received bytes
 *  @param nr_of_bytes  Number of bytes in buffer
 */
void _px_usb_cdc_stdio_on_rx_data(const uint8_t * data, uint16_t nr_of_bytes);

/**
 *  Get byte from circular buffer to send over USB CDC driver.
 *  
 *  @param data     Byte to send
 *  
 *  @retval TRUE    There is a byte to send
 *  @retval FALSE   Circular buffer is empty (no bytes to send)
 */
bool _px_usb_cdc_stdio_get_tx_byte(uint8_t * data);

/**
 *  Get data from circular buffer to send over USB CDC driver
 *  
 *  @param data         Pointer to buffer
 *  @param nr_of_bytes  Number of bytes to fetch from circular buffer
 *  
 *  @return uint16_t    The actual number of bytes retrieved, which may be less
 *                      than the number specified, because the buffer is empty.
 */
uint16_t _px_usb_cdc_stdio_get_tx_data(uint8_t * data, uint16_t nr_of_bytes);

/* _____MACROS_______________________________________________________________ */

/// @}
#ifdef __cplusplus
}
#endif

#endif // #ifndef __PX_USB_CDC_STDIO_H__
