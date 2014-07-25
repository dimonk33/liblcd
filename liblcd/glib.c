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

#include <stdlib.h>

#include "glib.h"

static int glib_utf8_charlen(const char *c)
{
    int ret = 0;
    char first = *c;

    if (first & 0x80) {
        do {
            first <<= 1;
            ret++;
        } while (first & 0x80);
    } else {
        ret = 1;
    }

    return ret;
}

static uint32_t glib_utf8(const char *c)
{
    uint32_t ret = 0;
    char first = *c;

    if (first & 0x80) {
        do {
            ret <<= 8;
            ret |= *c++;
            first <<= 1;
        } while (first & 0x80);
    } else {
        ret = first;
    }

    return ret;
}

static int glib_utf8_compare(const void *a, const void *b)
{
    struct glib_glyph *ga = (struct glib_glyph*)a;
    struct glib_glyph *gb = (struct glib_glyph*)b;
    return ga->utf8 - gb->utf8;
}

static struct glib_glyph *glib_findglyph(struct glib_dev *dev, const char *utf8)
{
    struct glib_glyph glyph = {
        .utf8 = glib_utf8(utf8)
    };

    return bsearch(
        (void*)&glyph,
        dev->font->glyphs,
        dev->font->charcount,
        sizeof(struct glib_glyph),
        glib_utf8_compare);
}

void glib_flush(struct glib_dev *dev)
{
    dev->lcd->flush(dev->lcd);
}

void glib_font_set(struct glib_dev *dev, const struct glib_font *font)
{
    dev->font = font;
}

void glib_print(struct glib_dev *dev, int x, int y, const char *utf8)
{
    struct glib_glyph *glyph;
    dev->x = x;
    dev->y = y;

    while (*utf8 != '\0') {
        glyph = glib_findglyph(dev, utf8);

        if (glyph != NULL) {
            if (glyph->bitmap != NULL) {
                dev->lcd->draw_bitmap(
                    dev->lcd,
                    dev->x + glyph->x,
                    dev->y + glyph->y,
                    glyph->bitmap);

                dev->x += glyph->bitmap->width;
            }

            dev->x += glyph->x;
        }

        utf8 += glib_utf8_charlen(utf8);
    }
}

void glib_clear(struct glib_dev *dev)
{
    dev->lcd->clear(dev->lcd);
}

void glib_init(struct glib_dev *dev, struct glib_lcd *lcd)
{
    dev->lcd = lcd;
    dev->x = 0;
    dev->y = 0;
}
