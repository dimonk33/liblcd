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

void glib_init(struct glib_dev *dev, struct glib_lcd *lcd)
{
    dev->lcd = lcd;
    dev->x = 0;
    dev->y = 0;
}
