/* =============================================================================
     ____    ___    ____    ___    _   _    ___    __  __   ___  __  __ TM
    |  _ \  |_ _|  / ___|  / _ \  | \ | |  / _ \  |  \/  | |_ _| \ \/ /
    | |_) |  | |  | |     | | | | |  \| | | | | | | |\/| |  | |   \  /
    |  __/   | |  | |___  | |_| | | |\  | | |_| | | |  | |  | |   /  \
    |_|     |___|  \____|  \___/  |_| \_|  \___/  |_|  |_| |___| /_/\_\

    Copyright (c) 2006 Pieter Conradie <https://piconomix.com>
 
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

    Title:          px_dbg.h : Debug module
    Author(s):      Pieter Conradie
    Creation Date:  2007-03-31

============================================================================= */

/* _____STANDARD INCLUDES____________________________________________________ */
#include <stdio.h>
#include <stdarg.h>

/* _____PROJECT INCLUDES_____________________________________________________ */
#include "px_dbg.h"

/* _____LOCAL DEFINITIONS____________________________________________________ */
/// Number of bytes per row for hex dump
#define PX_DBG_TRACE_DATA_BYTES_PER_ROW    16

/* _____MACROS_______________________________________________________________ */

/* _____GLOBAL VARIABLES_____________________________________________________ */

/* _____LOCAL VARIABLES______________________________________________________ */
/// Allocate internal buffer for debug output
static char px_dbg_buf[PX_DBG_CFG_BUF_SIZE];

/* _____LOCAL FUNCTION DECLARATIONS__________________________________________ */

/* _____LOCAL FUNCTIONS______________________________________________________ */
static void px_dbg_put_char(char data)
{
#ifdef PX_DBG_CFG_PUT_CHAR
    // Output character using configured function
    PX_DBG_CFG_PUT_CHAR(data);
#else
    // Output character to stdout
    putchar(data);
#endif
}

static void px_dbg_put_str(const char  * data)
{
    while(*data != '\0')
    {
        px_dbg_put_char(*data++);
    }
}

#ifdef PX_COMPILER_GCC_AVR
static void px_dbg_put_str_P(const char * data)
{
    char c;

    while(true)
    {
        c = px_pgm_rd_char(data++);
        if(c == '\0')
        {
            return;
        }
        px_dbg_put_char(c);
    }
}

static void px_dbg_trace_vargs_P(const char * format, va_list args) 
{
    // Populate buffer with debug string
    vsnprintf_P(px_dbg_buf, PX_DBG_CFG_BUF_SIZE, format, args);
    // Append terminating zero in case of buffer overrun
    px_dbg_buf[PX_DBG_CFG_BUF_SIZE-1] = '\0';
    // Output user formatted string
    px_dbg_put_str(px_dbg_buf);
}

static void px_dbg_printf_P(const char * format, ...)
{
    va_list args;

    va_start(args, format);
    px_dbg_trace_vargs_P(format, args);
    va_end(args);
}

#else

static void px_dbg_trace_vargs(const char * format, va_list args) 
{
    // Populate buffer with debug string
    vsnprintf(px_dbg_buf, PX_DBG_CFG_BUF_SIZE, format, args);
    // Append terminating zero in case of buffer overrun
    px_dbg_buf[PX_DBG_CFG_BUF_SIZE-1] = '\0';
    // Output user formatted string
    px_dbg_put_str(px_dbg_buf);
}

static void px_dbg_printf(const char * format, ...)
{
    va_list args;

    va_start(args, format);
    px_dbg_trace_vargs(format, args);
    va_end(args);
}
#endif

static void px_dbg_report_log_prefix(uint8_t      level, 
                                      const char * name, 
                                      uint16_t     line)
{
    // Output level
    if(level == PX_DBG_CFG_MSG_LEVEL_ERR)
    {
        // Error
        px_dbg_put_char('E');
    }
    else if(level == PX_DBG_CFG_MSG_LEVEL_WARN)
    {
        // Warning
        px_dbg_put_char('W');
    }
    else
    {
        // Info
        px_dbg_put_char('I');
    }
    px_dbg_put_char(' ');

    // Timestamp function provided in 'px_dbg_cfg.h'?
#ifdef PX_DBG_CFG_TIMESTAMP
    // Get user supplied timestamp string
    PX_DBG_CFG_TIMESTAMP(px_dbg_buf);
    // Append terminating zero in case of buffer overrun
    px_dbg_buf[PX_DBG_CFG_BUF_SIZE-1] = '\0';
    // Output timestamp
    px_dbg_put_str(px_dbg_buf);
    px_dbg_put_char(' ');
#endif

    // Output file and line
#ifdef PX_COMPILER_GCC_AVR
    px_dbg_put_str_P(name);
    px_dbg_printf_P(PX_PGM_STR(" %u : "), line);
#else
    px_dbg_printf("%s %u : ", name, line);
#endif
}

/* _____GLOBAL FUNCTIONS_____________________________________________________ */
void _px_dbg_log_info(const char * name, uint16_t line, const char * format, ...)
{
    va_list args;

    // Output log prefix (level, timestamp, name and line)
    px_dbg_report_log_prefix(PX_DBG_CFG_MSG_LEVEL_INFO, name, line);

    // Output user formatted string
    va_start(args, format);
#ifdef PX_COMPILER_GCC_AVR
    px_dbg_trace_vargs_P(format, args); 
#else
    px_dbg_trace_vargs(format, args); 
#endif
    va_end(args);
    
    // Append end-of-line
    px_dbg_put_char('\n');
}

void _px_dbg_log_warn(const char * name, uint16_t line, const char * format, ...)
{
    va_list args;

    // Output log prefix (level, timestamp, name and line)
    px_dbg_report_log_prefix(PX_DBG_CFG_MSG_LEVEL_WARN, name, line);

    // Output user formatted string
    va_start(args, format);
#ifdef PX_COMPILER_GCC_AVR
    px_dbg_trace_vargs_P(format, args); 
#else
    px_dbg_trace_vargs(format, args); 
#endif
    va_end(args);
    
    // Append end-of-line
    px_dbg_put_char('\n');
}

void _px_dbg_log_err(const char * name, uint16_t line, const char * format, ...)
{
    va_list args;

    // Output log prefix (level, timestamp, name and line)
    px_dbg_report_log_prefix(PX_DBG_CFG_MSG_LEVEL_ERR, name, line);

    // Output user formatted string
    va_start(args, format);
#ifdef PX_COMPILER_GCC_AVR
    px_dbg_trace_vargs_P(format, args); 
#else
    px_dbg_trace_vargs(format, args); 
#endif
    va_end(args);
    
    // Append end-of-line
    px_dbg_put_char('\n');
}

void _px_dbg_trace(const char * format, ...)
{
    va_list args;

    // Output user formatted string
    va_start(args, format);
#ifdef PX_COMPILER_GCC_AVR
    px_dbg_trace_vargs_P(format, args); 
#else
    px_dbg_trace_vargs(format, args); 
#endif
    va_end(args);
}

void _px_dbg_trace_data(const void * data, size_t nr_of_bytes)
{
    size_t          i;
    const uint8_t * data_u8 = (const uint8_t *)data;

    for(i=0; i<nr_of_bytes; i++)
    {
#ifdef PX_COMPILER_GCC_AVR
        px_dbg_printf_P(PX_PGM_STR("%02hX "), *data_u8++);
#else
        px_dbg_printf("%02hX ", *data_u8++);
#endif
    }
}

void _px_dbg_trace_hexdump(const void * data, size_t nr_of_bytes)
{
    size_t          i, j;
    const uint8_t * row_data;

    // Split data up into rows
    for(i=0; i<nr_of_bytes; i+= PX_DBG_TRACE_DATA_BYTES_PER_ROW)
    {
        // Insert extra empty row?
        if(  (i != 0) && ((i%(PX_DBG_TRACE_DATA_BYTES_PER_ROW*4)) == 0)  )
        {
            // Yes
            px_dbg_put_char('\n');
        }

        // Select row data
        row_data = &((const uint8_t *)data)[i];

        // Display Hexidecimal data
        for(j=0; j<PX_DBG_TRACE_DATA_BYTES_PER_ROW; j++)
        {
            // Insert extra space?
            if(  (j != 0) && ((j%4) == 0)  )
            {
                // Yes
                px_dbg_put_char(' ');
            }
            // End of data?
            if((i+j) < nr_of_bytes)
            {
                // No
#ifdef PX_COMPILER_GCC_AVR
                px_dbg_printf_P(PX_PGM_STR("%02hX "), row_data[j]);
#else
                px_dbg_printf("%02hX ", row_data[j]);
#endif
            }
            else
            {
                // No more data
#ifdef PX_COMPILER_GCC_AVR
                px_dbg_printf_P(PX_PGM_STR("   "));
#else
                px_dbg_put_str("   ");
#endif
            }            
        }

        // Display ASCII data
        for(j=0; j<PX_DBG_TRACE_DATA_BYTES_PER_ROW; j++)
        {
            // End of data?
            if((i+j) < nr_of_bytes)
            {
                // No. Displayable?
                if( (row_data[j] >= 32) && (row_data[j] <= 127) )
                {
                    // Yes
                    px_dbg_put_char(row_data[j]);
                }
                else
                {
                    // No
                    px_dbg_put_char('.');
                }
            }
            else
            {
                // No more data
                px_dbg_put_char(' ');
            }
        }
        px_dbg_put_char('\n');
    }
}