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

static struct glib_glyph *glib_findglyph(struct glib_ctx *ctx, const char *utf8)
{
    struct glib_glyph glyph = {
        .utf8 = glib_utf8(utf8)
    };

    return bsearch(
        (void*)&glyph,
        ctx->font->glyphs,
        ctx->font->charcount,
        sizeof(struct glib_glyph),
        glib_utf8_compare);
}

void glib_flush(struct glib_ctx *ctx)
{
    ctx->dev->flush(ctx->dev);
}

void glib_font_set(struct glib_ctx *ctx, const struct glib_font *font)
{
    ctx->font = font;
}

void glib_print(struct glib_ctx *ctx, int x, int y, const char *utf8)
{
    struct glib_glyph *glyph;
    ctx->x = x;
    ctx->y = y;

    while (*utf8 != '\0') {
        glyph = glib_findglyph(ctx, utf8);

        if (glyph != NULL) {
            if (glyph->bitmap != NULL) {
                ctx->dev->draw_bitmap(
                    ctx->dev,
                    ctx->x + glyph->x,
                    ctx->y + glyph->y,
                    glyph->bitmap);

                ctx->x += glyph->bitmap->width;
            }

            ctx->x += glyph->x;
        }

        utf8 += glib_utf8_charlen(utf8);
    }
}

void glib_clear(struct glib_ctx *ctx)
{
    ctx->dev->clear(ctx->dev);
}

void glib_enable(struct glib_ctx *ctx, enum glib_flags flags)
{
    /* To prevent multiple evaluation of the same flags in multiple layers,
     * we first enable the flags and then disable the accepted ones from the
     * lower layer.
     */

    ctx->flags |= flags;
    ctx->flags &= ~ctx->dev->enable(ctx->dev, flags);
}

void glib_disable(struct glib_ctx *ctx, enum glib_flags flags)
{
    ctx->flags &= ~flags;
    ctx->dev->disable(ctx->dev, flags);
}

void glib_init(struct glib_ctx *ctx, struct glib_dev *dev)
{
    ctx->dev = dev;
    ctx->x = 0;
    ctx->y = 0;
}
