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
static void eadog_flush(struct glib_dev *dev);
static void eadog_clear(struct glib_dev *dev);
static void eadog_getinfo(struct glib_dev *dev, struct glib_dev_info *info);
static enum glib_flags eadog_enable(struct glib_dev *dev, enum glib_flags flags);
static void eadog_disable(struct glib_dev *dev, enum glib_flags flags);
static void eadog_setpix(struct glib_dev *dev, int x, int y);
static void eadog_clrpix(struct glib_dev *dev, int x, int y);
static void eadog_draw_bitmap(struct glib_dev *dev, int x, int y,
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

static void eadog_flush(struct glib_dev *dev)
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

static void eadog_clear(struct glib_dev *dev)
{
    struct eadog *eadog = (struct eadog*)dev;

    /* Mark all pages dirty */

    memset(&eadog->dirty, 0xff, sizeof(eadog->dirty));

    memset(eadog->fb, eadog->flags & GLIB_INVERT ? 0xff : 0x00, sizeof(eadog->fb));
}

static void eadog_getinfo(struct glib_dev *dev, struct glib_dev_info *info)
{
    (void)dev;

    info->xres = EADOG_XRES;
    info->yres = EADOG_YRES;
}

static enum glib_flags eadog_enable(struct glib_dev *dev, enum glib_flags flags)
{
    struct eadog *eadog = (struct eadog*)dev;
    eadog->flags |= flags;
    return eadog->flags;
}

static void eadog_disable(struct glib_dev *dev, enum glib_flags flags)
{
    struct eadog *eadog = (struct eadog*)dev;
    eadog->flags &= ~flags;
}

static void eadog_setpix(struct glib_dev *dev, int x, int y)
{
    struct eadog *eadog = (struct eadog*)dev;

    if (x >= EADOG_XRES || x < 0 || y >= EADOG_YRES || y < 0)
        return;

    eadog->dirty |= (1 << (y / 8));

    if (eadog->flags & GLIB_INVERT) {
        eadog->fb[y / 8][x] &= ~(1 << (y % 8));
    } else {
        eadog->fb[y / 8][x] |= (1 << (y % 8));
    }
}

static void eadog_clrpix(struct glib_dev *dev, int x, int y)
{
    struct eadog *eadog = (struct eadog*)dev;

    if (x >= EADOG_XRES || x < 0 || y >= EADOG_YRES || y < 0)
        return;

    eadog->dirty |= (1 << (y / 8));

    if (eadog->flags & GLIB_INVERT) {
        eadog->fb[y / 8][x] |= (1 << (y % 8));
    } else {
        eadog->fb[y / 8][x] &= ~(1 << (y % 8));
    }
}

static void eadog_draw_bitmap(struct glib_dev *dev, int x, int y,
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

void eadog_init(struct eadog *eadog, void *priv)
{
    struct glib_dev *dev = (struct glib_dev*)eadog;

    eadog_reset(eadog, true);
    eadog_reset(eadog, false);

    eadog->priv = priv;

    /* OPs */
    dev->flush       = eadog_flush;
    dev->clear       = eadog_clear;
    dev->getinfo     = eadog_getinfo;
    dev->draw_bitmap = eadog_draw_bitmap;
    dev->setpix      = eadog_setpix;
    dev->clrpix      = eadog_clrpix;
    dev->enable      = eadog_enable;
    dev->disable     = eadog_disable;

    eadog_clear(dev);

    /* Initialize display with init sequence */

    eadog->data(eadog->priv, false);
    eadog->write(eadog->priv, g_init_seq, sizeof(g_init_seq));
}
