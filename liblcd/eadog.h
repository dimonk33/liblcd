/**
 * The MIT License (MIT)
 *
 * Copyright (c) 2014 UVC Ingenieure http://uvc.de/
 * Author: Max Holtzberg <mh@uvc.de>
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 * THE SOFTWARE.
 */

#ifndef _LIBLCD_DOGM_H_
#define _LIBLCD_DOGM_H_

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

#include "glib.h"

#define CONFIG_EADOG_M132


/* Number        | Type           | Controller
 * -----------------------------------------
 * EA DOGS102-6  | 102x64  - 1,7" | UC1701
 * EA DOGM132-5  | 132x32  - 2,1" | ST7565
 * EA DOGM128-6  | 128x64  - 2,3" | ST7565
 * EA DOGM240-6  | 240x64  - 3,6" | UC1611s
 * EA DOGL128-6  | 128x64  - 2,8" | ST7565
 * EA DOGXL160-7 | 160x104 - 3,3" | UC1610
 * EA DOGXL240-7 | 240x128 - 3,9" | UC1611s
 */

#if defined(CONFIG_EADOG_S102)
#  define EADOG_XRES 102
#  define EADOG_YRES 64
#elif defined(CONFIG_EADOG_M132)
#  define EADOG_XRES 132
#  define EADOG_YRES 32
#elif defined(CONFIG_EADOG_M128)
#  define EADOG_XRES 128
#  define EADOG_YRES 64
#elif defined(CONFIG_EADOG_M240)
#  define EADOGM_XRES 240
#  define EADOGM_YRES 64
#elif defined(CONFIG_EADOG_XL160)
#  define EADOGM_XRES 240
#  define EADOGM_YRES 104
#elif defined(CONFIG_EADOG_XL240)
#  define EADOGM_XRES 240
#  define EADOGM_YRES 128
#else
#  error "Display type not defined or unkown"
#endif

#define EADOG_NPAGES (EADOG_YRES / 8)

#define DELAY(l) do {         \
    int i;                    \
    for (i = 0; i < l; i++)   \
        __asm__("nop");       \
    } while(0)


struct eadog {
    struct glib_dev dev;

    void (*reset)(void *priv, bool enable);
    void (*data)(void *priv, bool enable);
    void (*write)(void *priv, const uint8_t *data, size_t len);
    void *priv;

    uint32_t flags;
    uint8_t dirty;        /* Bitmask to mark dirty pages */
    uint8_t fb[EADOG_NPAGES][EADOG_XRES];
};


void eadog_init(struct eadog *dev, void *priv);


#endif  /* _LIBLCD_DOGM_H_ */
