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

#include <string.h>

#include "eadog.h"
#include "glib.h"

#define ARRAY_SIZE(a) (sizeof(a) / sizeof(a[0]))

#if defined(CONFIG_EADOG_M132)
static const uint8_t g_init_seq[] = {
    0x40, 0xa1, 0xc0, 0xa6, 0xa2, 0x2f, 0xf8, 0x00, 0x23, 0x81, 0x1f, 0xac,
    0x00, 0xaf
};
#endif

static void eadog_cmd(struct eadog *eadog, uint8_t cmd);
static void eadog_reset(struct eadog *eadog, bool reset);

/* OPs */
static void eadog_flush(struct glib_lcd *dev);
static void eadog_clear(struct glib_lcd *dev);
static void eadog_setpix(struct glib_lcd *dev, int x, int y);
static void eadog_clrpix(struct glib_lcd *dev, int x, int y);
static void eadog_draw_bitmap(struct glib_lcd *dev, int x, int y,
                              const struct glib_bitmap *bitmap);

static void eadog_reset(struct eadog *eadog, bool reset)
{
    eadog->reset(eadog->priv, reset);
    DELAY(500);
}

static void eadog_cmd(struct eadog *eadog, uint8_t cmd)
{
    /* Set display into command mode */
    eadog->data(eadog->priv, false);
    eadog->write(eadog->priv, &cmd, sizeof(cmd));
}

static void eadog_flush(struct glib_lcd *dev)
{
    struct eadog *eadog = (struct eadog*)dev;
    int page;

    for (page = 0; page < EADOG_NPAGES; page++) {
        /* Flush only dirty pages */
        if (eadog->dirty & (1 << page) || true) {

            /* Mark page as clean */
            eadog->dirty &= ~(1 << page);

            eadog_cmd(eadog, 0xb0 + page); /* Set page. */
            eadog_cmd(eadog, 0x10);        /* Set column upper address to 0. */
            eadog_cmd(eadog, 0x00);        /* Set column lower address to 0. */


            /* Set display into data mode */
            eadog->data(eadog->priv, true);

            eadog->write(eadog->priv, eadog->fb[page], EADOG_XRES);
        }
    }
}

static void eadog_clear(struct glib_lcd *dev)
{
    struct eadog *eadog = (struct eadog*)dev;

    /* Mark all pages dirty */

    memset(&eadog->dirty, 0xff, sizeof(eadog->dirty));

    memset(eadog->fb, 0, sizeof(eadog->fb));
}

static void eadog_setpix(struct glib_lcd *dev, int x, int y)
{
    struct eadog *eadog = (struct eadog*)dev;

    if (x >= EADOG_XRES || x < 0 || y >= EADOG_YRES || y < 0)
        return;

    eadog->dirty |= (1 << (y / 8));
    eadog->fb[y / 8][x] |= (1 << (y % 8));
}

static void eadog_clrpix(struct glib_lcd *dev, int x, int y)
{
    struct eadog *eadog = (struct eadog*)dev;

    if (x >= EADOG_XRES || x < 0 || y >= EADOG_YRES || y < 0)
        return;

    eadog->dirty |= (1 << (y / 8));
    eadog->fb[y / 8][x] &= ~(1 << (y % 8));
}

static void eadog_draw_bitmap(struct glib_lcd *dev, int x, int y,
                              const struct glib_bitmap *bitmap)
{
    const uint8_t *p = bitmap->data;
    int bmx;
    int bmy;
    int pixel = 8;

    for (bmy = 0; bmy < bitmap->height; bmy++) {
        for (bmx = 0; bmx < bitmap->width; bmx++) {

            if (*p & (1 << --pixel)) {
                eadog_setpix(dev, x + bmx, y + bmy);
            } else {
                eadog_clrpix(dev, x + bmx, y + bmy);
            }
            if (pixel == 0) {
                pixel = 8;
                p++;
            }
        }
    }
}

void eadog_init(struct eadog *dev, void *priv)
{
    eadog_reset(dev, true);
    eadog_reset(dev, false);

    dev->priv = priv;

    /* OPs */
    dev->lcd.flush       = eadog_flush;
    dev->lcd.clear       = eadog_clear;
    dev->lcd.draw_bitmap = eadog_draw_bitmap;
    dev->lcd.setpix      = eadog_setpix;
    dev->lcd.clrpix      = eadog_clrpix;

    eadog_clear((struct glib_lcd*)dev);

    /* Initialize display with init sequence */

    dev->data(dev->priv, false);
    dev->write(dev->priv, g_init_seq, sizeof(g_init_seq));
}
