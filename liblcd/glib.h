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

#ifndef _LIBLCD_GLIB_H_
#define _LIBLCD_GLIB_H_

#include <stdbool.h>
#include <stdint.h>
#include <unistd.h>

struct glib_bitmap {
    int width;
    int height;
    const uint8_t data[];
};

struct glib_glyph {
    uint32_t utf8;
    int x;
    int y;
    const struct glib_bitmap *bitmap;
};

struct glib_font {
    uint16_t charcount;
    uint16_t size;
    const struct glib_glyph glyphs[];
};

enum glib_flags {
    GLIB_INVERT = 0x01
};

struct glib_dev_info {
    uint16_t xres;
    uint16_t yres;
};

struct glib_dev {
    void (*flush)(struct glib_dev *dev);
    void (*draw_bitmap)(struct glib_dev *dev, int x, int y,
                        const struct glib_bitmap *bitmap);
    void (*setpix)(struct glib_dev *dev, int x, int y);
    void (*clrpix)(struct glib_dev *dev, int x, int y);
    void (*clear)(struct glib_dev *dev);
    void (*getinfo)(struct glib_dev *dev, struct glib_dev_info *info);
    /* Enable should return the current flags to determite which were accepted
     * by the driver.
     */
    enum glib_flags (*enable)(struct glib_dev *dev, enum glib_flags flags);
    void (*disable)(struct glib_dev *dev, enum glib_flags flags);

    uint16_t xres;
    uint16_t yres;
};

struct glib_ctx {
    struct glib_dev *dev;
    const struct glib_font *font;
    int x;
    int y;
    uint32_t flags;
};


void glib_init(struct glib_ctx *ctx, struct glib_dev *dev);
void glib_clear(struct glib_ctx *ctx);
void glib_flush(struct glib_ctx *ctx);
void glib_font_set(struct glib_ctx *ctx, const struct glib_font *font);
const struct glib_font *glib_font_get(struct glib_ctx *ctx);
void glib_print(struct glib_ctx *ctx, int x, int y, const char *utf8);
void glib_enable(struct glib_ctx *ctx, enum glib_flags flags);
void glib_disable(struct glib_ctx *ctx, enum glib_flags flags);

void glib_devinfo_get(struct glib_ctx *ctx, struct glib_dev_info *info);

void glib_rect(struct glib_ctx *ctx, int x, int y, int width, int height,
               bool fill);

#endif  /* _LIBLCD_GLIB_H_ */
