#ifndef _GLIB_H_
#define _GLIB_H_

#include <stdlib.h>
#include <stdint.h>

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
    int charcount;
    const struct glib_glyph glyphs[];
};

struct glib_lcd {
    void (*flush)(struct glib_lcd *dev);
    void (*draw_bitmap)(struct glib_lcd *dev, int x, int y,
                        const struct glib_bitmap *bitmap);
    void (*setpix)(struct glib_lcd *dev, int x, int y);
    void (*clrpix)(struct glib_lcd *dev, int x, int y);
    uint16_t xres;
    uint16_t yres;
};

struct glib_dev {
    struct glib_lcd *lcd;
    const struct glib_font *font;
    int x;
    int y;
};


void glib_init(struct glib_dev *dev, struct glib_lcd *lcd);
void glib_flush(struct glib_dev *dev);
void glib_font_set(struct glib_dev *dev, const struct glib_font *font);
void glib_print(struct glib_dev *dev, int x, int y, const char *utf8);

#endif  /* _GLIB_H_ */
